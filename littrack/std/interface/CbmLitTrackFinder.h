/**
 * \brief CbmLitTrackFinder.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track finding algorithm.
 */

#ifndef CBMLITTRACKFINDER_H_
#define CBMLITTRACKFINDER_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitEnums.h"

/**
 * \class CbmLitTrackFinder
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Interface for track finding algorithm.
 */
class CbmLitTrackFinder
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackFinder() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackFinder() {}

   /**
    * \brief Main function to be implemented for concrete track finder algorithm.
    * \param[in] hits Input vector of hits.
    * \param[in] trackSeeds Input vector of track seeds.
    * \param[out] tracks Output vector of found tracks.
    * \return Status code.
    */
   virtual LitStatus DoFind(
      HitPtrVector& hits,
      TrackPtrVector& trackSeeds,
      TrackPtrVector& tracks) = 0;
};

#endif /*CBMLITTRACKFINDER_H_*/
