#include "CbmSttPoint.h"
#include "CbmSttHit.h"

#include "TVector3.h"

//___________________________________________________________________
//
// CbmSttHit
//
// Class for description of the hit in the STT
//

// ------------------------------------------------------------------
CbmSttHit::CbmSttHit()
  : CbmHit(),
    fPlaneID(-1),
    fChamber(-1),
    fTube(-1),
    fSegment(-1),
    fAlong(0.),
    fU(0.)
{
}
// ------------------------------------------------------------------
CbmSttHit::CbmSttHit(Int_t detID, TVector3& pos, TVector3& dpos, 
		     Int_t index, CbmSttPoint *point)
  : CbmHit(detID, pos, dpos, 0., index),
    fPlaneID(point->GetLayerNo() + (point->GetStationNo() - 1) * 6),
    fChamber(point->GetChamberNo()),
    fTube(0),
    fSegment(0),
    fAlong(0.),
    fU(0.)
{
  // Standard constructor
  /*
  fPlaneID = ;
  fTube = fSegment = 0;
  fChamber = ;
  fAlong = 0.;
  */
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
CbmSttHit::~CbmSttHit()
{
    // Destructor
}
// ------------------------------------------------------------------

ClassImp(CbmSttHit)

