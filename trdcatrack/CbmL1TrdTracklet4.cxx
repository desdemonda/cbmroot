#include "CbmL1TrdTracklet4.h"

ClassImp(CbmL1TrdTracklet4);

// ----------------------- Default constructor ---------------------------
CbmL1TrdTracklet4::CbmL1TrdTracklet4()
  : TObject(),
    vAccostTracklet(),
    vAccostLeft(),
    vAccostRight(),
    M(),
    fInd(),
    fVal(0),
    fY(0),
    fIsAlone(kFALSE),
    fIndex(0),
    fExt1(0.),
    fExt2(0.),
    fExt(),
    fCoord()
{
}

// ----------------------- Default destructor ---------------------------
CbmL1TrdTracklet4::~CbmL1TrdTracklet4()
{};
