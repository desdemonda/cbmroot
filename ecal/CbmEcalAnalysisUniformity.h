#ifndef CBMECALANALYSISUNIFORMITY_H
#define CBMECALANALYSISUNIFORMITY_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class TTree;
class TClonesArray;
class CbmEcalStructure;

class CbmEcalAnalysisUniformity : public FairTask
{
public:
  CbmEcalAnalysisUniformity(const char* name, const Int_t iVerbose=0);
public:
  /** Default constructor **/
  CbmEcalAnalysisUniformity();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisUniformity() {};
private:
  /** Initialize a tree **/
  void InitTree();
  /** Out tree **/
  TTree* fTree;			//!
  /** Number of found particles in this event **/
  Int_t fEntries;
  /** Number of particle in event **/
  Int_t fN;

  /** Information about first particle **/
  Double_t fE;
  Double_t fX;
  Double_t fY;

  /** Calorimeter structure **/
  CbmEcalStructure* fStr;
  /** An array points before the calorimeter**/
  TClonesArray* fPoints;

  CbmEcalAnalysisUniformity(const CbmEcalAnalysisUniformity&);
  CbmEcalAnalysisUniformity& operator=(const CbmEcalAnalysisUniformity&);

  ClassDef(CbmEcalAnalysisUniformity,1)
};

#endif 

