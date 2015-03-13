// -------------------------------------------------------------------------
// -----                     CbmVisPoint source file                  -----
// -----                  Created 02/04/07  by E. Kryshen              -----
// -------------------------------------------------------------------------
#include "CbmVisPoint.h"
#include "CbmMuchPoint.h"
#include "CbmStsPoint.h"
#include "CbmMCTrack.h"
#include "CbmMuchGeoScheme.h"


// -------------------------------------------------------------------------
CbmVisPoint::CbmVisPoint(Double_t xIn, Double_t yIn,
			   Double_t xOut, Double_t yOut,
			   Double_t r){
  fPoint = NULL;
  CreateSpread(xIn,yIn,xOut,yOut,r);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmVisPoint::~CbmVisPoint(){
 if (fMarkerIn)  delete fMarkerIn;
 if (fMarkerOut) delete fMarkerOut;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmVisPoint::CbmVisPoint(CbmMuchPoint* pt, Double_t r){
  fPoint = pt;
  Double_t xIn, yIn, xOut, yOut;
  xIn  = pt->GetXIn();
  yIn  = pt->GetYIn();
  xOut = pt->GetXOut();
  yOut = pt->GetYOut();
  CreateSpread(xIn,yIn,xOut,yOut,r);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmVisPoint::CbmVisPoint(CbmStsPoint* pt, Double_t r){
  fPoint = pt;
  Double_t xIn, yIn, xOut, yOut;
  xIn  = pt->GetXIn();
  yIn  = pt->GetYIn();
  xOut = pt->GetXOut();
  yOut = pt->GetYOut();
  CreateSpread(xIn,yIn,xOut,yOut,r);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisPoint::CreateSpread(Double_t xIn, Double_t yIn,
              		        Double_t xOut, Double_t yOut,
			        Double_t r){
  fR = r; // spread radius

  Double_t a,b,d,cosphi,sinphi;

  d = sqrt(pow(xOut-xIn,2)+pow(yOut-yIn,2)); // distance between points
  fX0  = (xIn+xOut)/2;       // x coordinate of the spread center
  fY0  = (yIn+yOut)/2;       // y coordinate of the spread center
  a  = fR + d/2;             // half-height of the spread
  b  = fR;                   // half-width of the spread

  Double_t x[5]   , y[5];    // coordinates in the spread center r.f.
  Double_t xRot[5], yRot[5]; // coordinates in the global r.f.

  x[1]=-a; y[1]=+b; x[2]=+a; y[2]=+b;
  x[0]=-a; y[0]=-b; x[3]=+a; y[3]=-b;

  cosphi = (xOut-xIn)/d;     // cos of the spread angle in xy plane
  sinphi = (yOut-yIn)/d;     // sin of the spread angle in xy plane

  // rotate and translate the spread to the global r.f.
  for (Int_t i=0;i<4;i++){
    xRot[i] = fX0 + x[i]*cosphi - y[i]*sinphi;
    yRot[i] = fY0 + x[i]*sinphi + y[i]*cosphi;
  }
  xRot[4]=xRot[0]; yRot[4]=yRot[0];

  // Set spread area properties
  SetPolyLine(5,xRot,yRot);
  //SetFillColor(45);
  //SetFillStyle(3001);
  //SetLineColor(48);
  SetFillColor(kRed);
  SetFillStyle(3001);
  SetLineColor(kRed);

  // Set in-marker properties
  fDefaultMarkerSize=1;
  fMarkerIn = new TMarker(xIn,yIn,7);
  fMarkerIn->SetMarkerColor(kRed);
  fMarkerIn->SetMarkerSize(fDefaultMarkerSize);

  // Set out-marker properties
  fMarkerOut = new TMarker(xOut,yOut,7);
  fMarkerOut->SetMarkerColor(kRed);
  fMarkerOut->SetMarkerSize(fDefaultMarkerSize);

  // Init MC point info
  fTrackId=fPoint->GetTrackID();
  fStationNr=0;
  fTrackPdg=0;
  fMotherId=-2;
  fMotherPdg=0;
  fTrack=NULL;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisPoint::SetTrack(CbmMCTrack* track){
    fTrack    = track;
    fTrackPdg = track->GetPdgCode();
    fMotherId = track->GetMotherId();
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmVisPoint::DrawSpread(){
  this->Draw("f");
  this->Draw();
  fMarkerIn->Draw();
  fMarkerOut->Draw();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
TString CbmVisPoint::GetInfo(){
  if (!fPoint) return TString("");
  Int_t iStation   = CbmMuchGeoScheme::GetStationIndex(fPoint->GetDetectorID());
  Int_t iLayer     = CbmMuchGeoScheme::GetLayerIndex(fPoint->GetDetectorID());
  Int_t iLayerSide = CbmMuchGeoScheme::GetLayerSideIndex(fPoint->GetDetectorID());
  return Form("Point:%i (%.2f,%.2f) pdg=%i MoId=%i MoPdg=%i %i %i %i",
	       fTrackId,fX0,fY0,fTrackPdg,fMotherId,fMotherPdg,iStation,iLayer,iLayerSide);
}
// -------------------------------------------------------------------------

void CbmVisPoint::SetVisible(Bool_t isVisible){
  Size_t size = (isVisible) ? fDefaultMarkerSize : 0;
  Int_t style = (isVisible) ? 3001 : 4100;
  Int_t width = (isVisible) ? 1 : 0;
  fMarkerIn->SetMarkerSize(size);
  fMarkerOut->SetMarkerSize(size);
  this->SetFillStyle(style);
  this->SetLineWidth(width);
  fVisible = isVisible;
}

ClassImp(CbmVisPoint)
