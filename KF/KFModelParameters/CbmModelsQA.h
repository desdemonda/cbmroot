/*
 *====================================================================
 *
 *  CBM Models
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
#include "CbmModelBase.h"
#include <vector>
#include <map>
#include <cstring>

class TClonesArray;
class CbmVertex;
class TDirectory;
class TH1F;
class TH2F;
class KFParticleTopoReconstructor;

class CbmModelsQA :public FairTask
{
 public:

  CbmModelsQA(Int_t iVerbose = 1, int findParticlesMode = 1, int perf = 3,
						 KFParticleTopoReconstructor *tr=0,
                         const char *name ="CbmModelsQA",
                         const char *title="Cbm Models QA", float ekin_=24.08);
  ~CbmModelsQA();

  void SetOutFile(TString outname) {outfileName = outname;}

  virtual InitStatus ReInit();
  virtual InitStatus Init();
  void Exec(Option_t * option);
  void Finish();
  
  
  void AddHRGAnalysis(int TracksType = -1, double SystError = 0., TString name = "MC", int EventStats = 1, Bool_t UseWidth = false, Bool_t UseStatistics = false, double rad = 0.);

  void AddMultiscatteringAnalysis(int TracksType = -1, double SystError = 0., TString name = "MC", int EventStats = 1);
  
  void AddImpactParameterAnalysis(int TracksType = -1, double SystError = 0., TString name = "MC", TString InputTable = "");
  
  void AddInverseSlopeAnalysis(int PDGID = -211, const char *pname ="pi-", int TracksType = -1, double SystError = 0., TString name = "MC", int EventStats = 1);
  
  void AddBoltzmannAnalysis(int PDGID = -211, const char *pname ="pi-", int TracksType = -1, double SystError = 0., TString name = "MC", int EventStats = 1);
  
  void AddBlastWaveAnalysis(int PDGID = -211, const char *pname ="pi-", int TracksType = -1, double SystError = 0., TString name = "MC", int EventStats = 1, double Tlong = 0.132);
  
 private:
 
  void WriteHistos( TObject *obj );
  void WriteHistosCurFile( TObject *obj );

  CbmVertex*  fPrimVtx;

// Names of files
  TString outfileName;

  TDirectory* histodir;

  std::vector<int> vStsHitMatch;
  std::vector<int> vStsPointMatch;
  std::vector<int> vMvdPointMatch;
  std::vector<int> vMCTrackMatch;
  
  int fNEvents;
  
  KFParticleTopoReconstructor *fTopoReconstructor;
  
  float ekin;
  
  ClassDef(CbmModelsQA,1);

 private:
  CbmModelsQA(const CbmModelsQA&);
  void operator=(const CbmModelsQA&);
  std::vector<CbmModelBase*> Models;
  TDirectory* histodirmod;
};

#endif // _CbmModelsQA_h_
