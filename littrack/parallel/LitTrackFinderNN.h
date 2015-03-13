/**
 * \file LitTrackFinderNN.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 * \brief Parallel implementation of the nearest neighbor tracking algorithm.
 */

#ifndef LITTRACKFINDERNN_H_
#define LITTRACKFINDERNN_H_

#include "LitDetectorLayout.h"
#include "LitHitData.h"
#include <vector>
#include <set>

using std::vector;
using std::set;

#include "LitScalPixelHit.h"
#include "LitScalTrack.h"

namespace lit {
namespace parallel {

class LitTrackFinderNN {
public:
   /**
    * \brief Constructor.
    */
   LitTrackFinderNN();

   /**
    * \brief Destructor.
    */
   virtual ~LitTrackFinderNN();

   /**
    * \brief Main function for track reconstruction.
    * \param[in] Array of hits.
    * \param[in] Array of track seeds.
    * \param[out] Output array of reconstructed tracks.
    */
   void DoFind(
      const vector<lit::parallel::LitScalPixelHit*>& hits,
      const vector<lit::parallel::LitScalTrack*>& trackSeeds,
      vector<lit::parallel::LitScalTrack*>& tracks);


   /* Setters */
   void SetDetectorLayout(const LitDetectorLayoutScal& layout) { fLayout = layout; }
   void SetNofIterations(int nofIterations) { fNofIterations = nofIterations; }
   void SetMaxNofMissingHits(const vector<int>& maxNofMissingHits) { fMaxNofMissingHits = maxNofMissingHits; }
   void SetPDG(const vector<int>& pdg) { fPDG = pdg; }
   void SetChiSqStripHitCut(const vector<fscal>& chiSqStripHitCut) { fChiSqStripHitCut = chiSqStripHitCut; }
   void SetChiSqPixelHitCut(const vector<fscal>& chiSqPixelHitCut) { fChiSqPixelHitCut = chiSqPixelHitCut; }
   void SetSigmaCoef(const vector<fscal>& sigmaCoef) { fSigmaCoef = sigmaCoef; }

protected:

   void ArrangeHits(
         const vector<lit::parallel::LitScalPixelHit*>& hits);

   /**
    * \brief Initialize track seeds and copy to local array.
    */
   void InitTrackSeeds(
         const vector<lit::parallel::LitScalTrack*>& trackSeeds);

   void PropagateVirtualStations(
         LitTrackParamScal& par);

   void PropagateToStation(
         unsigned char stationId,
         LitTrackParamScal& par);

   /**
    * \brief Follow tracks through detector
    */
   void FollowTracks();

   void SelectTracks();

   /**
    * \brief Write already used hits to a used hits set.
    */
   void RemoveHits();

   /**
    * \brief Copy tracks to output array.
    */
   void CopyToOutput(
         vector<lit::parallel::LitScalTrack*>& tracks);

private:
   vector<lit::parallel::LitScalTrack*> fTracks; // Local copy of tracks.
   LitHitData fHitData; // Hit storage.
   set<int> fUsedHitsSet; // Sets with hits that have been used.
   set<int> fUsedSeedsSet; // Set with track seeds that have been used.

   LitDetectorLayoutScal fLayout; // Detector layout
   int fNofIterations; // Number of tracking iterations
   int fIteration; // Current tracking iteration
   // Tracking parameters for each iteration
   vector<int> fMaxNofMissingHits; // Maximum number of acceptable missing hits.
   vector<int> fPDG; // Particle hypothesis for tracking.
   vector<fscal> fChiSqStripHitCut; // Chi-square cut for strip hits.
   vector<fscal> fChiSqPixelHitCut; // Chi-square cut for pixel hits.
   vector<fscal> fSigmaCoef; // Sigma coefficient for preliminary hit selection
};

} // namespace parallel
} // namespace lit
#endif /* LITTRACKFINDERNN_H_ */
