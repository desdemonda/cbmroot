/**
 * \file CbmLitTrackFinderBranch.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 * \brief Track reconstruction using branching method.
 *
 * Tracks are propagated from station to station and new
 * track branches are started for each hit in the validation gate.
 **/

#ifndef CBMLITTRACKFINDERBASEBRANCH_H_
#define CBMLITTRACKFINDERBASEBRANCH_H_

#include "interface/CbmLitTrackFinder.h"
#include "base/CbmLitHitData.h"
#include "base/CbmLitPtrTypes.h"

#include <map>
#include <set>

class CbmLitTrackSelection;
class CbmLitTrackUpdate;
class CbmLitTrackFitter;

using std::map;
using std::set;

class CbmLitTrackFinderBranch : public CbmLitTrackFinder
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackFinderBranch();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackFinderBranch();

   /**
    * \brief Inherited from CbmLitTrackFinder.
    */
   LitStatus DoFind(
      HitPtrVector& hits,
      TrackPtrVector& trackSeeds,
      TrackPtrVector& tracks);

   /* Setters */
   void SetFinalSelection(TrackSelectionPtr finalSelection) { fFinalSelection = finalSelection; }
   void SetSeedSelection(TrackSelectionPtr seedSelection) { fSeedSelection=seedSelection; }
   void SetFilter(TrackUpdatePtr filter) { fFilter = filter; }
   void SetPropagator(TrackPropagatorPtr propagator) { fPropagator = propagator; }
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
    * \brief Initialize track seeds.
    * \param[in] itBegin Iterator to the first track seed.
    * \param[in] itEnd Iterator to the last track seed.
    */
   void InitTrackSeeds(
      TrackPtrIterator itBegin,
      TrackPtrIterator itEnd);

   /**
    * \brief Main track following procedure.
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

   CbmLitHitData fHitData; // Hit storage.
   std::set<Int_t> fUsedHitsSet; // Sets with hits that have been used.
   std::set<Int_t> fUsedSeedsSet; // Set with track seeds that have been used.

   TrackPtrVector fTracks; // Local storage of tracks

   TrackSelectionPtr fSeedSelection; // Track seed selection tool
   TrackSelectionPtr fFinalSelection; // Final track selection tool
   TrackPropagatorPtr fPropagator; // Track propagation tool
   TrackUpdatePtr fFilter; // Kalman Filter track update tool

   Int_t fNofStations; // Number of tracking stations.
   Int_t fNofIterations; // Number of tracking iterations
   Int_t fIteration; // Current tracking iteration

   // Tracking parameters for each iteration
   Int_t fMaxNofHitsInValidationGate; // Maximum number of hits in the validation
                                      // gate for which a separate branch is created
   Int_t fMaxNofBranches; // Maximum number of branches for one input track seed
   vector<Int_t> fMaxNofMissingHits; // Maximum number of acceptable missing hits.
   vector<Int_t> fPDG; // Particle hypothesis for tracking.
   vector<litfloat> fChiSqStripHitCut; // Chi-square cut for strip hits.
   vector<litfloat> fChiSqPixelHitCut; // Chi-square cut for pixel hits.
   vector<litfloat> fSigmaCoef; // Sigma coefficient for preliminary hit selection
};

#endif /*CBMLITTRACKFINDERBASEBRANCH_H_*/

