// -------------------------------------------------------------------------
// -----                     CbmVisStripHit source file                -----
// -----                  Created 14/03/07  by E. Kryshen              -----
// -------------------------------------------------------------------------
#include "CbmVisStripHit.h"
#include "CbmStripHit.h"
#include "CbmMCTrack.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchModule.h"


// -------------------------------------------------------------------------
CbmVisStripHit::CbmVisStripHit(CbmStripHit* hit):CbmVisHit(),TLine(){
  fHit = hit;

  CbmMuchGeoScheme* fGeoScheme = CbmMuchGeoScheme::Instance();
  if (!fGeoScheme) {
    Fatal("CbmVisStripHit","CbmMuchGeoScheme not initialized");
    return;
  }
  CbmMuchModule* mod = fGeoScheme->GetModuleByDetId(fHit->GetDetectorId());
  Double_t x1 = mod->GetX1(); // left border of the module
  Double_t x2 = mod->GetX2(); // right border of the module
  Double_t y1 = mod->GetY1(); // bottom border of the module
  Double_t y2 = mod->GetY2(); // top border of the module
  Double_t x[2]; // X-coordinates of the module/strip crossing
  Double_t y[2]; // Y-coordinates of the module/strip crossing
  Double_t a = hit->GetPhi();
  Double_t p = hit->GetU();
  // Determine potential crossings
  Double_t yc1 = (p-x1*cos(a))/sin(a);
  Double_t yc2 = (p-x2*cos(a))/sin(a);
  Double_t xc1 = (p-y1*sin(a))/cos(a);
  Double_t xc2 = (p-y2*sin(a))/cos(a);
  Int_t i=0;
  // Select two crossings in the module range
  if (i<2 && yc1>y1 && yc1<y2) { x[i]=x1;  y[i]=yc1; i++; }
  if (i<2 && yc2>y1 && yc2<y2) { x[i]=x2;  y[i]=yc2; i++; }
  if (i<2 && xc1>x1 && xc1<x2) { x[i]=xc1; y[i]=y1;  i++; }
  if (i<2 && xc2>x1 && xc2<x2) { x[i]=xc2; y[i]=y2;  i++; }
  // Set strip coordinates
  fX1=x[0];
  fX2=x[1];
  fY1=y[0];
  fY2=y[1];
  //printf("x[0]=%6.1f,y[0]=%6.1f,x[1]=%6.1f,y[1]=%6.1f\n",x[0],y[0],x[1],y[1]);
  SetLineColor(kBlue);
  SetLineWidth(1);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
TString CbmVisStripHit::GetInfo(){
  return Form("Hit: pdg=%i MoId=%i MoPdg=%i",
      	      fTrackPdg,fMotherId,fMotherPdg);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisStripHit::SetVisible(Bool_t isVisible){
  fVisible = isVisible;
}
// -------------------------------------------------------------------------
ClassImp(CbmVisStripHit)
