/** Reconstruction taking care of correlations.
 ** A special showe lib (CbmEcalShLibCorr) required
 ** Can be used for data production.
 ** The tracks array can be imported from ROOT file. (see CbmEcalTrackExport. )
 ** The output can be exported to tree. 
 ** //Dr.Sys **/

#ifndef CBMECALRECOCORR_H
#define CBMECALRECOCORR_H

#include "FairTask.h"
#include "TString.h"

#include "Math/IFunction.h"
#include "Minuit2/Minuit2Minimizer.h"

class TChain;
class TClonesArray;
class TTree;
class CbmEcalStructure;
class CbmEcalCell;
class CbmEcalSCurveLib;
class CbmEcalInf;
class CbmEcalCluster;
class CbmEcalShLibCorr;
class CbmEcalCalibration;
class CbmEcalRecParticle;
class CbmEcalPosLib;

class FCNEcalClusterCorr : public ROOT::Math::IBaseFunctionMultiDim
{
public:
  FCNEcalClusterCorr(CbmEcalCalibration* cal, CbmEcalShLibCorr* shlib, CbmEcalInf* inf)
    : ROOT::Math::IBaseFunctionMultiDim (), fCal(cal), fShLib(shlib), fInf(inf), fStr(NULL),
    fCluster(NULL), fEStep(1e-4), fCStep(1e-4), fErrorDef(1.0), 
    fClusterEnergy(0.), fClusterResp(0.), fN(0), fCells(), fClsCells(),
    fFixClusterEnergy(0), fNDF(0) 
    {};
  
  CbmEcalCluster* GetCluster() const {return fCluster;}
  void SetCluster(CbmEcalCluster* cluster);
  void SetClusterEnergy(Double_t en) {fClusterEnergy=en;}
  void SetStructure(CbmEcalStructure* str) {fStr=str;}

  void SetParticleSeeds(const std::vector<CbmEcalCell*> cells) {fCells=cells;}

  /** Get/Set number of photons in cluster **/
  void SetN(Int_t n);
  Int_t GetN() const {return fN;}

  void SetNumParam(Int_t i) {fParam=i;}

  /** Get/Set various steps **/
  void SetEStep(Double_t step) {fEStep=step;}
  void SetCStep(Double_t step) {fCStep=step;}
  Double_t GetEStep() const {return fEStep;}
  Double_t GetCStep() const {return fCStep;}
  /** NDF of current fit **/
  Int_t NDF() const {return fNDF;}

  /** The chi2 stuff **/
  /** par[n] --- energy, par[n+1] --- x, par[n+2] --- y **/
  Double_t DoEval(const Double_t* x) const;

  unsigned int NDim() const
  {
    return fParam;
  }
  
  ROOT::Math::IBaseFunctionMultiDim* Clone() const
    {
      return new FCNEcalClusterCorr(fCal, fShLib, fInf);
    }

  void SetFixClusterEnergy(Int_t fix) {fFixClusterEnergy=fix;}
  Double_t ClusterEnergy() const {return fClusterEnergy;}
  Double_t Correlation(CbmEcalCell* c1, CbmEcalCell* c2, Double_t x, Double_t y, Double_t phi, Double_t theta, Double_t e) const;

  ~FCNEcalClusterCorr() {};
private:
  /** A calibration object **/
  CbmEcalCalibration* fCal; 		//!
  /** A shower library **/
  CbmEcalShLibCorr* fShLib;			//!
  /** Calorimeter information **/
  CbmEcalInf* fInf;			//!
  /** Calorimeter structure **/
  CbmEcalStructure* fStr;		//!
  /** Calorimeter cluster **/
  CbmEcalCluster* fCluster;		//!

  /** Steps if gradient business **/
  Double_t fEStep;
  Double_t fCStep;
  /** A stuff from FCNBase **/
  Double_t fErrorDef;
  /** Energy of cluster **/
  Double_t fClusterEnergy;
  /** Response of cluster **/
  Double_t fClusterResp;
  /** Number of photons in cluster **/
  Int_t fN;
  /** Seed cells of cluster **/
  std::vector<CbmEcalCell*> fCells;	//!
  /** All cells of the cluster **/
  std::vector<CbmEcalCell*> fClsCells;	//!
  /** Fix sum of energies of cluster particles to energy of cluster **/
  Int_t fFixClusterEnergy;
  /** NDF of fit **/
  Int_t fNDF;
  /** Number of parameters **/
  Int_t fParam;

  FCNEcalClusterCorr(const FCNEcalClusterCorr&);
  FCNEcalClusterCorr& operator=(const FCNEcalClusterCorr&);
};

class CbmEcalRecoCorr : public FairTask
{
public:
  /** Default constructor. Requirement of ROOT system **/
  CbmEcalRecoCorr();

