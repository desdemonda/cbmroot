/** CbmLitTGeoNavigator.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 **
 ** The class performs geometry navigation, which is based on the ROOT TGeoManager.
 **/

#ifndef CBMLITTGEONAVIGATOR_H_
#define CBMLITTGEONAVIGATOR_H_

#include "TObject.h"
#include "interface/CbmLitGeoNavigator.h"
#include "propagation/CbmLitMaterialInfo.h"

#include <vector>

class CbmLitTrackParam;

class CbmLitTGeoNavigator : public CbmLitGeoNavigator
{
public:
   /* Constructor */
   CbmLitTGeoNavigator();

   /* Destructor */
   virtual ~CbmLitTGeoNavigator();

   /* Inherited from CbmLitGeoNavigator */
   LitStatus FindIntersections(
      const CbmLitTrackParam* par,
      litfloat zOut,
      std::vector<CbmLitMaterialInfo>& inter);

private:
   /* Sets the current position and direction for the TGeoManager
    * @param par Track parameter from which the current position and direction will be initialized.
    */
   void InitTrack(
      const CbmLitTrackParam* par,
      Bool_t downstream) const;

   /* Make a step: if step=0 than to the next crossing boundary, else
    * make a step of length step.
    * @param step Step size [cm].
    * @return Crossed material information
    */
   CbmLitMaterialInfo MakeStep(
      litfloat step = 0.) const;

   /* Calculates length between current position and the point on the plane at zOut.
    * @param zOut Output Z position [cm]
    * @return Length.
    */
   litfloat CalcLength(
      litfloat zOut) const;
};

#endif /*CBMLITTGEONAVIGATOR_H_*/
