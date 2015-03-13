/**
 * \file CbmLitFitTracksParallel.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#include "CbmLitFitTracksParallel.h"
#include "cbm/base/CbmLitTrackingGeometryConstructor.h"
#include "cbm/utils/CbmLitConverterParallel.h"
#include "std/utils/CbmLitMemoryManagment.h"
#include "parallel/LitDetectorLayout.h"
#include "parallel/LitScalTrack.h"
#include "parallel/LitScalPixelHit.h"
#include "parallel/LitTrackFitter.h"
#include "FairRootManager.h"
#include "CbmTrack.h"
#include "CbmStsTrack.h"
#include "FairTrackParam.h"

#include "TClonesArray.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

using std::vector;
using std::for_each;
using std::cout;
using std::endl;

CbmLitFitTracksParallel::CbmLitFitTracksParallel():
   FairTask(),
   fGlobalTracks(NULL),
   fStsTracks(NULL),
   fMuchTracks(NULL),
   fTrdTracks(NULL),
   fMuchPixelHits(NULL),
   fMuchStrawHits(NULL),
   fTrdHits(NULL)
{

}

CbmLitFitTracksParallel::~CbmLitFitTracksParallel()
{

}

InitStatus CbmLitFitTracksParallel::Init()
{
   ReadDataBranches();

   fFitWatch.Reset();
   fFitWithIOWatch.Reset();

   return kSUCCESS;
}

void CbmLitFitTracksParallel::Exec(
    Option_t* opt)
{
   static Int_t eventNo = 0;
   std::cout << "CbmLitFitTracksParallel::Exec: eventNo=" << eventNo++ << std::endl;

   DoFit();
}

void CbmLitFitTracksParallel::Finish()
{
   PrintStopwatchStatistics();
}

void CbmLitFitTracksParallel::ReadDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   assert(ioman != NULL);

   fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
   fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
   fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
   fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
   fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
   fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
   fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
}

void CbmLitFitTracksParallel::DoFit()
{
   static Bool_t isTrd = fTrdTracks != NULL;
   static Bool_t isMuch = fMuchTracks != NULL;

   static Bool_t firstTime = true;
   static lit::parallel::LitDetectorLayoutScal layout;
   if (firstTime) {
      if (isTrd) {
         CbmLitTrackingGeometryConstructor::Instance()->GetTrdLayoutScal(layout);
      } else if (isMuch) {
         CbmLitTrackingGeometryConstructor::Instance()->GetMuchLayoutScal(layout);
      }
      firstTime = false;
   }

   fFitWithIOWatch.Start(kFALSE);

   // Convert input data
   vector<lit::parallel::LitScalTrack*> ltracks;
   vector<lit::parallel::LitScalPixelHit*> lhits;
   if (isTrd) {
      CbmLitConverterParallel::CbmPixelHitArrayToLitScalPixelHitArray(fTrdHits, lhits);
      CbmLitConverterParallel::CbmTrackArrayToLitScalTrackArray(fTrdTracks, lhits, ltracks);
   } else if (isMuch) {
      CbmLitConverterParallel::CbmPixelHitArrayToLitScalPixelHitArray(fMuchPixelHits, lhits);
      CbmLitConverterParallel::CbmTrackArrayToLitScalTrackArray(fMuchTracks, lhits, ltracks);
   }

   // Replace first track parameter of the converted tracks with the last STS track parameter
   Int_t nofTracks = ltracks.size();
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      lit::parallel::LitScalTrack& track = *ltracks[iTrack];
      Int_t stsTrackId = track.GetPreviousTrackId();
      CbmStsTrack* stsTrack = static_cast<CbmStsTrack*>(fStsTracks->At(stsTrackId));
      lit::parallel::LitTrackParamScal lpar;
      CbmLitConverterParallel::FairTrackParamToLitTrackParamScal(stsTrack->GetParamLast(), &lpar);
      track.SetParamFirst(lpar);
   }

   fFitWatch.Start(kFALSE);
  // Int_t nofTracks = ltracks.size();
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      lit::parallel::LitScalTrack& track = *ltracks[iTrack];
      LitTrackFitter(track, layout);
   }
   fFitWatch.Stop();

   // Replace first and last track parameters for the fitted track
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      lit::parallel::LitScalTrack& track = *ltracks[iTrack];
      CbmTrack* cbmTrack = (isTrd) ?
               static_cast<CbmTrack*>(fTrdTracks->At(iTrack)) :
               static_cast<CbmTrack*>(fMuchTracks->At(iTrack));
      FairTrackParam firstParam, lastParam;
      CbmLitConverterParallel::LitTrackParamScalToFairTrackParam(&track.GetParamFirst(), &firstParam);
      CbmLitConverterParallel::LitTrackParamScalToFairTrackParam(&track.GetParamLast(), &lastParam);
      cbmTrack->SetParamFirst(&firstParam);
      cbmTrack->SetParamLast(&lastParam);
   }

   // Clear memory
   for_each(ltracks.begin(), ltracks.end(), DeleteObject());
   for_each(lhits.begin(), lhits.end(), DeleteObject());
   ltracks.clear();
   lhits.clear();

   fFitWithIOWatch.Stop();
}

void CbmLitFitTracksParallel::PrintStopwatchStatistics()
{
   cout << "CbmLitFitTracksParallel::PrintStopwatchStatistics: " << endl;
   cout << "fit without IO: counts=" << fFitWatch.Counter()
        << ", real=" << fFitWatch.RealTime() / fFitWatch.Counter()
        << "/" << fFitWatch.RealTime()
        << " s, cpu=" << fFitWatch.CpuTime() / fFitWatch.Counter()
        << "/" << fFitWatch.CpuTime() << endl;
   cout << "fit with IO: counts=" << fFitWithIOWatch.Counter()
        << ", real=" << fFitWithIOWatch.RealTime() / fFitWithIOWatch.Counter()
        << "/" << fFitWithIOWatch.RealTime()
        << " s, cpu=" << fFitWithIOWatch.CpuTime() / fFitWithIOWatch.Counter()
        << "/" << fFitWithIOWatch.CpuTime() << endl;
}
ClassImp(CbmLitFitTracksParallel);
