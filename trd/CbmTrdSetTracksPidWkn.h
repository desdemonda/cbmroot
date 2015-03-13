// -------------------------------------------------------------------------
// -----                    CbmTrdSetTracksPidWkn header file          -----
// -----                  Created 13/02/2007  by F. Uhlig              -----
// -------------------------------------------------------------------------


/** CbmTrdSetTracksPidWkn
 **
 *@author F.Uhlig <F.Uhlig@gsi.de>
 **
 ** Task class for PID calculation in the TRD using the
 ** Wkn method (see V. Ivanovs presentation) at CBM
 ** Collaboration meeting in March 2007
 ** http://)
 ** Input: TClonesArray of CbmTrdTrack
 ** Parameters: The parameters of these method are fixed 
 ** using the energy loss spectra of pions. 
 ** The parameters set in the constructor
 ** are the default values for the SVN with the new corrected
 ** enrgy loss in the TRD. These parameters should only be
 ** changed when one knows how to extract them correctly from
 ** the pion enrgy loss spectra.
 **
 **/


#ifndef CBMTRDSETTRACKSPIDWKN
#define CBMTRDSETTRACKSPIDWKN 1


#include "FairTask.h"

class TClonesArray;

class CbmTrdSetTracksPidWkn : public FairTask
{

 public:

  /** Default constructor **/
  CbmTrdSetTracksPidWkn();


  /** Standard constructor 
   **
   *@param name   Name of class
   *@param title  Task title
   **/
  CbmTrdSetTracksPidWkn(const char* name, const char* title = "FairTask");


  /** Destructor **/
  virtual ~CbmTrdSetTracksPidWkn();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** SetParContainers **/
  virtual void SetParContainers();


  /** Accessors **/
  Int_t GetNofTracks()         { return fNofTracks; };
  Double_t GetWknPar1()        { return WknPar1; };
  Double_t GetWknPar2()        { return WknPar2; };

  /** Setters **/
  void SetWknPar1(Double_t param) { WknPar1=param; };
  void SetWknPar2(Double_t param) { WknPar2=param; };


 private:

  TClonesArray* fTrackArray;   // Input array of TRD tracks
  TClonesArray* fTrdHitArray;  // Input array of TRD Hits
  Int_t fNofTracks;            // Number of tracks successfully fitted
  Double_t WknPar1;            // Parameter1 for Wkn method
  Double_t WknPar2;            // Parameter2 for Wkn method


  Double_t CbmWknStat(Double_t* Y, Int_t k, Int_t n);

  CbmTrdSetTracksPidWkn(const CbmTrdSetTracksPidWkn&);
  CbmTrdSetTracksPidWkn& operator=(const CbmTrdSetTracksPidWkn&);

  ClassDef(CbmTrdSetTracksPidWkn,1);

};

#endif
