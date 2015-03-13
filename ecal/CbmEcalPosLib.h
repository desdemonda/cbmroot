#ifndef CBMECALPOSLIB_H
#define CBMECALPOSLIB_H

#include "FairTask.h"

#include "TMath.h"

class CbmEcalInf;

class CbmEcalPosLib : public FairTask
{
public:
  CbmEcalPosLib();
  CbmEcalPosLib(const char* nm, Int_t verb, const char* fname);

  Double_t X() const {return fX;}
  Double_t Y() const {return fY;}

  virtual void Exec(const Option_t* opt);
  virtual InitStatus Init();
  virtual void Finish();

  void GetXY(Double_t e, Double_t x, Double_t y);
  Double_t GetDR(Double_t e, Double_t theta);
  virtual ~CbmEcalPosLib();

private:
  /** Result of (X,Y) approximation **/
  Double_t fX;
  Double_t fY;

  /** Name of parameter file **/
  TString fParName;
  /** Name of container **/
  TString fContName;
  /** Position library **/
  Double_t fF0P0;
  Double_t fF1P0;
  Double_t fF2P0;
  Double_t fF0P1;
  Double_t fF1P1;
  Double_t fF2P1;
  Double_t fF0P2;
  Double_t fF1P2;
  Double_t fF2P2;
  /** Information container of calorimeter **/
  CbmEcalInf* fInf;		//!

  CbmEcalPosLib(const CbmEcalPosLib&);
  CbmEcalPosLib& operator=(const CbmEcalPosLib&);

  ClassDef(CbmEcalPosLib, 1);
};

inline Double_t CbmEcalPosLib::GetDR(Double_t e, Double_t theta)
{
  Double_t f0=fF0P0+e*(fF0P1+fF0P2*e);
  Double_t f1=fF1P0*TMath::Log(e)+fF1P1+fF1P2*e;
  Double_t f2=fF2P0+e*(fF2P1+fF2P2*e);
  return f0+theta*(f1+theta*f2);
}
#endif
