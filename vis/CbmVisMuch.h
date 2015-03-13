// -------------------------------------------------------------------------
// -----                       CbmVisMuch header file                  -----
// -----                  Created 14/03/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

/** CbmVisMuch
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 26.03.07
 *@version 1.0
 **
 **/


#ifndef CBMVISMUCH_H
#define CBMVISMUCH_H 1

#include "RQ_OBJECT.h"
#include "TGFrame.h"
#include "FairTask.h"
#include "TFile.h"

class TClonesArray;
class TObjArray;
class TGMainFrame;
class TGFrame;
class TGStatusBar;
class TGTextButton;
class TGVerticalFrame;
class TGHorizontalFrame;

class FairRootManager;
class CbmMCTrack;
class CbmGeoMuchPar;

class CbmMuchStation;
class CbmMuchSector;
class CbmMuchCluster;
class CbmMuchGeoScheme;

class CbmVisMuchStationFrame;
class CbmVisPoint;
class CbmVisHit;

class CbmVisMuch : public FairTask{
 RQ_OBJECT("CbmVisMuch")
 ClassDef(CbmVisMuch,1);

 public:
  CbmVisMuch();
  virtual ~CbmVisMuch();
  virtual void SetParContainers();
  InitStatus Init();
  void ReadEvent(Int_t iEvent);
  void ReadNextEvent();
  void ReadPrevEvent();
  void CreateDisplay();
  TClonesArray*      GetMCTracks()   { return fMCTracks;    }
  TClonesArray*      GetPoints()     { return fPoints;      }
  TClonesArray*      GetDigis()      { return fDigis;       }
  TClonesArray*      GetPixelHits()  { return fPixelHits;   }
  TClonesArray*      GetStripHits()  { return fStripHits;   }
  TClonesArray*      GetVisClusters(){ return fVisClusters; }
  TClonesArray*      GetDigiMatches(){ return fDigiMatches; }

  TGStatusBar*   GetStatusBar()         {return fStatusBar;     }
  TObjArray*     GetOpenModules()       {return fOpenModules;   }
  TClonesArray*  GetVisPoints(Int_t st) {return fVisPoints[st]; }
  TClonesArray*  GetVisHits  (Int_t st) {return fVisHits[st];   }

  Int_t SetPointInfo (CbmVisPoint* point);
  Int_t SetHitInfo   (CbmVisHit*   hit  );
  Int_t GetMotherPdg (CbmMCTrack* track);

  Int_t GetNStations() {return fNstations;}

  void SetDigiFileName(Char_t* name) {
    fDigiFileName = name; 
    fDigiFile = new TFile(fDigiFileName,"R");
    if (!fDigiFile->IsOpen()) Fatal("Init","Geo file failed to open");
  }

  void SetWindowName(TString name) {fDisplay->SetWindowName(name);}
  void CloseWindow();

private:
  Char_t*            fDigiFileName;
  CbmMuchGeoScheme*  fGeoScheme;

  CbmGeoMuchPar*     fGeoPar;

  FairRootManager* fRootManager;
  TClonesArray*   fMCTracks;
  TClonesArray*   fPoints;
  TClonesArray*   fPixelHits;
  TClonesArray*   fStripHits;
  TClonesArray*   fDigis;
  TClonesArray*   fDigiMatches;
  TClonesArray*   fClusters;

  TClonesArray*   fVisHits[20];
  TClonesArray*   fVisPoints[20];

  TClonesArray*   fVisClusters;

  TObjArray* fOpenModules;

  TObjArray* fStations;

  Int_t fEvent;
  Int_t fNpoints;
  Int_t fNhits;
  Int_t fNstations;
  Bool_t fSumEvents;

  // Widgets
  TGMainFrame*            fDisplay;
  TGVerticalFrame*        fMain;
  TGHorizontalFrame*      fButtonFrame;
  TGStatusBar*            fStatusBar;
  TGTextButton*           fNext;
  TGTextButton*           fPrev;
  TGTextButton*           fExit;
  CbmVisMuchStationFrame* fStationFrame;

  TFile* fDigiFile;
};


#endif
