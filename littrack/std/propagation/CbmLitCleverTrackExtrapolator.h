/**
 * \file CbmLitCleverTrackExtrapolator.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief "Clever" track extrapolation.
 *
 * Line track extrapolation in the field free regions or
 * Runge-Kutta of the 4th order in the magnetic field are used.
 * The start Z position for the line track extrapolation is defined by
 * lit::LINE_EXTRAPOLATION_START_Z in the littrack/base/CbmLitDefaultSettings.h.
 **/

#ifndef CBMLITCLEVERTRACKEXTRAPOLATOR_H_
#define CBMLITCLEVERTRACKEXTRAPOLATOR_H_

#include "base/CbmLitPtrTypes.h"
#include "interface/CbmLitTrackExtrapolator.h"

class CbmLitTrackParam;

class CbmLitCleverTrackExtrapolator : public CbmLitTrackExtrapolator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitCleverTrackExtrapolator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitCleverTrackExtrapolator();

   /**
    * \brief Inherited from CbmLitTrackExtrapolator.
    */
   virtual LitStatus Extrapolate(
      const CbmLitTrackParam* parIn,
      CbmLitTrackParam* parOut,
      litfloat zOut,
      std::vector<litfloat>* F);

   /**
    * \brief Inherited from CbmLitTrackExtrapolator.
    */
   virtual LitStatus Extrapolate(
      CbmLitTrackParam* par,
      litfloat zOut,
      std::vector<litfloat>* F);

private:
   TrackExtrapolatorPtr fLineExtrapolator; // Line track extrapolation tool.
   TrackExtrapolatorPtr fRK4Extrapolator; // RK4 track extrapolation tool.
};

#endif /* CBMLITCLEVERTRACKEXTRAPOLATOR_H_ */
