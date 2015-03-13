// -------------------------------------------------------------------------
// -----                         CbmTrdPhotonAnalysis header file      -----
// -----                        Created 13/02/12  by C.Bergmann        -----
// -------------------------------------------------------------------------



#ifndef CBMTRDPHOTONANALYSIS_H
#define CBMTRDPHOTONANALYSIS_H

#include "FairTask.h"
#include "TH2I.h"
#include "TH3I.h"
#include <vector>
#include <map>
#include "CbmMCTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmVertex.h"
#include "FairTrackParam.h"
#include "CbmRichElectronIdAnn.h"
#include "CbmStsKFTrackFitter.h"
class TClonesArray;
//class CbmStsKFTrackFitter;
class CbmRichElectronIdAnn;
class CbmVertex;
class CbmTrdDigiPar;
class CbmTrdTrack;
class CbmTrdDigi;
class CbmTrdModule;
class CbmRichRing;
class CbmMCTrack;
class CbmGlobalTrack;
class TLegend;
class TProfile;
class TH1;
class TH1I;
//class TH1F;
//class TH1D;
class TH2;
class TH2I;
//class TH2F;
//class TH2D;
//class vector;
//class map;

typedef struct MCParticle
{
  Int_t PID; // PDG Code
  Int_t motherId; // TConeArray ID of the mother 
  //Int_t motherPid; // PDG Code
  std::vector<Int_t> daughterIds; // TConeArray IDs of all daughters
  //std::vector<Int_t> daughterPids; // PDG Code
  Double_t Px;
  Double_t Py;
  Double_t Pz;
MCParticle() : PID(0), motherId(0), /*motherPid(0),*/ daughterIds(), /*daughterPids(),*/ Px(0), Py(0), Pz(0) {};
} MCParticle;

class ElectronCandidate{
public:
   TVector3 position, momentum;
   Double_t mass, energy, rapidity;
   Int_t charge;
   Double_t chi2Prim;
   Double_t chi2sts;
   Int_t McMotherId;
   Int_t stsMcTrackId;
   Int_t richMcTrackId;
   Int_t trdMcTrackId;
   Int_t tofMcTrackId;
   Int_t stsInd;
   Int_t richInd;
   Int_t trdInd;
   Int_t tofInd;
   Bool_t isElectron;
   Bool_t isMcSignalElectron;
   Bool_t isMcPi0Electron;
   Bool_t isMcGammaElectron;
   Bool_t isMcEtaElectron;

   Bool_t isGamma;
   Double_t dSts;
   Bool_t isTtCutElectron;
   Bool_t isStCutElectron;
   Bool_t isApmCutElectron;
   Bool_t isMvd1CutElectron;
   Bool_t isMvd2CutElectron;
   Double_t richAnn;
   Double_t trdAnn;
   Double_t mass2;
};

class KinematicParam{
public:
   Double_t momentumMag; // Absolute value of momentum
   Double_t pt; // Transverse momentum
   Double_t rapidity; // Rapidity
   Double_t minv; // Invariant mass
   Double_t angle; // Opening angle
};

class CbmTrdPhotonAnalysis : public FairTask {
  
 public:
  CbmTrdPhotonAnalysis();
  CbmTrdPhotonAnalysis(const char *name, const char *title="CBM Task", Int_t iVerbose=1);
  virtual ~CbmTrdPhotonAnalysis();


  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();

  void InitHistos();
  void SaveHistosToFile();
  void NormalizeHistos(TH1* h);

  Bool_t IsElec(CbmRichRing* ring, Double_t momentum, CbmTrdTrack* trdTrack, CbmGlobalTrack* gTrack, ElectronCandidate* cand);
  Bool_t IsRichElec(CbmRichRing *ring, Double_t momentum, ElectronCandidate* cand);
  Bool_t IsTrdElec(CbmTrdTrack* trdTrack, ElectronCandidate* cand);
  Bool_t IsTofElec(CbmGlobalTrack* gTrack, Double_t momentum, ElectronCandidate* cand);

  Bool_t VertexInMagnet(CbmMCTrack* track); 
  Bool_t VertexInTarget(CbmMCTrack* track);
  Bool_t CloseByVertex(CbmMCTrack* trackA, CbmMCTrack* trackB, Double_t minimumDistance /*[cm]*/);

  Double_t CalcVertexDistance(CbmMCTrack* trackA, CbmMCTrack* trackB);

  Bool_t PairFromGamma(Int_t firstId, Int_t secondId);
  Bool_t PairFromPi0(Int_t firstId, Int_t secondId);

  Double_t CalcInvariantMass(CbmMCTrack* trackA, CbmMCTrack* trackB);
  Double_t CalcOpeningAngle(CbmMCTrack* trackA, CbmMCTrack* trackB);
  Double_t CalcPt(CbmMCTrack* trackA, CbmMCTrack* trackB);
  Double_t CalcP(CbmMCTrack* trackA, CbmMCTrack* trackB);

