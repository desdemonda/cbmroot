#include "CbmAnaMuonCandidate.h"
#include "CbmStsTrack.h"
#include "CbmKFTrack.h"
#include "TLorentzVector.h"

CbmAnaMuonCandidate::CbmAnaMuonCandidate()
  : TObject(),
    track(),
    fMCTrackId(-1),
    fpMC(),
    fpRC(),
    muchPoints(),
    muchHits(),
    stsPoints(),
    stsHits(),
    fIsReconstructed(kFALSE),
    fNStsHits(0),
    fNMuchHits(0),
    fChiToVertex(100.),
    fNTriggerHits(0),
    fSign(0.),
    fNMuchPoints(0),
    fNStsPoints(0)
{
  for (Int_t i=0;i<NPLANES;i++){
    muchPoints[i]=-1;
    muchHits[i]=-1;
    stsPoints[i]=-1;
    stsHits[i]=-1;
  }  
}


CbmAnaMuonCandidate::CbmAnaMuonCandidate(Int_t iTrack, TLorentzVector pMC)
  : TObject(),
    track(),
    fMCTrackId(iTrack),
    fpMC(pMC),
    fpRC(),
    muchPoints(),
    muchHits(),
    stsPoints(),
    stsHits(),
    fIsReconstructed(kFALSE),
    fNStsHits(0),
    fNMuchHits(0),
    fChiToVertex(100.),
    fNTriggerHits(0),
    fSign(0.),
    fNMuchPoints(0),
    fNStsPoints(0)
{
  for (Int_t i=0;i<NPLANES;i++){
    muchPoints[i]=-1;
    muchHits[i]=-1;
    stsPoints[i]=-1;
    stsHits[i]=-1;
  }
  
}

void CbmAnaMuonCandidate::SetMomentumRC(Double_t* T){
  Double_t tx = T[2];
  Double_t ty = T[3];
  Double_t qp = T[4];
  Double_t p  = ( TMath::Abs(qp)>1.e-4 ) ? 1./TMath::Abs(qp) :1.e4;
  Double_t pz = TMath::Sqrt( p*p / ( tx*tx + ty*ty + 1 ) );
  Double_t px = tx * pz;
  Double_t py = ty * pz;
  fpRC.SetXYZM(px,py,pz,0.105658);
}

Bool_t CbmAnaMuonCandidate::IsReconstructed(Int_t nMuchHitsCut, Int_t nStsHitsCut, Double_t chiToVertexCut){
  if (fNMuchHits<nMuchHitsCut) return kFALSE;
  if (fNStsHits<nStsHitsCut) return kFALSE;
  if (fChiToVertex>chiToVertexCut) return kFALSE;
  return kTRUE;
}

ClassImp(CbmAnaMuonCandidate);
