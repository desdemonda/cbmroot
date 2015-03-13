/** CbmMuchStrawDigi.cxx
 **@author E.Kryshen <e.kryshen@gsi.de>
 **@since 23.07.2013
 **@version 1.0
 **
 ** Data class for straws MUCH information 
 ** Data level: RAW **
 **/

#include "CbmMuchStrawDigi.h"

// -------------------------------------------------------------------------
CbmMuchStrawDigi::CbmMuchStrawDigi() 
  : CbmDigi(),
    fAddress(),
    fX(),
    fY(),
    fZ(),
    fTime()
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmMuchStrawDigi::CbmMuchStrawDigi(UInt_t address, Float_t x, Float_t y, Float_t z, Double_t time)
  : CbmDigi(),
    fAddress(address),
    fX(x),
    fY(y),
    fZ(z),
    fTime(time)
{
}

ClassImp(CbmMuchStrawDigi)
