/** CbmAnaAngularAcceptance.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-01-02
 **/

#include "CbmAnaAngularAcceptance.h"
#include "CbmAnaMuonCandidate.h"
#include "CbmAnaDimuonCandidate.h"
#include "CbmAnaMuch.h"
#include "CbmMCTrack.h"
#include "CbmMuchPoint.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLorentzVector.h"

// -----   Default constructor   -------------------------------------------
CbmAnaAngularAcceptance::CbmAnaAngularAcceptance()
  : FairTask("AnaAngularAcceptance"),
    fEvent(0),
    fMCTracks(NULL),
    fStsTracks(NULL),
    fStsPoints(NULL),
    fMuchPoints(NULL),
    fStsTrackMatches(NULL),
    fSignalPairs(15),
    fDimuonCandidates(new TClonesArray("CbmAnaDimuonCandidate")),
    fTanMin(NULL),
    fTanMax(NULL)
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmAnaAngularAcceptance::CbmAnaAngularAcceptance(const char* name)
  : FairTask(name),
    fEvent(0),
    fMCTracks(NULL),
    fStsTracks(NULL),
    fStsPoints(NULL),
    fMuchPoints(NULL),
    fStsTrackMatches(NULL),
    fSignalPairs(15),
    fDimuonCandidates(new TClonesArray("CbmAnaDimuonCandidate")),
    fTanMin(NULL),
    fTanMax(NULL)
{
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmAnaAngularAcceptance::~CbmAnaAngularAcceptance(){
}
// -------------------------------------------------------------------------


// -----  SetParContainers -------------------------------------------------
void CbmAnaAngularAcceptance::SetParContainers(){
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
InitStatus CbmAnaAngularAcceptance::Init()
{
  // Get and check FairRootManager
  FairRootManager* fManager = FairRootManager::Instance();
  fMCTracks         = (TClonesArray*) fManager->GetObject("MCTrack");
  fStsPoints        = (TClonesArray*) fManager->GetObject("StsPoint");
  fMuchPoints       = (TClonesArray*) fManager->GetObject("MuchPoint");
  fEvent=0;
  fTanMin = new TH1D("fTanMin","",100,0,1);
  fTanMax = new TH1D("fTanMax","",100,0,1);
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaAngularAcceptance::Exec(Option_t* opt){
  Int_t nMCTracks      = fMCTracks->GetEntriesFast();
  Int_t nStsPoints     = fStsPoints->GetEntriesFast();
  Int_t nMuchPoints    = fMuchPoints->GetEntriesFast();
  if (fVerbose>-1) printf(" Event: %4i",fEvent++);
  if (fVerbose>0) printf(" MCtracks: %4i",nMCTracks);
  if (fVerbose>0) printf(" StsPoints: %4i",nStsPoints);
  if (fVerbose>0) printf(" MuchPoints: %4i",nMuchPoints);
  if (fVerbose>-1) printf("\n");
  
  fDimuonCandidates->Clear();
  
  
  // Create array of signal dimuons
  for (Int_t iDimuon=0;iDimuon<fSignalPairs;iDimuon++){
    new((*fDimuonCandidates)[iDimuon]) CbmAnaDimuonCandidate();
  }
  
  // Set MC momentum and MC track reference
  for (Int_t iMCTrack=0;iMCTrack<2*fSignalPairs;iMCTrack++){
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrack);
    // check mother Id for safety
    if (mcTrack->GetMotherId()>=0) { Error("Exec","motherId for signal track >=0"); continue; }
    // get info
    Int_t nAccStsPoints  = mcTrack->GetNPoints(kSTS);
    Int_t nAccMuchPoints = mcTrack->GetNPoints(kMUCH);
    Int_t pdgCode = mcTrack->GetPdgCode();
    TLorentzVector pMC;
    mcTrack->Get4Momentum(pMC);
    // set muon momentum and MC track reference
    CbmAnaMuonCandidate* mu = GetMu(iMCTrack);
    mu->SetMomentumMC(pMC);
    mu->SetMCTrackId(iMCTrack);
    mu->SetNMuchPoints(nAccMuchPoints);
    mu->SetNStsPoints(nAccStsPoints);
    // TODO add other info
  }
  
  for (Int_t iMuchPoint=0;iMuchPoint<nMuchPoints;iMuchPoint++){
    CbmMuchPoint* p = (CbmMuchPoint*) fMuchPoints->At(iMuchPoint);
    Int_t trackId = p->GetTrackID();
    if (trackId>=fSignalPairs*2) continue;
    CbmAnaMuonCandidate* mu = GetMu(trackId);
    mu->SetNMuchHits(iMuchPoint);
  }

  for (Int_t iDimuon=0;iDimuon<fSignalPairs;iDimuon++){
    CbmAnaDimuonCandidate* dimuon = (CbmAnaDimuonCandidate*) fDimuonCandidates->At(iDimuon);
    Int_t n1 = dimuon->GetMu(0)->GetNMuchHits();
    Int_t n2 = dimuon->GetMu(1)->GetNMuchHits();
    Int_t acc1 = dimuon->GetMu(0)->GetNMuchPoints();
    Int_t acc2 = dimuon->GetMu(1)->GetNMuchPoints();
    if (acc1<1 && acc2<1) continue;
    CbmMuchPoint* p1 = (CbmMuchPoint*) fMuchPoints->At(n1);
    CbmMuchPoint* p2 = (CbmMuchPoint*) fMuchPoints->At(n2);
    Double_t x1 = p1->GetXIn();
    Double_t y1 = p1->GetYIn();
    Double_t x2 = p2->GetXIn();
    Double_t y2 = p2->GetYIn();
    Double_t r1 = sqrt(x1*x1+y1*y1);
    Double_t r2 = sqrt(x2*x2+y2*y2);
    Double_t r = 205.;
    Double_t t1 = r1/r;
    Double_t t2 = r2/r;
    Double_t tMax = (t1>t2) ? t1 : t2;
    Double_t tMin = (t1>t2) ? t2 : t1;
    fTanMin->Fill(tMin);
    fTanMax->Fill(tMax);
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmAnaMuonCandidate* CbmAnaAngularAcceptance::GetMu(Int_t trackId){
  if (trackId<0) Fatal("GetMu","MCtrackId for signal muon < 0");
  if (trackId>=2*fSignalPairs) Fatal("GetMu","MCtrackId for signal muon > 2*dimuons");
  Int_t iDimuon = trackId/2;
  Int_t sign = trackId%2; // 0 - muPlus, 1 - muMinus
  return ((CbmAnaDimuonCandidate*) fDimuonCandidates->At(iDimuon))->GetMu(sign); 
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaAngularAcceptance::Finish(){
  fTanMin->Scale(1./fTanMin->Integral(0,101));
  fTanMax->Scale(1./fTanMax->Integral(0,101));
  
  TCanvas* cTan = new TCanvas("cTan","cTan",800,400);
  cTan->Divide(2,1);
  cTan->cd(1);
  fTanMin->Draw();
  cTan->cd(2);
  fTanMax->Draw();

  TH1D* fTanMinAc = new TH1D("fTanMinAc","",100,0,1);
  TH1D* fTanMaxAc = new TH1D("fTanMaxAc","",100,0,1);
  
  
  for (Int_t i=1;i<=100;i++) {
    fTanMinAc->SetBinContent(i,fTanMin->Integral(i,101));
    fTanMaxAc->SetBinContent(i,fTanMax->Integral(0,i));
  }
  
  TCanvas* cTanAc = new TCanvas("cTanAc","cTanAc",800,400);
  cTanAc->Divide(2,1);
  cTanAc->cd(1);
  fTanMinAc->Draw();
  cTanAc->cd(2);
  fTanMaxAc->Draw();

}
// -------------------------------------------------------------------------

ClassImp(CbmAnaAngularAcceptance);
