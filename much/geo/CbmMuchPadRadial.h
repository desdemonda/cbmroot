/** CbmMuchPadRadial.h
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 6.02.12
 *@version 1.0
 **
 ** Class describing a single radial pad
 **/
#ifndef CBMMUCHPADRADIAL_H
#define CBMMUCHPADRADIAL_H 1

#include "CbmMuchPad.h"
#include "TCrown.h"
#include "CbmMuchSectorRadial.h"

class CbmMuchPadRadial : public CbmMuchPad, public TCrown {

public:
  CbmMuchPadRadial();
  /**
   * Standard constructor
   * @param sector   Sector which this pad belongs to
   * @param iChannel Channel index corresponding to this pad
   */
//  CbmMuchPadRadial (CbmMuchSectorRadial* sector, Int_t iChannel);
  
  CbmMuchPadRadial(Int_t address, Double_t r1, Double_t r2, Double_t phi1, Double_t phi2);
  
  ~CbmMuchPadRadial() {}
  Double_t GetPhi1() const { return fPhi1; }
  Double_t GetPhi2() const { return fPhi2; }
  void DrawPad();
  void SetFired(Int_t iDigi, Int_t ADCcharge, Int_t nADCChannels=256);
private:
  Double_t fPhi1;
  Double_t fPhi2;
  ClassDef(CbmMuchPadRadial,1);
};
#endif
