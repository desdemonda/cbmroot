#ifndef CBMANAHYPPAIR_H
#define CBMANAHYPPAIR_H

#include "TObject.h"

class CbmAnaHypPair : public TObject
{

 public:

  CbmAnaHypPair();
  CbmAnaHypPair(Int_t id1, Int_t id2,
                 Int_t mc1, Int_t mc2,
                 Float_t b1, Float_t b2,
                 Float_t chi1, Float_t chi2,
                 Float_t chi, Float_t dca,
                 Float_t pca, Float_t bmo,
                 Float_t mass, Float_t mK0,
                 Float_t alpha, Float_t ptt,
                 Int_t sig);
  virtual ~CbmAnaHypPair();

  Int_t fId1;
  Int_t fId2;
  Int_t fMc1;
  Int_t fMc2;
  Float_t fB1;
  Float_t fB2;
  Float_t fChi1;
  Float_t fChi2;
  Float_t fChi;
  Float_t fDca;
  Float_t fPca;
  Float_t fBmo;
  Float_t fMass;
  Float_t fMK0;
  Float_t fAlpha;
  Float_t fPtt;
  Int_t fSig;

  ClassDef(CbmAnaHypPair,3);

};

#endif
