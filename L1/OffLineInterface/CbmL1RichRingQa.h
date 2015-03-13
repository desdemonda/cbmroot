#ifndef CBML1RICHRINGQA_H
#define CBML1RICHRINGQA_H

#include "FairTask.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TArc.h"
#include "TLatex.h"
#include "TText.h"
//#include "conv.h"

#include <map>
#include <vector>
#include <list>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <math.h>

class CbmL1RichRingQa : public FairTask{
  private:
    CbmL1RichRingQa(const CbmL1RichRingQa&);
    CbmL1RichRingQa operator=(const CbmL1RichRingQa&);
  struct MCRing
  {
    MCRing():MCTrackID(-1),primary(0),P(0),PDG(-1),NHits(0),Reconstructed(0),kind(0),x(0),y(0),r(0),k(0),Hits(),BestReco(0),NHitsBestReco(0),NHitsBestvsNHitsMC(0){};
    Int_t MCTrackID;
    bool primary;
    Double_t P;
    Int_t PDG;
    Int_t NHits;
    Int_t Reconstructed;
    Int_t kind;
    Double_t x, y, r;
    Int_t k;
    std::vector<int> Hits;
    Int_t BestReco;
    Int_t NHitsBestReco;
    Double_t NHitsBestvsNHitsMC;
  };

  struct PerfHit
  {
    Double_t x, y;
    Int_t MCTrackID;
    Int_t index;
    Bool_t found;  
    Bool_t on_ring;
  };

  TClonesArray* fRingArray;        // Array of CbmRichRings
  TClonesArray* fMCPointArray;       // Array of FairMCPoints
  TClonesArray* fMCTrackArray;       // Array of CbmMCTracks
  TClonesArray* fHitArray;         // Array of CbmRichHits

  private:

  TH1F *Chi2Ghost;
  TH1F *Chi2Ref;
  TH1F *Chi2All;
  TH1F *Chi2Clone;
  TH2F *Chi2NhitsGhost;
  TH2F *Chi2NhitsAll;
  TH1F *RGhost;
  TH1F *REl;
  TH1F *RPi;
  TH1F *NHitsMC;
  TH1F *NSameHits;

//  TH2F *Chi2NhitsGhost;
  TH2F *Chi2NhitsPi;
  TH2F *Chi2NhitsEll;
  TH2F *RNhitsGhost;
  TH2F *RNhitsPi;
  TH2F *RNhitsEll;
  TH2F *RChi2Ghost;
  TH2F *RChi2Pi;
  TH2F *RChi2Ell;
  TH2F *NSameHitsVsP;
  TH2F *NHitsVsMCP;
  TH2F *RadiusVsPForClone;
  TH2F *DistanceVsPClone;
  TH2F *Chi2VsPClone;
  TH2F *RadiusVsDistanceClone;
  TH2F *NHitsRecoVsNHitsMC;

  public:

  /** Default constructor **/

  CbmL1RichRingQa(const char *name="CbmL1RichRingQa", const char *title="CbmL1RichRingQa", Int_t verbose=1);

  /** Destructor **/
  ~CbmL1RichRingQa();

  /** Initialization of the task **/
  InitStatus Init();

  /** Executed task **/
  void Exec(Option_t* option);

  /** Finish task **/
  void Finish();

  void CirFit( std::list<std::pair<Double_t,Double_t> > &P, Double_t *X, Double_t *Y, Double_t *R );

  ClassDef(CbmL1RichRingQa,1)

};

#endif
