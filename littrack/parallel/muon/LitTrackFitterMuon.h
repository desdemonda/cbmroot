/** LitTrackFitterMuon.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Functions to fit a track.
 **/

#ifndef LITTRACKFITTERMUON_H_
#define LITTRACKFITTERMUON_H_

#include "LitDetectorGeometryMuon.h"
#include "../LitTypes.h"
#include "../LitTrackParam.h"
#include "../LitExtrapolation.h"
#include "../LitFiltration.h"
#include "../LitAddMaterial.h"

namespace lit {
namespace parallel {

/* Performs SIMDized/scalar fast track fit.
 * Number of hits must be equal to the number of stations.
 *
 */
template<class T>
inline void LitTrackFitterMuon(
   LitTrack<T>& track,
   const LitDetectorLayoutMuon<T>& layout)
{
   //   track.paramLast = track.paramFirst;
   LitTrackParam<T>& par = track.paramLast;
   unsigned char ihit = 0;

   for (unsigned char isg = 0; isg < layout.GetNofStationGroups(); isg++) {
      const LitStationGroupMuon<T>& stationGroup = layout.stationGroups[isg];
      //Propagation through the absorber
      LitFieldRegion<T> field;
      LitFieldValue<T> v1, v2;
      const LitAbsorber<T>& absorber = stationGroup.absorber;
      absorber.fieldSliceFront.GetFieldValue(par.X, par.Y, v1);
      absorber.fieldSliceBack.GetFieldValue(par.X, par.Y, v2);
      field.Set(v1, absorber.fieldSliceFront.Z, v2, absorber.fieldSliceBack.Z);
      LitRK4Extrapolation(par, absorber.Z, field);
      LitAddMaterial(par, absorber.material);

      //Approximate the field between the absorbers
      const LitSubstationMuon<T>& ss1 = stationGroup.stations[0].substations[0];
      const LitSubstationMuon<T>& ss2 = stationGroup.stations[1].substations[0];
      ss1.fieldSlice.GetFieldValue(par.X, par.Y, v1);
      ss2.fieldSlice.GetFieldValue(par.X, par.Y, v2);
      field.Set(v1, ss1.fieldSlice.Z, v2, ss2.fieldSlice.Z);
      for (unsigned char ist = 0; ist < stationGroup.GetNofStations(); ist++) {
         const LitStationMuon<T>& station = stationGroup.stations[ist];
//           const LitSubstationMuon<T>& substation0 = station.substations[0];
//           LitRK4Extrapolation(par, substation0.Z, field);
//       LitAddMaterial(par, substation0.material);
         for (unsigned char iss = 0; iss < station.GetNofSubstations(); iss++) {
            const LitSubstationMuon<T>& substation = station.substations[iss];
            // Propagation through station
            LitRK4Extrapolation(par, substation.Z, field);
//               LitLineExtrapolation(par, substation.Z);
            LitAddMaterial(par, substation.material);
         }
         LitPixelHit<T>* hit = track.hits[ihit];
         LitFiltration(par, *hit);
         ihit++;
      }
   }
}

} // namespace parallel
} // namespace lit
#endif /* LITTRACKFITTERMUON_H_ */
