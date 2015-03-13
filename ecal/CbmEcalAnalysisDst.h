#ifndef CBMECALANALYSISDST_H
#define CBMECALANALYSISDST_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class TTree;
class TClonesArray;
class CbmEcalStructure;

class CbmEcalAnalysisDst : public FairTask
{
public:
  CbmEcalAnalysisDst(const char* name, const Int_t iVerbose=0);
public:
  /** Default constructor **/
  CbmEcalAnalysisDst();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisDst() {};
private:
  /** Initialize a tree **/
  void InitTree();
  /** Out tree **/
  TTree* fTree;			//!
  /** Number of found particles in this event **/
  Int_t fEntries;

  /** Event number **/
  Int_t fEvent;
  /** Information about first particle **/
  Double_t fE;
  Double_t fX;
  Double_t fY;
  Double_t fCaloE;
  Double_t fCX;
  Double_t fCY;
  Double_t fP;
  Double_t fQ;
  Double_t fTheta;
  Double_t fPhi;
  Double_t fCTheta;
  Double_t fCPhi;
  Double_t fMX;
  Double_t fMY;
  Double_t fMP;
  Double_t fMQ;

  /** An array of clusters **/
  TClonesArray* fPts;
  /** A calorimeter structure **/
  CbmEcalStructure* fStr;

  CbmEcalAnalysisDst(const CbmEcalAnalysisDst&);
  CbmEcalAnalysisDst& operator=(const CbmEcalAnalysisDst&);

  ClassDef(CbmEcalAnalysisDst,1)
};

#endif 

