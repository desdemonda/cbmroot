#ifndef CBMECALANALYSISBSH_H
#define CBMECALANALYSISBSH_H

#include "FairTask.h"

class TTree;
class TClonesArray;
class CbmEcalStructure;
class CbmEcalCell;
class CbmEcalPreCluster;
class CbmEcalCalibration;
class CbmEcalInf;

class CbmEcalAnalysisBSh : public FairTask
{
public:
  CbmEcalAnalysisBSh(const char* name, const Int_t iVerbose=0, const char* cfgname="ana.cfg");
public:
  /** Default constructor **/
  CbmEcalAnalysisBSh();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisBSh() {};
private:
  /** Initialize a tree **/
  void InitTree();
  /** Get region number of the cell **/
  Int_t GetRegion(CbmEcalCell* cls);

  Double_t GetPH(Double_t e, Double_t theta);
  Double_t GetPL(Double_t e, Double_t theta);
  /** A default algorithm of precluster formation **/
  void FormPreCluster(CbmEcalCell* cell);
  /** A new algorithm of precluster formation **/
  void FormPreClusterNew(CbmEcalCell* cell);
  /** Find a common area of ellipse and cell**/
  Double_t SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy, Int_t reg);
  /** Out tree **/
  TTree* fTree;			//!
  Double_t GetEnergy(Double_t e, CbmEcalCell* cell);

  Double_t fX;
  Double_t fY;
  Double_t fEMC;
  Double_t fE;
  Double_t fTh;
  Double_t fERes;


  Int_t fCurAlgo;

  Int_t fRemoveCharged;
  Int_t fUseCalibration;
  Int_t fRegions;
  Int_t fCalibration;
  Int_t fIntSteps;
  Int_t fPreClusterAlgo[10];
  Double_t fMinClusterE[10];
  Double_t fMinMaxE[10];
  Double_t fTheta[10];
  Double_t fPar1[10];
  Double_t fPar2[10];
  Double_t fPar3[10];
  Int_t fParI[10];
  /** MC tracks array **/
  TClonesArray* fMC;		//!
  TClonesArray* fMCTr;		//!
  /** Calorimeter structure **/
  CbmEcalStructure* fStr;	//!
  CbmEcalCalibration* fCal;	//!
  CbmEcalInf* fInf;

  CbmEcalAnalysisBSh(const CbmEcalAnalysisBSh&);
  CbmEcalAnalysisBSh& operator=(const CbmEcalAnalysisBSh&);

  ClassDef(CbmEcalAnalysisBSh,1)
};

#endif 

