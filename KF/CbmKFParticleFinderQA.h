//-----------------------------------------------------------
//-----------------------------------------------------------

#ifndef CbmKFParticleFinderQA_HH
#define CbmKFParticleFinderQA_HH

#include "FairTask.h"

#include "TString.h"

#include <vector>

class KFParticleTopoReconstructor;
class KFTopoPerformance;
class TClonesArray;
class TFile;
class TObject;

class CbmKFParticleFinderQA : public FairTask {
 public:

  // Constructors/Destructors ---------
  CbmKFParticleFinderQA(const char* name = "CbmKFParticleFinderQA", Int_t iVerbose = 0, KFParticleTopoReconstructor* tr=0,
                        TString outFileName="CbmKFParticleFinderQA.root");
  ~CbmKFParticleFinderQA();

  void SetEffFileName(const TString& name) { fEfffileName = name; }
  void SetMCTrackBranchName(const TString& name)   { fMCTracksBranchName = name;  }
  void SetTrackMatchBranchName(const TString& name)   { fTrackMatchBranchName = name;  }

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  
  void SetPrintEffFrequency(Int_t n);

  void SaveParticles(Bool_t b = 1) { fSaveParticles = b; }
  void SaveMCParticles(Bool_t b = 1) { fSaveMCParticles = b; }
  
 private:
  
  const CbmKFParticleFinderQA& operator = (const CbmKFParticleFinderQA&);
  CbmKFParticleFinderQA(const CbmKFParticleFinderQA&);
  
  void WriteHistosCurFile( TObject *obj );
  
  //names of input branches
  TString fMCTracksBranchName;      //! Name of the input TCA with MC tracks
  TString fTrackMatchBranchName;      //! Name of the input TCA with track match

  //input branches
  TClonesArray *fMCTrackArray; //mc tracks
  TClonesArray *fTrackMatchArray; //track match
 
 // output arrays of particles
  TClonesArray* fRecParticles;    // output array of KF Particles
  TClonesArray* fMCParticles;     // output array of MC Particles
  TClonesArray* fMatchParticles;  // output array of match objects

  Bool_t fSaveParticles;
  Bool_t fSaveMCParticles;
  
  //output file with histograms
  TString fOutFileName;
  TFile* fOutFile;
  TString fEfffileName;
  //KF Particle QA
  KFTopoPerformance* fTopoPerformance;
  
  Int_t fPrintFrequency;
  Int_t fNEvents;
  Double_t fTime[5];
  
  ClassDef(CbmKFParticleFinderQA,1);
};

#endif
