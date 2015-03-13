#include "CbmAnaHypRecEff.h"

#include <vector>
#include <map>

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TH1D.h"
#include "TH2D.h"

#include "FairRootManager.h"

#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"

using std::vector;
using std::map;
using std::pair;

ClassImp(CbmAnaHypRecEff)

// -------------------------------------------------------------------------
CbmAnaHypRecEff::CbmAnaHypRecEff()
: FairTask("AnaHypRecEff"),
  fMCTracks(NULL),
  fRCTracks(NULL),
  fRCMatch(NULL),
  fMoPdg(0),
  fDaPdg(0),
  fEvent(0),
  hEventCounter(NULL),
  hMC(NULL),
  hAC(NULL),
  hRC(NULL),
  hACpr(NULL),
  hRCpr(NULL),
  hACpi(NULL),
  hRCpi(NULL)
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmAnaHypRecEff::CbmAnaHypRecEff(Int_t pdgCode, Int_t iVerbose)
  : FairTask("Task",iVerbose),
    fMCTracks(NULL),
    fRCTracks(NULL),
    fRCMatch(NULL),
    fMoPdg(pdgCode),
    fDaPdg(0),
    fEvent(0),
    hEventCounter(NULL),
    hMC(NULL),
    hAC(NULL),
    hRC(NULL),
    hACpr(NULL),
    hRCpr(NULL),
    hACpi(NULL),
    hRCpi(NULL)
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmAnaHypRecEff::~CbmAnaHypRecEff() {}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
InitStatus CbmAnaHypRecEff::Init() {
  FairRootManager* ioman = FairRootManager::Instance();
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  fRCTracks = (TClonesArray*) ioman->GetObject("StsTrack");
  fRCMatch  = (TClonesArray*) ioman->GetObject("StsTrackMatch");
  fEvent=0;

  hEventCounter = new TH1D("hEventCounter","",-1,1,1);

  hMC = new TH2D("hMC",";Rapidity;p_{t} [GeV/c]",20,0.,4.,20,0.,2.);
  hAC = new TH2D("hAC",";Rapidity;p_{t} [GeV/c]",20,0.,4.,20,0.,2.);
  hRC = new TH2D("hRC",";Rapidity;p_{t} [GeV/c]",20,0.,4.,20,0.,2.);

  hACpr = new TH1D("hACpr","p [GeV/c]",50,0.,10.);
  hRCpr = new TH1D("hRCpr","p [GeV/c]",50,0.,10.);
  hACpi = new TH1D("hACpi","p [GeV/c]",50,0.,2.5);
  hRCpi = new TH1D("hRCpi","p [GeV/c]",50,0.,2.5);

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypRecEff::Exec(Option_t* option){


  printf("Event:%i\n",fEvent++);
  hEventCounter->Fill(0);

  fDaPdg=0;
  if (fMoPdg==3312) fDaPdg=-211;
  if (fMoPdg==3334) fDaPdg=-321;

  map<int,Hyperon> mHyp;
  map<int,Hyperon>::iterator mHypIt;
  mHyp.clear();

  // Initial MC distributions
  for (Int_t i=0;i<fMCTracks->GetEntriesFast();i++){
      CbmMCTrack* mc = (CbmMCTrack*) fMCTracks->At(i);

//      if (mc->GetMotherID()>=0) continue;
      if (mc->GetPdgCode()==fMoPdg) {
          mHyp.insert(pair<int,Hyperon>(i,Hyperon()));
          TLorentzVector mom;
          mc->Get4Momentum(mom);
          mHyp[i].y=mom.Rapidity();
          mHyp[i].pt=mom.Pt();
          hMC->Fill(mHyp[i].y,mHyp[i].pt);
      }
  }

  // Accepted
  for (Int_t i=0;i<fMCTracks->GetEntriesFast();i++){
      CbmMCTrack* mc = (CbmMCTrack*) fMCTracks->At(i);
      if (mc->GetNPoints(kSTS)<4) continue;
      Int_t moId = mc->GetMotherId();
      if (moId<0) continue;
      CbmMCTrack* mo = (CbmMCTrack*) fMCTracks->At(moId);
      if (fMoPdg==3122) {
          if (mo->GetPdgCode()!=3122) continue;
          // if (mo->GetMotherID()>=0) continue;
          Double_t p = mc->GetP();
          if (mc->GetPdgCode()==-211) {mHyp[moId].piAC=1; hACpi->Fill(p);}
          if (mc->GetPdgCode()==2212) {mHyp[moId].prAC=1; hACpr->Fill(p);}
      } else if (fMoPdg==3312 || fMoPdg==3334) {
          if (mo->GetPdgCode()==fMoPdg && mc->GetPdgCode()==fDaPdg)
              mHyp[moId].daAC=1;
          if (mo->GetPdgCode()!=3122) continue;
          Int_t grId = mo->GetMotherId();
          if (grId<0) continue;
          CbmMCTrack* gr = (CbmMCTrack*) fMCTracks->At(grId);
          if (gr->GetPdgCode()!=fMoPdg) continue;
          if (mc->GetPdgCode()==-211) mHyp[grId].piAC=1;
          if (mc->GetPdgCode()==2212) mHyp[grId].prAC=1;
      }
  }


  // Reconstructed
  Double_t quota = 0.7;
  for (Int_t i=0;i<fRCMatch->GetEntriesFast();i++){
      CbmTrackMatch* match = (CbmTrackMatch*) fRCMatch->At(i);
      Int_t mcId = match->GetMCTrackId();
      if(mcId<0 || quota > (Double_t) match->GetNofTrueHits()
          /(match->GetNofTrueHits() + match->GetNofWrongHits()
            + match->GetNofFakeHits())) mcId=-1;
      if (mcId<0) continue;
      CbmMCTrack* mc = (CbmMCTrack*) fMCTracks->At(mcId);
      Int_t moId = mc->GetMotherId();
      if (moId<0) continue;
      CbmMCTrack* mo = (CbmMCTrack*) fMCTracks->At(moId);
      if (fMoPdg==3122) {
          if (mo->GetPdgCode()!=3122) continue;
//          if (mo->GetMotherID()>=0) continue;
          Double_t p = mc->GetP();
          if (mc->GetPdgCode()==-211) {mHyp[moId].piRC=1; hRCpi->Fill(p);}
          if (mc->GetPdgCode()==2212) {mHyp[moId].prRC=1; hRCpr->Fill(p);}
      } else if (fMoPdg==3312 || fMoPdg==3334) {
          if (mo->GetPdgCode()==fMoPdg && mc->GetPdgCode()==fDaPdg)
              mHyp[moId].daRC=1;
          if (mo->GetPdgCode()!=3122) continue;
          Int_t grId = mo->GetMotherId();
          if (grId<0) continue;
          CbmMCTrack* gr = (CbmMCTrack*) fMCTracks->At(grId);
          if (gr->GetPdgCode()!=fMoPdg) continue;
          if (mc->GetPdgCode()==-211) mHyp[grId].piRC=1;
          if (mc->GetPdgCode()==2212) mHyp[grId].prRC=1;
      }
  }


  for (mHypIt=mHyp.begin();mHypIt!=mHyp.end();mHypIt++){
    Hyperon &hyp = mHypIt->second;
    if (!hyp.piAC) continue;
    if (!hyp.prAC) continue;
    if (fMoPdg==3122 || hyp.daAC) hAC->Fill(hyp.y,hyp.pt);
  }

  for (mHypIt=mHyp.begin();mHypIt!=mHyp.end();mHypIt++){
    Hyperon &hyp = mHypIt->second;
    if (!hyp.piRC) continue;
    if (!hyp.prRC) continue;
    if (fMoPdg==3122 || hyp.daRC)  hRC->Fill(hyp.y,hyp.pt);
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypRecEff::Finish(){
    Double_t fMC = hMC->GetEntries();
    Double_t fAC = hAC->GetEntries();
    Double_t fRC = hRC->GetEntries();
    Double_t fACpr = hACpr->GetEntries();
    Double_t fRCpr = hRCpr->GetEntries();
    Double_t fACpi = hACpi->GetEntries();
    Double_t fRCpi = hRCpi->GetEntries();
    Double_t events = hEventCounter->GetEntries();
    printf("=======================\n");
    printf("Statistics on hyperons:\n");
    printf("Events:%i\n",(Int_t) events);
    printf("MC:%i (%6.3f)\n",(Int_t) fMC, fMC/fMC);
    printf("AC:%i (%6.3f)\n",(Int_t) fAC, fAC/fMC);
    printf("RC:%i (%6.3f)\n",(Int_t) fRC, fRC/fAC);
    printf("RCpr eff:(%6.3f)\n", fRCpr/fACpr);
    printf("RCpi eff:(%6.3f)\n", fRCpi/fACpi);
    hEventCounter->Write();
    hMC->Write();
    hAC->Write();
    hRC->Write();
    hACpr->Write();
    hRCpr->Write();
    hACpi->Write();
    hRCpi->Write();
}
// -------------------------------------------------------------------------
