/**
 * \file LitTrackFinderNNVecElectron.h
 *
 * \brief Parallel SIMDized implementation of TRD tracking.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 *
 **/

#ifndef LITTRACKFINDERNNVECELECTRON_H_
#define LITTRACKFINDERNNVECELECTRON_H_

#include "LitDetectorGeometryElectron.h"
#include "LitHitDataElectron.h"
#include "../LitHit.h"
#include "../LitTrack.h"

namespace lit {
namespace parallel {

/**
 * \class LitTrackFinderNNVecElectron
 *
 * \brief Parallel SIMDized implementation of TRD tracking.
 *
 * Parallel SIMDized implementation of TRD tracking.
 * Input: array with track seeds and array with hits.
 * Output: reconstructed tracks.
 * Algorithm is based on track following and Kalman Filter methods.
 * Implementation is based on nearest neighbor approach:
 * track is propagated from station to station and
 * nearest hit from validation gate is attached to track.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 *
 **/
class LitTrackFinderNNVecElectron
{
public:
   /**
    * \brief Constructor.
    */
   LitTrackFinderNNVecElectron();

   /**
    * \brief Destructor.
    */
   virtual ~LitTrackFinderNNVecElectron();

   /**
    * \brief Main function for track reconstruction.
    * \param[in] Hit array.
    * \param[in] Track seed array.
    * \param[out] Output array with reconstructed tracks.
    */
   void DoFind(
      const PixelHitArray& hits,
      const TrackArray& trackSeeds,
      TrackArray& tracks);

   /*
    * \brief Set detector layout.
    * \param[in] layout Detector layout to be set.
    */
   void SetDetectorLayout(
         const LitDetectorLayoutElectron<fvec>& layout) {
      fLayout = layout;
      fHitData.SetDetectorLayout(layout);
   }

private:

   void ArrangeHits(
       const PixelHitArray& hits);

   void InitTrackSeeds(
      const TrackArray& trackSeeds);

   /* Follows tracks through the detector
    * @param itBegin Iterator to the first track.
    * @param itEnd Iterator to the last track.
    */
   void FollowTracks();

   /* TODO: Add comments
    *
    */
   void PropagateToFirstStation(
      LitScalTrack* tracks[]);

   /*
    * TODO Add comments
    */
   void CollectHits(
      LitTrackParamScal* par,
      LitScalTrack* track,
      unsigned char stationGroup,
      unsigned char station);

   /* TODO: Add comment
    *
    */
   inline void ProcessStation(
      LitScalTrack* tracks[],
      unsigned char stationGroup,
      unsigned char station);

   /*
    *
    */
   bool AddNearestHit(
      LitScalTrack* track,
      const PixelHitConstIteratorPair& hits,
      unsigned int nofHits,
      int stationGroup,
      int station);

   /*
    *
    */
   void MinMaxIndex(
      const LitTrackParamScal* par,
      const PixelHitArray& hits,
      fscal maxErr,
      PixelHitConstIterator& first,
      PixelHitConstIterator& last);

private:
   /* Local copy of tracks */
   TrackArray fTracks;
   /* Detector geometry */
   LitDetectorLayoutElectron<fvec> fLayout;
   /* Arranged hits */
   LitHitDataElectron<fvec> fHitData;
   /* Maximum number of missing hits */
   unsigned char fMaxNofMissingHits;
   /* Sigma coefficient for fast hit search */
   fscal fSigmaCoef;
   /* Maximum covariance value */
   fscal fMaxCovSq;
   /* Chi square cut for pixel hits */
   fvec fChiSqPixelHitCut;
};

} // namespace parallel
} // namespace lit
#endif /* LITTRACKFINDERNNVECELECTRON_H_ */
