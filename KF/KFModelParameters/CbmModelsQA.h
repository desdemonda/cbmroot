/*
 *====================================================================
 *
 *  CBM KF Models
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  KF Particles Finder performance 
 *
 *====================================================================
 */

#ifndef _CbmModelsQA_h_
#define _CbmModelsQA_h_

//#include "KFMCParticle.h"
//#include "KFParticleMatch.h"
#include "../KFQA/CbmKFPartEfficiencies.h"
#include "../KFQA/CbmKFTrErrMCPoints.h"
#include "FairTask.h"
#include "CbmThermalModelNoFlow.h"
//#include "../KFModelParameters/CbmMSS.h"
#include <vector>
#include <map>
#include <cstring>

//class CbmKFParticlesFinder;
class TClonesArray;
class CbmVertex;
class TDirectory;
class TH1F;
class TH2F;

class CbmModelsQA :public FairTask
{
 public:

  CbmModelsQA(Int_t iVerbose = 1, int findParticlesMode = 1, int perf = 3,
                         const char *name ="CbmModelsQA",
                         const char *title="Cbm Models QA", float ekin_=24.08);
  ~CbmModelsQA();

  void SetOutFile(TString outname) {outfileName = outname;}

  virtual InitStatus ReInit();
  virtual InitStatus Init();
  void Exec(Option_t * option);
  void Finish();

 private:
 
  void WriteHistos( TObject *obj );
  void WriteHistosCurFile( TObject *obj );

  //CbmVertex*    fPrimVtx;

// Names of files
  TString outfileName;

  TDirectory* histodir;

  //std::vector<int> vStsHitMatch;
  //std::vector<int> vStsPointMatch;
  //std::vector<int> vMvdPointMatch;
  //std::vector<int> vMCTrackMatch;

  int fNEvents;
  
  ClassDef(CbmModelsQA,1);

 private:
  CbmModelsQA(const CbmModelsQA&);
  CbmModelsQA& operator=(const CbmModelsQA&);
  CbmThermalModelNoFlow* ThermalNoFlow;
  //CbmMSS* MSSModel;
  TDirectory* histodirmod;
};

#endif // _CbmKFParticlesFinderQA_h_
