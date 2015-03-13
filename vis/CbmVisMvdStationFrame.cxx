// -------------------------------------------------------------------------
// -----                CbmVisMvdStationFrame source file             -----
// -----                Created 01/04/10 by M. Ryzhinskiy             -----
// -------------------------------------------------------------------------

#include <iostream>

#include "TClass.h"
#include "TCanvas.h"
#include "TObjArray.h"
#include "TRootEmbeddedCanvas.h"
#include "TArc.h"
#include "TMath.h"
#include "TClonesArray.h"

#include "TGFrame.h"
#include "TGCanvas.h"
#include "TGButton.h"
#include "TGScrollBar.h"
#include "TGNumberEntry.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGStatusBar.h"
#include "TEllipse.h"

#include "CbmVisMvd.h"
#include "CbmVisMvdStationFrame.h"
#include "CbmVisPoint.h"
#include "CbmVisHit.h"
#include "CbmVisPixelHit.h"
#include "CbmVisStripHit.h"

#include "CbmMvdStation.h"
#include "CbmMvdPoint.h"
#include "CbmMvdDigi.h"
#include "CbmMvdHit.h"

#include <map>



CbmVisMvdStationFrame::CbmVisMvdStationFrame(const TGWindow *p, CbmVisMvd* display): /*FOLD00*/
    TGHorizontalFrame(p)
{
  printf("CbmVisMvdStationFrame constructor\n");
  fECanvasW=600;
  fECanvasH=600;
  fRangeX = 6.;
  fRangeY = 6.;

  fDisplay = display;

  fMain = new TGVerticalFrame(this,100,100);
  fStationCombo  = new TGComboBox(fMain);


  std::map<Int_t, CbmMvdStation*> stMap = display->GetStations();
  for(std::map<Int_t, CbmMvdStation*>::iterator it = stMap.begin(); it != stMap.end(); ++it){
    fStationCombo->AddEntry(Form("Station %i",(*it).first),(*it).first);
  }


  fStationCombo->Resize(85, 20);
  fStationCombo->Connect("Selected(Int_t)","CbmVisMvdStationFrame",this,"UpdateStation(Int_t)");
  fStationCombo->Select(1,kFALSE);

  // Create scale frame
  fScaleFrame = new TGHorizontalFrame(this,100,10);
  fScaleLabel = new TGLabel(fScaleFrame,"Scale");
  fScaleBox   = new TGComboBox(fScaleFrame);
  fScaleFrame->AddFrame(fScaleLabel, new TGLayoutHints(kLHintsCenterY,0,3,0,0));
  fScaleFrame->AddFrame(fScaleBox  , new TGLayoutHints(kLHintsCenterY,3,0,0,0));
  for (Int_t i=1;i<=7;i++) fScaleBox->AddEntry(Form("x %i",Int_t(TMath::Power(2.,i-1))),i);
  fScaleBox->Resize(50,20);
  fScaleBox->Connect("Selected(Int_t)","CbmVisMvdStationFrame", this,"ScaleStation(Int_t)");
  fScaleBox->Select(3,kFALSE);

  TGLayoutHints* hints1 = new TGLayoutHints(kLHintsCenterY|kLHintsExpandX,3,3,3,3);

  fMain->AddFrame(fStationCombo, hints1);
  fMain->AddFrame(fScaleFrame  , hints1);

  fGCanvas = new TGCanvas(this,600,600);
  TGCompositeFrame* container = new TGCompositeFrame(fGCanvas->GetViewPort(),10, 10, kHorizontalFrame,TGFrame::GetWhitePixel());
  fGCanvas->SetContainer(container);
  fECanvas = new TRootEmbeddedCanvas(0,container,fECanvasW,fECanvasH);

  fGCanvas->AddFrame(fECanvas,new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,0,0,0,0));
  fCanvas  = fECanvas->GetCanvas();
  fCanvas->Range(-fRangeX,-fRangeY,fRangeX,fRangeY);
  fCanvas->SetBorderMode(0);
  fCanvas->SetGrid();
  fCanvas->SetBit(kNoContextMenu);
  fCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
   "CbmVisMvdStationFrame", this,"HandleEmbeddedCanvas(Int_t,Int_t,Int_t,TObject*)");
  fScaleX = 2*fRangeX/fCanvas->GetWw();
  fScaleY = 2*fRangeY/fCanvas->GetWh();

  this->AddFrame(fMain   ,new TGLayoutHints(kLHintsExpandY|kLHintsLeft,0,0,0,0));
  this->AddFrame(fGCanvas,new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,0,0,0,0));

  this->Resize();
  this->MapSubwindows();

  TGHScrollBar* hbar = fGCanvas->GetHScrollbar();
  TGVScrollBar* vbar = fGCanvas->GetVScrollbar();
  Int_t hpsize  = hbar->GetPageSize();
  Int_t hrange2 = hbar->GetRange();
  Int_t vpsize  = vbar->GetPageSize();
  Int_t vrange2 = vbar->GetRange();
  hbar->SetPosition((hrange2-hpsize)/2);
  vbar->SetPosition((vrange2-vpsize)/2);

  InitPixels();
  ScaleStation(3);
  UpdateStation(1);
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
CbmVisMvdStationFrame::~CbmVisMvdStationFrame(){

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMvdStationFrame::CloseWindow(){
  printf("Delete frame\n");
  delete this;
}
// -------------------------------------------------------------------------

Bool_t CbmVisMvdStationFrame::InitPixels(){
  for(Int_t stationNr=1; stationNr <= fDisplay->GetNStations(); ++stationNr){

    CbmMvdStation* station = fDisplay->GetStation(stationNr);
    fPixels[stationNr] = new TObjArray(100);
    fPoints[stationNr] = new TObjArray(100);
    fHits[stationNr] = new TObjArray(100);


    Double_t stRadius = station->GetRmax();
    Double_t innerRadius = station->GetRmin();

    // Initialize pixels
    Double_t pSize = fDisplay->GetPixelSize();
    Int_t nCols = Int_t(2*stRadius/pSize) + 1;
    Int_t nRows = nCols;
    for(Int_t iRow = 0; iRow < nRows; ++iRow){
      Double_t yCell = iRow*pSize - stRadius;
      for(Int_t iCol = 0; iCol < nCols; ++iCol){
        Double_t xCell = iCol*pSize - stRadius;
        Double_t x[5] = {xCell, xCell+pSize, xCell+pSize, xCell, xCell};
        Double_t y[5] = {yCell, yCell, yCell+pSize, yCell+pSize, yCell};

        TPolyLine* pixel = new TPolyLine(5, x, y);
        fPixels[stationNr]->Add(pixel);
        pixel->SetFillColor(42);
      }
    }
  }
  return kTRUE;
}
// -------------------------------------------------------------------------

void CbmVisMvdStationFrame::ReadPoints(){
  // Clear previous list of graphic points
  for(Int_t stationNr=1; stationNr<=2; ++stationNr)
    fPoints[stationNr]->Clear();


  TClonesArray* points = fDisplay->GetPoints();
  for (Int_t iPoint = 0; iPoint < points->GetEntriesFast(); ++iPoint) {
    CbmMvdPoint* point = (CbmMvdPoint*) points->At(iPoint);
    Int_t stationNr = point->GetStationNr();

    TEllipse* graphPoint = new TEllipse(point->GetX(), point->GetY(), fDisplay->GetPixelSize()/2);
    graphPoint->SetFillColor(2);
    graphPoint->SetLineColor(2);
    fPoints[stationNr]->Add(graphPoint);
  }
}
// -------------------------------------------------------------------------
void CbmVisMvdStationFrame::ReadHits(){
  // Clear previous list of graphic points
  for(Int_t stationNr=1; stationNr<=2; ++stationNr)
    fHits[stationNr]->Clear();

  TClonesArray* hits = fDisplay->GetHits();
  for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); ++iHit) {
    CbmMvdHit* hit = (CbmMvdHit*) hits->At(iHit);
    Int_t stationNr = hit->GetStationNr();

    TEllipse* graphHit = new TEllipse(hit->GetX(), hit->GetY(), fDisplay->GetPixelSize()/2);
    graphHit->SetFillColor(1);
    graphHit->SetLineColor(1);
    fHits[stationNr]->Add(graphHit);
  }
}
// -------------------------------------------------------------------------
void CbmVisMvdStationFrame::DrawStation(Int_t stationNr){
  fCanvas->SetEditable(kTRUE);
  fCanvas->Clear();
  fCanvas->cd();

  CbmMvdStation* station = fDisplay->GetStation(stationNr);

  for(Int_t iPixel=0; iPixel<fPixels[stationNr]->GetEntriesFast(); ++iPixel){
    TPolyLine* pixel = (TPolyLine*) fPixels[stationNr]->At(iPixel);
    pixel->Draw("f");
    pixel->Draw();
  }

  TEllipse *rMax = new TEllipse(0, 0, station->GetRmax());
  TEllipse *rMin = new TEllipse(0, 0, station->GetRmin());
  rMax->SetLineWidth(2);
  rMax->SetFillStyle(0);
  rMin->SetLineWidth(2);

  rMax->Draw("f");
  rMin->Draw("f");
  rMax->Draw();
  rMin->Draw();

  fCanvas->Update();
  fCanvas->SetEditable(kFALSE);


}
// -------------------------------------------------------------------------

