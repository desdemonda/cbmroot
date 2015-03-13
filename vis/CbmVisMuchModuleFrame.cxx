// -------------------------------------------------------------------------
// -----                CbmVisMuchModuleFrame source file              -----
// -----                  Created 14/03/07  by E. Kryshen              -----
// -------------------------------------------------------------------------
#include <map>


#include "TClass.h"
#include "TMath.h"
#include "CbmVisMuchModuleFrame.h"

#include "TClonesArray.h"
#include "TCanvas.h"
#include "TRootEmbeddedCanvas.h"
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGStatusBar.h"
#include "TGSlider.h"
#include "TGCanvas.h"
#include "TGComboBox.h"
#include "TGButton.h"
#include "TArc.h"


#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchStation.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchSector.h"
#include "CbmVisMuch.h"
#include "CbmMuchPad.h"
#include "CbmVisPoint.h"
#include "CbmVisHit.h"
#include "CbmVisPixelHit.h"
#include "CbmVisMuchCluster.h"
#include "CbmMuchGeoScheme.h"
using std::map;
// -------------------------------------------------------------------------
CbmVisMuchModuleFrame::CbmVisMuchModuleFrame(CbmMuchModuleGem* module, CbmVisMuch* display){
  fNHits = 0;
  fNPoints = 0;
  fPlineSize = 0;
  fModule = module;
  fDisplay = display;
  fStationNr = 0; // TODO
  fModuleNr  = 0; // TODO
  Double_t x0, y0, lx, ly;
  lx = fModule->GetSize()[0];
  ly = fModule->GetSize()[1];
  x0 = fModule->GetPosition()[0];
  y0 = fModule->GetPosition()[1];
  Double_t l = 2*sqrt(lx*lx+ly*ly)/2;
  Int_t canvas_lx = 800;
  Int_t canvas_ly = Int_t(canvas_lx*ly/lx);

  fMain = new TGMainFrame(gClient->GetRoot(),800,800,kVerticalFrame);
  fMain->Connect("CloseWindow()", "CbmVisMuchModuleFrame", this, "CloseWindow()");

  //Button Frame
  fButtonFrame = new TGHorizontalFrame(fMain,500,100);


  fScale = new TGComboBox(fButtonFrame,1);
  for (Int_t i=1;i<=4;i++) fScale->AddEntry(Form("x %i",Int_t(TMath::Power(2.,i-1))),i);
  fScale->Resize(90,20);
  fScale->Connect("Selected(Int_t)","CbmVisMuchModuleFrame", this,"DoScale(Int_t)");
  fScale->Select(1,kFALSE);
  fScaleValue = 1;

//  fMain->AddFrame(fScale    ,new TGLayoutHints(kLHintsTop|kLHintsLeft,3,3,3,3));
  fButtonFrame->AddFrame(fScale,new TGLayoutHints(kLHintsTop|kLHintsLeft,3,3,3,3));

  fPrintButton = new TGTextButton(fButtonFrame," &Print ");
  fButtonFrame->AddFrame(fScale,new TGLayoutHints(kLHintsTop|kLHintsLeft,3,3,3,3));
  fButtonFrame->AddFrame(fPrintButton,new TGLayoutHints(kLHintsTop|kLHintsLeft,3,3,3,3));

  fPrintButton->Connect("Clicked()","CbmVisMuchModuleFrame",this,"PrintCanvas()");


  Int_t parts[] = {15,70,15};
  fStatusBar = new TGStatusBar(fMain,50,10,kHorizontalFrame);
  fStatusBar->SetParts(parts,3);

  fGCanvas = new TGCanvas(fMain,canvas_lx,canvas_ly);
  TGCompositeFrame* container = new TGCompositeFrame(fGCanvas->GetViewPort(),10, 10, kHorizontalFrame,TGFrame::GetWhitePixel());
  fGCanvas->SetContainer(container);

  fECanvas = new TRootEmbeddedCanvas(0,container,canvas_lx,canvas_ly);
  fGCanvas->AddFrame(fECanvas,new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,0,0,0,0));

  fCanvas = fECanvas->GetCanvas();
  fCanvas->SetBorderMode(0);
  fCanvas->SetBit(kNoContextMenu);
  fCanvas->cd();
  fCanvas->Clear();
  fCanvas->Range(x0-lx/2.,y0-ly/2.,x0+lx/2.,y0+ly/2.);
