/**
 * \file LitTrackFinderNNVecMuon.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "LitTrackFinderNNVecMuon.h"
#include "LitDetectorLayoutMuon.h"
#include "LitHitDataMuon.h"
#include "../LitTypes.h"
#include "../LitHit.h"
#include "../LitAddMaterial.h"
#include "../LitExtrapolation.h"
#include "../LitFiltration.h"
#include "../LitFieldGrid.h"
#include "../LitMath.h"
#include "../LitVecPack.h"
#include "../LitTrackSelection.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <limits>

lit::parallel::LitTrackFinderNNVecMuon::LitTrackFinderNNVecMuon():
	fTracks(),
	fLayout(),
	fHitData(),
	fMaxNofMissingHits(3),
	fIsProcessSubstationsTogether(true),
	fSigmaCoef(3.5),
	fMaxCovSq(20.*20.),
	fChiSqPixelHitCut(25)
{

}

lit::parallel::LitTrackFinderNNVecMuon::~LitTrackFinderNNVecMuon()
{

}

void lit::parallel::LitTrackFinderNNVecMuon::DoFind(
      const PixelHitArray& hits,
      const TrackArray& trackSeeds,
      TrackArray& tracks)
{
   ArrangeHits(hits);
   InitTrackSeeds(trackSeeds);
   FollowTracks();

   DoTrackSelectionMuon(fTracks.begin(), fTracks.end());

//   std::cout << "NOF TRACKS = " << fTracks.size() << std::endl;
   // Copy tracks to output
   for (unsigned int iTrack = 0; iTrack < fTracks.size(); iTrack++) {
      LitScalTrack* track = fTracks[iTrack];
     // std::cout << "BEFORE " << *track;
      if (!track->IsGood()) { continue; }
//      std::cout << "AFTER " << *track;
      tracks.push_back(new LitScalTrack(*track));
   }

//   for (unsigned int i = 0; i < tracks.size(); i++)
//     std::cout << *tracks[i];

   for_each(fTracks.begin(), fTracks.end(), DeleteObject());
   fTracks.clear();
   fHitData.Clear();
}

void lit::parallel::LitTrackFinderNNVecMuon::ArrangeHits(
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

void lit::parallel::LitTrackFinderNNVecMuon::InitTrackSeeds(
      const TrackArray& trackSeeds)
{
   fscal QpCut = 1./1.5;
   for (unsigned int iTrack = 0; iTrack < trackSeeds.size(); iTrack++) {
      LitScalTrack* track = trackSeeds[iTrack];
      if (fabs(track->GetParamLast().Qp) > QpCut) { continue; }
      track->SetPreviousTrackId(iTrack);
      LitScalTrack* newTrack = new LitScalTrack(*track);
      newTrack->SetParamLast(newTrack->GetParamFirst());
      fTracks.push_back(newTrack);
   }
}

void lit::parallel::LitTrackFinderNNVecMuon::FollowTracks()
{
   // Temporary arrays to store track indices from the fTracks array
   std::vector<unsigned int> tracksId1;
   std::vector<unsigned int> tracksId2;

   // Initially use all tracks from fTracks array
   for (unsigned int i = 0; i < fTracks.size(); i++) { tracksId1.push_back(i); }

   // Main loop over station groups for track following
   unsigned char nofStationGroups = fLayout.GetNofStationGroups();
   for(unsigned char iStationGroup = 0; iStationGroup < nofStationGroups; iStationGroup++) { // loop over station groups
      const LitStationGroupMuon<fvec>& stg = fLayout.GetStationGroup(iStationGroup);

      // First propagate all tracks through absorber
      PropagateThroughAbsorber(tracksId1, stg.GetAbsorber());

      // Loop over stations, and propagate tracks from station to station
      unsigned char nofStations = stg.GetNofStations();
      for(unsigned char iStation = 0; iStation < nofStations; iStation++) { // loop over stations

         // Process station for each track
         ProcessStation(tracksId1, iStationGroup, iStation);

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

void lit::parallel::LitTrackFinderNNVecMuon::PropagateThroughAbsorber(
      const std::vector<unsigned int>& tracksId1,
      const LitAbsorber<fvec>& absorber)
{
   unsigned int nofTracks = tracksId1.size(); // number of tracks
   unsigned int nofTracksVec = nofTracks / fvecLen; // number of tracks grouped in vectors
   unsigned int dTracks = nofTracks - fvecLen * nofTracksVec; // number of tracks remained after grouping in vectors

   // loop over fTracks, pack data and propagate through the absorber
   for (unsigned int iTrack = 0; iTrack < nofTracksVec; iTrack++) {
      unsigned int start = fvecLen * iTrack;
      // Collect track group
      std::vector<LitScalTrack*> tracks(fvecLen);
      for(unsigned int i = 0; i < fvecLen; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
      PropagateThroughAbsorber(tracks, absorber);
   } // loop over tracks

   // Propagate remaining dTracks through the absorber
   if (dTracks > 0) {
      std::vector<LitScalTrack*> tracks(fvecLen);
      std::vector<LitScalTrack> dummyTracks(fvecLen - dTracks);
      unsigned int start = fvecLen * nofTracksVec;
      for(unsigned int i = 0; i < dTracks; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
      // Check if the number of remaining tracks is less than fvecLen.
      if (dTracks < fvecLen) {
         for(unsigned int i = 0; i < fvecLen - dTracks; i++) { tracks[dTracks+i] = &dummyTracks[i]; }//tracks[dTracks-1];
      }
      PropagateThroughAbsorber(tracks, absorber);
   }
}

void lit::parallel::LitTrackFinderNNVecMuon::PropagateThroughAbsorber(
      const TrackArray& tracks,
      const LitAbsorber<fvec>& absorber)
{
   // Pack track parameters
   LitTrackParamScal par[fvecLen];
   for(unsigned int i = 0; i < fvecLen; i++) { par[i] = tracks[i]->GetParamLast(); }
   LitTrackParam<fvec> lpar;
   PackTrackParam(par, lpar);

   // Propagate through the absorber
//   LitFieldValue<fvec> v1, v2, v3;
//   absorber.GetFieldSliceFront().GetFieldValue(lpar.X, lpar.Y, v1);
//   absorber.GetFieldSliceMiddle().GetFieldValue(lpar.X, lpar.Y, v2);
//   absorber.GetFieldSliceBack().GetFieldValue(lpar.X, lpar.Y, v3);
   LitRK4Extrapolation(lpar, absorber.GetZ(),
		   absorber.GetFieldGridFront(), absorber.GetFieldGridMiddle(), absorber.GetFieldGridBack());
   LitAddMaterial(lpar, absorber.GetMaterial());

  // std::cout << "absorber:" << absorber.GetZ() << " " << lpar;

   // Unpack track parameters
   UnpackTrackParam(lpar, par);
   for(unsigned int i = 0; i < fvecLen; i++) { tracks[i]->SetParamLast(par[i]); }
}

void lit::parallel::LitTrackFinderNNVecMuon::ProcessStation(
      const std::vector<unsigned int>& tracksId1,
      unsigned char stationGroup,
      unsigned char station)
{
   unsigned int nofTracks = tracksId1.size(); // number of tracks
   unsigned int nofTracksVec = nofTracks / fvecLen; // number of tracks grouped in vectors
   unsigned int dTracks = nofTracks - fvecLen * nofTracksVec; // number of tracks remained after grouping in vectors

   for (unsigned int iTrack = 0; iTrack < nofTracksVec; iTrack++) { // loop over tracks
      unsigned int start = fvecLen * iTrack;
      // Collect track group
      std::vector<LitScalTrack*> tracks(fvecLen);
      for(unsigned int i = 0; i < fvecLen; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
      ProcessStation(tracks, stationGroup, station);
   } // loop over tracks

   // Propagate remaining dTracks
   if (dTracks > 0) {
      std::vector<LitScalTrack*> tracks(fvecLen);
      std::vector<LitScalTrack> dummyTracks(fvecLen - dTracks);
      unsigned int start = fvecLen * nofTracksVec;
      for(unsigned int i = 0; i < dTracks; i++) { tracks[i] = fTracks[tracksId1[start + i]]; }
      // Check if the number of remaining tracks is less than fvecLen.
      if (dTracks < fvecLen) {
         for(unsigned int i = 0; i < fvecLen - dTracks; i++) { tracks[dTracks+i] = &dummyTracks[i]; }//tracks[dTracks-1];
      }
      ProcessStation(tracks, stationGroup, station);
   }
}

void lit::parallel::LitTrackFinderNNVecMuon::ProcessStation(
      const TrackArray& tracks,
      unsigned char stationGroup,
      unsigned char station)
{
   const LitStationGroupMuon<fvec>& stg = fLayout.GetStationGroup(stationGroup);
   const LitStationMuon<fvec>& sta = stg.GetStation(station);
   unsigned char nofSubstations = sta.GetNofSubstations();

   // Pack track parameters
   std::vector< std::vector<LitTrackParamScal> > par(nofSubstations, std::vector<LitTrackParamScal>(fvecLen));
   for(unsigned int i = 0; i < fvecLen; i++) { par[0][i] = tracks[i]->GetParamLast(); }
   std::vector<LitTrackParam<fvec> > lpar(nofSubstations);
   PackTrackParam(&par[0][0], lpar[0]);

   //Approximate the field between the absorbers
   // TODO: Do not need to recalculate it for each station??
   LitFieldRegion<fvec> field;
   stg.GetFieldRegion(lpar[0].X, lpar[0].Y, field);

   // Propagate to each of the substations
   for (unsigned char iSubstation = 0; iSubstation < nofSubstations; iSubstation++) { // loop over substations
      const LitSubstationMuon<fvec>& substation = sta.GetSubstation(iSubstation);
      // Propagation through station
//      LitRK4Extrapolation(lpar[iSubstation], substation.GetZ(), field);
      LitLineExtrapolation(lpar[iSubstation], substation.GetZ());
      LitAddMaterial(lpar[iSubstation], substation.GetMaterial());
      if (iSubstation < nofSubstations - 1) { lpar[iSubstation + 1] = lpar[iSubstation]; }
   } // loop over substations

   for (unsigned char iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
      UnpackTrackParam(lpar[iSubstation], &par[iSubstation][0]);
   }
   for(unsigned int i = 0; i < fvecLen; i++) {
      std::vector<LitTrackParamScal> spar(nofSubstations);
      for (unsigned char iSubstation = 0; iSubstation < nofSubstations; iSubstation++) {
         spar[iSubstation] = par[iSubstation][i];
      }

      CollectHits(spar, tracks[i], stationGroup, station, nofSubstations);
   }
}

void lit::parallel::LitTrackFinderNNVecMuon::CollectHits(
   std::vector<LitTrackParamScal>& par,
   LitScalTrack* track,
   unsigned char stationGroup,
   unsigned char station,
   unsigned char nofSubstations)
{
   std::vector<PixelHitConstIteratorPair> hits(nofSubstations);
   unsigned int nofHits = 0;

   // TODO implement multithreading for this loop
   for (unsigned char iSubstation = 0; iSubstation < nofSubstations; iSubstation++) { // loop over substations
      track->SetParamLast(par[iSubstation]);

      const PixelHitArray& hitvec = fHitData.GetHits(stationGroup, station, iSubstation);
      fscal err = fHitData.GetMaxErr(stationGroup, station, iSubstation);

      MinMaxIndex(&par[iSubstation], hitvec, err, hits[iSubstation].first, hits[iSubstation].second);
      nofHits += std::distance(hits[iSubstation].first, hits[iSubstation].second);
   } // loop over substations


   bool hitAdded = false;
   PixelHitArray ahits(nofHits);
   std::vector<LitTrackParamScal*> apars(nofHits);
   unsigned int cnt = 0;
   for(unsigned char iss = 0; iss < nofSubstations; iss++) {
      for (PixelHitConstIterator j = hits[iss].first; j != hits[iss].second; j++) {
         ahits[cnt] = *j;
         apars[cnt] = &par[iss];
         cnt++;
      }
   }

   if (AddNearestHit(track, ahits, apars, nofHits)) { hitAdded = true; }
   // Check if hit was added, if not than increase number of missing hits
   if (!hitAdded) { track->IncNofMissingHits(); }
}

bool lit::parallel::LitTrackFinderNNVecMuon::AddNearestHit(
      LitScalTrack* track,
      const PixelHitArray& hits,
      const std::vector<LitTrackParamScal*>& pars,
      unsigned int nofHits)
{
   bool hitAdded = false;
   LitScalPixelHit* hita = NULL;
   LitTrackParamScal param;
   fscal chiSq = std::numeric_limits<fscal>::max();

   unsigned int nofHitsVec = nofHits / fvecLen; // number of hits grouped in vectors
   unsigned int dHits = nofHits - fvecLen * nofHitsVec; // number of hits remained after grouping in vectors
   for (unsigned int iHit = 0; iHit < nofHitsVec; iHit++) {
      unsigned int start = fvecLen * iHit;

      // Pack hit
      LitScalPixelHit hit[fvecLen];
      for(unsigned int i = 0; i < fvecLen; i++) { hit[i] = *hits[start + i]; }
      LitPixelHit<fvec> lhit;
      PackPixelHit(hit, lhit);
      // Pack track parameters
      LitTrackParamScal par[fvecLen];
      for(unsigned int i = 0; i < fvecLen; i++) { par[i] = *pars[start + i]; }
      LitTrackParam<fvec> lpar;
      PackTrackParam(par, lpar);

      //First update track parameters with KF, than check whether the hit is in the validation gate.
      fvec chisq = 0;
      LitFiltration(lpar, lhit, chisq);

      // Unpack track parameters
      UnpackTrackParam(lpar, par);
      for (unsigned int i = 0; i < fvecLen; i++) {
         if (chisq[i] < fChiSqPixelHitCut[i] && chisq[i] < chiSq) {
            chiSq = chisq[i];
            hita = hits[start + i];
            param = par[i];
         }
      }
   }
   if (dHits > 0) {
      unsigned int start = fvecLen * nofHitsVec;
      LitScalPixelHit hit[fvecLen];
      LitPixelHit<fvec> lhit;
      LitTrackParamScal par[fvecLen];
      LitTrackParam<fvec> lpar;
      for(unsigned int i = 0; i < dHits; i++) {
         hit[i] = *hits[start + i];
         par[i] = *pars[start + i];
      }
      // Check if the number of remaining tracks is less than fvecLen.
      if (dHits < fvecLen) {
         for(unsigned int i = 0; i < fvecLen - dHits; i++) {
            hit[dHits+i] = *hits[nofHits-1];
            par[dHits+i] = *pars[nofHits-1];
         }
      }
      PackPixelHit(hit, lhit);
      PackTrackParam(par, lpar);

      //First update track parameters with KF, than check whether the hit is in the validation gate.
      fvec chisq = 0;
      LitFiltration(lpar, lhit, chisq);

      // Unpack track parameters
      UnpackTrackParam(lpar, par);
      for (unsigned int i = 0; i < fvecLen; i++) {
         if (chisq[i] < fChiSqPixelHitCut[i] && chisq[i] < chiSq) {
            chiSq = chisq[i];
            hita = hits[start + i];
            param = par[i];
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

void lit::parallel::LitTrackFinderNNVecMuon::MinMaxIndex(
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
