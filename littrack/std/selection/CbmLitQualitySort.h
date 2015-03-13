/*
 * \file CbmLitQualitySort.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 * \brief Sort array of tracks by the quality criterion.
 */

#ifndef CBMLITQUALITYSORT_H_
#define CBMLITQUALITYSORT_H_

#include "base/CbmLitEnums.h"
#include "base/CbmLitTypes.h"

class CbmLitQualitySort
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitQualitySort();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQualitySort();
//
//   /**
//    * \brief Sort array of tracks by quality.
//    * \param[in] itBegin Iterator to first track in array.
//    * \paran[in] itEnd Iterator to last track in array.
//    */
//   virtual LitStatus DoSort(
//      TrackPtrIterator itBegin,
//      TrackPtrIterator itEnd);
//
//   /**
//    * \brief Sort array of tracks by quality.
//    * \param[in,out] tracks Array of tracks.
//    */
//   virtual LitStatus DoSort(
//      TrackPtrVector& tracks);
//
//private:

   /**
    * \brief Sort array of tracks by quality using number of hits and chi square.
    */
   static LitStatus DoSortNofHits(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /**
    * \brief Sort array of tracks by quality using last station id and chi square.
    */
   static LitStatus DoSortLastStation(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /**
    * \brief Sort array of tracks by quality using (chi square / NDF).
    */
   static LitStatus DoSortChiSqOverNDF(
         TrackPtrIterator itBegin,
         TrackPtrIterator itEnd);
};

#endif /* CBMLITQUALITYSORT_H_ */
