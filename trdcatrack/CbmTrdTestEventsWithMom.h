// -------------------------------------------------------------------------
// -----                  CbmTrdTestEventsWithMom header file          -----
// -----                  Created 26/09/06  by M. Krauze               -----
// -------------------------------------------------------------------------


/** CbmTrdTestEventsWithMom.h
 *@author M. Krauze <meszik@nuph.us.edu.pl>
 **
 ** Task class for matching a reconstructed CbmTrdTrack with a simulated
 ** CbmMCTrack. The matching criterion is a maximal number of common
 ** hits/points. The task fills the data class CbmTrdTrackMatch for
 ** each CbmTrdTrack.
 **/


#ifndef CBMTRDTESTEVENTSWITHMOM_H
#define CBMTRDTESTEVENTSWITHMOM_H 1


#include "FairTask.h"

#include <map>

class CbmTrdTrack;
class CbmTrdHit;
class CbmTrdPoint;
class CbmKF;
class CbmKFTrack;
class CbmTrdTrackFitterKF_CA;
class CbmGeoTrdPar;

class FairTrackParam;

class TClonesArray;
class TH1F;
class TH2F;
class TLorentzVector;
class TVector3;

class CbmTrdTestEventsWithMom : public FairTask
{

 public:

  /** Default constructor **/
  CbmTrdTestEventsWithMom();


  /** constructor with verbosity level **/
  CbmTrdTestEventsWithMom(Int_t verbose);


  /** Constructor with name, title and verbosity

   **
   *@param name     Name of taks
   *@param title    Title of task   (default FairTask)
   *@param verbose  Verbosity level (default 1)
   **/
  CbmTrdTestEventsWithMom(const char* name, const char* title = "FairTask",
			  Int_t verbose = 1, Double_t RF=95., 
			  Double_t chi=2., Double_t PtThr=0.5);


  /** Destructor **/
  virtual ~CbmTrdTestEventsWithMom();


  /** Intialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Execution **/
  virtual void Exec(Option_t* opt);


  /** Finishing */
  virtual void Finish();


  Int_t pdgCharge(Int_t pdgCode); //converts pdg code into '+' or '-' charge

  Bool_t Rejection(Double_t Procent, Bool_t &accept, Int_t num, Int_t pdg); //decides whether to reject a particle or not, based on a random choice

  Double_t GetInvMass(TLorentzVector &pos, TLorentzVector &neg); //self-explanating name

  TVector3 backtrack(CbmTrdPoint* trd0,CbmTrdPoint* trd8, Double_t z);

  TVector3 backtrack(FairTrackParam* paramFirst, FairTrackParam* paramLast, Double_t z);

  Double_t prosta(TVector3 v0, TVector3 v1, int w0, int w1);  //wo, w1 - numery wspolrzednych wektora ktore chcemy uzyc

  Double_t AngleBetweenVectors(TVector3 v1, TVector3 v2);

  Double_t AngleBetweenVectors(TLorentzVector* vPos, TLorentzVector* vNeg);

  TVector3 GetMCMomentum(Int_t trdRecoTrackID);


  void ShowMCDistance();

  Double_t EstimateMomentum(CbmTrdTrack *trdTrack, Double_t Bydl);
  Double_t GetBydl(CbmTrdTrack *trdTrack, Double_t pMC, Int_t charge);


  Double_t GetKickPt(CbmTrdTrack *trdTrack);

  Double_t GetKickPt(CbmTrdPoint *trdPoint);

  void CreateHistogramms();

  void WriteHistogramms();

  Int_t fArr[100];
  Int_t fArrB[100];

  TH1F
    *fRejInvMass;

  TH1F
    *fNormRejInvMass;


  TH1F
    *fRejNew,
    *fEffNew;

  TH2F
    *fRejInvMass2d;


  inline void SetPtThr(Double_t iPtThr) { fPtThr = iPtThr; }
  inline void SetRF(Double_t iRF)       { fRF = iRF; }
  inline void SetLowerMassThr(Double_t iLowerMassThr) { fLowerMassThr = iLowerMassThr; }
  inline void SetUpperMassThr(Double_t iUpperMassThr) { fUpperMassThr = iUpperMassThr; }
  inline void SetChi(Double_t iChi) { fChi = iChi; }
  inline void SetChi2(Double_t iChi) { fChi2 = iChi; }
  inline void SetNevents(Int_t iNevents) { fNevents = iNevents; }
  inline void SetPrimOnly(Int_t iPrimOnly) { fPrimOnly = iPrimOnly; }
  inline Int_t  GetNevents() { return fNevents; }



  CbmGeoTrdPar* TrdPar;
   

  Int_t
    fTrueJPsiEvents,
    fPassedJpsiEvents;
  

