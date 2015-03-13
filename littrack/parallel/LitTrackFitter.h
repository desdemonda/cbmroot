/**
 * \file LitTrackFitter.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 * \brief Kalman filter based track fit.
 **/

#ifndef LITTRACKFITTER_H_
#define LITTRACKFITTER_H_

#include "LitDetectorLayout.h"
#include "LitTypes.h"
#include "LitTrackParam.h"
#include "LitExtrapolation.h"
#include "LitFiltration.h"
#include "LitAddMaterial.h"
#include "LitScalPixelHit.h"

namespace lit {
namespace parallel {

/**
 * \brief Fast scalar track fit.
 */
inline void LitTrackFitter(
   LitScalTrack& track,
   const LitDetectorLayoutScal& layout)
{
   LitTrackParamScal par = track.GetParamFirst();

   unsigned char nofVirtualStations = layout.GetNofVirtualStations();
   unsigned char nofSteps = (nofVirtualStations - 1) / 2;
   for (unsigned char iStep = 0; iStep < nofSteps; iStep++) {
      const LitVirtualStationScal& vsFront = layout.GetVirtualStation(2 * iStep + 0);
      const LitVirtualStationScal& vsMiddle = layout.GetVirtualStation(2 * iStep + 1);
      const LitVirtualStationScal& vsBack = layout.GetVirtualStation(2 * iStep + 2);
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

   fscal chiSq = 0.;
   unsigned char prevStationId = -1; // start with -1, i.e. there where no previous station id
   unsigned short nofHits = track.GetNofHits();
   for (unsigned short iHit = 0; iHit < nofHits; iHit++) {
      const LitScalPixelHit* hit = track.GetHit(iHit);

      LitLineExtrapolation(par, hit->Z);

     // lit::parallel::LitFieldGrid field1, field2, field3;
     // LitRK4Extrapolation(par, hit->Z, field1, field2, field3);

      // Add material taking into account missing hits
      for (unsigned char iStation = prevStationId + 1; iStation <= hit->stationId; iStation++) {
         const LitVirtualStationScal& vs = layout.GetStation(iStation).GetVirtualStation(0);
         if (!vs.GetMaterial().IsEmpty()) {
            fscal thickness = vs.GetMaterial().GetMaterial(par.X, par.Y);
            if (thickness > 0) LitAddMaterial<fscal>(par, thickness);
         }
      }
      prevStationId = hit->stationId;

      LitFiltration(par, *hit, chiSq);

      if (iHit == 0) track.SetParamFirst(par);
   }
   track.SetParamLast(par);
}

} // namespace parallel
} // namespace lit
#endif /* LITTRACKFITTER_H_ */
