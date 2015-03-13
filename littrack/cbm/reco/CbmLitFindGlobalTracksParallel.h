/**
 * \file CbmLitFindGlobalTracksParallel.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 * \brief CBM task for parallel global track reconstruction.
 **/

#ifndef CBMLITFINDGLOBALTRACKSPARALLEL_H_
#define CBMLITFINDGLOBALTRACKSPARALLEL_H_

#include "FairTask.h"
#include "base/CbmLitDetectorSetup.h"

#include "TStopwatch.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

class CbmLitFindGlobalTracksParallel : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFindGlobalTracksParallel();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFindGlobalTracksParallel();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(Option_t* opt);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

   /* Setters */
   void SetTrackingType(const string& trackingType) { fTrackingType = trackingType; }
   void SetMergerType(const string& mergerType) { fMergerType = mergerType; }
   void SetFitterType(const string& fitterType) { fFitterType = fitterType; }

private:

   /**
    * \brief Reads necessary data branches from the input data files and
    * creates branches for CbmGlobalTrack, CbmTrdTrack, CbmMuchTrack
    */
   void ReadAndCreateDataBranches();

   /**
    * \brief Do the track reconstruction.
    */
   void DoTracking();

   /**
    * \brief Create and fill global tracks.
    */
   void ConstructGlobalTracks();

   /**
    * \brief Print stopwatch statistics.
    */
   void PrintStopwatchStatistics();

   CbmLitDetectorSetup fDet; // detector setup

   // Pointers to data arrays
   TClonesArray* fStsTracks; // CbmStsTrack array
   TClonesArray* fTrdHits; // CbmTrdHit array
   TClonesArray* fTrdTracks; // output CbmTrdTrack array
   TClonesArray* fMuchPixelHits; // CbmMuchPixelHit array
   TClonesArray* fMuchTracks; // output CbmMuchTrack array
   TClonesArray* fGlobalTracks; //output CbmGlobalTrack array

   // Settings
   // Tracking method to be used
   // "branch" - branching method
   // "nn" - nearest neighbor method
   string fTrackingType;

   // Merger method to be used
   // "nearest_hit" - assigns nearest hit to the track
   string fMergerType;

   // Track fitter to be used for the final track fit
   // "lit_kalman" - forward Kalman track fit with LIT propagation and TGeo navigation
   string fFitterType;

   // stopwatches
   TStopwatch fTrackingWatch; // stopwatch for tracking without IO
   TStopwatch fTrackingWithIOWatch; // stopwatch for tracking including IO

   CbmLitFindGlobalTracksParallel(const CbmLitFindGlobalTracksParallel&);
   CbmLitFindGlobalTracksParallel& operator=(const CbmLitFindGlobalTracksParallel&);
   
   ClassDef(CbmLitFindGlobalTracksParallel, 1);
};

#endif /* CBMLITFINDGLOBALTRACKSPARALLEL_H_ */
