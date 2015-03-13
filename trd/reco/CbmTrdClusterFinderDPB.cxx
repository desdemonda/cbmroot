#include "CbmTrdClusterFinderDPB.h"
#include "CbmTrdSPADIC.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TArray.h"

#include <iostream>
#include <iomanip>
#include <cmath>
using std::cout;
using std::endl;

CbmTrdClusterFinderDPB::CbmTrdClusterFinderDPB()
  :FairTask("TrdClusterFinderDPB",1),
   fDigis(NULL),
   fClusters(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(NULL)
{
}

CbmTrdClusterFinderDPB::~CbmTrdClusterFinderDPB()
{
    fClusters->Delete();
    delete fClusters;
}

void CbmTrdClusterFinderDPB::SetParContainers()
{
  fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdClusterFinderDPB::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();

   fDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   if (fDigis == NULL) LOG(FATAL) << "CbmTrdClusterFinderDPB::Init No TrdDigi array" << FairLogger::endl;

   fClusters = new TClonesArray("CbmTrdCluster", 100);
   ioman->Register("TrdCluster","Trd Cluster",fClusters,kTRUE);

   fGeoHandler = new CbmTrdGeoHandler();
   fGeoHandler->Init();

   return kSUCCESS;
} 

Bool_t CbmDigiSorter2(std::pair< Int_t, Int_t> a, std::pair< Int_t, Int_t> b)
{return (a.first < b.first);}

void CbmTrdClusterFinderDPB::Exec(Option_t *option)
{
  cout << "================CbmTrdClusterFinderDPB===============" << endl;

  std::map<Int_t, DigiList*> ModuleDigiMap;
  Int_t nDigis = fDigis->GetEntries();
  for (Int_t iDigi=0; iDigi < nDigis; iDigi++ ) {
    CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);  
    Int_t digiAddress  = digi->GetAddress();
    fModuleInfo        = fDigiPar->GetModule(CbmTrdAddress::GetModuleAddress(digiAddress));
    Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
    Int_t secRow = CbmTrdAddress::GetRowId(digi->GetAddress());
    Int_t iSector = CbmTrdAddress::GetSectorId(digi->GetAddress());
    Int_t globalRow = 0;
    globalRow = fModuleInfo->GetModuleRow(iSector,secRow);
    Int_t iCol = CbmTrdAddress::GetColumnId(digi->GetAddress());  
    Int_t combiId = globalRow * (fModuleInfo->GetNofColumns() + 1) + iCol;
    if (ModuleDigiMap.find(moduleAddress)==ModuleDigiMap.end())
      ModuleDigiMap[moduleAddress] = new DigiList;
    ModuleDigiMap[moduleAddress]->push_back(std::make_pair(combiId, iDigi));
  }
  for (std::map<Int_t, DigiList*>::iterator iMod = ModuleDigiMap.begin(); iMod != ModuleDigiMap.end(); iMod++){
    doClustering(iMod->first, iMod->second);   
  }
  for (std::map<Int_t, DigiList*>::iterator iMod = ModuleDigiMap.begin(); iMod != ModuleDigiMap.end(); iMod++){
    delete iMod->second;
  }
  ModuleDigiMap.clear();
}

