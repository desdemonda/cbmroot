/**
 * \file CbmLitTrackSelection.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track selection algorithm.
 */

#ifndef CBMLITTRACKSELECTION_H_
#define CBMLITTRACKSELECTION_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitEnums.h"

/**
 * \class CbmLitTrackSelection
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track selection algorithm.
 */
class CbmLitTrackSelection
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackSelection() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackSelection() {}

   /**
    * \brief Main function to be implemented for concrete track selection algorithm.
    * \param[in] itBegin Iterator to first track.
    * \param[in] itEnd Iterator to last track.
    * \return Status code.
    */
   virtual LitStatus DoSelect(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd) = 0;

   /**
    * \brief Main function to be implemented for concrete track selection algorithm.
    * \param tracks Input track array.
    * \return Status code.
    */
   virtual LitStatus DoSelect(
      TrackPtrVector& tracks) = 0;
};

#endif /*CBMLITTRACKSELECTION_H_*/
