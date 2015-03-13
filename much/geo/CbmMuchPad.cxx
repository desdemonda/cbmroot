#include "CbmMuchPad.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"

// -------------------------------------------------------------------------
CbmMuchPad::CbmMuchPad() 
  : fAddress(0),
    fX(0.), 
    fY(0.),
    fDx(0.),
    fDy(0.),
    fDigiIndex(-1),
    fDigi(NULL),
    fMatch(NULL),
    fNeighbours()
{
  fNeighbours.resize(20);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchPad::CbmMuchPad (Int_t address, Double_t x, Double_t y, Double_t dx, Double_t dy) 
  : fAddress(address),
    fX(x),
    fY(y),
    fDx(dx),
    fDy(dy),
    fDigiIndex(-1),
    fDigi(new CbmMuchDigi(address)),
    fMatch(new CbmMuchDigiMatch()),
    fNeighbours()
{
  fNeighbours.resize(20);
}
// -------------------------------------------------------------------------

