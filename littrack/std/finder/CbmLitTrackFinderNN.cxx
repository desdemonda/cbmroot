/**
 * \file CbmLitTrackFinderNN.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \since 2008
 **/

#include "finder/CbmLitTrackFinderNN.h"

#include "data/CbmLitHit.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitPixelHit.h"
#include "interface/CbmLitTrackSelection.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackUpdate.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitMath.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <cmath>

using std::map;
using std::set;
using std::sqrt;
using std::make_pair;

CbmLitTrackFinderNN::CbmLitTrackFinderNN() :
   fTracks(),
   fFilter(),
   fSeedSelection(),
   fFinalSelection(),
   fPropagator(),
   fNofStations(0),
   fNofIterations(0),
   fIteration(0),
   fHitData(),
   fUsedHitsSet(),
   fUsedSeedsSet(),
   fMaxNofMissingHits(),
   fPDG(),
   fChiSqStripHitCut(),
   fChiSqPixelHitCut(),
   fSigmaCoef()
{
}

CbmLitTrackFinderNN::~CbmLitTrackFinderNN()
{
}

LitStatus CbmLitTrackFinderNN::DoFind(
   HitPtrVector& hits,
   TrackPtrVector& trackSeeds,
   TrackPtrVector& tracks)
{
   fTracks.clear();
   fUsedSeedsSet.clear();
   fUsedHitsSet.clear();
   fHitData.SetNofStations(fNofStations);

   for (fIteration = 0; fIteration < fNofIterations; fIteration++) {
//      std::cout << "CbmLitTrackFinderNN::DoFind: iteration=" << fIteration << std::endl;
      ArrangeHits(hits.begin(), hits.end());
//      std::cout << fHitData.ToString();

      InitTrackSeeds(trackSeeds.begin(), trackSeeds.end());
      FollowTracks(fTracks.begin(), fTracks.end());

      fFinalSelection->DoSelect(fTracks.begin(), fTracks.end());
      RemoveHits(fTracks.begin(), fTracks.end());
      CopyToOutput(fTracks.begin(), fTracks.end(), tracks);

      for_each(fTracks.begin(), fTracks.end(), DeleteObject());
      fTracks.clear();
      fHitData.Clear();
   }

//   std::cout << "tracks.size()=" << tracks.size() << std::endl;
//   for (Int_t i = 0; i < tracks.size(); i++) {
//	   std::cout << i << " " << tracks[i]->ToString();
//   }

   static Int_t eventNo = 0;
   std::cout << "CbmLitTrackFinderNN::DoFind: " << eventNo++ << " events processed" << std::endl;
   return kLITSUCCESS;
}

void CbmLitTrackFinderNN::ArrangeHits(
   HitPtrIterator itBegin,
   HitPtrIterator itEnd)
{
   for(HitPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitHit* hit = *it;
      if (fUsedHitsSet.find(hit->GetRefId()) != fUsedHitsSet.end()) { continue; } // FIXME Check if it works correctly for MUCH-TRD
      fHitData.AddHit(hit);
   }
   fHitData.Arrange();
}

void CbmLitTrackFinderNN::InitTrackSeeds(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
      (*it)->SetQuality(kLITGOOD);
   }

   fSeedSelection->DoSelect(itBegin, itEnd);

   for (TrackPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitTrack* track = *it;
      if (track->GetQuality() == kLITBAD) { continue; }
      if (fUsedSeedsSet.find(track->GetPreviousTrackId()) != fUsedSeedsSet.end()) { continue; }
      track->SetPDG(fPDG[fIteration]);
      track->SetChi2(0.);
      fTracks.push_back(new CbmLitTrack(*track));
   }
}

