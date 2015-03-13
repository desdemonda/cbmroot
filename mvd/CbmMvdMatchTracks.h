// -------------------------------------------------------------------------
// -----                  CbmMvdMatchTracks header file                -----
// -----                  Derived from CbmStsMatchTracks by V. Friese  -----
// -----                  Created by M.Deveaux 16.06.2009              -----
// -------------------------------------------------------------------------


/** CbmMvdMatchTracks.h
 *@author M.Deveaux <M.Deveaux@gsi.de>
 **
 ** Task class for matching a reconstructed CbmMvdTrack with a simulated
 ** CbmMCTrack. The matching criterion is a maximal number of common
 ** hits/points. The task fills the data class CbmMvdTrackMatch for
 ** each CbmMvdTrack.
 **/


#ifndef CBMMVDMATCHTRACKS_H
#define CBMMVDMATCHTRACKS_H 1


#include "FairTask.h"

#include "TStopwatch.h"

#include <map>

class TClonesArray;



class CbmMvdMatchTracks : public FairTask
{

 public:

  /** Default constructor **/
  CbmMvdMatchTracks();


  /** Standard constructor
   **
   *@param verbose  Verbosity level
   **/
  CbmMvdMatchTracks(Int_t iVerbose );


  /** Constructor with task name
   **
   *@param name     Name of task
   *@param verbose  Verbosity level
   **/
  CbmMvdMatchTracks(const char* name, Int_t verbose );


  /** Destructor **/
  virtual ~CbmMvdMatchTracks();


  /** Intialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Execution **/
  virtual void Exec(Option_t* opt);


  /** Finishing */
  virtual void Finish();


 private:

  TClonesArray* fTracks;       // Array of CbmStsTracks
  TClonesArray* fPoints;       // Array of CbmMvdPoints
  TClonesArray* fHits;         // Array of CbmMvdHits
  TClonesArray* fMatches;      // Array of CbmMvdTrackMatch
  TStopwatch    fTimer;        // Timer

  /** Map from MCTrackId to number of common hits **/
  std::map<Int_t, Int_t> fMatchMap;

  Int_t    fNEvents;        /** Number of events with success **/
  Int_t    fNEventsFailed;  /** Number of events with failure **/
  Double_t fTime;           /** Total real time used for good events **/
  Double_t fNTrackMatches;  /** Total number of matched tracks **/
  Double_t fNAllHits;       /** Total number of hits **/
  Double_t fNTrueHits;      /** Number pf correctly assigned hits **/
  



  ClassDef(CbmMvdMatchTracks,1);

};

#endif
