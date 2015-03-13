/** LitTrackFinderNNVecElectron.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **/

#include "LitTrackFinderNNVecElectron.h"

#include "LitDetectorGeometryElectron.h"
#include "LitHitDataElectron.h"
//#include "../LitTrackSelection.h"
#include "../LitTypes.h"
#include "../LitHit.h"
#include "../LitAddMaterial.h"
#include "../LitExtrapolation.h"
#include "../LitFiltration.h"
#include "../LitFieldGrid.h"
#include "../LitMath.h"
#include "../LitVecPack.h"
#include "../LitTrack.h"

#include <algorithm>
#include <iostream>
#include <limits>


lit::parallel::LitTrackFinderNNVecElectron::LitTrackFinderNNVecElectron():
   fMaxNofMissingHits(4),
   fSigmaCoef(5.),
   fMaxCovSq(20.*20.),
   fChiSqPixelHitCut(15.)
{
}

lit::parallel::LitTrackFinderNNVecElectron::~LitTrackFinderNNVecElectron()
{
}

void lit::parallel::LitTrackFinderNNVecElectron::DoFind(
   const PixelHitArray& hits,
   const TrackArray& trackSeeds,
   TrackArray& tracks)
{
   ArrangeHits(hits);
   InitTrackSeeds(trackSeeds);
   FollowTracks();

//   DoTrackSelectionElectron(fTracks.begin(), fTracks.end());

   // Copy tracks to output
   for (unsigned int iTrack = 0; iTrack < fTracks.size(); iTrack++) {
      LitScalTrack* track = fTracks[iTrack];
      if (track->GetNofHits() < 2) { continue; }
      if (!track->IsGood()) { continue; }
      tracks.push_back(new LitScalTrack(*track));
   }

//   for (unsigned int i = 0; i < tracks.size(); i++)
//      std::cout << *tracks[i];

   for_each(fTracks.begin(), fTracks.end(), DeleteObject());
   fTracks.clear();
   fHitData.Clear();
}

void lit::parallel::LitTrackFinderNNVecElectron::ArrangeHits(
      const PixelHitArray& hits)
{
   for(unsigned int iHit = 0; iHit < hits.size(); iHit++) {
      LitScalPixelHit* hit = hits[iHit];
//       if (fUsedHitsSet.find(hit->GetRefId()) != fUsedHitsSet.end()) continue;
      fHitData.AddHit(hit->planeId, hit);
   }
   fHitData.SortHits();
//   std::cout << fHitData;
}

void lit::parallel::LitTrackFinderNNVecElectron::InitTrackSeeds(
      const TrackArray& trackSeeds)
{
   fscal QpCut = 1./0.1;
   for (unsigned int iTrack = 0; iTrack < trackSeeds.size(); iTrack++) {
      LitScalTrack* track = trackSeeds[iTrack];
      if (fabs(track->GetParamLast().Qp) > QpCut) { continue; }
      track->SetPreviousTrackId(iTrack);
      LitScalTrack* newTrack = new LitScalTrack(*track);
      newTrack->SetParamLast(newTrack->GetParamFirst());
      fTracks.push_back(newTrack);
   }
}

