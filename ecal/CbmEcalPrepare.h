// Converts ADC -> energy depostion in plastic
// Very simple realization. 

#ifndef CBMECALPREPARE_H
#define CBMECALPREPARE_H

#include "FairTask.h"

class CbmEcalStructure;

class CbmEcalPrepare : public FairTask
{
public:
  /** Default constructor **/
  CbmEcalPrepare();
  /** Standard constructor. Use this **/
  CbmEcalPrepare(const char* name, Int_t iVerbose);
  /** Destructor **/
  virtual ~CbmEcalPrepare();
  /** Initialization of the task **/  
  virtual InitStatus Init();
  /** Executed task **/ 
  virtual void Exec(Option_t* option);
  /** Finish task **/ 
  virtual void Finish();
  

  void SetPedestal(Short_t ped=80) {fPedestal=ped;}
  void SetADCMax(Short_t adcmax=4096) {fADCMax=adcmax;}
  void SetADCChannel(Float_t adcchannel=0.5e-3) {fADCChannel=adcchannel;}

  Short_t GetPedestal() const {return fPedestal;}
  Short_t GetADCMax() const {return fADCMax;}
  Float_t GetADCChannel() const {return fADCChannel;}
private:
  // Pedestal 
  Short_t fPedestal;
  // ADC maximum
  Short_t fADCMax;
  // ADC channel (in energy deposition in _SCINTILLATOR_)
  Float_t fADCChannel;
  // Calorimeter structure
  CbmEcalStructure* fStr;	//!

  CbmEcalPrepare(const CbmEcalPrepare&);
  CbmEcalPrepare& operator=(const CbmEcalPrepare&);

  ClassDef(CbmEcalPrepare, 1);
};

#endif
