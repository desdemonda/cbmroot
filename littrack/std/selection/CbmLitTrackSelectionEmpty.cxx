/* CbmLitTrackSelectionEmpty.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 */

#include "selection/CbmLitTrackSelectionEmpty.h"

CbmLitTrackSelectionEmpty::CbmLitTrackSelectionEmpty()
{

}

CbmLitTrackSelectionEmpty::~CbmLitTrackSelectionEmpty()
{

}

LitStatus CbmLitTrackSelectionEmpty::DoSelect(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionEmpty::DoSelect(
   TrackPtrVector& tracks)
{
   return DoSelect(tracks.begin(), tracks.end());
}
