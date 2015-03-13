/** CbmLitLineTrackExtrapolator.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2007
 **
 ** Class performs line track extrapolation of track parameters.
 **/

#ifndef CBMLITLINETRACKEXTRAPOLATOR_H_
#define CBMLITLINETRACKEXTRAPOLATOR_H_

#include "interface/CbmLitTrackExtrapolator.h"

class CbmLitTrackParam;

class CbmLitLineTrackExtrapolator: public CbmLitTrackExtrapolator
{
public:
   /* Constructor */
   CbmLitLineTrackExtrapolator();

   /* Destructor */
   virtual ~CbmLitLineTrackExtrapolator();

   /* Inherited from CbmLitTrackExtrapolator */
   virtual LitStatus Extrapolate(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      litfloat zOut,
      std::vector<litfloat>* F);

   /* Inherited from CbmLitTrackExtrapolator */
   virtual LitStatus Extrapolate(
      CbmLitTrackParam* par,
      litfloat zOut,
      std::vector<litfloat>* F);
};

#endif //CbmLitLineTrackExtrapolator

