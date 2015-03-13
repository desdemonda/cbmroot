// -------------------------------------------------------------------------
// -----                  CbmEcalAnalysis header file                  -----
// -----                  Created 04/01/05                             -----
// -----                  Yuri Kharlov                                 -----
// -------------------------------------------------------------------------

#ifndef CBMECALANALYSIS_H
#define CBMECALANALYSIS_H

#include "FairTask.h"
#include "CbmStack.h"

class CbmStack;
class TArrayD;
class TH1F;
class TH2F;

class CbmEcalAnalysis : public FairTask {

public:

  CbmEcalAnalysis();
  CbmEcalAnalysis(const char *name, const char *title="ECAL analysis");
  virtual ~CbmEcalAnalysis();

  virtual InitStatus Init();
  void Exec(Option_t* option);
  void Finish();
  void SetDebug(Option_t *debug="") {fDebug = debug;}
  void SetVectorMeson(Int_t kfVectorMeson) {fKfVectorMeson = kfVectorMeson;}
  void AnalyzePrimary();
  void AnalyzeMCPointsEdep();
  void AnalyzeMCPointsWall();
  void AnalyzeHits();
  void AnalyzeRecParticles();
  void AnalyzeAcceptanceElectrons();
  void AnalyzeAcceptanceVectorMesons();

private:

  TClonesArray* fListStack;      // List of particles in a stack
  TClonesArray* fListECALptsEdep;// ECAL MC points inside ECAL
  TClonesArray* fListECALptsWall;// ECAL MC points on entrance to ECAL
  TClonesArray* fListECALhits;   // ECAL hits
  TClonesArray* fListECALrp;     // ECAL rec.particles

  Int_t    fEvent;     // current event number
  Int_t    fNprimary;  // number of primary tracks

  Int_t    fKfVectorMeson;  // PDG code of vector meson

  // Primary particles

  TH1F *fhPrimPabs;  // Abs momentum distribution of primaries
  TH1F *fhPrimPt;    // Transverse momentum distribution of primaries
  TH1F *fhPrimEta;   // Pseudorapidity distribution of primaries
  TH1F *fhPrimTheta; // Polar angle distribution of primaries
  TH1F *fhPrimPhi;   // Azimuthal angle distribution of primaries
  TH2F *fhPrimPtEta; // Transverse momentum vs Eta of primaries

  // MC points

  TH1F *fhMcpPabs;   // Abs momentum distribution of detected
  TH1F *fhMcpPt;     // Transverse momentum distribution of detected
  TH1F *fhMcpEta;    // Pseudorapidity distribution of detected
  TH1F *fhMcpTheta;  // Polar angle distribution of detected
  TH1F *fhMcpPhi;    // Azimuthal angle distribution of detected
  TH2F *fhMcpPtEta;  // Transverse momentum vs Eta of detected
  TH2F *fhMcpXY;        // (X,Y) coordinates of MC points

  // Hits

  TH2F *fhHitXY;     // x,y coordinates of ECAL hits
  TH1F *fhHitE;      // ECAL hit energy

  // Reconstructed particles

  TH1F *fhRecPabs;   // reconstructed absolute momentum
  TH1F *fhRecPt;     // reconstructed pt
  TH1F *fhRecYrap;   // reconstructed rapidity
  TH1F *fhRecPhi;    // reconstructed azimuthal angle

  // Acceptance

  TH2F* fhPtYrap;       // Pt and Y rapidity of primary particles
  TH2F* fhPtYrapSignal; // Pt and Y rapidity of signal particles
  TH2F* fhPtYrapAccept; // acceptance

  Option_t *fDebug;    //! debug flag

  CbmEcalAnalysis(const CbmEcalAnalysis&);
  CbmEcalAnalysis& operator=(const CbmEcalAnalysis&);

  ClassDef(CbmEcalAnalysis,2)

}; 

#endif
