/** CbmMuchPadRectangular.h
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 02.08.12
 *@version 2.0
*
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 10.11.07
 *@version 1.0
 **
 ** Class describing a standalone pad
 **/
#ifndef CBMMUCHPADRECTANGULAR_H
#define CBMMUCHPADRECTANGULAR_H 1

#include "CbmMuchPad.h"
#include "TArrayL64.h"
#include "TPave.h"


class CbmMuchPad;
class CbmMuchSector;
class CbmMuchDigi;
class CbmMuchDigiMatch;

#include "vector"
using std::vector;

class CbmMuchPadRectangular : public CbmMuchPad, public TPave {
public:
  CbmMuchPadRectangular();
  CbmMuchPadRectangular(Int_t address, Double_t x0, Double_t y0, Double_t lx, Double_t ly);
  virtual ~CbmMuchPadRectangular(){};
  void DrawPad();
  void SetFired(Int_t iDigi, Int_t ADCcharge, Int_t nADCChannels=256);
protected:
  ClassDef(CbmMuchPadRectangular,1)
};
#endif
