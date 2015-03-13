/** CbmAnaTimingStsTracking.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-07-11
 **/

#include "CbmAnaTimingStsTracking.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairMCPoint.h"
#include "CbmMCEpoch.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiMatch.h"
#include "TClonesArray.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "CbmStsHit.h"
#include "TVector3.h"
#include <map>
#include <vector>
#include "CbmStsTrack.h"

using std::vector;
using std::multimap;

// -----   Default constructor   -------------------------------------------
CbmAnaTimingStsTracking::CbmAnaTimingStsTracking():
    FairTask("Task"),fEvent(0),fHistoName("histo.root")
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmAnaTimingStsTracking::CbmAnaTimingStsTracking(const char* name, TString histoName):
  FairTask(name),fEvent(0),fHistoName(histoName)
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmAnaTimingStsTracking::~CbmAnaTimingStsTracking(){}
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void CbmAnaTimingStsTracking::SetParContainers(){
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmGeoMuchPar");
  rtdb->getContainer("CbmFieldPar");
}
// -------------------------------------------------------------------------


// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmAnaTimingStsTracking::Init(){
  FairRootManager* fManager = FairRootManager::Instance();
  fDigis       = (TClonesArray*) fManager->GetObject("StsDigi");
  fDigiMatches = (TClonesArray*) fManager->GetObject("StsDigiMatch");
  fHits        = (TClonesArray*) fManager->GetObject("StsHit");
  fMcEpoch     = (CbmMCEpoch*)   fManager->GetObject("MCEpoch.");
  fStsTracks   = (TClonesArray*) fManager->GetObject("StsTrack");
  fTrackTime        = new TH1D("fTrackTime","; time [ns]; Entries",1000,0,1000);
  fTrackTimeFromHit = new TH1D("fTrackTimeFromHit","; time [ns]; Entries",1000,0,1000);
  fTrackTimeFromLastHit = new TH1D("fTrackTimeFromLastHit","; time [ns]; Entries",1000,0,1000);
  fTrackTime->SetLineColor(kBlue);
  fTrackTimeFromHit->SetLineColor(kBlue);
  fTrackTimeFromLastHit->SetLineColor(kBlue);
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingStsTracking::Exec(Option_t* opt){
  Int_t nTracks = fStsTracks->GetEntriesFast();
  printf("nTracks=%i\n",nTracks);
  for (Int_t i=0;i<nTracks;i++){
    CbmStsTrack* track = (CbmStsTrack*) fStsTracks->At(i);
    Double_t t = track->GetTime();
    fTrackTime->Fill(t);
    Int_t hitId = track->GetStsHitIndex(0);
    CbmStsHit* hit = (CbmStsHit*) fHits->At(hitId);
    fTrackTimeFromHit->Fill(hit->GetTimeStamp());
    hitId = track->GetStsHitIndex(track->GetNStsHits()-1);
    hit = (CbmStsHit*) fHits->At(hitId);
    fTrackTimeFromLastHit->Fill(hit->GetTimeStamp());
  }
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaTimingStsTracking::Finish(){
  TCanvas* c1 = new TCanvas("c1","c1",1200,1000);
  c1->Divide(1,3);
  c1->cd(1);
  fTrackTime->Draw();
  c1->cd(2);
  fTrackTimeFromHit->Draw();
  c1->cd(3);
  fTrackTimeFromLastHit->Draw();

  TFile* f = new TFile(fHistoName.Data(),"RECREATE");
  fTrackTime->Write();
  fTrackTimeFromHit->Write();
  fTrackTimeFromLastHit->Write();
  f->Close();
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaTimingStsTracking);
