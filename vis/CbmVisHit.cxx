// -------------------------------------------------------------------------
// -----                     CbmVisHit source file                     -----
// -----                  Created 14/03/07  by E. Kryshen              -----
// -------------------------------------------------------------------------
#include "CbmVisHit.h"
#include "CbmMCTrack.h"

// -------------------------------------------------------------------------
CbmVisHit::CbmVisHit(){
    fDefaultSize = 0.5;
    fPointId     = -1;
    fTrackId     = -1;
    fTrackPdg    = 0;
    fMotherId    = -2;
    fMotherPdg   = 0;
    fTrack       = NULL;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmVisHit::SetTrack(CbmMCTrack* track){
    fTrack    = track;
    fTrackPdg = track->GetPdgCode();
    fMotherId = track->GetMotherId();
}
// -------------------------------------------------------------------------

ClassImp(CbmVisHit)
