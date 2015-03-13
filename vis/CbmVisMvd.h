// -------------------------------------------------------------------------
// -----                       CbmVisMvd header file                   -----
// -----                  Created 01/04/10 by M. Ryzhinskiy            -----
// -------------------------------------------------------------------------

/** CbmVisMvd
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 01.04.2010
 *@version 1.0
 **
 **/


#ifndef CBMVISMVD_H
#define CBMVISMVD_H 1

#include "RQ_OBJECT.h"
#include "TGFrame.h"
#include "FairTask.h"
#include "TFile.h"

#include <map>

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
class CbmMvdGeoPar;

class CbmMvdStation;
class CbmVisMvdStationFrame;
class CbmVisPoint;
class CbmVisHit;

class CbmVisMvd : public FairTask{
 RQ_OBJECT("CbmVisMvd")
 ClassDef(CbmVisMvd,1);

 public:
  CbmVisMvd();
  virtual ~CbmVisMvd();
  virtual void SetParContainers();
  InitStatus Init();
  void ReadEvent(Int_t iEvent);
  void ReadNextEvent();
  void ReadPrevEvent();
  void CreateDisplay();
  TClonesArray*      GetMCTracks()   { return fMCTracks;    }
  TClonesArray*      GetPoints()     { return fPoints;      }
  TClonesArray*      GetDigis()      { return fDigis;       }
  TClonesArray*      GetHits()       { return fHits;   }
  TGStatusBar*   GetStatusBar()         {return fStatusBar;     }

  Int_t GetNStations() {return fStationMap.size();}
  std::map<Int_t, CbmMvdStation*> GetStations() { return fStationMap; }
  CbmMvdStation* GetStation(Int_t stationNr)    { return fStationMap[stationNr]; }

  void SetWindowName(TString name) {fDisplay->SetWindowName(name);}
  void CloseWindow();

  /**
   * Sets size of square pixels.
   * @param pixelSize Size of pixels [microns]
   */
  void SetPixelSize(Double_t pixelSize);
  Double_t GetPixelSize() {return fPixelSizeX; }


private:
  CbmMvdGeoPar*     fGeoPar;

  FairRootManager* fRootManager;
  TClonesArray*   fMCTracks;
  TClonesArray*   fPoints;
  TClonesArray*   fHits;
  TClonesArray*   fDigis;

  Int_t fEvent;
  Bool_t fSumEvents;

  // Widgets
  TGMainFrame*            fDisplay;
  TGVerticalFrame*        fMain;
  TGHorizontalFrame*      fButtonFrame;
  TGStatusBar*            fStatusBar;
  TGTextButton*           fNext;
  TGTextButton*           fPrev;
  TGTextButton*           fExit;
  CbmVisMvdStationFrame* fStationFrame;



  Double_t fPixelSizeX; // Pixel size in X [cm]
  Double_t fPixelSizeY; // Pixel size in Y [cm]

  /** Map from station number to MvdStation **/
   std::map<Int_t, CbmMvdStation*> fStationMap;
   Int_t GetMvdGeometry();
};


#endif
