//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// CbmStsCMTrackFinder class -                                                  //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef CBM_STS_CM_TRACK_FINDER_H
#define CBM_STS_CM_TRACK_FINDER_H

#include "TObject.h"
//#include "TBenchmark.h"
#include "FairTask.h"
#include "CbmStsVertex.h"
#include "CbmStsHit.h"
#include "TObjArray.h"
#include "CbmStsCMPoint.h"
#include "CbmStsCMTrack.h"
#include "CbmStsTrackletCuts.h"
#include "CbmStsTrackCuts.h"
#include "CbmStsSegments.h"

#include "Riostream.h"

class CbmStsCMTrackFinder : public FairTask {

protected:

  CbmStsVertex   *fVertex;          // pointer to the vertex
  TObjArray   *fCMHits;       // Array of hits transformed with conformal mapping

  TClonesArray  *fHits;            // Array of event's hits
  TClonesArray  *fFoundTracks;     // Array of found tracks
  TObjArray     *fCMTracks;        // Array of tracks


  // Number of cells (segments)
  Int_t  fNumOfStation;      //  the number of station
  Int_t  fMaxSTStation;      // max number of silicon stations

  // Tracking information
  Int_t fMainVertexTracks; // number of tracks coming from the main vertex
  Int_t fMergedSplits;     // number of merged split tracks
  Int_t fExtendedTracks;   // number of extended tracks
  Int_t fHitsUnused;       // number of unused hits

  Bool_t fVertexConstraint;    // vertex constraint (true for main vertex tracking)

  // Cuts
  CbmStsTrackletCuts  *fTrackletCuts;    // cuts for tracklets
  CbmStsTrackCuts     *fTrackCuts;       // cuts for tracks
  CbmStsSegments      *fSegment;         //

public:

  CbmStsCMTrackFinder();            //
  CbmStsCMTrackFinder(const char *name, const char *title="CBM Task");
  virtual  ~CbmStsCMTrackFinder();  //

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();              // 
  virtual void Finish();        // 

  void  Reset();                    //
  void  Register();                 //

  // getters

  CbmStsVertex  *GetVertex()     { return fVertex;}
  Int_t   GetNumberOfHits()      { return fCMHits->GetEntriesFast();}
  Int_t   GetNumberOfTracks()    { return fCMTracks->GetEntriesFast();}

  TObjArray  *GetTracks()        { return fCMTracks;}

  Int_t  GetNumPrimaryTracks()    { return fMainVertexTracks;} // # of main vertex tracks
  Int_t  GetNumMergedSplits()     { return fMergedSplits;}     // # of merged tracks
  Int_t  GetNumExtendedTracks()   { return fExtendedTracks;}   // # of extended tracks

  Int_t  GetNumHitsUnused()   { return fHitsUnused;}       // # of unused hits

  // setters:

  void SetVertexConstraint (Bool_t f) { fVertexConstraint = f; } //
  void AddTrackForFit(CbmStsJointTrack *track);     //

  // Tracking procedures:
  void  MainVertexTracking();              // tracking of main vertex tracks
  void  PrimaryHighMomTracking();
  void  PrimaryIntermedMomTracking();
  void  PrimaryLowMomTracking();

  //  void  FreeTracking();                        // tracking without vertex constraint 
  void  LoopOverHits();                        // loops over hits
  void  CreateTrack(CbmStsCMPoint *hit);       // create track with start at hit
  void  RemoveTrack(CbmStsCMTrack *track);
  void  CompleteTrack(CbmStsCMTrack *track);   // completes track
  void  LoopUpdate(Int_t *sub_row_segm,
		   Bool_t backward);           // tests if loop should be continued

  void GetNextNeighbor(CbmStsCMPoint *start_hit,
		       CbmStsCMTrack *t,
		       Bool_t backward);

  Bool_t const TestExpression(Int_t srs, Int_t er, Bool_t bkwd = (Bool_t)true);

  Bool_t TrackExtension(CbmStsCMTrack *track);      // Trys to extend given track.
  void  ExtendTracks();      //

  void  CheckClonedTracks();
  void  AlignTracks(CbmStsCMTrack *t1, CbmStsCMTrack *t2);
  void  MergeSplitTracks(CbmStsCMTrack *t1, CbmStsCMTrack *t2);  // merges two tracks
  Bool_t TestTrackMasks(unsigned char mask1, unsigned char mask2);

  void  OutputTracks();                     // 
  void  TrackingInfo(char* info);           // information about the tracking 

  ClassDef(CbmStsCMTrackFinder, 1)          //  
    
    };

//________________________________________________________________
inline void CbmStsCMTrackFinder::RemoveTrack(CbmStsCMTrack *track)
{
  // Removes track from TObjArray and release the points.

  track->ClearPoints();            // release points
  fCMTracks->Remove(track);        // remove TObjArray
  delete track;                    // delete track
  track = NULL;

}

#endif
