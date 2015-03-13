// -------------------------------------------------------------------------
// -----                CbmVisMvdStationFrame header file             -----
// -----                Created 01/04/10 by M. Ryzhinskiy             -----
// -------------------------------------------------------------------------

/** CbmVisMvdStationFrame
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 01.04.2010
 *@version 1.0
 **
 **/
#ifndef CBMVISMVDSTATIONFRAME_H
#define CBMVISMVDSTATIONFRAME_H 1


#include "TGFrame.h"
#include "RQ_OBJECT.h"
#include "TGComboBox.h"



class TRootEmbeddedCanvas;
class TObjArray;
class TCanvas;


class TGCanvas;
class TGHorizontalFrame;
class TGNumberEntry;
class TGGroupFrame;
class TGTextButton;
class TGCheckButton;
class TGLabel;

class CbmVisMvd;

#include <map>

using std::map;

//class CbmMuchGeoScheme;

class CbmVisMvdStationFrame: public TGHorizontalFrame{
 RQ_OBJECT("CbmVisMvdStationFrame")
 ClassDef(CbmVisMvdStationFrame,1);

 public:
  CbmVisMvdStationFrame(const TGWindow *p, CbmVisMvd* task);
  ~CbmVisMvdStationFrame();
  void DrawStation(Int_t stationNr=1);
  void ScaleStation(Int_t scale);
  void UpdateStation(Int_t stationNr=1);
  void CloseWindow();
  void HandleEmbeddedCanvas(Int_t event, Int_t px, Int_t py,
                                        TObject *sel);

  void ReadPoints();
  void ReadHits();

  Int_t GetStationNr() {return fStationCombo->GetSelected();}

private:
  bool ShowTrack(Int_t pdg);

private:
  //CbmMuchGeoScheme*    fGeoScheme;
  TGVerticalFrame*     fMain;
  TGComboBox*          fStationCombo;
  TGComboBox*          fLayerCombo;
  TGCanvas*            fGCanvas;
  TRootEmbeddedCanvas* fECanvas;
  TCanvas*             fCanvas;

  
  CbmVisMvd*        fDisplay;

  Double_t fRangeX;
  Double_t fRangeY;
  Int_t fECanvasW;
  Int_t fECanvasH;
  Double_t fScaleX;
  Double_t fScaleY;


  TGHorizontalFrame*   fScaleFrame;
  TGLabel*             fScaleLabel;
  TGComboBox*          fScaleBox;


  Bool_t InitPixels();
  void DrawHits(Int_t stationNr);
  void DrawPoints(Int_t stationNr);
  void DrawDigis(Int_t stationNr);
  map<Int_t, TObjArray* > fPixels; // Graphic pixel objects
  map<Int_t, TObjArray*>  fPoints; // Graphic MC-point objects
  map<Int_t, TObjArray*>  fHits;   // Graphic hit objects


};


#endif
