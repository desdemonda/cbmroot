#ifndef CBMECALANALYSISRE_H
#define CBMECALANALYSISRE_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class TTree;
class CbmEcalStructure;
class TClonesArray;

class CbmEcalAnalysisRe : public FairTask
{
public:
  CbmEcalAnalysisRe(const char* name, const Int_t iVerbose);
public:
  /** Default constructor **/
  CbmEcalAnalysisRe() {};

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisRe() {};

  void SetD(Double_t d) {fD=d;}
private:
  TTree* fTree;
  Double_t fOut[81];
  Double_t fX[81];
  Double_t fY[81];
  Double_t fMCX;
  Double_t fMCY;
  void InitTree();
  Int_t fS;
  Double_t fD;
  Double_t fSX;
  Double_t fSY;
  CbmEcalStructure* fStr;
  TClonesArray* fMCPoints;

  ClassDef(CbmEcalAnalysisRe,1)
};

#endif 

