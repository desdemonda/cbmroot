#ifndef CBMECALMAXIMUM_H
#define CBMECALMAXIMUM_H

#include "TObject.h"

class CbmEcalCell;
/** A calorimeter maximum. Used for unification of photon reconstruction and
 ** electron identification procedures **/
class CbmEcalMaximum : public TObject
{
public:
  /** An emtry constructor **/
  CbmEcalMaximum() : TObject(), fCell(NULL), fCX(0.), fCY(0.), fX(0.), fY(0.), 
    fTanTheta(0.), fI(0), fMark(0), fRegion(0)
    {};
  /** Simplest constructor **/
  CbmEcalMaximum(CbmEcalCell* cell, Double_t z);
  /** Standard constructor **/
  CbmEcalMaximum(CbmEcalCell* cell, Double_t cx, Double_t cy, Double_t x, Double_t y, Int_t i);
  ~CbmEcalMaximum() {};

  CbmEcalCell* Cell() const {return fCell;}
  Double_t CX() const {return fCX;}
  Double_t CY() const {return fCY;}
  Double_t X() const {return fX;}
  Double_t Y() const {return fY;}
  Int_t I() const {return fI;}
  Int_t Mark() const {return fMark;}
  /** Region is used for calibration **/
  Int_t Region() const {return fRegion;}
  Double_t TanTheta() const {return fTanTheta;}

  void SetMark(Int_t mark) {fMark=mark;}
  void SetRegion(Int_t region) {fRegion=region;}
  void SetTanTheta(Double_t tantheta) {fTanTheta=tantheta;}
private:
  CbmEcalCell* fCell;
  /** Coordinates of cell **/
  Double_t fCX;
  Double_t fCY;
  /** Coobdinates of center of mass of maximum subcluster **/
  Double_t fX;
  Double_t fY;
  /** Sqrt(fX*fX+fY*fY)/Z_calorimeter**/
  Double_t fTanTheta;
  /** Number maximum subcluster **/
  Int_t fI;
  /** A mark. Used for maximum exclusion. **/
  Int_t fMark;
  /** A calorimeter region **/
  Int_t fRegion;


  CbmEcalMaximum(const CbmEcalMaximum&);
  CbmEcalMaximum& operator=(const CbmEcalMaximum&);

  ClassDef(CbmEcalMaximum, 1)
};

inline CbmEcalMaximum::CbmEcalMaximum(CbmEcalCell* cell, Double_t cx, Double_t cy, Double_t x, Double_t y, Int_t i)
		      : TObject(), fCell(cell), fCX(cx), fCY(cy), fX(x), fY(y), 
		      fTanTheta(0.), fI(i), fMark(0), fRegion(0)
{
  ;
}
#endif

