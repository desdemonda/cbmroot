#include "CbmEcalMaximum.h"

#include "CbmEcalCell.h"

#include "TMath.h"

#include <list>

using namespace std;

CbmEcalMaximum::CbmEcalMaximum(CbmEcalCell* cell, Double_t z)
  : TObject(), 
    fCell(cell), 
    fCX(0.), 
    fCY(0.), 
    fX(0.), 
    fY(0.), 
    fTanTheta(0.), 
    fI(0), 
    fMark(0), 
    fRegion(0)
{
  Double_t me=0;
  Double_t e;
  Int_t i;
  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;

  fCX=cell->GetCenterX();
  fCY=cell->GetCenterY();
  fI=1;
  for(i=1;i<5;i++)
  {
    cell->GetNeighborsList(i, cells);
    e=0;
    for(p=cells.begin();p!=cells.end();++p)
      e+=(*p)->GetTotalEnergy();
    if (e>me)
    {
      fI=i;
      me=e;
    }
  }
  me+=cell->GetTotalEnergy();
  fX=cell->GetTotalEnergy()*fCX;
  fY=cell->GetTotalEnergy()*fCY;
  cell->GetNeighborsList(fI, cells);
  for(p=cells.begin();p!=cells.end();++p)
  {
    fX+=(*p)->GetTotalEnergy()*(*p)->GetCenterX();
    fY+=(*p)->GetTotalEnergy()*(*p)->GetCenterY();
  }
  fX/=me; fY/=me;
  fTanTheta=TMath::Sqrt(fX*fX+fY*fY)/z;
  fMark=0;
  fRegion=-1111;
}

ClassImp(CbmEcalMaximum)
