// -------------------------------------------------------------------------
// -----                   CbmTrdFindTracks header file                -----
// -----                  Created 25/11/05  by D. Kresan               -----
// -----                 according to the CbmStsFindTracks             -----
// -------------------------------------------------------------------------


/** CbmTrdFindTracks
 *@author D.Kresan <D.Kresan@gsi.de>
 **
 ** Task class for track finding in the TRD.
 ** Input: TClonesArray of CbmTrdHit
 ** Output: TClonesArray of CbmTrdTrack
 **
 ** Uses as track finding algorithm classes derived from CbmTrdTrackFinder.
 **/


#ifndef CBMTRDFINDTRACKS
#define CBMTRDFINDTRACKS 1


#include "FairTask.h"

class CbmTrdTrackFinder;
class TClonesArray;

class CbmTrdFindTracks : public FairTask
{

 public:

  /** Default constructor **/
  CbmTrdFindTracks();


  /** Standard constructor 
   **
   *@param name   Name of class
   *@param title  Task title
   *@param finder Pointer to STS track finder concrete class
   **/
  CbmTrdFindTracks(const char* name, const char* title = "FairTask", 
		   CbmTrdTrackFinder* finder = NULL);


  /** Destructor **/
  virtual ~CbmTrdFindTracks();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** SetParContainers **/
  virtual void SetParContainers();


  /** Accessors **/
  CbmTrdTrackFinder* GetFinder() { return fFinder; };
  Int_t GetNofTracks()           { return fNofTracks; };


  /** Set concrete track finder **/
  void UseFinder(CbmTrdTrackFinder* finder) { fFinder = finder; };



 private:

  CbmTrdTrackFinder* fFinder;    // Pointer to TrackFinder concrete class
  TClonesArray* fTrdHitArray;    // Input array of TRD hits
  TClonesArray* fTrackArray;     // Output array of CbmTrdTracks 
  Int_t fNofTracks;              // Number of tracks created

  CbmTrdFindTracks(const CbmTrdFindTracks&);
  CbmTrdFindTracks& operator=(const CbmTrdFindTracks&);

  ClassDef(CbmTrdFindTracks,1);

};

#endif
