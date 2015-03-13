#include "CbmEcalAnalysis.h"

#include "CbmEcalPoint.h"
#include "CbmEcalPointLite.h"
#include "CbmEcalHitFastMC.h"
#include "CbmEcalRecParticle.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"

#include "TParticle.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

//------------------------------------------------------------------------------
CbmEcalAnalysis::CbmEcalAnalysis() 
  : FairTask(),
    fListStack(NULL),
    fListECALptsEdep(NULL),
    fListECALptsWall(NULL),
    fListECALhits(NULL),
    fListECALrp(NULL),
    fEvent(0),
    fNprimary(0),
    fKfVectorMeson(0),
    fhPrimPabs(NULL),
    fhPrimPt(NULL),
    fhPrimEta(NULL),
    fhPrimTheta(NULL),
    fhPrimPhi(NULL),
    fhPrimPtEta(NULL),
    fhMcpPabs(NULL),
    fhMcpPt(NULL),
    fhMcpEta(NULL),
    fhMcpTheta(NULL),
    fhMcpPhi(NULL),
    fhMcpPtEta(NULL),
    fhMcpXY(NULL),
    fhHitXY(NULL),
    fhHitE(NULL),
    fhRecPabs(NULL),
    fhRecPt(NULL),
    fhRecYrap(NULL),
    fhRecPhi(NULL),
    fhPtYrap(NULL),
    fhPtYrapSignal(NULL),
    fhPtYrapAccept(NULL),
    fDebug("")
{
}

//------------------------------------------------------------------------------
CbmEcalAnalysis::CbmEcalAnalysis(const char *name, const char *title)
  : FairTask(name),
    fListStack(NULL),
    fListECALptsEdep(NULL),
    fListECALptsWall(NULL),
    fListECALhits(NULL),
    fListECALrp(NULL),
    fEvent(0),
    fNprimary(0),
    fKfVectorMeson(443),
    fhPrimPabs(NULL),
    fhPrimPt(NULL),
    fhPrimEta(NULL),
    fhPrimTheta(NULL),
    fhPrimPhi(NULL),
    fhPrimPtEta(NULL),
    fhMcpPabs(NULL),
    fhMcpPt(NULL),
    fhMcpEta(NULL),
    fhMcpTheta(NULL),
    fhMcpPhi(NULL),
    fhMcpPtEta(NULL),
    fhMcpXY(NULL),
    fhHitXY(NULL),
    fhHitE(NULL),
    fhRecPabs(NULL),
    fhRecPt(NULL),
    fhRecYrap(NULL),
    fhRecPhi(NULL),
    fhPtYrap(NULL),
    fhPtYrapSignal(NULL),
    fhPtYrapAccept(NULL),
    fDebug("")
{
  SetTitle(title);
  Double_t pi = TMath::Pi();

  // Primary tracks histograms

  fhPrimPabs = new TH1F("hPrimPabs" ,"Momentum of primary particles",100, 0.,50.);
  fhPrimPt   = new TH1F("hPrimPt"   ,"p_{T} of primary particles"   ,100, 0.,10.);
  fhPrimEta  = new TH1F("hPrimEta"  ,"#eta of primary particles"    , 80,-2., 6.);
  fhPrimTheta= new TH1F("hPrimTheta","#theta of primary particles"  ,100, 0., pi);
  fhPrimPhi  = new TH1F("hPrimPhi"  ,"#phi of primary particles"    ,100,-pi, pi);
  fhPrimPtEta= new TH2F("hPrimPtEta","p_{T},#eta of primary particles",100, 0.,10., 80,-2., 6.);

  // MC points histograms

  fhMcpPabs = new TH1F("hMcpPabs" ,"Momentum of detected particles",120, 0.,30.);
  fhMcpPt   = new TH1F("hMcpPt"   ,"p_{T} of detected particles"   ,100, 0.,10.);
  fhMcpEta  = new TH1F("hMcpEta"  ,"#eta of detected particles"    , 80,-2., 6.);
  fhMcpTheta= new TH1F("hMcpTheta","#theta of detected particles"  ,100, 0., pi);
  fhMcpPhi  = new TH1F("hMcpPhi"  ,"#phi of detected particles"    ,100,-pi, pi);
  fhMcpPtEta= new TH2F("hMcpPtEta","p_{T},#eta of detected particles",
		       100, 0.,10., 80,-2., 6.);
  fhMcpXY   = new TH2F("hMcpXY","(X,Y) coordinates of MC points",
		       300,-600,600,300,-600,600);

  // Hits histograms

  fhHitXY = new TH2F("hHitXY","ECAL hit (x,y)",100,-600.,600.,100,-500.,500.);
  fhHitE  = new TH1F("hHitE" ,"ECAL hit energy",120,0.,30.);

  // Reconstructed particles histograms

  fhRecPabs = new TH1F("hRecPabs","Reconstructed |p|"  ,120, 0.,30.);
  fhRecPt   = new TH1F("hRecPt"  ,"Reconstructed p_{T}",100, 0.,10.);
  fhRecYrap = new TH1F("hRecYrap","Reconstructed y"    , 80,-2., 6.);
  fhRecPhi  = new TH1F("hRecPhi" ,"Reconstructed #phi" ,100,-pi, pi);

  // Acceptance

  fhPtYrap = new TH2F("PtYrap","pt vs y of primary particles", 100,0.,6.,100,0.,5.);
  fhPtYrapSignal = new TH2F("PtYrapSignal","pt vs y of signal particles", 100,0.,6.,100,0.,5.);
  fhPtYrapAccept = new TH2F("PtYrapAccept","ECAL p_{T}-y acceptance",100,0.,6.,100,0.,5.);

}

