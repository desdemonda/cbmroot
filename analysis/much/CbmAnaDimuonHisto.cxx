/** CbmAnaDimuonHisto.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-01-02
 **/

#include "CbmAnaDimuonHisto.h"
#include "CbmAnaDimuonCandidate.h"
#include "CbmAnaMuonCandidate.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TChain.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"

CbmAnaDimuonHisto::CbmAnaDimuonHisto()
  : FairTask("AnaDimuonHisto"),
    fEvent(0),
    fMuCandidates(NULL),
    fDimuonCandidates(NULL),
    fChain(NULL),
    fDimuonMmc(NULL),
    fDimuonMrc(NULL),
    fBgdM(NULL),
    fMmin(0.7),
    fMmax(0.9),
    fMbins(100),
    fMminCut(0.78259-2*0.0083),
    fMmaxCut(0.78259+2*0.0083),
    fMuchHitsCut(1),
    fStsHitsCut(1),
    fChiToVertexCut(101.),
    fMultiplicity(38),
    fBranching(9.e-5),
    fSignalPairs(1),
    fNoMixedEv(1),
    fNEvents(0)
{
}

// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmAnaDimuonHisto::Init()
{
  // Get and check FairRootManager
  FairRootManager* fManager = FairRootManager::Instance();
  fMuCandidates     = (TClonesArray*) fManager->GetObject("MuCandidates");
  fDimuonCandidates = (TClonesArray*) fManager->GetObject("DimuonCandidates");
  fChain=fManager->GetInChain();
  fNEvents = fChain->GetEntries();
  fEvent=0;
  printf("fNEvents=%i\n",fNEvents);
  if (!(fMuCandidates && fDimuonCandidates)){
    printf(" %p",fMuCandidates);
    printf(" %p",fDimuonCandidates);
    printf("\n");
    Fatal("Init","One of TCloneArrays not available");
  }  

  fDimuonMmc = new TH1D("hDimuonMmc",";M_{#mu#mu} [GeV/c];Entries",fMbins,fMmin,fMmax);
  fDimuonMrc = new TH1D("hDimuonMrc",";M_{#mu#mu} [GeV/c];Entries",fMbins,fMmin,fMmax);
  fBgdM      = new TH1D("hBgdM"     ,";M_{#mu#mu} [GeV/c];Entries",fMbins,fMmin,fMmax);

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaDimuonHisto::Exec(Option_t* opt){
  Int_t nDimuons       = fDimuonCandidates->GetEntriesFast();
  Int_t nMuons         = fMuCandidates->GetEntriesFast();
  if (fVerbose>-1) printf(" Event: %4i",fEvent);
  if (fVerbose>0) printf(" Dimuons: %2i",nDimuons);
  if (fVerbose>0) printf(" Muons: %2i",nMuons);
  if (fVerbose>-1) printf("\n");
 
  for (Int_t iDimuon=0;iDimuon<nDimuons;iDimuon++){
    CbmAnaDimuonCandidate* dimuon = (CbmAnaDimuonCandidate*) fDimuonCandidates->At(iDimuon);
    fDimuonMmc->Fill(dimuon->GetMomentumMC().M());
    if (!dimuon->IsReconstructed(fMuchHitsCut,fStsHitsCut,fChiToVertexCut)) continue;
    fDimuonMrc->Fill(dimuon->GetMomentumRC().M());
    printf("  m = %f\n",dimuon->GetMomentumRC().M());
  }//dimuon

  for (Int_t iMuP=0;iMuP<nMuons;iMuP++){
    CbmAnaMuonCandidate* muP = (CbmAnaMuonCandidate*) fMuCandidates->At(iMuP);
    if (muP->GetSign()<0) continue;
    if (!muP->IsReconstructed(fMuchHitsCut,fStsHitsCut,fChiToVertexCut)) continue;
    TLorentzVector pP = TLorentzVector(*(muP->GetMomentumRC()));
    for (Int_t ev=fEvent+1;ev<fEvent+1+fNoMixedEv;ev++){
      fChain->GetEntry(ev < fNEvents ? ev : ev-fNEvents);
      for (Int_t iMuN=0;iMuN<fMuCandidates->GetEntriesFast();iMuN++){
        CbmAnaMuonCandidate* muN = (CbmAnaMuonCandidate*) fMuCandidates->At(iMuN);
        if (muN->GetSign()>0) continue;
        if (!muN->IsReconstructed(fMuchHitsCut,fStsHitsCut,fChiToVertexCut)) continue;
        TLorentzVector &pN = *muN->GetMomentumRC();
        fBgdM->Fill((pN+pP).M());
      } // negative muons
    } // events
    fChain->GetEntry(fEvent);
  } // positive muons

  fEvent++;
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaDimuonHisto::Finish(){
  fDimuonMmc->Sumw2();
  fDimuonMrc->Sumw2();
  fBgdM->Sumw2();
  
  fDimuonMmc->Scale(1./fSignalPairs*fMultiplicity*fBranching/fEvent);
  fDimuonMrc->Scale(1./fSignalPairs*fMultiplicity*fBranching/fEvent);
  fBgdM->Scale(1./fNoMixedEv/fEvent);

  Int_t mMinCutBin = fDimuonMrc->GetXaxis()->FindFixBin(fMminCut);
  Int_t mMaxCutBin = fDimuonMrc->GetXaxis()->FindFixBin(fMmaxCut);
  
  TCanvas* c1 = new TCanvas("cM","Invariant mass",800,800);
  c1->Divide(2,2);
  c1->cd(1);
  fDimuonMmc->Draw("e");
  
  c1->cd(2);
  fDimuonMrc->Fit("gaus");
  fDimuonMrc->Draw("e");
  
  c1->cd(3);
  fBgdM->SetMinimum(0);
//  fBgdM->Add(fDimuonMrc);
  fBgdM->Draw("e");

  Double_t S = fDimuonMrc->Integral(mMinCutBin,mMaxCutBin);
  Double_t B = fBgdM->Integral(mMinCutBin,mMaxCutBin);
  Double_t Stotal = fDimuonMmc->Integral();
  printf(" Ef=%6.4f",S/Stotal);
  printf(" S/B=%6.4f",S/B);
  printf("\n");

  fDimuonMmc->Write();
  fDimuonMrc->Write();
  fBgdM->Write();
}
// -------------------------------------------------------------------------


ClassImp(CbmAnaDimuonHisto);