  /** Standard constructor **/
  CbmEcalRecoCorr(const char *name, const Int_t iVerbose=1, const char* configname="none");

  /** Destructor **/
  virtual ~CbmEcalRecoCorr();

  /** Init **/
  virtual InitStatus Init();
  virtual void Exec(Option_t* option);
  virtual void Finish();

  /** Tools for debugging **/
  void SetOutToTree(Bool_t totree=kTRUE) {fToTree=totree;}
  void SetRecoName(const char* name) {fRecoName=name;}
  TString GetRecoName() const {return fRecoName;}
  void SetStoreClusterInfo(Bool_t store) {fStoreClusterInfo=store;}
private:
  /** Reconstruct photon from maximum **/
  void Reco(CbmEcalCell* cell, CbmEcalCluster* clstr);
  /** Reconstruct photon from maximum. PosLib instead of S-curves **/
  void Reco2(CbmEcalCell* cell, CbmEcalCluster* clstr);
  /** Fit a given cluster. A first approximation should be available **/
  void FitCluster(CbmEcalCluster* clstr);
  /** Write a cluster info **/
  void WriteClusterInfo(CbmEcalCluster* clstr);
  /** Calculate a chi2 for just reconstructed photons **/
  Double_t CalculateChi2(CbmEcalCluster* cluster);
  /** Fit a cluster. A**/
  Double_t FitCluster();
  /** Number of reconstructed tracks **/
  Int_t fN;
  Int_t fNOld;

  CbmEcalCell** fMaximums;	//!
  CbmEcalCell** fExcluded;	//!

  /** Event number **/
  Int_t fEventN;
  /** Calorimeter structure **/
  CbmEcalStructure* fStr;	//!
  /** An information about calorimeter **/
  CbmEcalInf* fInf;		//!
  /** S-curve library **/
  CbmEcalSCurveLib* fLib;	//!
  /** A position library **/
  CbmEcalPosLib* fPosLib;	//!
  /** A shower library **/
  CbmEcalShLibCorr* fShLib;		//!
  /** A calibration of the calorimeter **/
  CbmEcalCalibration* fCal;	//!

  /** Array of tracks **/
  TClonesArray* fTracks;	//!
  /** Array of reconstructed particles **/
  TClonesArray* fReco;		//!
  /** Array of clusters **/
  TClonesArray* fClusters;	//!
  /** Name of output tree **/
  TString fRecoName;

  /** A tool, to export reconstructed particles to file. Should be moved to
   ** a separate task in future. For debugging now. **/
  Bool_t fToTree;
  TTree* fOutTree;		//!
  Bool_t fStoreClusterInfo;
  void CreateTree();
  void FillTree();
  Short_t fType;
  Short_t fDiffType;
  Double_t fE;
  Double_t fE3x3;
  Double_t fE2x2;
  Double_t fEReco;
  Double_t fXReco;
  Double_t fYReco;
  Double_t fECls;
  Double_t fE2Cls;
  Double_t fE3Cls;
  Double_t fECluster;
  Double_t fAX;
  Double_t fAY;
  Double_t fCellX;
  Double_t fCellY;
  Double_t fChi2;
  Double_t fTheta[50];
  Short_t fTypes[50];
  Double_t fEmeas[50];
  Double_t fEpred[50];
  Double_t fChi2Fit[50];
  Double_t fEfirst[50];
  Double_t fChi2First[50];
  Double_t fCX[50];
  Double_t fCY[50];
  Int_t fCMaxs;

  /** A threshold for chi^2. Is chi^2<fChi2Th no fitting **/
  Double_t fChi2Th;
  Double_t fEStep;
  Double_t fCStep;
  /** Maximum number of photons per cluster for reconstruction
   ** If cluster contains more local maximums, it rejected. **/
  Int_t fMaxPhotonsPerCluster;
  /** Maximum number of iterations in fitting process **/
  Int_t fMaxIterations;
  /** Fix sum of energies of cluster particles to energy of cluster **/
  Int_t fFixClusterEnergy;
  /** A chi^2 and gradient function for cluster/particles **/
  FCNEcalClusterCorr* fFCN;		//!
  /** A fitter **/
  ROOT::Minuit2::Minuit2Minimizer* fFitter;
  /** Minimum energy of precluster maximum for consideration **/
  Double_t fMinMaxE;
  /** Use a position library instead of S-curves **/
  Int_t fUsePosLib;

  /** Energy deposition in precluster **/
  Double_t fPreE;
  /** Region **/
  Int_t fRegion;

  CbmEcalRecoCorr(const CbmEcalRecoCorr&);
  CbmEcalRecoCorr& operator=(const CbmEcalRecoCorr&);

  ClassDef(CbmEcalRecoCorr, 1)
};

#endif

