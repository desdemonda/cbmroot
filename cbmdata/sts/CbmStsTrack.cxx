// -------------------------------------------------------------------------
// -----                      CbmStsTrack source file                  -----
// -----                  Created 26/01/05  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmStsTrack.h"
#include <sstream>
using std::stringstream;

CbmStsTrack::CbmStsTrack()
  : CbmTrack(),
    fMvdHitIndex(),
    fB(0.)
{
}

CbmStsTrack::~CbmStsTrack() {
}

void CbmStsTrack::AddMvdHit(Int_t hitIndex) {
  fMvdHitIndex.push_back(hitIndex);
}

string CbmStsTrack::ToString() const {
   stringstream ss;
   ss << "CbmStsTrack: nof STS hits=" << GetNofHits() << ", nof MVD hits="
         << GetNofMvdHits() << "chiSq=" << GetChiSq() << ", NDF=" << GetNDF()
         << ", pidHypo=" << GetPidHypo() << ", previousTrackId="
         << GetPreviousTrackId() << ", flag=" << GetFlag() << ", B=" << GetB()
         << "\n";
   return ss.str();
}

ClassImp(CbmStsTrack)
