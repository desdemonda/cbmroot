/** CbmAnaDimuonAnalysisDraw.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-01-02
 **
 **/

#ifndef CBMANADIMUONHISTO_H_
#define CBMANADIMUONHISTO_H_ 1

#include "FairTask.h"
#include "TString.h"

class CbmAnaDimuonHisto;
class CbmAnaMuonCandidate;
class TClonesArray;
class TH1D;
class TH2D;
class TH3D;
class TLorentzVector;
class TChain;

class CbmAnaDimuonHisto : public FairTask{
public:
  CbmAnaDimuonHisto();
  virtual ~CbmAnaDimuonHisto(){}
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();

  inline void SetMbinning(Double_t min, Double_t max, Int_t nBins) {
    fMmin=min;
    fMmax=max;
    fMbins=nBins;
  }
  
  inline void SetMuchHitsCut(Int_t nHits) {fMuchHitsCut=nHits; }
  inline void SetStsHitsCut(Int_t nHits) {fStsHitsCut=nHits; }
  inline void SetChiToVertexCut(Double_t chi) {fChiToVertexCut=chi; }

  inline void SetMcuts(Double_t mMinCut, Double_t mMaxCut) {
    fMminCut = mMinCut;
    fMmaxCut = mMaxCut;
  }
  inline void SetMultiplicity(Double_t mult) { fMultiplicity = mult; }
  inline void SetBranching(Double_t b) { fBranching = b; }
  inline void SetSignalPairs(Int_t n) {fSignalPairs = n; }
  inline void SetNoMixedEv(Int_t n) {fNoMixedEv = n; }
  
private:
  Int_t         fEvent;             //!
  TClonesArray* fMuCandidates;      //!
  TClonesArray* fDimuonCandidates;  //!
  TChain* fChain;                   //!

  TH1D* fDimuonMmc;
  TH1D* fDimuonMrc;
  TH1D* fBgdM;
  
  Double_t fMmin;
  Double_t fMmax;
  Int_t fMbins;
  Double_t fMminCut;
  Double_t fMmaxCut;

  Int_t fMuchHitsCut;
  Int_t fStsHitsCut;
  Double_t fChiToVertexCut;
  Double_t fMultiplicity;
  Double_t fBranching;
  Int_t fSignalPairs;
  Int_t fNoMixedEv;
  Int_t fNEvents;

  CbmAnaDimuonHisto(const CbmAnaDimuonHisto&);
  CbmAnaDimuonHisto operator=(const CbmAnaDimuonHisto&);

  ClassDef(CbmAnaDimuonHisto,1);
};

#endif
