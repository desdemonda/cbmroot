#ifndef CBMECALANALYSISERES_H
#define CBMECALANALYSISERES_H

#include "FairTask.h"

#include "TString.h"

class TClonesArray;
class TTree;

class CbmEcalAnalysisEres : public FairTask
{
public:
  CbmEcalAnalysisEres(const char* name, const Int_t iVerbose);
public:
  /** Default constructor **/
  CbmEcalAnalysisEres();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisEres() {};
private:
  void InitTree();
  /** Array of maximums **/
  TClonesArray* fMax;		//!
  TTree* fTree;			//!
  
  Int_t fCellType;
  Double_t fE;

  Int_t fEv;

  CbmEcalAnalysisEres(const CbmEcalAnalysisEres&);
  CbmEcalAnalysisEres& operator=(const CbmEcalAnalysisEres&);

  ClassDef(CbmEcalAnalysisEres,1)
};

#endif 

