/**
 * \file CbmLitTrackSelectionTrd.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 */

#include "selection/CbmLitTrackSelectionTrd.h"

#include "data/CbmLitTrack.h"
#include "selection/CbmLitTrackSelectionSharedHits.h"

#include <set>
#include <functional>

CbmLitTrackSelectionTrd::CbmLitTrackSelectionTrd():
   fNofSharedHits(0)
{
   fSharedHitsSelection = TrackSelectionPtr(new CbmLitTrackSelectionSharedHits());
}

CbmLitTrackSelectionTrd::~CbmLitTrackSelectionTrd()
{
}

LitStatus CbmLitTrackSelectionTrd::DoSelect(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   if (itBegin == itEnd) { return kLITSUCCESS; }

   ((CbmLitTrackSelectionSharedHits*)fSharedHitsSelection.get())->SetNofSharedHits(fNofSharedHits);

   for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
      (*iTrack)->SetQuality(kLITGOOD);
   }

   fSharedHitsSelection->DoSelect(itBegin, itEnd);

   return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionTrd::DoSelect(
   TrackPtrVector& tracks)
{
   return DoSelect(tracks.begin(), tracks.end());
}
