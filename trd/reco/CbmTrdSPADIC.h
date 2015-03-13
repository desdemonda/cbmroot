// -------------------------------------------------------------------------
// -----          CbmTrdSPADIC header file                  -----
// -----   Created 03/06/14  by Cyrano Bergmann       -----
// -------------------------------------------------------------------------


#ifndef CBMTRDSPADIC_H
#define CBMTRDSPADIC_H 

#include "FairTask.h"

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;
class TClonesArray;
class TH1D;
class CbmTrdDigi;

class CbmTrdSPADIC : public FairTask
{

 public:

  /**
   * Default constructor.
   */
  CbmTrdSPADIC();
//  CbmTrdSPADIC(Bool_t MultiHit, Bool_t NeighbourReadout, Bool_t RowClusterMerger, Double_t MinimumChargeTH);

  /**
   * \brief Destructor.
   */
  virtual ~CbmTrdSPADIC();
 
  /**
   * \brief Inherited from FairTask.
   */
  virtual InitStatus Init();

  /**
   * \breif Inherited from FairTask.
   */
  virtual void SetParContainers();
  
  /**
   * \breif Inherited from FairTask.
   */
  virtual void Exec(Option_t * option);

  void SetTriggerThreshold(Double_t triggerthreshold);
  void SetPulseShapeSim(Bool_t pulseShape);
  void SetSelectionMask(Bool_t mask[45]);
  void SetBitResolution(Int_t bit);
  void SetMaxRange(Double_t maxRange);
 private:

  void CR_RC_Shaper(CbmTrdDigi* digi, TH1D* spadicPulse);
  void ADC(TH1D* spadicPulse);
  void ADC(CbmTrdDigi* digi);
  void InitSpadicResponseFunction();
  TH1D *fSpadicResponse;
  Int_t fShaperOrder;
  Double_t fShapingTime;
  Int_t fPeakBin;
  Int_t fBitResolution;
  static const Int_t fnBins = 45;
  Double_t fmaxdEdx;
  Double_t fAdcBit;
  Bool_t fPulseShape;
  Bool_t fSelectionMask[fnBins];

  //Bool_t CbmDigiSorter(std::pair< Int_t, Int_t> a, std::pair< Int_t, Int_t> b);

  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!
  
  CbmTrdGeoHandler* fGeoHandler; //!

  Double_t fMinimumChargeTH;

  CbmTrdSPADIC(const CbmTrdSPADIC&);
  CbmTrdSPADIC& operator=(const CbmTrdSPADIC&);

  ClassDef(CbmTrdSPADIC,2);
};
#endif
