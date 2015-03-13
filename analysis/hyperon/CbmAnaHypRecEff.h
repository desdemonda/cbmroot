#ifndef CBMANAHYPRECEFF_H
#define CBMANAHYPRECEFF_H

#include "FairTask.h"

class TClonesArray;
class TH1D;
class TH2D;

class CbmAnaHypRecEff : public FairTask
{
 public:
  CbmAnaHypRecEff();
  CbmAnaHypRecEff(Int_t pdgCode, Int_t iVerbose);
  virtual ~CbmAnaHypRecEff();
  virtual InitStatus Init();
  virtual void Exec(Option_t* option);
  virtual void Finish();

  private:
  TClonesArray* fMCTracks;
  TClonesArray* fRCTracks;
  TClonesArray* fRCMatch;

  Int_t fMoPdg;
  Int_t fDaPdg;

  Int_t fEvent;

  TH1D* hEventCounter;
  TH2D* hMC;
  TH2D* hAC;
  TH2D* hRC;
  TH1D* hACpr;
  TH1D* hRCpr;
  TH1D* hACpi;
  TH1D* hRCpi;

  CbmAnaHypRecEff(const CbmAnaHypRecEff&);
  CbmAnaHypRecEff operator=(const CbmAnaHypRecEff&);

  ClassDef(CbmAnaHypRecEff,1);
};

class Hyperon {
 public:
    Hyperon()
      : daAC(0), piAC(0), prAC(0), daRC(0), piRC(0),
    prRC(0), y(0), pt(0) {}
    ~Hyperon(){};
    Int_t daAC;
    Int_t piAC;
    Int_t prAC;
    Int_t daRC;
    Int_t piRC;
    Int_t prRC;
    Double_t y;
    Double_t pt;
};




#endif