void CbmVisMvdStationFrame::DrawDigis(Int_t stationNr){
  fCanvas->SetEditable(kTRUE);
  fCanvas->cd();

  for(Int_t iPixel=0; iPixel<fPixels[stationNr]->GetEntriesFast(); ++iPixel){
    TPolyLine* pixel = (TPolyLine*)fPixels[stationNr]->At(iPixel);
    pixel->SetFillColor(42);
  }

  Double_t stRadius = fDisplay->GetStation(stationNr)->GetRmax();
  Double_t pSize = fDisplay->GetPixelSize();
  Int_t nCols = Int_t(2*stRadius/pSize) + 1;

  TClonesArray* digis = fDisplay->GetDigis();
  for(Int_t iDigi=0; iDigi<digis->GetEntriesFast(); ++iDigi){
    CbmMvdDigi* digi = (CbmMvdDigi*) digis->At(iDigi);
    if( stationNr != digi->GetStationNr()) continue;

    Int_t iX = digi->GetPixelX();
    Int_t iY = digi->GetPixelY();
    Int_t iChannel = (iX) + (iY)*nCols;

    TPolyLine* pixel = (TPolyLine*)fPixels[stationNr]->At(iChannel);
    pixel->SetFillColor(4);
  }
  fCanvas->Update();
  fCanvas->SetEditable(kFALSE);
}

