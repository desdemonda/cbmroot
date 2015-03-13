#ifndef CBMECALANALYSISCLUSTER_H
#define CBMECALANALYSISCLUSTER_H

#include "FairTask.h"

class TTree;
class TClonesArray;
class CbmEcalStructure;
class CbmEcalCell;

class CbmEcalAnalysisCluster : public FairTask
{
public:
  CbmEcalAnalysisCluster(const char* name, const Int_t iVerbose=0, const char* cfgname="ana.cfg");
public:
  /** Default constructor **/
  CbmEcalAnalysisCluster();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisCluster() {};
private:
  /** Build a cluster information **/
  void BuildCluster(CbmEcalCell* cell, Int_t clusters, Double_t x, Double_t y);
  /** Build a cluster information **/
  void BuildCluster2(CbmEcalCell* cell, Int_t cls, Double_t x, Double_t y);
  /** Build a cluster information **/
  void BuildCluster3(CbmEcalCell* cell, Int_t cls, Double_t x, Double_t y);
  /** Initialize a tree **/
  void InitTree();
  /** Out tree **/
  TTree* fTree;			//!
  Double_t SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy);

  /** Event number **/
  Int_t fEvent;
  Double_t fX;
  Double_t fY;
  Double_t fZ;
  Double_t fEFull;
  Double_t fEMax;
  Double_t fE2;
  Double_t fE3;
  Double_t fE4;
  Double_t fE5;
  Double_t fE5_2;
  Double_t fGX;
  Double_t fGY;
  Double_t fGX2;
  Double_t fGY2;
  Double_t fMX;
  Double_t fMY;
  Double_t fTheta;
  Double_t fPhi;
  Int_t fClsSize;
  Double_t* fClsX;		//!
  Double_t* fClsY;		//!
  Double_t* fClsP;		//!
  Double_t* fClsQ;		//!
  Double_t* fClsE;		//!
  Double_t* fClsSE;		//!
  Int_t* fClsN;			//!
  Double_t* fClsS;		//!
  Int_t fCSize;
  Double_t fCE;
  Double_t fCE2;
  Int_t fCSize2;
  Double_t fCE3;
  Int_t fCSize3;
  Int_t fC2ParI;
  Double_t fC2Par1;
  Double_t fC2Par2;
  Double_t fC2Par3;


  /** MC tracks array **/
  TClonesArray* fMC;		//!
  /** Calorimeter structure **/
  CbmEcalStructure* fStr;	//!

  CbmEcalAnalysisCluster(const CbmEcalAnalysisCluster&);
  CbmEcalAnalysisCluster& operator=(const CbmEcalAnalysisCluster&);

  ClassDef(CbmEcalAnalysisCluster,1)
};

#endif 

