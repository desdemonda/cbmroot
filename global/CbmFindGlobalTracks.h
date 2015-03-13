// -------------------------------------------------------------------------
// -----                  CbmFindGlobalTracks header file              -----
// -----                  Created 01/12/05  by V. Friese               -----
// -------------------------------------------------------------------------


/**  CbmFindGlobalTracks.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Task class for Global CBM tracking. To be run in the reconstruction
 ** chain. Matches StsTrack and TrdTrack and finds the corresponding 
 ** RichRing.
 **/


#ifndef CBMFINDGLOBALTRACKS_H
#define CBMFINDGLOBALTRACKS_H 1


#include "FairTask.h"


class TClonesArray;
class CbmTrackMerger;
class CbmRichMerger;
class CbmTofMerger;


class CbmFindGlobalTracks : public FairTask
{

 public:

  /** Default constructor **/
  CbmFindGlobalTracks();


  /** Standard constructor
   *@param trackMarger  concrete instance of CbmTrackMerger class
   *@param richMerger   concrete instance of CbmRichMerger class
   *@param tofMerger    concrete instance of CbmTofMerger class
   **/
  CbmFindGlobalTracks(CbmTrackMerger* trackMerger,
		      CbmRichMerger* richMerger,
		      CbmTofMerger* tofMerger, Int_t iVerbose = 1);


  /** Constructor with name and title **/
  CbmFindGlobalTracks(const char* name, const char* title="Global",
		      CbmTrackMerger* = NULL,
		      CbmRichMerger*  = NULL,
                      CbmTofMerger* = NULL,
		      Int_t iVerbose = 1);


  /** Destructor **/
  virtual ~CbmFindGlobalTracks();


  /** Initialisation **/
  virtual InitStatus Init();


  /** Execution **/
  virtual void Exec(Option_t* opt);



 private:

  /** Pointer to instance of CbmTrackMerger class **/
  CbmTrackMerger* fTrackMerger;

  /** Pointer to instance of CbmRichMerger class **/
  CbmRichMerger* fRichMerger;

  /** Pointer to instance of CbmTofMerger class **/
  CbmTofMerger* fTofMerger;

  /** Input and output arrays **/
  TClonesArray* fStsTracks;      // input
  TClonesArray* fTrdTracks;      // input
  TClonesArray* fRichRings;      // input
  TClonesArray* fTofHits;        // input
  TClonesArray* fGlobalTracks;   // output

  /** Verbosity level **/
  Int_t fVerbose;

  /** Counters **/
  Int_t fNAll;
  Int_t fNStsOnly;
  Int_t fNTrdOnly;
  Int_t fNWithRich;
  Int_t fNWithTof;


  CbmFindGlobalTracks(const CbmFindGlobalTracks&);
  CbmFindGlobalTracks& operator=(const CbmFindGlobalTracks&);

  ClassDef(CbmFindGlobalTracks,1);

};

#endif
