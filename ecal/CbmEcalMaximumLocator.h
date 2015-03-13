#ifndef CBMECALMAXIMUMLOCATOR_H
#define CBMECALMAXIMUMLOCATOR_H

#include "FairTask.h"

class TClonesArray;
class CbmEcalStructure;
class TClonesArray;

class CbmEcalMaximumLocator : public FairTask
{
public: 
  /** An empty constructor **/
  CbmEcalMaximumLocator();
  CbmEcalMaximumLocator(const char* name, Int_t verbose);
  virtual void Exec(const Option_t* option);
  virtual InitStatus Init();
  virtual void Finish();
  ~CbmEcalMaximumLocator();

  Double_t ECut() const {return fECut;}
  void SetECut(Double_t ecut=0.005) {fECut=ecut;}
private:
  /** Array of found maximums **/
  TClonesArray* fMaximums;
  /** A calorimeter structure **/
  CbmEcalStructure* fStr;
  /** energy cut on energy of maximum**/
  Double_t fECut;
  /** Current event **/
  Int_t fEvent;

  CbmEcalMaximumLocator(const CbmEcalMaximumLocator&);
  CbmEcalMaximumLocator& operator=(const CbmEcalMaximumLocator&);

  ClassDef(CbmEcalMaximumLocator, 1);
};

#endif
