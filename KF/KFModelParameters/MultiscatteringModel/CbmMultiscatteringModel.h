/*
 *====================================================================
 *
 *  CBM Multiscattering Model
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  Multiscattering model parameter extraction, works well only for 4-pi MC data atm
 *
 *====================================================================
 */
 
#ifndef _CbmMultiscatteringModel_h_
#define _CbmMultiscatteringModel_h_
 
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

class MultiscatteringModel;

class CbmMultiscatteringModel: public CbmModelBase
{
 public:
 //static const int p_sz = 8;
  CbmMultiscatteringModel(Int_t recoLevel = -1,	// -1 - MC Tracks, 0 - Reconstructible MC Tracks, 1 - Reco Tracks with MC Data, 2 - Reco Tracks with Reco Momentum and MC Primary, 3 - Reco Tracks with Reco Momentum and Reco Primary
			   //Int_t usePID = 1,	// 1 - MC, 2 - TOF
			   //Int_t trackNumber = 1,
			   Int_t iVerbose = 1,
			   TString Mode = "MC",
			   Int_t EventStats=1,
			   KFParticleTopoReconstructor* tr=0,
			   Float_t	ekin_ = 25.);
  ~CbmMultiscatteringModel();

  virtual void ReInit(FairRootManager *fManger);
  virtual void Init();
  virtual void Exec();
  virtual void Finish();

  ClassDef(CbmMultiscatteringModel,1);

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
  
  static const int nHisto1D = 9; //sigt, sigz, qz, pt, pz, dndy
  int IndexSigt, IndexSigz, IndexQz, IndexPt, IndexPz, IndexY, IndexModelPt, IndexModelPz, IndexModelY;
  
  TH1F *histo1D[nHisto1D];
  
  static const int nHisto2D = 2; //y-pt
  int IndexYPt, IndexModelYPt;
  
  TH2F *histo2D[nHisto2D];
  
  TH1F *pullsigt, *pullsigz, *pullqz;
  std::vector<double> sigts, sigzs, qzs;
  
  int PPDG;
  std::vector<double> paramsGlobal, paramsLocal;
  int totalGlobal, totalLocal;
  
  double kProtonMass;
  

  CbmMultiscatteringModel(const CbmMultiscatteringModel&);
  CbmMultiscatteringModel& operator=(const CbmMultiscatteringModel&);
  
  MultiscatteringModel *model;
};

#endif