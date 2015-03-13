#include "CbmAnaMuchBgd.h"
#include "TTree.h"
#include "vector"
#include "FairRunAna.h"
#include "TH1D.h"
#include "CbmGlobalTrack.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "CbmMuchTrack.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
#include "TCanvas.h"
#include "TTree.h"
using std::vector;
// -----   Default constructor   -------------------------------------------
CbmAnaMuchBgd::CbmAnaMuchBgd() : FairTask("AnaDimuonAnalysis"),
  fEvent(0),
  fMCTracks(0x0),
  fStsTracks(0x0),
  fMuchTracks(0x0),
  fGlobalTracks(0x0),
  fVertex(0x0),
  fFitter(0x0),
  fM(0x0),
  fMuons(),
  fNAll(0),
  fNSts(0),
  fNStsChi(0),
  fNStsChiMuch13(0),
  fNStsChiMuch15(0),
  fNoMCTracks(0),
  fNoStsTracks(0),
  fNoGlobalTracks(0),
  fNoMuchTracks(0),
  fMuonTree(),
  fMuon()
{
}
// -------------------------------------------------------------------------


// -----  SetParContainers -------------------------------------------------
void CbmAnaMuchBgd::SetParContainers(){
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
InitStatus CbmAnaMuchBgd::Init(){
  FairRootManager* fManager = FairRootManager::Instance();
  fMCTracks         = (TClonesArray*) fManager->GetObject("MCTrack");
  fStsTracks        = (TClonesArray*) fManager->GetObject("StsTrack");
  fMuchTracks       = (TClonesArray*) fManager->GetObject("MuchTrack");
  fGlobalTracks     = (TClonesArray*) fManager->GetObject("GlobalTrack");
  fVertex           = (CbmVertex*)    fManager->GetObject("PrimaryVertex");
  fEvent=0;
  
  fM = new TH1D("fM",";M_{#mu#mu}(GeV/c);Entries",100,0,4);
  fFitter = new CbmStsKFTrackFitter();
  fFitter->Init();
  
  
  fMuonTree = new TTree("muons","muons");
  fMuonTree->Branch("chunk",&fMuon.fChunk);
  fMuonTree->Branch("event",&fMuon.fEvent);
  fMuonTree->Branch("nmuons",&fMuon.fMuons);
  fMuonTree->Branch("sign",&fMuon.fSign);
  fMuonTree->Branch("px",&(fMuon.fP[0]));
  fMuonTree->Branch("py",&(fMuon.fP[1]));
  fMuonTree->Branch("pz",&(fMuon.fP[2]));
  fMuonTree->Branch("muchHits",&fMuon.fMuchHits);
  fMuonTree->Branch("stsHits",&fMuon.fStsHits);
  fMuonTree->Branch("chi",&fMuon.fChi);
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaMuchBgd::Exec(Option_t* opt){
  fMuon.fEvent = fEvent;
  fMuon.fChunk = 0;
  fMuon.fMuons = 0;
  printf("Event: %4i\n",fEvent++);
  fNoMCTracks     += fMCTracks->GetEntriesFast();
  fNoStsTracks    += fStsTracks->GetEntriesFast();
  fNoGlobalTracks += fGlobalTracks->GetEntriesFast();
  fNoMuchTracks   += fMuchTracks->GetEntriesFast();
  
  for (Int_t iTrack=0;iTrack<fGlobalTracks->GetEntriesFast();iTrack++){
    CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) fGlobalTracks->At(iTrack);
    Int_t iMuchTrack = globalTrack->GetMuchTrackIndex();
    Int_t iStsTrack  = globalTrack->GetStsTrackIndex();
    if (iMuchTrack<0 || iStsTrack<0) continue;
    CbmMuchTrack*  muchTrack = (CbmMuchTrack*)  fMuchTracks->At(iMuchTrack);
    CbmStsTrack*   stsTrack = (CbmStsTrack*) fStsTracks->At(iStsTrack);
    fMuon.fMuchHits = muchTrack->GetNofHits();
    fMuon.fStsHits  = stsTrack->GetNofHits();
    fMuon.fChi      = fFitter->GetChiToVertex(stsTrack);
    fNAll++;
    if (fMuon.fStsHits <4)  continue;
    fNSts++;
    if (fMuon.fChi>3)       continue;
    fNStsChi++;
    if (fMuon.fMuchHits<13) continue;
    fNStsChiMuch13++;
    if (fMuon.fMuchHits<15) continue;
    fNStsChiMuch15++;
    FairTrackParam par;
    fFitter->Extrapolate(stsTrack,fVertex->GetZ(),&par);
    TVector3 p;
    par.Momentum(p);
    fMuon.fP.SetVectM(p,0.105658);
    fMuon.fSign = par.GetQp()>0 ? 1 : -1;

    fMuons.push_back(fMuon);
    fMuonTree->Fill();
  }
  
  
  for (Int_t i=0;i<fMuons.size();i++){
    CbmMuon mp = fMuons[i];
    if (mp.fSign>0) continue;
    for (Int_t j=0;j<fMuons.size();j++){
      CbmMuon mn = fMuons[j];
      if (mn.fSign<0) continue;
      Double_t m = (mp.fP+mn.fP).M();
      fM->Fill(m);
    }
  }
  fMuons.clear();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaMuchBgd::Finish(){
  fM->Scale(1./fEvent);
  fM->Write();
  printf("%i\n",fNAll);
  printf("%i\n",fNSts);
  printf("%i\n",fNStsChi);
  printf("%i\n",fNStsChiMuch13);
  printf("%i\n",fNStsChiMuch15);
  printf("%6.1f\n",Double_t(fNoMCTracks)/fEvent);
  printf("%6.1f\n",Double_t(fNoStsTracks)/fEvent);
  printf("%6.1f\n",Double_t(fNoGlobalTracks)/fEvent);
  printf("%6.1f\n",Double_t(fNoMuchTracks)/fEvent);

  TCanvas* c1 = new TCanvas("c1","c1",1000,600);
  fM->Draw();
  fMuonTree->Write();
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaMuchBgd);
