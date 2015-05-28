/*
 *====================================================================
 *
 *  CBM impact parameter extraction
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  Extraction of event impact parameter from total participant electric charge, based on Glauber model predictions
 *
 *====================================================================
 */
 
#ifndef _CbmImpactParameterModel_h_
#define _CbmImpactParameterModel_h_
 
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
class FairMCEventHeader;

class ImpactParameterModel;

class CbmImpactParameterModel: public CbmModelBase
{
 public:
 //static const int p_sz = 8;
  CbmImpactParameterModel(Int_t recoLevel = -1,	// -1 - MC Tracks, 0 - Reconstructible MC Tracks, 1 - Reco Tracks with MC Data, 2 - Reco Tracks with Reco Momentum and MC Primary, 3 - Reco Tracks with Reco Momentum and Reco Primary
			   //Int_t usePID = 1,	// 1 - MC, 2 - TOF
			   //Int_t trackNumber = 1,
			   Int_t iVerbose = 1,
			   TString Mode = "MC",
			   //Int_t EventStats=1,
			   KFParticleTopoReconstructor* tr=0,
			   Float_t	ekin_ = 25.,
			   TString InputTable = "Npvsb-AuAu.dat");
  ~CbmImpactParameterModel();

  virtual void ReInit(FairRootManager *fManger);
  virtual void Init();
  virtual void Exec();
  virtual void Finish();

  ClassDef(CbmImpactParameterModel,1);

 private:
  
  int CalculateTotalChargeInEvent(int RecoLevel);
  
//  Float_t fCuts[2][3];
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
  FairMCEventHeader *MCEvent;
  
  static const int nHisto1D = 9; //b, Nwp, Npe, bMC, bpe, bpeMC, bdiff, bres, bpull
  int Indexb, IndexNwp, IndexNpe, IndexbMC, Indexbpe, IndexbpeMC, Indexbdiff, Indexbres, Indexbpull;
  
  TH1F *histo1D[nHisto1D];
  
  int totalEvents;
  int PPDG;
  
  double kProtonMass;
  double fCor;
  

  CbmImpactParameterModel(const CbmImpactParameterModel&);
  CbmImpactParameterModel& operator=(const CbmImpactParameterModel&);
  
  ImpactParameterModel *model;
};

#endif