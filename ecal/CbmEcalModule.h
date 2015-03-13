// -------------------------------------------------------------------------
// -----                  CbmEcalModule header file                    -----
// -----                  Created  27/01/06  by M.Prokudin             -----
// -------------------------------------------------------------------------

/**  CbmEcalModule.h
 *@author Mikhail Prokudin
 **
 ** ECAL module structure, consisting of cells
 **/

/* $Id: CbmEcalModule.h,v 1.5 2006/09/12 18:15:23 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalModule.h,v $
 * Revision 1.5  2006/09/12 18:15:23  prokudin
 * GetType moved to CbmEcalCell
 *
 * Revision 1.4  2006/06/30 06:17:17  kharlov
 * Optimization, commenting
 *
 * Revision 1.3  2006/04/02 16:55:23  kharlov
 * Merging full and fast MC
 *
 * Revision 1.2  2006/01/31 17:07:17  kharlov
 * Correction for CbmEcalv1
 *
 * Revision 1.1  2006/01/27 17:26:30  kharlov
 * First adaptation of ECAL detailed geometry to cbmroot2
 *
 */

#ifndef CBMECALMODULE_H
#define CBMECALMODULE_H

#include "CbmEcalCell.h"

#include <vector>
#include <list>

class CbmEcalModule : public CbmEcalCell
{

public:
  // Set mc==1 to construct CbmEcalCellMC, not CbmEcalCell 
  CbmEcalModule(char type=1, Int_t cellnumber=-1, Float_t x1=0, Float_t y1=0, Float_t x2=0, Float_t y2=0, Int_t mc=0,Float_t energy=0);
	
  CbmEcalCell* Locate(Int_t x, Int_t y) const;
  
  //Faster than Locate, but doesn't check boundaries
  inline CbmEcalCell* At(Int_t x, Int_t y) const {return fCells[y*GetType()+x];}
  CbmEcalCell* FindCell(Float_t x, Float_t y) const;
  void AddEnergy(Float_t x, Float_t y, Float_t energy); 
  inline Float_t GetEnergy(Float_t x, Float_t y) const
  {
    CbmEcalCell* tmp=FindCell(x,y);
    if (tmp) return tmp->GetEnergy();
    return -1;
  }
  void ResetModule();
  
  inline Float_t GetDX() const {return fDx;}
  inline Float_t GetDY() const {return fDy;}
  std::vector<CbmEcalCell*> GetCells() const {return fCells;}
  
  //returns cells for which X1<x<X2
  std::list<CbmEcalCell*> GetCellsX(Float_t x) const; 
  //returns cells for which Y1<y<Y2
  std::list<CbmEcalCell*> GetCellsY(Float_t y) const;

private:
  /** module x-size **/
  Float_t fDx;
  /** module y-size **/
  Float_t fDy;
  /** list of cells contained in a module **/
  std::vector<CbmEcalCell*> fCells;

  ClassDef(CbmEcalModule,1);
};

inline void CbmEcalModule::ResetModule()
{
  ResetEnergyFast();
  for(UInt_t i=0;i<fCells.size();i++) fCells[i]->ResetEnergyFast();
}
#endif
