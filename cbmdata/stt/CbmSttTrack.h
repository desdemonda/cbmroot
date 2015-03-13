#ifndef CbmSTTTRACK_H
#define CbmSTTTRACK_H 1


#include "FairTrackParam.h"
#include "TObject.h"

class CbmSttTrack : public TObject
{

 public:

  /** Default constructor **/
  CbmSttTrack();


  /** Destructor **/
  virtual ~CbmSttTrack();

  void Print();

  /** Adds the hit index to the index array **/

  void AddHitIndex(Int_t hitID){ fHits[fNHits++] = hitID; }

  /** Accessors  **/

  Int_t GetStsTrackID() const { return fStsTrackID;}

  Int_t GetNHits() const { return  fNHits;}
  Int_t GetNMissedHits() const { return  fNMissedHits;}
  Int_t GetNMissedStations() const { return  fNMissedStations;}
  Int_t GetHitIndex(Int_t iHit)   const { return fHits[iHit]; }
  Double_t GetChi2()   const { return fChi2; } 
  Int_t GetNDF() const { return fNDF; }
  FairTrackParam *GetSttTrack() { return &fSttTrack; }
  Int_t GetFlag() const {return fFlag;};
  Double_t GetScatAng() const {return fScatAng;};
  //Double_t GetChi2Seed() const {return fChi2Seed;};
  
  void SetNMissedHits( Int_t n ){ fNMissedHits = n; }
  void SetNMissedStations( Int_t n ){  fNMissedStations = n; }
  void SetChi2( Double_t chi2 ) { fChi2  = chi2; }
  void SetNDF( Int_t ndf ){ fNDF = ndf; }
  void SetStsTrackID(Int_t it) { fStsTrackID = it; }
  void SetSttTrack( FairTrackParam *track ){ fSttTrack = *track; }
  void SetFlag(Int_t flag) { fFlag = flag;};
  void SetScatAng(Double_t angle) { fScatAng = angle;};
  //void SetChi2Seed(Double_t chi2) { fChi2Seed = chi2;};

 private:

  Int_t fStsTrackID;
  Int_t fNHits;
  Int_t fNMissedHits;
  Int_t fNMissedStations;
  Int_t fHits[30];
  FairTrackParam fSttTrack;
  Double_t fChi2;
  Int_t fNDF;
  Int_t fFlag; 
  Double_t fScatAng; 
  //Double_t fChi2Seed; 

  ClassDef(CbmSttTrack,1);

};

#endif
