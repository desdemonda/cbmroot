// -------------------------------------------------------------------------
// -----                CbmVisMuchStationFrame header file             -----
// -----                  Created 26/03/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

/** CbmVisMuchStationFrame
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 26.03.07
 *@version 1.0
 **
 **/
#ifndef CBMVISMUCHSTATIONFRAME_H
#define CBMVISMUCHSTATIONFRAME_H 1


#include "TGFrame.h"
#include "RQ_OBJECT.h"

class TRootEmbeddedCanvas;
class TObjArray;
class TCanvas;

class TGComboBox;
class TGCanvas;
class TGHorizontalFrame;
class TGNumberEntry;
class TGGroupFrame;
class TGTextButton;
class TGCheckButton;
class TGLabel;

class CbmVisMuch;
class CbmMuchGeoScheme;

class CbmVisMuchStationFrame: public TGHorizontalFrame{
 RQ_OBJECT("CbmVisMuchStationFrame")
 ClassDef(CbmVisMuchStationFrame,1);

 public:
  CbmVisMuchStationFrame(const TGWindow *p, CbmVisMuch* task);
  ~CbmVisMuchStationFrame();
  void DoDrawStation(Int_t stationNr=0,Int_t layer=0);
  void DoScaleStation(Int_t scale);
  void HandleEmbeddedCanvas(Int_t event,Int_t x,Int_t y,TObject *sel);
  void HandleButtons();
  void UpdateStation(Int_t stationNr=0);
  void UpdateLayer(Int_t layerNr=0);
  void DrawHits();
  void DrawPoints();
  void DrawModules();
  void SetInfo(CbmMuchStation* station);
  void CloseWindow();
  Int_t GetStationNr() {return fStationNr;}
  TObjArray* GetSectors() { return fSectors; }

private:
  bool ShowTrack(Int_t pdg);

private:
  CbmMuchGeoScheme*    fGeoScheme;
  TGVerticalFrame*     fMain;
  TGComboBox*          fStationCombo;
  TGComboBox*          fLayerCombo;
  TGCanvas*            fGCanvas;
  TRootEmbeddedCanvas* fECanvas;
  TCanvas*             fCanvas;
  TGGroupFrame*        fLayoutFrame;
  TGGroupFrame*        fPdgFrame;
  TGGroupFrame*        fMoPdgFrame;
  TGNumberEntry*       fEntryPtId;
  TGNumberEntry*       fEntryMcId;
  TGNumberEntry*       fEntryMcPdg;
  TGNumberEntry*       fEntryMoId;
  TGNumberEntry*       fEntryMoPdg;
  TGTextButton*        fSelectButton;
  TGCheckButton*       fSideFVisibleBtn;
  TGCheckButton*       fSideBVisibleBtn;
  TGCheckButton*       fModulesVisibleBtn;
  TGCheckButton*       fSectorsVisibleBtn;
  TGCheckButton*       fPointsVisibleBtn;
  TGCheckButton*       fHitsVisibleBtn;
  TGCheckButton*       fPdgBtn[10];
  TGCheckButton*       fMoPdgBtn[10];

  
  CbmVisMuch*        fDisplay;
  TObjArray*         fSectors;

  Int_t fNstations;
  Int_t fStationNr;
  Int_t fLayerNr;
  Bool_t fShowSide[2];
  Bool_t fShowModules;
  Bool_t fShowSectors;
  Bool_t fShowPoints;
  Bool_t fShowHits;

  Int_t fNsectors;
  Double_t fRangeX;
  Double_t fRangeY;
  Int_t fECanvasW;
  Int_t fECanvasH;
  Double_t fScaleX;
  Double_t fScaleY;


  TGHorizontalFrame*   fScaleFrame;
  TGLabel*             fScaleLabel;
  TGComboBox*          fScaleBox;
};


#endif