//------------------------------------------------------------------------------
CbmEcalAnalysis::~CbmEcalAnalysis()
{

  // Primary tracks histograms

  fhPrimPabs ->Write();
  fhPrimPt   ->Write();
  fhPrimEta  ->Write();
  fhPrimTheta->Write();
  fhPrimPhi  ->Write();
  fhPrimPtEta->Write();

  // MC points histograms

  fhMcpPabs ->Write();
  fhMcpPt   ->Write();
  fhMcpEta  ->Write();
  fhMcpTheta->Write();
  fhMcpPhi  ->Write();
  fhMcpPtEta->Write();
  fhMcpXY->Write();

  // Hits histograms

  fhHitXY->Write();
  fhHitE ->Write();

  // Reconstructed particles histograms

  fhRecPabs ->Write();
  fhRecPt   ->Write();
  fhRecYrap ->Write();
  fhRecPhi  ->Write();

  //Acceptance histograms

  fhPtYrap->Sumw2(); fhPtYrap->Write(); 
  fhPtYrapSignal->Sumw2(); fhPtYrapSignal->Write();
  fhPtYrapAccept->Divide(fhPtYrapSignal,fhPtYrap);
  fhPtYrapAccept->Write();

}

//------------------------------------------------------------------------------
InitStatus CbmEcalAnalysis::Init()
{
  // Activate branches with ECAL objects

  FairRootManager *manager= FairRootManager::Instance();
  
  // all tracks
  fListStack        = (TClonesArray*)manager->GetObject("MCTrack");

  //ECAL MC points inside ECAL
  fListECALptsEdep  = (TClonesArray*)manager->GetObject("EcalPointLite");

  //ECAL MC points on entrance to ECAL
  fListECALptsWall  = (TClonesArray*)manager->GetObject("EcalPoint");

  //ECAL hits
  fListECALhits     = (TClonesArray*)manager->GetObject("EcalHitFastMC");

  //ECAL reconstructed points
  fListECALrp       = (TClonesArray*)manager->GetObject("EcalRecParticle");

  return kSUCCESS;
}

//------------------------------------------------------------------------------
void CbmEcalAnalysis::Exec(Option_t* option)
{
  // make ECAL analysis

  printf("ECAL analysis: event %d\n",fEvent);

  AnalyzePrimary();
  AnalyzeMCPointsEdep();
  AnalyzeMCPointsWall();
  AnalyzeHits();
  AnalyzeRecParticles();
  AnalyzeAcceptanceVectorMesons();

  fEvent++;
}