void CbmTrdClusterFinderDPB::doClustering(Int_t ModuleAddress, DigiList* digiList)
{
  /*
    Any sorting of the digi stream is done here. 
    All infos about the module and the digi are provided by CbmTrdAddress and CbmTrdModule. 
    One cluster per row is created for continous combiId Digis.
  */
  //printf("=========================================================================================\n");
  DigiList *clusterList = new DigiList;

  digiList->sort(CbmDigiSorter2);
  Int_t lastCombiId = (*digiList->begin()).first;
  for (DigiList::iterator iDigi = digiList->begin(); iDigi != digiList->end(); iDigi++) {  
   
    CbmTrdDigi *digi =  (CbmTrdDigi*) fDigis->At((*iDigi).second);  //(*iDigi).second; 
    
    Int_t digiAddress  = digi->GetAddress();
    Int_t layerId      = CbmTrdAddress::GetLayerId(digiAddress);
    Int_t moduleId     = CbmTrdAddress::GetModuleId(digiAddress);
    Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
    fModuleInfo        = fDigiPar->GetModule(moduleAddress);
    Int_t secRow       = CbmTrdAddress::GetRowId(digiAddress);
    Int_t iSector      = CbmTrdAddress::GetSectorId(digiAddress);
    Int_t iCol = CbmTrdAddress::GetColumnId(digiAddress);
    Int_t globalRow    = 0;
    Double_t charge    = digi->GetCharge();
    Double_t time      = digi->GetTime();
    globalRow          = fModuleInfo->GetModuleRow(iSector,secRow);
    Int_t combiId = globalRow * (fModuleInfo->GetNofColumns() + 1) + iCol;
    if (combiId != (*iDigi).first) printf("Wrong CombiId %i %i\n",combiId,(*iDigi).first);
   
    if (digi->GetPrimeTriggerStatus() == true || digi->GetFNR_TriggerStatus() == true){
    
      if ((*iDigi).first - lastCombiId > 1) {  
	//printf("--------------------------------------------------------close\n");
	//printf("{%3i|%3i (%4i)}   <%4i,%4i> %i\n",iCol,globalRow,combiId,(clusterList->front()).first,(clusterList->back()).first,(Int_t)clusterList->size());//cout << combiId << endl;
	//printf("--------------------------------------------------------\n");
	/*
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// search for parallel cluster in following row
	Int_t nextRowCombiId = combiId;
	DigiList::iterator jDigi = iDigi;

	CbmTrdDigi *digiNextRow = NULL;
	Int_t start(clusterList->front().first + (fModuleInfo->GetNofColumns() + 1) ), stop(clusterList->back().first + (fModuleInfo->GetNofColumns() + 1) );
	while ((*jDigi).first <= (combiId + fModuleInfo->GetNofColumns() + 2) && jDigi != digiList->end()){
	  jDigi++;
	  if (digi->GetPrimeTriggerStatus() == true || digi->GetFNR_TriggerStatus() == true){
	    digiNextRow =  (CbmTrdDigi*) fDigis->At((*jDigi).second);
	    if (NULL == digiNextRow) {
	      printf("Digi %i CombiId %i not found!\n",(*jDigi).second,(*jDigi).first);
	      continue;
	    }
	     
	    //Int_t nextRowDigiAddress  = digiNextRow->GetAddress();
	    //Int_t nextRowSecRow       = CbmTrdAddress::GetRowId(nextRowDigiAddress);
	    //Int_t nextRowISector      = CbmTrdAddress::GetSectorId(nextRowDigiAddress);
	    //Int_t nextRowICol = CbmTrdAddress::GetColumnId(nextRowDigiAddress);
	    //Int_t nextRowGlobalRow    = 0;
	    //nextRowGlobalRow          = fModuleInfo->GetModuleRow(nextRowISector,nextRowSecRow);
	    //nextRowCombiId = nextRowGlobalRow * (fModuleInfo->GetNofColumns() + 1) + nextRowICol;
	    

	    nextRowCombiId = (*jDigi).first;	   
	    if (nextRowCombiId >= start && nextRowCombiId <=  stop)
	      {
		//printf("[%3i|%3i (%4i)]   <%4i,%4i> %2i  [%4i,%4i]\n",nextRowICol,nextRowGlobalRow,nextRowCombiId,(clusterList->front()).first,(clusterList->back()).first,(Int_t)clusterList->size(), start, stop);
		clusterList->push_back(std::make_pair((*jDigi).first, (*jDigi).second));
	
		//digiList->erase(jDigi);  
		//if (jDigi == digiList->end()) break;
	      } else {
	      //printf(" %3i|%3i (%4i)\n",nextRowICol,nextRowGlobalRow,nextRowCombiId);
	    }	  
	  }
	}
	*/
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//new cluster is started
	//printf("==============new Cluster\n");
	addCluster(clusterList);  
	clusterList->clear();
	//printf("--------------------------------------------------------new \n");
      }    
      //printf("{%3i|%3i (%4i)}    %4i,%4i  %i\n",iCol,globalRow,combiId,(clusterList->front()).first,(clusterList->back()).first,(Int_t)clusterList->size());//cout << combiId << endl;
      clusterList->push_back(std::make_pair((*iDigi).first, (*iDigi).second));
      lastCombiId = (*iDigi).first;
    }
  }
  addCluster(clusterList);                                                // process last cluster   
}

void CbmTrdClusterFinderDPB::addCluster(DigiList* clusterList)
{   
  //printf("                    %4i,%4i  %2i\n",(clusterList->front()).first,(clusterList->back()).first,(Int_t)clusterList->size());
  Int_t idigi = 0;
  std::vector<Int_t> digiIndices( (*clusterList).size() );
  for (DigiList::iterator iDigi = clusterList->begin(); iDigi != clusterList->end(); iDigi++) {
    digiIndices[idigi] = (*iDigi).second;                                                             // store all digiIds (position in TClonesArray) to a vector
    idigi++;
  }
  Int_t size = fClusters->GetEntriesFast();                                                          // put new cluster to the end of the TClonesArray
  CbmTrdCluster* cluster = new ((*fClusters)[size]) CbmTrdCluster();                                 // Create final CbmTrdDigi
  CbmTrdDigi *digi =  (CbmTrdDigi*) fDigis->At((*clusterList->begin()).second);
  if (digi == NULL) printf("digi:%i not found\n",(*clusterList->begin()).second);
  cluster->SetAddress(CbmTrdAddress::GetModuleAddress(digi->GetAddress()));//Add module address
  cluster->SetDigis(digiIndices);                                                                    // add all digis of this cluster
}
ClassImp(CbmTrdClusterFinderDPB)
