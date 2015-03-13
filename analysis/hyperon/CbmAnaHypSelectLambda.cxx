// -------------------------------------------------------------------------
// -----                 CbmAnaHypSelectLambda source file             -----
// -----                  Created 07/02/05  by E. Kryshen              -----
// -------------------------------------------------------------------------
// 25/06/06 Adopted to the framework
// 07/02/07 Modified

#include "CbmAnaHypSelectLambda.h"

#include "TClonesArray.h"
#include "TH2D.h"

#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "CbmStsTrack.h"
#include "CbmMCTrack.h"
#include "CbmVertex.h"

#include "CbmKF.h"
#include "CbmKFMath.h"
#include "CbmKFFieldMath.h"
#include "CbmKFSecondaryVertexFinder.h"
#include "CbmKFVertex.h"
#include "CbmStsKFTrackFitter.h"

#include "CbmAnaHypTools.h"
#include "CbmAnaHypLambdaCandidate.h"

// -----   Standard constructor   ------------------------------------------
CbmAnaHypSelectLambda::CbmAnaHypSelectLambda()
  : CbmAnaHypRecLambda("AnaHypSelectLambda", 0),
    fCutB1(0.14),
    fCutB2(0.65),
    fCutChi(4.6),
    fCutDca(0.13),
    fCutPca(3.0),
    fCutPcaMax(50.),
    fCutMmin(1.1157-2*0.0013),
    fCutMmax(1.1157+2*0.0013),
    fListRCLambda(new TClonesArray("CbmAnaHypLambdaCandidate",1))
{
  fCutChi1 = 4.8;
  fCutChi2 = 4.0;
  fPid = 0;
}
// -------------------------------------------------------------------------

CbmAnaHypSelectLambda::CbmAnaHypSelectLambda(const char* name, Int_t iVerbose)
  : CbmAnaHypRecLambda(name,iVerbose),
    fCutB1(0.14),
    fCutB2(0.65),
    fCutChi(4.6),
    fCutDca(0.13),
    fCutPca(3.0),
    fCutPcaMax(50.),
    fCutMmin(1.1157-2*0.0013),
    fCutMmax(1.1157+2*0.0013),
    fListRCLambda(new TClonesArray("CbmAnaHypLambdaCandidate",1))
{
  fCutChi1 = 4.8;
  fCutChi2 = 4.0;
  fPid = 0;
}
// -------------------------------------------------------------------------


// -----   Init   ----------------------------------------------------------
InitStatus CbmAnaHypSelectLambda::Init(){
  fEvent = 0;

  FairRootManager* fManager = FairRootManager::Instance();
  fMCTracks  = (TClonesArray*) fManager->GetObject("MCTrack");
  fRCTracks  = (TClonesArray*) fManager->GetObject("StsTrack");
  fRCMatch   = (TClonesArray*) fManager->GetObject("StsTrackMatch");
  fPV        = (CbmVertex*)    fManager->GetObject("PrimaryVertex");

  fManager->Register("V0Candidates","Sts",fListRCLambda,kTRUE);

  fKF= CbmKF::Instance();
  fTools = new CbmAnaHypTools();
  fTools->Init();
  fSVF = new CbmKFSecondaryVertexFinder();
  fFitter = new CbmStsKFTrackFitter();
  fFitter->Init();

  return kSUCCESS;

}
// -------------------------------------------------------------------------


