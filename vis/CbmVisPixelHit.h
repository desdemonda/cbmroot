// -------------------------------------------------------------------------
// -----                      CbmVisPixelHit header file               -----
// -----                  Created 03/06/09  by E. Kryshen              -----
// -------------------------------------------------------------------------

/** CbmVisPixelHit
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 14.03.07
 *@version 1.0
 **
 **/




#ifndef CBMVISPIXELHIT_H
#define CBMVISPIXELHIT_H 1

#include "TMarker.h"
class CbmPixelHit;
class CbmMCTrack;
class CbmVisHit;

class CbmVisPixelHit : public TMarker, public CbmVisHit{
public:
  CbmVisPixelHit(CbmPixelHit* hit);
  ~CbmVisPixelHit(){};
  void SetVisible(Bool_t isVisible);
  CbmPixelHit* GetHit() {return fHit; }
  TString GetInfo();

private:
  CbmPixelHit* fHit;

  ClassDef(CbmVisPixelHit,1);
};
#endif
