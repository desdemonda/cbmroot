/** A very simple calorimeter calibration container.
 ** Paramers now hardcoded. This is BAD. Waiting for general
 ** parameter hadlning in the framework. **/

#ifndef CBMECALCALIBRATIONV2_H
#define CBMECALCALIBRATIONV2_H

#include "CbmEcalCalibration.h"
#include "TMath.h"
#include "TString.h"

// #include <iostream>

// using namespace std;

class CbmEcalCell;
class CbmEcalInf;


class CbmEcalCalibrationV2 : public CbmEcalCalibration
{
public:
  /** Standard constructor **/
  CbmEcalCalibrationV2(const char *name, const Int_t iVerbose, const char* parname);
  /** Default constructor **/
  CbmEcalCalibrationV2();

  virtual void Exec(const Option_t* option);
  virtual InitStatus Init();
  virtual void Finish();

  /** Get energy with standart 2x2 procedure **/
  virtual Double_t GetEnergy(const CbmEcalCell* cell);
  virtual Double_t GetEnergy(Double_t Ecls, const CbmEcalCell* cell);
  virtual Double_t GetEnergy(Double_t Ecls, Double_t tantheta, Int_t celltype) const;
  /** Get calorimeter respose to photon of given energy **/
  virtual Double_t GetE2(Double_t e, Double_t tantheta, Int_t celltype) const;
  /** Rough estimation of energy **/
  virtual Double_t GetERough(Double_t e) const;
  virtual ~CbmEcalCalibrationV2();
private:
  /** Method to use **/
  Int_t GetMethod(Double_t m) const;
  /** Index in parameter array**/
  Int_t N(Int_t p, Int_t m, Int_t n) const;
  /** Name of the parameter file **/
  TString fParName;
  /** Name of the container **/
  TString fContName;
  /** Calibration **/
  Double_t* fP; 		//!
  /** Number of methods **/
  Int_t fMethods;	
  /** Angular bounradies of method **/
  Double_t* fMethodB;		//!
  /** Information about the calorimeter **/
  CbmEcalInf* fInf;		//!
  
  CbmEcalCalibrationV2(const CbmEcalCalibrationV2&);
  CbmEcalCalibrationV2& operator=(const CbmEcalCalibrationV2&);

  ClassDef(CbmEcalCalibrationV2, 1)
};

/** We will never have more than 5 methods in this calibration. I hope... **/
inline Int_t CbmEcalCalibrationV2::GetMethod(Double_t m) const
{
  if (fMethodB[0]>m) return 0;
  if (fMethodB[1]>m) return 1;
  if (fMethodB[2]>m) return 2;
  if (fMethodB[3]>m) return 3;
  return 4;
}

inline Int_t CbmEcalCalibrationV2::N(Int_t p, Int_t m, Int_t n) const 
  {return n+3*m+3*fMethods*p;}

inline Double_t CbmEcalCalibrationV2::GetEnergy(Double_t Ecls, Double_t tantheta, Int_t celltype) const
{
  if (Ecls<1e-5) return 0;
  Double_t theta=TMath::ATan(tantheta)*TMath::RadToDeg();
  Int_t m=celltype;
  if (m<0) GetMethod(tantheta);
  Double_t p00=fP[N(0,m,0)];
  Double_t p01=fP[N(0,m,1)];
  Double_t p02=fP[N(0,m,2)];
  Double_t p0=p00+theta*(p01+theta*p02);

  Double_t p10=fP[N(1,m,0)];
  Double_t p11=fP[N(1,m,1)];
  Double_t p12=fP[N(1,m,2)];
  Double_t p1=p10+theta*(p11+theta*p12);

  Double_t p20=fP[N(2,m,0)];
  Double_t p21=fP[N(2,m,1)];
  Double_t p22=fP[N(2,m,2)];
  Double_t p2=p20+theta*(p21+theta*p22);

  Double_t res=TMath::Log(Ecls)*p0+p1+p2/Ecls;

  return Ecls/res;
}

/** Get calorimeter respose to photon of given energy **/
inline Double_t CbmEcalCalibrationV2::GetE2(Double_t e, Double_t tantheta, Int_t celltype) const
{
//  Double_t a=fP0a[celltype]+fP1a[celltype]*tantheta;
//  Double_t b=fP0b[celltype]+fP1b[celltype]*tantheta;
//  return e*(b+a/TMath::Sqrt(e));
  return -1111;
}

/** Rough estimation of energy **/
inline Double_t CbmEcalCalibrationV2::GetERough(Double_t e) const
{
  return -1111;
}
#endif
