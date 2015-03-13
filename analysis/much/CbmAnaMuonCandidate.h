#ifndef CBMANAMUONCANDIDATE_H
#define CBMANAMUONCANDIDATE_H

#include "CbmKFTrack.h"
#include "TLorentzVector.h"
#define NPLANES 31

class CbmAnaMuonCandidate : public TObject{
  public:
    CbmAnaMuonCandidate();
    CbmAnaMuonCandidate(Int_t iTrack, TLorentzVector pMC);
    virtual ~CbmAnaMuonCandidate(){};
    Int_t* GetMuchPoints() { return muchPoints; };
    Int_t* GetMuchHits()   { return muchHits;   };
    void SetMuchPoint(Int_t planeId, Int_t index) { muchPoints[planeId]=index; }
    void SetMuchHit  (Int_t planeId, Int_t index) { muchHits[planeId]=index;   }
    void SetMomentumMC(TLorentzVector mom) {fpMC = TLorentzVector(mom); }
    void SetMomentumRC(TLorentzVector mom) {fpRC = TLorentzVector(mom); }
    void SetMomentumRC(Double_t* T);
    void SetMCTrackId(Int_t trackId) {fMCTrackId    = trackId; }
    void SetNStsHits(Int_t nStsHits) {fNStsHits     = nStsHits; }
    void SetNMuchHits(Int_t nMuchHits) {fNMuchHits     = nMuchHits; }
    void SetChiToVertex(Double_t chi) {fChiToVertex = chi; }
    void SetNTriggerHits(Int_t n) { fNTriggerHits = n; }
    void SetSign(Double_t sign) {fSign = sign; }
    TLorentzVector* GetMomentumMC() {return &fpMC; }
    TLorentzVector* GetMomentumRC() {return &fpRC; }
    Bool_t IsReconstructed(Int_t nMuchHitsCut=13, Int_t nStsHitsCut=8, Double_t chiToVertexCut=3.);
    void SetReconstructed(Bool_t isRec) {fIsReconstructed = isRec; }
    Double_t GetSign() {return fSign;}
    void SetNMuchPoints(Int_t nPoints) {fNMuchPoints = nPoints; }
    void SetNStsPoints(Int_t nPoints) {fNStsPoints = nPoints; }
    Int_t GetNMuchPoints() { return fNMuchPoints; }
    Int_t GetNStsPoints() { return fNStsPoints; }
    Int_t GetNMuchHits() { return fNMuchHits; }
    Int_t GetNStsHits() { return fNStsHits; }
  private:
    CbmKFTrack track;
    Int_t fMCTrackId;
    TLorentzVector fpMC;
    TLorentzVector fpRC;
    Int_t muchPoints[NPLANES];
    Int_t muchHits[NPLANES];
    Int_t stsPoints[NPLANES];
    Int_t stsHits[NPLANES];
    Bool_t fIsReconstructed;
    Int_t fNStsHits;
    Int_t fNMuchHits;
    Double_t fChiToVertex;
    Int_t fNTriggerHits;
    Double_t fSign;
    Int_t fNMuchPoints;
    Int_t fNStsPoints;
  ClassDef(CbmAnaMuonCandidate,2);
};

#endif
