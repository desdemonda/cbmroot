/*
 * \file CbmLitQualitySort.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#include "selection/CbmLitQualitySort.h"
#include "data/CbmLitTrack.h"

#include <algorithm>

CbmLitQualitySort::CbmLitQualitySort()
{

}

CbmLitQualitySort::~CbmLitQualitySort()
{

}

//LitStatus CbmLitQualitySort::DoSort(
//   TrackPtrIterator itBegin,
//   TrackPtrIterator itEnd)
//{
//   if (itBegin == itEnd) { return kLITSUCCESS; }
//
//   SortNofHits(itBegin, itEnd);
//   //SortLastPlaneId(itBegin, itEnd);
//
//   return kLITSUCCESS;
//}
//
//LitStatus CbmLitQualitySort::DoSort(
//   TrackPtrVector& tracks)
//{
//   return DoSort(tracks.begin(), tracks.end());
//}

LitStatus CbmLitQualitySort::DoSortNofHits(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   std::sort(itBegin, itEnd, CompareTrackPtrNofHitsMore());

   Int_t maxNofHits = (*itBegin)->GetNofHits();
   Int_t minNofHits = (*(itEnd-1))->GetNofHits();

   for (Int_t iNofHits = minNofHits; iNofHits <= maxNofHits; iNofHits++) {
      CbmLitTrack value;
      value.SetNofHits(iNofHits);

      std::pair<TrackPtrIterator, TrackPtrIterator> bounds;
      bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPtrNofHitsMore());

      if(bounds.first == bounds.second) { continue; }

      std::sort(bounds.first, bounds.second, CompareTrackPtrChiSqOverNdfLess());
   }
   return kLITSUCCESS;
}

LitStatus CbmLitQualitySort::DoSortLastStation(
   TrackPtrIterator itBegin,
   TrackPtrIterator itEnd)
{
   std::sort(itBegin, itEnd, CompareTrackPtrLastStationIdMore());

   Int_t maxPlaneId = (*itBegin)->GetLastStationId();
   Int_t minPlaneId = (*(itEnd-1))->GetLastStationId();

   for (Int_t iPlaneId = minPlaneId; iPlaneId <= maxPlaneId; iPlaneId++) {
      CbmLitTrack value;
      value.SetLastStationId(iPlaneId);

      std::pair<TrackPtrIterator, TrackPtrIterator> bounds;
      bounds = std::equal_range(itBegin, itEnd, &value, CompareTrackPtrLastStationIdMore());

      if(bounds.first == bounds.second) { continue; }

      std::sort(bounds.first, bounds.second, CompareTrackPtrChiSqOverNdfLess());
   }
   return kLITSUCCESS;
}

LitStatus CbmLitQualitySort::DoSortChiSqOverNDF(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd)
{
   std::sort(itBegin, itEnd, CompareTrackPtrChiSqOverNdfLess());
   return kLITSUCCESS;
}
