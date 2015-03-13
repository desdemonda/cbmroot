/**
 * \file CbmLitHitToTrackMerger.h
 * \author A.Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for hit-to-track merging algorithm.
 */

#ifndef CBMLITHITTOTRACKMERGER_H_
#define CBMLITHITTOTRACKMERGER_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitEnums.h"

/**
 * \class CbmLitHitToTrackMerger
 * \author A.Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for hit-to-track merging algorithm.
 */
class CbmLitHitToTrackMerger
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitHitToTrackMerger() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitHitToTrackMerger() {}

   /**
    * \brief Main function to be implemented for concrete hit-to-track merging algorithm.
    * \param hits Vector of hits that have to be merged with tracks.
    * \param tracks Vector of tracks that have to be merged with hits.
    * \param tracks Output vector of TOF tracks.
    * \return Status code.
    */
   virtual LitStatus DoMerge(
      HitPtrVector& hits,
      TrackPtrVector& tracks,
      TofTrackPtrVector& tofTracks) = 0;
};

#endif /* CBMLITHITTOTRACKMERGER_H_ */
