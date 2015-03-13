/** CbmLitTrackFitterIter.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 ** Class implements iterative Kalman Filter track fit
 ** with detection of outliers by the chi square.
 **/

#ifndef CBMLITTRACKFITTERITER_H_
#define CBMLITTRACKFITTERITER_H_

#include "base/CbmLitPtrTypes.h"
#include "interface/CbmLitTrackFitter.h"

class CbmLitTrack;

class CbmLitTrackFitterIter : public CbmLitTrackFitter
{
public:
   /* Constructor
    *@param propagator Track propagation tool
    *@param update Track update tool */
   CbmLitTrackFitterIter(
      TrackFitterPtr fitter,
      TrackFitterPtr smoother);

   /* Destructor */
   virtual ~CbmLitTrackFitterIter();

   /* inherited from CbmLitTrackFitter */
   virtual LitStatus Fit(
      CbmLitTrack* track,
      bool downstream = true);

private:
   /* Kalman filter track fitting tool */
   TrackFitterPtr fFitter;
   /* Kalman smoother tool */
   TrackFitterPtr fSmoother;
   /* Number of iterations */
   int fNofIterations;
   /* Cut on chi square for single hit */
   litfloat fChiSqCut;
   /* Minimum number of hits in track */
   int fMinNofHits;
};

#endif /*CBMLITTRACKFITTERITER_H_*/
