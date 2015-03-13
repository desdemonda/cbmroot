#include "finder/CbmLitFindMuchTracksEpoch.h"

#include "base/CbmLitToolFactory.h"
#include "base/CbmLitEnvironment.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitPixelHit.h"
#include "data/CbmLitTrack.h"
#include "utils/CbmLitConverter.h"
#include "utils/CbmLitMemoryManagment.h"

#include "CbmBaseHit.h"
#include "CbmPixelHit.h"
#include "CbmStsTrack.h"
#include "CbmMuchTrack.h"
#include "CbmGlobalTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmStsHit.h"
#include "CbmMuchPixelHit.h"

#include "TClonesArray.h"

#include <iostream>
#include <cmath>
#include <map>

using namespace std;
CbmLitFindMuchTracksEpoch::CbmLitFindMuchTracksEpoch():
   fStsTracks(NULL),
   fStsHits(NULL),
   fMuchPixelHits(NULL),
   fMuchTracks(NULL),
   fGlobalTracks(NULL),
   fLitStsTracks(),
   fLitHits(),
   fLitOutputTracks(),
   fFinder(),
   fMerger(),
   fFitter(),
   fTrackingType("branch"),
   fMergerType("nearest_hit"),
   fFitterType("lit_kalman"),
   fEventNo(0)
   ,fSliceSeparationTime(2)
   ,fHitTimeResolution(4)
{}

CbmLitFindMuchTracksEpoch::~CbmLitFindMuchTracksEpoch(){}

InitStatus CbmLitFindMuchTracksEpoch::Init(){
   ReadAndCreateDataBranches();
   InitTrackReconstruction();
   return kSUCCESS;
}

void CbmLitFindMuchTracksEpoch::Exec(Option_t* opt){
  fMuchTracks->Clear();
  fGlobalTracks->Clear();
  TClonesArray* fStsTracksSlice = new TClonesArray("CbmStsTrack",1000);
  TClonesArray* fMuchPixelHitsSlice = new TClonesArray("CbmMuchPixelHit",1000);
  multimap<Double_t,Int_t> mapTracks;
  multimap<Double_t,Int_t> mapHits;
  multimap<Double_t,Int_t>::iterator itTrackFirst;
  multimap<Double_t,Int_t>::iterator itTrackLast;
  multimap<Double_t,Int_t>::iterator itHitFirst;
  multimap<Double_t,Int_t>::iterator itHitLast;
  multimap<Double_t,Int_t>::iterator it;

  // sort sts tracks in time
  for (Int_t i=0;i<fStsTracks->GetEntriesFast();i++){
    CbmStsTrack* track = (CbmStsTrack*) fStsTracks->At(i);
    mapTracks.insert(pair<Double_t,Int_t>(track->GetTime(),i));
  }
  if (mapTracks.size()==0) return;

  // sort much hits in time
  TVector3 pos;
  for(Int_t ihit=0;ihit<fMuchPixelHits->GetEntriesFast();ihit++){
    CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchPixelHits->At(ihit);
    Double_t t = hit->GetTime();
    hit->Position(pos);
    mapHits.insert(pair<Double_t,Int_t>(t-pos.Mag()/29.97,ihit));
  }
  if (mapHits.size()==0) return;
  
  // create slices
  vector<Double_t> sliceFirstTime;
  vector<Double_t> sliceLastTime;
  sliceFirstTime.push_back(mapTracks.begin()->first);
  sliceLastTime.push_back(mapTracks.begin()->first);
  for (it=mapTracks.begin();it!=mapTracks.end();it++){
    Double_t t = it->first;
    if (t < *sliceLastTime.rbegin()+fSliceSeparationTime) *sliceLastTime.rbegin() = t;
    else {
      sliceFirstTime.push_back(t);
      sliceLastTime.push_back(t);
    }
  }
  printf("Slices: %i\n",sliceFirstTime.size());
  for (Int_t i=0;i<sliceFirstTime.size();i++){
    itTrackFirst = mapTracks.lower_bound(sliceFirstTime[i]);
    itTrackLast  = mapTracks.lower_bound(sliceLastTime[i]);
    itHitFirst   = mapHits.lower_bound(sliceFirstTime[i]-3*fHitTimeResolution);
    itHitLast    = mapHits.lower_bound(sliceLastTime[i]+3*fHitTimeResolution);
    ++itTrackLast;
    // Fill TClonesArrays of tracks and hits withing the slice
    Int_t nTracks = 0;
    for (it=itTrackFirst;it!=itTrackLast;it++)
      new((*fStsTracksSlice)[nTracks++]) CbmStsTrack(*((CbmStsTrack*) fStsTracks->At(it->second))); 
    Int_t nHits = 0;
    for (it=itHitFirst;it!=itHitLast;it++)
      new((*fMuchPixelHitsSlice)[nHits++]) CbmMuchPixelHit(*((CbmMuchPixelHit*) fMuchPixelHits->At(it->second))); 
    --itTrackLast;
    --itHitLast;
    printf("Slice %2i:",i);
    printf(" (%4.0f - %4.0f)",sliceFirstTime[i],sliceLastTime[i]);
    printf(" nTracks=%4i",nTracks);
    printf(" (%4.0f - %4.0f)",itTrackFirst->first,itTrackLast->first);
    printf(" nHits=%4i",nHits);
    printf(" (%4.0f - %4.0f)",itHitFirst->first,itHitLast->first);
    printf("\n");
    // Lit tracking
    CbmLitConverter::StsTrackArrayToTrackVector(fStsTracksSlice, fLitStsTracks);
    CbmLitConverter::HitArrayToHitVector(fMuchPixelHitsSlice, fLitHits);
    RunTrackReconstruction();
    CbmLitConverter::LitTrackVectorToGlobalTrackArray(fLitOutputTracks, fGlobalTracks, fStsTracks,NULL, fMuchTracks);
    ClearArrays();
    fStsTracksSlice->Clear();
    fMuchPixelHitsSlice->Clear();
  }
  printf("Event: %i\n",fEventNo++);
}

