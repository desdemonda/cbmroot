// -------------------------------------------------------------------------
// -----                  CbmMuchStrawHitFinderQa header file          -----
// -----                  Created 26/05/09  by A. Zinchenko            -----
// -------------------------------------------------------------------------

#ifndef CBMMUCHSTRAWHITFINDERQA_H
#define CBMMUCHSTRAWHITFINDERQA_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "TString.h"
#include "TH2.h"

class CbmMuchGeoScheme;
class TFile;
class TH1D;

class CbmMuchStrawHitFinderQa : public FairTask {

public:
  CbmMuchStrawHitFinderQa(const char *name, const char* digiFileName, Int_t verbose = 1);
  virtual ~CbmMuchStrawHitFinderQa();
  virtual InitStatus Init();
  virtual void Exec(Option_t * option);
  virtual void Finish();
  virtual void SetParContainers();

  void SetMirror(Int_t mirror) { fMirror = mirror; }
private:
  Int_t fVerbose;
  Int_t fEvent;
  TClonesArray *fPoints;
  TClonesArray *fDigis;
  TClonesArray *fDigiMatches;
  TClonesArray *fHits;

  TString            fDigiFile;      // Digitization parameter file
  CbmMuchGeoScheme*  fGeoScheme;     // Geometry scheme

  Int_t fMirror; // flag if mirror hits present (left-right ambig.)
  Double_t fRadIn[6]; // inner radii of stations 
  //Int_t  fNstations;
  TH1D* fhOccup[6];
  TH1D* fhOccTub[6];
  TH1D* fhMult[6];
  TH1D* fhDx[6];
  TH1D* fhAngTrue[6];
  TH1D* fhAngWrong[6];
  TH1D* fhAngMin[6];
  TH1D* fhAngDif[6];
  TH2D* fhAngDif2[6];
  TH1D* fhAngDifW[6];
  TH2D* fhAngDifW2[6];
  TH1D* fhDtube[6];
  TH1D* fhComb[6];
  TH1D* fhAll;
  //TH1D** fhPadsTotalR;
  //TH1D** fhPadsFiredR;

  TH1D* fhPoolX;
  TH1D* fhPoolY;

  void CheckMirrors(); // check mirror hits

  CbmMuchStrawHitFinderQa(const CbmMuchStrawHitFinderQa&);
  CbmMuchStrawHitFinderQa& operator=(const CbmMuchStrawHitFinderQa&);

  ClassDef(CbmMuchStrawHitFinderQa,1) 
};

#endif 

