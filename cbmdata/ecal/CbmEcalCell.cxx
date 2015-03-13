// -------------------------------------------------------------------------
// -----                  CbmEcalCell source file                      -----
// -----                  Created  27/01/06  by M.Prokudin             -----
// -------------------------------------------------------------------------

/**  CbmEcalCell.cxx
 *@author Mikhail Prokudin
 **
 ** ECAL cell structure, a part of ECAL module
 **/

/* $Id: CbmEcalCell.cxx,v 1.6 2006/07/19 09:33:34 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalCell.cxx,v $
 * Revision 1.6  2006/07/19 09:33:34  prokudin
 * Modifing for CbmEcalPhotonCalibrator
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

#include "CbmEcalCell.h"

#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;
using std::map;
using std::list;

//-----------------------------------------------------------------------------
Int_t CbmEcalCell::CountNeighbors(const std::list<CbmEcalCell*>& lst) const
{
  Int_t c=0;
  list<CbmEcalCell*>::const_iterator p=lst.begin();
  for(;p!=lst.end();++p)
    if (find(fNeighbors[0].begin(), fNeighbors[0].end(), *p)!=fNeighbors[0].end())
      ++c;

  return c;
}

//-----------------------------------------------------------------------------
void CbmEcalCell::GetClusterEnergy(Int_t code, Float_t& EcalEnergy)
{
  EcalEnergy=-1;
  if (code>4||code<0)
    return;
  EcalEnergy=GetEnergy();
  list<CbmEcalCell*>::const_iterator p;
  for(p=fNeighbors[code].begin();p!=fNeighbors[code].end();++p)
  {
    EcalEnergy+=(*p)->GetEnergy();
  }
}

//-----------------------------------------------------------------------------
void CbmEcalCell::IvansProcedure(Float_t &clusterE, Float_t &ivansE, Float_t &shape)
{
  Float_t e;
  GetClusterEnergy(0,e);
  clusterE=e;
  ivansE=0;
  for(Int_t i=1;i<5;i++)
  {
    GetClusterEnergy(i,e);
    if (e>ivansE) ivansE=e;
  }
  shape=ivansE/clusterE;
}

