// -------------------------------------------------------------------------
// -----                  CbmEcalModule source file                    -----
// -----                  Created  27/01/06  by M.Prokudin             -----
// -------------------------------------------------------------------------

/**  CbmEcalModule.cxx
 *@author Mikhail Prokudin
 **
 ** ECAL module structure, consisting of cells
 **/

/* $Id: CbmEcalModule.cxx,v 1.6 2006/09/12 18:15:23 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalModule.cxx,v $
 * Revision 1.6  2006/09/12 18:15:23  prokudin
 * GetType moved to CbmEcalCell
 *
 * Revision 1.5  2006/07/12 08:11:14  prokudin
 * Magic number 48 removed.
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

#include "CbmEcalModule.h"

#include "CbmEcalCellMC.h"

#include "TMath.h"

using std::list;
using std::vector;

//-----------------------------------------------------------------------------
CbmEcalModule::CbmEcalModule(char type, Int_t cellnumber, Float_t x1, Float_t y1, Float_t x2, Float_t y2, Int_t mc, Float_t energy) 
  : CbmEcalCell(cellnumber, x1,y1,x2,y2, type, energy), 
    fDx(x2-x1), 
    fDy(y2-y1),
    fCells()
{
  if (GetType()<1) return;
	
  Int_t i;
  Int_t j;
  Int_t mt;
  Int_t num;
	
  mt=type;
  fCells.resize(mt*mt,NULL);

  num=cellnumber/100;

  if (mc==0)
    for(i=0;i<mt;i++)
      for(j=0;j<mt;j++)
        fCells[i*mt+j]=new CbmEcalCell((num*10+i+1)*10+j+1, x1+j*fDx/mt, y1+i*fDy/mt, x1+(j+1)*fDx/mt, y1+(i+1)*fDy/mt, type);
  else 
    for(i=0;i<mt;i++)
      for(j=0;j<mt;j++)
        fCells[i*mt+j]=new CbmEcalCellMC((num*10+i+1)*10+j+1, x1+j*fDx/mt, y1+i*fDy/mt, x1+(j+1)*fDx/mt, y1+(i+1)*fDy/mt, type);
}

//-----------------------------------------------------------------------------
CbmEcalCell* CbmEcalModule::Locate(Int_t x, Int_t y) const
{
  if (x<0||y<0||x>=GetType()||y>=GetType()) return NULL;
  return fCells[y*GetType()+x];
}

//-----------------------------------------------------------------------------
CbmEcalCell* CbmEcalModule::FindCell(Float_t x, Float_t y) const
{
  Int_t ix=(Int_t)TMath::Floor( (x-GetX1())/GetDX()*GetType() );
  Int_t iy=(Int_t)TMath::Floor( (y-GetY1())/GetDY()*GetType() );
  if (ix<0) ix=0; if (ix>=GetType()) ix=GetType()-1;
  if (iy<0) iy=0; if (iy>=GetType()) iy=GetType()-1;
  return At(ix,iy);
}

//-----------------------------------------------------------------------------
void CbmEcalModule::AddEnergy(Float_t x, Float_t y, Float_t energy)
{
  CbmEcalCell* tmp=FindCell(x,y);
  if (!tmp) return;
  tmp->AddEnergy(energy);
  CbmEcalCell::AddEnergy(energy);
}

//-----------------------------------------------------------------------------
list<CbmEcalCell*> CbmEcalModule::GetCellsX(Float_t x) const 
{
  list<CbmEcalCell*> tmp;
  vector<CbmEcalCell*>::const_iterator p;

  for(p=fCells.begin();p!=fCells.end();++p)
    if (x>(*p)->GetX1()&&x<(*p)->GetX2()) tmp.push_back(*p);
  return tmp;
}

//-----------------------------------------------------------------------------
list<CbmEcalCell*> CbmEcalModule::GetCellsY(Float_t y) const
{
  list<CbmEcalCell*> tmp;
  vector<CbmEcalCell*>::const_iterator p;

  for(p=fCells.begin();p!=fCells.end();++p)
    if (y>(*p)->GetY1()&&y<(*p)->GetY2()) tmp.push_back(*p);
  return tmp;
}
