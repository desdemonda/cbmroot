/** CbmMuchModuleGemRadial.h
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@version 1.0
 *@since   03.08.12
 */

#ifndef CBMMUCHMODULEGEMRADIAL_H
#define CBMMUCHMODULEGEMRADIAL_H 1

#include "CbmMuchModuleGem.h"
#include "TVector3.h"
#include <vector>
using std::vector;

class CbmMuchSectorRadial;
class CbmMuchPadRadial;

class CbmMuchModuleGemRadial : public CbmMuchModuleGem{
 public:
  CbmMuchModuleGemRadial();
  CbmMuchModuleGemRadial(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, 
      TVector3 position, Double_t dx1, Double_t dx2, Double_t dy, Double_t dz, Double_t cutRadius);
  virtual ~CbmMuchModuleGemRadial() {}
  Double_t GetDx1() const { return fDx1; }
  Double_t GetDx2() const { return fDx2; }
  Double_t GetDy()  const { return fDy;  }
  Double_t GetDz()  const { return fDz;  }
  CbmMuchSectorRadial* GetSectorByRadius(Double_t r);
  CbmMuchPadRadial* GetPad(Double_t x, Double_t y);
  Bool_t InitModule();
 protected:
  vector<Double_t> fSectorRadii; //! vector of sector radii
  
  Double_t fDx1;
  Double_t fDx2;
  Double_t fDy;
  Double_t fDz;
  ClassDef(CbmMuchModuleGemRadial,1);
};
#endif


