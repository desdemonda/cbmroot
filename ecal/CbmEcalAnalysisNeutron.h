#ifndef CBMECALANALYSISNEUTRON_H
#define CBMECALANALYSISNEUTRON_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class TTree;
class TClonesArray;
class CbmEcalStructure;

class CbmEcalAnalysisNeutron : public FairTask
{
public:
  CbmEcalAnalysisNeutron(const char* name, const Int_t iVerbose=0);
public:
  /** Default constructor **/
  CbmEcalAnalysisNeutron();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisNeutron() {};
private:
  /** Initialize a tree **/
  void InitTree();
  /** Out tree **/
  TTree* fTree;			//!
  /** Number of found particles in this event **/
  Int_t fEntries;
  /** Number of particle in event **/
  Int_t fN;

  /** Event number **/
  Int_t fEvent;
  /** Information about first particle **/
  Double_t fE;
  Double_t fPx;
  Double_t fPy;
  Double_t fPz;
  Double_t fX;
  Double_t fY;
  Int_t fCellType;
  Int_t fDiffType;
  Double_t fM3;
  Double_t fMx3;
  Double_t fMy3;
  Double_t fChi2;
  Double_t fM;
  Double_t fMx;
  Double_t fMy;
  Int_t fPdg;
  Int_t fMaxs;

  /** An array of reconstructed particles **/
  TClonesArray* fReco;
  /** MC tracks array **/
  TClonesArray* fMC;
  /** An array of clusters **/
  TClonesArray* fClusters;
  /** A calorimeter structure **/
  CbmEcalStructure* fStr;

  CbmEcalAnalysisNeutron(const CbmEcalAnalysisNeutron&);
  CbmEcalAnalysisNeutron& operator=(const CbmEcalAnalysisNeutron&);

  ClassDef(CbmEcalAnalysisNeutron,1)
};

#endif 

