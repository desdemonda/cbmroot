/*
 *====================================================================
 *
 *  Extraction of temperature from Boltzmann distribution
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  Extraction of temperature from Boltzmann distribution
 *
 *====================================================================
 */
 
#ifndef _CbmBoltzmannDistribution_h_
#define _CbmBoltzmannDistribution_h_
 
#include "../CbmModelBase.h"
#include "../KFQA/CbmKFTrErrMCPoints.h"
#include "TMath.h"
#include <vector>
#include <algorithm>
#include <map>
 
class TClonesArray;
class CbmVertex;
class TDirectory;
class TH1F;
class TH2F;
class TGraphErrors;
class TSpline3;
class FairRootManager;

class BoltzmannDistribution;

class CbmBoltzmannDistribution: public CbmModelBase
{
 public:
 //static const int p_sz = 8;
  TString name;
  CbmBoltzmannDistribution(Int_t recoLevel = -1,	// -1 - MC Tracks, 0 - Reconstructible MC Tracks, 1 - Reco Tracks with MC Data, 2 - Reco Tracks with Reco Momentum and MC Primary, 3 - Reco Tracks with Reco Momentum and Reco Primary
			   //Int_t usePID = 1,	// 1 - MC, 2 - TOF
			   //Int_t trackNumber = 1,
			   Int_t iVerbose = 1,
			   TString Mode = "MC",
			   Int_t PDG    = -211,	// ID of particle to analyze
			   TString pname = "pi-",
			   Int_t EventStats = 1,
			   KFParticleTopoReconstructor* tr = 0,
			   Float_t	ekin_ = 25.);
  ~CbmBoltzmannDistribution();

  virtual void ReInit(FairRootManager *fManger);
  virtual void Init();
  virtual void Exec();
  virtual void Finish();
  
  void AddRapidityInterval(double ymin, double ymax);
  void AddHistos();

  ClassDef(CbmBoltzmannDistribution,1);

 private:
  
  void CalculateAveragesInEvent(int RecoLevel, bool UpdateGlobal=0);
  
  Float_t ekin;
  Float_t p0cm;
  Float_t ycm;
  Bool_t fUpdate;
  Int_t fusePID;
  Int_t fRecoLevel;
  Int_t fTrackNumber;	// 0 - test calculation, only Pions, 1 - UrQMD Au+Au
  Int_t fEventStats;
  Int_t events;
  

  TString fModeName;
  TString outfileName;
  
  TDirectory* histodir;
  
  TClonesArray* flistMCTracks;
  
  //Int_t events;
  
  static const int nHisto1D = 6; //T
  int IndexT, IndexMt, IndexModelMt, IndexMt2, IndexModelMt2, IndexModelMt4Pi;
  
  TH1F *histo1D[nHisto1D];
  TH1F *histodndy, *histodndymodel;
  TH1F ***histo1DIntervals;
  
  TGraphErrors *grTy;
  TGraphErrors *grdndyReco;
  
  TH1F *pullT;
  std::vector<double> Ts;
  
  
  double kProtonMass;
  
  int fPDGID;
  double fMass;
  std::vector<double> fYminv, fYmaxv;
  double paramGlobal;
  std::vector<double> paramGlobalInterval, param2GlobalInterval;
  double paramLocal;
  std::vector<double> paramLocalInterval;
  int totalLocal, totalGlobal;
  std::vector<int> totalGlobalInterval, totalLocalInterval;
  int totalEvents;


  CbmBoltzmannDistribution(const CbmBoltzmannDistribution&);
  CbmBoltzmannDistribution& operator=(const CbmBoltzmannDistribution&);
  
  BoltzmannDistribution *model, *modelmc;
  std::vector<BoltzmannDistribution*> modelsY;
};

#endif