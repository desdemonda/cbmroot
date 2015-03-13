#ifndef CBMECALANALYSISSHAPE_H
#define CBMECALANALYSISSHAPE_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class TTree;
class CbmEcalInf;
class TClonesArray;

class CbmEcalAnalysisShape: public FairTask
{
public:
  CbmEcalAnalysisShape(const char* name, const Int_t iVerbose, const char* fname);
public:
  /** Default constructor **/
  CbmEcalAnalysisShape();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisShape();
/*
  void SetE(Double_t e)  {fE=e;}
  void SetPhi(Double_t phi)  {fPhi=phi;}
  void SetTheta(Double_t theta)  {fTheta=theta;}
*/
  void SetXSize(Int_t xsize) {fXSize=xsize;}
  void SetYSize(Int_t ysize) {fYSize=ysize;}
  void SetDX(Double_t dx) {fDX=dx;}
  void SetDY(Double_t dy) {fDY=dy;}
//  void SetStartX(Double_t startx) {fStartX=startx;}
//  void SetStartY(Double_t starty) {fStartY=starty;}
  void SetWriteEach(Bool_t writeeach) {fWriteEach=writeeach;}
  Int_t GetNum(Double_t x,Double_t y) const;
private:
  void InitTree();
  void InitE();
  TString fName;

  Double_t fE;
  Double_t fPhi;
  Double_t fTheta;
  Double_t* fShape;			//!
  Float_t* fShapeOut;			//!
  Int_t fXSize;
  Int_t fYSize;
  Int_t fSize;
  Double_t fDX;
  Double_t fDY;
  Double_t fStep;
  Double_t fStartX;
  Double_t fStartY;
  Bool_t fWriteEach;

  Int_t fEv;

  TTree* fTree;				//!
  CbmEcalInf* fInf;			//!
  TClonesArray* fMC;			//!
  TClonesArray* fP;			//!
  TClonesArray* fMCTracks;		//!

  CbmEcalAnalysisShape(const CbmEcalAnalysisShape&);
  CbmEcalAnalysisShape& operator=(const CbmEcalAnalysisShape&);

  ClassDef(CbmEcalAnalysisShape,1)
};

#endif 

