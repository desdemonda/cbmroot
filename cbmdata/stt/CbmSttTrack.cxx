
#include "CbmSttTrack.h"

ClassImp(CbmSttTrack);

// -----   Default constructor   -------------------------------------------
CbmSttTrack::CbmSttTrack() 
  : TObject(),
    fStsTrackID(-1),
    fNHits(0),
    fNMissedHits(0),
    fNMissedStations(0),
    fHits(),
    fSttTrack(),
    fChi2(0.),
    fNDF(0),
    fFlag(1),
    fScatAng(0.)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmSttTrack::~CbmSttTrack() {
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmSttTrack::Print() {
}
// -------------------------------------------------------------------------
