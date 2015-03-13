/**
 * \file CbmLitNearestHitTofMerger.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 **/

#include "finder/CbmLitNearestHitTofMerger.h"

#include "data/CbmLitHit.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitTofTrack.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackUpdate.h"
#include "utils/CbmLitMath.h"

#include <iostream>
#include <map>

using std::map;

CbmLitNearestHitTofMerger::CbmLitNearestHitTofMerger():
      fFieldPropagator(),
      fLinePropagator(),
      fFilter(),
      fPDG(211),
      fChiSqCut(25.)
{
}

CbmLitNearestHitTofMerger::~CbmLitNearestHitTofMerger()
{
}

LitStatus CbmLitNearestHitTofMerger::DoMerge(
   HitPtrVector& hits,
   TrackPtrVector& tracks,
   TofTrackPtrVector& tofTracks)
{
   // First find hit with minimum Z position and build map from Z hit position
   // to track parameter to improve the calculation speed.
   litfloat zMin = std::numeric_limits<litfloat>::max();
   map<litfloat, CbmLitTrackParam> zParamMap;
   for (HitPtrIterator it = hits.begin(); it != hits.end(); it++) {
      const CbmLitHit* hit = *it;
      zMin = std::min(zMin, hit->GetZ());
      zParamMap[hit->GetZ()] = CbmLitTrackParam();
   }

   // Loop over input tracks
   for (TrackPtrIterator it = tracks.begin(); it != tracks.end(); it++) {
         CbmLitTrack* track = *it;
         if (track->GetQuality() != kLITGOODMERGE) { continue; }
         CbmLitTrackParam par(*track->GetParamLast());

         // Extrapolate track minimum Z position of hit using magnetic field propagator.
         if (fFieldPropagator->Propagate(&par, zMin, fPDG) == kLITERROR) {
        	   break; // break if track propagation failed
         }

         // Extrapolate track parameters to each Z position in the map.
         // This is done to improve calculation speed.
         // In case of planar TOF geometry only 1 track extrapolation is required,
         // since all hits located at the same Z.
         for (map<litfloat, CbmLitTrackParam>::iterator it2 = zParamMap.begin(); it2 != zParamMap.end(); it2++) {
            (*it2).second = par;
            fLinePropagator->Propagate(&(*it2).second, (*it2).first, fPDG);
         }

         // Loop over hits
         litfloat minChiSq = std::numeric_limits<litfloat>::max(); // minimum chi-square of hit
         const CbmLitHit* minHit = NULL; // Pointer to hit with minimum chi-square
         CbmLitTrackParam minPar; // Track parameters for closest hit
         for (HitPtrIterator it = hits.begin(); it != hits.end(); it++) {
            const CbmLitHit* hit = *it;
            if (zParamMap.find(hit->GetZ()) == zParamMap.end()) { // This should never happen
               std::cout << "-E- CbmLitNearestHitTofMerger::DoMerge: Z position " << hit->GetZ() << " not found in map. Something is wrong.\n";
            }
            CbmLitTrackParam tpar(zParamMap[hit->GetZ()]);
            litfloat chi = 0.;
            fFilter->Update(&tpar, hit, chi);
            if (chi < fChiSqCut && chi < minChiSq) { // Check if hit is inside validation gate and closer to the track.
               minChiSq = chi;
               minHit = hit;
               minPar = tpar;
            }
         }

         if (minHit != NULL) { // Check if hit was added
            //Create new TOF track
            CbmLitTofTrack* tofTrack = new CbmLitTofTrack();
            tofTrack->SetTrack(track);
            tofTrack->SetHit(minHit);
            tofTrack->SetTrackParam(&minPar);
            tofTrack->SetDistance(minChiSq);
            tofTracks.push_back(tofTrack);
         }
   }
   return kLITSUCCESS;
}
