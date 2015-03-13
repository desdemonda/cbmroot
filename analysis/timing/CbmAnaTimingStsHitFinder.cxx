/** CbmAnaTimingStsHitFinder.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-07-07
 **/

#include "CbmAnaTimingStsHitFinder.h"
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

using std::vector;
using std::multimap;

// -----   Default constructor   -------------------------------------------
CbmAnaTimingStsHitFinder::CbmAnaTimingStsHitFinder():
    FairTask("Task"),fEvent(0),fHistoName("histo.root")
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmAnaTimingStsHitFinder::CbmAnaTimingStsHitFinder(const char* name, TString histoName):
  FairTask(name),fEvent(0),fHistoName(histoName)
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmAnaTimingStsHitFinder::~CbmAnaTimingStsHitFinder(){}
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void CbmAnaTimingStsHitFinder::SetParContainers(){
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
InitStatus CbmAnaTimingStsHitFinder::Init(){
  FairRootManager* fManager = FairRootManager::Instance();
  fDigis       = (TClonesArray*) fManager->GetObject("StsDigi");
  fDigiMatches = (TClonesArray*) fManager->GetObject("StsDigiMatch");
  fHits        = (TClonesArray*) fManager->GetObject("StsHit");
  fMcEpoch     = (CbmMCEpoch*)   fManager->GetObject("MCEpoch.");
  fHitTimeAll       = new TH1D("fHitTimeAll",";time [ns]; Entries",600,0,600);
  fHitTimeCorrected = new TH1D("fHitTimeCorrected",";time [ns]; Entries",600,0,600);
  fHitTimeAll->SetLineColor(kBlue);
  fHitTimeCorrected->SetLineColor(kBlue);
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingStsHitFinder::Exec(Option_t* opt){
  Int_t nHits = fHits->GetEntriesFast();
  printf("nHits=%i\n",nHits);
  for (Int_t i=0;i<nHits;i++){
    CbmStsHit* hit = (CbmStsHit*) fHits->At(i);
    Double_t t = hit->GetTimeStamp();
    TVector3 r;
    hit->Position(r);
    fHitTimeAll->Fill(t);
    fHitTimeCorrected->Fill(t-r.Mag()/29.97);
  }
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaTimingStsHitFinder::Finish(){
  TCanvas* c1 = new TCanvas("c1","c1",1000,800);
  c1->Divide(1,2);
  c1->cd(1);
  fHitTimeAll->Draw();
  c1->cd(2);
  fHitTimeCorrected->Draw();
  
  TFile* f = new TFile(fHistoName.Data(),"RECREATE");
  fHitTimeAll->Write();
  fHitTimeCorrected->Write();
  f->Close();
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaTimingStsHitFinder);
