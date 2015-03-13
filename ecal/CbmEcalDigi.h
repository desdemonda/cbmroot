// A very simple digitization scheme for Cbm Calorimeter
// Operate over CbmEcalStructure

#ifndef CBMECALDIGI_H
#define CBMECALDIGI_H

#include "FairTask.h"

class CbmEcalStructure;

class CbmEcalDigi : public FairTask
{
public:
  /** Default constructor **/
  CbmEcalDigi();
  /** Standard constructor. Use this **/
  CbmEcalDigi(const char* name, Int_t iVerbose);
  /** Destructor **/
  virtual ~CbmEcalDigi();
  /** Initialization of the task **/  
  virtual InitStatus Init();
  /** Executed task **/ 
  virtual void Exec(Option_t* option);
  /** Finish task **/ 
  virtual void Finish();
  

  void SetPedestal(Short_t ped=80) {fPedestal=ped;}
  void SetADCMax(Short_t adcmax=4096) {fADCMax=adcmax;}
  void SetADCNoise(Float_t adcnoise=0.5e-3) {fADCNoise=adcnoise;}
  void SetADCChannel(Float_t adcchannel=0.5e-3) {fADCChannel=adcchannel;}

  Short_t GetPedestal() const {return fPedestal;}
  Short_t GetADCMax() const {return fADCMax;}
  Float_t GetADCNoise() const {return fADCNoise;}
  Float_t GetADCChannel() const {return fADCChannel;}
private:
  // Pedestal 
  Short_t fPedestal;
  // ADC maximum
  Short_t fADCMax;
  // ADC Noise
  Float_t fADCNoise;
  // ADC channel (in energy deposition in _SCINTILLATOR_)
  Float_t fADCChannel;
  // Calorimeter structure
  CbmEcalStructure* fStr;	//!

  CbmEcalDigi(const CbmEcalDigi&);
  CbmEcalDigi& operator=(const CbmEcalDigi&);

  ClassDef(CbmEcalDigi, 1);
};

#endif