// -------------------------------------------------------------------------
void CbmVisMvdStationFrame::DrawHits(Int_t stationNr){
  fCanvas->SetEditable(kTRUE);
  fCanvas->cd();

  for(Int_t iHit=0; iHit<fHits[stationNr]->GetEntriesFast(); ++iHit){
    TEllipse* hit = (TEllipse*) fHits[stationNr]->At(iHit);
    hit->Draw("f");
    hit->Draw();
  }
  fCanvas->Update();
  fCanvas->SetEditable(kFALSE);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMvdStationFrame::DrawPoints(Int_t stationNr){
  fCanvas->SetEditable(kTRUE);
  fCanvas->cd();

  for(Int_t iPoint=0; iPoint<fPoints[stationNr]->GetEntriesFast(); ++iPoint){
    TEllipse* point = (TEllipse*) fPoints[stationNr]->At(iPoint);
    point->Draw("f");
    point->Draw();
  }
  fCanvas->Update();
  fCanvas->SetEditable(kFALSE);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMvdStationFrame::ScaleStation(Int_t scale){

  TGHScrollBar* hbar = fGCanvas->GetHScrollbar();
  TGVScrollBar* vbar = fGCanvas->GetVScrollbar();
  Int_t hpsize  = hbar->GetPageSize();
  Int_t hpos    = hbar->GetPosition();
  Int_t hrange1 = hbar->GetRange();
  Int_t vpsize  = vbar->GetPageSize();
  Int_t vpos    = vbar->GetPosition();
  Int_t vrange1 = vbar->GetRange();

  Double_t norm=500;
  Int_t s = Int_t( norm*TMath::Power(2.,(Double_t)scale-1.));
  fECanvas->SetWidth (s);
  fECanvas->SetHeight(s);
  fECanvasH=s;
  fECanvasW=s;
  fScaleX = 2*fRangeX/fCanvas->GetWw();
  fScaleY = 2*fRangeY/fCanvas->GetWh();

  fGCanvas->MapSubwindows();
  Int_t hrange2 = hbar->GetRange();
  Int_t vrange2 = vbar->GetRange();
  Double_t hratio = (Double_t)hrange2/(Double_t)hrange1;
  Double_t vratio = (Double_t)vrange2/(Double_t)vrange1;
  hbar->SetPosition(Int_t(hratio*(hpos+hpsize/2)-hpsize/2));
  vbar->SetPosition(Int_t(vratio*(vpos+vpsize/2)-vpsize/2));
}
// -------------------------------------------------------------------------

void CbmVisMvdStationFrame::UpdateStation(Int_t stationNr){
  DrawStation(stationNr);
  DrawPoints(stationNr);
  DrawHits(stationNr);
  DrawDigis(stationNr);
}
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
void CbmVisMvdStationFrame::HandleEmbeddedCanvas(Int_t event, Int_t px, Int_t py,
                                      TObject *sel)
{

  Double_t x=fCanvas->AbsPixeltoX(px);
  Double_t y=fCanvas->AbsPixeltoY(py);

  TString selName = sel->IsA()->GetName();

  if (event == kMouseMotion){
    fDisplay->GetStatusBar()->SetText(Form("(%.3f,%.3f)",x,y),2);

    Double_t pSize = fDisplay->GetPixelSize();

    Int_t stationNr = fStationCombo->GetSelected();
    Double_t stRadius = fDisplay->GetStation(stationNr)->GetRmax();
    Int_t nCols = Int_t(2*stRadius/pSize) + 1;
    Int_t iCol = Int_t((x+stRadius)/pSize);
    Int_t iRow = Int_t((y+stRadius)/pSize);
    Double_t xPos = (iCol+0.5)*pSize - stRadius;
    Double_t yPos = (iRow+0.5)*pSize - stRadius;
    fDisplay->GetStatusBar()->SetText(Form("Pixel (%i, %i); Center (%.4f, %.4f)", iCol, iRow, xPos, yPos), 1);
  }
}
// -------------------------------------------------------------------------
ClassImp(CbmVisMvdStationFrame)
