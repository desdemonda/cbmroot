/*
 *====================================================================
 *
 *  CBM Thermal Model, No Flow. This module is obsolete, to be completely replaced by CbmBoltzmannDistribution
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  Boltzmann distribution calculations
 *
 *====================================================================
 */
 
#ifndef _CbmThermalModelNoFlow_h_
#define _CbmThermalModelNoFlow_h_
 
#include "CbmModelBase.h"
#include "../KFQA/CbmKFTrErrMCPoints.h"
#include "TMath.h"
#include <vector>
#include "CbmBilinearSplineFunction.h"
 
class TClonesArray;
class CbmVertex;
class TDirectory;
class TH1F;
class TH2F;
class TSpline3;
class FairRootManager;

namespace ThermalModelNoFlowNamespace {
	
	struct AcceptanceFunction {
	  Double_t dy, dpt;
	  vector<Double_t> ys, pts, probs;
	  BilinearSplineFunction sfunc;
	  void setSpline() { sfunc.setData(ys, pts, probs); }
	  Double_t getAcceptance(const Double_t & y, const Double_t & pt) const;
	  AcceptanceFunction(): dy(), dpt(), ys(), pts(), probs(), sfunc() { }
	};
	
	struct ReconstructionEfficiencyFunction {
	  Double_t p1, p2, p3;
	  ReconstructionEfficiencyFunction(double p1_=1., double p2_=0., double p3_=1.):p1(p1_), p2(p2_), p3(p3_) { }
	  Double_t f(double p) { 
		return p1 - p2 * TMath::Exp(-p*p/2./p3/p3); 
	  }
	};
}

using namespace ThermalModelNoFlowNamespace;

class CbmThermalModelNoFlow: public CbmModelBase
{
 public:
 static const int p_sz = 2;
  CbmThermalModelNoFlow(Float_t ekin_ = 24.08,
					   Int_t recoLevel = 3,
                       Int_t usePID = 1,
					   Int_t trackNumber = 1,
                       Int_t iVerbose = 1);
  ~CbmThermalModelNoFlow();

  virtual void ReInit(FairRootManager *fManger);
  virtual void Init();
  virtual void Exec();
  virtual void Finish();

  ClassDef(CbmThermalModelNoFlow,1);

 private:
   

  void ReadAcceptanceFunction(ThermalModelNoFlowNamespace::AcceptanceFunction & func, TString filename);
  
  Double_t RecEfficiency(Double_t p);
  Double_t ThermalMt(double T, double m);
  Double_t ThermalMt2(double T, double m);
  Double_t ThermalMtAcc(double T, double m, const ThermalModelNoFlowNamespace::AcceptanceFunction & func);
  Double_t ThermalMt2Acc(double T, double m, const ThermalModelNoFlowNamespace::AcceptanceFunction & func);
  Double_t NFracAcc(double T, double m, const ThermalModelNoFlowNamespace::AcceptanceFunction & func);
  
  Double_t chi2func(double y, double pt, double m);
  Double_t tempCrit(double y, double pt, double m);
  
  Double_t tempFunction(double T, double mt, double mo);
  Double_t tempDerivFunction(double T, double dT, double mt, double mo);
  Double_t getInverseSlope(double mt, double mo, int iters);
  
  void ComputeThermalDependence(Int_t part=0);
//  Double_t getInverseSlope(double mt, double mo, int iters);
  Double_t getRadius(double T, double N, double mo);
  Double_t getRadiusCor(double T, double N, double mo, TSpline3 *Np);
  Double_t getRadiusDerivT(double T, double N, double mo);
  Double_t getRadiusDerivTCor(double T, double N, double mo, TSpline3 *Np);
  Double_t getRadiusDerivN(double T, double N, double mo);
  Double_t getRadiusDerivNCor(double T, double N, double mo, TSpline3 *Np);
  Double_t getTemperature(double mt, double mo, int iters);
  Double_t getTemperatureAll(double mt, int part, int iters);
  Double_t getTemperatureDerivAll(double mt, int part, int iters);
  Double_t getTemperatureAllCor(double mt, int part, int iters, TSpline3 *Np);
  Double_t getTemperatureDerivAllCor(double mt, int part, int iters, TSpline3 *Np);
  Double_t getTemperatureRapidity(double mt, double mo, int iters, double y = 0.);
  Double_t getDerivTemperature(double mt, double mo, double T, int iters);
  Double_t getMtErrorSquare(double qp, double Tx, double Ty, double covMatrix[], double mo);
  bool checkIfReconstructable(CbmKFTrErrMCPoints *inTrack);	// 0 - No, 1 - Yes

//  Float_t fCuts[2][3];
  Float_t ekin;
  Float_t ycm;
  Bool_t fUpdate;
  Int_t fusePID;
  Int_t fRecoLevel;
  Int_t fTrackNumber;	// 0 - test calculation, only Pions, 1 - UrQMD Au+Au

