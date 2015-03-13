//-----------------------------------------------------------
//-----------------------------------------------------------

#ifndef CbmKFParticleFinder_HH
#define CbmKFParticleFinder_HH

#include "FairTask.h"

#include "TString.h"

#include <vector>

class CbmKFParticleFinderPID;
class KFParticleTopoReconstructor;
class TClonesArray;
class CbmVertex;

class CbmKFParticleFinder : public FairTask {
 public:

  // Constructors/Destructors ---------
  CbmKFParticleFinder(const char* name = "CbmKFParticleFinder", Int_t iVerbose = 0);
  ~CbmKFParticleFinder();

  void SetPVToZero() { fPVFindMode = 0; }
  void ReconstructSinglePV() { fPVFindMode = 1; }
  void RconstructMultiplePV() { fPVFindMode = 2; }
  
  void SetStsTrackBranchName(const TString& name)   { fStsTrackBranchName = name;  }

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);

  const KFParticleTopoReconstructor * GetTopoReconstructor() const { return fTopoReconstructor; }
  
  void SetPIDInformation(CbmKFParticleFinderPID* pid) { fPID = pid; }
  
  // set cuts
  void SetPrimaryProbCut(float prob);
  
 private:
  
  double InversedChi2Prob(double p, int ndf) const;
  
  const CbmKFParticleFinder& operator = (const CbmKFParticleFinder&);
  CbmKFParticleFinder(const CbmKFParticleFinder&);
   
  //names of input branches
  TString fStsTrackBranchName;      //! Name of the input TCA with reco tracks

  //input branches
  TClonesArray *fTrackArray;
  CbmVertex* fPrimVtx;

  //topology reconstructor
  KFParticleTopoReconstructor *fTopoReconstructor;
  int fPVFindMode;
  
  //PID information
  CbmKFParticleFinderPID* fPID;
  
  ClassDef(CbmKFParticleFinder,1);
};

#endif
