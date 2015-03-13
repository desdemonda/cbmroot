/**
 * \file CbmLitTrackFitter.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track fitter algorithm.
 */

#ifndef CBMLITTRACKFITTER_H_
#define CBMLITTRACKFITTER_H_

#include "base/CbmLitEnums.h"
class CbmLitTrack;

/**
 * \class CbmLitTrackFitter
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track fitter algorithm.
 */
class CbmLitTrackFitter
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackFitter() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackFitter() {}

   /**
    * \brief Main function to be implemented for concrete track fitter algorithm.
    * \param[in,out] track Pointer to track to be fitted.
    * \param[in] downstream Track fit direction.
    * \return Status code.
    */
   virtual LitStatus Fit(
      CbmLitTrack* track,
      bool downstream = true) = 0;
};

#endif
