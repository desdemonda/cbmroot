#include "StsCosyClusterFinder.h"

#include "CbmStsDigi.h"
#include "CbmStsCluster.h"
#include "CbmStsAddress.h"
#include "CbmCluster.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"
#include <iostream>

using std::map;
using std::set;
using namespace std;

// ---- Default constructor -------------------------------------------
StsCosyClusterFinder::StsCosyClusterFinder()
  :FairTask("StsCosyClusterFinder",1),
   fDigis(NULL),
   fClusters(NULL),
   finalClusters(NULL),
   fTriggeredMode(kFALSE),
   fTriggeredStation(2)
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
StsCosyClusterFinder::~StsCosyClusterFinder()
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
void StsCosyClusterFinder::SetParContainers()
{

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus StsCosyClusterFinder::ReInit(){
  
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus StsCosyClusterFinder::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  
  fDigis =(TClonesArray *)  ioman->GetObject("StsCalibDigi");
  if ( ! fDigis ) {
    LOG(ERROR) << "No StsDigi array!" << FairLogger::endl;
    LOG(ERROR) << "Task will be inactive" << FairLogger::endl;
    return kERROR;
  }
  
  finalClusters = new TClonesArray("CbmStsCluster", 100);
  ioman->Register("StsCluster","STS",finalClusters,kTRUE);

  fClusters = new TClonesArray("CbmStsCluster", 100);
  ioman->Register("StsClusterCandidate","STS",fClusters,kTRUE);
  
  return kSUCCESS;
  
} 
// --------------------------------------------------------------------

// ---- Exec ----------------------------------------------------------
void StsCosyClusterFinder::Exec(Option_t * option)
{
 
  fClusters->Clear();
  finalClusters->Clear();
  
  Double_t minCharge = 50;
  Double_t maxCharge = 500;

  
  map<Int_t, set<CbmStsDigi*, classcomp1> >::iterator mapIt;
  for (mapIt=fDigiMap.begin(); mapIt!=fDigiMap.end(); mapIt++) 
    {
      ((*mapIt).second).clear();
    }
  fDigiMap.clear();
  
  set<Int_t> layerSet;
  
  CbmStsDigi* digi = NULL;
  // sort all digis according the detector layer into different sets
  // defining a order functor for the set should result in a set which
  // is already ordered from small to big strip numbers 
  
  map<CbmStsDigi*, Int_t> fIndices;
  
  Int_t nofDigis = fDigis->GetEntries();

  for (Int_t iDigi=0; iDigi < nofDigis; iDigi++ ) 
    {
      //      cout <<  "StsCosyClusterFinder::   Clear all output structures/containers" << endl;
      digi = (CbmStsDigi*) fDigis->At(iDigi);
      int station = CbmStsAddress::GetElementId(digi->GetAddress(),kStsStation);
      int side = CbmStsAddress::GetElementId(digi->GetAddress(),kStsSide);
      Int_t layer= 2*station+side;
      
      if(fTriggeredMode && station==fTriggeredStation)maxCharge=200;

      if(digi->GetCharge() > minCharge && digi->GetCharge() < maxCharge)
      {
	layerSet.insert(layer);
	fDigiMap[layer].insert(digi);
	fIndices[digi]=iDigi;          
      }
    }
  
  set <CbmStsDigi*, classcomp1> digiSet;
  
  //  cout << " loop over all detector layers and for each detector layer loop over the " << endl;
  // already sorted strips to do the cluster finding. A cluster is defined by
  // neighboring strips which are above the threshold
  // TODO: Decide if the condition of the ADC threshold should be done here or
  //       in the unpacking
  //       Check if the set is realy ordered with increasing strip number

  for (set<Int_t>::iterator i = layerSet.begin(); i != layerSet.end(); i++) 
    {
      digiSet = fDigiMap[*i];
      Bool_t newCluster = kTRUE;
      Int_t stripNr = -1;
      Int_t stripNrPrev = -1;
      Int_t cluster_size =0;
      CbmStsCluster* cluster=NULL;
      Double_t time = -1;
      Double_t timePrev = -1;

      for (set<CbmStsDigi*, classcomp1>::iterator j = digiSet.begin(); j != digiSet.end(); j++) 
	{
	  if (newCluster) 
	    {
	      Int_t size = fClusters->GetEntriesFast();
	      cluster = new ((*fClusters)[size]) CbmStsCluster();
	      Int_t index = fIndices[(*j)];
	      //----time----
	      digi = static_cast<CbmStsDigi*>(fDigis->At(index));
	      timePrev = digi->GetTime();

	      cluster->AddDigi(index);  
	      stripNrPrev = CbmStsAddress::GetElementId((*j)->GetAddress(),kStsChannel);
	      newCluster = kFALSE;
	    } 
	  else 
	    {
	      Int_t index = fIndices[(*j)];
	      digi = static_cast<CbmStsDigi*>(fDigis->At(index));
	      time = digi->GetTime();
	      stripNr = CbmStsAddress::GetElementId((*j)->GetAddress(),kStsChannel);
	      
	      int station = CbmStsAddress::GetElementId(digi->GetAddress(),kStsStation);
	      
	      Bool_t TrCl = kTRUE;
	      
	      if(fTriggeredMode && station==fTriggeredStation)
	      {
		if(TMath::Abs(time - timePrev) > 1)TrCl=kFALSE;
	      }

	      if (1 == stripNr-stripNrPrev && TMath::Abs(time - timePrev)< 40 && TrCl) 
		{
		  stripNrPrev = stripNr;
		  timePrev = time;
		  cluster = (CbmStsCluster*) fClusters->Last();
		  cluster->AddDigi(index);  
		} 
	      else if(TrCl)
		{
		  Int_t size = fClusters->GetEntriesFast();
		  cluster = new ((*fClusters)[size]) CbmStsCluster();
		  index = fIndices[(*j)];
                  cluster->AddDigi(index);  
		  stripNrPrev = CbmStsAddress::GetElementId((*j)->GetAddress(),kStsChannel);
		  newCluster = kFALSE;
		}
 	    }
	}
    }
  
  if(nofDigis>0)
    {
      Int_t nofClusterCandidates = fClusters->GetEntriesFast();
      int clust[6]={0,0,0,0,0,0}; 
      int layer=0;
      for (Int_t iclus = 0; iclus < nofClusterCandidates; iclus++)
	{
	  Double_t chanNr = 0;
	  Double_t chanADC = 0.;
	  Double_t sumW = 0;
	  Double_t sumWX = 0;
	  Double_t error = 0;
	  layer=0;
	  const CbmStsCluster* cluster = static_cast<const CbmStsCluster*>(fClusters->At(iclus));
	  Int_t nofStrips = cluster->GetNofDigis();
	  double time = -999.;
	  double mean_time = 0.;
	  for(int st=0; st<nofStrips; st++)
	    {
	      const CbmStsDigi* temp_digi = (CbmStsDigi*)fDigis->At(cluster->GetDigi(st));
	      int station = CbmStsAddress::GetElementId(temp_digi->GetAddress(),kStsStation);
	      int side = CbmStsAddress::GetElementId(temp_digi->GetAddress(),kStsSide);
	      int ch = CbmStsAddress::GetElementId(temp_digi->GetAddress(),kStsChannel);
	      layer = 2*station+side;
	      chanNr += ch;
	      chanADC = temp_digi->GetCharge();
	      sumW += chanADC;
	      sumWX += ch * chanADC;
	      error += (chanADC * chanADC);
	      
	      time = temp_digi->GetTime();
	      mean_time+=temp_digi->GetTime();
	    }
	  if(nofStrips>1)
	    {
	      for(int st=0; st<nofStrips; st++)
		{
		  const CbmStsDigi* temp_digi = (CbmStsDigi*)fDigis->At(cluster->GetDigi(st));
		  int station = CbmStsAddress::GetElementId(temp_digi->GetAddress(),kStsStation);
		  int side = CbmStsAddress::GetElementId(temp_digi->GetAddress(),kStsSide);
		}
	    }
	  
	  Int_t size = finalClusters->GetEntriesFast();
	  CbmStsCluster* new_cluster = new ((*finalClusters)[size]) CbmStsCluster();
	  
	  for (Int_t i = 0; i < nofStrips; ++i)
	    {
	      Int_t digi_index=cluster->GetDigi(i);
	      const CbmStsDigi* temp_digi = static_cast<const CbmStsDigi*>(fDigis->At(digi_index));
	      if (i == 0)
		{
		  new_cluster->SetAddress(temp_digi->GetAddress());
		}
	      new_cluster->AddDigi(digi_index);
	    }
	  new_cluster->SetMean(sumWX / sumW);
	  new_cluster->SetProperties(0.,0.,0.,mean_time/nofStrips);
	  new_cluster->SetMeanError((1. / (sumW)) * TMath::Sqrt(error));
	}
    }
  
  
}
// --------------------------------------------------------------------

  // ---- Finish --------------------------------------------------------
  void StsCosyClusterFinder::Finish()
  {
  }





  ClassImp(StsCosyClusterFinder)