//  fCanvas->Range(-100,-100,100,100);
//  sec->SetFillColor(10);
//  sec->Draw("f");
//  sec->Draw();

// TODO
//  fGSector->CreatePads();
//  fStatusBar->SetText(Form("Occupancy:%4.1f",100*fGSector->CalcOccupancy()),0);

  fCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
   "CbmVisMuchModuleFrame", this,"HandleEmbeddedCanvas(Int_t,Int_t,Int_t,TObject*)");

  fMain->AddFrame(fButtonFrame , new TGLayoutHints(kLHintsTop|kLHintsCenterX,2,2,2,2));
  fMain->AddFrame(fGCanvas  ,new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,3,3,3,3));
  fMain->AddFrame(fStatusBar,new TGLayoutHints(kLHintsBottom|kLHintsLeft |kLHintsExpandX,0,0,2,0));

  fMain->MapSubwindows();
  fMain->Resize();



  TString wname = Form("Station %i: Sector %i at (%.2f,%.2f) cm, size: (%.1f,%.1f)cm",
     fStationNr,fModuleNr,x0,y0,lx,ly);
  fMain->SetWindowName(wname);
  fMain->MapWindow();

  TGHScrollBar* hbar = fGCanvas->GetHScrollbar();
  TGVScrollBar* vbar = fGCanvas->GetVScrollbar();
  Int_t hpsize  = hbar->GetPageSize();
  Int_t hrange2 = hbar->GetRange();
  Int_t vpsize  = vbar->GetPageSize();
  Int_t vrange2 = vbar->GetRange();
  hbar->SetPosition((hrange2-hpsize)/2);
  vbar->SetPosition((vrange2-vpsize)/2);

  UpdateModule();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmVisMuchModuleFrame::~CbmVisMuchModuleFrame(){
  if (fScale)     delete fScale;
  if (fStatusBar) delete fStatusBar;
  if (fGCanvas)   delete fGCanvas;
  if (fECanvas)   delete fECanvas;
  fDisplay->GetOpenModules()->Remove(this);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuchModuleFrame::CloseWindow(){
   delete this;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuchModuleFrame::DoScale(Int_t scale){
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
  fScaleValue = scale;
  fECanvas->SetWidth (s);
  fECanvas->SetHeight(s);

  fGCanvas->MapSubwindows();
  Int_t hrange2 = hbar->GetRange();
  Int_t vrange2 = vbar->GetRange();
  Double_t hratio = (Double_t)hrange2/(Double_t)hrange1;
  Double_t vratio = (Double_t)vrange2/(Double_t)vrange1;
  hbar->SetPosition(Int_t(hratio*(hpos+hpsize/2)-hpsize/2));
  vbar->SetPosition(Int_t(vratio*(vpos+vpsize/2)-vpsize/2));
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuchModuleFrame::HandleEmbeddedCanvas(Int_t event, Int_t px, Int_t py,
                                      TObject *sel)
{
    Double_t x=fCanvas->AbsPixeltoX(px);
    Double_t y=fCanvas->AbsPixeltoY(py);

    TString selName = sel->IsA()->GetName();

    if (event == kButton1Down) {
      if (!strcmp(selName,"CbmMuchPad")) {
        CbmMuchPad* pad = (CbmMuchPad*) sel;
        Int_t id = pad->GetDigiIndex();
        if (id<0) return;
        TClonesArray* digiMatches = fDisplay->GetDigiMatches();
        CbmMuchDigiMatch* match = (CbmMuchDigiMatch*)  digiMatches->At(id);
//        Int_t clusterId = match->GetClusterIndex();
//        TClonesArray* visClusters = fDisplay->GetVisClusters();
//        CbmVisMuchCluster* cluster = (CbmVisMuchCluster*) visClusters->At(clusterId);
//        cluster->PrintInfo();
//        cluster->DrawHisto();
      }
    }

//    if (event == kButton3Down) {
//      if (!strcmp(selName,"CbmVisPad")) {
//        CbmPad* vpad = (CbmVisPad*) sel;
//        CbmMuchSector* sector = vpad->GetSector();
//        CbmMuchPad* mpad = sector->GetPad(vpad->GetChannel());
//        vector<CbmMuchPad*> neighbours=mpad->GetNeighbours();
//        vector<CbmMuchPad*>::iterator it;
//        fPlineSize = neighbours.size();
////        fPline = new TPolyLine[fPlineSize];
//        printf("%i\n",fPlineSize);
//        Int_t i=0;
//        for(it=neighbours.begin(); it!=neighbours.end(); it++){
//          CbmMuchPad* pad = *it;
////          fStatusBar->SetText(Form("%i",CbmMuchGeoScheme::GetChannelIndex(pad->GetChannelId())),2);
////          printf("%i\n",CbmMuchGeoScheme::GetChannelIndex(pad->GetChannelId()));
//          fCanvas->SetEditable(kTRUE);
//          fCanvas->cd();
////          TPolyLine pline = pad->GetPadPolygon();
////          fPline[i] = ;
////          fPline[i].SetFillColor(kGreen);
////          fPline[i].Draw("f");
//          fCanvas->Update();
//          fCanvas->SetEditable(kFALSE);
//          i++;
//        }

    if (event == kButton3Up && fPlineSize>0) {
//       delete [] fPline;
//       fPlineSize = 0;
    }

    if (event == kMouseMotion){
      fStatusBar->SetText(Form("(%.2f,%.2f)",x,y),2);
      if (!strcmp(selName,"CbmVisPoint")) {
        CbmVisPoint* vpoint = (CbmVisPoint*) sel;
        fStatusBar->SetText(vpoint->GetInfo(),1);
      }
      else if (!strcmp(selName,"CbmVisHit")) {
        CbmVisHit* vhit = (CbmVisHit*) sel;
        fStatusBar->SetText(vhit->GetInfo(),1);
      }
      else if (!strcmp(selName,"CbmMuchPad")) {
        CbmMuchPad* vpad = (CbmMuchPad*) sel;
        fStatusBar->SetText(vpad->GetInfo(),1);
      }
      else {
        fStatusBar->SetText("",1);
      }
  }

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmVisMuchModuleFrame::DrawHits(){
  fNHits=0;
  TClonesArray* hits = fModule->GetHits();
  for (Int_t i=0;i<hits->GetEntriesFast();i++){
    CbmVisPixelHit* hit = (CbmVisPixelHit*) hits->At(i);
    hit->SetMarkerSize(1);
    hit->SetMarkerStyle(20);
    hit->Draw();
    fNHits++;
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuchModuleFrame::DrawPoints(){
  fNPoints=0;
  TClonesArray* points = fModule->GetPoints();
  for (Int_t i=0;i<points->GetEntriesFast();i++){
    CbmVisPoint* point = (CbmVisPoint*) points->At(i);
    point->DrawSpread();
//    point->GetMarkerIn()->Draw();
//    point->GetMarkerOut()->Draw();
    fNPoints++;
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmVisMuchModuleFrame::UpdateModule(){
  fCanvas->SetEditable(kTRUE);
  fCanvas->cd();

  for (Int_t i=0;i<fModule->GetNSectors();i++){
    CbmMuchSector* sector = fModule->GetSector(i);
//    sector->Draw("f");
//    sector->Draw();
    for (Int_t p=0;p<sector->GetNChannels();p++){
      CbmMuchPad* pad = sector->GetPad(p);
      pad->Draw("f");
      pad->Draw();
    }
    sector->SetLineWidth(2);
    sector->SetLineColor(kGreen+3);
    sector->Draw();
  }

  CbmMuchStation* station = CbmMuchGeoScheme::Instance()->GetStationByDetId(fModule->GetDetectorId());
  Double_t rmin = station->GetRmin();
  TArc* cMin = new TArc(0,0,rmin,0,360);
  cMin->SetFillColor(10);
  cMin->SetLineColor(kGreen+3);
  cMin->SetLineWidth(2);
  cMin->Draw("f");

  DrawPoints();
  DrawHits();
  fCanvas->Update();
  fCanvas->SetEditable(kFALSE);
//  fCanvas->Print("station2.png");
//  fStatusBar->SetText(Form("Occupancy:%4.1f",100*fGSector->GetOccupancy()),0);
}
// -------------------------------------------------------------------------


void CbmVisMuchModuleFrame::PrintCanvas(){
  fCanvas->Print("module_view.eps");
}

ClassImp(CbmVisMuchModuleFrame)

