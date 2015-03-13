#ifndef CBMECALANALYSISPAIR_H
#define CBMECALANALYSISPAIR_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class TTree;
class TClonesArray;
class CbmEcalStructure;


class CbmEcalAnalysisPair : public FairTask
{
public:
  CbmEcalAnalysisPair(const char* name, const Int_t iVerbose=0);
public:
  /** Default constructor **/
  CbmEcalAnalysisPair();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisPair() {};
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
  Double_t fE1;
  Double_t fPx1;
  Double_t fPy1;
  Double_t fPz1;
  Double_t fX1;
  Double_t fY1;
  Double_t fCellX1;
  Double_t fCellY1;
  Int_t fCellType1;
  Double_t fChi1;

  /** Information about second particle **/
  Double_t fE2;
  Double_t fPx2;
  Double_t fPy2;
  Double_t fPz2;
  Double_t fX2;
  Double_t fY2;
  Double_t fCellX2;
  Double_t fCellY2;
  Int_t fCellType2;
  Double_t fChi2;
  Double_t fR;
  Int_t fCellN;
  Int_t fCells;
  Double_t fM;
  Double_t fMx;
  Double_t fMy;


  /** An array of reconstructed particles **/
  TClonesArray* fReco;
  /** MC tracks array **/
  TClonesArray* fMC;
  /** An array of clusters **/
  TClonesArray* fClusters;
  /** Calorimeter structure **/
  CbmEcalStructure* fStr;		//!

  CbmEcalAnalysisPair(const CbmEcalAnalysisPair&);
  CbmEcalAnalysisPair& operator=(const CbmEcalAnalysisPair&);

  ClassDef(CbmEcalAnalysisPair,1)
};

#endif 

