#include "CbmAnaHypTools.h"

#include "TClonesArray.h"
#include "TTree.h"
#include "TMath.h"

#include "FairRootManager.h"
#include "CbmHit.h"
#include "CbmStsPoint.h"
#include "CbmStsTrack.h"
#include "CbmKFTrack.h"
#include "CbmVertex.h"
#include "CbmKFFieldMath.h"
#include "CbmKFParticle.h"
#include "CbmTrackMatch.h"

void CbmAnaHypTools::Init(){
  FairRootManager* ioman = FairRootManager::Instance();
  fListMCTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
  fListRCTracks  = (TClonesArray*) ioman->GetObject("StsTrack");
  fStsPointArray = (TClonesArray*) ioman->GetObject("StsPoint");
  fmArray        = (TClonesArray*) ioman->GetObject("MvdHit");
  fsArray        = (TClonesArray*) ioman->GetObject("StsHit");
  fPV            = (CbmVertex*)    ioman->GetObject("PrimaryVertex");
  fTrackMatch    = (TClonesArray*) ioman->GetObject("StsTrackMatch");

  fPV->Position(pv);
}

Int_t CbmAnaHypTools::GetTrackId(CbmTrackMatch* match){
  Int_t trackId = match->GetMCTrackId();
  if (trackId<=0) return -1;
  Int_t nTrue  = match->GetNofTrueHits();
  Int_t nWrong = match->GetNofWrongHits();
  Int_t nFake  = match->GetNofFakeHits();
  Double_t true_ratio = nTrue/((Double_t) (nTrue+nWrong+nFake));
  Double_t quota = 0.7;
  if (true_ratio<quota) return -2;
  return trackId;
}


Int_t CbmAnaHypTools::GetTrackId(CbmHit* hit){
  Int_t index = hit->GetRefIndex();
  if (index<0) return -1;
  if (index>=fStsPointArray->GetEntriesFast()) {
     printf("CbmAnaHypTools::GetTrackId WARNING!!!!\n");
     return -1;
  }

  CbmStsPoint* point = (CbmStsPoint*) fStsPointArray->At(index);
  return point->GetTrackID();
}

void CbmAnaHypTools::SetFlags(){
  for (Int_t i=0;i<fListRCTracks->GetEntriesFast();i++){
    CbmStsTrack* track = (CbmStsTrack*) fListRCTracks->At(i);
    CbmTrackMatch* match = (CbmTrackMatch*) fTrackMatch->At(i);
    CbmKFTrack kf(*track);
    kf.Extrapolate(pv[2]);
    Double_t* T=kf.GetTrack();
    Double_t b=sqrt(pow(T[0]-pv[0],2)+pow(T[1]-pv[1],2));
    track->SetB(b);
    Int_t sign = (int)(T[4]/TMath::Abs(T[4]));
    track->SetPidHypo(211*sign);
    Int_t flag = GetTrackId(match);
    track->SetFlag(flag);
  }
}

void CbmAnaHypTools::SetFlagsOnly(){
  for (Int_t i=0;i<fListRCTracks->GetEntriesFast();i++){
    CbmStsTrack* track = (CbmStsTrack*) fListRCTracks->At(i);
    CbmTrackMatch* match = (CbmTrackMatch*) fTrackMatch->At(i);
    Int_t flag = GetTrackId(match);
    track->SetFlag(flag);
  }
}


void CbmAnaHypTools::CopyTrack(CbmStsTrack* trIn, CbmStsTrack* trOut){
  trOut->SetParamFirst(trIn->GetParamFirst());
  trOut->SetParamLast(trIn->GetParamLast());
  trOut->SetFlag(trIn->GetFlag());
  trOut->SetChiSq(trIn->GetChiSq());
  trOut->SetPidHypo(trIn->GetPidHypo());
  trOut->SetB(trIn->GetB());
}


