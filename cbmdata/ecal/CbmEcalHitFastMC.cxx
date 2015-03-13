// -------------------------------------------------------------------------
// -----                CbmEcalHitFastMC source file                   -----
// -----               Created 21/06/06  by Yu.Kharlov                 -----
// -------------------------------------------------------------------------

  /* $Id: CbmEcalHitFastMC.cxx,v 1.1 2006/06/22 06:55:01 kharlov Exp $ */

  /* History of cvs commits:
   *
   * $Log: CbmEcalHitFastMC.cxx,v $
   * Revision 1.1  2006/06/22 06:55:01  kharlov
   * Newly added classes for Fas-MC hit producer
   *
   */

/**  CbmEcalHitFastMC.h
 *@author Yu.Kharlov
 **
 ** ECAL hit which is a photon with the total energy and
 ** coordinate position. To be used in Fast MC.
 **/
#include "CbmEcalHitFastMC.h"


// -----   Default constructor   -------------------------------------------
CbmEcalHitFastMC::CbmEcalHitFastMC() 
  : FairHit(),
    fAmplitude(0.)
{
}
// -------------------------------------------------------------------------

// -----   Constructor   -------------------------------------------
CbmEcalHitFastMC::CbmEcalHitFastMC(Double_t x, Double_t y, Double_t E)
  : FairHit(),
    fAmplitude(E)
{
  // Fill position and energy of the hit
  fX = x;
  fY = y;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmEcalHitFastMC::~CbmEcalHitFastMC(){}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void CbmEcalHitFastMC::Print(const Option_t* opt) const {
  printf("ECAL hit: (x,y)=(%f.1,%f.1) cm, Amp=%f GeV\n",
	 fX,fY,fAmplitude);
}
// -------------------------------------------------------------------------

ClassImp(CbmEcalHitFastMC)
