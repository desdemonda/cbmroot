/**
 * \file CbmLitTrackFinderBranch.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 **/

#include "finder/CbmLitTrackFinderBranch.h"

#include "data/CbmLitTrack.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitHit.h"
#include "interface/CbmLitTrackSelection.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackUpdate.h"
#include "interface/CbmLitTrackFitter.h"
#include "selection/CbmLitQualitySort.h"
#include "utils/CbmLitComparators.h"
#include "utils/CbmLitMemoryManagment.h"
#include "utils/CbmLitMath.h"

#include <iostream>
#include <algorithm>

using std::make_pair;

CbmLitTrackFinderBranch::CbmLitTrackFinderBranch():
   fHitData(),
   fUsedHitsSet(),
   fUsedSeedsSet(),
   fTracks(),
   fFilter(),
   fSeedSelection(),
   fFinalSelection(),
   fPropagator(),
   fNofStations(0),
   fNofIterations(0),
   fIteration(0),
   fMaxNofHitsInValidationGate(3),
   fMaxNofBranches(512),
   fMaxNofMissingHits(),
   fPDG(),
   fChiSqStripHitCut(),
   fChiSqPixelHitCut(),
   fSigmaCoef()
{
}

CbmLitTrackFinderBranch::~CbmLitTrackFinderBranch()
{
}

LitStatus CbmLitTrackFinderBranch::DoFind(
   HitPtrVector& hits,
   TrackPtrVector& trackSeeds,
   TrackPtrVector& tracks)
{
   fTracks.clear();
   fUsedSeedsSet.clear();
   fUsedHitsSet.clear();
   fHitData.SetNofStations(fNofStations);

   for (fIteration = 0; fIteration < fNofIterations; fIteration++) {
//      std::cout << "CbmLitTrackFinderBranch::DoFind: iteration=" << fIteration << std::endl;
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

   static Int_t eventNo = 0;
   std::cout << "-I- CbmLitTrackFinderBranch: " << eventNo++  << " events processed" << std::endl;
   return kLITSUCCESS;
}

void CbmLitTrackFinderBranch::ArrangeHits(
   HitPtrIterator itBegin,
   HitPtrIterator itEnd)
{
   for(HitPtrIterator it = itBegin; it != itEnd; it++) {
      CbmLitHit* hit = *it;
      if (fUsedHitsSet.find(hit->GetRefId()) != fUsedHitsSet.end()) { continue; }
      fHitData.AddHit(hit);
   }
   fHitData.Arrange();
}

void CbmLitTrackFinderBranch::InitTrackSeeds(
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
      CbmLitTrack* newTrack = new CbmLitTrack(*track);
      newTrack->SetPDG(fPDG[fIteration]);
      newTrack->SetChi2(0.);
      fTracks.push_back(newTrack);
   }
}

void CbmLitTrackFinderBranch::FollowTracks(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd)
{
   // Loop over the track seeds
   for (TrackPtrIterator itTrack = itBegin; itTrack != itEnd; itTrack++) {
      CbmLitTrack* track = *itTrack;

      // Vector with track branches for one input track seed
      vector<CbmLitTrack*> branches;
      // Initially start with one branch which is the same as track seed
      branches.push_back(new CbmLitTrack(*track));

      for (Int_t iStation = 0; iStation < fNofStations; iStation++) { // Loop over stations
         litfloat zMin = fHitData.GetMinZPos(iStation);
         const vector<Int_t>& bins = fHitData.GetZPosBins(iStation);
         // map<bin index, pair<track parameter for the bin, true if track was propagated correctly >>
         map<Int_t, pair<CbmLitTrackParam, Bool_t> > binParamMap;
         vector<Int_t>::const_iterator itBins;
         for (itBins = bins.begin(); itBins != bins.end(); itBins++) {
            binParamMap[*itBins] = make_pair<CbmLitTrackParam, Bool_t>(CbmLitTrackParam(), true);
         }
         // Number of branches can change in the next loop turn
         // since branches array is filled with additional track branches
         // which were created on current station
         Int_t nofBranches = branches.size();
         for (Int_t iBranch = 0; iBranch < nofBranches; iBranch++) { // Loop over branches
            CbmLitTrack* branch = branches[iBranch];
            // Check for the missing hits
            if (branch->GetNofMissingHits() > fMaxNofMissingHits[fIteration]) { continue; }

            CbmLitTrackParam par(*branch->GetParamLast());
            if (fPropagator->Propagate(&par, zMin, fPDG[fIteration]) == kLITERROR) {
            	break;
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
            map<litfloat, pair<const CbmLitHit*, CbmLitTrackParam> > chiHitPar;
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
               if (hitInValidationGate) { // Check if hit is inside validation gate
                  chiHitPar[chi] = make_pair(hit, tpar);
               }
            }

            Int_t nofHitsInValidationGate = chiHitPar.size();
            // Check if hit was added
            if (nofHitsInValidationGate == 1) {
               // Only one hit added, no additional branches are created, just update branch parameters
               const map<litfloat, pair<const CbmLitHit*, CbmLitTrackParam> >::const_iterator it = chiHitPar.begin();
               branch->AddHit((*it).second.first);
               branch->SetLastStationId(iStation);
               branch->SetParamLast(&(*it).second.second);
               branch->SetChi2(branch->GetChi2() + (*it).first);
               branch->SetNDF(lit::NDF(branch));
            } else if (nofHitsInValidationGate > 1) {
               // If more than one hit is in the validation gate, than create additional branches.
               // For the first hit use the same branch.
               Int_t counter = 0;
               CbmLitTrack tt(*branch);
               map<litfloat, pair<const CbmLitHit*, CbmLitTrackParam> >::const_iterator it;
               for (it = chiHitPar.begin(); it != chiHitPar.end(); it++) {
                  // Create new branch starting with the second hit
                  CbmLitTrack* tbranch = (it != chiHitPar.begin()) ? new CbmLitTrack(tt) : branch;
                  if (it != chiHitPar.begin()) branches.push_back(tbranch);
                  tbranch->AddHit((*it).second.first);
                  tbranch->SetLastStationId(iStation);
                  tbranch->SetParamLast(&(*it).second.second);
                  tbranch->SetChi2(tbranch->GetChi2() + (*it).first);
                  tbranch->SetNDF(lit::NDF(branch));
                  counter++;
                  // Stop if number of hits in the validation gate is too high
                  if (counter > fMaxNofHitsInValidationGate) break;
            	  // Check if number of branches exceeds the limit.
            	  // Do not create additional branches in this case and continue propagation of all current branches.
            	  // Use the best hit as in case of nearest neighbor tracking.
            	  if (nofBranches > fMaxNofBranches) break;
            	  //if (branches.size() > fMaxNofBranches) break;
               }
            } else { // Missing hit
               branch->SetNofMissingHits(branch->GetNofMissingHits() + 1);
            }
         } // Loop over branches
      } // Loop over stations
      // Put somewhere a cut on maximum number of branches for one input seed

      // Select the best branch
      Int_t nofBranches = branches.size();
      if (nofBranches > 0) {
          // Sort all branches by quality
         CbmLitQualitySort::DoSortChiSqOverNDF(branches.begin(), branches.end());
         // Override existing track with the best branch
         *track = *branches.front();
      }

      // Clean branches array
      for_each(branches.begin(), branches.end(), DeleteObject());
      branches.clear();
   }
}

void CbmLitTrackFinderBranch::RemoveHits(
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

void CbmLitTrackFinderBranch::CopyToOutput(
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
