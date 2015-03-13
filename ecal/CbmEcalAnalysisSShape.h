#ifndef CBMECALANALYSISSSHAPE_H
#define CBMECALANALYSISSSHAPE_H

#include "FairTask.h"

#include "TString.h"

class TTree;
class CbmEcalStructure;
class TClonesArray;
class CbmEcalCell;

class CbmEcalAnalysisSShape : public FairTask
{
public:
  CbmEcalAnalysisSShape(const char* name, const Int_t iVerbose);
public:
  /** Default constructor **/
  CbmEcalAnalysisSShape();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisSShape() {};
private:
  void InitTree();
  void InitVariables();
  void FillClusterInfo(CbmEcalCell* cell);
  Double_t SolveEllipse(CbmEcalCell* cell, Double_t cx,Double_t cy);
  TTree* fTree;
  Double_t fE[49];
  Double_t fX[49];
  Double_t fY[49];
  Double_t fP[49];
  Double_t fQ[49];
  Int_t fCls[49];
  Double_t fMaxE;
  Double_t fMaxX;
  Double_t fMaxY;
  Double_t fTX;
  Double_t fTY;
  Double_t fCX;
  Double_t fCY;
  Double_t fTTheta;
  Double_t fTPhi;
  Int_t fEv;

  TClonesArray* fMCPoints;		//!
  CbmEcalStructure* fStr;		//!

  CbmEcalAnalysisSShape(const CbmEcalAnalysisSShape&);
  CbmEcalAnalysisSShape& operator=(const CbmEcalAnalysisSShape&);

  ClassDef(CbmEcalAnalysisSShape,1)
};

#endif 

