#ifndef CbmStsMatchTracksEpoch_H
#define CbmStsMatchTracksEpoch_H 1


#include "FairTask.h"

#include "TStopwatch.h"

#include <map>

class TClonesArray;
class CbmMCEpoch;


class CbmStsMatchTracksEpoch : public FairTask
{

 public:

  /** Default constructor **/
  CbmStsMatchTracksEpoch();


  /** Standard constructor
   **
   *@param verbose  Verbosity level
   **/
  CbmStsMatchTracksEpoch(Int_t iVerbose );


  /** Constructor with task name
   **
   *@param name     Name of task
   *@param verbose  Verbosity level
   **/
  CbmStsMatchTracksEpoch(const char* name, Int_t verbose );


  /** Destructor **/
  virtual ~CbmStsMatchTracksEpoch();


  /** Intialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Execution **/
  virtual void Exec(Option_t* opt);


  /** Finishing */
  virtual void Finish();


 private:

  TClonesArray* fTracks;       // Array of CbmStsTracks
  TClonesArray* fPoints;       // Array of CbmStsPoints
  TClonesArray* fHits;         // Array of CbmStsHits
  TClonesArray* fMatches;      // Array of CbmStsTrackMatch
  TStopwatch    fTimer;        // Timer
  CbmMCEpoch*   fMcEpoch;
  
  /** Map from MCTrackId to number of common hits **/
  std::map<Int_t, Int_t> fMatchMap;

  Int_t    fNEvents;        /** Number of events with success **/
  Int_t    fNEventsFailed;  /** Number of events with failure **/
  Double_t fTime;           /** Total real time used for good events **/
  Double_t fNTrackMatches;  /** Total number of matched tracks **/
  Double_t fNAllHits;       /** Total number of hits **/
  Double_t fNTrueHits;      /** Number pf correctly assigned hits **/
  
  CbmStsMatchTracksEpoch(const CbmStsMatchTracksEpoch&);
  CbmStsMatchTracksEpoch operator=(const CbmStsMatchTracksEpoch&);

  ClassDef(CbmStsMatchTracksEpoch,1);

};

#endif
