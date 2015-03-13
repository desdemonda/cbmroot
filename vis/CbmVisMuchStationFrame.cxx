// -------------------------------------------------------------------------
// -----                CbmVisMuchStationFrame source file             -----
// -----                   Created 26/03/07 by E. Kryshen              -----
// -------------------------------------------------------------------------

#include <iostream>

#include "TClass.h"
#include "TCanvas.h"
#include "TObjArray.h"
#include "TRootEmbeddedCanvas.h"
#include "TArc.h"
#include "TMath.h"

#include "TGFrame.h"
#include "TGComboBox.h"
#include "TGCanvas.h"
#include "TGButton.h"
#include "TGScrollBar.h"
#include "TGNumberEntry.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGStatusBar.h"

#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#include "CbmMuchSector.h"
#include "CbmMuchGeoScheme.h"
#include "CbmVisMuch.h"
#include "CbmVisMuchStationFrame.h"
#include "CbmVisMuchModuleFrame.h"
#include "CbmVisPoint.h"
#include "CbmVisHit.h"
#include "CbmVisPixelHit.h"
#include "CbmVisStripHit.h"

CbmVisMuchStationFrame::CbmVisMuchStationFrame(const TGWindow *p, CbmVisMuch* display): /*FOLD00*/
    TGHorizontalFrame(p)
{
  printf("CbmVisMuchStationFrame constructor\n");
  fECanvasW=600;
  fECanvasH=600;
  fRangeX = 250.;
  fRangeY = 250.;

  fDisplay = display;
  fGeoScheme = CbmMuchGeoScheme::Instance();
  fGeoScheme->GetNStations();

  fMain = new TGVerticalFrame(this,100,100);
  fStationCombo  = new TGComboBox(fMain);
  fLayerCombo    = new TGComboBox(fMain);

  fStationNr = 1;
  fLayerNr = 1;
  fShowSide[0] = 1;
  fShowSide[1] = 1;
  fShowModules = 1;
  fShowSectors = 1;
  fShowPoints  = 1;
  fShowHits    = 1;

  for (Int_t st=0;st<fGeoScheme->GetNStations();st++){
    fStationCombo->AddEntry(Form("Station %i",st+1),st+1);
  }

  fStationCombo->Resize(85, 20);
  fStationCombo->Connect("Selected(Int_t)","CbmVisMuchStationFrame",this,"UpdateStation(Int_t)");
  fStationCombo->Select(fStationNr,kFALSE);

  fLayerCombo->Resize(85, 20);
  fLayerCombo->Connect("Selected(Int_t)","CbmVisMuchStationFrame",this,"UpdateLayer(Int_t)");


  // Create scale frame
  fScaleFrame = new TGHorizontalFrame(this,100,10);
  fScaleLabel = new TGLabel(fScaleFrame,"Scale");
  fScaleBox   = new TGComboBox(fScaleFrame);
  fScaleFrame->AddFrame(fScaleLabel, new TGLayoutHints(kLHintsCenterY,0,3,0,0));
  fScaleFrame->AddFrame(fScaleBox  , new TGLayoutHints(kLHintsCenterY,3,0,0,0));
  for (Int_t i=1;i<=3;i++) fScaleBox->AddEntry(Form("x %i",Int_t(TMath::Power(2.,i-1))),i);
  fScaleBox->Resize(50,20);
  fScaleBox->Connect("Selected(Int_t)","CbmVisMuchStationFrame", this,"DoScaleStation(Int_t)");
  fScaleBox->Select(1,kFALSE);

  TGLayoutHints* hints = new TGLayoutHints(kLHintsExpandX,0,0,0,0);

  // Create layout frame
  fLayoutFrame = new TGGroupFrame(fMain,"Show/hide");
  // Create checkbox to show/hide front side
  fSideFVisibleBtn = new TGCheckButton(fLayoutFrame,"Front layer side",96);
  fSideFVisibleBtn->SetState(kButtonDown);
  fSideFVisibleBtn->Connect("Clicked()","CbmVisMuchStationFrame",this,"HandleButtons()");
  fLayoutFrame->AddFrame(fSideFVisibleBtn,hints);
  // Create checkbox to show/hide back side
  fSideBVisibleBtn = new TGCheckButton(fLayoutFrame,"Back layer side",97);
  fSideBVisibleBtn->SetState(kButtonDown);
  fSideBVisibleBtn->Connect("Clicked()","CbmVisMuchStationFrame",this,"HandleButtons()");
  fLayoutFrame->AddFrame(fSideBVisibleBtn,hints);
  // Create checkbox to show/hide modules
  fModulesVisibleBtn = 0;
  fModulesVisibleBtn = new TGCheckButton(fLayoutFrame,"Modules",98);
  fModulesVisibleBtn->SetState(kButtonDown);
  fModulesVisibleBtn->Connect("Clicked()","CbmVisMuchStationFrame",this,"HandleButtons()");
  fLayoutFrame->AddFrame(fModulesVisibleBtn,hints);
  // Create checkbox to show/hide sectors
  fSectorsVisibleBtn = 0;
  fSectorsVisibleBtn = new TGCheckButton(fLayoutFrame,"Sectors",99);
  fSectorsVisibleBtn->SetState(kButtonDown);
  fSectorsVisibleBtn->Connect("Clicked()","CbmVisMuchStationFrame",this,"HandleButtons()");
  fLayoutFrame->AddFrame(fSectorsVisibleBtn,hints);








  // Create checkbox to show/hide points
  fPointsVisibleBtn = new TGCheckButton(fLayoutFrame,"Points",100);
  fPointsVisibleBtn->SetState(kButtonDown);
  fPointsVisibleBtn->Connect("Clicked()","CbmVisMuchStationFrame",this,"HandleButtons()");
  fLayoutFrame->AddFrame(fPointsVisibleBtn,hints);
  // Create checkbox to show/hide hits
  fHitsVisibleBtn = new TGCheckButton(fLayoutFrame,"Hits",101);
  fHitsVisibleBtn->SetState(kButtonDown);
  fHitsVisibleBtn->Connect("Clicked()","CbmVisMuchStationFrame",this,"HandleButtons()");
  fLayoutFrame->AddFrame(fHitsVisibleBtn,hints);

  Int_t fPdgs[10]={2212,211,-211,321,-321,11,-11,13,-13,0};
  Int_t fMoPdgs[10]={-1,22,211,-211,321,-321,3122,3312,3334,0};

  fPdgFrame = new TGGroupFrame(fMain,"Select track");
  for (Int_t i=0;i<10;i++) {
      TString name = Form("%i",fPdgs[i]);
      if (fPdgs[i]==0) name = "Other";
      fPdgBtn[i] = new TGCheckButton(fPdgFrame,name,i);
      fPdgBtn[i]->SetState(kButtonDown);
      fPdgBtn[i]->Connect("Clicked()","CbmVisMuchStationFrame",this,"HandleButtons()");
      fPdgFrame->AddFrame(fPdgBtn[i],hints);
  }

  fMoPdgFrame = new TGGroupFrame(fMain,"Select mother");
  for (Int_t i=0;i<10;i++) {
      TString name = Form("%i",fMoPdgs[i]);
      if (fMoPdgs[i]==0)  name = "Other";
      if (fMoPdgs[i]==-1) name = "Primary";
      fMoPdgBtn[i] = new TGCheckButton(fMoPdgFrame,name,20+i);
      fMoPdgBtn[i]->SetState(kButtonDown);
      fMoPdgBtn[i]->Connect("Clicked()","CbmVisMuchStationFrame",this,"HandleButtons()");
      fMoPdgFrame->AddFrame(fMoPdgBtn[i],hints);
  }

/*
  fEntryPtId    = new TGNumberEntry(fSelectionFrame);
  fEntryMcId    = new TGNumberEntry(fSelectionFrame);
  fEntryMcPdg   = new TGNumberEntry(fSelectionFrame);
  fEntryMoId    = new TGNumberEntry(fSelectionFrame);
  fEntryMoPdg   = new TGNumberEntry(fSelectionFrame);
  fSelectButton = new TGTextButton(fSelectionFrame,"Select");

  fEntryPtId->SetText("PointId");


  fSelectionFrame->AddFrame(fEntryPtId,hints);
  fSelectionFrame->AddFrame(fEntryMcId,hints);
  fSelectionFrame->AddFrame(fEntryMcPdg,hints);
  fSelectionFrame->AddFrame(fEntryMoId,hints);
  fSelectionFrame->AddFrame(fEntryMoPdg,hints);
  fSelectionFrame->AddFrame(fSelectButton,hints);
*/

  TGLayoutHints* hints1 = new TGLayoutHints(kLHintsCenterY|kLHintsExpandX,3,3,3,3);

  fMain->AddFrame(fStationCombo, hints1);
  fMain->AddFrame(fLayerCombo  , hints1);
  fMain->AddFrame(fScaleFrame  , hints1);
  fMain->AddFrame(fLayoutFrame , hints1);
  fMain->AddFrame(fPdgFrame    , hints1);
  fMain->AddFrame(fMoPdgFrame  , hints1);

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
   "CbmVisMuchStationFrame", this,"HandleEmbeddedCanvas(Int_t,Int_t,Int_t,TObject*)");
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

  UpdateStation(fStationNr);

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmVisMuchStationFrame::~CbmVisMuchStationFrame(){
/*
  if (fSectors) {fSectors->Delete(); delete fSectors;}

   if (fStationCombo)  delete fStationCombo;
   if (fScaleLabel)    delete fScaleLabel;
   if (fScaleBox)      delete fScaleBox;
   if (fScaleFrame)    delete fScaleFrame;
   if (fECanvas)       delete fECanvas;
   if (fGCanvas)       delete fGCanvas;
   if (fMain)          delete fMain;
*/
//   if (fPdgFrame)
//   if (fMoPrgFrame)
//   if (fPdgBtn)
//   if (fMoPdgBtn)
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuchStationFrame::CloseWindow(){
  printf("Delete frame\n");
  delete this;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuchStationFrame::DoDrawStation(Int_t stationNr, Int_t layerNr){
  if (stationNr!=0) fStationNr = stationNr;
  if (layerNr!=0) fLayerNr = layerNr;
  printf("DoDrawStation:%i layer %i\n",fStationNr,fLayerNr);

  fLayerCombo->Select(fLayerNr,kFALSE);
  fCanvas->SetEditable(kTRUE);
  fCanvas->Clear();
  DrawModules();

  fCanvas->SetEditable(kTRUE);
  fCanvas->cd();
  CbmMuchStation* station = fGeoScheme->GetStation(fStationNr-1);
  Double_t rmin = station->GetRmin();
  Double_t rmax = station->GetRmax();
  TArc* cMin = new TArc(0,0,rmin,0,360);
  cMin->SetFillColor(10);
  cMin->Draw("f");
  TArc* cMax = new TArc(0,0,rmax,0,360);
  cMax->SetFillStyle(0);
  cMax->Draw();
  fCanvas->Update();
  fCanvas->SetEditable(kFALSE);

//  SetInfo(station);

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmVisMuchStationFrame::DrawHits(){
  if (!fShowHits) return;
  fCanvas->SetEditable(kTRUE);
  fCanvas->cd();

  for (Int_t s=0;s<2;s++){
    if (!fShowSide[s]) continue;
    CbmMuchLayerSide* side = fGeoScheme->GetLayerSide(fStationNr-1,fLayerNr-1,s);
    for (Int_t m=0;m<side->GetNModules();m++){
      CbmMuchModule* module = side->GetModule(m);
      TClonesArray* hits = module->GetHits();
      if (module->GetDetectorType()==1) {
        for (Int_t i=0;i<hits->GetEntriesFast();i++){
          CbmVisPixelHit* hit = (CbmVisPixelHit*) hits->At(i);
          hit->Draw();
        }
      }
      else if (module->GetDetectorType()==2) {
        for (Int_t i=0;i<hits->GetEntriesFast();i++){
          CbmVisStripHit* hit = (CbmVisStripHit*) hits->At(i);
          hit->Draw();
        }
      }
    }
  }

  fCanvas->Update();
  fCanvas->SetEditable(kFALSE);

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuchStationFrame::DrawPoints(){
  if (!fShowPoints) return;
  fCanvas->SetEditable(kTRUE);
  fCanvas->cd();

  for (Int_t s=0;s<2;s++){
    if (!fShowSide[s]) continue;
    CbmMuchLayerSide* side = fGeoScheme->GetLayerSide(fStationNr-1,fLayerNr-1,s);
    for (Int_t m=0;m<side->GetNModules();m++){
      CbmMuchModule* module = side->GetModule(m);
      TClonesArray* points = module->GetPoints();
      for (Int_t i=0;i<points->GetEntriesFast();i++){
        CbmVisPoint* point = (CbmVisPoint*) points->At(i);
        point->DrawSpread();
      }
    }
  }

  fCanvas->Update();
  fCanvas->SetEditable(kFALSE);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuchStationFrame::DrawModules(){
  fCanvas->SetEditable(kTRUE);
  fCanvas->cd();

  for (Int_t s=1;s>=0;s--){
    if (!fShowSide[s]) continue;
    CbmMuchLayerSide* side = fGeoScheme->GetLayerSide(fStationNr-1,fLayerNr-1,s);
    Color_t color = (s) ? kOrange : kYellow;
    side->DrawModules(color,fShowModules,fShowSectors);
  }

  fCanvas->Update();
  fCanvas->SetEditable(kFALSE);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMuchStationFrame::DoScaleStation(Int_t scale){

  TGHScrollBar* hbar = fGCanvas->GetHScrollbar();
  TGVScrollBar* vbar = fGCanvas->GetVScrollbar();
  Int_t hpsize  = hbar->GetPageSize();
  Int_t hpos    = hbar->GetPosition();
  Int_t hrange1 = hbar->GetRange();
  Int_t vpsize  = vbar->GetPageSize();
  Int_t vpos    = vbar->GetPosition();
  Int_t vrange1 = vbar->GetRange();

  Double_t norm=500;
  //Int_t s_old = fECanvas->GetWidth();
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
  //if (hrange2<hpsize) hbar->SetPosition(0);
  //if (vrange2<vpsize) vbar->SetPosition(0);
  //if (s_old < hpsize && hrange2>hpsize)
  //hbar->SetPosition((hrange2-hpsize)/2);
  //if (s_old < vpsize && vrange2>vpsize)
  //hbar->SetPosition((hrange2-hpsize)/2);
  //vbar->SetPosition((vrange2-vpsize)/2);
  //fGCanvas->MapSubwindows();

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmVisMuchStationFrame::SetInfo(CbmMuchStation* station){
/*
  TString name =Form("Station %i: Z=%.fcm, D=%.0fmm, Rmin=%.1fcm, Rmax=%.1fcm",
         fStationNr,station->GetZ(),10*station->GetD(),
         station->GetRmin(),station->GetRmax());
  fDisplay->SetWindowName(name);

  TGStatusBar* sbar = fDisplay->GetStatusBar();
  sbar->SetText(Form("NoSectors=%3i",station->GetNSectors()),0);
*/
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmVisMuchStationFrame::HandleEmbeddedCanvas(Int_t event, Int_t px, Int_t py,
                                      TObject *sel)
{

  Double_t x=fCanvas->AbsPixeltoX(px);
  Double_t y=fCanvas->AbsPixeltoY(py);

  TString selName = sel->IsA()->GetName();

  if (event == kButton1Down) {
    if (!strcmp(selName,"CbmMuchSector")) {
      CbmMuchSector* sector = (CbmMuchSector*) sel;
      CbmMuchModule* module = fGeoScheme->GetModuleByDetId(sector->GetDetectorId());
      if (module->GetDetectorType()!=1) return;
      CbmMuchModuleGem* gem_module = (CbmMuchModuleGem*) module;
      CbmVisMuchModuleFrame* moduleFrame = new CbmVisMuchModuleFrame(gem_module,fDisplay);
      fDisplay->GetOpenModules()->AddAtFree(moduleFrame);
    }

    if (!strcmp(selName,"CbmMuchModuleGem")) {
      CbmMuchModuleGem* gem_module = (CbmMuchModuleGem*) sel;
      CbmVisMuchModuleFrame* moduleFrame
         = new CbmVisMuchModuleFrame(gem_module,fDisplay);
      fDisplay->GetOpenModules()->AddAtFree(moduleFrame);
    }
//    for(Int_t i=0;i<fSectors->GetEntriesFast();i++){
//      CbmMuchModule*
//      CbmMuchSector* sector = gsector->GetSector();
//      if (!sector->Inside(x,y)) continue;
//      printf("sector: %i\n",i+1);
//      CbmVisMuchSectorFrame* sectorFrame
//        = new CbmVisMuchSectorFrame(gsector,fDisplay);
//      fDisplay->GetOpenSectors()->AddAtFree(sectorFrame);
//      break;
//    }
  }

  if (event == kMouseMotion){
    fDisplay->GetStatusBar()->SetText(Form("(%.1f,%.1f)",x,y),2);
    if (!strcmp(selName,"CbmMuchModule")) {
      CbmMuchModule* module = (CbmMuchModule*) sel;
      fDisplay->GetStatusBar()->SetText("Module",1);
    } else if (!strcmp(selName,"CbmMuchSector")) {
      CbmMuchSector* sector = (CbmMuchSector*) sel;
      fDisplay->GetStatusBar()->SetText("Sector",1);
    } else if (!strcmp(selName,"CbmVisPoint")) {
      CbmVisPoint* vpoint = (CbmVisPoint*) sel;
      fDisplay->GetStatusBar()->SetText(vpoint->GetInfo(),1);
    } else if (!strcmp(selName,"CbmVisHit")) {
      CbmVisHit* vhit = (CbmVisHit*) sel;
      fDisplay->GetStatusBar()->SetText(vhit->GetInfo(),1);
    } else {
      fDisplay->GetStatusBar()->SetText("",1);
    }
  }
}
// -------------------------------------------------------------------------

void CbmVisMuchStationFrame::UpdateStation(Int_t stationNr){
  Int_t layerNr = 1;

  if (stationNr==0) { stationNr = fStationNr; layerNr = fLayerNr; }
  DoDrawStation(stationNr,layerNr);
  DrawPoints();
  DrawHits();

  // Update selector of layers
  CbmMuchStation* station = fGeoScheme->GetStation(stationNr-1);
  fLayerCombo->RemoveAll();
  for (Int_t l=0;l<station->GetNLayers();l++){
    fLayerCombo->AddEntry(Form("Layer %i",l+1),l+1);
  }
  fLayerCombo->Select(fLayerNr,kFALSE);

}


void CbmVisMuchStationFrame::UpdateLayer(Int_t layerNr){
  if (layerNr==0) { layerNr = fLayerNr; }
  DoDrawStation(fStationNr,layerNr);
  DrawPoints();
  DrawHits();
}


// -------------------------------------------------------------------------
void CbmVisMuchStationFrame::HandleButtons(){

  TGCheckButton *btn = (TGCheckButton*) gTQSender;
  Int_t id   = btn->WidgetId();
  Int_t isOn = btn->IsOn();
  printf("DoButton: id = %d\n",id);

  // SetFrontSideVisible
  if (id==96) {
    fShowSide[0] = !fShowSide[0];
    UpdateLayer();
  }

  if (id==97) {
    fShowSide[1] = !fShowSide[1];
    UpdateLayer();
  }

  // SetFrontSideVisible
  if (id==98) {
    fShowModules = !fShowModules;
    UpdateLayer();
  }

  if (id==99) {
    fShowSectors = !fShowSectors;
    UpdateLayer();
  }

  if (id==100) {
    fShowPoints = !fShowPoints;
    UpdateLayer();
  }

  if (id==100) {
    fShowHits = !fShowHits;
    UpdateLayer();
  }

  /*
  // Set points visible
  if (id==100) {
    for (Int_t st=1;st<=fDisplay->GetNStations();st++){
      TClonesArray* points = fDisplay->GetVisPoints(st);
      for (Int_t i=0;i<points->GetEntriesFast();i++){
        CbmVisPoint* point = (CbmVisPoint*) points->At(i);
        Int_t pdg=point->GetTrackPdg();
        if (ShowTrack(pdg)) point->SetVisible(isOn);
      }
    }
  }

  if (id==101) {
    for (Int_t st=1;st<=fDisplay->GetNStations();st++){
      TClonesArray* hits = fDisplay->GetVisHits(st);
      for (Int_t i=0;i<hits->GetEntriesFast();i++){
        CbmVisHit* hit = (CbmVisHit*) hits->At(i);
        hit->SetVisible(isOn);
      }
    }
  }

  Int_t fPdgs[10]={2212,211,-211,321,-321,11,-11,13,-13,0};
//  Int_t fMoPdgs[10]={-1,22,211,-211,321,-321,3122,3312,3334,0};

  for (Int_t st=1;st<=fDisplay->GetNStations();st++){

    TClonesArray* points = fDisplay->GetVisPoints(st);
    for (Int_t i=0;i<points->GetEntriesFast();i++){
      CbmVisPoint* point = (CbmVisPoint*) points->At(i);
      Int_t pdg=point->GetTrackPdg();
//      Int_t moPdg=point->GetMotherPdg();
      if (id==9) {
          Bool_t isInTheList=0;
	  for (Int_t iPdg=0;iPdg<9;iPdg++){
	      if (pdg==fPdgs[iPdg]) {isInTheList=1; break;}
	  }
          if (!isInTheList) point->SetVisible(isOn);
      }
      else if (id>=0 && id <9){
          if (fPdgs[id]!=pdg) continue;
	  point->SetVisible(isOn);
//          cout << pdg << endl;
      }
    }

  }


  fCanvas->Modified();
  fCanvas->Update();

  TObjArray* openSectors = fDisplay->GetOpenSectors();
  for (Int_t i=0;i<openSectors->GetEntriesFast();i++){
    CbmVisMuchSectorFrame* secFrame = (CbmVisMuchSectorFrame*) openSectors->At(i);
    if (!secFrame) continue;
    secFrame->GetCanvas()->Modified();
    secFrame->GetCanvas()->Update();
  }
*/
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
bool CbmVisMuchStationFrame::ShowTrack(Int_t pdg){
/*
  Int_t fPdgs[10]={2212,211,-211,321,-321,11,-11,13,-13,0};
  // check if pdg is in the list of available pdg codes
  Int_t btnId=-1;
  for (Int_t iPdg=0;iPdg<9;iPdg++){
    if (pdg==fPdgs[iPdg]) {btnId=iPdg; break;}
  }
  // if not in the list than check "other" check box
  if (btnId<0) return fPdgBtn[9]->IsOn();
  return fPdgBtn[btnId]->IsOn();
*/
}
// -------------------------------------------------------------------------



ClassImp(CbmVisMuchStationFrame)
