/** LitCudaTrackFitter.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/

#ifndef LITCUDATRACKFITTER_H_
#define LITCUDATRACKFITTER_H_

//#include "parallel/LitTypes.h"
#include "LitCudaTrack.h"
#include "LitCudaTrackParam.h"
#include "LitCudaDetectorGeometry.h"
#include "LitCudaTracking.cu"
#include "LitCudaField.cu"

__device__ void LitCudaTrackFitter(
   LitCudaTrack* track,
   const LitCudaDetectorLayout* layout)
{
   //   track.paramLast = track.paramFirst;
   LitCudaTrackParam* par = &track->paramLast;
   unsigned char ihit = 0;

   for (unsigned char isg = 0; isg < layout->nofStationGroups; isg++) {
      const LitCudaStationGroup* stationGroup = &layout->stationGroups[isg];
      //Propagation through the absorber
      LitCudaFieldRegion field;
      LitCudaFieldValue v1, v2;
      const LitCudaAbsorber* absorber = &stationGroup->absorber;
      GetFieldValue(par->X, par->Y, &absorber->fieldSliceFront, &v1);
      GetFieldValue(par->X, par->Y, &absorber->fieldSliceBack, &v2);
      Set(&v1, absorber->fieldSliceFront.Z, &v2, absorber->fieldSliceBack.Z, &field);
      LitCudaRK4Extrapolation(par, absorber->Z, &field);
      LitCudaAddMaterial(par, &absorber->material);

      //Approximate the field between the absorbers
      const LitCudaSubstation* ss1 = &stationGroup->stations[0].substations[0];
      const LitCudaSubstation* ss2 = &stationGroup->stations[1].substations[0];
      GetFieldValue(par->X, par->Y, &ss1->fieldSlice, &v1);
      GetFieldValue(par->X, par->Y, &ss2->fieldSlice, &v2);
      Set(&v1, ss1->fieldSlice.Z, &v2, ss2->fieldSlice.Z, &field);
      for (unsigned char ist = 0; ist < stationGroup->nofStations; ist++) {
         const LitCudaStation* station = &stationGroup->stations[ist];
//           const LitCudaSubstation& substation0 = station.substations[0];
//           LitRK4Extrapolation(par, substation0.Z, field);
//       LitAddMaterial(par, substation0.material);
         for (unsigned char iss = 0; iss < station->nofSubstations; iss++) {
            const LitCudaSubstation* substation = &station->substations[iss];
            // Propagation through station
            LitCudaRK4Extrapolation(par, substation->Z, &field);
//               LitLineExtrapolation(par, substation.Z);
            LitCudaAddMaterial(par, &substation->material);
         }
         LitCudaPixelHit* hit = track->hits[ihit];
         LitCudaFiltration(par, hit);
         ihit++;
      }
   }
}

#endif /* LITCUDATRACKFITTER_H_ */
