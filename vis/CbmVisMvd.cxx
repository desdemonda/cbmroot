// -------------------------------------------------------------------------
// -----                       CbmVisMvd source file                   -----
// -----                  Created 01/04/10 by M. Ryzhinskiy            -----
// -------------------------------------------------------------------------


#include "CbmVisMvd.h"

#include "TApplication.h"
#include "TClonesArray.h"
#include "TGClient.h"
#include "TGButton.h"
#include "TGFrame.h"
#include "TGStatusBar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmMvdGeoPar.h"
#include "FairGeoNode.h"
#include "CbmMCTrack.h"

#include "CbmVisMvdStationFrame.h"
#include "CbmVisMuchModuleFrame.h"
#include "CbmVisPoint.h"
#include "CbmVisHit.h"
#include "CbmVisPixelHit.h"
#include "CbmVisStripHit.h"

#include "TColor.h"
#include "TFile.h"
#include "TGeoManager.h"
#include "TGeoTube.h"
#include "CbmMvdStation.h"


// -----   Default constructor   ------------------------------------------
CbmVisMvd::CbmVisMvd():FairTask("Task",0){

  fEvent  = 0;
  fMCTracks    = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fHits   = NULL;
  fSumEvents   = kFALSE;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmVisMvd::~CbmVisMvd() {
}
// -------------------------------------------------------------------------


// -----   Private method SetParContainers   -------------------------------
void CbmVisMvd::SetParContainers() {
  FairRunAna*    ana  = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  fGeoPar  = (CbmMvdGeoPar*)  (rtdb->getContainer("CbmMvdGeoPar"));
}
// -------------------------------------------------------------------------

Int_t CbmVisMvd::GetMvdGeometry() {

  printf("-I- %s : Reading MVD geometry...\n", GetName());
  Int_t iStation =  1;
  Int_t volId    = -1;
  fStationMap.clear();

  do {

    // Volume name according to convention
    TString volName  = Form("mvdstation%02i", iStation);
    volId = gGeoManager->GetUID(volName);
    if ( volId > -1 ) {

      // Get shape parameters
      TGeoVolume* volume = gGeoManager->GetVolume(volName.Data());
      TGeoTube* tube = (TGeoTube*) volume->GetShape();
      Double_t rmin = tube->GetRmin();
      Double_t rmax = tube->GetRmax();
      Double_t d    = 2. * tube->GetDz();

      // Full path to node
      TString nodeName = "/cave_1/pipevac1_0/" + volName + "_0";

      // Get z position of node
      Bool_t nodeFound = gGeoManager->cd(nodeName.Data());
      if ( ! nodeFound ) {
        printf("-E- %s::GetMvdGeometry: Node %s not found in geometry!",GetName(), nodeName.Data());
        Fatal("GetMvdGeometry", "Node not found");
      }
      Double_t local[3] = {0., 0., 0.};  // Local centre of volume
      Double_t global[3];                // Global centre of volume
      gGeoManager->LocalToMaster(local, global);
      Double_t z = global[2];


      // Check for already existing station with the same ID
      // (Just in case, one never knows...)
      if ( fStationMap.find(iStation) != fStationMap.end() ) {
        printf("-E- %s::GetMvdGeometry: Volume ID %i already in map!", GetName(), iStation);
        Fatal("GetMvdGeometry", "Double station number in TGeoManager!");
      }

      // Create new CbmMvdStation and add it to the map
      fStationMap[iStation] = new CbmMvdStation(volName.Data(), iStation, volId,  z, d, rmin, rmax);
      fStationMap[iStation]->Print();

      iStation++;

    }     // Volume found

  } while ( volId > -1 );


  return iStation - 1;
}

// -------------------------------------------------------------------------
InitStatus CbmVisMvd::Init() {
  fRootManager = FairRootManager::Instance();
  fMCTracks    = (TClonesArray*) fRootManager->GetObject("MCTrack");
  fPoints      = (TClonesArray*) fRootManager->GetObject("MvdPoint");
  fHits        = (TClonesArray*) fRootManager->GetObject("MvdHit");
  fDigis       = (TClonesArray*) fRootManager->GetObject("MvdDigi");

  // Read MVD station from the geometry
  GetMvdGeometry();
  CreateDisplay();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

void CbmVisMvd::CloseWindow(){
    gApplication->Terminate(0);
}


// -------------------------------------------------------------------------
void CbmVisMvd::CreateDisplay(){
  fDisplay = new TGMainFrame(gClient->GetRoot(),800,800,kVerticalFrame);
  fDisplay->SetWindowName("Mvd Event Display");
  fDisplay->Connect("CloseWindow()","TApplication",gApplication,"Terminate()");

  fMain = new TGVerticalFrame(fDisplay);

  Int_t parts[] = {20,65,15};
  fStatusBar = new TGStatusBar(fDisplay,50,10,kHorizontalFrame);
  fStatusBar->SetParts(parts,3);

  fStationFrame = new CbmVisMvdStationFrame(fMain,this);

  //Button Frame
  fButtonFrame = new TGHorizontalFrame(fMain,500,100);

  fPrev = new TGTextButton(fButtonFrame,"&Previous");
  fNext = new TGTextButton(fButtonFrame,"&Next");
  fExit = new TGTextButton(fButtonFrame,"&Exit","gApplication->Terminate(0)");

  fNext->Connect("Clicked()","CbmVisMvd",this,"ReadNextEvent()");
  fPrev->Connect("Clicked()","CbmVisMvd",this,"ReadPrevEvent()");

  TGLayoutHints* lhButton = new TGLayoutHints(kLHintsCenterX,5,5,2,2);
  fButtonFrame->AddFrame(fPrev,lhButton);
  fButtonFrame->AddFrame(fNext,lhButton);
  fButtonFrame->AddFrame(fExit,lhButton);

  fMain->AddFrame(fStationFrame, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,2,2,2,2));
  fMain->AddFrame(fButtonFrame , new TGLayoutHints(kLHintsBottom|kLHintsCenterX,2,2,2,2));

  fDisplay->AddFrame(fMain     ,new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX|kLHintsExpandY,2,2,2,2));
  fDisplay->AddFrame(fStatusBar,new TGLayoutHints(kLHintsBottom|kLHintsLeft |kLHintsExpandX,0,0,2,0));


  fDisplay->MapSubwindows();
  fDisplay->Resize(fDisplay->GetDefaultSize());
  fDisplay->MapWindow();
  ReadEvent(fEvent);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMvd::ReadEvent(Int_t event){
  int verbose = 0;
  fEvent=event;
  printf("Event:%i\n",fEvent);
  fRootManager->ReadEvent(fEvent);

  // Update current station view
  fStationFrame->ReadPoints();
  fStationFrame->ReadHits();
  fStationFrame->UpdateStation(fStationFrame->GetStationNr());
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMvd::ReadNextEvent(){
  fEvent++;
  ReadEvent(fEvent);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmVisMvd::ReadPrevEvent(){
  fEvent--;
  ReadEvent(fEvent);
}
// -------------------------------------------------------------------------



void CbmVisMvd::SetPixelSize(Double_t pixelSize){
  fPixelSizeX = pixelSize/10000;
  fPixelSizeY = pixelSize/10000;
}

ClassImp(CbmVisMvd)
