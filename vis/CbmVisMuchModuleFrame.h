// -------------------------------------------------------------------------
// -----                 CbmVisMuchModuleFrame header file             -----
// -----                  Created 14/03/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

/** CbmVisMuchModuleFrame
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 14.03.07
 *@version 1.0
 **
 **/


#ifndef CBMVISMUCHMODULEFRAME_H
#define CBMVISMUCHMODULEFRAME_H 1

#include "RQ_OBJECT.h"
#include "TObject.h"
#include "TGFrame.h"
#include "CbmVisMuch.h"
#include "TClonesArray.h"
#include "TPolyLine.h"

class TGWindow;
class TGMainFrame;
class TGCompositeFrame;
class TGHSlider;
class TGStatusBar;
class TCanvas;
class TGCanvas;
class TRootEmbeddedCanvas;
class TGComboBox;
class CbmMuchModuleGem;
class TGTextButton;
class TGHorizontalFrame;

class CbmVisMuchModuleFrame : public TObject{
RQ_OBJECT("CbmVisMuchModuleFrame")
ClassDef(CbmVisMuchModuleFrame,1)
public:
   CbmVisMuchModuleFrame(CbmMuchModuleGem* module, CbmVisMuch* display);
   virtual ~CbmVisMuchModuleFrame();
   void CloseWindow();
   void HandleEmbeddedCanvas(Int_t event, Int_t x, Int_t y, TObject *sel);
   void DrawPoints();
   void DrawHits();
   void DoScale(Int_t scale);
   void UpdateModule();
   void PrintCanvas();

   Int_t GetStationNr() {return fStationNr;}
   Int_t GetModuleNr()  {return fModuleNr;}
   TCanvas* GetCanvas() {return fCanvas;}

   //protected:
   TGMainFrame         *fMain;
   TGComboBox          *fScale;
   TGCompositeFrame    *fFrame;
   TGStatusBar         *fStatusBar;
   TGCanvas            *fGCanvas;
   TRootEmbeddedCanvas *fECanvas;
   TCanvas             *fCanvas;
   TGComboBox          *fSectorCombo;
   TGTextButton        *fPrintButton;
   TGHorizontalFrame   *fButtonFrame;

   CbmMuchModuleGem    *fModule;

   CbmVisMuch          *fDisplay;

   Int_t fNPoints;
   Int_t fNHits;
   Int_t fStationNr;
   Int_t fModuleNr;
   TPolyLine* fPline;
   Int_t fPlineSize;
   Int_t fScaleValue;
};

#endif
