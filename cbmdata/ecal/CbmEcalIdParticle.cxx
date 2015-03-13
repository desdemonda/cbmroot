// -------------------------------------------------------------------------
// -----                   CbmEcalIdParticle source file               -----
// -----                   Created 2009.03.13 by //Dr.Sys              -----
// -------------------------------------------------------------------------

/**  CbmEcalIdParticle.cxx
 *@author //Dr.Sys
 **
 ** Identified particle in ECAL, characterized by reconstructed 4-momentum
 **/

#include "CbmEcalIdParticle.h"
#include "CbmEcalCluster.h"

#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
CbmEcalIdParticle::CbmEcalIdParticle() 
  : TObject(), 
    fX(-1111), 
    fY(-1111), 
    fZ(-1111), 
    fE(-1111), 
    fPSE(-1111), 
    fShape(-1111), 
    fCellNum(-1111), 
    fTrack(-1111), 
    fEProb(-1111), 
    fMCTrack(-1111)
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmEcalIdParticle::CbmEcalIdParticle(CbmEcalCell* cell,
                    Double_t x, Double_t y, Double_t z,
		    Double_t e, Double_t pse, Double_t shape,
                    Int_t tracknum, Double_t prob, Int_t mctr)
  : TObject(),
    fX(x), 
    fY(y), 
    fZ(z), 
    fE(e), 
    fPSE(pse), 
    fShape(shape),
    fCellNum(cell->GetCellNumber()), 
    fTrack(tracknum), 
    fEProb(prob), 
    fMCTrack(mctr)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmEcalIdParticle::CbmEcalIdParticle(Int_t cell,
                    Double_t x, Double_t y, Double_t z,
		    Double_t e, Double_t pse, Double_t shape,
                    Int_t tracknum, Double_t prob, Int_t mctr)
  : TObject(),
    fX(x), 
    fY(y), 
    fZ(z), 
    fE(e), 
    fPSE(pse), 
    fShape(shape),
    fCellNum(cell), 
    fTrack(tracknum), 
    fEProb(prob), 
    fMCTrack(mctr)
{
}
// -------------------------------------------------------------------------
CbmEcalIdParticle::CbmEcalIdParticle(const CbmEcalIdParticle& obj)

  : TObject(obj),
    fX(obj.fX), 
    fY(obj.fY), 
    fZ(obj.fZ), 
    fE(obj.fE), 
    fPSE(obj.fPSE), 
    fShape(obj.fShape),
    fCellNum(obj.fCellNum), 
    fTrack(obj.fTrack), 
    fEProb(obj.fEProb), 
    fMCTrack(obj.fMCTrack)
{
}

// -----   Destructor   ----------------------------------------------------
CbmEcalIdParticle::~CbmEcalIdParticle()
{
  ;
}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void CbmEcalIdParticle::Print(const Option_t* opt) const {
  printf("-I- CbmEcalIdParticle: ECAL id particle :(track, E, PSE, Shape) = (%d,%f,%f,%f)\n", fTrack, fE, fPSE, fShape);
  printf("                                        :(x, y) = (%f,%f)\n", fX, fY);
}
// -------------------------------------------------------------------------


ClassImp(CbmEcalIdParticle)
