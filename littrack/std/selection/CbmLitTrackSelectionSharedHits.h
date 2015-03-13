/* CbmLitTrackSelectionSharedHits.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Track selection removes clones and ghosts
 * sorting by quality and checking shared hits.
 */

#ifndef CBMLITTRACKSELECTIONSHAREDHITS_H_
#define CBMLITTRACKSELECTIONSHAREDHITS_H_

#include "base/CbmLitTypes.h"
#include "interface/CbmLitTrackSelection.h"
#include "selection/CbmLitQualitySort.h"

class CbmLitTrackSelectionSharedHits : public CbmLitTrackSelection,
   public CbmLitQualitySort
{
public:
   /* Constructor */
   CbmLitTrackSelectionSharedHits();

   /* Destructor */
   virtual ~CbmLitTrackSelectionSharedHits();

   /* Derived from CbmLitTrackSelection */
   virtual LitStatus DoSelect(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /* Derived from CbmLitTrackSelection */
   virtual LitStatus DoSelect(
      TrackPtrVector& tracks);

   /* Returns number of shared hits */
   int GetNofSharedHits() const {return fNofSharedHits;}

   /* Sets number of shared hits */
   void SetNofSharedHits(int nofHits) {fNofSharedHits = nofHits;}

private:
   /* Number of shared hits */
   int fNofSharedHits;

   /* Checks shared hits in the track array sorted by quality */
   void CheckSharedHits(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);
};

#endif /*CBMLITTRACKSELECTIONSHAREDHITS_H_*/
