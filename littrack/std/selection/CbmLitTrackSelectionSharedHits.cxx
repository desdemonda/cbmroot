/* CbmLitTrackSelectionSharedHits.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 */

#include "selection/CbmLitTrackSelectionSharedHits.h"

#include "data/CbmLitTrack.h"
#include "utils/CbmLitComparators.h"

#include <set>
#include <algorithm>

CbmLitTrackSelectionSharedHits::CbmLitTrackSelectionSharedHits():
   fNofSharedHits(0)
{
}

CbmLitTrackSelectionSharedHits::~CbmLitTrackSelectionSharedHits()
{
}

LitStatus CbmLitTrackSelectionSharedHits::DoSelect(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   if (itBegin == itEnd) { return kLITSUCCESS; }

   CbmLitQualitySort::DoSortNofHits(itBegin, itEnd);
   CheckSharedHits(itBegin, itEnd);

   return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionSharedHits::DoSelect(
   TrackPtrVector& tracks)
{
   return DoSelect(tracks.begin(), tracks.end());
}

void CbmLitTrackSelectionSharedHits::CheckSharedHits(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   std::set<int> hitsId;
   const int STRIPSTART = 100000;
   const int TRDSTART = 1000000;

   for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++) {
      CbmLitTrack* track = *iTrack;

      if (track->GetQuality() == kLITBAD) { continue; }

      int nofSharedHits = 0;
      int nofHits = track->GetNofHits();

      for(int iHit = 0; iHit < nofHits; iHit++) {
         int hitId = track->GetHit(iHit)->GetRefId();
         LitHitType type = track->GetHit(iHit)->GetType();
         LitSystemId sysId = track->GetHit(iHit)->GetSystem();
         if (type == kLITSTRIPHIT) { hitId += STRIPSTART; }
         if (sysId == kLITTRD) { hitId += TRDSTART; }
         if(hitsId.find(hitId) != hitsId.end()) {
            nofSharedHits++;
            if (nofSharedHits > fNofSharedHits) {
               track->SetQuality(kLITBAD);
               break;
            }
         }
      }

      if (track->GetQuality() == kLITBAD) { continue; }

      for(int iHit = 0; iHit < nofHits; iHit++) {
         int hitId = track->GetHit(iHit)->GetRefId();
         LitHitType type = track->GetHit(iHit)->GetType();
         LitSystemId detId = track->GetHit(iHit)->GetSystem();
         if (type == kLITSTRIPHIT) { hitId += STRIPSTART; }
         if (detId == kLITTRD) { hitId += TRDSTART; }
         hitsId.insert(hitId);
      }
   }
   hitsId.clear();
}