  Double_t
    fTrd11_Z,
    fTrd13_Z,     
    fTrd14_Z,
    fTrd21_Z,
    fTrd24_Z,
    fTrd31_Z,
    fTrd34_Z;



   TH1F
     *fMomRes0,
     *fMomRes05,
     *fMomRes10,
     *fMomRes15,
     *fMomRes20,
     *fMomRes25,
     *fMomRes30,
     *fMomRes35,
     *fMomRes40,
     *fMomRes45,
     *fMomRes50;





  TH1F
    *fInvMassRes;

  TH1F
    *fBydlRes,
    *fBydl,
    *fPointBydlRes,
    *fPointBydl;
  

  TH1F 
    *invMass,
    *invMassMC,
    *invMassAfter,
    *invMassSmeared;
   
   TH1F 
     *fRvsCentr,
     *fAllvsCentr;
   
   
  TH1F *fMCDistY0;
  TH1F *fRecoDistY0;


  TH1F *fMCDistX1;

  TH1F *fCombDistY0;
  TH1F *fCombDistX1;

  TH2F *fPtVsY;

  TH1F *fPtAngle;
  TH1F *fOpeningAngle;

  TH1F *fdPx;
  TH1F *fdPy;
  TH1F *fdPz;

  TH1F *fTxDist;
  TH1F *fTxPos;
  TH1F *fTxNeg;

  TH1F *fMomDist;
  TH1F *fPull;
  TH1F *fPRecoMc;
  
  TH1F 
    *fPMc,
    *fPMC,
    *fPtMC,
    *fPReco,
    *fPtReco;

  TH1F 
    *fPassedPMC,
    *fPassedPtMC,
    *fPassedPReco,
    *fPassedPtReco;

  TH1F
    *fRes1,
    *fPull1,
    *fRes2,
    *fPull2,
    *fRes3,
    *fPull3,
    *fRes4,
    *fPull4;

  TH1F
    *fPtRes;

  TH2F
    *fPtVsY_Before,
    *fPtVsY_Pt,
    *fPtVsY_XZ,
    *fPtVsY_YZ,
    *fPtVsY_Angle;
  
  TH2F
    *fQvsQ;

  TH2F
    *fHitsInLayer1;

  TH1F
    *fNoElecPerEvent,
    *fNoPositPerEvent,
    *fNoPPionPerEvent,
    *fNoNPionPerEvent,
    *fNoProtonPerEvent,
    *fNoPosTracks,
    *fNoNegTracks,
    *fNoAllPions;

  TH1F
    *fNoPassedProton,
    *fNoPassedPPion,
    *fNoPassedNPion,
    *fNoPassedPosTracks,
    *fNoPassedNegTracks,
    *fNoPassedAllPions,
    *fNoPassedPosButPositrons,
    *fNoPassedNegButElectrons;


  TH1F
    *fPdgCodes;

  TH1F
    *fChiDist,  //quality of vertex fit
    *fChi2Dist; //quality of track fit
  
  Int_t
    noPosPart,
    noNegPart,
    fNoTotCrossedX,
    fNoCrossedX;


  std::map<Int_t, Int_t> totParticlesSum;
  std::map<Int_t, Int_t> totPassedParticlesSum;

 private:

  TClonesArray* fTracks;       // Array of CbmTrdTracks
  TClonesArray* fPoints;       // Array of FairMCPoints
  TClonesArray* fHits;         // Array of CbmTrdHits
  TClonesArray* fMatches;      // Array of CbmTrdTrackMatch
  TClonesArray* fMCTracks;
  TClonesArray* fArrayKFTrdHit; //array of Kalman Filter hits
  
  /** Map from MCTrackID to number of common hits **/
  std::map<Int_t, Int_t> fMatchMap;
  
  
  
  CbmTrdTrackFitterKF_CA* trdTrackFitter;
  CbmTrdTrackFitterKF_CA* trdTrackFitterKF;
  
  
  /** Verbosity level **/
  Int_t fVerbose;
  
  Int_t fCurrentEvent; //number of a currently processed event
  
  Int_t fPassedEvents;  //number of a total events accepted by a selection
 
  Double_t fRF;  //hadron rejection factor
  Double_t fChi; //value to discard secondary tracks
  Double_t fChi2; //value to discard secondary tracks
  Double_t fPtThr; //value of a Pt Threshold for each particle
  Double_t
    fLowerMassThr,
    fUpperMassThr;

  

  Int_t
    //    fPassedEvents,
    fNevents,
    fPrimOnly;

  CbmTrdTestEventsWithMom(const CbmTrdTestEventsWithMom&);
  CbmTrdTestEventsWithMom& operator=(const CbmTrdTestEventsWithMom&);

  ClassDef(CbmTrdTestEventsWithMom,1);

};

#endif
