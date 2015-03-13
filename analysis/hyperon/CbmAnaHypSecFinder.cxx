// -------------------------------------------------------------------------
// -----                   CbmAnaHypSecFinder source file              -----
// -----                  Created 24/11/06  by E. Kryshen              -----
// -------------------------------------------------------------------------
#include "CbmAnaHypSecFinder.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "CbmStsTrack.h"
#include "CbmStsHit.h"
#include "CbmMvdHit.h"
#include "CbmKF.h"
#include "CbmKFTrack.h"
#include "CbmKFStsHit.h"
#include <algorithm>

ClassImp(CbmAnaHypSecFinder)

// -------------------------------------------------------------------------
CbmAnaHypSecFinder::CbmAnaHypSecFinder()
  : FairTask("AnaHypSecFinder"),
    fRCTracks(0),
    fStandAlone(kFALSE),
    fTxMax(NULL),
    fTyMax(NULL),
    fdTxMax(NULL),
    fdTyMax(NULL),
    fChi2Max(3.),
    fQpMax(0.2),
    fNsigma(3.),    
    mcTrackArray(NULL),
    rcTrackArray(NULL),
    mcPointArray(NULL),
    mHitArray(NULL),
    hHitArray(NULL),
    sHitArray(NULL),
    newTrackArray(NULL),
    vHits(),
    fSt_z(),
    fSt_dz(),
    fStR1_z(),
    fStR2_z(),
    fDx1(),
    fDy1(),
    fDx2(),
    fDy2(),
    fStL(0),
    fStF(0),
    fNstations(0),
    fFitter(new CbmStsKFTrackFitter()),
    h_used(),
    s_used(),
    vTrackHits()
{
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmAnaHypSecFinder::CbmAnaHypSecFinder(char* name, Int_t iVerbose)
  : FairTask(name,iVerbose),
    fRCTracks(0),
    fStandAlone(kFALSE),
    fTxMax(NULL),
    fTyMax(NULL),
    fdTxMax(NULL),
    fdTyMax(NULL),
    fChi2Max(3.),
    fQpMax(0.2),
    fNsigma(3.),    
    mcTrackArray(NULL),
    rcTrackArray(NULL),
    mcPointArray(NULL),
    mHitArray(NULL),
    hHitArray(NULL),
    sHitArray(NULL),
    newTrackArray(NULL),
    vHits(),
    fSt_z(),
    fSt_dz(),
    fStR1_z(),
    fStR2_z(),
    fDx1(),
    fDy1(),
    fDx2(),
    fDy2(),
    fStL(0),
    fStF(0),
    fNstations(0),
    fFitter(new CbmStsKFTrackFitter()),
    h_used(),
    s_used(),
    vTrackHits()
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmAnaHypSecFinder::~CbmAnaHypSecFinder() {}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
InitStatus CbmAnaHypSecFinder::Init() {
  FairRootManager* ioman = FairRootManager::Instance();
  mcTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  rcTrackArray = (TClonesArray*) ioman->GetObject("StsTrack");
  sHitArray    = (TClonesArray*) ioman->GetObject("StsHit");
  mcPointArray = (TClonesArray*) ioman->GetObject("StsPoint");

  if (fStandAlone) {
    newTrackArray = new TClonesArray("CbmStsTrack",100);
    ioman->Register("StsTrackSec", "STS", newTrackArray, kTRUE);
  }
  else {
    newTrackArray = (TClonesArray*) ioman->GetObject("StsTrack");
  }

  CbmKF& KF = *CbmKF::Instance();
  fNstations=KF.vStsMaterial.size();

//  printf("fNstations=%i\n",fNstations);

  for (Int_t st=0;st<fNstations;st++) {
    fSt_z[st] = KF.vStsMaterial[st].z;
//    printf("fSt_z[%i]=%f\n",st,fSt_z[st]);
    if (st>0) {
      fSt_dz [st-1] = fSt_z[st]-fSt_z[st-1];
      fStR1_z[st-1] = fSt_z[st]/fSt_z[st-1];
      fStR2_z[st-1] = fStR1_z[st-1]+1;
      fDx1[st-1]  = fTxMax [st-1]*fSt_dz[st-1];
      fDy1[st-1]  = fTyMax [st-1]*fSt_dz[st-1];
      if (st>1) {
        fDx2[st-2] = fdTxMax[st-2]*fSt_dz[st-1];
        fDy2[st-2] = fdTyMax[st-2]*fSt_dz[st-1];
      }
    }
  }

  fFitter->Init();

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypSecFinder::Exec(Option_t* option){
  ReadHits();
  DoFind();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypSecFinder::DoFind(){
  if (fStandAlone) newTrackArray->Clear();
  fRCTracks = newTrackArray->GetEntriesFast();


  Double_t xm_min, xm_max, xr_min, xr_max, x_a, x_b;
  Double_t ym_min, ym_max, yr_min, yr_max, y_a, y_b;
  Double_t chi2;
  Double_t* T;

  for (Int_t stl=0;stl<fNstations-3;stl++){

    for (unsigned il=0;il<vHits[stl].size();il++){
      Hit &hl = vHits[stl][il];
      if (hl.used) continue;

      //Calculate limits for middle hits
      xm_min = hl.x - fDx1[stl];
      xm_max = hl.x + fDx1[stl];
      ym_min = hl.y - fDy1[stl];
      ym_max = hl.y + fDy1[stl];
      x_a = fStR1_z[stl]*hl.x;
      y_a = fStR1_z[stl]*hl.y;

      Int_t stm=stl+1;

      vector<Hit>::iterator itm =
        find_if(vHits[stm].begin(),vHits[stm].end(),GreaterY(ym_min));

      for (vector<Hit>::iterator im=itm; im!=vHits[stm].end(); im++){
        if (hl.used) break;
        Hit &hm = *im;
        if (hm.used) continue;

        //Cut on the slope
        if ( hm.y > ym_max ) break;
        if ( hm.x < xm_min ) continue;
        if ( hm.x > xm_max ) continue;

        //Calculate limits for right hits
        x_b = hm.x*fStR2_z[stl]-x_a;
        y_b = hm.y*fStR2_z[stl]-y_a;
        xr_min = x_b - fDx2[stl];
        xr_max = x_b + fDx2[stl];
        yr_min = y_b - fDy2[stl];
        yr_max = y_b + fDy2[stl];

        Int_t str = stm+1;

        vector<Hit>::iterator itr =
        find_if(vHits[str].begin(),vHits[str].end(),GreaterY(yr_min));

        for (vector<Hit>::iterator ir=itr; ir!=vHits[str].end(); ir++){
          if (hl.used || hm.used) break;
          Hit &hr = *ir;
          if (hr.used) continue;

          //Cut on the slope
          if ( hr.y > yr_max ) break;
          if ( hr.x < xr_min ) continue;
          if ( hr.x > xr_max ) continue;

          CbmKFTrack track;
          T = track.GetTrack();
          for (Int_t i=0;i<5;i++) T[i]=0;
          track.fHits.clear();
          track.fHits.push_back(hl.GetKFHit());
          track.fHits.push_back(hm.GetKFHit());
          track.fHits.push_back(hr.GetKFHit());
          track.Fit(1);
          chi2 = track.GetRefChi2()/track.GetRefNDF();
          if (chi2>1000 || TMath::Abs(T[4])>fQpMax) continue;
          track.Fit(1);
          chi2 = track.GetRefChi2()/track.GetRefNDF();
          if (chi2>fChi2Max*3 || TMath::Abs(T[4])>fQpMax) continue;

          vTrackHits.clear();
          vTrackHits.push_back(&hl);
          vTrackHits.push_back(&hm);
          vTrackHits.push_back(&hr);

          if (AddNextStation(track,str+1)<4) continue;
          CreateStsTrack(track);
        }// third hit
      }// second hit
    }// first hit
  }//station

  printf("-I- CbmAnaHypSecFinder: %i tracks\n",fRCTracks);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Int_t CbmAnaHypSecFinder::AddNextStation(CbmKFTrack &track, Int_t st){
  Int_t nHits = track.fHits.size();
  if (st>=fNstations) return nHits;

  vector<Hit>::iterator iBest, it;
  Double_t* T = track.GetTrack();
  Double_t chi2;
  Double_t z=fSt_z[st];

  Double_t qp0=T[4];
  track.Propagate(z,qp0);
  Double_t x   = T[0];
  Double_t y   = T[1];
  Double_t dx =  fNsigma*sqrt(track.GetCovMatrix()[0]);
  Double_t dy =  fNsigma*sqrt(track.GetCovMatrix()[2]);
  Double_t y_min = y-dy;
  Double_t y_max = y+dy;
  Double_t x_min = x-dx;
  Double_t x_max = x+dx;



  iBest = vHits[st].end();
  Double_t s2Best=1000.;

  it = find_if(vHits[st].begin(),vHits[st].end(),GreaterY(y_min));

  for (vector<Hit>::iterator i = it; i!=vHits[st].end(); i++){
    Hit &h = *i;
    if ( h.y > y_max ) break;
    if ( h.x < x_min ) continue;
    if ( h.x > x_max ) continue;
    Double_t s2 = pow(h.x-x,2)+pow(h.y-y,2);
    if (s2<s2Best) {s2Best=s2;iBest=i;}
  }

  if (iBest==vHits[st].end()) {
//    if (CbmKF::Instance()->vStsMaterial[st].IsOutside(x,y)
//        && nHits==4) return -1;
    return nHits;
  }
  Hit &h = *iBest;
  track.fHits.push_back(h.GetKFHit());
  track.Fit(0);
  track.Fit(1);
  chi2 = track.GetRefChi2()/track.GetRefNDF();
  if (!(chi2>0 && chi2<fChi2Max && TMath::Abs(T[4])<fQpMax)) return nHits;
  vTrackHits.push_back(&h);
  return AddNextStation(track,st+1);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypSecFinder::ReadHits(){
  CbmKF &KF = *CbmKF::Instance();
  vector<Hit>::iterator hitIt;
  for(Int_t st=0;st<fNstations;st++) vHits[st].clear();

  Int_t nShits = (sHitArray) ? sHitArray->GetEntriesFast() : 0;
  Int_t nRCtracks = (rcTrackArray) ? rcTrackArray->GetEntriesFast() : 0;

  for (Int_t i=0;i<nShits;i++) s_used[i]=false;

  for (Int_t tr=0;tr<nRCtracks;tr++){
    CbmStsTrack* track = (CbmStsTrack*) rcTrackArray->At(tr);
    for (Int_t h=0;h<track->GetNStsHits();h++) {
      Int_t id = track->GetStsHitIndex(h);
      s_used[id]=true;
      CbmHit* hit = (CbmHit*) sHitArray->At(id);
      for (Int_t i=0;i<nShits;i++) {
        if (SharedStrips(hit,(CbmHit*) sHitArray->At(i)))
          s_used[i]=true;
      }
    }
  }

  for (Int_t i=0;i<nShits;i++) {
    CbmStsHit* hit = (CbmStsHit*) sHitArray->At(i);
    if (s_used[i]) continue;
//    Int_t st = KF.StsStationIDMap[hit->GetDetectorID()];
    Int_t st = hit->GetStationNr()-1;
//    printf("st=%i\n",st);
    vHits[st].push_back(Hit(i,hit,-1,1));
  }

  // Sort hits in y
  for(Int_t st=0;st<fNstations;st++) {
     sort(vHits[st].begin(),vHits[st].end(),CompareY);
//     printf("station=%i nHits=%i\n",st,vHits[st].size());
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Int_t CbmAnaHypSecFinder::CreateStsTrack(CbmKFTrack track){
  CbmStsTrack stsTrack;
  stsTrack.SetPidHypo(track.GetTrack()[4]>=0 ? 211 : -211);
  for (unsigned i=0;i<vTrackHits.size();i++){
    Hit h = *vTrackHits[i];
    if      (h.type==0) stsTrack.AddMvdHit (h.id,(CbmMvdHit*) h.pt);
    else if (h.type==1) stsTrack.AddStsHit (h.id,(CbmStsHit*) h.pt);
  }
  stsTrack.SortHits();
  fFitter->DoFit(&stsTrack);
  if (stsTrack.GetChi2()/stsTrack.GetNDF()>fChi2Max) return 0;
  printf("Track added\n");
  new((*newTrackArray)[fRCTracks++]) CbmStsTrack(stsTrack);
  for (unsigned i=0;i<vTrackHits.size();i++)  MarkUsed(*vTrackHits[i]);
  return 1;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Bool_t CbmKFTrackSec::Sig(){
    if (hits.size()<2) return false;
    bool s = true;
    Int_t tr = hits[0]->tr;
    for (unsigned i = 0; i<hits.size(); i++){
      if (hits[i]->tr==-1) return false;
      if (hits[i]->tr!=tr) return false;
    }
    return s;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Bool_t CbmAnaHypSecFinder::SharedStrips (CbmHit* hit1, CbmHit* hit2) {
  CbmStsHit* h1 = (CbmStsHit*) hit1;
  CbmStsHit* h2 = (CbmStsHit*) hit2;
  if ( h1->GetDetectorID() != h2->GetDetectorID() ) return false;
  if ( h1->GetDigi(0) == h2->GetDigi(0) ) return true;
  if ( h1->GetDigi(1) == h2->GetDigi(1) ) return true;
  return false;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmAnaHypSecFinder::MarkUsed(Hit &h){
  h.used=true;
  if (h.type!=2) return;

  for (unsigned i=0;i<vHits[h.st].size();i++){
    Hit &h1 = vHits[h.st][i];
    if (SharedStrips(h.pt,h1.pt)) h1.used=true;
  }
}
// -------------------------------------------------------------------------
