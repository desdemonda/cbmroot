#ifndef CBMANADIMUONCANDIDATE_H
#define CBMANADIMUONCANDIDATE_H

#include "CbmAnaMuonCandidate.h"
class TLorentzVector;

class CbmAnaDimuonCandidate : public TObject{
  public:
    CbmAnaDimuonCandidate();
    CbmAnaDimuonCandidate(CbmAnaMuonCandidate muPlus, CbmAnaMuonCandidate muMinus);
    CbmAnaMuonCandidate* GetMuPlus()  { return &fMuPlus;  }
    CbmAnaMuonCandidate* GetMuMinus() { return &fMuMinus; }
    CbmAnaMuonCandidate* GetMu(Bool_t sign) { return sign ? &fMuPlus: &fMuMinus; }
    TLorentzVector GetMomentumMC();
    TLorentzVector GetMomentumRC();
    inline Bool_t IsReconstructed(Int_t nMuchHitsCut=13, Int_t nStsHitsCut=8, Double_t chiToVertexCut=3.){ 
      return fMuPlus.IsReconstructed(nMuchHitsCut,nStsHitsCut,chiToVertexCut)*
             fMuMinus.IsReconstructed(nMuchHitsCut,nStsHitsCut,chiToVertexCut); 
    }
    virtual ~CbmAnaDimuonCandidate(){};
  private:
  CbmAnaMuonCandidate fMuPlus;
  CbmAnaMuonCandidate fMuMinus;
  ClassDef(CbmAnaDimuonCandidate,1);
};

#endif
