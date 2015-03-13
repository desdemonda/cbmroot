#include "CbmAnaHypPair.h"

ClassImp(CbmAnaHypPair)

CbmAnaHypPair::CbmAnaHypPair() 
: TObject(),
  fId1(0),
  fId2(0),
  fMc1(0),
  fMc2(0),
  fB1(0.),
  fB2(0.),
  fChi1(0.),
  fChi2(0.),
  fChi(0.),
  fDca(0.),
  fPca(0.),
  fBmo(0.),
  fMass(0.),
  fMK0(0.),
  fAlpha(0.),
  fPtt(0.),
  fSig(0)
{
}

CbmAnaHypPair::CbmAnaHypPair(Int_t id1, Int_t id2,
                             Int_t mc1, Int_t mc2,
                             Float_t b1, Float_t b2,
                             Float_t chi1, Float_t chi2,
                             Float_t chi, Float_t dca,
                             Float_t pca, Float_t bmo,
                             Float_t mass, Float_t mK0,
                             Float_t alpha, Float_t ptt,
                             Int_t sig)
  : TObject(),
    fId1(id1),
    fId2(id2),
    fMc1(mc1),
    fMc2(mc2),
    fB1(b1),
    fB2(b2),
    fChi1(chi1),
    fChi2(chi2),
    fChi(chi),
    fDca(dca),
    fPca(pca),
    fBmo(bmo),
    fMass(mass),
    fMK0(mK0),
    fAlpha(alpha),
    fPtt(ptt),
    fSig(sig)
{
}

CbmAnaHypPair::~CbmAnaHypPair() {}

