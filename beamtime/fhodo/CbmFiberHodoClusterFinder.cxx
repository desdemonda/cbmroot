#include "CbmFiberHodoClusterFinder.h"

#include "CbmFiberHodoDigi.h"
#include "CbmFiberHodoCluster.h"
#include "CbmFiberHodoAddress.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"

#include <iostream>

using std::map;
using std::set;
using namespace std;

// ---- Default constructor -------------------------------------------
CbmFiberHodoClusterFinder::CbmFiberHodoClusterFinder()
  :FairTask("FiberHodoClusterFinder",1),
   fDigis(NULL),
   fClusters(NULL),
   finalClusters(NULL),
   fInputLevelName("HodoCalibDigi"),
   hodo1_pos(NULL),
   hodo2_pos(NULL),
   hodo1_xcor(NULL),
   hodo2_xcor(NULL),
   hodo1_ycor(NULL),
   hodo2_ycor(NULL),
   hodo1x(NULL),
   hodo1y(NULL),
   hodo2x(NULL),
   hodo2y(NULL),
   hodo_xx(NULL),
   hodo_yy(NULL),
   hodo1_pos_nocut(NULL),
   hodo1x_nocut(NULL),
   hodo1y_nocut(NULL)
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmFiberHodoClusterFinder::~CbmFiberHodoClusterFinder()
{

  if(fClusters){
    fClusters->Clear("C");
    fClusters->Delete();
    delete fClusters;
  }
  if(finalClusters){
    finalClusters->Clear("C");
    finalClusters->Delete();
    delete finalClusters;
  }
  if(fDigis){
    fDigis->Delete();
    delete fDigis;
  }

}

