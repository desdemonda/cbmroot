// -------------------------------------------------------------------------
// -----                  CbmEcalCell header file                      -----
// -----                  Created  27/01/06  by M.Prokudin             -----
// -------------------------------------------------------------------------

/**  CbmEcalCell.h
 *@author Mikhail Prokudin
 **
 ** ECAL cell structure, a part of ECAL module
 **/

#ifndef CBMECALCELL_H
#define CBMECALCELL_H

/* $Id: CbmEcalCell.h,v 1.9 2006/09/12 18:15:23 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalCell.h,v $
 * Revision 1.9  2006/09/12 18:15:23  prokudin
 * GetType moved to CbmEcalCell
 *
 * Revision 1.8  2006/07/19 11:49:39  prokudin
 * Commenting, optimizing hitproducer
 *
 * Revision 1.7  2006/07/19 09:33:34  prokudin
 * Modifing for CbmEcalPhotonCalibrator
 *
 * Revision 1.6  2006/07/13 14:46:00  prokudin
 * New version of hit producer
 *
 * Revision 1.5  2006/06/30 06:17:17  kharlov
 * Optimization, commenting
 *
 * Revision 1.4  2006/04/02 16:55:23  kharlov
 * Merging full and fast MC
 *
 * Revision 1.3  2006/02/03 14:53:40  kharlov
 * CbmEcalAnalysisDS.cxx
 *
 * Revision 1.2  2006/01/31 17:07:17  kharlov
 * Correction for CbmEcalv1
 *
 * Revision 1.1  2006/01/27 17:26:29  kharlov
 * First adaptation of ECAL detailed geometry to cbmroot2
 *
 */

#include "TObject.h"

#include <list>
#include <map>
#include <algorithm>

class CbmEcalCell : public TObject
{
public:
  CbmEcalCell(Int_t cellnumber, Float_t x1=0, Float_t y1=0, Float_t x2=0, Float_t y2=0, Char_t type=0, Float_t energy=0) 
    : TObject(), fNumber(cellnumber), fX1(x1), fY1(y1), fX2(x2),
    fY2(y2), fType(type), fEnergy(energy), fADC(-1111), fNeighbors(), fTime(-1111)
  {};

  inline Bool_t IsInside(Float_t x, Float_t y) {return x>GetX1()&&x<GetX2()&&y>GetY1()&&y<GetY2();}
  //getters
  inline Char_t GetType() const {return fType;}
  inline Float_t X1() const {return fX1;}
  inline Float_t Y1() const {return fY1;}
  inline Float_t X2() const {return fX2;}
  inline Float_t Y2() const {return fY2;}
  inline Float_t GetX1() const {return fX1;}
  inline Float_t GetY1() const {return fY1;}
  inline Float_t GetX2() const {return fX2;}
  inline Float_t GetY2() const {return fY2;}
  inline Float_t GetCenterX() const {return (fX1+fX2)/2.0;}
  inline Float_t GetCenterY() const {return (fY1+fY2)/2.0;}
  inline Short_t ADC() const {return fADC;}
  inline Short_t GetADC() const {return fADC;}

  inline Int_t   GetCellNumber() const {return fNumber;}
	
  inline Float_t GetEnergy() const {return fEnergy;}
  inline Float_t GetTotalEnergy() const {return fEnergy;}	//Dummy: better use GetEnergy
  Float_t GetTime() const {return fTime;}
  void SetTime(Float_t time) {fTime=time;}
	
  inline void GetNeighborsList(Int_t num, std::list<CbmEcalCell*> &neib) const
  {
    if (num<0||num>4) return;
    neib=fNeighbors[num];
  }
  inline void SetNeighborsList(Int_t num, std::list<CbmEcalCell*> &neib)
  {
    if (num<0||num>4) return;
    fNeighbors[num]=neib;
  }
  inline void SetEnergy(Float_t energy) {fEnergy=energy;}
  inline void SetADC(Short_t adc) {fADC=adc;}
  /** Reset all energies in cell **/
  void ResetEnergyFast();
  inline void AddEnergy(Float_t energy) {fEnergy+=energy;}
	
  // code=0 for "3x3" cluster
  // code=1-4 for other clusters for Ivan's procedure
  void GetClusterEnergy(Int_t code, Float_t& EcalEnergy);

  /** Get 2x2 subcluster with maximum energy deposition of 3x3 cluster**/
  void IvansProcedure(Float_t &clusterE, Float_t &ivansE, Float_t &shape);
  void TrackIvansProcedure(Int_t num, Float_t &clusterE, Float_t &ivansE, Float_t &shape) {};

  inline void SetCoord(Float_t x1, Float_t y1, Float_t x2, Float_t y2)
    { fX1=x1; fY1=y1; fX2=x2; fY2=y2; }
  inline void SetType(Char_t type) {fType=type;}
  /** returns number of neighbors in lst with cell **/
  Int_t CountNeighbors(const std::list<CbmEcalCell*>& lst) const; 
private:
  /**  cell number within the module **/
  Int_t fNumber;
  /** left edge of the cell **/
  Float_t fX1;
  /** bottom edge of the cell **/
  Float_t fY1;
  /** right edge of the cell **/
  Float_t fX2;
  /** upper edge of the cell **/
  Float_t fY2;
  /** type of cell **/
  Char_t fType;
  /** energy in the calorimeter cell **/
  Float_t fEnergy;
  /** ADC counts read **/
  Short_t fADC;


  /** list of neighbor cells **/
  std::list<CbmEcalCell*> fNeighbors[5];

  /** Time of cell to fire **/
  Double_t fTime;

  ClassDef(CbmEcalCell,1);
};
  
inline void CbmEcalCell::ResetEnergyFast()
{
  fEnergy=0;
  fADC=-1111;
  fTime=-1111;
}

#endif
