// -------------------------------------------------------------------------
// -----                    CbmTrdSetTracksPidModWkn header file          -----
// -----                  Created 13/02/2007  by F. Uhlig              -----
// -------------------------------------------------------------------------


/** CbmTrdSetTracksPidModWkn
 **
 *@author O.Derenovskaya <odenisova@jinr.ru>
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


#ifndef CBM_TRD_SET_TRACKS_PID_MOD_WKN_H
#define CBM_TRD_SET_TRACKS_PID_MOD_WKN_H

#include "TH1D.h"
#include <vector>
#include "FairTask.h"
#include <string>

class TClonesArray;
using std::string;

class CbmTrdSetTracksPidModWkn : public FairTask
{

 public:

  /** Default constructor **/
  CbmTrdSetTracksPidModWkn();


  /** Standard constructor 
   **
   *@param name   Name of class
   *@param title  Task title
   **/
  CbmTrdSetTracksPidModWkn(const char* name, const char* title = "FairTask");


  /** Destructor **/
  virtual ~CbmTrdSetTracksPidModWkn();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();

  /** Set Type **/
  void SetType(const string& SISType) { fSISType = SISType;}
  void SetSIS100() { fSISType="sis100"; }
  void SetSIS300() { fSISType="sis300"; }
  
  void SetParameters();

  private:
  TClonesArray* fTrackArray;   // Input array of TRD tracks
  TClonesArray* fTrdHitArray;  // Input array of TRD Hits

// Parametrs
  int fnSet; //  number of the layers with TR
  float  fdegWkn, //  statistics degree
         fk1,
         fwkn0,
         fEmp, // energy loss transformation parameter
         fXi,  // energy loss transformation parameter
		 fWmin,
		 fWmax,
		 fDiff;


   // Settings
   // Set-up
   // "sis100" - TRD with 10 layers
   // "sis300" - TRD with 4 layers
   string fSISType;
   
  CbmTrdSetTracksPidModWkn(const CbmTrdSetTracksPidModWkn&);
  CbmTrdSetTracksPidModWkn& operator=(const CbmTrdSetTracksPidModWkn&);

  ClassDef(CbmTrdSetTracksPidModWkn,1);

};

#endif
