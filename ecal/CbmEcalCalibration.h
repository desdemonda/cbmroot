/** A very simple calorimeter calibration container.
 ** Paramers now hardcoded. This is BAD. Waiting for general
 ** parameter hadlning in the framework. **/

#ifndef CBMECALCALIBRATION_H
#define CBMECALCALIBRATION_H

#include "FairTask.h"
#include "TMath.h"
#include "TString.h"

class CbmEcalCell;
class CbmEcalInf;


class CbmEcalCalibration : public FairTask 
{
public:
  /** Standard constructor **/
  CbmEcalCalibration(const char *name, const Int_t iVerbose, const char* parname);
  /** Default constructor **/
  CbmEcalCalibration();

  virtual void Exec(const Option_t* option);
  virtual InitStatus Init();
  virtual void Finish();

  /** Get energy with standart 2x2 procedure **/
  virtual Double_t GetEnergy(const CbmEcalCell* cell);
  virtual Double_t GetEnergy(Double_t e2, const CbmEcalCell* cell);
  virtual Double_t GetEnergy(Double_t e2, Double_t tantheta, Int_t celltype) const;
  /** Get calorimeter respose to photon of given energy **/
  virtual Double_t GetE2(Double_t e, Double_t tantheta, Int_t celltype) const;
  /** Rough estimation of energy **/
  virtual Double_t GetERough(Double_t e) const;
  virtual ~CbmEcalCalibration();
  virtual TString ParName() const {return fParName;} 
private:
  /** Name of the parameter file **/
  TString fParName;
  /** Calibration **/
  Double_t fP0a[5];		//!
  Double_t fP0b[5];		//!
  Double_t fP1a[5];		//!
  Double_t fP1b[5];		//!

  /** Information about the calorimeter **/
  CbmEcalInf* fInf;		//!
  
  CbmEcalCalibration(const CbmEcalCalibration&);
  CbmEcalCalibration& operator=(const CbmEcalCalibration&);

  ClassDef(CbmEcalCalibration, 1)
};

inline Double_t CbmEcalCalibration::GetEnergy(Double_t e2, Double_t tantheta, Int_t celltype) const
{
  Double_t a=fP0a[celltype]+fP1a[celltype]*tantheta;
  Double_t b=fP0b[celltype]+fP1b[celltype]*tantheta;
  Double_t d=TMath::Sqrt(a*a+4.0*e2*b);
  Double_t res=(-a+d)/2.0/b; res*=res;
  return res;
}

/** Get calorimeter respose to photon of given energy **/
inline Double_t CbmEcalCalibration::GetE2(Double_t e, Double_t tantheta, Int_t celltype) const
{
  Double_t a=fP0a[celltype]+fP1a[celltype]*tantheta;
  Double_t b=fP0b[celltype]+fP1b[celltype]*tantheta;
  return e*(b+a/TMath::Sqrt(e));
}

/** Rough estimation of energy **/
inline Double_t CbmEcalCalibration::GetERough(Double_t e) const
{
  return e/fP0b[1];
}
#endif
