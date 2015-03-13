#ifndef CBMECALANALYSISCALIB_H
#define CBMECALANALYSISCALIB_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class TTree;
class CbmEcalStructure;
class TClonesArray;

class CbmEcalAnalysisCalib : public FairTask
{
public:
  CbmEcalAnalysisCalib(const char* name, const Int_t iVerbose);

  /** Default constructor **/
  CbmEcalAnalysisCalib();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisCalib() {};

private:
  TTree* fTree;
  Double_t fX;
  Double_t fY;
  Double_t fCX;
  Double_t fCY;
  Double_t fP;
  Double_t fCE;
  Double_t fOE;
  Double_t fPX;
  Double_t fPY;
  Double_t fPZ;
  Int_t fEv;
  Int_t fCellNum;
  Int_t fADC;
  void InitTree();
  CbmEcalStructure* fStr;
  TClonesArray* fTracks;

  CbmEcalAnalysisCalib(const CbmEcalAnalysisCalib&);
  CbmEcalAnalysisCalib operator=(const CbmEcalAnalysisCalib&);

  ClassDef(CbmEcalAnalysisCalib,1)
};

#endif 

