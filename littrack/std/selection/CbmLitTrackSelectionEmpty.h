/* CbmLitTrackSelectionEmpty.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 *
 * Does nothing, used as a stub.
 */

#ifndef CBMLITTRACKSELECTIONEMPTY_H_
#define CBMLITTRACKSELECTIONEMPTY_H_

#include "interface/CbmLitTrackSelection.h"

class CbmLitTrackSelectionEmpty : public CbmLitTrackSelection
{
public:
   /* Constructor */
   CbmLitTrackSelectionEmpty();

   /* Destructor */
   virtual ~CbmLitTrackSelectionEmpty();

   /* Derived from CbmLitTrackSelection */
   virtual LitStatus DoSelect(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /* Derived from CbmLitTrackSelection */
   virtual LitStatus DoSelect(
      TrackPtrVector& tracks);
};

#endif /*CBMLITTRACKSELECTIONEMPTY_H_*/
