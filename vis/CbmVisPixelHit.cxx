// -------------------------------------------------------------------------
// -----                     CbmVisPixelHit source file                -----
// -----                  Created 03/06/09  by E. Kryshen              -----
// -------------------------------------------------------------------------
#include "CbmVisHit.h"
#include "CbmVisPixelHit.h"
#include "CbmPixelHit.h"
#include "CbmMCTrack.h"


// -------------------------------------------------------------------------
CbmVisPixelHit::CbmVisPixelHit(CbmPixelHit* hit):CbmVisHit(),TMarker(hit->GetX(),hit->GetY(),21)
{
    SetMarkerSize(fDefaultSize);
    SetMarkerColor(kBlack);
    fHit = hit;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
TString CbmVisPixelHit::GetInfo(){
  return Form("Hit: (%.2f,%.2f) pdg=%i MoId=%i MoPdg=%i",
      	      fX,fY,fTrackPdg,fMotherId,fMotherPdg);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisPixelHit::SetVisible(Bool_t isVisible){
  if (isVisible) SetMarkerSize(fDefaultSize);
  else           SetMarkerSize(0);
  fVisible = isVisible;
}
// -------------------------------------------------------------------------

ClassImp(CbmVisPixelHit)