void CbmLitTrackFinderNN::FollowTracks(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   for (TrackPtrIterator itTrack = itBegin; itTrack != itEnd; itTrack++) {
      CbmLitTrack* track = *itTrack;

      for (Int_t iStation = 0; iStation < fNofStations; iStation++) {
         CbmLitTrackParam par(*track->GetParamLast());

         litfloat zMin = fHitData.GetMinZPos(iStation);
         if (fPropagator->Propagate(&par, zMin, fPDG[fIteration]) == kLITERROR) {
        	 break;
         }

         const vector<Int_t>& bins = fHitData.GetZPosBins(iStation);
         // map<bin index, pair<track parameter for the bin, true if track was propagated correctly >>
         map<Int_t, pair<CbmLitTrackParam, Bool_t> > binParamMap;
         vector<Int_t>::const_iterator itBins;
         for (itBins = bins.begin(); itBins != bins.end(); itBins++) {
            binParamMap[*itBins] = make_pair<CbmLitTrackParam, Bool_t>(CbmLitTrackParam(), true);
         }

         // Extrapolate track parameters to each Z position in the map.
         // This is done to improve calculation speed.
         // In case of planar station only 1 track extrapolation is required,
         // since all hits located at the same Z.
         map<Int_t, pair<CbmLitTrackParam, Bool_t> >::iterator itMap;
         for (itMap = binParamMap.begin(); itMap != binParamMap.end(); itMap++) {
            (*itMap).second.first = par;
            litfloat z = fHitData.GetZPosByBin(iStation, (*itMap).first);
            if (fPropagator->Propagate(&(*itMap).second.first, z, fPDG[fIteration]) == kLITERROR) {
            	(*itMap).second.second = false;
            }
         }

         // Loop over hits
         litfloat minChiSq = std::numeric_limits<litfloat>::max(); // minimum chi-square of hit
         const CbmLitHit* minHit = NULL; // Pointer to hit with minimum chi-square
         CbmLitTrackParam minPar; // Track parameters for closest hit
         const HitPtrVector& hits = fHitData.GetHits(iStation);
         for (HitPtrConstIterator itHit = hits.begin(); itHit != hits.end(); itHit++) {
            const CbmLitHit* hit = *itHit;
            Int_t bin = fHitData.GetBinByZPos(iStation, hit->GetZ());
            assert(binParamMap.find(bin) != binParamMap.end());
            if (!binParamMap[bin].second) continue; // Track parameters are wrong for this propagation
            CbmLitTrackParam tpar(binParamMap[bin].first);

            // Check preliminary if hit is in the validation gate.
            // This is done in order to speed up the algorithm.
            // Based on the predicted track position (w/o KF update step)
            // and maximum hit position error.
            if (hit->GetType() == kLITPIXELHIT) {
               const CbmLitPixelHit* pixelHit = static_cast<const CbmLitPixelHit*>(hit);
               litfloat maxErrX = fHitData.GetMaxErrX(iStation);
               litfloat devX = fSigmaCoef[fIteration] * (sqrt(tpar.GetCovariance(0) + maxErrX * maxErrX));
               litfloat maxErrY = fHitData.GetMaxErrY(iStation);
               litfloat devY = fSigmaCoef[fIteration] * (sqrt(tpar.GetCovariance(5) + maxErrY * maxErrY));
               bool hitInside = (pixelHit->GetX() < (tpar.GetX() + devX)) && (pixelHit->GetX() > (tpar.GetX() - devX))
                     && (pixelHit->GetY() < (tpar.GetY() + devY)) && (pixelHit->GetY() > (tpar.GetY() - devY));
               if (!hitInside) continue;
            }

            litfloat chi = std::numeric_limits<litfloat>::max();
            fFilter->Update(&tpar, hit, chi);
            bool hitInValidationGate = (hit->GetType() == kLITPIXELHIT && chi < fChiSqPixelHitCut[fIteration])
                  || (hit->GetType() == kLITSTRIPHIT && chi < fChiSqStripHitCut[fIteration]);
            if (hitInValidationGate && chi < minChiSq) { // Check if hit is inside validation gate and closer to the track.
               minChiSq = chi;
               minHit = hit;
               minPar = tpar;
            }
         }

         if (minHit != NULL) { // Check if hit was added
            track->AddHit(minHit);
            track->SetLastStationId(iStation);
            track->SetParamLast(&minPar);
            track->SetChi2(track->GetChi2() + minChiSq);
            track->SetNDF(lit::NDF(track));
         } else { // Missing hit
            track->SetNofMissingHits(track->GetNofMissingHits() + 1);
            if (track->GetNofMissingHits() > fMaxNofMissingHits[fIteration]) { break; }
         }
      }
   }
}

void CbmLitTrackFinderNN::RemoveHits(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   for(TrackPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitTrack* track = *it;
      if(track->GetQuality() == kLITBAD) { continue; }
      for (Int_t hit = 0; hit < track->GetNofHits(); hit++) {
         fUsedHitsSet.insert(track->GetHit(hit)->GetRefId());
      }
   }
}

void CbmLitTrackFinderNN::CopyToOutput(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd,
   TrackPtrVector& tracks)
{
   for(TrackPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitTrack* track = *it;
      if(track->GetQuality() == kLITBAD) { continue; }
      if (!track->CheckParams()) { continue; }
      fUsedSeedsSet.insert(track->GetPreviousTrackId());
      tracks.push_back(new CbmLitTrack(*track));
   }
}