  Double_t CalcInvariantMass(ElectronCandidate candA, ElectronCandidate candB);
  Double_t CalcOpeningAngle(ElectronCandidate candA, ElectronCandidate candB);
  Double_t CalcPt(ElectronCandidate candA, ElectronCandidate candB);
  Double_t CalcP(ElectronCandidate candA, ElectronCandidate candB);

  void StatisticHistos();

  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Finish (called after each event) **/
  virtual void FinishEvent();
 
  /** Finish task (called after all event) **/
  virtual void FinishTask();

  void Register();

  Int_t PdgToGeant(Int_t PdgCode);

  //void NiceHisto(TProfile *h, Int_t color, Int_t mStyle, Int_t mSize);
  void NiceHisto1(TH1 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle);
  void NiceHisto2(TH2 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle, TString zTitle);
  void NiceHisto3(TH3 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle, TString zTitle);
  void NiceProfile(TProfile *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle);
  void NiceLegend(TLegend *l);
  void Statusbar(Int_t i, Int_t n);
  void CalcDeltaInvMassSpectrum(TString name);
 private:
  CbmTrdPhotonAnalysis(const CbmTrdPhotonAnalysis&);
  CbmTrdPhotonAnalysis& operator=(const CbmTrdPhotonAnalysis&);

  TClonesArray*     fMCTracks; 
  TClonesArray*     fTrdPoints;      /**  array of Trd MC points **/
  TClonesArray*     fTrdDigis;       /**  array of CbmTrdDigi    **/
  TClonesArray*     fTrdClusters;    /**  array of CbmTrdCluster **/
  TClonesArray*     fTrdHits;        /**  array of CbmTrdHit     **/
  TClonesArray*     fTrdTracks;      /**  CbmTrdTrack array      **/
  TClonesArray*     fTrdTrackMatches;
  TClonesArray*     fGlobalTracks;   /**  CbmGlobalTrack array   **/
  TClonesArray*     fPrimaryVertex;  /**  PrimaryVertex  array   **/
  TClonesArray *fRichRings;
  TClonesArray *fRichProj;
  TClonesArray *fRichPoints;
  TClonesArray *fRichRingMatches;
  TClonesArray *fRichHits;
  TClonesArray *fStsTracks;
  TClonesArray *fStsTrackMatches;
  TClonesArray *fStsHits;
  TClonesArray *fMvdHits;   
  TClonesArray *fTofHits;
  TClonesArray *fTofPoints;
  CbmStsKFTrackFitter *fKFFitter;
  //CbmRichElectronIdAnn* fElIdAnn;
  CbmVertex *fPrimVertex;


  //FairTrackParam* fParamFirst;
  //FairTrackParam* fParamLast;

  CbmTrdDigiPar  *fDigiPar;
  CbmTrdModule   *fModuleInfo;
  std::vector<ElectronCandidate> fGTElectronCandidates;
  std::vector<ElectronCandidate> fGTPositronCandidates;
  std::vector<ElectronCandidate> fGTCandidates;
  std::vector<ElectronCandidate> fCandidates;
  std::vector<ElectronCandidate> fElectronCandidates;
  std::vector<ElectronCandidate> fPositronCandidates;

  // ID cuts
  Double_t fTrdAnnCut;
  Double_t fTrdLikeCut;
  Double_t fTrdWknCut;
  Double_t fRichAnnCut;
  Int_t fRichHitsMinCut;
  Int_t fRichHitsMaxCut;
  Double_t fRichMinRadiusCut;
  Double_t fRichMaxRadiusCut;
  Double_t fMeanA;
  Double_t fMeanB;
  Double_t fRmsA;
  Double_t fRmsB;
  Double_t fRmsCoeff;
  Double_t fDistCut;
  CbmRichElectronIdAnn* fElIdAnn;
  Bool_t fUseRichAnn;
  // Analysis cuts
  Double_t fChiPrimCut;
  Double_t fPtCut;
  Double_t fAngleCut;
  Double_t fGammaCut;
  Double_t fStCutAngle;
  Double_t fStCutPP;
  Double_t fTtCutAngle;
  Double_t fTtCutPP;
  Double_t fMvd1CutP;
  Double_t fMvd1CutD;
  Double_t fMvd2CutP;
  Double_t fMvd2CutD;

  //Histos

  std::map<TString, TH1*> fHistoMap;
  std::map<TString, TH1*>::iterator fHistoMapIt;
  
  // Birth Vertex
  TH3I*  fMotherDaughterZBirth;
  TH3I*  fBirthGamma;
  TH3I*  fBirthPi0;
  TH3I*  fBirthPair;
  
  ClassDef(CbmTrdPhotonAnalysis,2)
    };


#endif //CBMTRDPHOTONANALYSIS_H
