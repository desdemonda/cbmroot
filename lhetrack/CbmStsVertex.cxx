#include "CbmStsVertex.h"
#include "CbmStsHit.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// CbmStsVertex class - representation of the vertex for the Sts.         //
//                                                                        //
////////////////////////////////////////////////////////////////////////////


ClassImp(CbmStsVertex)


//________________________________________________________________________
CbmStsVertex::CbmStsVertex()
{
  // Default constructor.

  SetX(0.);
  SetY(0.);
  SetZ(0.);

  SetXerr(0.);
  SetYerr(0.);
  SetZerr(0.);
  
}

//________________________________________________________________________
CbmStsVertex::CbmStsVertex(Double_t pos[3])
{
  
  SetX((Double_t) pos[0]);
  SetY((Double_t) pos[1]);
  SetZ((Double_t) pos[2]);
}  


//________________________________________________________________________
CbmStsVertex::CbmStsVertex(Double_t x, Double_t y, Double_t z)
{
  
  SetX(x);
  SetY(y);
  SetZ(z);
}  

//________________________________________________________________________
CbmStsVertex::~CbmStsVertex() 
{
  // Destructor.
}


