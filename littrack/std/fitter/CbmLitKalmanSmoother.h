/**
 * \file CbmLitKalmanSmoother.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 * \brief Implementation of Kalman smoother algorithm.
 **/

#ifndef CBMLITKALMANSMOOTHER_H_
#define CBMLITKALMANSMOOTHER_H_

#include "interface/CbmLitTrackFitter.h"
class CbmLitFitNode;

/**
 * \class CbmLitKalmanSmoother
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 * \brief Implementation of Kalman smoother algorithm.
 **/
class CbmLitKalmanSmoother : public CbmLitTrackFitter
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitKalmanSmoother();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitKalmanSmoother();

   /**
    * \brief Inherited from CbmLitTrackFitter.
    */
   virtual LitStatus Fit(
      CbmLitTrack* track,
      bool downstream = false);

private:
   /**
    * \brief Smooth one fit node.
    * \param[out] thisNode Current fit node to be smoothed.
    * \param[in] prevNode Previous fit node.
    */
   void Smooth(
      CbmLitFitNode* thisNode,
      const CbmLitFitNode* prevNode);
};

#endif /*CBMLITKALMANSMOOTHER_H_*/
