/** CbmAnaTimingStsDigitizer.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-07-07
 **/

#include "CbmAnaTimingStsDigitizer.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairMCPoint.h"
#include "CbmMCEpoch.h"
#include "TClonesArray.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiMatch.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"


// -----   Default constructor   -------------------------------------------
CbmAnaTimingStsDigitizer::CbmAnaTimingStsDigitizer():
  fEvent(0),fHistoName("histo.root"),
  fDigiScheme(new CbmStsDigiScheme()),
  fStationId(1),fSectorId(62),fSideId(0),fChannelId(0),fEpoch(1)
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmAnaTimingStsDigitizer::CbmAnaTimingStsDigitizer(const char* name, TString histoName):
  FairTask(name),fEvent(0),fHistoName(histoName),
  fDigiScheme(new CbmStsDigiScheme()),
  fStationId(1),fSectorId(62),fSideId(0),fChannelId(0),fEpoch(1)
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmAnaTimingStsDigitizer::~CbmAnaTimingStsDigitizer(){}
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void CbmAnaTimingStsDigitizer::SetParContainers(){
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmGeoMuchPar");
  rtdb->getContainer("CbmFieldPar");
  // Get STS geometry parameter container
  fGeoPar = (CbmGeoStsPar*) rtdb->getContainer("CbmGeoStsPar");

  // Get STS digitisation parameter container
  fDigiPar = (CbmStsDigiPar*) rtdb->getContainer("CbmStsDigiPar");
}
// -------------------------------------------------------------------------


// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmAnaTimingStsDigitizer::Init(){
  FairRootManager* fManager = FairRootManager::Instance();
  // Build digitisation scheme
  Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
  if ( ! success ) return kERROR;
  
  fStsDigis       = (TClonesArray*) fManager->GetObject("StsDigi");
  fStsDigiMatches = (TClonesArray*) fManager->GetObject("StsDigiMatch");
  if (fEpoch)
    fMcEpoch         = (CbmMCEpoch*)   fManager->GetObject("MCEpoch.");
  else { 
    fStsPoints   = (TClonesArray*) fManager->GetObject("StsPoint");
    fMcTracks    = (TClonesArray*) fManager->GetObject("MCTrack");
  }
  fPointsTimeAll    = new TH1D("fPointsTimeAll","; time [ns]; Entries",600,0,600);
  fPointsTimeSector = new TH1D("fPointsTimeSector","; time [ns]; Entries",600,0,600);
  fDigiTimeAll      = new TH1D("fDigiTimeAll","; time [ns]; Entries",600,0,600);
  fDigiTimeSector   = new TH1D("fDigiTimeSector","; time [ns]; Entries",600,0,600);
  fPointsTimeAll->SetLineColor(kBlue);
  fPointsTimeSector->SetLineColor(kBlue);
  fDigiTimeAll->SetLineColor(kBlue);
  fDigiTimeSector->SetLineColor(kBlue);
  
  fDigiVsSector = new TH1D("fDigiVsSector","DigiVsSector",100,0,100);
  fDigiVsSector->SetLineColor(kBlue);
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingStsDigitizer::Exec(Option_t* opt){
  if (fEpoch) fStsPoints = fMcEpoch->GetPoints(kSTS);
  
  Int_t nStsPoints = fStsPoints ? fStsPoints->GetEntriesFast() : 0;
  Int_t nStsDigis  = fStsDigis  ? fStsDigis->GetEntriesFast() : 0;
  if (fVerbose>-1) printf(" Event: %4i",fEvent++);
  if (fVerbose>0) printf(" nMuchPoints: %4i",nStsPoints);
  if (fVerbose>0) printf(" nMuchDigis: %4i",nStsDigis);
  if (fVerbose>-1) printf("\n");
   
  for (Int_t i=0;i<nStsPoints;i++){
    FairMCPoint* point = (FairMCPoint*) fStsPoints->At(i);
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t t = point->GetTime();
    if (fVerbose>2) printf("  Sts point: %5i time=%8.1f",i,t);
    if (fVerbose>2) printf(" Detector Id: %8i",point->GetDetectorID());
    if (fVerbose>2) printf("\n");
    fPointsTimeAll->Fill(t);
    if (point->GetDetectorID() != 121) continue;
    fPointsTimeSector->Fill(t);
    
    if (fEpoch) continue;
    if (t<2) continue;
    CbmMCTrack* track = (CbmMCTrack*) fMcTracks->At(point->GetTrackID());
    printf("iTrack=%4i t=%4.0f pdg=%i p=%f\n",point->GetTrackID(),t,track->GetPdgCode(),track->GetP());
  }
  
  for (Int_t i=0;i<nStsDigis;i++) {
    CbmStsDigi* digi = (CbmStsDigi*) fStsDigis->At(i);
    CbmStsDigiMatch* digiMatch = (CbmStsDigiMatch*) fStsDigiMatches->At(i);
    Int_t station = digi->GetStationNr();
    Int_t sector  = digi->GetSectorNr();
    Int_t side    = digi->GetSide();
    Int_t channel = digi->GetChannelNr();
    Double_t t    = digi->GetTimeStamp();
    Int_t detId   = digi->GetDetectorId();
    
    if (fVerbose>2) printf("  Digi: %5i time=%8i detId=%8i",i,t,detId);
    if (fVerbose>2) printf(" station=%i", station);
    if (fVerbose>2) printf(" sector=%i", sector);
    if (fVerbose>2) printf(" side=%i", side);
    if (fVerbose>2) printf("\n");
    fDigiTimeAll->Fill(t);
    if (station==fStationId && side==fSideId) fDigiVsSector->Fill(sector);
    FairMCPoint* point= (FairMCPoint*) fStsPoints->At(digiMatch->GetRefIndex(0));
    if (point->GetDetectorID()==121) fDigiTimeSector->Fill(t);
  }
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaTimingStsDigitizer::Finish(){
  TCanvas* c1 = new TCanvas("c1","Points",1000,800);
  c1->Divide(1,2);
  c1->cd(1);
//  fDigiVsSector->Draw();
  fPointsTimeAll->Draw();
  c1->cd(2);
  fPointsTimeSector->Draw();

  
  TCanvas* c2 = new TCanvas("c2","Digis",1000,800);
  c2->Divide(1,2);
  c2->cd(1);
  fDigiTimeAll->Draw();
  c2->cd(2);
  fDigiTimeSector->Draw();

  TFile* f = new TFile(fHistoName.Data(),"RECREATE");
  fPointsTimeAll->Write();
  fPointsTimeSector->Write();
  fDigiTimeAll->Write();
  fDigiVsSector->Write();
  fDigiTimeSector->Write();
  f->Close();
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaTimingStsDigitizer);
