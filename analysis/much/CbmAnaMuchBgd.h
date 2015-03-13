#ifndef CBMANAMUCHBGD_H_
#define CBMANAMUCHBGD_H_ 1

#include "FairTask.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "TH1D.h"
#include "CbmStsKFTrackFitter.h"
#include "vector"
class TTree;

using std::vector;
struct CbmMuon{
  Char_t fChunk;
  Short_t fEvent;
  Short_t fMuons;
  Char_t  fSign;
  TLorentzVector fP;
  Char_t  fMuchHits;
  Char_t  fStsHits;
  Float_t fChi;
CbmMuon() : fChunk(0), fEvent(0), fMuons(0), fSign(0), fP(),
    fMuchHits(0), fStsHits(0), fChi(0.) {;}
};

class CbmAnaMuchBgd : public FairTask{
public:
  CbmAnaMuchBgd();
  virtual ~CbmAnaMuchBgd(){}
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  virtual void SetParContainers();
private:
  Int_t fEvent;
  TClonesArray* fMCTracks;
  TClonesArray* fStsTracks;
  TClonesArray* fMuchTracks;
  TClonesArray* fGlobalTracks;
  CbmVertex*    fVertex;
  CbmStsKFTrackFitter* fFitter;
  vector<CbmMuon> fMuons;
  TH1D* fM;
  Int_t fNAll;
  Int_t fNSts;
  Int_t fNStsChi;
  Int_t fNStsChiMuch13;
  Int_t fNStsChiMuch15;
  Int_t fNoMCTracks;
  Int_t fNoStsTracks;
  Int_t fNoGlobalTracks;
  Int_t fNoMuchTracks;
  TTree* fMuonTree;
  CbmMuon fMuon;
  CbmAnaMuchBgd(const CbmAnaMuchBgd&);
  CbmAnaMuchBgd operator=(const CbmAnaMuchBgd&);
  ClassDef(CbmAnaMuchBgd,1);
};

#endif
