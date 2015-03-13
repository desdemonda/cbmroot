/** CbmAnaMuchJpsiPolarization.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2009-09-11
 **/
#define PBINNING 50,0.,20.
#define PTBINNING 20,0.,3.
#define YBINNING 20,1.,3.
#define JPSIMBINNING 50,2.,4.
#define COSTBINNING 20,-1.,1.

#include "CbmAnaMuchJpsiPolarization.h"
#include "CbmAnaMuch.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmMuchTrack.h"
#include "CbmTrackMatch.h"
#include "CbmMuchPixelHit.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TF1.h"

// -----   Default constructor   -------------------------------------------
CbmAnaMuchJpsiPolarization::CbmAnaMuchJpsiPolarization()
  : FairTask("AnaMuchJpsiPolarization"),
    fEvent(0),
    fMCTracks(NULL),
    fStsTracks(NULL),
    fStsPoints(NULL),
    fMuchPoints(NULL),
    fMuchHits(NULL),
    fMuchTracks(NULL),
    fMuchTrackMatches(NULL),
    fStsTrackMatches(NULL),
    fStsPointsAccQuota(4),
    fStsTrueHitQuota(0.7),
    fMuchPointsAccQuota(13),
    fMuchTrueHitQuota(0.7),
    fHistoFileName("histo.root"),
    fhJpsiAllYPtCost(NULL),
    fhJpsiAccYPtCost(NULL),
    fhJpsiRecYPtCost(NULL),
    mapRecSts(),
    mapRecMuch(),
    mapAllMuPlus(),
    mapAccMuPlus(),
    mapRecMuPlus(),
    mapAllMuMinus(),
    mapAccMuMinus(),
    mapRecMuMinus()
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmAnaMuchJpsiPolarization::CbmAnaMuchJpsiPolarization(const char* name,TString histoFileName)
  : FairTask(name),
    fEvent(0),
    fMCTracks(NULL),
    fStsTracks(NULL),
    fStsPoints(NULL),
    fMuchPoints(NULL),
    fMuchHits(NULL),
    fMuchTracks(NULL),
    fMuchTrackMatches(NULL),
    fStsTrackMatches(NULL),
    fStsPointsAccQuota(4),
    fStsTrueHitQuota(0.7),
    fMuchPointsAccQuota(10),
    fMuchTrueHitQuota(0.7),
    fHistoFileName(histoFileName),
    fhJpsiAllYPtCost(NULL),
    fhJpsiAccYPtCost(NULL),
    fhJpsiRecYPtCost(NULL),
    mapRecSts(),
    mapRecMuch(),
    mapAllMuPlus(),
    mapAccMuPlus(),
    mapRecMuPlus(),
    mapAllMuMinus(),
    mapAccMuMinus(),
    mapRecMuMinus()
{
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmAnaMuchJpsiPolarization::~CbmAnaMuchJpsiPolarization(){
}
// -------------------------------------------------------------------------


// -----  SetParContainers -------------------------------------------------
void CbmAnaMuchJpsiPolarization::SetParContainers(){
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
InitStatus CbmAnaMuchJpsiPolarization::Init()
{
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
  
  if (!(fMCTracks&&fStsPoints&&fMuchPoints&&fMuchHits&&fStsTracks&&fMuchTracks&&fMuchTrackMatches&&fStsTrackMatches)){
    printf(" %p",fStsPoints);
    printf(" %p",fMuchPoints);
    printf(" %p",fStsTracks);
    printf(" %p",fMuchHits);
    printf(" %p",fMuchTracks);
    printf(" %p",fMuchTrackMatches);
    printf("\n");
    Fatal("Init","One of TCloneArrays not available");
  }  
  fhJpsiAllYPtCost = new TH3D("fhJpsiAllYPtCost","",YBINNING,PTBINNING,COSTBINNING);
  fhJpsiAccYPtCost = new TH3D("fhJpsiAccYPtCost","",YBINNING,PTBINNING,COSTBINNING);
  fhJpsiRecYPtCost = new TH3D("fhJpsiRecYPtCost","",YBINNING,PTBINNING,COSTBINNING);
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaMuchJpsiPolarization::Exec(Option_t* opt){
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
  if (fVerbose>0) printf("\n");

  Int_t muPlusAccepted = -1;
  Int_t muMinusAccepted = -1;
  
  TLorentzVector pMCmuPlus;
  TLorentzVector pMCmuMinus;
  TVector3 pRCmuPlus;
  TVector3 pRCmuMinus;
  
  mapRecSts.clear();
  mapRecMuch.clear();
  
  for (Int_t iTrack=0;iTrack<nMCTracks;iTrack++){
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iTrack);
    mapRecSts[iTrack]=-1;
    mapRecMuch[iTrack]=-1;
    if (mcTrack->GetMotherId()>=0) continue;
    Int_t nAccStsPoints  = mcTrack->GetNPoints(kSTS);
    Int_t nAccMuchPoints = mcTrack->GetNPoints(kMUCH);
    Int_t pdgCode = mcTrack->GetPdgCode();
    if (pdgCode== 13) mcTrack->Get4Momentum(pMCmuPlus);
    if (pdgCode==-13) mcTrack->Get4Momentum(pMCmuMinus);
    if (nAccStsPoints<fStsPointsAccQuota) continue;
    if (nAccMuchPoints<fMuchPointsAccQuota) continue;
    if (pdgCode== 13) muPlusAccepted   = iTrack;
    if (pdgCode==-13) muMinusAccepted  = iTrack;
  }

  TLorentzVector pMCJpsi = pMCmuPlus + pMCmuMinus;
  Double_t ptMC   = pMCJpsi.Pt();
  Double_t yMC    = pMCJpsi.Rapidity();
  Double_t costMC = Cost(pMCmuPlus,pMCmuMinus);
  fhJpsiAllYPtCost->Fill(yMC,ptMC,costMC);
  if (muPlusAccepted==1 && muMinusAccepted==0) fhJpsiAccYPtCost->Fill(yMC,ptMC,costMC);
  
  for (Int_t iTrack=0;iTrack<nStsTracks;iTrack++){
    CbmStsTrack*   stsTrack      = (CbmStsTrack*) fStsTracks->At(iTrack);
    CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*) fStsTrackMatches->At(iTrack);
    Int_t trackId = CbmAnaMuch::GetTrackId(stsTrackMatch,fStsTrueHitQuota);
    mapRecSts[trackId]=iTrack;
  }
  
  for (Int_t iTrack=0;iTrack<nMuchTracks;iTrack++){
    CbmMuchTrack*  muchTrack      = (CbmMuchTrack*)  fMuchTracks->At(iTrack);
    if (!Trigger(muchTrack)) continue;
    CbmTrackMatch* muchTrackMatch = (CbmTrackMatch*) fMuchTrackMatches->At(iTrack);
    Int_t trackId = CbmAnaMuch::GetTrackId(muchTrackMatch,fMuchTrueHitQuota);
    mapRecMuch[trackId]=iTrack;
  }

  // reconstructed dimuon
  if (mapRecSts[0]>=0 && mapRecSts[1]>=0 && mapRecMuch[0]>=0 && mapRecMuch[1]>=0){
    fhJpsiRecYPtCost->Fill(yMC,ptMC,costMC);
  }
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaMuchJpsiPolarization::Finish(){
  TH1D* fhJpsiAllCost = fhJpsiAllYPtCost->ProjectionZ();
  TH1D* fhJpsiAccCost = fhJpsiAccYPtCost->ProjectionZ();
  TH1D* fhJpsiRecCost = fhJpsiRecYPtCost->ProjectionZ();
  fhJpsiAllCost->Sumw2();
  fhJpsiAccCost->Sumw2();
  fhJpsiRecCost->Sumw2();
  fhJpsiAllCost->SetMinimum(0);

  TCanvas* cCost = new TCanvas("cCost","Cos #theta distributions",1200,400);
  cCost->Divide(3,1);
  cCost->cd(1);
  fhJpsiAllCost->Draw();
  cCost->cd(2);
  fhJpsiAccCost->Draw();
  cCost->cd(3);
  fhJpsiRecCost->Draw();
  
  TFile* f = new TFile(fHistoFileName,"recreate");
  fhJpsiAllYPtCost->Write();
  fhJpsiRecYPtCost->Write();
  f->Close();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t CbmAnaMuchJpsiPolarization::Cost(TLorentzVector pMuPlus, TLorentzVector pMuMinus){
  TLorentzVector pJpsi = pMuPlus + pMuMinus;
  TVector3 beta = pJpsi.BoostVector();
  pMuPlus.Boost(-beta);
  TVector3 zaxis = (pJpsi.Vect()).Unit();
  return zaxis.Dot((pMuPlus.Vect()).Unit());
}
// -------------------------------------------------------------------------

Bool_t CbmAnaMuchJpsiPolarization::Trigger(CbmMuchTrack* track){
  Int_t nHits = track->GetNofHits();
  Int_t triggerPlaneId[3]={11,12,13};
  Bool_t triggeredPlanes[3]={0,0,0};
  for (Int_t i=0;i<nHits;i++){
    CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchHits->At(track->GetHitIndex(i));
    Int_t planeId = hit->GetPlaneId();
    for (Int_t t=0;t<3;t++) if (planeId==triggerPlaneId[t]) triggeredPlanes[t]=1;
//    printf("%i\n",planeId);
  }
  Bool_t trigger = 1;
  for (Int_t t=0;t<3;t++) trigger*=triggeredPlanes[t];
//  printf("trigger=%i\n",trigger);
  return trigger;
}

ClassImp(CbmAnaMuchJpsiPolarization);