void lit::parallel::LitTrackFinderNNVecElectron::FollowTracks()
{
   // Temporary arrays to store track indices from the fTracks array
   std::vector<unsigned int> tracksId1;
   std::vector<unsigned int> tracksId2;

   // Initially use all tracks from fTracks array
   for (unsigned int i = 0; i < fTracks.size(); i++) { tracksId1.push_back(i); }

   //First propagate all tracks to the first station
   unsigned int nofTracks = tracksId1.size(); // number of tracks
   unsigned int nofTracksVec = nofTracks / fvecLen; // number of tracks grouped in vectors
   unsigned int dTracks = nofTracks - fvecLen * nofTracksVec; // number of tracks remained after grouping in vectors

   // loop over fTracks, pack data and propagate to the first station
   for (unsigned int iTrack = 0; iTrack < nofTracksVec; iTrack++) {
      unsigned int start = fvecLen * iTrack;
      // Collect track group
      LitScalTrack* tracks[fvecLen];
      for(unsigned int i = 0; i < fvecLen; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
      PropagateToFirstStation(tracks);
   } // loop over tracks

   // Propagate remaining dTracks to the first station
   if (dTracks > 0) {
      LitScalTrack* tracks[fvecLen];
      std::vector<LitScalTrack> dummyTracks(fvecLen - dTracks);
      unsigned int start = fvecLen * nofTracksVec;
      for(unsigned int i = 0; i < dTracks; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
      // Check if the number of remaining tracks is less than fvecLen.
      if (dTracks < fvecLen) {
         for(unsigned int i = 0; i < fvecLen - dTracks; i++) { tracks[dTracks+i] = &dummyTracks[i]; }//tracks[dTracks-1];
      }
      PropagateToFirstStation(tracks);
   }
   // end propagation to the first station

// std::cout << "Propagation to the first station finished..." << std::endl;


   // Main loop over station groups for track following
   unsigned char nofStationGroups = fLayout.GetNofStationGroups();
   for(unsigned char iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) { // loop over station groups
      const LitStationGroupElectron<fvec>& stg = fLayout.GetStationGroup(iStationGroup);

      // Loop over stations, and propagate tracks from station to station
      unsigned char nofStations = stg.GetNofStations();
      for(unsigned char iStation = 0; iStation < nofStations; iStation++) { // loop over stations

         unsigned int nofTracks = tracksId1.size(); // number of tracks
         unsigned int nofTracksVec = nofTracks / fvecLen; // number of tracks grouped in vectors
         unsigned int dTracks = nofTracks - fvecLen * nofTracksVec; // number of tracks remained after grouping in vectors
//       std::cout << "iStation --> nofTracks=" << nofTracks << " nofTracksVec=" << nofTracksVec
//          << " dTracks=" << dTracks << std::endl;

         for (unsigned int iTrack = 0; iTrack < nofTracksVec; iTrack++) { // loop over tracks
            unsigned int start = fvecLen * iTrack;
            // Collect track group
            LitScalTrack* tracks[fvecLen];
            for(unsigned int i = 0; i < fvecLen; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
            ProcessStation(tracks, iStationGroup, iStation);
         } // loop over tracks

         // Propagate remaining dTracks
         if (dTracks > 0) {
//          std::cout << "Process remaining dTracks=" << dTracks << " " << (int) iStation << ":" << (int) iStationGroup << std::endl;
            LitScalTrack* tracks[fvecLen];
            std::vector<LitScalTrack> dummyTracks(fvecLen - dTracks);
            unsigned int start = fvecLen * nofTracksVec;
            for(unsigned int i = 0; i < dTracks; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
            // Check if the number of remaining tracks is less than fvecLen.
            if (dTracks < fvecLen) {
               for(unsigned int i = 0; i < fvecLen - dTracks; i++) { tracks[dTracks+i] = &dummyTracks[i]; }//tracks[dTracks-1];
            }
            ProcessStation(tracks, iStationGroup, iStation);
         }


         // Check missing hits in each track.
         // Propagate further only tracks which pass the cut.
         for (unsigned int iTrack = 0; iTrack < tracksId1.size(); iTrack++) {
            unsigned int id = tracksId1[iTrack];
            if (fTracks[id]->GetNofMissingHits() <= fMaxNofMissingHits) {
               tracksId2.push_back(id);
            }
         }
         tracksId1.assign(tracksId2.begin(), tracksId2.end());
         tracksId2.clear();
      } // loop over stations
   } // loop over station groups
}

void lit::parallel::LitTrackFinderNNVecElectron::PropagateToFirstStation(
   LitScalTrack* tracks[])
{
   // Pack track parameters
   LitTrackParamScal par[fvecLen];
   for(unsigned int i = 0; i < fvecLen; i++) { par[i] = tracks[i]->GetParamLast(); }
   LitTrackParamVec lpar;
   PackTrackParam(par, lpar);

   for (unsigned char ivp = 0; ivp < fLayout.GetNofVirtualPlanes() - 1; ivp++) {
      const LitVirtualPlaneElectronVec& vp1 = fLayout.GetVirtualPlane(ivp);
      const LitVirtualPlaneElectronVec& vp2 = fLayout.GetVirtualPlane(ivp + 1);

//      lit::parallel::LitFieldValue<fvec> v1, v2, v3;
//      vp1.GetFieldGrid().GetFieldValue(lpar.X, lpar.Y, v1);
//      vp1.GetFieldGridMid().GetFieldValue(lpar.X, lpar.Y, v2);
//      vp2.GetFieldGrid().GetFieldValue(lpar.X, lpar.Y, v3);

      lit::parallel::LitRK4Extrapolation(lpar, vp2.GetZ(),
            vp1.GetFieldGrid(), vp1.GetFieldGridMid(), vp2.GetFieldGrid());
      lit::parallel::LitAddMaterial(lpar, vp2.GetMaterial());
   }

   //lit::parallel::LitLineExtrapolation(lpar, vp2.GetZ());

   // Unpack track parameters
   UnpackTrackParam(lpar, par);
   for(unsigned int i = 0; i < fvecLen; i++) { tracks[i]->SetParamLast(par[i]); }
}

void lit::parallel::LitTrackFinderNNVecElectron::ProcessStation(
   LitScalTrack* tracks[],
   unsigned char stationGroup,
   unsigned char station)
{
// std::cout << "Processing station " << (int) station << ":" << (int)stationGroup << std::endl;
   const LitStationGroupElectron<fvec>& stg = fLayout.GetStationGroup(stationGroup);
   const LitStationElectron<fvec>& sta = stg.GetStation(station);

   // Pack track parameters
   LitTrackParamScal par[fvecLen];
   for(unsigned int i = 0; i < fvecLen; i++) { par[i] = tracks[i]->GetParamLast(); }
   LitTrackParam<fvec> lpar;
   PackTrackParam(par, lpar);

   // Propagate to station
   LitLineExtrapolation(lpar, sta.GetZ());
   for (unsigned char im = 0; im < sta.GetNofMaterialsBefore(); im++) {
      LitAddMaterial(lpar, sta.GetMaterialBefore(im));
   }

   UnpackTrackParam(lpar, par);
   for(unsigned int i = 0; i < fvecLen; i++) {
      CollectHits(&par[i], tracks[i], stationGroup, station);
   }

   for (unsigned char im = 0; im < sta.GetNofMaterialsAfter(); im++) {
      LitAddMaterial(lpar, sta.GetMaterialAfter(im));
   }
}

void lit::parallel::LitTrackFinderNNVecElectron::CollectHits(
   LitTrackParamScal* par,
   LitScalTrack* track,
   unsigned char stationGroup,
   unsigned char station)
{
   PixelHitConstIteratorPair hits;
   track->SetParamLast(*par);

   const std::vector<LitScalPixelHit*>& hitvec = fHitData.GetHits(stationGroup, station);
   fscal err = fHitData.GetMaxErr(stationGroup, station);

   MinMaxIndex(par, hitvec, err, hits.first, hits.second);
   unsigned int nofHits = std::distance(hits.first, hits.second);

   bool hitAdded = AddNearestHit(track, hits, nofHits, stationGroup, station);

   // Check if hit was added, if not than increase number of missing hits
   if (!hitAdded) { track->IncNofMissingHits(); }
}

bool lit::parallel::LitTrackFinderNNVecElectron::AddNearestHit(
   LitScalTrack* track,
   const PixelHitConstIteratorPair& hits,
   unsigned int nofHits,
   int stationGroup,
   int station)
{
   bool hitAdded = false;
   LitScalPixelHit* hita = NULL;
   LitTrackParamScal param;
   fscal chiSq = std::numeric_limits<fscal>::max();

   // Pack track parameters
   LitTrackParamScal pars[fvecLen];
   for(unsigned int i = 0; i < fvecLen; i++) { pars[i] = track->GetParamLast(); }
   LitTrackParam<fvec> lpar;
   PackTrackParam(pars, lpar);

   const std::vector<LitScalPixelHit*>& hitvec = fHitData.GetHits(stationGroup, station);
   unsigned int nofHitsVec = nofHits / fvecLen; // number of hits grouped in vectors
   unsigned int dHits = nofHits - fvecLen * nofHitsVec; // number of hits remained after grouping in vectors
   for (unsigned int iHit = 0; iHit < nofHitsVec; iHit++) {
      unsigned int start = std::distance(hitvec.begin(), hits.first) + fvecLen * iHit;

      // Pack hit
      LitScalPixelHit hit[fvecLen];
      for(unsigned int i = 0; i < fvecLen; i++) { hit[i] = *hitvec[start + i]; }
      LitPixelHit<fvec> lhit;
      PackPixelHit(hit, lhit);

      LitTrackParam<fvec> ulpar = lpar;

      //First update track parameters with KF, than check whether the hit is in the validation gate.
      fvec chisq = 0;
      LitFiltration(ulpar, lhit, chisq);

      // Unpack track parameters
      UnpackTrackParam(ulpar, pars);
      for (unsigned int i = 0; i < fvecLen; i++) {
         if (chisq[i] < fChiSqPixelHitCut[i] && chisq[i] < chiSq) {
            chiSq = chisq[i];
            hita = hitvec[start + i];//hit[start + i];
            param = pars[i];
         }
      }
   }
   if (dHits > 0) {
      unsigned int start = std::distance(hitvec.begin(), hits.first) + fvecLen * nofHitsVec;
      LitScalPixelHit hit[fvecLen];
      LitPixelHit<fvec> lhit;
      LitTrackParamScal pars[fvecLen];
      LitTrackParam<fvec> lpar;
      for(unsigned int i = 0; i < dHits; i++) {
         hit[i] = *hitvec[start + i];
         pars[i] = track->GetParamLast();
      }
      // Check if the number of remaining tracks is less than fvecLen.
      if (dHits < fvecLen) {
         for(unsigned int i = 0; i < fvecLen - dHits; i++) {
            hit[dHits+i] = *hitvec[start + dHits - 1];
            pars[dHits+i] = track->GetParamLast();
         }
      }
      PackPixelHit(hit, lhit);
      PackTrackParam(pars, lpar);

      //First update track parameters with KF, than check whether the hit is in the validation gate.
      fvec chisq = 0;
      LitFiltration(lpar, lhit, chisq);

      // Unpack track parameters
      UnpackTrackParam(lpar, pars);
      for (unsigned int i = 0; i < fvecLen; i++) {
         if ( (chisq[i] < fChiSqPixelHitCut[i]) && (chisq[i] < chiSq) ) {
            chiSq = chisq[i];
            hita = hitvec[start + i];
            param = pars[i];
         }
      }
   }

   // if hit was attached than change track information
   if (hita != NULL) {
      track->AddHit(hita);
      track->SetParamLast(param);
      track->IncChiSq(chiSq);
      track->SetNDF(NDF(*track));
      hitAdded = true;
   }
   return hitAdded;
}

void lit::parallel::LitTrackFinderNNVecElectron::MinMaxIndex(
   const LitTrackParamScal* par,
   const PixelHitArray& hits,
   fscal maxErr,
   PixelHitConstIterator& first,
   PixelHitConstIterator& last)
{
   first = hits.begin();
   last = hits.begin();
   LitScalPixelHit hit;
   fscal C0 = par->C0;
   if(C0 > fMaxCovSq || C0 < 0.) { return; }
   fscal devX = fSigmaCoef * (std::sqrt(C0) + maxErr);
   hit.X = par->X - devX;
   first = std::lower_bound(hits.begin(), hits.end(), &hit, ComparePixelHitXLess());
   hit.X = par->X + devX;
   last =  std::lower_bound(hits.begin(), hits.end(), &hit, ComparePixelHitXLess());
}
