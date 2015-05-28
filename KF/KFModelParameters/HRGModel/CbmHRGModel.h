/*
 *====================================================================
 *
 *  CBM Hadron Resonance Gas
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  Statistical hadron-resonance gas model calculations
 *
 *====================================================================
 */
 
#ifndef _CbmHRGModel_h_
#define _CbmHRGModel_h_
 
#include "../CbmModelBase.h"
#include "../KFQA/CbmKFTrErrMCPoints.h"
#include "TMath.h"
#include <vector>
#include <algorithm>
#include <map>
#include "CbmBilinearSplineFunction.h"
#include "ThermalModelFit.h"
 
class TClonesArray;
class CbmVertex;
class TDirectory;
class TH1F;
class TH2F;
class TGraphErrors;
class TSpline3;
class FairRootManager;

namespace HRGModelNamespace {
	
	struct AcceptanceFunction {
	  Double_t dy, dpt;
	  vector<Double_t> ys, pts, probs;
	  BilinearSplineFunction sfunc;
	  void setSpline() { sfunc.setData(ys, pts, probs); }
	  Double_t getAcceptance(const Double_t & y, const Double_t & pt) const;
	  AcceptanceFunction():dy(), dpt(), ys(), pts(), probs(), sfunc() { }
	};
	
	struct ReconstructionEfficiencyFunction {
	  Double_t p1, p2, p3;
	  ReconstructionEfficiencyFunction(double p1_=1., double p2_=0., double p3_=1.):p1(p1_), p2(p2_), p3(p3_) { }
	  Double_t f(double p) { 
		return p1 - p2 * TMath::Exp(-p*p/2./p3/p3); 
	  }
	};
}

using namespace HRGModelNamespace;

class ThermalModelBase;

class CbmHRGModel: public CbmModelBase
{
 public:
 //static const int p_sz = 8;
  CbmHRGModel(Int_t recoLevel = -1,	// -1 - MC Tracks, 0 - Reconstructible MC Tracks, 1 - Reco Tracks with MC Data, 2 - Reco Tracks with Reco Momentum and MC Primary, 3 - Reco Tracks with Reco Momentum and Reco Primary
			   //Int_t usePID = 1,	// 1 - MC, 2 - TOF
			   //Int_t trackNumber = 1,
			   Int_t iVerbose = 1,
			   TString Mode="MC",
			   Int_t EventStats=1,
			   KFParticleTopoReconstructor* tr=0,
			   Bool_t useWidth=0,
			   Bool_t useStatistics=0,
			   Double_t rad=0.);
  ~CbmHRGModel();

  virtual void ReInit(FairRootManager *fManger);
  virtual void Init();
  virtual void Exec();
  virtual void Finish();
  
  void AddRatio(int pdgid1, int pdgid2, /*TString name1="", TString name2="",*/ double SystError = 0.);

  ClassDef(CbmHRGModel,1);

 private:

  
  void ReadAcceptanceFunction(HRGModelNamespace::AcceptanceFunction & func, TString filename);
  
  Double_t RecEfficiency(Double_t p);
  bool checkIfReconstructable(CbmKFTrErrMCPoints *inTrack);	// 0 - No, 1 - Yes
  
  void CalculateMultiplicitiesInEvent(std::vector<int> & Mult, int RecoLevel, bool UpdateGlobal=0);
  
  ThermalModelFitParameters GetThermalParameters(const std::vector<int> & Mults);

  Float_t ekin;
  Float_t ycm;
  Bool_t fUpdate;
  Int_t fusePID;
  Int_t fRecoLevel;
  Int_t fTrackNumber;	// 0 - test calculation, only Pions, 1 - UrQMD Au+Au
  Int_t fEventStats;
  Int_t events;
  
  Bool_t fUseWidth, fUseStatistics;
  double fRadius;
  

  TString fModeName;
  TString outfileName;
  
  TDirectory* histodir;
  
  TClonesArray* flistMCTracks;
  
  //Int_t events;
  
  static const int nHisto1D = 15; //T,errT,muB,errmuB,muS,errmuS,muQ,errmuQ,nB,energy,entropy,pressure,energy per particle,etaovers,chi2fit
  int IndexTemperature, IndexErrorTemperature, IndexMuB, IndexErrorMuB, IndexMuS, IndexErrorMuS, IndexMuQ, IndexErrorMuQ,
	    IndexnB, IndexEnergy, IndexEntropy, IndexPressure, IndexEoverN, IndexEtaoverS, IndexChi2Fit;
  
  TH1F *histo1D[nHisto1D];
  
  static const int nHisto2D = 3; //T-muB,T-nB,T-energy
  int IndexTmuB, IndexTnB, IndexTE;
  
  TH2F *histo2D[nHisto2D];
  
  TGraphErrors *grTmuB;
  TH1F *pullT, *pullmuB;
  std::vector<double> Ts, muB, errT, errmuB;
  
  std::map<int, int> PDGtoIndex;
  std::vector<int> ParticlePDGsTrack;
  std::vector<TString> ParticleNames;
  std::vector< std::pair<int, int> > RatiosToFit;
  std::vector<double> SystErrors;
  std::vector<int> MultGlobal, MultLocal;
  
  HRGModelNamespace::AcceptanceFunction AcceptanceSTS, AcceptanceSTSTOF;

  CbmHRGModel(const CbmHRGModel&);
  CbmHRGModel& operator=(const CbmHRGModel&);
  
  ThermalParticleSystem TPS;
  
  ThermalModelBase *model;

};

#endif