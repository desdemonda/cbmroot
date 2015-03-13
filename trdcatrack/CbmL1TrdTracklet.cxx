#include "CbmL1TrdTracklet.h"

ClassImp(CbmL1TrdTracklet);

// ----------------------- Default constructor ---------------------------
CbmL1TrdTracklet::CbmL1TrdTracklet()
  : TObject(),
    vAccostTracklet(),
    fID(),
    fIndLeft(0),
    fIndRight(0),
    fVal(0),
    fY(0),
    fIsAlone(kFALSE),
    fIndex(0),
    fCoord()
{
}

// ----------------------- Default destructor ---------------------------
CbmL1TrdTracklet::~CbmL1TrdTracklet()
{};
