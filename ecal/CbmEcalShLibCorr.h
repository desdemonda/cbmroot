#ifndef CBMECALSHLIBCORR_H
#define CBMECALSHLIBCORR_H

#include "CbmEcalShLib.h"
#include "CbmEcalCell.h"

// Abstract class for shower library with correlations

class CbmEcalShLibCorr : public CbmEcalShLib
{
public:
  CbmEcalShLibCorr() {};
  CbmEcalShLibCorr(const char* nm, Int_t verb) : CbmEcalShLib(nm, verb) {};
  // Cell is a size of cell in cm.
  // Correlation between cells
  virtual Float_t Correlation(Float_t x1, Float_t y1, Float_t x2, Float_t y2, Float_t phi, Float_t theta, Float_t e, Int_t celltype=0)=0;
  virtual Float_t Correlation(Float_t x1, Float_t y1, Float_t x2, Float_t y2, Float_t phi, Float_t theta, Float_t e, CbmEcalCell* cell)
  {
    return Correlation(x1, y1, x2, y2, phi, theta, e, cell->GetType());
  }
  // Correlation between cells with different size.
  // x2, y2 --- coordinates of cell with 2x size
  virtual Float_t Correlation2(Float_t x1, Float_t y1, Float_t x2, Float_t y2, Float_t phi, Float_t theta, Float_t e, Int_t celltype=0)=0;
  virtual ~CbmEcalShLibCorr()=0;
private:
  ClassDef(CbmEcalShLibCorr, 1);
};

#endif
