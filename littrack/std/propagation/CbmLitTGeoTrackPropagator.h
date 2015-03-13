/** CbmLitTGeoTrackPropagator.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 1.0
 **
 ** The class performs track parameters propagation. The track propagation
 ** uses TGeoManager for the navigation.
 **/

#ifndef CBMLITTGEOTRACKPROPAGATOR_H_
#define CBMLITTGEOTRACKPROPAGATOR_H_

#include "base/CbmLitPtrTypes.h"
#include "interface/CbmLitTrackPropagator.h"

#include <vector>

class CbmLitTGeoTrackPropagator: public CbmLitTrackPropagator
{
public:
   /* Standard constructor with the pointer to track extrapolation tool.
    * @param extrapolator Pointer to the LIT track extrapolator class to be used during the propagation.
    */
   CbmLitTGeoTrackPropagator(
      TrackExtrapolatorPtr extrapolator);

   /* Destructor */
   virtual ~CbmLitTGeoTrackPropagator();

   /* Inherited from CbmLitTrackPropagator */
   virtual LitStatus Propagate(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      litfloat zOut,
      int pdg,
      std::vector<litfloat>* F,
      litfloat* length);

   /* Inherited from CbmLitTrackPropagator */
   virtual LitStatus Propagate(
      CbmLitTrackParam* par,
      litfloat zOut,
      int pdg,
      std::vector<litfloat>* F,
      litfloat* length);

	static litfloat MAXIMUM_PROPAGATION_STEP_SIZE; // maximum step size in the TGeo track propagation

protected:
   /* Updates the transport matrix F = newF * F.
     * @param F Input/output Current transport matrix.
     * @param newF Transport matrix to be included in the current transport matrix.
     */
   void UpdateF(
      std::vector<litfloat>& F,
      const std::vector<litfloat>& newF);

   /* Checks if the track parameters are correct by checking their maximum acceptable values.
     * @param par Track parameters to be checked.
     */
   bool IsParCorrect(
      const CbmLitTrackParam* par);

private:
   TrackExtrapolatorPtr fExtrapolator; // Track extrapolator tool
   GeoNavigatorPtr fNavigator; // Geometry navigator tool
   MaterialEffectsPtr fMaterial; // Material effects tool
};

#endif //CBMLITTGEOTRACKPROPAGATOR_H_
