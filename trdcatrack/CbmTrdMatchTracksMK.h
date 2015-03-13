// -------------------------------------------------------------------------
// -----                  CbmTrdMatchTracksMK header file              -----
// -----                  Created 20/02/06  by M. Krauze               -----
// -------------------------------------------------------------------------


/** CbmTrdMatchTracksMK.h
 *@author M.Krauze <meszik@nuph.us.edu.pl>
 **
 ** Task class for matching a reconstructed CbmTrdTrack with a simulated
 ** CbmMCTrack. The matching criterion is a maximal number of common
 ** hits/points. The task fills the data class CbmTrdTrackMatch for
 ** each CbmTrdTrack.
 **/


#ifndef CBMTRDMATCHTRACKSMK_H
#define CBMTRDMATCHTRACKSMK_H 1

#include "FairTask.h"

#include <map>

using std::map;

class TClonesArray;
class TH1F;
class TH2F;
class TProfile;
class CbmTrdTrack;

class CbmTrdMatchTracksMK : public FairTask
{
 public:

  /** Default constructor **/
  CbmTrdMatchTracksMK();

  /** Constructor with verbosity level **/
  CbmTrdMatchTracksMK(Int_t verbose);

  /** Destructor **/
  virtual ~CbmTrdMatchTracksMK();

  /** Intialisation at beginning of each event **/
  virtual InitStatus Init();

  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Finishing */
  virtual void Finish();
  
  /** Function for writing histogramms **/
  void WriteHistogramms();

  map<Int_t, Int_t> MC_noPrimRefAll;
  map<Int_t, Int_t> RECO_noPrimRefAll;

  Int_t
    NO_TOTAL_MC_REF_TRACKS,
    NO_TOTAL_RECO_TRACKS,
    NO_TOTAL_CORREC_TRECO_TRACKS,
    NO_TOTAL_GHOST_TRACKS,
    NO_TOTAL_CLONE_TRACKS,
    NO_TOTAL_EVENTS;

  Int_t
    NO_TOTAL_CLONES_PRIM_FAST,
    NO_TOTAL_CLONES_PRIM_SLOW,
    NO_TOTAL_CLONES_FAST,
    NO_TOTAL_CLONES_SLOW;



  Int_t
    RECO_Tot_PrimaryRefFast,
    RECO_Tot_PrimaryRefSlow,
    RECO_Tot_PrimaryRefAll,
  
    RECO_Tot_AllRefFast,
    RECO_Tot_AllRefSlow,
    RECO_Tot_AllRefAll,

    RECO_Tot_ExtraRefFast,
    RECO_Tot_ExtraRefSlow,
    RECO_Tot_ExtraRefAll;
  
  Int_t
    MC_Tot_PrimaryRefFast,
    MC_Tot_PrimaryRefSlow,
    MC_Tot_PrimaryRefAll,

    MC_Tot_AllRefFast,
    MC_Tot_AllRefSlow,   
    MC_Tot_AllRefAll,

    MC_Tot_ExtraRefFast,
    MC_Tot_ExtraRefSlow,
    MC_Tot_ExtraRefAll;


 private:

  TClonesArray* fTracks;       // Array of CbmTrdTracks
  TClonesArray* fPoints;       // Array of FairMCPoints
  TClonesArray* fHits;         // Array of CbmTrdHits
  TClonesArray* fMatches;      // Array of CbmTrdTrackMatch
  TClonesArray* fMCTracks;     // Array of CbmMCTracks


  /** Map from MCTrackID to number of common hits **/
  map<Int_t, Int_t> fMatchMap;

  /** Verbosity level **/
  Int_t fVerbose;

  //Number of hits used to create a tracklet
  Int_t fNoHitsPerTracklet;  
  //Declare some histogramms

  //  TProfile* fEffVsMom;
  TProfile* fEffVsMom;
  TH1F* fh_EffVsMom;
  TH1F* fh_mom_trdtrack_good;
  TH1F* fh_mom_mctrack_reco;


  TH1F* fNoHitsPerTrack;
  TH1F* fNoTrueHits;
  TH1F* fNoTracks;
  TH1F* fNoPrimHighMom;
  TH1F* fNoPrimLowMom;
  TH1F* fPartTy;

  TH1F* fGhostTracks_meanZ;

  TH1F* fXDistShort14;
  TH1F* fYDistShort14;

  TH1F* fXDistShortGh14;
  TH1F* fYDistShortGh14;

  TH1F* fXDistLong45;
  TH1F* fYDistLong45;

  TH1F* fXDistLongGh45;
  TH1F* fYDistLongGh45;

  TH1F* fXDistShort58;
  TH1F* fYDistShort58;

  TH1F* fXDistShortGh58;
  TH1F* fYDistShortGh58;

  TH1F* fXDistLong89;
  TH1F* fYDistLong89;

  TH1F* fXDistLongGh89;
  TH1F* fYDistLongGh89;

  TH1F* fXDistShort912;
  TH1F* fYDistShort912;

  TH1F* fXDistShortGh912;
  TH1F* fYDistShortGh912;

  TH1F *fh_Chi2_Ghost;
  TH1F *fh_Chi2_CorrectFast;
  TH1F *fh_Chi2_CorrectSlow;

  TH1F *fh_Tx_Ghost;
  TH1F *fh_Tx_CorrectFast;
  TH1F *fh_Tx_CorrectSlow;

  TH1F *fh_Ty_Ghost;
  TH1F *fh_Ty_CorrectFast;
  TH1F *fh_Ty_CorrectSlow;

  TH1F *fh_Y_Ghost;
  TH1F *fh_Y_CorrectFast;
  TH1F *fh_Y_CorrectSlow;

  TH1F *fh_X_Ghost;
  TH1F *fh_X_CorrectFast;
  TH1F *fh_X_CorrectSlow;
   /** Function for creating histogramms **/

  void CreateHistogramms();

  Int_t GetMCIndex(CbmTrdTrack* trdTrack);

  Double_t DistTracklet(TClonesArray* fHit,
			Int_t iIndexFirst,
			Int_t iIndexSecond);

  Double_t DistTrackletCutX(TClonesArray* trdMCPoint,
			   Int_t iIndexFirst,
			   Int_t iIndexSecond);

  CbmTrdMatchTracksMK(const CbmTrdMatchTracksMK&);
  CbmTrdMatchTracksMK& operator=(const CbmTrdMatchTracksMK&);

  ClassDef(CbmTrdMatchTracksMK,1);

};

#endif
