// -------------------------------------------------------------------------
// -----              CbmMvdQa  header file                            -----
// -----              Created 12/01/15  by P. Sitzmann                 -----
// -------------------------------------------------------------------------


/**  CbmMvdQa.h
 *@author P.Sitzmann <p.sitzmann@gsi.de>
 *
 *  Selection of Qa functions for the mvd
 *
 **/


#ifndef CBMMVDQA_H
#define CBMMVDQA_H 1

#include "FairTask.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TNtuple.h"
#include <iostream>
#include "TVector3.h"
#include "TClonesArray.h"
#include "base/CbmLitPtrTypes.h"
#include "CbmTrackMatchNew.h"
#include "CbmStsTrack.h"


class TClonesArray;
class CbmStsTrack;
class CbmVertex;
class FairTrackParam;

class CbmLitTrackParam;

class CbmStsTrack;

class CbmMvdQa : public FairTask
{
public:
  CbmMvdQa();
  
  CbmMvdQa(const char* name, Int_t iMode = 0, Int_t iVerbose = 0);
 
  ~CbmMvdQa();

  Double_t GetImpactParameterRadius(CbmLitTrackParam etrack);
  Double_t GetImpactParameterX(CbmLitTrackParam etrack);
  Double_t GetImpactParameterY(CbmLitTrackParam etrack);

  Double_t GetTransverseMomentum(CbmLitTrackParam etrack);
  Double_t GetMomentum(CbmLitTrackParam etrack);
  Double_t GetMomentumZ(CbmLitTrackParam etrack);
  Double_t GetAngle(CbmLitTrackParam etrack);

  void     SetMinHitReq(Int_t nrOfHits){fminHitReq = nrOfHits;};
  void     SetMatches(Int_t trackID, CbmStsTrack* stsTrack);
  void     Exec(Option_t* opt); 

  InitStatus Init();

  void Finish();
   

private:

  Int_t fNHitsOfLongTracks;
  Int_t fEventNumber;
  Int_t fminHitReq;
  Float_t fMvdRecoRatio;
  Int_t fBadTrack;
  Int_t fUsedTracks;
  Int_t fnrOfMergedHits;

  TClonesArray* fStsTrackArray;
  TClonesArray* fStsTrackArrayP;
  TClonesArray* fStsTrackArrayN;
  TClonesArray* fStsTrackMatches;
  TClonesArray* fMcPoints; 

  TClonesArray* fListMCTracks;
  TClonesArray* fMCTrackArrayP;
  TClonesArray* fMCTrackArrayN;

  TClonesArray* fMvdHits;
  TClonesArray* fMvdHitMatchArray;
  TClonesArray* fBadTracks;

  TClonesArray* fInfoArray;
  TH2F* fMvdResHisto;
  TH2F* fMvd4ResHisto;
  TH2F* fVtxResHisto;
  TH2F* fMvdAngleResHisto;
  TH2F* fMvdAngleResHisto1GeV;
  TH2F* fMvdAngleResHistolarger1GeV;
  TH2F* fMvdAngleResHisto2GeV;
  TH2F* fMvdAngleResHistolarger2GeV;
  TH2F* fMvdResHistoTrasMom;
  TH1F* fMatchingEffiHisto;


  TrackExtrapolatorPtr fExtrapolator;
  CbmVertex* fPrimVtx;
  CbmVertex* fSecVtx;


 ClassDef(CbmMvdQa,1);
};


#endif
