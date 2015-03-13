#include "CbmTrdClusterFinder.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TArray.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TImage.h"

#include <iostream>
#include <iomanip>
#include <cmath>
using std::fabs;
using std::cout;
using std::endl;
using std::setprecision;

// ---- Default constructor -------------------------------------------
CbmTrdClusterFinder::CbmTrdClusterFinder()
  :FairTask("TrdClusterFinder",1),
   fUseSimpleClustering(kTRUE),
   fDigis(NULL),
   fClusters(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(new CbmTrdGeoHandler()),
   mapIt(),
   it(),
   search(),
   neighbour(),
   fDigiMap(),
   fModuleMap(),
   fNeighbours(),
   fModDigiMap(),
   fDigiRow(),
   fDigiCol(),
   fDigiCharge(),
   fClusterBuffer(),
   fModClusterDigiMap()
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdClusterFinder::~CbmTrdClusterFinder()
{

  if(fClusters){
    fClusters->Clear("C");
    fClusters->Delete();
    delete fClusters;
  }
  if(fDigis){
    fDigis->Delete();
    delete fDigis;
  }
  if(fDigiPar){
    delete fDigiPar;
  }
  if(fModuleInfo){
    delete fModuleInfo;
  }

}

// ----  Initialisation  ----------------------------------------------
void CbmTrdClusterFinder::SetParContainers()
{

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
  
}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdClusterFinder::ReInit(){
  
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdClusterFinder::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  
  fDigis =(TClonesArray *)  ioman->GetObject("TrdDigi");
  if ( ! fDigis ) {
    cout << "-W CbmTrdHitProducerDigi::Init: No TrdDigi array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }
  
  fClusters = new TClonesArray("CbmTrdCluster", 100);
  ioman->Register("TrdCluster","TRD",fClusters,kTRUE);

  fGeoHandler->Init();
  
  return kSUCCESS;
  
} 
// --------------------------------------------------------------------

// ---- Exec ----------------------------------------------------------
void CbmTrdClusterFinder::Exec(Option_t * option)
{
  //ChargeTH = 0.1;
  fClusters->Clear();

  if(fUseSimpleClustering == /*kTRUE*/kFALSE){
    SimpleClustering();
  } else {
    RealClustering();
  }

}
// --------------------------------------------------------------------
void CbmTrdClusterFinder::SimpleClustering()
{
  // Loop over all digis and create for each digi a cluster

  Int_t nofDigis = fDigis->GetEntries();
  for (Int_t iDigi=0; iDigi < nofDigis; iDigi++ ) {
    const CbmTrdDigi* digi = static_cast<const CbmTrdDigi*>(fDigis->At(iDigi));

    TClonesArray& clref = *fClusters;
    Int_t size = clref.GetEntriesFast();

    CbmTrdCluster* cluster = new ((*fClusters)[size]) CbmTrdCluster();
    cluster->AddDigi(iDigi);
    cluster->SetCharge(digi->GetCharge());
    cluster->SetMaxCharge(digi->GetCharge());
  }
}
// --------------------------------------------------------------------
void CbmTrdClusterFinder::RealClustering()
{
  Char_t name[50];
  Char_t title[50];
  Int_t moduleId;
  Int_t DigiId;
  SortDigis();

  //Loop over all modules with a fired pad in at least on of their
  //sectors. Loop for each of these modules over all sectors, check 
  //if they contain a fired pad and if any pad is fired loop over all 
  //fired pads of these sector and create clusters of neighbouring pads.
  //In the end check if the clusters end at a boundary between sectors. 
  //If the cluster ends at such a boundary and there is a corresponding
  //cluster in the neighbouring sector merge the two clusters.

  Int_t searchRow = -1;
  Int_t searchCol = -1; 

  for ( mapIt=fModDigiMap.begin(); mapIt!=fModDigiMap.end(); mapIt++) //module map iterator
    {
      it = ((*mapIt).second).begin();                                 // Digi set iterator
     
      moduleId = (*mapIt).first;
      //cout << " ModuleId: " << moduleId << "   Numer of Digis: " << ((*mapIt).second).size();
      sprintf(name,"M%d",(*mapIt).first);
      sprintf(title,"M%d",(*mapIt).first);
      //DrawDigi(moduleId, name, title);

      while ( ((*mapIt).second).size() > 0 )
	{
	  it = ((*mapIt).second).begin();
	  DigiId = *it;
	  //cout << " search Digi:" << *it << "        Col: " << fDigiCol[*it] << "     Row: " << fDigiRow[*it] << endl;
	  std::set<Int_t> ClusterDigiID;  //Final container to store the digiIDs belonging to the same cluster 
	  //cout << fDigiCharge[*it] << endl;
	  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	  /*
	    if ( fDigiCharge[*it] > ChargeTH )
	    {
	  */
	  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	  ClusterDigiID.insert(*it);      //First element of the module map digi set is used as cluster seed
	  fNeighbours.insert(*it);        //First element is copied to the set of neighbour digis
	  ((*mapIt).second).erase(*it);   //First element belongs now to a cluster and is deleted from the module map digi set to avoid double proceeding

	  while (fNeighbours.size() > 0)
	    {
	      neighbour = fNeighbours.begin();  //neighbour iterator is put to the first element of the set
	      searchRow = fDigiRow[*neighbour]; //coordinate value to be compared to every digi of this module
	      searchCol = fDigiCol[*neighbour]; // "
	      SearchNeighbourDigis(searchRow, searchCol, ClusterDigiID);
	      fNeighbours.erase(*neighbour);   //After searching for new neighbour digis, the activ digi is deleted to avoid double proceeding
	      neighbour = fNeighbours.begin(); // neighbour iterator is put to the new first element of the set
	    }
	  fNeighbours.clear();     //If all neighbour digis which was found for the activ cluster are processed the set is cleared to be used for the next cluster
	  fClusterBuffer.push_back(ClusterDigiID); // Each cluster digiSet is stored to one vector element
	  ClusterDigiID.clear();  //if one cluster is finished the digiIDSet is cleared to be used for the next cluster
	  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	  /*
	    }
	    else
	    {
	    ((*mapIt).second).erase(*it);   //First element is deleted since it is below Charge(T)resh(H)old 
	    }
	  */
	  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	}
      //cout << "  Number of found clusters: " <<  fClusterBuffer.size() << endl;
      fModClusterDigiMap[(*mapIt).first] = fClusterBuffer;
      fClusterBuffer.clear();
      //cout << "                   set size: " << ((*mapIt).second).size() << endl;
      //DrawCluster(moduleId,name,title);
    }  
}
  // --------------------------------------------------------------------
Bool_t CbmTrdClusterFinder::SearchNeighbourDigis(Int_t searchRow, Int_t searchCol, std::set<Int_t>& ClusterDigiID)
{
  for ( search = ((*mapIt).second).begin(); search != ((*mapIt).second).end(); search++) // The search iterator is put to the first element (after erasing the activ one) to the next (new first) one and the interated until reaching the last digi within the module map digi set
    {
      if (fabs(fDigiRow[*search] - searchRow) < 2 && fabs(fDigiCol[*search] - searchCol) < 2) // test if digi is a neighbour of the activ one
	{
	  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	  /*	 
	  if (fDigiCharge[*search] > ChargeTH)
	    {	
	  */
	      //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	      ClusterDigiID.insert(*search); // new found neighbour of the activ one is inserted in the finale cluster digi set
	      fNeighbours.insert(*search);   // new found neighbour digi is inserted in the neighbour set to search for neighbour neighbour digis
	      //cout << "        Digi:" << *search << "        Col: " << fDigiCol[*search] << "     Row: " << fDigiRow[*search] << endl;
	      //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	      // }
	  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	  ((*mapIt).second).erase(search); //found neighbour element is erased from module map digi set
	}    
    }
  return true;    
}
  // --------------------------------------------------------------------
void CbmTrdClusterFinder::DrawCluster(Int_t moduleId, Char_t* name, Char_t* title)
{
  Char_t Canfile[100];
  sprintf(Canfile,"Pics/ModuleID%sClusterNo%d.png",name,int(fModClusterDigiMap[moduleId].size()));
  TH2F* Test = new TH2F(title,name,200,0,200,30,0,30);
  Test->SetContour(99);
  std::set<Int_t>::iterator iPad;
  Int_t Row;
  Int_t Col;
  for (Int_t iCluster = 0; iCluster < fModClusterDigiMap[moduleId].size(); iCluster++)
    {
      for (iPad = fModClusterDigiMap[moduleId][iCluster].begin(); iPad != fModClusterDigiMap[moduleId][iCluster].end(); iPad++)
	{
	  Row = fDigiRow[*iPad]+1;
	  Col = fDigiCol[*iPad]+1;
	  Test->SetBinContent( Col, Row, iCluster+1);
	}
    }
  TCanvas* c = new TCanvas(title,name,900,900);
  c->Divide(1,1);
  c->cd(1);
  Test->Draw("colz");
  TImage *Outimage = TImage::Create();
  Outimage->FromPad(c);
  Outimage->WriteImage(Canfile);
  delete Test;
  delete c;
}
  // --------------------------------------------------------------------
  void CbmTrdClusterFinder::DrawDigi(Int_t moduleId, Char_t* name, Char_t* title)
  {
    Char_t Canfile[100];
    sprintf(Canfile,"Pics/ModuleID%sDigi.png",name);
    TH2F* Test = new TH2F(title,name,200,0,200,30,0,30);
    Test->SetContour(99);
    std::set<Int_t>::iterator iPad;
    Int_t Row;
    Int_t Col;

    for (iPad = fModDigiMap[moduleId].begin(); iPad != fModDigiMap[moduleId].end(); iPad++)
      {
	Row = fDigiRow[*iPad]+1;
	Col = fDigiCol[*iPad]+1;
	Test->SetBinContent( Col, Row, fDigiCharge[*iPad]);
      }

    TCanvas* c = new TCanvas(title,name,900,900);
    c->Divide(1,1);
    c->cd(1);
    Test->Draw("colz");
    TImage *Outimage = TImage::Create();
    Outimage->FromPad(c);
    Outimage->WriteImage(Canfile);
    delete Test;
    delete c;
  }
  // --------------------------------------------------------------------
void CbmTrdClusterFinder::SortDigis()
{
  //Put all digis which belong to one sector (unique sector 
  //in a unique module) into a set. Put all of theses sets
  //into a map.

  // Clear sector digi sets

  for (mapIt=fDigiMap.begin(); mapIt!=fDigiMap.end(); mapIt++)
    ((*mapIt).second).clear();

  for (mapIt=fModuleMap.begin(); mapIt!=fModuleMap.end(); mapIt++)
    ((*mapIt).second).clear();

  for (mapIt=fModDigiMap.begin(); mapIt!=fModDigiMap.end(); mapIt++)
    ((*mapIt).second).clear();
 
  Int_t sectorId, moduleId, iRow, iCol;
  CbmTrdDigi *digi=NULL;
  Int_t nentries = fDigis->GetEntries();

  for (int iDigi=0; iDigi < nentries; iDigi++ ) {
    digi =  (CbmTrdDigi*) fDigis->At(iDigi);
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    if (digi->GetCharge() > ChargeTH) // to improve speed
      {
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//Get the sectorId 
	sectorId = digi->GetDetId();
	moduleId = fGeoHandler->GetModuleId(sectorId);

	//cout << moduleId << endl;
	iRow = digi->GetRow();
	iCol = digi->GetCol();
	fDigiCol[iDigi] = iCol;
	fDigiRow[iDigi] = iRow;
	fDigiCharge[iDigi] = digi->GetCharge();
    
	// TODO: Maybe do this in the initilization

	// Add all sectors of a module which have at least one
	// fired pad to the map of modules
	if ( fModuleMap.find(moduleId) == fModuleMap.end() ) {
	  std::set<Int_t> a;
	  a.insert(sectorId);
	  fModuleMap[moduleId] = a;
      
	  std::set<Int_t> DigiSet;
	  DigiSet.insert(iDigi);
	  fModDigiMap[moduleId] = DigiSet;
 
	} else {
	  fModuleMap[moduleId].insert(sectorId);      
	  fModDigiMap[moduleId].insert(iDigi);
	}
  

	// if the sector is not in the map add it, otherwise
	// add the new digi to the map.
	if ( fDigiMap.find(sectorId) == fDigiMap.end() ) {
	  std::set<Int_t> a;
	  a.insert(iDigi);
	  fDigiMap[sectorId] = a;

	} else {
	  fDigiMap[sectorId].insert(iDigi);
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      }
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  }
}
  /*
    TODO: Add protocluster to CbmTrdCluster collection


  */
  // ---- Finish --------------------------------------------------------
  void CbmTrdClusterFinder::Finish()
  {
  }

  ClassImp(CbmTrdClusterFinder)

