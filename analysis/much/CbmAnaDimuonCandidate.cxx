#include "CbmAnaDimuonCandidate.h"
#include "CbmAnaMuonCandidate.h"
#include "CbmStsTrack.h"
#include "CbmKFTrack.h"
#include "TLorentzVector.h"

CbmAnaDimuonCandidate::CbmAnaDimuonCandidate()
  : TObject(),
    fMuPlus(),
    fMuMinus()
{  
}

CbmAnaDimuonCandidate::CbmAnaDimuonCandidate(
    CbmAnaMuonCandidate muPlus, 
    CbmAnaMuonCandidate muMinus)
  : TObject(),
    fMuPlus(CbmAnaMuonCandidate(muPlus)),
    fMuMinus(CbmAnaMuonCandidate(muMinus))
{  
}

TLorentzVector CbmAnaDimuonCandidate::GetMomentumMC(){
  TLorentzVector* p1 = fMuPlus.GetMomentumMC();
  TLorentzVector* p2 = fMuMinus.GetMomentumMC();
  TLorentzVector p = (*p1)+(*p2);
  return p;
}

TLorentzVector CbmAnaDimuonCandidate::GetMomentumRC(){
  TLorentzVector* p1 = fMuPlus.GetMomentumRC();
  TLorentzVector* p2 = fMuMinus.GetMomentumRC();
  TLorentzVector p = (*p1)+(*p2);
  return p;
}


ClassImp(CbmAnaDimuonCandidate);
