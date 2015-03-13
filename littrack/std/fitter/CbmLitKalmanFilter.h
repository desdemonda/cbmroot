/** CbmLitKalmanFilter.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 ** Class implements Kalman Filter update step.
 **/

#ifndef CBMLITKALMANFILTER_H_
#define CBMLITKALMANFILTER_H_

//#include "base/CbmLitTypes.h"
#include "interface/CbmLitTrackUpdate.h"

class CbmLitHit;
class CbmLitStripHit;
class CbmLitPixelHit;
class CbmLitTrackParam;

class CbmLitKalmanFilter: public CbmLitTrackUpdate
{
public:
   /* Constructor */
   CbmLitKalmanFilter();

   /* Destructor */
   virtual ~CbmLitKalmanFilter();

   /* Inherited from CbmLitTrackUpdate */
   virtual LitStatus Update(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      const CbmLitHit* hit,
      litfloat& chiSq);

   /* Inherited from CbmLitTrackUpdate */
   virtual LitStatus Update(
      CbmLitTrackParam* par,
      const CbmLitHit* hit,
      litfloat& chiSq);

private:
   /* Implements KF update step for pixel hits using gain matrix formalism
    *@param par Input/Output track parameter
    *@param hit Pointer to the hit
    *@param chiSq Output calculated chi square value
    *@return Status code */
   LitStatus Update(
      CbmLitTrackParam* par,
      const CbmLitPixelHit* hit,
      litfloat& chiSq);

   /* Implements KF update step for pixel hits using weighted means formalism
    *@param par Input/Output track parameter
    *@param hit Pointer to the hit
    *@param chiSq Output calculated chi square value
    *@return Status code */
   LitStatus UpdateWMF(
      CbmLitTrackParam* par,
      const CbmLitPixelHit* hit,
      litfloat& chiSq);

   /* Implements KF update step for strip hits using gain matrix formalism
    *@param par Input/Output track parameter
    *@param hit Pointer to the hit
    *@param chiSq Output calculated chi square value
    *@return Status code */
   LitStatus Update(
      CbmLitTrackParam* par,
      const CbmLitStripHit* hit,
      litfloat& chiSq);

   /* Implements KF update step for strip hits using weighted means formalism
    *@param par Input/Output track parameter
    *@param hit Pointer to the hit
    *@param chiSq Output calculated chi square value
    *@return Status code */
   LitStatus UpdateWMF(
      CbmLitTrackParam* par,
      const CbmLitStripHit* hit,
      litfloat& chiSq);
};

#endif //CbmLitKalmanFilter

