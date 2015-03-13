// -------------------------------------------------------------------------
// -----                     CbmVisStripHit header file                -----
// -----                  Created 03/06/09  by E. Kryshen              -----
// -------------------------------------------------------------------------

/** CbmVisStripHit
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 03.06.09
 *@version 1.0
 **
 **/

#ifndef CBMVISSTRIPHIT_H
#define CBMVISSTRIPHIT_H 1

#include "TLine.h"
#include "CbmVisHit.h"
class CbmStripHit;
class CbmMCTrack;

class CbmVisStripHit : public TLine, public CbmVisHit{
public:
  CbmVisStripHit(CbmStripHit* hit);
  ~CbmVisStripHit(){};
  void SetVisible(Bool_t isVisible);
  CbmStripHit* GetHit() {return fHit; }
  TString GetInfo();

private:
  CbmStripHit* fHit;

  ClassDef(CbmVisStripHit,1);
};
#endif