// ----  Initialisation  ----------------------------------------------
void CbmFiberHodoClusterFinder::SetParContainers()
{

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmFiberHodoClusterFinder::ReInit(){
  
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmFiberHodoClusterFinder::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  
  fDigis =(TClonesArray *)  ioman->GetObject(fInputLevelName);
  if ( ! fDigis ) {
    LOG(ERROR) << "No FiberHodoDigi array!" << FairLogger::endl;
    LOG(ERROR) << "Task will be inactive" << FairLogger::endl;
    return kERROR;
  }
  
  finalClusters = new TClonesArray("CbmFiberHodoCluster", 100);
  ioman->Register("FiberHodoCluster","TRD",finalClusters,kTRUE);

  fClusters = new TClonesArray("CbmFiberHodoCluster", 100);
  ioman->Register("FiberHodoClusterCandidate","TRD",fClusters,kTRUE);

  hodo1x = new TH1F("hodo1x", "Hodo 1 x", 64, 0., 64.);
  hodo1y = new TH1F("hodo1y", "Hodo 1 y", 64, 0., 64.);
  hodo1x_nocut = new TH1F("hodo1x_nocut", "Hodo 1 x, nocut", 64, 0., 64.);
  hodo1y_nocut = new TH1F("hodo1y_nocut", "Hodo 1 y, nocut", 64, 0., 64.);
  hodo2x = new TH1F("hodo2x", "Hodo 2 x", 64, 0., 64.);
  hodo2y = new TH1F("hodo2y", "Hodo 2 y", 64, 0., 64.);

  hodo1_pos = new TH2F("xy1", "y vs. x Hodo 1", 64, 0., 64., 64, 0., 64.);
  hodo1_pos_nocut = new TH2F("xy1_nocut", "y vs. x Hodo 1, nocut", 64, 0., 64., 64, 0., 64.);
  hodo2_pos = new TH2F("xy2", "y vs. x Hodo 2", 64, 0., 64., 64, 0., 64.);

  hodo1_xcor = new TH2F("x1deltax", "deltax vs. x1", 64, 0., 64., 32, -32., 32.);
  hodo2_xcor = new TH2F("x2deltax", "deltax vs. x2", 64, 0., 64., 32, -32., 32.);

  hodo1_ycor = new TH2F("y1deltay", "deltay vs. y1", 64, 0., 64., 32, -32., 32.);
  hodo2_ycor = new TH2F("y2deltay", "deltay vs. y2", 64, 0., 64., 32, -32., 32.);

  hodo_xx = new TH2F("xx", "x2 vs. x1", 64, 0., 64., 64, 0., 64.);
  hodo_yy = new TH2F("yy", "y2 vs. y1", 64, 0., 64., 64, 0., 64.);

  return kSUCCESS;
  
} 
// --------------------------------------------------------------------

// ---- Exec ----------------------------------------------------------
void CbmFiberHodoClusterFinder::Exec(Option_t * option)
{

  fClusters->Clear();
  finalClusters->Clear();
  
  map<Int_t, set<CbmFiberHodoDigi*, classcomp> >::iterator mapIt;
  for (mapIt=fDigiMap.begin(); mapIt!=fDigiMap.end(); mapIt++) 
    {
      ((*mapIt).second).clear();
    }
  fDigiMap.clear();
  
  set<Int_t> layerSet;
  
  CbmFiberHodoDigi* digi = NULL;
  
  // sort all digis according the detector layer into different sets
  // defining a order functor for the set should result in a set which
  // is already ordered from small to big strip numbers 
 
  map<CbmFiberHodoDigi*, Int_t> fIndices;
  
  Int_t nofDigis = fDigis->GetEntries();
  
  for (Int_t iDigi=0; iDigi < nofDigis; iDigi++ ) 
    {
      // cout << " CbmFiberHodoClusterFinder:: Clear all output structures/container" << endl;
      digi = (CbmFiberHodoDigi*) fDigis->At(iDigi);
      Int_t layer = CbmFiberHodoAddress::GetLayerAddress(digi->GetAddress());
      if(digi->GetCharge() > 100)
      {
	layerSet.insert(layer);
	fDigiMap[layer].insert(digi);
	fIndices[digi]=iDigi;          
      }
    }
  set <CbmFiberHodoDigi*, classcomp> digiSet;
  
  //  cout << " loop over all detector layers and for each detector layer loop over the " << endl;
  // already sorted strips to do the cluster finding. A cluster is defined by
  // neighboring strips which are above the threshold
  // TODO: Decide if the condition of the ADC threshold should be done here or
  //       in the unpacking
  //       Check if the set is realy ordered with increasing strip number
  for (set<Int_t>::iterator i = layerSet.begin(); i != layerSet.end(); i++) 
    {
      digiSet = fDigiMap[*i];
      Bool_t   newCluster = kTRUE;
      Int_t    fiberNr = -1;
      Int_t    fiberNrPrev = -1;
      Int_t    cluster_size =0;
      Double_t time = -1;
      Double_t timePrev = -1;
      for (set<CbmFiberHodoDigi*, classcomp>::iterator j = digiSet.begin(); j != digiSet.end(); j++) 
	{
	  if (newCluster) 
	    {
	      new ((*fClusters)[fClusters->GetEntriesFast()]) 
		CbmFiberHodoCluster(fIndices[*j]);
	      fiberNrPrev = CbmFiberHodoAddress::GetStripId((*j)->GetAddress());
	      newCluster = kFALSE;
	      digi = static_cast<CbmFiberHodoDigi*>(fDigis->At(fIndices[*j]));
	      timePrev = digi->GetTime();
	    } 
	  else 
	    {
	      // check if the next fiber is a direct neighbor of the previos one
	      fiberNr = CbmFiberHodoAddress::GetStripId((*j)->GetAddress());
	      digi = static_cast<CbmFiberHodoDigi*>(fDigis->At(fIndices[*j]));
	      time = digi->GetTime();
	      if (1 == fiberNr-fiberNrPrev && TMath::Abs(time -timePrev)< 40 ) 
		{
		  fiberNrPrev = fiberNr;
		  CbmFiberHodoCluster* cluster = (CbmFiberHodoCluster*) fClusters->Last();
		  Int_t index = fIndices[(*j)];
                  cluster->AddDigi(index);  
		}
	      else 
		{
		  new ((*fClusters)[fClusters->GetEntriesFast()]) 
		    CbmFiberHodoCluster(fIndices[*j]);
		  fiberNrPrev = CbmFiberHodoAddress::GetStripId((*j)->GetAddress());
		  newCluster = kFALSE;
		}
 	    }
	}
    }
  if(nofDigis>0)
    {
      Int_t nofClusterCandidates = fClusters->GetEntriesFast();
      int clust[4]={0,0,0,0}; 
      for (Int_t iclus = 0; iclus < nofClusterCandidates; iclus++)
	{
	  const CbmFiberHodoCluster* cluster = static_cast<const CbmFiberHodoCluster*>(fClusters->At(iclus));
	  Int_t nofFiber = cluster->GetNofDigis();
	  const CbmFiberHodoDigi* hodoDigi = (CbmFiberHodoDigi*)fDigis->At(cluster->GetDigi(0));
	  int layer= CbmFiberHodoAddress::GetLayerAddress(hodoDigi->GetAddress());
	  int fiber = CbmFiberHodoAddress::GetStripId(hodoDigi->GetAddress());
	  if(layer>2)
	    {
	      if(layer==16)
		clust[2]++;
	      if(layer==17)
		clust[3]++;
	    }
	  else
	    clust[layer]++;
	  
	}
      
      for(Int_t iclus = 0; iclus < nofClusterCandidates; iclus++) {
	const CbmFiberHodoCluster* cluster = static_cast<const CbmFiberHodoCluster*>(fClusters->At(iclus));
	Int_t nofFiber = cluster->GetNofDigis();
	if(nofFiber>2)
	  return;
	double strip = 0.;
	double time = 0.;
	Double_t charge = 0.;
	for (Int_t i = 0; i < nofFiber; ++i) {
	  const CbmFiberHodoDigi* hodoDigi = static_cast<CbmFiberHodoDigi*>(fDigis->At(cluster->GetDigi(i)));
	  strip += CbmFiberHodoAddress::GetStripId(hodoDigi->GetAddress());
	  time += hodoDigi->GetTime();
	  charge += hodoDigi->GetCharge();
	}
	Int_t size = finalClusters->GetEntriesFast();
	CbmFiberHodoCluster* new_cluster = new ((*finalClusters)[size]) CbmFiberHodoCluster();
	for (Int_t i = 0; i < nofFiber; ++i) {
	  const CbmFiberHodoDigi* hodo_digi = static_cast<const CbmFiberHodoDigi*>(fDigis->At(cluster->GetDigi(i)));
	  if (i == 0) {
	    new_cluster->SetAddress(hodo_digi->GetAddress());
	    new_cluster->AddDigi(cluster->GetDigi(i));
	  }
	}
	new_cluster->SetMean(strip/nofFiber);
	new_cluster->SetTime(time/nofFiber);
	new_cluster->SetCharge(charge);
	new_cluster->SetMeanError(nofFiber / TMath::Sqrt(12.));
      }  
    }
  

  Double_t x1;
  Double_t x2;
  Double_t y1;
  Double_t y2;
  Double_t pos;

  Double_t charge_x1 = 0.;
  Double_t charge_x2 = 0.;
  Double_t charge_y1 = 0.;
  Double_t charge_y2 = 0.;
  Double_t charge = 0.;
  Int_t address;
  Int_t layerID;
  CbmFiberHodoCluster* hodoClust = NULL;

  for ( UInt_t iClust=0; iClust < finalClusters->GetEntriesFast(); ++iClust) {
    hodoClust = static_cast<CbmFiberHodoCluster*>(finalClusters->At(iClust));
    address = hodoClust->GetAddress();        
    layerID = CbmFiberHodoAddress::GetLayerAddress(address);
    pos = hodoClust->GetMean();
    charge = hodoClust->GetCharge();

    switch (layerID) {
    case 0:
      if (charge > charge_x1 ) {        
	x1=64-pos;
	charge_x1 = charge;
      }
      break;
    case 16:
      if (charge > charge_y1 ) {        
	y1=pos;
	charge_y1 = charge;
      }
      break;
    case 1:
      if (charge > charge_x2 ) {        
	x2=pos;
	charge_x2 = charge;
      }
      break;
    case 17:
      if (charge > charge_y2 ) {        
	y2=pos;
	charge_y2 = charge;
      }
      break;
    }

  }

  if ( charge_x1 > 0. && charge_y1 > 0.) {
    hodo1_pos->Fill(x1, y1);
    hodo1x->Fill(x1);
    hodo1y->Fill(y1);
  }
 if ( charge_x2 > 0. && charge_y2 > 0.) {
   hodo2_pos->Fill(x2, y2);
   hodo2x->Fill(x2);
   hodo2y->Fill(y2);
  }
  if ( charge_x1 > 0. && charge_y1 > 0. && charge_x2 > 0. && charge_y2 > 0.) {
    hodo_xx->Fill(x1, x2);
    hodo_yy->Fill(y1, y2);
    hodo1_xcor->Fill(x1, x1-x2);
    hodo2_xcor->Fill(x2, x1-x2);
    hodo1_ycor->Fill(y1, y1-y2);
    hodo2_ycor->Fill(y2, y1-y2);
  }

  //    Double_t x1;
  //  Double_t y1;
    for(Int_t iclust=0; iclust<finalClusters->GetEntriesFast(); ++iclust ) {
      layerID = CbmFiberHodoAddress::GetLayerAddress(static_cast<CbmFiberHodoCluster*>(finalClusters->At(iclust))->GetAddress());
      switch (layerID) {
      case 0:
	x1=64-static_cast<CbmFiberHodoCluster*>(finalClusters->At(iclust))->GetMean();
	break;
      case 16:
	y1=static_cast<CbmFiberHodoCluster*>(finalClusters->At(iclust))->GetMean();
	break;
      }
      hodo1_pos_nocut->Fill(x1, y1);
      hodo1x_nocut->Fill(x1);
      hodo1y_nocut->Fill(y1);
    }
    

}
// --------------------------------------------------------------------

  // ---- Finish --------------------------------------------------------
  void CbmFiberHodoClusterFinder::Finish()
  {
    hodo1_pos->Write();
      hodo2_pos->Write();
      hodo_xx->Write();
      hodo_yy->Write();
      hodo1_xcor->Write();
      hodo2_xcor->Write();
      hodo1_ycor->Write();
      hodo2_ycor->Write();
      hodo1x->Write();
      hodo1y->Write();
      hodo2x->Write();
      hodo2y->Write();

  }





  ClassImp(CbmFiberHodoClusterFinder)

