/* $Id: CbmEcalDrawer2.h,v 1.3 2006/07/14 15:11:51 prokudin Exp $ */
/** CbmEcalDrawer2 
 ** ECAL drawer. Only for calorimeters with constant granularity
 ** Use TH2D for drawing
 **/
#ifndef CBMECALDRAWER2_H
#define CBMECALDRAWER2_H

#include "FairTask.h"

#include <list>

class CbmEcalStructure;
class TH2D;
class TCanvas;
class CbmEcalCell;

class CbmEcalDrawer2: public FairTask
{
public:
  /** Default constructor **/
  CbmEcalDrawer2();
  CbmEcalDrawer2(const char* name, const char* title="ECAL drawer");
  virtual ~CbmEcalDrawer2();
  virtual InitStatus Init();
  virtual void Exec(Option_t* option);
  virtual void Finish();
  void SetDrawOpt(const char* opt="colz") {fDrawOpt=opt;}
  void SetNamePrefix(const char* prefix="ecal") {fNamePrefix=prefix;}
private:
  CbmEcalStructure* fStr;			//!
  /** Canvas to draw **/
  TCanvas* fC;					//!
  TH2D* fOut;					//!
  Int_t fEvent;
  TString fDrawOpt;
  TString fNamePrefix;
  TClonesArray* fMCPoints;			//!

  Double_t SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy);
  void ConstructCluster(CbmEcalCell* max);
  std::list<CbmEcalCell*> fCluster;		//!
  Double_t fClusterE;
  Int_t fParI;
  Int_t fIntSteps;
  Double_t fPar1;
  Double_t fPar2;
  Double_t fPar3;

  CbmEcalDrawer2(const CbmEcalDrawer2&);
  CbmEcalDrawer2& operator=(const CbmEcalDrawer2&);

  ClassDef(CbmEcalDrawer2,1);
};

#endif