void CbmLitFindMuchTracksEpoch::SetParContainers(){
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();
   rtdb->getContainer("FairBaseParSet");
   rtdb->getContainer("CbmGeoMuchPar");
}

void CbmLitFindMuchTracksEpoch::Finish(){}

void CbmLitFindMuchTracksEpoch::ReadAndCreateDataBranches(){
  FairRootManager* ioman = FairRootManager::Instance();
  fStsTracks     = (TClonesArray*) ioman->GetObject("StsTrack");
  fStsHits       = (TClonesArray*) ioman->GetObject("StsHit");
  fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
  fGlobalTracks = new TClonesArray("CbmGlobalTrack",100);
  fMuchTracks = new TClonesArray("CbmMuchTrack", 100);
  ioman->Register("MuchTrack", "Much", fMuchTracks, kTRUE);
  ioman->Register("GlobalTrack", "Global", fGlobalTracks, kTRUE);
}

void CbmLitFindMuchTracksEpoch::InitTrackReconstruction(){
   CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
   if (fTrackingType == "branch" || fTrackingType == "nn" || fTrackingType == "weight" || fTrackingType == "nn_parallel") {
     std::string st("mu_");
     st += fTrackingType;
     fFinder = factory->CreateTrackFinder(st);
   } else {
     TObject::Fatal("CbmLitFindMuchTracksEpoch","Tracking type not found");
   }

   if (fFitterType == "lit_kalman") fFitter = factory->CreateTrackFitter("lit_kalman");
   else  TObject::Fatal("CbmLitFindMuchTracksEpoch","Fitter type not found");
}

void CbmLitFindMuchTracksEpoch::ClearArrays(){
   // Free memory
   for_each(fLitStsTracks.begin(), fLitStsTracks.end(), DeleteObject());
   for_each(fLitOutputTracks.begin(), fLitOutputTracks.end(), DeleteObject());
   for_each(fLitHits.begin(), fLitHits.end(), DeleteObject());
   fLitStsTracks.clear();
   fLitOutputTracks.clear();
   fLitHits.clear();
}

void CbmLitFindMuchTracksEpoch::RunTrackReconstruction(){
  fFinder->DoFind(fLitHits, fLitStsTracks, fLitOutputTracks);
   // Refit found tracks
   for(TrackPtrIterator it = fLitOutputTracks.begin(); it != fLitOutputTracks.end(); it++) {
      CbmLitTrack* track = *it;
      fFitter->Fit(track);
   }
}

ClassImp(CbmLitFindMuchTracksEpoch);