Double_t CbmAnaHypTools::CalculateDca(CbmStsTrack* tr1, CbmStsTrack* tr2, CbmVertex vtx){
  Double_t x1,y1,x2,y2;
  TVector3 v;

  CbmKFTrack kf1(*tr1);
  CbmKFTrack kf2(*tr2);

  vtx.Position(v);
  kf1.Extrapolate(v[2]);
  kf2.Extrapolate(v[2]);
  x1 = kf1.GetTrack()[0];
  y1 = kf1.GetTrack()[1];
  x2 = kf2.GetTrack()[0];
  y2 = kf2.GetTrack()[1];

  return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t CbmAnaHypTools::CalculateCascadeDca
 (CbmStsTrack* tr1, CbmStsTrack* tr2, CbmVertex vtx){
  Double_t x1,y1,x2,y2;
  TVector3 v;

  CbmKFTrack kf1(*tr1);
  CbmKFTrack kf2(*tr2);

  vtx.Position(v);
  Double_t* T = kf1.GetTrack();
  Double_t* C = kf1.GetCovMatrix();
  CbmKFFieldMath::ExtrapolateLine(T,C,T,C,v[2]);
  kf2.Extrapolate(v[2]);
  x1 = T[0];
  y1 = T[1];
  x2 = kf2.GetTrack()[0];
  y2 = kf2.GetTrack()[1];

  return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}
// -------------------------------------------------------------------------



// -----   SignalPair   ----------------------------------------------------
Int_t CbmAnaHypTools::SignalPair(CbmStsTrack* tr1, CbmStsTrack* tr2){
  Int_t id1MC=tr1->GetFlag();
  Int_t id2MC=tr2->GetFlag();
  if (id1MC<0 || id2MC<0) return -1;

  CbmMCTrack* tr1MC = (CbmMCTrack*) fListMCTracks->At(id1MC);
  CbmMCTrack* tr2MC = (CbmMCTrack*) fListMCTracks->At(id2MC);

  if (tr1MC->GetPdgCode()!=2212 || tr2MC->GetPdgCode()!=-211) return -1;
  if (tr1MC->GetMotherId()==-1) return -1;
  CbmMCTrack* mother = (CbmMCTrack*) fListMCTracks->At(tr1MC->GetMotherId());
  if (mother->GetPdgCode()!=3122) return -1;
  if (tr1MC->GetMotherId()!=tr2MC->GetMotherId()) return -1;
  return tr1MC->GetMotherId();
}
// -------------------------------------------------------------------------

// -----   SignalCascade   -------------------------------------------------
Int_t CbmAnaHypTools::SignalCascade(CbmStsTrack* la, CbmStsTrack* da,
                Int_t motherPdg, Int_t sisterPdg){
  Int_t i=la->GetFlag();
  Int_t j=da->GetFlag();
  if (i<0 || j<0) return -1;

  CbmMCTrack* laMC = (CbmMCTrack*) fListMCTracks->At(i);
  CbmMCTrack* daMC = (CbmMCTrack*) fListMCTracks->At(j);

  if (laMC->GetPdgCode()!=3122 || daMC->GetPdgCode()!=sisterPdg) return -1;
  if (laMC->GetMotherId()==-1) return -1;
  if (laMC->GetMotherId()!=daMC->GetMotherId()) return -1;
  CbmMCTrack* mother = (CbmMCTrack*) fListMCTracks->At(laMC->GetMotherId());
  if (mother->GetPdgCode()!=motherPdg) return -1;
  return laMC->GetMotherId();
}
// -------------------------------------------------------------------------


// ---- GetB  --------------------------------------------------------------
Double_t CbmAnaHypTools::GetB(CbmStsTrack track){
  CbmKFTrack kf(track);
  kf.Extrapolate(pv[2]);
  Double_t* T=kf.GetTrack();
  return sqrt(pow(T[0]-pv[0],2)+pow(T[1]-pv[1],2));
}

// ---- GetPidHypo  --------------------------------------------------------
Int_t CbmAnaHypTools::GetPidHypo(CbmStsTrack track){
  CbmKFTrack kf(track);
  Double_t* T=kf.GetTrack();
  Int_t sign = (int)(T[4]/TMath::Abs(T[4]));
  track.SetPidHypo(211*sign);
  return 211*sign;
}

// ---- Check L1 accepted --------------------------------------------------
Int_t CbmAnaHypTools::IsL1accepted(Int_t trackId){
// TODO Add MVD
//  vector<int> vM;
  vector<int> vS;
  vector<int> vStationID;

//  vM.clear();
  vS.clear();
  vStationID.clear();

//  Int_t nM = (fmArray) ? fmArray->GetEntriesFast() :0;
  Int_t nS = (fsArray) ? fsArray->GetEntriesFast() :0;

//  printf ("  nM:%i",nM);
//  printf ("  nS:%i",nS);
//  printf ("\n");
//  printf ("trackID!!!:%i\n",trackId);

//  for (Int_t i=0;i<nM;i++){
//    CbmHit* hit = (CbmHit*)fmArray->At(i);
//    //printf("trackId:%i\n",GetTrackId(hit));
//    if (GetTrackId(hit)!=trackId) continue;
//    vStationID.push_back(hit->GetDetectorID());
///*
//    printf("station: %i",hit->GetDetectorID());
//    printf("  id: %i",hit->GetRefIndex());
//    printf("  z: %8.4f",hit->GetZ());
//    printf("  x: %8.4f",hit->GetX());
//    printf("  y: %8.4f\n",hit->GetY());
//*/
//  }

  for (Int_t i=0;i<nS;i++){
    CbmHit* hit = (CbmHit*)fsArray->At(i);
    //printf("trackId:%i\n",GetTrackId(hit));
    if (GetTrackId(hit)!=trackId) continue;
    vStationID.push_back(hit->GetStationNr());

//    printf("station: %i",hit->GetStationNr());
//    printf("  id: %i",hit->GetRefIndex());
//    printf("  z: %8.4f",hit->GetZ());
//    printf("  x: %8.4f",hit->GetX());
//    printf("  y: %8.4f\n",hit->GetY());
  }
  //printf("\n");
  if (vStationID.size()<1)return 0;
  Int_t sta_count = 1;
  Int_t sta_count_max = 1;
  Int_t prev_stationID = vStationID[0];
  for (unsigned j=1; j<vStationID.size(); j++){
    Int_t stationID = vStationID[j];
    if (stationID-prev_stationID==1) sta_count++;
    else { if (sta_count>sta_count_max) sta_count_max=sta_count; sta_count = 1;}
    prev_stationID = stationID;
  }
  if (sta_count>sta_count_max) sta_count_max=sta_count;
  return sta_count_max;
}
