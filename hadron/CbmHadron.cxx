// ------------------------------------------------------------------
// -----                      CbmHadron.h                       -----
// -----            Created 2008-01-18 by D.Kresan              -----
// ------------------------------------------------------------------

#include "CbmHadron.h"


// ------------------------------------------------------------------
CbmHadron::CbmHadron()
  : TObject(),
    fCharge(0),
    fP(0.),
    fPt(0.),
    fPz(0.),
    fNMvdHits(0),
    fNStsHits(0),
    fNTrdHits(0),
    fB(0.),
    fLength(0.),
    fMass2(0.),
    fPdg(0),
    fGhost(kFALSE),
    fTDH(kFALSE)
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmHadron::CbmHadron(Int_t charge, Double_t p, Double_t pt, Double_t pz,
                     Int_t nMvdHits, Int_t nStsHits, Int_t nTrdHits,
		     Double_t b, Double_t length, Double_t mass2,
		     Int_t pdg, Bool_t ghost, Bool_t tdh)
  : TObject(),
    fCharge(charge),
    fP(p),
    fPt(pt),
    fPz(pz),
    fNMvdHits(nMvdHits),
    fNStsHits(nStsHits),
    fNTrdHits(nTrdHits),
    fB(b),
    fLength(length),
    fMass2(mass2),
    fPdg(pdg),
    fGhost(ghost),
    fTDH(tdh)
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmHadron::~CbmHadron()
{
}
// ------------------------------------------------------------------


ClassImp(CbmHadron);

