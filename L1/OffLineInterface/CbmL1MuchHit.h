#ifndef CBM_L1_MuchHit_h
#define CBM_L1_MuchHit_h

#include "CbmKFHit.h"
#include "CbmKFPixelMeasurement.h"

class CbmMuchHit;
class CbmKFTrackInterface;

class CbmL1MuchHit : public CbmKFHit
{
 public:

  CbmL1MuchHit(){}
  CbmL1MuchHit( CbmMuchHit *h, int index ){ Create(h, index); }
  ~CbmL1MuchHit(){}

  void Create( CbmMuchHit *h, int index );  
  Int_t Filter( CbmKFTrackInterface &track, Bool_t downstream, Double_t &QP0 );

  CbmKFPixelMeasurement FitPoint;

  int index;
  int iStation;
  Double_t time;
  bool busy;

 public:
  ClassDef(CbmL1MuchHit, 1);
};

#endif
