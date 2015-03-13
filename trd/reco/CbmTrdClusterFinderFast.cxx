#include "CbmTrdClusterFinderFast.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
#include "CbmTrdModule.h"
#include "CbmTrdGeoHandler.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TArray.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TVector3.h"
#include "TStopwatch.h"

#include <iostream>
#include <iomanip>
#include <cmath>
using std::cout;
using std::endl;

CbmTrdClusterFinderFast::CbmTrdClusterFinderFast()
  :FairTask("TrdClusterFinderFast",1),
   fDigis(NULL),
   fClusters(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(NULL),
   ClusterSum(-1),
   fRowMergerCounter(0),
   fMinimumChargeTH(1.0e-06),//1.0e-08),
   fTrianglePads(false),
   fMultiHit(false),
   fRowClusterMerger(true),
   fNeighbourRowTrigger(true),
   fNeighbourReadout(true)
{
}

CbmTrdClusterFinderFast::CbmTrdClusterFinderFast(Bool_t MultiHit, Bool_t NeighbourReadout, Bool_t RowClusterMerger, Double_t MinimumChargeTH)
  :FairTask("TrdClusterFinderFast",1),
   fDigis(NULL),
   fClusters(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fGeoHandler(NULL),
   ClusterSum(-1),
   fRowMergerCounter(0),
   fMinimumChargeTH(MinimumChargeTH),
   fTrianglePads(false),
   fMultiHit(MultiHit),
   fRowClusterMerger(RowClusterMerger),
   fNeighbourRowTrigger(true),
   fNeighbourReadout(NeighbourReadout)
{
}

CbmTrdClusterFinderFast::~CbmTrdClusterFinderFast()
{
    fClusters->Delete();
    delete fClusters;
}

void CbmTrdClusterFinderFast::SetParContainers()
{
  fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdClusterFinderFast::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();

   fDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   if (fDigis == NULL) LOG(FATAL) << "CbmTrdClusterFinderFast::Init No TrdDigi array" << FairLogger::endl;

   fClusters = new TClonesArray("CbmTrdCluster", 100);
   ioman->Register("TrdCluster","Trd Cluster",fClusters,kTRUE);

   fGeoHandler = new CbmTrdGeoHandler();
   fGeoHandler->Init();

   return kSUCCESS;
} 
// --------------------------------------------------------------------
void CbmTrdClusterFinderFast::SetTriangularPads(Bool_t triangles)
{
  fTrianglePads = triangles;
}
// --------------------------------------------------------------------
void CbmTrdClusterFinderFast::SetTriggerThreshold(Double_t minCharge){
  fMinimumChargeTH = minCharge;//  To be used for test beam data processing
}
// --------------------------------------------------------------------
void CbmTrdClusterFinderFast::SetNeighbourTrigger(Bool_t trigger){
  fNeighbourReadout = trigger;//  
}
// --------------------------------------------------------------------
void CbmTrdClusterFinderFast::SetNeighbourRowTrigger(Bool_t trigger){
  fNeighbourRowTrigger = trigger;//
}
// --------------------------------------------------------------------
void CbmTrdClusterFinderFast::SetPrimaryClusterRowMerger(Bool_t rowMerger){
  fRowClusterMerger = rowMerger;
}
// ---- Exec ----------------------------------------------------------
void CbmTrdClusterFinderFast::Exec(Option_t *option)
{
  fClusters->Delete();

  TStopwatch timer;
  timer.Start();
  cout << "================CbmTrdClusterFinderFast===============" << endl;
  LOG(INFO) << "CbmTrdClusterFinderFast::Exec : Triangular Pads: " << (Bool_t)fTrianglePads << FairLogger::endl;
  Int_t counterI = 0;
  Int_t counterJ = 0;
  Float_t mRMax = 0;//7500;
  Bool_t dynamic = false;//true;  
  Bool_t optimization = false;//true;//false;//
  if (optimization) {
    dynamic = false;
    //fRowClusterMerger = true;
    LOG(INFO) << "CbmTrdClusterFinderFast::Exec : minimum charge threshold optimization run: fRowClusterMerger: on" << FairLogger::endl;
    //cout << "  minimum charge threshold optimization run" << endl <<"  fRowClusterMerger: on" << endl;
  }
  else {
    if (fMultiHit){
      LOG(INFO) << "CbmTrdClusterFinderFast::Exec : multi hit detection: on" << FairLogger::endl;
      //cout << "  multi hit detection: on" << endl;
    }
    else {
      LOG(INFO) << "CbmTrdClusterFinderFast::Exec : multi hit detection: off" << FairLogger::endl;
      //cout << "  multi hit detection: off" << endl;
      if (dynamic) {
	LOG(INFO) << "CbmTrdClusterFinderFast::Exec : fRowClusterMerger:   on (as function of module distance (r_min = " << mRMax <<" mm) from beam pipe)"<< FairLogger::endl;
	//cout << "  fRowClusterMerger: on (as function of module distance (r_min = " << mRMax <<" mm) from beam pipe)"<< endl;
      }
      else {
	if (fRowClusterMerger) {
	  LOG(INFO) << "CbmTrdClusterFinderFast::Exec : fRowClusterMerger:   on" << FairLogger::endl;
	  //cout << "  fRowClusterMerger: on" << endl;
	}
	else {
	  LOG(INFO) << "CbmTrdClusterFinderFast::Exec : fRowClusterMerger:   off" << FairLogger::endl;
	  //cout << "  fRowClusterMerger: off" << endl;
	}
      }
    }
  }
  //optimization ist only usefull if rowClusters are merged !!!
 
  Float_t mChargeTH[84] = { 0.003, 
			    /*2e-06,*/ //for gausssian distributed charge
			    0, 
			    1e-09, 2e-09, 3e-09, 4e-09, 5e-09, 6e-09, 7e-09, 8e-09, 9e-09,
			    1e-08, 2e-08, 3e-08, 4e-08, 5e-08, 6e-08, 7e-08, 8e-08, 9e-08,
			    1e-07, 2e-07, 3e-07, 4e-07, 5e-07, 6e-07, 7e-07, 8e-07, 9e-07,
			    1e-06, 2e-06, 3e-06, 4e-06, 5e-06, 6e-06, 7e-06, 8e-06, 9e-06,
			    1e-05, 2e-05, 3e-05, 4e-05, 5e-05, 6e-05, 7e-05, 8e-05, 9e-05,
			    1e-04, 2e-04, 3e-04, 4e-04, 5e-04, 6e-04, 7e-04, 8e-04, 9e-04,
			    1e-03, 2e-03, 3e-03, 4e-03, 5e-03, 6e-03, 7e-03, 8e-03, 9e-03,
			    1e-02, 2e-02, 3e-02, 4e-02, 5e-02, 6e-02, 7e-02, 8e-02, 9e-02,
			    1e-01, 2e-01, 3e-01, 4e-01, 5e-01, 6e-01, 7e-01, 8e-01, 9e-01,
			    1e-00  };
  Int_t nChargeTH = 1;

  TH1F* DigiChargeSpectrum = NULL;
  TProfile* OptimalTH = NULL;

  if (optimization)
    {
      nChargeTH = 84;
      OptimalTH = new TProfile("OptimalTH","Optimization of the fMinimumChargeTH",Int_t(1/mChargeTH[2]),0,mChargeTH[nChargeTH-1]);
      OptimalTH->SetXTitle("fMinimumChargeTH value");
      OptimalTH->SetYTitle("Number of found clusters");
      OptimalTH->SetStats(kFALSE);
      OptimalTH->GetXaxis()->SetLabelSize(0.02);
      OptimalTH->GetYaxis()->SetLabelSize(0.02);
      OptimalTH->GetXaxis()->SetTitleSize(0.02);
      OptimalTH->GetXaxis()->SetTitleOffset(1.5);
      OptimalTH->GetYaxis()->SetTitleSize(0.02);
      OptimalTH->GetYaxis()->SetTitleOffset(2);
      OptimalTH->SetMarkerStyle(4);
      DigiChargeSpectrum = new TH1F("DigiChargeSpectrum","DigiChargeSpectrum",1e6,0,1);
      DigiChargeSpectrum->SetXTitle("Digi charge [a.u.]");
      DigiChargeSpectrum->SetYTitle("Number of Digis");
      DigiChargeSpectrum->SetStats(kFALSE);
      DigiChargeSpectrum->GetXaxis()->SetLabelSize(0.02);
      DigiChargeSpectrum->GetYaxis()->SetLabelSize(0.02);
      DigiChargeSpectrum->GetXaxis()->SetTitleSize(0.02);
      DigiChargeSpectrum->GetXaxis()->SetTitleOffset(1.5);
      DigiChargeSpectrum->GetYaxis()->SetTitleSize(0.02);
      DigiChargeSpectrum->GetYaxis()->SetTitleOffset(2);
      //DigiChargeSpectrum->SetMarkerStyle(4);
    }
  //  MyDigi *d;

  for (Int_t iChargeTH = 0; iChargeTH < nChargeTH; iChargeTH++){
    /*
      if (fMultiHit)
      fMinimumChargeTH = 1.0e-08;
      else
      fMinimumChargeTH = mChargeTH[iChargeTH];
    */
    //fMinimumChargeTH = 0.0;
    /*
     * Digis are sorted according to the moduleAddress. A combiId is calculted based 
     * on the rowId and the colId to have a neighbouring criterion for digis within 
     * the same pad row. The digis of each module are sorted according to this combiId.
     * All sorted digis of one pad row are 'clustered' into rowCluster. For a new row
     * the rowClusters are compared to the rowClusters of the last row. If an overlap 
     * is found they are marked to be parents(last row) and childrens(activ row) 
     * (mergeRowCluster()). After this, the finale clusters are created. Therefor 
     * walkCluster() walks along the list of marked parents and markes every visited 
     * rowCluster to avoid multiple usage of one rowCluster. drawCluster() can be used to
     * get a visual output.
     */
      
    Int_t nentries = fDigis->GetEntries();
    Int_t digiCounter = 0;
    LOG(INFO) << "CbmTrdClusterFinderFast::Exec : Digis: " << nentries << FairLogger::endl;
    std::map<Int_t, MyDigiList*> modules; //map of <moduleAddress, List of struct 'MyDigi' pointer>
    std::map<Int_t, MyDigiList*> ModuleNeighbourDigiMap; //map of <moduleAddress, List of struct 'MyDigi' pointer>

    for (Int_t iDigi=0; iDigi < nentries; iDigi++ ) {
      CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
	
      /*
      //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      Int_t size = digi->GetMCIndex().size();
      std::vector<int> MCIndex = digi->GetMCIndex();
      cout << size << ": ";
      for (Int_t i = 0; i < size; i++) {
      cout << " ," << MCIndex[i];
      }
      cout << endl;
      //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      */
      /*
	if (digi->GetCharge() > fMinimumChargeTH)
	{
      */
      /*
       *unrotate rotated modules in x- and y-direction (row <-> col)
       */
      Int_t digiAddress  = digi->GetAddress();
      Int_t layerId    = CbmTrdAddress::GetLayerId(digiAddress);
      Int_t moduleId = CbmTrdAddress::GetModuleId(digiAddress);
      Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);

      //Int_t Station  = fGeoHandler->GetStation(moduleId);
      //Int_t Layer    = fGeoHandler->GetLayer(moduleId);
      //printf("digiAddress:%i  modId:%i  layer:%i\n",digiAddress,moduleId,layerId);
      MyDigi *d = new MyDigi;
      //d = new MyDigi;
	    
      fModuleInfo = fDigiPar->GetModule(CbmTrdAddress::GetModuleAddress(digiAddress));
      if (fModuleInfo){
	//printf("digiAddress %i found\n",digiAddress);
      } else {
	printf("digi %3i digiAddress %i layer %i and modId %i  Sec%i Row:%i Col%i not found\n",
	       iDigi,digiAddress,layerId,moduleId,CbmTrdAddress::GetSectorId(digi->GetAddress()),
	       CbmTrdAddress::GetRowId(digi->GetAddress()),CbmTrdAddress::GetColumnId(digi->GetAddress())); 
	continue;
      }
      d->digiId = iDigi;
      /*
       *'rotated' modules are backrotated to get consistent notation of Row and Col
       *//* // not longer needed since all modules ar in a local not rotated cs
	    if (layerId%2 == 0) {
	    d->colId = digi->GetRow();
	    d->rowId = digi->GetCol();
	    d->combiId = d->rowId * (fModuleInfo->GetNofRows() + 1) + d->colId;
	    }
	    else {
	    d->rowId = digi->GetRow();
	    d->colId = digi->GetCol();
	    d->combiId = d->rowId * (fModuleInfo->GetNofColumns() + 1) + d->colId;
	    }
	 */
      Int_t secRow = CbmTrdAddress::GetRowId(digi->GetAddress());
      Int_t iSector = CbmTrdAddress::GetSectorId(digi->GetAddress());
      Int_t globalRow = 0;
      //for (Int_t iS = 0; iS < iSector; iS++)
      //globalRow += fModuleInfo->GetNofRowsInSector(iS);

      globalRow = fModuleInfo->GetModuleRow(iSector,secRow);

      d->colId = CbmTrdAddress::GetColumnId(digi->GetAddress());
      d->rowId = globalRow;
      d->combiId = d->rowId * (fModuleInfo->GetNofColumns() + 1) + d->colId;
      //if (moduleAddress == 5)
      //printf("c:%4i  sr:%4i mr:%4i nr:%4i  ->  combi:%5i\n",d->colId,CbmTrdAddress::GetRowId(digi->GetAddress()),d->rowId, fModuleInfo->GetNofRows(), d->combiId);

      d->charge = digi->GetCharge();

      if (optimization && fMinimumChargeTH == 0){
	DigiChargeSpectrum->Fill(digi->GetCharge());
      }
      //cout << "Searching neighbour digis below threshold" << endl;
      if (ModuleNeighbourDigiMap.find(moduleAddress) == ModuleNeighbourDigiMap.end()) {	     
	ModuleNeighbourDigiMap[moduleAddress] = new MyDigiList;
      } 
      ModuleNeighbourDigiMap[moduleAddress]->push_back(d);
      if (digi->GetCharge() >= fMinimumChargeTH)	{
	digiCounter++;
	if (modules.find(moduleAddress) == modules.end()) {	     
	  modules[moduleAddress] = new MyDigiList;
	} 
	modules[moduleAddress]->push_back(d);
      }
      // Since the pointer is stored in a stl container which is used later out of the scope of this loop
      // the pointer is set to NULL here to make clear that the objects must deleted elswhere. If the objects 
      // are not deleted later this is a perfect memory leak. 
      // The problem here is that the pointers are stored in several stl containers, so one has to use the stl
      // container holding all pointers to delete the objects. 
      d = NULL;
      //	delete d;
    }
    LOG(INFO) << "CbmTrdClusterFinderFast::Exec : Digis:  " << digiCounter << " above threshold (" << fMinimumChargeTH << " GeV)"<< FairLogger::endl;
    
    std::map<Int_t, ClusterList*> fModClusterMap; //map of <moduleAddress, pointer of Vector of List of struct 'MyDigi' >
    for (std::map<Int_t, MyDigiList*>::iterator it = modules.begin(); it != modules.end(); ++it) {
      if (dynamic) {
	fModuleInfo = fDigiPar->GetModule(it->first);
	  
	Float_t mSizeX   = (fModuleInfo->GetSizeX()) * 2 * 10;
	Float_t mSizeY   = (fModuleInfo->GetSizeY()) * 2 * 10;
	Float_t mPosXC   = fModuleInfo->GetX() * 10;
	Float_t mPosYC   = fModuleInfo->GetY() * 10;
	Float_t mR = sqrt(pow(mPosXC,2) + pow(mPosYC,2));
	counterJ++;
	if (mR > mRMax) {
	  //fRowClusterMerger = true;
	  counterI++;
	  //cout << it->first << "   "<< mR << endl;
	}
	else {
	  //fRowClusterMerger = false;
	  //cout << "  " << it->first << "   " << mR <<endl;
	}
      }
      //if (it->first==5)
      fModClusterMap[it->first] = clusterModule(it->second/*, ModuleNeighbourDigiMap[it->first]*/);
      //drawCluster(it->first, fModClusterMap[it->first]);
      if (fNeighbourReadout){
	if (fTrianglePads)
	  addNeighbourDigisTriangular(fDigiPar->GetModule(it->first)->GetNofColumns(), fModClusterMap[it->first], ModuleNeighbourDigiMap[it->first]);
	else
	  addNeighbourDigis(fDigiPar->GetModule(it->first)->GetNofColumns(), fModClusterMap[it->first], ModuleNeighbourDigiMap[it->first]);

      }
    }
    //cout << "addCluster(fModClusterMap)" << endl;
    addCluster(fModClusterMap);
    /*
     * clean up
     */
    // Here is now the point to delete all objects which are stored in stl containers. 
    // ModuleNeighbourDigiMap holds all pointers to objects of type MyDigi, so this is
    // the correct container to delete all of them. For the other stl containers one
    // has only to delete the lists and not the MyDigi stored in the lists.
    for (std::map<Int_t, MyDigiList*>::iterator it =  ModuleNeighbourDigiMap.begin(); 
	 it !=  ModuleNeighbourDigiMap.end(); ++it) {
      for (MyDigiList::iterator digisIt = it->second->begin(); 
	   digisIt != it->second->end(); ++digisIt) {
	delete *digisIt;
      }
      delete it->second;
    }
    ModuleNeighbourDigiMap.clear();

    for (std::map<Int_t, ClusterList*>::iterator it = fModClusterMap.begin(); 
	 it != fModClusterMap.end(); ++it) {
      for (ClusterList::iterator clusterIt = it->second->begin(); 
	   clusterIt != it->second->end(); ++clusterIt) {
	/*
	  for (MyDigiList::iterator digisIt = (*clusterIt)->begin(); 
	  digisIt != (*clusterIt)->end(); ++digisIt) {
	  delete *digisIt;
	  }
	*/
	delete *clusterIt;
      }
      delete it->second;
    }
    fModClusterMap.clear();
      
    for (std::map<Int_t, MyDigiList*>::iterator it = modules.begin();
	 it != modules.end(); ++it) {
      /*
	for (MyDigiList::iterator digisIt = it->second->begin(); 
	digisIt != it->second->end(); ++digisIt) {
	delete *digisIt;
	}
      */
      delete it->second;
    }
    modules.clear();
    if (optimization)
      {
	OptimalTH->Fill(mChargeTH[iChargeTH],ClusterSum);
      }
  } //for (iChargeTH)
  //     delete d;
  if (optimization)
    {
      TCanvas* c = new TCanvas ("c","c",1600,800);
      c->Divide(2,1);
      c->cd(1)->SetLogx();
      OptimalTH->DrawCopy();
      c->cd(2)->SetLogx();
      c->cd(2)->SetLogy();
      DigiChargeSpectrum->DrawCopy();
      delete OptimalTH;
      delete DigiChargeSpectrum;
    }

  timer.Stop();
  LOG(INFO) << "CbmTrdClusterFinderFast::Exec : real time=" << timer.RealTime()
            << " CPU time=" << timer.CpuTime() << FairLogger::endl;





  //cout << "  " << counterI << " (" << counterI*100.0/Float_t(counterJ) << "%)" <<  " are reconstructed with fRowClusterMerger" << endl;
  //printf("   %4d modules (%6.3f%%) are reconstructed with fRowClusterMerger\n",counterI,counterI*100/Float_t(counterJ));
  LOG(INFO) << "CbmTrdClusterFinderFast::Exec : RowClusterMerger are used " << fRowMergerCounter << " times" << FairLogger::endl;
  
}

  //----------------------------------------------------------------------
bool digiSorter(MyDigi *a, MyDigi *b)
{ return (a->combiId < b->combiId); } 
//----------------------------------------------------------------------
void CbmTrdClusterFinderFast::addNeighbourDigisTriangular(Int_t nCol, ClusterList *clusters, MyDigiList *neighbours)
{
  Int_t activeCombiId(0), testCombiId(0);
  Int_t activeRow(0), firstRow(0), lastRow(0);
  Int_t activeCol(0), firstCol(0), lastCol(0);

  neighbours->sort(digiSorter);

  for (ClusterList::iterator it = clusters->begin(); it != clusters->end(); ++it) {
    MyDigiList *tobeIncluded = new MyDigiList;
    for (MyDigiList::iterator i = (*it)->begin(); i != (*it)->end(); i++){
      activeCombiId = (*i)->combiId;
      activeRow = (*i)->rowId;
      activeCol = (*i)->colId;
      
      if (activeRow%2 == 0){
	firstRow = activeRow;
	lastRow  = activeRow+1;
	firstCol = activeCol-1;
	lastCol  = activeCol+2;
      } else {
	firstRow = activeRow-1;
	lastRow  = activeRow;
	firstCol = activeCol-2;
	lastCol  = activeCol+1;
      }
   
      /*
       * walk around the active digi and test if neigbours are already included in second list. second list is used to avoid neighbours of neighbours to be included
       */
      for (Int_t iRow = firstRow; iRow <= lastRow; iRow++){
	for (Int_t iCol = firstCol; iCol <= lastCol; iCol++){
	  Bool_t alreadyInList = false;
	  testCombiId = iRow * (nCol+1) + iCol;
	  if (testCombiId != activeCombiId){
	    for (MyDigiList::iterator n = neighbours->begin(); n != neighbours->end(); n++){
	      if (testCombiId == (*n)->combiId){ 
		for (MyDigiList::iterator t = tobeIncluded->begin(); t != tobeIncluded->end(); t++){ // list of neighbours to be included in cluster
		  if ((*t)->combiId == testCombiId) {
		    alreadyInList = true;
		  }
		}
		if (!alreadyInList)	      
		  for (MyDigiList::iterator d = (*it)->begin(); d != (*it)->end(); d++){ // digis which are already inside cluster
		    if ((*d)->combiId == testCombiId){
		      alreadyInList = true;
		    }
		  }
		if (!alreadyInList){
		  tobeIncluded->push_back(*n);
		}
	      }
	    } 
	  } //else cout << " test == active" << endl;
	}
      }
    }
    tobeIncluded->sort(digiSorter);
    for (MyDigiList::iterator t = tobeIncluded->begin(); t != tobeIncluded->end(); t++){
      (*it)->push_back(*t);
    }
    delete tobeIncluded;
  }
}
  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
void CbmTrdClusterFinderFast::addNeighbourDigis(Int_t nCol, ClusterList *clusters, MyDigiList *neighbours)
{
  Int_t activeCombiId(0), testCombiId(0);
  Int_t activeRow(0), firstRow(0), lastRow(0);
  Int_t activeCol(0), firstCol(0), lastCol(0);

  neighbours->sort(digiSorter);

  for (ClusterList::iterator it = clusters->begin(); it != clusters->end(); ++it) {
    //cout << "new cluster" << endl;
    MyDigiList *tobeIncluded = new MyDigiList;
    for (MyDigiList::iterator i = (*it)->begin(); i != (*it)->end(); i++){//Int_t iDigi = 0; iDigi < (*it)->size(); iDigi++){
      //printf("%i %i %i %i\n",activeCombiId,activeRow,activeCol,nRow);
      activeCombiId = (*i)->combiId;
      //printf("%i %i %i %i\n",activeCombiId,activeRow,activeCol,nRow);
      activeRow = (*i)->rowId;
      //printf("%i %i %i %i\n",activeCombiId,activeRow,activeCol,nRow);
      activeCol = (*i)->colId;
      //printf("%i %i %i %i\n",activeCombiId,activeRow,activeCol,nRow);
      //nRow = (activeCombiId - activeCol) / activeRow - 1;
      //printf("%i %i %i %i\n",activeCombiId,activeRow,activeCol,nRow);
      lastCol = activeCol+1;
      if(fNeighbourRowTrigger){
	lastRow = activeRow+1;
	if (activeCol > 0)
	  firstCol = activeCol-1;
	else 
	  firstCol = 0;
	if (activeRow > 0)
	  firstRow = activeRow-1;
	else
	  firstRow = 0;
      } else {
	firstRow = activeRow;
	lastRow = activeRow;
	if (activeCol > 0)
	  firstCol = activeCol-1;
	else 
	  firstCol = 0;
      }
      //cout << " " << activeCombiId << endl;
      /*
       * walk around the active digi and test if neigbours are already included in second list. second list is used to avoid neighbours of neighbours to be included
       */
      for (Int_t iRow = firstRow; iRow <= lastRow; iRow++){
	for (Int_t iCol = firstCol; iCol <= lastCol; iCol++){
	  Bool_t alreadyInList = false;
	  testCombiId = iRow * (nCol+1) + iCol;
	  //cout << testCombiId;
	  if (testCombiId != activeCombiId){
	    for (MyDigiList::iterator n = neighbours->begin(); n != neighbours->end(); n++){
	      if (testCombiId == (*n)->combiId){ //cout << " within neighbour list";
		for (MyDigiList::iterator t = tobeIncluded->begin(); t != tobeIncluded->end(); t++){ // list of neighbours to be included in cluster
		  if ((*t)->combiId == testCombiId) {
		    alreadyInList = true;
		    //cout << ", but already in tobe list" << endl;
		  }
		}
		if (!alreadyInList)	      
		  for (MyDigiList::iterator d = (*it)->begin(); d != (*it)->end(); d++){ // digis which are already inside cluster
		    if ((*d)->combiId == testCombiId){
		      alreadyInList = true;
		      //cout << ", but already in cluster list" << endl;
		    }
		  }
		if (!alreadyInList){
		  tobeIncluded->push_back(*n);
		  //cout << ". Yeah baby, new neighbour" << endl;
		}
	      }
	    } 
	  } //else cout << " test == active" << endl;
	}
      }
      //cout << endl;
    }
    tobeIncluded->sort(digiSorter);
    //cout << " new neighbours" << endl;
    for (MyDigiList::iterator t = tobeIncluded->begin(); t != tobeIncluded->end(); t++){
      //cout << "       " << (*t)->combiId;
      (*it)->push_back(*t);
    }
    delete tobeIncluded;
    //cout << endl;
  }
}
  //----------------------------------------------------------------------
ClusterList *CbmTrdClusterFinderFast::clusterModule(MyDigiList *digis/*, MyDigiList *neighbours*/)
{
  //cout << "----------ClusterModule----------" << endl;
  /*
   *  3 new Lists are initialized: 
   *  currentList for the activ row, 
   *  openList for the previous row and 
   *  closedList for finished rows.
   *  Digis in the TCloneArray digis are sorted according to their combiId (rowId * (nRow+1) + colId).
   */
  std::list<RowCluster*> closedList;
  std::list<RowCluster*> openList;
  std::list<RowCluster*> currentList;
  RowCluster *currentCluster = new RowCluster;

  digis->sort(digiSorter);
  /*
    neighbours->sort(digiSorter);
    MyDigiList::iterator left  = neighbours->begin();
    MyDigiList::iterator right = neighbours->end();
  */
  for (MyDigiList::iterator it = digis->begin(); it != digis->end(); ++it) {

    if (currentCluster->digis->empty() || 
	currentCluster->digis->back()->combiId + 1 == (*it)->combiId) {
      /*
       * Digis within the same row are related to the 'currentCluster' if they are closed-by.
       */

      currentCluster->digis->push_back(*it);
      
    } else {
  
      /*
       * the currentCluster is stored to the currentList
       * (and merged to the clusters of the previous row) 
       * if a break between the activ digi and the currentCluster is found.
       */
      if (fRowClusterMerger) {
	mergeRowCluster(currentCluster, &openList);
      }
      currentList.push_back(currentCluster);
 
      if ((*it)->rowId > currentCluster->row) {
	/*
	 * did we move into a new row?      
	 * The openList is spliced to the end of the closedList if the next row is reached
	 */
	closedList.splice(closedList.end(), openList);

	if ((*it)->rowId > currentCluster->row + 1) {
	  /*
	   *  did we skip at least one line?
	   *  Then the currentList is directly spliced to the end of the closedList since the 
	   *  cluster is definitely finished.
	   */
	  closedList.splice(closedList.end(), currentList);
	} else {
	  /*
	   * if we didn't skip a line, the currentList ist spliced to the end of the openList.
	   */
	  openList.splice(openList.begin(), currentList);
	}
      }
      /*
       * A new currentCluster is initialized if the activ digi is not closed-by the currentCluster.
       */
      currentCluster = new RowCluster;
      currentCluster->digis->push_back(*it);
    }
  }
  /*
   * handle last row
   */
  mergeRowCluster(currentCluster, &openList);
  currentList.push_back(currentCluster);
  closedList.splice(closedList.end(), openList);
  closedList.splice(closedList.end(), currentList);

  return findCluster(&closedList);
}

  //----------------------------------------------------------------------
void CbmTrdClusterFinderFast::mergeRowCluster(RowCluster *currentCluster,
					      std::list<RowCluster*> *openList)
{
  //cout << "----------mergeRowcluster----------" << endl;
  /*
   *  finish up the current rowCluster
   *  Limitations of the currentCluster are compared to the limitations of the RowClusters of the openList.
   *  They are associated to be 'child' (currentList = activ row) and 'parent' (openList = previous row) 
   *  by pointer if a overlap of the limitations are found.
   */
  //currentCluster->digis->sort(digiSorter); // new but should not be necessary
  //printf("new cluster\n");
  //for (std::list<MyDigi*>::iterator i = currentCluster->digis->begin(); i != currentCluster->digis->end(); i++){
  //printf("r:%4i    c:%4i                     combi:%4i\n",(*i)->rowId,(*i)->colId, (*i)->combiId);
  //}
  //printf("-------------------------------------------------\n");
  currentCluster->row = currentCluster->digis->back()->rowId;  
  currentCluster->minCol = currentCluster->digis->front()->colId;
  currentCluster->maxCol = currentCluster->digis->back()->colId;
  currentCluster->centerOfGravity = CenterOfGravity(currentCluster);
  //printf("r:%4i minc:%4i maxc:%4i\n",currentCluster->row, currentCluster->minCol, currentCluster->maxCol);
  //printf("-------------------------------------------------\n");
  /*
   * merge current RowCluster with openList
   */
  Double_t maxDiviationX = 0.25 * fModuleInfo->GetPadSizeX(0); // Pad size is constant within same module therefore it is OK to use here constant sector ID
  for (std::list<RowCluster*>::iterator openIt = openList->begin(); openIt != openList->end(); ++openIt) {
    //printf("r:%4i minc: %4i maxc: %4i ",(*openIt)->row,(*openIt)->minCol,(*openIt)->maxCol);
    if (currentCluster->maxCol < (*openIt)->minCol){// - 1) //avoid diagonal row merging
      //printf("too much left\n");
      break;
    } else if (currentCluster->minCol > (*openIt)->maxCol){// + 1) //avoid diagonal row merging
      //printf("too much right\n");
      continue;
    } else {
      //printf("merged\n");
      if (fabs(CenterOfGravity((*openIt)) - currentCluster->centerOfGravity) < maxDiviationX) {
	fRowMergerCounter++;
	currentCluster->parents.push_back(*openIt);
	(*openIt)->children.push_back(currentCluster);
      }
    }
  }
}					      

  //----------------------------------------------------------------------
  ClusterList *CbmTrdClusterFinderFast::findCluster(std::list<RowCluster*> *rowClusterList)
  {
    //cout << "----------findCluster----------" << endl;
    /*
     *  all found RowClusters are merged to clusters and copyed to the finale clusterList
     *  since the rowClusterList is empty
     */
    RowCluster *currentRowCluster;
    ClusterList *clusterList = new ClusterList;
    while (!rowClusterList->empty()) {
      currentRowCluster = *rowClusterList->begin();
      MyDigiList *cluster = new MyDigiList;
      walkCluster(rowClusterList, currentRowCluster, cluster);
      clusterList->push_back(cluster);
    }
    return clusterList;
  }

  //----------------------------------------------------------------------
  void CbmTrdClusterFinderFast::walkCluster(std::list<RowCluster*> *rowClusterList, 
					    RowCluster *currentRowCluster,
					    MyDigiList *cluster)
  {
    //cout << "----------walkCluster----------" << endl;
    /*
     * the rowClusters are sorted in the neighbour list if connected by a child oder parent pointer.
     * This pointer is removed and the rowCluster is marked as 'hasBeenVisited'. 
     * Therefor it can not be merged to any cluster.
     */
    cluster->splice(cluster->end(), *(currentRowCluster->digis));
    rowClusterList->remove(currentRowCluster);
    std::list<RowCluster*> neighbours;
    for (std::list<RowCluster*>::iterator it = currentRowCluster->parents.begin();
	 it != currentRowCluster->parents.end();) {
      if (!(*it)->hasBeenVisited) {
	neighbours.push_back(*it);
	(*it)->hasBeenVisited = true;
      }
      (*it)->children.remove(currentRowCluster);
      it = currentRowCluster->parents.erase(it);
    }
    for (std::list<RowCluster*>::iterator it = currentRowCluster->children.begin();
	 it != currentRowCluster->children.end();) {
      if (!(*it)->hasBeenVisited) {
	neighbours.push_back(*it);
	(*it)->hasBeenVisited = true;
      }
      (*it)->parents.remove(currentRowCluster);
      it = currentRowCluster->children.erase(it);
    }
    delete currentRowCluster;
    currentRowCluster = NULL;
    for (std::list<RowCluster*>::iterator it = neighbours.begin();
	 it != neighbours.end(); ++it) {
      walkCluster(rowClusterList, *it, cluster);
    }
  }

  //----------------------------------------------------------------------
  void CbmTrdClusterFinderFast::drawCluster(Int_t moduleAddress, ClusterList *clusterList)
  {
    //cout << "----------drawCluster----------" << endl;
    Char_t name[50];
    Char_t title[50];
    sprintf(name,"C%d",moduleAddress);
    sprintf(title,"%s",name);
    Char_t Canfile[100];
    sprintf(Canfile,"Pics/ModuleAddress%s.png",name);
    TH2F* Test = new TH2F(title,name,200,0,200,30,0,30);
    Test->SetContour(99);
 
    Int_t Row;
    Int_t Col;
    Int_t counter = 0;
    for (ClusterList::iterator iCluster = clusterList->begin(); 
	 iCluster != clusterList->end(); iCluster++, counter++)
      {
	for (MyDigiList::iterator iPad = (*iCluster)->begin(); iPad != (*iCluster)->end(); iPad++)
	  {
	    Row = (*iPad)->rowId+1;
	    Col = (*iPad)->colId+1;
	    Test->SetBinContent( Col, Row, counter+1);
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

//--------------------------------------------------------------------
void CbmTrdClusterFinderFast::addCluster(std::map<Int_t, ClusterList*> fModClusterMap)
{
  ClusterSum = 0;
  Float_t deltaCharge;
  Float_t preDelta;
  Int_t modCounter = 0;
  for (std::map<Int_t, ClusterList*>::iterator iMod = fModClusterMap.begin(); iMod != fModClusterMap.end(); ++iMod) {
    modCounter++;
    Int_t cluCounter = 0;
    for (ClusterList::iterator iCluster = (iMod->second)->begin(); iCluster != (iMod->second)->end(); iCluster++) {
      cluCounter++;
      ClusterSum++;
      std::vector<Int_t> digiIndices( (*iCluster)->size() );
      Int_t idigi = 0;
      Int_t i = 1;
      deltaCharge = 0;
      preDelta = 0;
      MyDigiList::iterator lastDigi = (*iCluster)->begin();
      MyDigiList::iterator nextDigi = (*iCluster)->begin();
      for (MyDigiList::iterator iDigi = (*iCluster)->begin(); iDigi != (*iCluster)->end(); iDigi++) {
	if (fMultiHit) {
	  if (idigi < (*iCluster)->size()-1) {
	    nextDigi++;
	  }
	  if (nextDigi != (*iCluster)->end()) {
	    deltaCharge = (*nextDigi)->charge - (*iDigi)->charge;
	    if (preDelta >= 0 && deltaCharge <= 0) { // local maximum in charge distribution
	      i = 1;
	      if ((*lastDigi) != (*iDigi)) i++;
	      if ((*nextDigi) != (*iDigi)) i++;

	      digiIndices.resize(i, 0.);

	      i = 0;
	      if ((*lastDigi) != (*iDigi)){
		digiIndices[i] = (*lastDigi)->digiId;
		i++;
	      }

	      digiIndices[i] = (*iDigi)->digiId;
	      i++;

	      if ((*nextDigi) != (*iDigi)) {
		digiIndices[i] = (*nextDigi)->digiId;
	      }

	      Int_t size = fClusters->GetEntriesFast();
	      const CbmDigi* digi = static_cast<const CbmDigi*>(fDigis->At(digiIndices.front()));
	      CbmTrdCluster* cluster = new ((*fClusters)[size]) CbmTrdCluster();
	      cluster->SetAddress(CbmTrdAddress::GetModuleAddress(digi->GetAddress()));
	      cluster->SetDigis(digiIndices);
	      digiIndices.clear();
	    }
	    preDelta = deltaCharge;
	    lastDigi = iDigi;
	  }
	} else {
	  digiIndices[idigi] = (*iDigi)->digiId;
	}
	idigi++;
      }
      if (!fMultiHit) {
	Int_t size = fClusters->GetEntriesFast();
	const CbmDigi* digi = static_cast<const CbmDigi*>(fDigis->At(digiIndices.front()));
	CbmTrdCluster* cluster = new ((*fClusters)[size]) CbmTrdCluster();
	cluster->SetAddress(CbmTrdAddress::GetModuleAddress(digi->GetAddress()));
	//if (digi->GetAddress() < 0)
	//printf("DigiAddress:%i ModuleAddress:%i\n",digi->GetAddress(), CbmTrdAddress::GetModuleAddress(digi->GetAddress()));
	cluster->SetDigis(digiIndices);
      }
    } // for iCluster
  } // for iModule
  LOG(INFO) << "CbmTrdClusterFinderFast::Exec : Clusters:" << ClusterSum << FairLogger::endl;
}
//%%%%%%%%%%%%%%%%%%%%%
Double_t CbmTrdClusterFinderFast::CenterOfGravity(RowCluster *rowCluster)
{
  //const CbmTrdCluster* cluster = static_cast<const CbmTrdCluster*>(fClusters->At(clusterId));

  TVector3 hit_posV;
  TVector3 local_pad_posV;
  TVector3 local_pad_dposV;
  for (Int_t iDim = 0; iDim < 3; iDim++) {
    hit_posV[iDim] = 0.0;
    local_pad_posV[iDim] = 0.0;
    local_pad_dposV[iDim] = 0.0;
  }
  Double_t totalCharge = 0;
  Int_t moduleAddress = 0;
  Int_t nofDigis = (Int_t)rowCluster->digis->size();//cluster->GetNofDigis();
  //for (Int_t iDigi = 0; iDigi < nofDigis; iDigi++) {
  for (MyDigiList::iterator iDigi = rowCluster->digis->begin(); iDigi != rowCluster->digis->end(); iDigi++) {
    const CbmTrdDigi* digi = static_cast<const CbmTrdDigi*>(fDigis->At((*iDigi)->digiId));//cluster->GetDigi(iDigi)));

    moduleAddress = CbmTrdAddress::GetModuleAddress(digi->GetAddress());
    CbmTrdModule* moduleInfo = fDigiPar->GetModule(moduleAddress);
    //gGeoManager->FindNode(moduleInfo->GetX(), moduleInfo->GetY(), moduleInfo->GetZ());

    if (moduleInfo == NULL) {
      LOG(WARNING) << "CbmTrdHitProducerCluster::CenterOfCharge No CbmTrdModule found digiAddress="
		   << digi->GetAddress() << " moduleAddress=" << CbmTrdAddress::GetModuleAddress(digi->GetAddress()) << FairLogger::endl;
      return -1.;
    }
    totalCharge += digi->GetCharge();
    //printf("DigiAddress:%i ModuleAddress:%i\n",digi->GetAddress(), CbmTrdAddress::GetModuleAddress(digi->GetAddress()));
    moduleInfo->GetPadPosition(digi->GetAddress(), local_pad_posV, local_pad_dposV);//local_pad_pos[0], local_pad_pos[1], local_pad_pos[2]);
    if (fTrianglePads){
      /*
	Double_t local_pad_pos[3];
	for (Int_t i = 0; i < 3; i++) {
	local_pad_pos[i] = local_pad_posV[i];
	}
	Int_t secId(-1), colId(-1), rowId(-1);
	moduleInfo->GetPadInfo(local_pad_pos, secId, colId, rowId);
	if (rowId%2 == 0) { // Calculate center of gravity for triangular pads 
	local_pad_posV[0] = 1./3. * ((local_pad_pos[0] - 0.5 * moduleInfo->GetPadSizeX(secId)) + 
	(local_pad_pos[0] + 0.5 * moduleInfo->GetPadSizeX(secId)) + 
	(local_pad_pos[0] + 0.5 * moduleInfo->GetPadSizeX(secId)));
	local_pad_posV[1] = 1./3. * ((local_pad_pos[1] - 0.5 * moduleInfo->GetPadSizeX(secId)) + 
	(local_pad_pos[1] - 0.5 * moduleInfo->GetPadSizeX(secId)) + 
	(local_pad_pos[1] + 0.5 * moduleInfo->GetPadSizeX(moduleInfo->GetSector(rowId+1, secId))));
	} else {
	local_pad_posV[0] = 1./3. * ((local_pad_pos[0] - 0.5 * moduleInfo->GetPadSizeX(secId)) + 
	(local_pad_pos[0] + 0.5 * moduleInfo->GetPadSizeX(secId)) + 
	(local_pad_pos[0] - 0.5 * moduleInfo->GetPadSizeX(secId)));
	local_pad_posV[1] = 1./3. * ((local_pad_pos[1] + 0.5 * moduleInfo->GetPadSizeX(secId)) + 
	(local_pad_pos[1] + 0.5 * moduleInfo->GetPadSizeX(secId)) + 
	(local_pad_pos[1] - 0.5 * moduleInfo->GetPadSizeX(moduleInfo->GetSector(rowId-1, secId))));
	}
      */
    }
    //moduleInfo->TransformHitError(local_pad_dposV);

    for (Int_t iDim = 0; iDim < 3; iDim++) {
      hit_posV[iDim] += local_pad_posV[iDim] * digi->GetCharge();
    }
  }
  Double_t hit_pos[3];
  for (Int_t iDim = 0; iDim < 3; iDim++) {
    hit_posV[iDim] /= totalCharge;
    hit_pos[iDim] = hit_posV[iDim];
  }
  /*
  Double_t global_hit[3];
  gGeoManager->LocalToMaster(hit_pos, global_hit);

  for (Int_t iDim = 0; iDim < 3; iDim++){
    hit_posV[iDim] = global_hit[iDim];
  }
*/
  return hit_pos[0];
}

ClassImp(CbmTrdClusterFinderFast)
