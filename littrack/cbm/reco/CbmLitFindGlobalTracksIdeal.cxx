/**
 * \file CbmLitFindGlobalTracksIdeal.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
#include "CbmLitFindGlobalTracksIdeal.h"
#include "CbmTrackMatchNew.h"
#include "CbmTofHit.h"
#include "CbmTofPoint.h"
#include "CbmGlobalTrack.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

CbmLitFindGlobalTracksIdeal::CbmLitFindGlobalTracksIdeal():
   fDet(),

   fMCTracks(NULL),
   fStsMatches(NULL),
   fMuchMatches(NULL),
   fTrdMatches(NULL),
   fTofMCPoints(NULL),
   fTofHits(NULL),
   fGlobalTracks(NULL),

   fMcStsMap(),
   fMcTrdMap(),
   fMcMuchMap(),
   fMcTofMap(),

   fEventNo(0)
{
}

CbmLitFindGlobalTracksIdeal::~CbmLitFindGlobalTracksIdeal()
{

}

InitStatus CbmLitFindGlobalTracksIdeal::Init()
{
   fDet.DetermineSetup();
   std::cout << fDet.ToString();

   ReadDataBranches();

   return kSUCCESS;
}

void CbmLitFindGlobalTracksIdeal::Exec(
   Option_t* opt)
{
   if (fDet.GetDet(kSTS)) { FillTrackMap(fMcStsMap, fStsMatches); }
   if (fDet.GetDet(kTRD)) { FillTrackMap(fMcTrdMap, fTrdMatches); }
   if (fDet.GetDet(kMUCH)) { FillTrackMap(fMcMuchMap, fMuchMatches); }
   if (fDet.GetDet(kTOF)) { FillMapTof(); }

   CreateGlobalTracks();

   std::cout << "Event: " << fEventNo++ << std::endl;
}

void CbmLitFindGlobalTracksIdeal::Finish()
{
}

void CbmLitFindGlobalTracksIdeal::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("Init","CbmRootManager is not instantiated"); }

   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   if (NULL == fMCTracks) { Fatal("Init","No MCTrack array!"); }

   //STS data
   if (fDet.GetDet(kSTS)) {
      fStsMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
      if (NULL == fStsMatches) { Fatal("Init","No StsTrackMatch array!"); }
   }

   //MUCH data
   if (fDet.GetDet(kMUCH)) {
      fMuchMatches = (TClonesArray*) ioman->GetObject("MuchTrackMatch");
      if (NULL == fMuchMatches) { Fatal("Init","No MuchTrackMatch array!"); }
   }

   //TRD data
   if (fDet.GetDet(kTRD)) {
      fTrdMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
      if (NULL == fTrdMatches) { Fatal("Init","No TrdTrackMatch array!"); }
   }

   //TOF data
   if (fDet.GetDet(kTOF)) {
      fTofMCPoints = (TClonesArray*) ioman->GetObject("TofPoint");
      if (NULL == fTofMCPoints ) { Fatal("Init","No TofPoint array!"); }
      fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
      if (NULL == fTofHits) { Fatal("Init", "No TofHit array!"); }
   }

   // Create and register CbmGlobalTrack array
   fGlobalTracks = new TClonesArray("CbmGlobalTrack",100);
   ioman->Register("GlobalTrack", "Global", fGlobalTracks, kTRUE);
}

void CbmLitFindGlobalTracksIdeal::FillTrackMap(
   std::map<Int_t, Int_t>& mcMap,
   const TClonesArray* matches)
{
   mcMap.clear();
   Int_t nofTracks = matches->GetEntriesFast();
   for(Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      CbmTrackMatchNew* trackMatch = (CbmTrackMatchNew*) matches->At(iTrack);
      if (trackMatch == NULL) { continue; }
      Int_t mcId = trackMatch->GetMatchedLink().GetIndex();
      if(mcId == -1) { continue; }
      mcMap.insert(std::pair<Int_t, Int_t>(mcId, iTrack));
   }
}

void CbmLitFindGlobalTracksIdeal::FillMapTof()
{
   fMcTofMap.clear();
   Int_t nofTofHits = fTofHits->GetEntriesFast();
   for(Int_t iTofHit = 0; iTofHit < nofTofHits; iTofHit++) {
      CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(iTofHit);
      if (tofHit == NULL) { continue; }
      CbmTofPoint* tofPoint = (CbmTofPoint*) fTofMCPoints->At(tofHit->GetRefId());
      if (tofPoint == NULL) { continue; }
      Int_t mcId = tofPoint->GetTrackID();
      if(mcId == -1) { continue; }
      fMcTofMap.insert(std::pair<Int_t, Int_t>(mcId, iTofHit));
   }
}

void CbmLitFindGlobalTracksIdeal::CreateGlobalTracks()
{
   fGlobalTracks->Clear();
   Int_t nGlobalTracks = 0;
   Int_t nofMCTracks = fMCTracks->GetEntriesFast();
   for (Int_t iMCTrack = 0; iMCTrack < nofMCTracks; iMCTrack++) {
      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrack);
      if (mcTrack==NULL) { continue; }
      Int_t stsId = -1, trdId = -1, muchId = -1, tofId = -1;
      if (fDet.GetDet(kSTS) && (fMcStsMap.find(iMCTrack) != fMcStsMap.end())) { stsId = fMcStsMap[iMCTrack]; }
      if (fDet.GetDet(kTRD) && (fMcTrdMap.find(iMCTrack) != fMcTrdMap.end())) { trdId = fMcTrdMap[iMCTrack]; }
      if (fDet.GetDet(kMUCH) && (fMcMuchMap.find(iMCTrack) != fMcMuchMap.end())) { muchId = fMcMuchMap[iMCTrack]; }
      if (fDet.GetDet(kTOF) && (fMcTofMap.find(iMCTrack) != fMcTofMap.end())) { tofId = fMcTofMap[iMCTrack]; }

      if (stsId == -1 && trdId == -1 && muchId == -1 && tofId == -1) { continue; }

      CbmGlobalTrack* globalTrack = new ((*fGlobalTracks)[nGlobalTracks++]) CbmGlobalTrack();
      globalTrack->SetStsTrackIndex(stsId);
      globalTrack->SetTrdTrackIndex(trdId);
      globalTrack->SetTofHitIndex(tofId);
      globalTrack->SetMuchTrackIndex(muchId);
   }
}

ClassImp(CbmLitFindGlobalTracksIdeal);
