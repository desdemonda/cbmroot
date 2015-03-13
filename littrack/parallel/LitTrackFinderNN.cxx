/**
 * \file LitTrackFinderNN.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#include "LitTrackFinderNN.h"
#include "LitExtrapolation.h"
#include "LitAddMaterial.h"
#include "LitFiltration.h"
#include "LitMath.h"
#include "LitTrackSelection.h"

#include <iostream>
#include <limits>
#include <algorithm>
#include <map>
using std::map;
using std::for_each;
using std::cout;
using std::endl;
using std::numeric_limits;

const fscal lit::parallel::LitHitData::EPSILON = 0.005;

lit::parallel::LitTrackFinderNN::LitTrackFinderNN() :
   fTracks(),
   fHitData(),
   fUsedHitsSet(),
   fUsedSeedsSet(),
   fLayout(),
   fNofIterations(0),
   fIteration(0),
   fMaxNofMissingHits(),
   fPDG(),
   fChiSqStripHitCut(),
   fChiSqPixelHitCut(),
   fSigmaCoef()
{

}

lit::parallel::LitTrackFinderNN::~LitTrackFinderNN()
{

}

void lit::parallel::LitTrackFinderNN::DoFind(
   const vector<lit::parallel::LitScalPixelHit*>& hits,
   const vector<lit::parallel::LitScalTrack*>& trackSeeds,
   vector<lit::parallel::LitScalTrack*>& tracks)
{
   fTracks.clear();
   fUsedSeedsSet.clear();
   fUsedHitsSet.clear();
   fHitData.SetNofStations(fLayout.GetNofStations());

   for (fIteration = 0; fIteration < fNofIterations; fIteration++) {
    //  cout << "LitTrackFinderNN::DoFind: iteration=" << fIteration << endl;
      ArrangeHits(hits);
    //  cout << fHitData.ToString();

      InitTrackSeeds(trackSeeds);
      FollowTracks();
      SelectTracks();
      RemoveHits();
      CopyToOutput(tracks);

      for_each(fTracks.begin(), fTracks.end(), DeleteObject());
      fTracks.clear();
      fHitData.Clear();
   }

   static int eventNo = 0;
   cout << "LitTrackFinderNN::DoFind: eventNo=" << eventNo++ << endl;
}

void lit::parallel::LitTrackFinderNN::ArrangeHits(
      const vector<lit::parallel::LitScalPixelHit*>& hits)
{
   unsigned int nofHits = hits.size();
   for (unsigned int iHit = 0; iHit < nofHits; iHit++) {
      LitScalPixelHit* hit = hits[iHit];
      if (fUsedHitsSet.find(hit->refId) != fUsedHitsSet.end()) { continue; }
      fHitData.AddHit(hit);
   }
   fHitData.Arrange();
}

void lit::parallel::LitTrackFinderNN::InitTrackSeeds(
      const vector<lit::parallel::LitScalTrack*>& trackSeeds)
{
   unsigned int nofSeeds = trackSeeds.size();
   for (unsigned int iTrack = 0; iTrack < nofSeeds; iTrack++) {
      LitScalTrack* seed = trackSeeds[iTrack];
      // Apply cuts here
      if (std::fabs(seed->GetParamFirst().Qp) > 10) continue; // momentum cut 0.1 GeV

      //if (fUsedSeedsSet.find(seed->GetPreviousTrackId()) != fUsedSeedsSet.end()) { continue; }
      LitScalTrack* track = new LitScalTrack();
      // FIXME if several iterations this procedure will be repeated!!
      LitTrackParamScal par = seed->GetParamFirst();
      PropagateVirtualStations(par);
      track->SetParamFirst(par);
      track->SetParamLast(par);
      track->SetPreviousTrackId(seed->GetPreviousTrackId());
      fTracks.push_back(track);
   }
}

void lit::parallel::LitTrackFinderNN::PropagateVirtualStations(
      LitTrackParamScal& par)
{
   unsigned char nofVirtualStations = fLayout.GetNofVirtualStations();
   unsigned char nofSteps = (nofVirtualStations - 1) / 2;
   for (unsigned char iStep = 0; iStep < nofSteps; iStep++) {
      const LitVirtualStationScal& vsFront = fLayout.GetVirtualStation(2 * iStep + 0);
      const LitVirtualStationScal& vsMiddle = fLayout.GetVirtualStation(2 * iStep + 1);
      const LitVirtualStationScal& vsBack = fLayout.GetVirtualStation(2 * iStep + 2);
      if (vsFront.GetField().IsEmpty() || vsMiddle.GetField().IsEmpty() || vsBack.GetField().IsEmpty()) {
         LitLineExtrapolation(par, vsBack.GetZ());
      } else {
         LitRK4Extrapolation(par, vsBack.GetZ(), vsFront.GetField(), vsMiddle.GetField(), vsBack.GetField());
      }

      if (!vsFront.GetMaterial().IsEmpty()) {
         fscal thickness = vsFront.GetMaterial().GetMaterial(par.X, par.Y);
         if (thickness > 0) LitAddMaterial<fscal>(par, thickness);
      }

      if (!vsMiddle.GetMaterial().IsEmpty()) {
         fscal thickness = vsMiddle.GetMaterial().GetMaterial(par.X, par.Y);
         if (thickness > 0) LitAddMaterial<fscal>(par, thickness);
      }

      if (!vsBack.GetMaterial().IsEmpty()) {
         fscal thickness = vsBack.GetMaterial().GetMaterial(par.X, par.Y);
         if (thickness > 0) LitAddMaterial<fscal>(par, thickness);
      }
   }
}

void lit::parallel::LitTrackFinderNN::PropagateToStation(
      unsigned char stationId,
      LitTrackParamScal& par)
{
   const LitStationScal& station = fLayout.GetStation(stationId);
   unsigned char nofVirtualStations = station.GetNofVirtualStations();
   if (nofVirtualStations == 1) {
      const LitVirtualStationScal& vs = station.GetVirtualStation(0);
      fscal z = vs.GetZ();
      LitLineExtrapolation(par, z);
      if (!vs.GetMaterial().IsEmpty()) {
         fscal thickness = vs.GetMaterial().GetMaterial(par.X, par.Y);
         if (thickness > 0) LitAddMaterial<fscal>(par, thickness);
      }
   } else {
      unsigned char nofSteps = (nofVirtualStations - 1) / 2;
      for (unsigned char iStep = 0; iStep < nofSteps; iStep++) {
         const LitVirtualStationScal& vsFront = station.GetVirtualStation(2 * iStep + 0);
         const LitVirtualStationScal& vsMiddle = station.GetVirtualStation(2 * iStep + 1);
         const LitVirtualStationScal& vsBack = station.GetVirtualStation(2 * iStep + 2);
         if (vsFront.GetField().IsEmpty() || vsMiddle.GetField().IsEmpty() || vsBack.GetField().IsEmpty()) {
            LitLineExtrapolation(par, vsBack.GetZ());
         } else {
            LitRK4Extrapolation(par, vsBack.GetZ(), vsFront.GetField(), vsMiddle.GetField(), vsBack.GetField());
         }

         if (!vsFront.GetMaterial().IsEmpty()) {
            fscal thickness = vsFront.GetMaterial().GetMaterial(par.X, par.Y);
            if (thickness > 0) LitAddMaterial<fscal>(par, thickness);
         }

         if (!vsMiddle.GetMaterial().IsEmpty()) {
            fscal thickness = vsMiddle.GetMaterial().GetMaterial(par.X, par.Y);
            if (thickness > 0) LitAddMaterial<fscal>(par, thickness);
         }

         if (!vsBack.GetMaterial().IsEmpty()) {
            fscal thickness = vsBack.GetMaterial().GetMaterial(par.X, par.Y);
            if (thickness > 0) LitAddMaterial<fscal>(par, thickness);
         }
      }
   }
}

void lit::parallel::LitTrackFinderNN::FollowTracks()
{
   unsigned int nofTracks = fTracks.size();
   for (unsigned int iTrack = 0; iTrack < nofTracks; iTrack++) {
      LitScalTrack* track = fTracks[iTrack];

      unsigned char nofStations = fLayout.GetNofStations();
      for (int iStation = 0; iStation < nofStations; iStation++) {
         LitTrackParamScal par(track->GetParamLast());
         const LitStationScal& station = fLayout.GetStation(iStation);

         fscal zMin = fHitData.GetMinZPos(iStation);
         LitLineExtrapolation(par, zMin);

         const vector<int>& bins = fHitData.GetZPosBins(iStation);
         map<int, LitTrackParamScal> binParamMap;
         vector<int>::const_iterator itBins;
         for (itBins = bins.begin(); itBins != bins.end(); itBins++) {
            binParamMap[*itBins] = LitTrackParamScal();
         }

         // Extrapolate track parameters to each Z position in the map.
         // This is done to improve calculation speed.
         // In case of planar station only 1 track extrapolation is required,
         // since all hits located at the same Z.
         map<int, LitTrackParamScal>::iterator itMap;
         for (itMap = binParamMap.begin(); itMap != binParamMap.end(); itMap++) {
            (*itMap).second = par;
            fscal z = fHitData.GetZPosByBin(iStation, (*itMap).first);
            LitTrackParamScal& tpar = (*itMap).second;
            PropagateToStation(iStation, tpar);
//            LitLineExtrapolation(tpar, z);
//            if (!station.GetVirtualStation().GetMaterial().IsEmpty()) {
//               fscal thickness = station.GetVirtualStation().GetMaterial().GetMaterial(tpar.X, tpar.Y);
//               if (thickness > 0) LitAddMaterial<fscal>(tpar, thickness);
//            }
         }

         // Loop over hits
         fscal minChiSq = numeric_limits<fscal>::max(); // minimum chi-square of hit
         const LitScalPixelHit* minHit = NULL; // Pointer to hit with minimum chi-square
         LitTrackParamScal minPar; // Track parameters for closest hit
         const vector<LitScalPixelHit*>& hits = fHitData.GetHits(iStation);
         unsigned int nofHits = hits.size();
         for (unsigned int iHit = 0; iHit < nofHits; iHit++) {
            const LitScalPixelHit* hit = hits[iHit];
            int bin = fHitData.GetBinByZPos(iStation, hit->Z);
            assert(binParamMap.find(bin) != binParamMap.end());
            LitTrackParamScal tpar(binParamMap[bin]);

            // Check preliminary if hit is in the validation gate.
            // This is done in order to speed up the algorithm.
            // Based on the predicted track position (w/o KF update step)
            // and maximum hit position error.
            fscal maxErrX = fHitData.GetMaxErrX(iStation);
            fscal devX = fSigmaCoef[fIteration] * (sqrt(tpar.C0 + maxErrX * maxErrX));
            fscal maxErrY = fHitData.GetMaxErrY(iStation);
            fscal devY = fSigmaCoef[fIteration] * (sqrt(tpar.C5 + maxErrY * maxErrY));
            bool hitInside = (hit->X < (tpar.X + devX)) && (hit->X > (tpar.X - devX))
                  && (hit->Y < (tpar.Y + devY)) && (hit->Y > (tpar.Y - devY));
            if (!hitInside) continue;

            fscal chi = numeric_limits<fscal>::max();
            LitFiltration(tpar, *hit, chi);
            bool hitInValidationGate = chi < fChiSqStripHitCut[fIteration];
            if (hitInValidationGate && chi < minChiSq) { // Check if hit is inside validation gate and closer to the track.
               minChiSq = chi;
               minHit = hit;
               minPar = tpar;
            }
         }

         if (minHit != NULL) { // Check if hit was added
            if (track->GetNofHits() == 0) track->SetParamFirst(minPar);
            track->AddHit(minHit);
            track->SetParamLast(minPar);
            track->IncChiSq(minChiSq);
            track->SetNDF(NDF(*track));
            track->SetLastStationId(iStation);
         } else { // Missing hit
            track->SetNofMissingHits(track->GetNofMissingHits() + 1);
            if (track->GetNofMissingHits() > fMaxNofMissingHits[fIteration]) { break; }
         }
      }
   }
}

void lit::parallel::LitTrackFinderNN::SelectTracks()
{
   unsigned int nofTracks = fTracks.size();
   for (unsigned int iTrack = 0; iTrack < nofTracks; iTrack++) {
      LitScalTrack* track = fTracks[iTrack];
      track->IsGood(track->GetNofHits() > 0);
   }
   DoSelectSharedHits(fTracks);
}

void lit::parallel::LitTrackFinderNN::RemoveHits()
{
   unsigned int nofTracks = fTracks.size();
   for (unsigned int iTrack = 0; iTrack < nofTracks; iTrack++) {
      LitScalTrack* track = fTracks[iTrack];
      if (!track->IsGood()) { continue; }
      for (unsigned int iHit = 0; iHit < track->GetNofHits(); iHit++) {
         fUsedHitsSet.insert(track->GetHit(iHit)->refId);
      }
   }
}

void lit::parallel::LitTrackFinderNN::CopyToOutput(
      vector<lit::parallel::LitScalTrack*>& tracks)
{
   unsigned int nofTracks = fTracks.size();
   for (unsigned int iTrack = 0; iTrack < nofTracks; iTrack++) {
      LitScalTrack* track = fTracks[iTrack];
      if (!track->IsGood()) { continue; }
      fUsedSeedsSet.insert(track->GetPreviousTrackId());
      tracks.push_back(new LitScalTrack(*track));
   }
}