  TClonesArray *flistStsPts;
  TClonesArray *flistTofPts;
  //TClonesArray *flistMvdPts;
  TClonesArray* flistStsTracksMatch;
  TClonesArray* flistStsTracks;
  CbmVertex* fPrimVtx;
  

  
  TString outfileName;

//  std::vector<KFParticle> fParticles;
  std::vector<float> fChiToPrimVtx;
  
  //TSpline3 **mtTall, **mtTacc, **Npart, **mtTsts, **Npartsts, **mtTststof, **Npartststof;
  TSpline3 *mtTall[p_sz], *mtTacc[p_sz], *Npart[p_sz], *mtTsts[p_sz], *Npartsts[p_sz], *mtTststof[p_sz], *Npartststof[p_sz];
  
  
  /*Double_t *globalmtavMC;
  Double_t *globalmt2avMC;
  Double_t *globalfavMC;
  Double_t *globalf2avMC;
  Double_t *globalmtmomerrMC;
  Int_t *globalnTracksMC;
  Double_t *globalmtavReco;
  Double_t *globalmt2avReco;
  Double_t *globalfavReco;
  Double_t *globalf2avReco;
  Double_t *globalmtmomerrReco;
  Int_t *globalnTracksReco;*/
  Double_t globalmtavMC[p_sz];
  Double_t globalmt2avMC[p_sz];
  Double_t globalfavMC[p_sz];
  Double_t globalf2avMC[p_sz];
  Double_t globalmtmomerrMC[p_sz];
  Int_t globalnTracksMC[p_sz];
  Double_t globalmtavReco[p_sz];
  Double_t globalmt2avReco[p_sz];
  Double_t globalfavReco[p_sz];
  Double_t globalf2avReco[p_sz];
  Double_t globalmtmomerrReco[p_sz];
  Int_t globalnTracksReco[p_sz];
  //Double_t *globalmtavRecoCor;
  //Double_t *globalnTracksRecoCor;
  
  
  Int_t NPrimGlobalMC, NPrimGlobalReco;
  TClonesArray* flistMCTracks;
  TClonesArray* flsitGlobalTracks;
  TClonesArray* flistTofHits;
  TClonesArray* flistTofPoints;
//  TClonesArray* flistRichRings;
//  TClonesArray* flistTrdTracks;
  
  TDirectory* histodir;
  
  Int_t events;
  
  TH1F *hTempFullMC[p_sz];
  TH1F *hTempFullReco[p_sz];
  TH1F *hTempFullRecoCor[p_sz];
  TH1F *hTempErrStatFullMC[p_sz];
  TH1F *hTempErrStatFullReco[p_sz];
  TH1F *hTempErrStatFullRecoCor[p_sz];
  TH1F *hTempErrMomFullMC[p_sz];
  TH1F *hTempErrMomFullReco[p_sz];
  TH1F *hTempErrMomFullRecoCor[p_sz];
  TH1F *hRadFullMC[p_sz];
  TH1F *hRadFullReco[p_sz];
  TH1F *hRadFullRecoCor[p_sz];
  TH1F *hRadErrStatFullMC[p_sz];
  TH1F *hRadErrStatFullReco[p_sz];
  TH1F *hRadErrStatFullRecoCor[p_sz];
  TH1F *hRadErrMomFullMC[p_sz];
  TH1F *hRadErrMomFullReco[p_sz];
  TH1F *hRadErrMomFullRecoCor[p_sz];
  
  TH1F *hfyMC[p_sz];
  TH1F *hfyMCmodel[p_sz];
  TH2F *hfdndydptMC[p_sz];
  TH2F *hfdndydptMCmodel[p_sz];
  
  TH1F *hfyReco[p_sz];
  TH1F *hfyRecomodel[p_sz];
  TH2F *hfdndydptReco[p_sz];
  TH2F *hfdndydptRecomodel[p_sz];
  
  TH1F *hfyRecoCor[p_sz];
  TH1F *hfyRecoCormodel[p_sz];
  TH2F *hfdndydptRecoCor[p_sz];
  TH2F *hfdndydptRecoCormodel[p_sz];
  
  ThermalModelNoFlowNamespace::AcceptanceFunction AcceptanceSTS, AcceptanceSTSTOF;

  CbmThermalModelNoFlow(const CbmThermalModelNoFlow&);
  CbmThermalModelNoFlow& operator=(const CbmThermalModelNoFlow&);

};

#endif
