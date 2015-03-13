// -------------------------------------------------------------------------
// -----                    CbmTrdFitTracks header file                -----
// -----                  Created 28/11/05  by D. Kresan               -----
// -----                 according to the CbmStsFitTracks              -----
// -------------------------------------------------------------------------


/** CbmTrdFitTracks
 *@author D.Kresan <D.Kresan@gsi.de>
 **
 ** Task class for track fitting in the TRD.
 ** Input: TClonesArray of CbmTrdTrack
 ** Parameters of these objects are updated
 **
 ** Uses as track fitting algorithm classes derived from CbmTrdTrackFitter.
 **/


#ifndef CBMTRDFITTRACKS
#define CBMTRDFITTRACKS 1


#include "FairTask.h"

class CbmTrdTrackFitter;
class TClonesArray;

class CbmTrdFitTracks : public FairTask
{

 public:

  /** Default constructor **/
  CbmTrdFitTracks();


  /** Standard constructor 
   **
   *@param name   Name of class
   *@param title  Task title
   *@param finder Pointer to STS track finder concrete class
   **/
  CbmTrdFitTracks(const char* name, const char* title = "FairTask",
		  CbmTrdTrackFitter* fitter = NULL);


  /** Destructor **/
  virtual ~CbmTrdFitTracks();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** SetParContainers **/
  virtual void SetParContainers();


  /** Accessors **/
  CbmTrdTrackFitter* GetFitter() { return fFitter; };
  Int_t GetNofTracks()           { return fNofTracks; };


  /** Set concrete track finder **/
  void UseFitter(CbmTrdTrackFitter* fitter) { fFitter = fitter; };



 private:

  CbmTrdTrackFitter* fFitter;    // Pointer to TrackFinder concrete class
  TClonesArray* fTrackArray;     // Input array of STS tracks
  Int_t fNofTracks;              // Number of tracks successfully fitted

  CbmTrdFitTracks(const CbmTrdFitTracks&);
  CbmTrdFitTracks& operator=(const CbmTrdFitTracks&);

  ClassDef(CbmTrdFitTracks,1);

};

#endif
