#ifndef CBMANAHYPSECFINDERQA_H
#define CBMANAHYPSECFINDERQA_H

#include "CbmAnaHypSecFinder.h"

class CbmAnaHypTools;
class TClonesArray;
class TH1D;
class TH2D;

class CbmAnaHypSecFinderQa : public CbmAnaHypSecFinder
{
 public:
  CbmAnaHypSecFinderQa();
  CbmAnaHypSecFinderQa(char* name, Int_t iVerbose);
  virtual ~CbmAnaHypSecFinderQa();
  virtual InitStatus Init();
  virtual void Exec(Option_t* option);
  void PrintEventStatistics();
  void PrintMCTrackInfo();
  void PrintHitInfo();
  void AddHitRefTracks();
  void Analyse();
  void WriteHisto(TString fileName);
  void DrawHisto();
  void Performance();
  void AnalyseDoubleStrips();
  void AddNextStationAna(CbmKFTrackSec &track, Int_t st);
  void AnalysePulls();
  Int_t CreateStsTrackAna(CbmKFTrackSec track);
 protected:
  CbmAnaHypTools* fTools;
  TClonesArray* fTrackMatch;

  TH1D* hTxB[10];
  TH1D* hTyB[10];
  TH1D* hTxS[10];
  TH1D* hTyS[10];
  TH1D* hDtxB[10];
  TH1D* hDtyB[10];
  TH1D* hDtxS[10];
  TH1D* hDtyS[10];
  TH1D* hChi2S;
  TH1D* hChi2B;
  TH1D* hPS;
  TH1D* hPB;

  TH1D* hDx[10];
  TH1D* hDy[10];

  TH1D* hDxPull[10];
  TH1D* hDyPull[10];

  TH1D* hPullX;
  TH1D* hPullY;
  TH1D* hPullQp;


  TH2D* hQpDtx;

  Int_t fEvent;
  Int_t fFinder;
  Int_t fFound;
  Int_t fAll;
  Int_t fSig;

  Int_t fSigD;
  Int_t fFakeD;

  Double_t fL1AC;
  Int_t fRC,fRC_good,fRC_fake;
  Int_t fSR,fSR_good,fSR_fake;
  Int_t fAR,fAR_good,fAR_fake;

 private:

  CbmAnaHypSecFinderQa(const CbmAnaHypSecFinderQa&);
  CbmAnaHypSecFinderQa operator=(const CbmAnaHypSecFinderQa&);

  ClassDef(CbmAnaHypSecFinderQa,1);
};



#endif