// -----   Exec   ----------------------------------------------------------
void CbmAnaHypSelectLambda::Exec(Option_t * option){
  printf("Event:%3i",fEvent++);
  if (fVerbose>1) printf("\n");
  fListRCLambda->Clear();
  Int_t iLambda=0;

  vector<CbmAnaHypTrack> vP;
  vector<CbmAnaHypTrack> vN;

  CbmVertex vtx;
  TVector3 pv;
  TVector3 v;
  Double_t m,mK0,mLa;
  CbmKFTrack mo;
  Double_t alpha, ptt;

  fPV->Position(pv);
  CbmKFVertex kfPV(*fPV);
  CbmKFVertex kfVtx;

  for (Int_t i=0;i<fRCTracks->GetEntriesFast();i++){
    CbmStsTrack* tr = (CbmStsTrack*) fRCTracks->At(i);
    Int_t sign = (tr->GetParamFirst()->GetQp()>0) ? 1 : -1;
    if (fVerbose>1) printf("Track: %4i sign=%2i",i,sign);
    if (tr->GetNofHits()<4) {
      if (fVerbose>1) printf("\n");
      continue;
    }

//TODO Refit protons
//    if (sign>0) {
//        fFitter->DoFit(tr,2212);
//    }

    Double_t chi = fFitter->GetChiToVertex(tr);
    if (fVerbose>1) printf(" chi=%5.2f",chi);
    if ((sign>0 && chi<fCutChi1) ||
        (sign<0 && chi<fCutChi2)) {
      if (fVerbose>1) printf("\n");
      continue;
    }

    Int_t mcId=-1,mcPdg=0,moId=-1,moPdg=0;
    mcId  = GetMCTrack(i);
    if (mcId>0){
      CbmMCTrack* mc = (CbmMCTrack*) fMCTracks->At(mcId);
      mcPdg = mc->GetPdgCode();
      moId  = mc->GetMotherId();
      moPdg = (moId<0) ? 0 :((CbmMCTrack*) fMCTracks->At(moId))->GetPdgCode();
    }
    CbmAnaHypTrack track(*tr,i,mcId,moId,mcPdg,moPdg,chi,0);
    track.Extrapolate(pv[2]);
    Double_t* T=track.GetTrack();
    track.fB = sqrt(pow(T[0]-pv[0],2)+pow(T[1]-pv[1],2));

    // Imp. cut
    if (sign>0 && track.fB<fCutB1) continue;
    if (sign<0 && track.fB<fCutB2) continue;

    if (sign>0) {
      if (fPid && GetPid(track)!=2212) continue;
      track.SetPID(2212);
      vP.push_back(track);
    } else {
     if (fPid && GetPid(track)!=-211) continue;
     vN.push_back(track);
    }
    if (fVerbose>1) printf("\n");
  }

  if (fVerbose) printf(" pos.tracks: %2lu",vP.size());
  if (fVerbose) printf(" neg.tracks: %2lu",vN.size());

  for (unsigned i=0;i<vP.size();i++){
    CbmAnaHypTrack &tp = vP[i];
    for (unsigned j=0;j<vN.size();j++){
      CbmAnaHypTrack &tn = vN[j];
      fSVF->Clear();
      fSVF->AddTrack(&tp);
      fSVF->AddTrack(&tn);
      fSVF->SetApproximation(&kfPV);
      fSVF->Fit();
      fSVF->GetVertex(vtx);
      fSVF->GetVertex(kfVtx);
      fSVF->SetApproximation(&kfVtx);
      fSVF->Fit();
      fSVF->GetVertex(vtx);
      fSVF->GetMass(&m,0);

      if (!(m>fCutMmin && m<fCutMmax)) continue;

      Double_t T[6],C[15];
      fSVF->GetMotherTrack(T,C);
      vtx.Position(v);

      Int_t sig = (tp.fMoId==tn.fMoId && tp.fMoPdg==3122) ? tp.fMoId : -1;
      Double_t bp  = tp.fB;
      Double_t bn  = tn.fB;
      Double_t chi1 = tp.fChi;
      Double_t chi2 = tn.fChi;
      Double_t chi = vtx.GetChi2()/vtx.GetNDF();
      Double_t pca = v[2];

      if (!(pca>fCutPca && pca<fCutPcaMax)) continue;
      Double_t dca = CalculateDca(tp,tn,pca,alpha,ptt,mK0,mLa);
      if (dca>fCutDca) continue;

      CbmKFFieldMath::ExtrapolateLine(T,C,T,C,pv[2]);
      Double_t bmo = sqrt(pow(T[0]-pv[0],2)+pow(T[1]-pv[1],2));

      CbmStsTrack mother;
      FairTrackParam parFirst, parLast;
      CbmKFMath::CopyTC2TrackParam(&parFirst,T,C);
      CbmKFMath::CopyTC2TrackParam(&parLast,T,C);
      mother.SetParamFirst(&parFirst);
      mother.SetParamLast(&parLast);
      mother.SetPidHypo(3122);
      mother.SetFlag(sig);
      mother.SetChiSq(0);
      mother.SetNDF(1);
      mother.SetB(bmo);

      CbmAnaHypPair pair(tp.fRcId,tn.fRcId,tp.fMcId,tn.fMcId,
                         bp,bn,chi1,chi2,chi,dca,pca,bmo,
                         m,mK0,alpha,ptt,sig);

      new((*fListRCLambda)[iLambda++]) CbmAnaHypLambdaCandidate(pair,mother);
    }
  }
  if (fVerbose) printf("  Selected:%2i",iLambda);
  printf("\n");
  vP.clear();
  vN.clear();
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaHypSelectLambda)
