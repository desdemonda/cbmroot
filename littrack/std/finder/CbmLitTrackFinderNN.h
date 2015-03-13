/**
 * \file CbmLitTrackFinderNN.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \since 2008
 * \brief Implementation of nearest neighbor tracking algorithm.
 *
 * Input to tracking: array of track seeds and array of hits.
 * The track is propagated from station to station and the
 * nearest hit is attached to track.
 **/

#ifndef CBMLITTRACKFINDERNN_H_
#define CBMLITTRACKFINDERNN_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitPtrTypes.h"
#include "base/CbmLitHitData.h"
#include "interface/CbmLitTrackFinder.h"
#include <vector>

using std::vector;

class CbmLitTrackFinderNN : public CbmLitTrackFinder
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackFinderNN();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackFinderNN();

   /**
    * \brief Inherited from CbmLitTrackFinder.
    */
   virtual LitStatus DoFind(
      HitPtrVector& hits,
      TrackPtrVector& trackSeeds,
      TrackPtrVector& tracks);

   /* Setters */
   void SetSeedSelection(TrackSelectionPtr seedSelection) { fSeedSelection = seedSelection; }
   void SetFinalSelection(TrackSelectionPtr finalSelection) { fFinalSelection = finalSelection; }
   void SetPropagator(TrackPropagatorPtr propagator) { fPropagator = propagator; }
   void SetFilter(TrackUpdatePtr filter) { fFilter = filter; }
   void SetNofStations(Int_t nofStations) { fNofStations = nofStations; }
   void SetNofIterations(Int_t nofIterations) { fNofIterations = nofIterations; }
   void SetMaxNofMissingHits(const vector<Int_t>& maxNofMissingHits) { fMaxNofMissingHits = maxNofMissingHits; }
   void SetPDG(const vector<Int_t>& pdg) { fPDG = pdg; }
   void SetChiSqStripHitCut(const vector<litfloat>& chiSqStripHitCut) { fChiSqStripHitCut = chiSqStripHitCut; }
   void SetChiSqPixelHitCut(const vector<litfloat>& chiSqPixelHitCut) { fChiSqPixelHitCut = chiSqPixelHitCut; }
   void SetSigmaCoef(const vector<litfloat>& sigmaCoef) { fSigmaCoef = sigmaCoef; }

protected:

   void ArrangeHits(
      HitPtrIterator itBegin,
      HitPtrIterator itEnd);

   /**
    * \brief Initialize track seeds and copy to local array.
    * \param[in] itBegin Iterator to the first track seed.
    * \param[in] itEnd Iterator to the last track seed.
    */
   void InitTrackSeeds(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /**
    * \brief Follow tracks through detector
    * \param[in] itBegin Iterator to the first track.
    * \param[in] itEnd Iterator to the last track.
    */
   void FollowTracks(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /**
    * \brief Write already used hits to a used hits set.
    * \param[in] Iterator to the first track.
    * \param[in] Iterator to the last track.
    */
   void RemoveHits(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /**
    * \brief Copy tracks to output array.
    * \param[in] Iterator to the first track.
    * \param[in] Iterator to the last track.
    * \param[out] Output track array.
    */
   void CopyToOutput(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd,
      TrackPtrVector& tracks);

private:
   TrackPtrVector fTracks; // Local copy of tracks.
   CbmLitHitData fHitData; // Hit storage.
   std::set<Int_t> fUsedHitsSet; // Sets with hits that have been used.
   std::set<Int_t> fUsedSeedsSet; // Set with track seeds that have been used.

   TrackSelectionPtr fSeedSelection; // Seed selection tool.
   TrackSelectionPtr fFinalSelection; // Final track selection tool.
   TrackPropagatorPtr fPropagator; // Track propagation tool.
   TrackUpdatePtr fFilter; // KF update tool.

   Int_t fNofStations; // Number of tracking stations.
   Int_t fNofIterations; // Number of tracking iterations
   Int_t fIteration; // Current tracking iteration
   // Tracking parameters for each iteration
   vector<Int_t> fMaxNofMissingHits; // Maximum number of acceptable missing hits.
   vector<Int_t> fPDG; // Particle hypothesis for tracking.
   vector<litfloat> fChiSqStripHitCut; // Chi-square cut for strip hits.
   vector<litfloat> fChiSqPixelHitCut; // Chi-square cut for pixel hits.
   vector<litfloat> fSigmaCoef; // Sigma coefficient for preliminary hit selection
};

#endif /* CBMLITTRACKFINDERNN_H_ */

