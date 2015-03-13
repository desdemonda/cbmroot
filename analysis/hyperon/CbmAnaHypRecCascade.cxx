// -------------------------------------------------------------------------
// -----               CbmAnaHypRecCascade source file                 -----
// -----               Created 27/06/06  by E. Kryshen                 -----
// -------------------------------------------------------------------------
// Updated 19/01/07
#include "CbmAnaHypRecCascade.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include "FairRootManager.h"

#include "CbmAnaHypLambdaCandidate.h"
#include "CbmMCTrack.h"
#include "CbmVertex.h"
#include "CbmStsTrack.h"
#include "CbmStsKFTrackFitter.h"

#include "CbmKF.h"
#include "CbmKFTrack.h"
#include "CbmKFFieldMath.h"
#include "CbmKFSecondaryVertexFinder.h"
#include "CbmKFMath.h"

// -------------------------------------------------------------------------
CbmAnaHypRecCascade::CbmAnaHypRecCascade()
  : CbmAnaHypRecLambda("AnaHypRecCascade", 0),
    fTree(NULL),
    fListRCLambda(NULL),
    fListCascades(new TClonesArray("CbmAnaHypPair")),
    fCascades(0),
    fMotherPdg(3334),
    fSisterPdg(-321),
    fCutChiDa(4.4),
    fCutBla(0.1),
    fPid(0),
    fCheckB(1),
    vDa(),
    kfPV(),
    pv()
{
}
// -------------------------------------------------------------------------
CbmAnaHypRecCascade::CbmAnaHypRecCascade(const char * name, Int_t iVerbose)
  : CbmAnaHypRecLambda(name,iVerbose),
    fTree(NULL),
    fListRCLambda(NULL),
    fListCascades(new TClonesArray("CbmAnaHypPair")),
    fCascades(0),
    fMotherPdg(3334),
    fSisterPdg(-321),
    fCutChiDa(4.4),
    fCutBla(0.1),
    fPid(0),
    fCheckB(1),
    vDa(),
    kfPV(),
    pv()
{
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
InitStatus CbmAnaHypRecCascade::Init(){
  fEvent = 0;

  FairRootManager* fManager =FairRootManager::Instance();
  fMCTracks  = (TClonesArray*) fManager->GetObject("MCTrack");
  fRCTracks  = (TClonesArray*) fManager->GetObject("StsTrack");
  fRCMatch   = (TClonesArray*) fManager->GetObject("StsTrackMatch");
  fPV        = (CbmVertex*)    fManager->GetObject("PrimaryVertex");
  fListRCLambda  = (TClonesArray*) fManager->GetObject("V0Candidates");
  fManager->Register("CascadeCandidates","Sts",fListCascades,kTRUE);
  fTree=fManager->GetInTree();

  fKF= CbmKF::Instance();
  fSVF = new CbmKFSecondaryVertexFinder();
  fFitter = new CbmStsKFTrackFitter();
  fFitter->Init();

  printf("===========\n");
  printf("CheckChi=%i\n",fCheckChi);
  printf("CheckB  =%i\n",fCheckB);
  printf("===========\n");
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypRecCascade::Exec(Option_t* option){
  printf("Event:%3i",fEvent);
  fListCascades->Clear();
  fCascades=0;

  CreateDaVector();
  printf("  nLambda=%2i",fListRCLambda->GetEntriesFast());
  printf("  nDa=%3lu",vDa.size());
  printf("\n");

  for (unsigned j=0;j<vDa.size();j++){
    CbmAnaHypTrack &da = vDa[j];
    // Check if second daughter was already used in the creation of LambdaCandidate
    Bool_t isUsed=0;
    for (Int_t i=0;i<fListRCLambda->GetEntriesFast();i++){
      CbmAnaHypLambdaCandidate* v0 = (CbmAnaHypLambdaCandidate*) fListRCLambda->At(i);
      if (v0->fId2==da.fRcId) { isUsed=1; break; }
    }
    if (isUsed) continue;

    // Create Cascade pair
    for (Int_t i=0;i<fListRCLambda->GetEntriesFast();i++){
      CbmAnaHypLambdaCandidate* v0 = (CbmAnaHypLambdaCandidate*) fListRCLambda->At(i);
      CreatePair(da,*v0,i);
    }
  }
  fEvent++;
}
// -------------------------------------------------------------------------

void CbmAnaHypRecCascade::CreateDaVector(){
  kfPV = CbmKFVertex(*fPV);
  fPV->Position(pv);

  vDa.clear();
  for (Int_t i=0;i<fRCTracks->GetEntriesFast();i++){
    CbmStsTrack* tr = (CbmStsTrack*) fRCTracks->At(i);
    if (tr->GetNofHits()<4) continue;
    Int_t sign = (tr->GetParamFirst()->GetQp()>0) ? 1 : -1;
    if (sign>0) continue;
    tr->SetPidHypo(fSisterPdg);
    Double_t chi = fFitter->GetChiToVertex(tr);
    if (fCheckChi && chi<fCutChiDa) continue;

    Int_t mcId=-1,mcPdg=0,moId=-1,moPdg=0;
    mcId  = GetMCTrack(i);
    if (mcId>0){
      CbmMCTrack* mc = (CbmMCTrack*) fMCTracks->At(mcId);
      mcPdg = mc->GetPdgCode();
      moId  = mc->GetMotherId();
      moPdg = (moId<0) ? 0 :((CbmMCTrack*) fMCTracks->At(moId))->GetPdgCode();
    }
    CbmAnaHypTrack track(*tr,i,mcId,moId,mcPdg,moPdg,chi,0);
    if (fPid && fSisterPdg==-321 && GetPid(track)==-211) continue;

    track.Extrapolate(pv[2]);
    Double_t* T=track.GetTrack();
    track.fB = sqrt(pow(T[0]-pv[0],2)+pow(T[1]-pv[1],2));
    vDa.push_back(track);
  }
}


void CbmAnaHypRecCascade::CreatePair(CbmAnaHypTrack da, CbmAnaHypLambdaCandidate v0, Int_t iLa) {
  CbmKFVertex kfVtx;
  CbmVertex vtx;
  TVector3 v;
  Double_t m;

  Double_t bla = v0.fBmo;
  if (fCheckB && bla<fCutBla) return;
  CbmKFTrack la(v0.track);

  //===
  // Check lambda mother id
  Int_t laMoId=-1;
  if (v0.fSig>=0) {
    CbmMCTrack* moMC = (CbmMCTrack*) fMCTracks->At(v0.fSig);
    laMoId=moMC->GetMotherId();
  }
  //===
//  printf("Lambda q/p=%f\n",la.GetTrack()[4]);
  fSVF->Clear();
  fSVF->AddTrack(&la);
  fSVF->AddTrack(&da);
  fSVF->SetApproximation(&kfPV);
  fSVF->Fit(); //TODO fSVF->Fit(1);
  fSVF->GetVertex(vtx);
  fSVF->GetVertex(kfVtx);
  fSVF->SetApproximation(&kfVtx);
  fSVF->Fit(); //TODO fSVF->Fit(1); 
  fSVF->GetVertex(vtx);
  fSVF->GetMass(&m,0);

  vtx.Position(v);

  Int_t sig = (iLa>=0 && laMoId==da.fMoId && da.fMoPdg==fMotherPdg) ? da.fMoId : -1;
  Double_t bda  = da.fB;
  Double_t chi1 = 0;
  Double_t chi2 = da.fChi;
  Double_t chi = vtx.GetChi2()/vtx.GetNDF();
  Double_t pca = v[2];

  TVector3 v1,v2,p2;
  Double_t *t, *c;
  t=la.GetTrack();
  c=la.GetCovMatrix();
  CbmKFFieldMath::ExtrapolateLine(t,c,t,c,pca);
  v1 =TVector3(t[0],t[1],pca);
  Extrapolate(pca,da,v2,p2);
  Double_t dca=(v2-v1).Perp();


  // Get impact parameter
  Double_t T[6],C[15];
  fSVF->GetMotherTrack(T,C);
  T[4]=-T[4]; // the track in SVF is created with positive sign.
  CbmStsTrack mother;
  FairTrackParam par;
  CbmKFMath::CopyTC2TrackParam(&par,T,C);
  mother.SetParamFirst(&par);
  CbmKFTrack mo(mother);
  mo.SetPID(fMotherPdg);
  mo.Extrapolate(pv[2]);
  Double_t *TT = mo.GetTrack();
  Double_t bmo = sqrt(pow(TT[0]-pv[0],2)+pow(TT[1]-pv[1],2));
  Double_t alpha=0;
  Double_t ptt=0;

  new((*fListCascades)[fCascades++])
      CbmAnaHypPair(iLa,da.fRcId,v0.fSig,da.fMcId,
                    bla,bda,chi1,chi2,chi,dca,pca,bmo,
                    m,0,alpha,ptt,sig);
}
