#ifndef CBMANAHYPSECFINDER_H
#define CBMANAHYPSECFINDER_H

#include "CbmStsTrackFinder.h"
#include "CbmStsKFTrackFitter.h"
#include <map>
#include <vector>
#include "CbmHit.h"
#include "FairTask.h"
#include "CbmKFHit.h"
#include "CbmKFTrack.h"
#include "CbmStsTrack.h"
#include "CbmKF.h"
#include "CbmAnaHypTools.h"
#include "TMath.h"
#include "CbmStsHit.h"

using std::map;
using std::vector;

class TClonesArray;
class Hit;
class CbmKFTrackSec;

class CbmAnaHypSecFinder : public FairTask
{
 public:
  CbmAnaHypSecFinder();
  CbmAnaHypSecFinder(char* name, Int_t iVerbose);
  virtual ~CbmAnaHypSecFinder();
  virtual InitStatus Init();
  virtual void Exec(Option_t* option);

  void SetStandAlone(Bool_t alone)   {fStandAlone = alone;}
  void SetTxMax(Double_t* txMax)     {fTxMax = txMax;     }
  void SetTyMax(Double_t* tyMax)     {fTyMax = tyMax;     }
  void SetDTxMax(Double_t* dTxMax)   {fdTxMax = dTxMax;   }
  void SetDTyMax(Double_t* dTyMax)   {fdTyMax = dTyMax;   }
  void SetChi2Max(Double_t chi2Max)  {fChi2Max = chi2Max; }
  void SetPMin(Double_t pMin)        {fQpMax  = 1./pMin;  }
  void SetNsigma(Double_t nSigma)    {fNsigma = nSigma;   }
 protected:
  void ReadHits();
  void DoFind();
  Int_t AddNextStation(CbmKFTrack &track, Int_t st);
  Int_t CreateStsTrack(CbmKFTrack track);
  void PrintHitInfo();
  Bool_t SharedStrips(CbmHit* hit1, CbmHit* hit2);
  void MarkUsed(Hit &hit);
 protected:
  Int_t fRCTracks;
  Bool_t fStandAlone;

  Double_t* fTxMax;
  Double_t* fTyMax;
  Double_t* fdTxMax;
  Double_t* fdTyMax;
  Double_t fChi2Max;
  Double_t fQpMax;
  Double_t fNsigma;

  TClonesArray* mcTrackArray;
  TClonesArray* rcTrackArray;
  TClonesArray* mcPointArray;
  TClonesArray* mHitArray;
  TClonesArray* hHitArray;
  TClonesArray* sHitArray;
  TClonesArray* newTrackArray;

  map<int,vector<Hit> > vHits;
  Double_t fSt_z [10];
  Double_t fSt_dz[10];
  Double_t fStR1_z[10];
  Double_t fStR2_z[10];
  Double_t fDx1[10];
  Double_t fDy1[10];
  Double_t fDx2[10];
  Double_t fDy2[10];
  Int_t fStL;
  Int_t fStF;
  Int_t fNstations;
  CbmStsKFTrackFitter* fFitter;
  Bool_t h_used[100000];
  Bool_t s_used[100000];
  vector<Hit*> vTrackHits;

 private:
  CbmAnaHypSecFinder(const CbmAnaHypSecFinder&);
  CbmAnaHypSecFinder& operator=(const CbmAnaHypSecFinder&);
  
  ClassDef(CbmAnaHypSecFinder,1);
};

class CbmKFTrackSec : public CbmKFTrack {
 public:
 CbmKFTrackSec() : CbmKFTrack(), hits(), sig(0), qp0(0.)
    {
      CbmKFTrack();
      for (Int_t i=0;i<6;i++) GetTrack()[i]=0;
      Double_t* C = GetCovMatrix();
      Double_t INF = 100000000;
      C[ 0] = INF;
      C[ 1] = 0.; C[ 2] = INF;
      C[ 3] = 0.; C[ 4] = 0.; C[ 5] = INF;
      C[ 6] = 0.; C[ 7] = 0.; C[ 8] = 0.; C[ 9] = INF;
      C[10] = 0.; C[11] = 0.; C[12] = 0.; C[13] =  0.; C[14] = INF;
      GetRefNDF() = -5;
      GetRefChi2() = 0;
      hits.clear();
    }
  ~CbmKFTrackSec(){ hits.clear();};

  Bool_t Sig();

  void Print(){
    Double_t* T=GetTrack();
    printf("TrackInfo:");
    if (this->Sig()) printf("+"); else printf(" ");
    printf("  (x,y,z)=(%6.2f,%6.2f,%6.2f)",T[0],T[1],T[5]);
    printf("  p=%6.3f",TMath::Abs(1./T[4]));
    printf("  chi2=%9.2f",GetRefChi2()/GetRefNDF());
    Double_t* C=GetCovMatrix();
    printf("  (dx,dy,dqp)=(%7.5f,%7.5f,%7.5f)",sqrt(C[0]),sqrt(C[2]),sqrt(C[14]));
    printf("\n");
  }

  vector<Hit*> hits;
  Int_t sig;
  Double_t qp0;
};


class Hit{
 public:
  Hit() : pt(NULL), id(0), type(0), x(0.), y(0.), z(0.), st(0), used(kFALSE),
    tr(-1), hit(NULL), sHit() {};
  Hit(Int_t i, CbmHit* hitPt, Int_t trackId, Int_t t=1)
    : pt(hitPt), id(i), type(t), x(pt->GetX()), y(pt->GetY()), 
    z(pt->GetZ()), st(CbmKF::Instance()->StsStationIDMap[pt->GetDetectorID()]), 
    used(kFALSE), tr(trackId), hit(NULL), sHit()
    {
      sHit.Create( (CbmStsHit*) hitPt );
      hit=&this->sHit;
    }

  ~Hit(){};

  CbmHit*  pt;
  Int_t    id;
  Int_t    type; // 0 - maps; 1 - pixel; 2 - strips
  Double_t x;
  Double_t y;
  Double_t z;
  Int_t    st;
  Bool_t used;
  Int_t    tr;
  CbmKFHit* hit;
  CbmKFStsHit sHit;
  int operator==(const Hit &h){ return id == h.id; }

  CbmKFHit *GetKFHit(){
    return &sHit;
  }

  void Filter(CbmKFTrackSec &track, bool downstream){
    GetKFHit()->Filter(track,downstream,track.qp0);
  }

 private:
 
  //Hit(const Hit&);
  //Hit operator=(const Hit&);

};



static bool CompareY(const Hit h1, const Hit h2) {return h1.y<h2.y;}

class GreaterY{
  Double_t y;
 public:
 GreaterY(Double_t y_min) : y(y_min) { }
  bool operator () (Hit &h) { return (h.y > y); }
};

#endif
