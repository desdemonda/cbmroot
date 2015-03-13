// -------------------------------------------------------------------------
// -----               CbmAnaHypRecLambda source file                  -----
// -----               Created 27/06/06  by E. Kryshen                 -----
// -------------------------------------------------------------------------
// Updated 2006-12-22
#include "CbmAnaHypRecLambda.h"

#include <vector>

#include "TClonesArray.h"
#include "TVector3.h"

#include "FairRootManager.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmVertex.h"
#include "CbmKF.h"
#include "CbmKFSecondaryVertexFinder.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmKFVertex.h"
#include "CbmAnaHypTools.h"


#include "CbmAnaHypPair.h"
#include "CbmKFFieldMath.h"

// -------------------------------------------------------------------------
CbmAnaHypRecLambda::CbmAnaHypRecLambda()
  : FairTask("AnaHypRecLambda"),
    fPid(0),
    fCheckChi(1),
    fEvent(0),
    fMCTracks(NULL),
    fRCTracks(NULL),
    fRCMatch(NULL),
    fPV(NULL),
    fListV0Candidates(new TClonesArray("CbmAnaHypPair",10)),
    fCutChi1(4.4),
    fCutChi2(3.6),
    fKF(NULL),
    fTools(NULL),
    fSVF(NULL),
    fFitter(NULL)
{
}
CbmAnaHypRecLambda::CbmAnaHypRecLambda(const char * name, Int_t iVerbose)
  : FairTask(name,iVerbose),
    fPid(0),
    fCheckChi(1),
    fEvent(0),
    fMCTracks(NULL),
    fRCTracks(NULL),
    fRCMatch(NULL),
    fPV(NULL),
    fListV0Candidates(new TClonesArray("CbmAnaHypPair",10)),
    fCutChi1(4.4),
    fCutChi2(3.6),
    fKF(NULL),
    fTools(NULL),
    fSVF(NULL),
    fFitter(NULL)
{
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
InitStatus CbmAnaHypRecLambda::Init(){
  fEvent = 0;

  FairRootManager* fManager = FairRootManager::Instance();
  fMCTracks  = (TClonesArray*) fManager->GetObject("MCTrack");
  fRCTracks  = (TClonesArray*) fManager->GetObject("StsTrack");
  fRCMatch   = (TClonesArray*) fManager->GetObject("StsTrackMatch");
  fPV        = (CbmVertex*)    fManager->GetObject("PrimaryVertex");

  fManager->Register("V0Candidates","Sts",fListV0Candidates,kTRUE);

  fKF= CbmKF::Instance();
  fTools = new CbmAnaHypTools();
  fTools->Init();
  fSVF = new CbmKFSecondaryVertexFinder();
  fFitter = new CbmStsKFTrackFitter();
  fFitter->Init();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmAnaHypRecLambda::Exec(Option_t* option){
  printf("Event:%i\n",fEvent++);
  fListV0Candidates->Clear();
  Int_t iV0Candidates=0;

  vector<CbmAnaHypTrack> vP;
  vector<CbmAnaHypTrack> vN;
  vP.clear();
  vN.clear();

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
    if (tr->GetNofHits()<4) continue;

// TODO Refit
//    if (sign>0) {
//      tr->SetPidHypo(2212);
//      fFitter->DoFit(tr);
//    }

    Double_t chi = fFitter->GetChiToVertex(tr);
    if (fCheckChi && sign>0 && chi<fCutChi1) continue;
    if (fCheckChi && sign<0 && chi<fCutChi2) continue;

    Int_t mcId=-1,mcPdg=0,moId=-1,moPdg=0;
    mcId  = GetMCTrack(i);
    if (mcId>0){
      CbmMCTrack* mc = (CbmMCTrack*) fMCTracks->At(mcId);
      mcPdg = mc->GetPdgCode();
      moId  = mc->GetMotherId();
      moPdg = (moId<0) ? 0 :((CbmMCTrack*) fMCTracks->At(moId))->GetPdgCode();
    }
    CbmAnaHypTrack track(*tr,i,mcId,moId,mcPdg,moPdg,chi,0);
    //printf("trackPID:%5i\n",GetPid(track));
    track.Extrapolate(pv[2]);
    Double_t* T=track.GetTrack();
    track.fB = sqrt(pow(T[0]-pv[0],2)+pow(T[1]-pv[1],2));
    if (sign>0) {
      if (fPid && GetPid(track)!=2212) continue;
      track.SetPID(2212);
      vP.push_back(track);
    } else {
      if (fPid && GetPid(track)!=-211) continue;
      vN.push_back(track);
    }
  }

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
      Double_t dca = CalculateDca(tp,tn,pca,alpha,ptt,mK0,mLa);

      CbmKFFieldMath::ExtrapolateLine(T,C,T,C,pv[2]);
      Double_t bmo = sqrt(pow(T[0]-pv[0],2)+pow(T[1]-pv[1],2));

      if (fVerbose>1 || sig>=0) {
        if (sig>=0) printf("+"); else printf(" ");
        printf("  (%4i,%4i) m=%7.4f",i,j,m);
        printf("  bp=%7.3f bn=%7.3f",bp*10,bn*10);
        printf("  chi1=%6.2f chi2=%7.3f",chi1,chi2);
        printf("  chi=%6.2f dca=%7.3f",chi,dca);
        printf("  pca=%6.2f bmo=%7.3f",pca,bmo);
        printf("  mK0=%7.4f mLa=%7.4f",mK0,mLa);
        printf(" alpha=%6.3f ptt=%6.3f",alpha,ptt);
        printf("\n");
      }

      new((*fListV0Candidates)[iV0Candidates++])
          CbmAnaHypPair(tp.fRcId,tn.fRcId,tp.fMcId,tn.fMcId,
                         bp,bn,chi1,chi2,chi,dca,pca,bmo,
                         m,mK0,alpha,ptt,sig);
    }
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Int_t CbmAnaHypRecLambda::GetMCTrack(Int_t iRC){
    Double_t quota=0.7;

    CbmTrackMatch* match = (CbmTrackMatch*) fRCMatch->At(iRC);
    Int_t mcId = match->GetMCTrackId();
    if(  mcId<0 || quota > (Double_t) match->GetNofTrueHits()
         /(match->GetNofTrueHits()
         + match->GetNofWrongHits()
         + match->GetNofFakeHits())
      ) return -1;
    return mcId;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypRecLambda::Extrapolate(Double_t z,
                              CbmKFTrack &tr, TVector3 &v, TVector3 &p){
  tr.Extrapolate(z);
  Double_t *T=tr.GetTrack();
  Double_t pz = 1./sqrt(T[4]*T[4]*(T[2]*T[2]+T[3]*T[3]+1));
  p =TVector3(T[2]*pz,T[3]*pz,pz);
  v =TVector3(T[0],T[1],z);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t CbmAnaHypRecLambda::CalculateDca
  (CbmKFTrack tp, CbmKFTrack tn, Double_t z,
   Double_t &alpha, Double_t &ptt, Double_t &mK0, Double_t &mLa){
  TVector3 v1,v2,p1,p2;
  Extrapolate(z,tp,v1,p1);
  Extrapolate(z,tn,v2,p2);
  GetAP(p1,p2,alpha,ptt);
  Double_t m = 0.13957;
  Double_t M = 0.938272;
  TLorentzVector P1,P2;
  P1.SetVectM(p1,m);
  P2.SetVectM(p2,m);
  mK0=(P1+P2).M();
  P1.SetVectM(p1,M);
  mLa=(P1+P2).M();
  return (v2-v1).Perp();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypRecLambda::GetAP(TVector3 p1, TVector3 p2,
                                Double_t &alpha, Double_t &ptt){

  alpha = ptt = 0.;
  TVector3 s = p1 + p2;
  Double_t sp  = s.Mag();

  if(sp == 0.0) return;

  Double_t pn, pp, pln, plp;
  pp = p1.Mag();
  pn = p2.Mag();
  plp  = (s*p1)/sp;
  pln  = (s*p2)/sp;

  if(pn == 0.0) return;
  Double_t ptm  = (1.-((pln/pn)*(pln/pn)));
  ptt= (ptm>=0.) ?  pn*sqrt(ptm) :0;
  alpha = (plp-pln)/(plp+pln);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Int_t CbmAnaHypRecLambda::GetPid(CbmAnaHypTrack tr){
  if (tr.fMcId<0) return -1;
  CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(tr.fMcId);
  if (mcTrack->GetNPoints(kTOF)<=0) return 0;
  return mcTrack->GetPdgCode();
}
// -------------------------------------------------------------------------
