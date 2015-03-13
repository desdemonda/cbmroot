/** CbmAnaMuch.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2009-09-08
 **/
#define PBINNING 50,0.,20.
#define PTBINNING 50,0.,5.
#define YBINNING 50,0.,4.
#define JPSIMBINNING 50,2.,4.

#include "CbmAnaMuch.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmMuchTrack.h"
#include "CbmTrackMatch.h"
#include "CbmMuchGeoScheme.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchStation.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TF1.h"
#include "TObjArray.h"
#include <vector>

// -----   Default constructor   -------------------------------------------
CbmAnaMuch::CbmAnaMuch()
  : FairTask("AnaMuch"),
    fDigiFileName(""),
    fGeoScheme(CbmMuchGeoScheme::Instance()),
    fEvent(0),
    fMCTracks(NULL),
    fStsTracks(NULL),
    fStsPoints(NULL),
    fMuchPoints(NULL),
    fMuchHits(NULL),
    fMuchTracks(NULL),
    fMuchTrackMatches(NULL),
    fStsTrackMatches(NULL),
    mapAccSts(),
    mapAccMuch(),
    mapRecSts(),
    mapRecMuch(),
    hAllP(NULL),
    hAccStsP(NULL),
    hAccMuchP(NULL),
    hRecStsP(NULL),
    hRecMuchP(NULL),
    hMuPlusYPt(NULL),
    hMuMinusYPt(NULL),
    hJpsiM(NULL),
    hJpsiYPt(NULL),
    fStsPointsAccQuota(4),
    fStsTrueHitQuota(0.7),
    fMuchPointsAccQuota(13),
    fMuchTrueHitQuota(0.7),
    fHistoFileName("histo.root")   
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmAnaMuch::CbmAnaMuch(const char* name, TString digiFileName, TString histoFileName)
  : FairTask(name),
    fDigiFileName(""),
    fGeoScheme(CbmMuchGeoScheme::Instance()),
    fEvent(0),
    fMCTracks(NULL),
    fStsTracks(NULL),
    fStsPoints(NULL),
    fMuchPoints(NULL),
    fMuchHits(NULL),
    fMuchTracks(NULL),
    fMuchTrackMatches(NULL),
    fStsTrackMatches(NULL),
    mapAccSts(),
    mapAccMuch(),
    mapRecSts(),
    mapRecMuch(),
    hAllP(NULL),
    hAccStsP(NULL),
    hAccMuchP(NULL),
    hRecStsP(NULL),
    hRecMuchP(NULL),
    hMuPlusYPt(NULL),
    hMuMinusYPt(NULL),
    hJpsiM(NULL),
    hJpsiYPt(NULL),
    fStsPointsAccQuota(4),
    fStsTrueHitQuota(0.7),
    fMuchPointsAccQuota(13),
    fMuchTrueHitQuota(0.7),
    fHistoFileName("histo.root")   
{
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmAnaMuch::~CbmAnaMuch(){
}
// -------------------------------------------------------------------------


// -----  SetParContainers -------------------------------------------------
void CbmAnaMuch::SetParContainers(){
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
InitStatus CbmAnaMuch::Init(){
  TFile* f = new TFile(fDigiFileName,"R");
  if (!f->IsOpen()) Fatal("Init","Digi file failed to open");
  TObjArray* stations = (TObjArray*) f->Get("stations");
  if (!stations) Fatal("Init","No array of stations");
  // Init geo scheme
  fGeoScheme->Init(stations);
  
  // Get and check FairRootManager
  FairRootManager* fManager = FairRootManager::Instance();
  fMCTracks         = (TClonesArray*) fManager->GetObject("MCTrack");
  fStsPoints        = (TClonesArray*) fManager->GetObject("StsPoint");
  fMuchPoints       = (TClonesArray*) fManager->GetObject("MuchPoint");
  fMuchHits         = (TClonesArray*) fManager->GetObject("MuchPixelHit");
  fStsTracks        = (TClonesArray*) fManager->GetObject("StsTrack");
  fMuchTracks       = (TClonesArray*) fManager->GetObject("MuchTrack");
  fMuchTrackMatches = (TClonesArray*) fManager->GetObject("MuchTrackMatch");
  fStsTrackMatches  = (TClonesArray*) fManager->GetObject("StsTrackMatch");
  fEvent=0;
//  printf(" %i",fStsPoints);
//  printf(" %i",fMuchPoints);
  printf(" %p",fStsTracks);
  printf(" %p",fMuchHits);
  printf(" %p",fMuchTracks);
  printf(" %p",fMuchTrackMatches);
  printf("\n");

  hAllP     = new TH1D("hAllP"    ,"All muons distribution vs P;p [GeV/c]; Entries"    ,PBINNING);
  hAccStsP  = new TH1D("hAccStsP" ,"STS accepted vs P;p [GeV/c]; Entries" ,PBINNING);
  hAccMuchP = new TH1D("hAccMuchP","STS+MUCH accepted P;p [GeV/c]; Entries",PBINNING);
  hRecStsP  = new TH1D("hRecStsP" ,"STS reconstructed vs P;p [GeV/c]; Entries" ,PBINNING);
  hRecMuchP = new TH1D("hRecMuchP","STS+MUCH reconstructed vs P;p [GeV/c]; Entries",PBINNING);
  
  hMuPlusYPt  = new TH2D("hMuPlusYPt" ,"y-pt distribution for signal muons; y; p_{T} [GeV/c]",YBINNING,PTBINNING);
  hMuMinusYPt = new TH2D("hMuMinusYPt","y-pt distribution for signal muons; y; p_{T} [GeV/c]",YBINNING,PTBINNING);
  
  hJpsiYPt = new TH2D("hJpsiYPt","y-pt distribution for J/#Psi; y; p_{T} [GeV/c]",YBINNING,PTBINNING);
  hJpsiM   = new TH1D("hJpsiM","Dimuon invariant mass;M [GeV/c];Entries",JPSIMBINNING);
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaMuch::Exec(Option_t* opt){
  Int_t nMCTracks   = fMCTracks->GetEntriesFast();
  Int_t nStsTracks  = fStsTracks->GetEntriesFast();
  Int_t nMuchTracks = fMuchTracks->GetEntriesFast();
  Int_t nStsPoints  = fStsPoints->GetEntriesFast();
  Int_t nMuchPoints = fMuchPoints->GetEntriesFast();
  Int_t nMuchHits   = fMuchHits->GetEntriesFast();
  if (fVerbose>0) printf(" Event: %4i",fEvent++);
  if (fVerbose>0) printf(" MCtracks: %4i",nMCTracks);
  if (fVerbose>0) printf(" StsTracks: %4i",nStsTracks);
  if (fVerbose>0) printf(" MuchTracks: %4i",nMuchTracks);
  if (fVerbose>0) printf(" MCtracks: %4i",nMCTracks);
  if (fVerbose>0) printf(" StsPoints: %4i",nStsPoints);
  if (fVerbose>0) printf(" MuchPoints: %4i",nMuchPoints);
  if (fVerbose>0) printf(" MuchHits: %4i",nMuchHits);
  if (fVerbose>1) printf("\n");

  mapAccSts.clear();
  mapAccMuch.clear();
  mapRecSts.clear();
  mapRecMuch.clear();
  
  
  for (Int_t iTrack=0;iTrack<nMCTracks;iTrack++){
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iTrack);
    Int_t nAccStsPoints = mcTrack->GetNPoints(kSTS);
    Int_t nAccMuchPoints = mcTrack->GetNPoints(kMUCH);
    if (fVerbose>1) printf(" StsPoints: %4i",nAccStsPoints);
    if (fVerbose>1) printf(" MuchPoints: %4i",nAccMuchPoints);
    if (fVerbose>1) printf("\n");
    mapAccSts[iTrack]=nAccStsPoints;
    mapAccMuch[iTrack]=nAccMuchPoints;
    mapRecSts[iTrack]=-1;
    mapRecMuch[iTrack]=-1;
  }
  
  for (Int_t iTrack=0;iTrack<nStsTracks;iTrack++){
    CbmStsTrack*   stsTrack      = (CbmStsTrack*) fStsTracks->At(iTrack);
    CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*) fStsTrackMatches->At(iTrack);
//    Int_t mcTrackID = stsTrackMatch->GetMCTrackId();
//    Int_t trueHits  = stsTrackMatch->GetNofTrueHits();
//    Int_t wrongHits = stsTrackMatch->GetNofWrongHits();
//    Int_t fakeHits  = stsTrackMatch->GetNofFakeHits();
//    Double_t totalHits = trueHits + wrongHits + fakeHits;
//    Double_t ratio = trueHits/totalHits;
    Int_t trackId = GetTrackId(stsTrackMatch,fStsTrueHitQuota);
//    if (fVerbose>2) printf(" id=%4i",mcTrackID);
//    if (fVerbose>2) printf(" (true,wrong,fake)=(%2i,%2i,%2i)",trueHits,wrongHits,fakeHits);
//    if (fVerbose>2) printf(" ratio=%5.3f",ratio);
//    if (fVerbose>2) printf(" trackId=%i",trackId);
//    if (fVerbose>2) printf("\n");
    mapRecSts[trackId]=iTrack;
  }

  for (Int_t iTrack=0;iTrack<nMuchTracks;iTrack++){
    CbmMuchTrack*  muchTrack      = (CbmMuchTrack*)  fMuchTracks->At(iTrack);
    if (!IsReconstructed(muchTrack)) continue;
    CbmTrackMatch* muchTrackMatch = (CbmTrackMatch*) fMuchTrackMatches->At(iTrack);
    Int_t trackId = GetTrackId(muchTrackMatch,fMuchTrueHitQuota);
    mapRecMuch[trackId]=iTrack;
  }
  
  Int_t nAllMuons = 0;
  Int_t nAccStsMuons = 0;
  Int_t nAccMuchMuons = 0;
  Int_t nRecStsMuons = 0;
  Int_t nRecMuchMuons = 0;
  
  TLorentzVector momMuPlus;
  TLorentzVector momMuMinus;
  for (Int_t iTrack=0;iTrack<nMCTracks;iTrack++){
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iTrack);
    Int_t pdgCode = mcTrack->GetPdgCode();
    if (fVerbose>1) printf(" %5i",pdgCode);
    if (fVerbose>1) printf("\n");

    if (TMath::Abs(pdgCode)!=13) continue;

    Double_t p  = mcTrack->GetP();
    Double_t pt = mcTrack->GetPt();
    Double_t y  = mcTrack->GetRapidity();

    if (pdgCode== 13) { 
      mcTrack->Get4Momentum(momMuPlus);
      hMuPlusYPt->Fill(y,pt);
    }
    if (pdgCode==-13) {
      mcTrack->Get4Momentum(momMuMinus);
      hMuMinusYPt->Fill(y,pt);
    }
    
    nAllMuons++;
    hAllP->Fill(p);
    if (mapAccSts[iTrack]<fStsPointsAccQuota) continue; 
    hAccStsP->Fill(p);
    nAccStsMuons++;
    if (mapAccMuch[iTrack]<fMuchPointsAccQuota) continue; 
    hAccMuchP->Fill(p);
    nAccMuchMuons++;
    if (mapRecSts[iTrack]<0) continue;
    hRecStsP->Fill(p);
    nRecStsMuons++;
    if (mapRecMuch[iTrack]<0) continue; 
    hRecMuchP->Fill(p);
    nRecMuchMuons++;
  }
  
  TLorentzVector momJpsi = momMuPlus + momMuMinus;
  Double_t yJpsi   = momJpsi.Rapidity();
  Double_t ptJpsi  = momJpsi.Pt();
  hJpsiYPt->Fill(yJpsi,ptJpsi);
  
  if (0) {
    Int_t nReconstructedMuons=0;
    if (fVerbose>2) printf("Track matches: \n");
    for (Int_t iTrack=0;iTrack<nMuchTracks;iTrack++){
      CbmMuchTrack*  muchTrack      = (CbmMuchTrack*) fMuchTracks->At(iTrack);
      CbmTrackMatch* muchTrackMatch = (CbmTrackMatch*) fMuchTrackMatches->At(iTrack);
      Int_t mcTrackID = muchTrackMatch->GetMCTrackId();
      Int_t trueHits  = muchTrackMatch->GetNofTrueHits();
      Int_t wrongHits = muchTrackMatch->GetNofWrongHits();
      Int_t fakeHits  = muchTrackMatch->GetNofFakeHits();
      Double_t totalHits = trueHits + wrongHits + fakeHits;
      if(totalHits<13) continue;
      Double_t ratio = trueHits/totalHits;
      if (fVerbose>2) printf(" %4i",mcTrackID);
      if (fVerbose>2) printf(" %4i",trueHits);
      if (fVerbose>2) printf(" %4i",wrongHits);
      if (fVerbose>2) printf(" %4i",fakeHits);
      if (fVerbose>2) printf(" %5.3f",ratio);
      if (fVerbose>2) printf("\n");
      nReconstructedMuons++;
    }
    if (fVerbose>2) printf("\n");
//    if (fVerbose>0) printf(" Reconstructed muons: %2i/%2i/%2i",nReconstructedMuons,nAcceptedMuons,nGeneratedMuons);
  }
  if (fVerbose>0) printf("\n");
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaMuch::Finish(){
  hAllP->Sumw2();
  hAccStsP->Sumw2();
  hRecStsP->Sumw2();
  hAccMuchP->Sumw2();
  hRecMuchP->Sumw2();
  hMuMinusYPt->Sumw2();
  hMuPlusYPt->Sumw2();
  
  TH1D* hAcceptanceStsP     = new TH1D("hAcceptanceStsP","STS acceptance; p [MeV/c]",PBINNING);
  TH1D* hRecEfficiencyStsP  = new TH1D("hRecEfficiencyStsP","STS rec. efficiency; p [MeV/c]",PBINNING);
  TH1D* hAcceptanceMuchP    = new TH1D("hAcceptanceMuchP","STS+MUCH acceptance; p [MeV/c]",PBINNING);
  TH1D* hRecEfficiencyMuchP = new TH1D("hRecEfficiencyMuchP","STS+MUCH rec. efficiency; p [MeV/c]",PBINNING);

  hAcceptanceStsP->Divide(hAccStsP,hAllP);
  hAcceptanceMuchP->Divide(hAccMuchP,hAllP);
  hRecEfficiencyStsP->Divide(hRecStsP,hAccStsP);
  hRecEfficiencyMuchP->Divide(hRecMuchP,hAccMuchP);
  
  hAllP->SetLineColor(1);
  hAccStsP->SetLineColor(2);
  hRecStsP->SetLineColor(3);
  hAccMuchP->SetLineColor(4);
  hRecMuchP->SetLineColor(6);
  hAcceptanceStsP->SetLineColor(2);
  hRecEfficiencyStsP->SetLineColor(3);
  hAcceptanceMuchP->SetLineColor(4);
  hRecEfficiencyMuchP->SetLineColor(6);
  hAcceptanceStsP->SetMaximum(1);
  hAcceptanceMuchP->SetMaximum(1);
  hRecEfficiencyStsP->SetMaximum(1);
  hRecEfficiencyMuchP->SetMaximum(1);

  // Jpsi distributions
  TH1D* hJpsiY   = hJpsiYPt->ProjectionX();
  TH1D* hJpsiPt  = hJpsiYPt->ProjectionY();
  
  TCanvas* cJpsiDistrYPt = new TCanvas("cJpsiDistrYPt","J/#Psi y-pt distributions",300*3,300);
  cJpsiDistrYPt->Divide(3,1);
  cJpsiDistrYPt->cd(1);
  hJpsiYPt->Draw("colz");
  cJpsiDistrYPt->cd(2);
  hJpsiY->Fit("gaus");
  hJpsiY->Draw("e");
  cJpsiDistrYPt->cd(3);
  TF1* fDistPt = new TF1("distPt","[1]*x*exp(-sqrt(x*x+3.096616*3.096616)/[0])",0.,5.);
  fDistPt->SetParameter(0,0.150);
  fDistPt->SetParameter(1,1.);
  hJpsiPt->Fit("distPt");
  hJpsiPt->Draw("e");
  
  
  TCanvas* cMuDistrYPt = new TCanvas("cMuDistrYPt","Muon y-pt distributions",300*2,300);
  cMuDistrYPt->Divide(2,1);
  cMuDistrYPt->cd(1);
  hMuMinusYPt->Draw("colz");
  cMuDistrYPt->cd(2);
  hMuPlusYPt->Draw("colz");
  
  TCanvas* cAllDistrP = new TCanvas("cAllDistrP","All Distributions vs P",500,400);

  hAllP->Draw("e");
  hAccStsP->Draw("same");
  hRecStsP->Draw("same");
  hAccMuchP->Draw("same");
  hRecMuchP->Draw("same");
  
  TLegend* leg = new TLegend(0.6,0.6,0.9,0.9);
  leg->AddEntry(hAllP    ,"All","lp");
  leg->AddEntry(hAccStsP ,"STS accepted","lp");
  leg->AddEntry(hRecStsP ,"STS recontructed","lp");
  leg->AddEntry(hAccMuchP,"STS+MUCH accepted","lp");
  leg->AddEntry(hRecMuchP,"STS+MUCH reconstructed","lp");
  leg->Draw();
  
  TCanvas* cAcceptanceStsP = new TCanvas("cAcceptanceStsP","STS acceptance vs P",500,400);
  hAcceptanceStsP->Draw("e");

  TCanvas* cAcceptanceMuchP = new TCanvas("cAcceptanceMuchP","MUCH acceptance vs P",500,400);
  hAcceptanceMuchP->Draw("e");

  TCanvas* cRecEfficiencyStsP = new TCanvas("cRecEfficiencyStsP","STS rec. efficiency vs P",500,400);
  hRecEfficiencyStsP->Draw("e");
  
  TCanvas* cRecEfficiencyMuchP = new TCanvas("cRecEfficiencyMuchP","MUCH rec. efficiency vs P",500,400);
  hRecEfficiencyMuchP->Draw("e");
  
  TFile* f = new TFile(fHistoFileName.Data(),"recreate");
  hAllP->Write();
  hAccStsP->Write();
  hRecStsP->Write();
  hAccMuchP->Write();
  hRecMuchP->Write();
  hMuPlusYPt->Write();
  hMuMinusYPt->Write();
  hJpsiYPt->Write();
  hAcceptanceStsP->Write();
  hAcceptanceMuchP->Write();
  hRecEfficiencyStsP->Write();
  hRecEfficiencyMuchP->Write();
  cAllDistrP->Write();
  cMuDistrYPt->Write();
  cJpsiDistrYPt->Write();
  f->Close();
  
  printf("=========== Summary ============\n");
  printf("Events analysed: %i",fEvent);
  printf("STS acceptance: %5.3f\n",hAccStsP->Integral()/hAllP->Integral());
  printf("STS+MUCH acceptance: %5.3f\n",hAccMuchP->Integral()/hAllP->Integral());
  printf("STS rec. efficiency: %5.3f\n",hRecStsP->Integral()/hAccMuchP->Integral());
  printf("STS+MUCH rec. efficiency: %5.3f\n",hRecMuchP->Integral()/hRecStsP->Integral());
  printf("================================\n");
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Int_t CbmAnaMuch::GetTrackId(CbmTrackMatch* match, Double_t quota){
  Int_t trackId = match->GetMCTrackId();
  if (trackId<0) return -1;
  Int_t nTrue  = match->GetNofTrueHits();
  Int_t nWrong = match->GetNofWrongHits();
  Int_t nFake  = match->GetNofFakeHits();
  Double_t true_ratio = nTrue/((Double_t) (nTrue+nWrong+nFake));
  if (true_ratio<quota) return -2;
  return trackId;
}
// -------------------------------------------------------------------------

Bool_t CbmAnaMuch::IsReconstructed(CbmMuchTrack* track){
  Int_t nStations = fGeoScheme->GetNStations();
  Int_t nTriggerLayers = fGeoScheme->GetStation(nStations-1)->GetNLayers();
  Int_t nHits = track->GetNofHits();
  Int_t* triggerPlaneId = new Int_t[nTriggerLayers];
  Int_t* triggeredPlanes = new Int_t[nTriggerLayers];
  Int_t* hitsInStations = new Int_t[nStations];
  Int_t* layersInStations = new Int_t[nStations];
  
  for (Int_t iStation=0; iStation<nStations; iStation++){
    CbmMuchStation* station = fGeoScheme->GetStation(iStation);
    layersInStations[iStation] = station->GetNLayers();
  }
  
  for (Int_t i=0;i<nHits;i++){
    CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchHits->At(track->GetHitIndex(i));
    Int_t iStation = fGeoScheme->GetStationIndex(hit->GetAddress());
    hitsInStations[iStation]++;
  }
  
  Bool_t isReconstructed = 1;
  for (Int_t iStation=0;iStation<nStations-1;iStation++){
    isReconstructed*=hitsInStations[iStation];
  }
  isReconstructed*=(hitsInStations[nStations-1]>=layersInStations[nStations-1]);
  return isReconstructed;
}

ClassImp(CbmAnaMuch);
