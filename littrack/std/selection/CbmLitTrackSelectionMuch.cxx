/**
 * \file CbmLitTrackSelectionMuch.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 */

#include "selection/CbmLitTrackSelectionMuch.h"

#include "data/CbmLitTrack.h"
#include "selection/CbmLitTrackSelectionSharedHits.h"

CbmLitTrackSelectionMuch::CbmLitTrackSelectionMuch():
   fNofSharedHits(0)
{
   fSharedHitsSelection = TrackSelectionPtr(new CbmLitTrackSelectionSharedHits());
}

CbmLitTrackSelectionMuch::~CbmLitTrackSelectionMuch()
{
}

LitStatus CbmLitTrackSelectionMuch::DoSelect(
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

LitStatus CbmLitTrackSelectionMuch::DoSelect(
   TrackPtrVector& tracks)
{
   return DoSelect(tracks.begin(), tracks.end());
}