//------------------------------------------------------------------------------
void CbmEcalAnalysis::AnalyzePrimary()
{
  // Primary track analysis

  CbmMCTrack *primary;
  fNprimary = 0;
  TLorentzVector momentum;
  Float_t energyTotal = 0;
  for (Int_t iPrimary=0; iPrimary<fListStack->GetEntries(); iPrimary++) {
    primary = (CbmMCTrack*)fListStack->At(iPrimary);
    if (primary->GetMotherId() != -1) continue;
    fNprimary++;
    primary->Get4Momentum(momentum);
    if (strstr(fDebug,"prim"))
      printf("track %d: id=%d, p=(%f,%f,%f,%f) GeV/c, pt,y,theta=%f,%f,%f\n",
	     iPrimary,
	     primary->GetPdgCode(),
	     momentum.Px(),
	     momentum.Py(),
	     momentum.Pz(),
	     momentum.Energy(),
	     momentum.Pt(),
	     momentum.Eta(),
	     momentum.Theta());
    fhPrimPabs ->Fill(momentum.P());
    fhPrimPt   ->Fill(momentum.Pt());
    fhPrimEta  ->Fill(momentum.Eta());
    fhPrimTheta->Fill(momentum.Theta());
    fhPrimPhi  ->Fill(momentum.Phi());
    fhPrimPtEta->Fill(momentum.Pt(),momentum.Eta());
    energyTotal+=momentum.P();
  }
  printf("Total energy of primaries: %F GeV\n",energyTotal);

  if (strstr(fDebug,"prim"))
    printf("\tNumber of primaries: %d\n",fNprimary);

  
}

//------------------------------------------------------------------------------
void CbmEcalAnalysis::AnalyzeMCPointsEdep()
{
  // Monte-Carlo points inside ECAL

  if (fListECALptsEdep == 0) return;
  if (strstr(fDebug,"mcp"))
      printf("\tNumber of ECAL MC points in ECAL: %d\n",fListECALptsEdep->GetEntries());

  CbmEcalPointLite* mcPoint=NULL;
  TVector3 xyz;
  TLorentzVector momentum;

  Float_t energyTotal = 0;
  for (Int_t iPoint=0; iPoint<fListECALptsEdep->GetEntries(); iPoint++) {
    mcPoint = (CbmEcalPointLite*)fListECALptsEdep->At(iPoint); // MC point
    energyTotal+=mcPoint->GetEnergyLoss();
  }
  printf("Total energy in MC points: %F GeV\n",energyTotal);
}

//------------------------------------------------------------------------------
void CbmEcalAnalysis::AnalyzeMCPointsWall()
{
  // Monte-Carlo points on entrance to ECAL

  if (fListECALptsWall == 0) return;
  if (strstr(fDebug,"mcp"))
      printf("\tNumber of ECAL MC points on ECAL: %d\n",fListECALptsWall->GetEntries());

  CbmEcalPoint* mcPoint=NULL;
  TVector3 xyz;
  TLorentzVector momentum;

  for (Int_t iPoint=0; iPoint<fListECALptsWall->GetEntries(); iPoint++) {
    mcPoint = (CbmEcalPoint*)fListECALptsWall->At(iPoint); // MC point
    CbmMCTrack* track = (CbmMCTrack*)fListStack->At(mcPoint->GetTrackID());
    if (track->GetPdgCode() != 22 ) continue; // not a photon!
    mcPoint->Position(xyz);
    fhMcpXY->Fill(xyz.X(),xyz.Y());

    track->Get4Momentum(momentum);
    if (strstr(fDebug,"mcp"))
      printf("MC point %d: id=%d, p=(%f,%f,%f,%f) GeV/c, pt,y,theta=%f,%f,%f\n",
	     iPoint,
	     track->GetPdgCode(),
	     momentum.Px(),
	     momentum.Py(),
	     momentum.Pz(),
	     momentum.Energy(),
	     momentum.Pt(),
	     momentum.Eta(),
	     momentum.Theta());
    fhMcpPabs ->Fill(momentum.P());
    fhMcpPt   ->Fill(momentum.Pt());
    fhMcpEta  ->Fill(momentum.Eta());
    fhMcpTheta->Fill(momentum.Theta());
    fhMcpPhi  ->Fill(momentum.Phi());
    fhMcpPtEta->Fill(momentum.Pt(),momentum.Eta());
  }
}

