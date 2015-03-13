#ifndef CbmLitFindMuchTracksEpoch_H_
#define CbmLitFindMuchTracksEpoch_H_

#include "FairTask.h"

#include "base/CbmLitTypes.h"
#include "base/CbmLitPtrTypes.h"
#include <string>

class TClonesArray;

class CbmLitFindMuchTracksEpoch : public FairTask
{
public:
   CbmLitFindMuchTracksEpoch();
   virtual ~CbmLitFindMuchTracksEpoch();
   virtual InitStatus Init();
   virtual void Exec(Option_t* opt);
   virtual void SetParContainers();
   void SetTrackingType(const std::string& trackingType) { fTrackingType = trackingType;}
   void SetMergerType(const std::string& mergerType) { fMergerType = mergerType;}
   void SetFitterType(const std::string& fitterType) { fFitterType = fitterType;}
   void SetSliceSeparationTime(double time) { fSliceSeparationTime = time; }
   void SetHitTimeResolution(double time) { fHitTimeResolution = time; }
private:
   virtual void Finish();
   void ReadAndCreateDataBranches();
   void InitTrackReconstruction();
   void ClearArrays();
   void RunTrackReconstruction();
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fStsHits; // CbmStsHit
   TClonesArray* fMuchPixelHits; // CbmMuchPixelHit array
   TClonesArray* fMuchTracks; // output CbmMuchTrack array
   TClonesArray* fGlobalTracks; //output CbmGlobalTrack array

   // LIT data arrays
   TrackPtrVector fLitStsTracks; // STS tracks
   HitPtrVector   fLitHits; // MUCH hits
   TrackPtrVector fLitOutputTracks; // output Lit tracks

   // Tools
   TrackFinderPtr fFinder; // track finder
   HitToTrackMergerPtr fMerger; // hit-to-track merger
   TrackFitterPtr fFitter; // track fitter

   // Settings
   // Tracking method to be used
   // "branch" - branching method
   // "nn" - nearest neighbor method
   // "weight" - weighting method
   std::string fTrackingType;

   // Merger method to be used
   // "nearest_hit" - assigns nearest hit to the track
   std::string fMergerType;

   // Track fitter to be used for the final track fit
   // "lit_kalman" - forward Kalman track fit with LIT propagation and TGeo navigation
   std::string fFitterType;


   Int_t fEventNo; // event counter
   Double_t fSliceSeparationTime;
   Double_t fHitTimeResolution;
   ClassDef(CbmLitFindMuchTracksEpoch, 1);
};

#endif /* CbmLitFindMuchTracksEpoch_H_ */
