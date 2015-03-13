/**
 * \file LitTrackFinderNNVecMuon.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Parallel implementation of MUCH tracking.
 **/

#ifndef LITTRACKFINDERNNVECMUON_H_
#define LITTRACKFINDERNNVECMUON_H_

#include "LitDetectorLayoutMuon.h"
#include "LitHitDataMuon.h"

#include <vector>
#include <map>

namespace lit {
namespace parallel {

/**
 * \class LitTrackFinderNNVecMuon.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Parallel implementation of MUCH tracking.
 *
 * Input: array of track seeds and array of hits.
 * Output: reconstructed tracks.
 * Algorithm is based on track following and Kalman Filter methods.
 * The track is propagated from station to station and nearest
 * hit in the validation gate is attached to track.
 **/
class LitTrackFinderNNVecMuon
{
public:
   /**
    * \brief Constructor.
    */
   LitTrackFinderNNVecMuon();

   /**
    * \brief Destructor.
    */
   virtual ~LitTrackFinderNNVecMuon();

   /**
    * \brief Main function for tracking.
    * \param[in] hits Array of hits.
    * \param[in] trackSeeds Array of track seeds.
    * \param[out] track Output reconstructed tracks.
    */
   virtual void DoFind(
      const PixelHitArray& hits,
      const TrackArray& trackSeeds,
      TrackArray& tracks);

   /**
    * \brief Sets detector layout for the tracking.
    * \param[in] layout Detector layout.
    */
   void SetDetectorLayout(
         const LitDetectorLayoutMuon<fvec>& layout) {
      fLayout = layout;
      fHitData.SetDetectorLayout(layout);
   }

private:

   void ArrangeHits(
       const PixelHitArray& hits);

    /* Initializes the track seeds
     * @param itBegin Iterator to the first track seed.
     * @param itEnd iterator to the last track seed. */
    void InitTrackSeeds(
       const TrackArray& trackSeeds);

    /* Follows tracks through the detector
     * @param itBegin Iterator to the first track.
     * @param itEnd Iterator to the last track. */
    void FollowTracks();

    /* TODO: Add comments
     *
     */
    void PropagateThroughAbsorber(
          const std::vector<unsigned int>& tracksId1,
          const LitAbsorber<fvec>& absorber);

    /* TODO: Add comments
     *
     */
    inline void PropagateThroughAbsorber(
       const TrackArray& tracks,
       const LitAbsorber<fvec>& absorber);

    /*
     * TODO Add comments
     */
    void CollectHits(
       std::vector<LitTrackParamScal>& par,
       LitScalTrack* track,
       unsigned char stationGroup,
       unsigned char station,
       unsigned char nofSubstations);

    /*
     * TODO Add comments
     */
    void ProcessStation(
          const std::vector<unsigned int>& tracksId1,
          unsigned char stationGroup,
          unsigned char station);

    /* TODO: Add comment
     *
     */
    inline void ProcessStation(
       const TrackArray& tracks,
       unsigned char stationGroup,
       unsigned char station);

    /* Adds the nearest hit to the track.
     * @param track Pointer to the track.
     * @param hits Reference to hit vector from which the nearest hit will be attached.
     * @return True if the hit was attached to track.
     */
    bool AddNearestHit(
       LitScalTrack* track,
       const PixelHitArray& hits,
       const std::vector<LitTrackParamScal*>& pars,
       unsigned int nofHits);

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
   TrackArray fTracks; // Local copy of tracks
   LitDetectorLayoutMuon<fvec> fLayout; // Detector geometry
   LitHitDataMuon<fvec> fHitData; // Arranged hits
   unsigned char fMaxNofMissingHits; // Maximum number of missing hits
   bool fIsProcessSubstationsTogether; //  If true than hits from all substations are gathered together and hit selection is done over all this hits at a time
   fscal fSigmaCoef; // Sigma coefficient for fast hit search
   fscal fMaxCovSq; // Maximum covariance value
   fvec fChiSqPixelHitCut; // Chi square cut for pixel hits
};

} // namespace parallel
} // namespace lit
#endif /* LITTRACKFINDERNNVECMUON_H_ */