//------------------------------------------------------------------------------
void CbmEcalAnalysis::AnalyzeHits()
{
  // ECAL hit analysis for Fast MC

  if (fListECALhits == 0) return;
  if (strstr(fDebug,"hit"))
      printf("\tNumber of ECAL hits: %d\n",fListECALhits->GetEntries());

  CbmEcalHitFastMC* hit=NULL;

  for (Int_t iHit=0; iHit<fListECALhits->GetEntries(); iHit++) {
    hit = (CbmEcalHitFastMC*)fListECALhits->At(iHit);
    fhHitXY->Fill(hit->GetX(),hit->GetY());
    fhHitE ->Fill(hit->GetAmplitude());
  }
}

//------------------------------------------------------------------------------
void CbmEcalAnalysis::AnalyzeRecParticles()
{
  // ECAL reconstructed particle analysis

  if (fListECALrp   == 0) return;
  if (strstr(fDebug,"rp"))
      printf("\tNumber of ECAL reconstructed particles: %d\n",fListECALrp->GetEntries());

  CbmEcalRecParticle* recpart=NULL;

  for (Int_t iRP=0; iRP<fListECALrp->GetEntries(); iRP++) {
    recpart = (CbmEcalRecParticle*)fListECALrp->At(iRP);
    TLorentzVector momentum = recpart->GetMomentum();
    fhRecPabs->Fill(momentum.P());
    fhRecPt  ->Fill(momentum.Pt());
    fhRecYrap->Fill(momentum.Rapidity());
    fhRecPhi ->Fill(momentum.Phi());
  }
}

//------------------------------------------------------------------------------
void CbmEcalAnalysis::AnalyzeAcceptanceElectrons()
{
  // Acceptance analysis
}

//------------------------------------------------------------------------------
void CbmEcalAnalysis::AnalyzeAcceptanceVectorMesons()
{
  // Acceptance analysis for rho --> e+e-

  if (fListStack == 0 || fListECALptsWall == 0) return;
  if (!fListStack->GetEntries()) {
    Fatal("AnalyzeAcceptances","No particles in stack!");
    return;
  }

  if ( ((CbmMCTrack*)fListStack->At(0))->GetPdgCode() != fKfVectorMeson)
    return;

  // if we have the single primary charged particle in event

  CbmMCTrack* primary = (CbmMCTrack*)fListStack->At(0);
  Double_t y = primary->GetRapidity();;
  Double_t pt = primary->GetPt();
  fhPtYrap->Fill(y, pt);

  Bool_t detected = kFALSE;
  Int_t nDetected = 0;
  CbmEcalPoint* mcPoint=NULL;
  // Detected e+e- from J/psi decay
  if (fKfVectorMeson == 443) {
    for (Int_t iPoint=0; iPoint<fListECALptsWall->GetEntries(); iPoint++) {
      mcPoint = (CbmEcalPoint*)fListECALptsWall->At(iPoint); // MC point
      CbmMCTrack* track = (CbmMCTrack*)fListStack->At(mcPoint->GetTrackID());
      if (track->GetMotherId()==0 && track->GetPdgCode() == 11) {
	nDetected++;
      }
      else if (track->GetMotherId()==0 && track->GetPdgCode() == -11) {
	nDetected++;
      }
    }
    if (nDetected == 2) detected = kTRUE;
  }
  else if (fKfVectorMeson == 223) {
    for (Int_t iPoint=0; iPoint<fListECALptsWall->GetEntries(); iPoint++) {
      mcPoint = (CbmEcalPoint*)fListECALptsWall->At(iPoint); // MC point
      CbmMCTrack* track       = (CbmMCTrack*)fListStack->At(mcPoint->GetTrackID());
      CbmMCTrack* trackMother = (CbmMCTrack*)fListStack->At(track->GetMotherId());
      if (track->GetMotherId()==0 && track->GetPdgCode() == 22) {
	nDetected++;
      }
      else if (trackMother->GetMotherId()==   0 &&
	       trackMother->GetPdgCode() == 111 &&
	       track->GetPdgCode()       == 22) {
	nDetected++;
      }
    }
    if (nDetected == 3) detected = kTRUE;
  }

  if (detected) fhPtYrapSignal->Fill(y, pt);
}

//------------------------------------------------------------------------------
void CbmEcalAnalysis::Finish()
{}


//------------------------------------------------------------------------------
ClassImp(CbmEcalAnalysis)
