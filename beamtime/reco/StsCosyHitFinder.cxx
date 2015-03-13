#include "StsCosyHitFinder.h"

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
#include "TVector3.h"
#include <iostream>

#include "CbmFiberHodoClusterFinder.h"
#include "CbmFiberHodoDigi.h"
#include "CbmFiberHodoCluster.h"
#include "CbmFiberHodoAddress.h"
#include "CbmStsHit.h"

using std::map;
using std::set;
using namespace std;

// ---- Default constructor -------------------------------------------
StsCosyHitFinder::StsCosyHitFinder()
  :FairTask("StsCosyHitFinder",1),
   fDigis(NULL),
   fHits(NULL),
   stsClusters(NULL),
   hodoClusters(NULL),
   fTriggeredMode(kFALSE),
   fTriggeredStation(2)
{
  fChain = new TChain("cbmsim");
  //  outFile=NULL;
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
StsCosyHitFinder::~StsCosyHitFinder()
{
  if (fHits){
    fHits->Delete();
    delete fHits;
  }
  if(fDigis){
    fDigis->Clear("C");
    fDigis->Delete();
    delete fDigis;
  }

  if(stsClusters){
    stsClusters->Clear("C");
    stsClusters->Delete();
    delete stsClusters;
  }
  if(hodoClusters){
    hodoClusters->Clear("C");
    hodoClusters->Delete();
    delete hodoClusters;
  }

}

// ----  Initialisation  ----------------------------------------------
void StsCosyHitFinder::SetParContainers()
{

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus StsCosyHitFinder::ReInit(){
  
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus StsCosyHitFinder::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  fHits = new TClonesArray("CbmStsHit", 1000);
  ioman->Register("StsHit", "Hit in STS", fHits, kTRUE);
  
  fDigis =(TClonesArray *)  ioman->GetObject("StsCalibDigi");
  if ( ! fDigis ) {
    LOG(ERROR) << "No StsDigi array!" << FairLogger::endl;
    LOG(ERROR) << "Task will be inactive" << FairLogger::endl;
    return kERROR;
  }
  
  stsClusters =(TClonesArray *)  ioman->GetObject("StsCluster");
  if ( !stsClusters  ) {
    LOG(ERROR) << "No StsCluster array!" << FairLogger::endl;
    LOG(ERROR) << "Task will be inactive" << FairLogger::endl;
    return kERROR;
  }

  hodoClusters =(TClonesArray *)  ioman->GetObject("FiberHodoCluster");
  if ( !hodoClusters  ) {
    LOG(ERROR) << "No FiberHodoCluster array!" << FairLogger::endl;
    LOG(ERROR) << "Task will be inactive" << FairLogger::endl;
    return kERROR;
  }

  return kSUCCESS;
  
} 
// --------------------------------------------------------------------

// ---- Exec ----------------------------------------------------------
void StsCosyHitFinder::Exec(Option_t * option)
{
 
  fHits->Delete();
  
  Int_t nofSTSClusters = stsClusters->GetEntries();
  Int_t nofHodoClusters = hodoClusters->GetEntries();
  if(nofSTSClusters<1 || nofHodoClusters <1  )
    return;
  
  Double_t time_limits[] = {20, 22, 18};
  Double_t time_shifts[] = {-6, -4, 10};
  if(fTriggeredMode){time_limits[2]=4;time_shifts[2]=38;}
      
  
  int detector_layer=-1;
  CbmFiberHodoCluster* hodo_temp=NULL;
  Double_t h1_time=999.;
  Double_t h2_time=999.;
 
  vector<int>  hodo1X, hodo1Y, hodo2X, hodo2Y;
  for (Int_t iclus = 0; iclus < nofHodoClusters; iclus++)
    {
      const CbmFiberHodoCluster* cluster = static_cast<const CbmFiberHodoCluster*>( hodoClusters->At(iclus));
      detector_layer= CbmFiberHodoAddress::GetLayerAddress(cluster->GetAddress());
      if(detector_layer ==0)
	hodo1Y.push_back(iclus);
      if(detector_layer ==16)
	hodo1X.push_back(iclus);
      if(detector_layer ==1)
	hodo2Y.push_back(iclus);
      if(detector_layer ==17)
	hodo2X.push_back(iclus);
    }
  if(hodo1X.size() >3 || hodo1Y.size() >3 || hodo2X.size()>3 || hodo2Y.size()>3)
      return; 
  for(int i=0; i< hodo1X.size(); i++)
    {
      Int_t frontClusterId =hodo1X[i];
      const CbmFiberHodoCluster* frontCluster = static_cast<const CbmFiberHodoCluster*>( hodoClusters->At(hodo1X[i]));
      Double_t frontChannel=frontCluster->GetMean();
      Double_t front_time = frontCluster->GetTime();
      for(int k=0; k< hodo1Y.size(); k++)
	{
	  Int_t backClusterId = hodo1Y[k];
	  const CbmFiberHodoCluster* backCluster = static_cast<const CbmFiberHodoCluster*>( hodoClusters->At(hodo1Y[k]));
	  Double_t backChannel = backCluster->GetMean();
	  Double_t back_time = backCluster->GetTime();
	  if(TMath::Abs(back_time -front_time)<10)
	    {
	      Double_t xHit = 0.1*((frontChannel-32.5)*1.01);
	      Double_t yHit = 0.1*((backChannel-32.5)*1.01);
	      // Double_t xHit = 0.1*((frontChannel-32.5)*1.01+3.56);
	      // Double_t yHit = 0.1*((backChannel-32.5)*1.01-2.72);
	      Double_t zHit = 5.5+0.4;
	      h1_time = (back_time + front_time)/2.;
	      TVector3 pos(xHit, yHit, zHit);
	      TVector3 dpos;
	      dpos.SetXYZ(frontCluster->GetMeanError()*0.1,backCluster->GetMeanError()*0.1, 0.);
	      Int_t size = fHits->GetEntriesFast();
	      new ((*fHits)[size]) CbmStsHit(frontCluster->GetAddress(), 
					     pos, dpos, 0, frontClusterId, backClusterId, 
					     0, 0, 0,(back_time + front_time)/2.);
	    }
	}
    }

  for(int i=0; i< hodo2X.size(); i++)
    {
      Int_t frontClusterId =hodo2X[i];
      const CbmFiberHodoCluster* frontCluster = static_cast<const CbmFiberHodoCluster*>( hodoClusters->At(hodo2X[i]));
      Double_t frontChannel=frontCluster->GetMean();
      Double_t front_time = frontCluster->GetTime();
      for(int k=0; k< hodo2Y.size(); k++)
	{
	  Int_t backClusterId = hodo2Y[k];
	  const CbmFiberHodoCluster* backCluster = static_cast<const CbmFiberHodoCluster*>( hodoClusters->At(hodo2Y[k]));
	  Double_t backChannel = backCluster->GetMean();
	  Double_t back_time = backCluster->GetTime();
	  if(TMath::Abs(back_time -front_time)<10)
	    {
	      Double_t xHit = 0.1*((frontChannel-32.5)*1.01);
	      Double_t yHit = 0.1*((backChannel-32.5)*1.01);
	      // Double_t xHit = 0.1*((frontChannel-32.5)*1.01-15.14);
	      // Double_t yHit = 0.1*((backChannel-32.5)*1.01+0.43);
	      Double_t zHit = 301.5+0.4;
	      h2_time = (back_time + front_time)/2.;
	      TVector3 pos(xHit, yHit, zHit);
	      TVector3 dpos;
	      dpos.SetXYZ(frontCluster->GetMeanError()*0.1,backCluster->GetMeanError()*0.1, 0.);
	      Int_t size = fHits->GetEntriesFast();
	      new ((*fHits)[size]) CbmStsHit(frontCluster->GetAddress(), 
					     pos, dpos, 0, frontClusterId, backClusterId, 
					     0, 0, 4,(back_time + front_time)/2.);
	    }
	}
    }
 
  if( (h2_time - h1_time) < -110 || (h2_time - h1_time) >  -85)
    return;

  vector<int>  sts_0p, sts_0n, sts_1p, sts_1n, sts_2p, sts_2n; 
  
  double x=-100;
  double y=-100;
  int clust[6]={0,0,0,0,0,0}; 
  
  for (Int_t iclus = 0; iclus < nofSTSClusters; iclus++)
    {
      int layer =-1;
      const CbmStsCluster* cluster = static_cast<const CbmStsCluster*>(stsClusters->At(iclus));
      Int_t nofStrips = cluster->GetNofDigis();
      double adc=-100;
      
      int station = CbmStsAddress::GetElementId(cluster->GetAddress(),kStsStation);
      int side = CbmStsAddress::GetElementId(cluster->GetAddress(),kStsSide);
      int ch = CbmStsAddress::GetElementId(cluster->GetAddress(),kStsChannel);

      layer = 2*station+side;

      clust[layer]++;
      
      if(layer ==0)
	sts_0n.push_back(iclus);
      if(layer ==1)
	sts_0p.push_back(iclus);
      if(layer ==2)
	sts_1n.push_back(iclus);
      if(layer ==3)
	sts_1p.push_back(iclus);
      if(layer ==4)
	sts_2n.push_back(iclus);
      if(layer ==5)
	sts_2p.push_back(iclus);
    }
  
  for(int i=0; i< sts_0n.size(); i++)
    {
      Int_t frontClusterId = sts_0n[i];
      const CbmStsCluster* frontCluster = static_cast<const CbmStsCluster*>(stsClusters->At(sts_0n[i]));
      Double_t frontChannel = frontCluster->GetMean();
      Double_t front_time = frontCluster->GetTime();
      for(int k=0; k< sts_0p.size(); k++)
	{
	  Int_t backClusterId = sts_0p[k];
	  const CbmStsCluster* backCluster = static_cast<const CbmStsCluster*>(stsClusters->At(sts_0p[k]));
	  Double_t backChannel = backCluster->GetMean();
	  Double_t back_time = backCluster->GetTime();
	  if(TMath::Abs(back_time -front_time - time_shifts[0]) < time_limits[0])
	    {
	      Double_t xHit = (frontChannel - 127)*0.005;
	      Double_t yHit = (backChannel-127)*0.005;
	      Double_t zHit = 25.40;
	      TVector3 pos(xHit, yHit, zHit);
	      //	  cout << pos << endl;
	      TVector3 dpos;
	      dpos.SetXYZ(frontCluster->GetMeanError()*0.005,backCluster->GetMeanError()*0.005, 0.);
	      Int_t size = fHits->GetEntriesFast();
	      new ((*fHits)[size]) CbmStsHit(frontCluster->GetAddress(), 
					     pos, dpos, 0, frontClusterId, backClusterId, 
					     0, 0, 1,(back_time + front_time)/2.);
	    }
	}
    }
  
  for(int i=0; i< sts_1n.size(); i++)
    {
      Int_t frontClusterId = sts_1n[i];
      const CbmStsCluster* frontCluster = static_cast<const CbmStsCluster*>(stsClusters->At(sts_1n[i]));
      Double_t frontChannel = frontCluster->GetMean();
      Double_t front_time = frontCluster->GetTime();
      for(int k=0; k< sts_1p.size(); k++)
	{
	  Int_t backClusterId = sts_1p[k];
	  const CbmStsCluster* backCluster = static_cast<const CbmStsCluster*>(stsClusters->At(sts_1p[k]));
	  Double_t backChannel = backCluster->GetMean();
	  Double_t back_time = backCluster->GetTime();
	  if(TMath::Abs(back_time -front_time - time_shifts[1]) < time_limits[1])
	    {
	      Double_t xHit = (frontChannel-127)*0.005;
	      Double_t yHit = (backChannel-127)*0.005;
	      Double_t zHit = 123.40;
	      
	      TVector3 pos(xHit, yHit, zHit);
	      TVector3 dpos;
	      dpos.SetXYZ(frontCluster->GetMeanError()*0.005,backCluster->GetMeanError()*0.005, 0.);
	      Int_t size = fHits->GetEntriesFast();
	      new ((*fHits)[size]) CbmStsHit(frontCluster->GetAddress(), 
					     pos, dpos, 0, frontClusterId, backClusterId, 
					     0, 0, 3,(back_time + front_time)/2.);
	    }
	}
    }

  for(int i=0; i< sts_2n.size(); i++)
    {
      Int_t frontClusterId = sts_2n[i];
      const CbmStsCluster* frontCluster = static_cast<const CbmStsCluster*>(stsClusters->At(sts_2n[i]));
      Double_t frontChannel = frontCluster->GetMean();
      Double_t front_time = frontCluster->GetTime();
      for(int k=0; k< sts_2p.size(); k++)
	{
	  Int_t backClusterId = sts_2p[k];
	  const CbmStsCluster* backCluster = static_cast<const CbmStsCluster*>(stsClusters->At(sts_2p[k]));
	  Double_t backChannel = backCluster->GetMean();
	  Double_t back_time = backCluster->GetTime();
	  
	  if(TMath::Abs(back_time -front_time - time_shifts[2]) < time_limits[2])
	    {
	      Double_t zHit = 70.0;
	      Double_t xHit = (frontChannel+448)*0.0058 - 3.;
	      Double_t yHit = (frontChannel+448 -(backChannel+515))*0.0058/TMath::Tan(7.5*TMath::DegToRad()) + 3.;	      
	      TVector3 pos(xHit, yHit, zHit);
	      TVector3 dpos;
	      dpos.SetXYZ(frontCluster->GetMeanError()*0.0058,backCluster->GetMeanError()*0.0058, 0.);
	      Int_t size = fHits->GetEntriesFast();
	      new ((*fHits)[size]) CbmStsHit(frontCluster->GetAddress(), 
					     pos, dpos, 0, frontClusterId, backClusterId, 
					     0, 0, 2,(back_time + front_time)/2.);
	    }
	}
    }

  stsClusters->Clear();
  hodoClusters->Clear();
}
// --------------------------------------------------------------------

  // ---- Finish --------------------------------------------------------
  void StsCosyHitFinder::Finish()
  {
  }





  ClassImp(StsCosyHitFinder)

