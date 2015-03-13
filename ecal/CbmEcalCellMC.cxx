// -------------------------------------------------------------------------
// -----                  CbmEcalCellMC source file                    -----
// -----                  Created  18/01/12  by M.Prokudin             -----
// -------------------------------------------------------------------------

/**  CbmEcalCellMC.cxx
 *@author Mikhail Prokudin
 **
 ** ECAL cell structure, a part of ECAL module. This implementation carries an MC information 
 **/

/* $Id: CbmEcalCellMC.cxx,v 1.6 2011/01/18 09:33:34 prokudin Exp $ */

#include "CbmEcalCellMC.h"

#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;
using std::map;
using std::list;

//-----------------------------------------------------------------------------
CbmEcalCellMC::CbmEcalCellMC(Int_t cellnumber, Float_t x1, Float_t y1, Float_t x2, Float_t y2, Char_t type, Float_t energy)
  : CbmEcalCell(cellnumber, x1, y1, x2, y2, type, energy),
    fTrackEnergy(),
    fTrackTime()
{
}
//-----------------------------------------------------------------------------
Float_t CbmEcalCellMC::GetTrackTime(Int_t num) const
{
  map<Int_t, Float_t>::const_iterator p=fTrackTime.find(num);
  if (p==fTrackTime.end()) return 0; else return p->second;
}


//-----------------------------------------------------------------------------
Float_t CbmEcalCellMC::GetTrackEnergy(Int_t num) const
{
  map<Int_t, Float_t>::const_iterator p=fTrackEnergy.find(num);
  if (p==fTrackEnergy.end()) return 0; else return p->second;
}

//-----------------------------------------------------------------------------
void CbmEcalCellMC::ResetEnergy()
{
  ResetEnergyFast();
  fTrackEnergy.clear();
  fTrackTime.clear();
}

//-----------------------------------------------------------------------------
Float_t CbmEcalCellMC::GetTrackClusterEnergy(Int_t num, Int_t code)
{
  if (code>4||code<0) return -1;
  Float_t energy=GetTrackTotalEnergy(num);
  list<CbmEcalCell*> cls; GetNeighborsList(code, cls);
  list<CbmEcalCell*>::const_iterator p=cls.begin();
  for(;p!=cls.end();++p)
    energy+=((CbmEcalCellMC*)(*p))->GetTrackTotalEnergy(num);
  return energy;
}

//-----------------------------------------------------------------------------
void CbmEcalCellMC::TrackIvansProcedure(Int_t num, Float_t &clusterE, Float_t &ivansE, Float_t &shape)
{
  clusterE=GetTrackClusterEnergy(num,0);
  ivansE=0;
  Float_t en;
  for(Int_t i=1;i<5;i++)
    if ((en=GetTrackClusterEnergy(num,i))>ivansE) ivansE=en;
  shape=ivansE/clusterE;
}

ClassImp(CbmEcalCellMC)

