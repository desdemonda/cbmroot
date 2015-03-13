/**
 * \file CbmTrdDigi.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 03.05.2013
 **/

#ifndef CBMTRDDIGI_H
#define CBMTRDDIGI_H 1

#include "CbmDigi.h"
#include "CbmTrdAddress.h"

#include <string>
#include <list>
using std::string;
using std::list;

class CbmTrdDigi : public CbmDigi
{
 public:
  /**
   * \brief Default constructor.
   */
  CbmTrdDigi();

  /**
   * \brief Constructor with assignment.
   * \param[in] address Unique channel address.
   * \param[in] charge Charge.
   * \param[in] time Absolute time [ns].
   */
  CbmTrdDigi(Int_t address, Double_t charge, Double_t time);
  CbmTrdDigi(Int_t address, Double_t charge, Double_t time, Bool_t primeTriggered, Bool_t fnr_Triggered, Bool_t globalTriggered, Bool_t normalStop, Bool_t bufferFullStop, Bool_t doubleHitStop);
  /**
   * \brief Destructor.
   */
  ~CbmTrdDigi();

  /**
   * \brief Inherited from CbmDigi.
   */
  Int_t GetAddress() const { return fAddress; };

  /**
   * \brief Inherited from CbmDigi.
   */
  Double_t GetCharge() const { return fCharge; }

  /**
   * \brief Inherited from CbmDigi.
   */
  Int_t GetSystemId() const { return CbmTrdAddress::GetSystemId(fAddress); }

  /**
   * \brief Inherited from CbmDigi.
   */
  Double_t GetTime() const { return fTime; }

  Bool_t GetPrimeTriggerStatus() const { return fPrimeTriggered; }

  Bool_t GetFNR_TriggerStatus() const { return fFNR_Triggered; }

  Bool_t GetGlobal_TriggerStatus() const { return fGlobalTriggered; }

  Int_t* GetNeighbourTriggerIds() {return fNeighbourTriggerIds; }

  Int_t GetNrFNR_Triggers() const { return fn_FNR_Triggers;}

  /** Accessors **/
  void SetAddress(Int_t address) { fAddress = address; }
  void SetCharge(Double_t charge) { fCharge = charge; }
  void SetTime(Double_t time) { fTime = time; }
  void SetPrimeTriggerStatus(Bool_t primeTriggered)  { fPrimeTriggered = primeTriggered; }
  void SetFNR_TriggerStatus(Bool_t fnr_Triggered)  { fFNR_Triggered = fnr_Triggered; }
  void SetNormalStop(Bool_t normalStop)  { fNormalStop = normalStop; }
  void SetBufferFullStop(Bool_t bufferFullStop)  { fBufferFullStop = bufferFullStop; }
  void SetDoubleHitStop(Bool_t doubleHitStop)  { fDoubleHitStop = doubleHitStop; }
  void AddNeighbourTriggerId(Int_t digiId);
  void SetPulseShape(Float_t pulse[45]);
  /** Modifiers **/
  void AddCharge(Double_t charge) { fCharge += charge; }

  string ToString() const;
  
 private:
  Int_t fn_FNR_Triggers;
  Int_t fAddress; ///< Unique channel address
  Double_t fCharge; ///< Charge
  Double_t fTime; ///< Absolute time [ns]
  Bool_t fGlobalTriggered;
  Bool_t fPrimeTriggered;
  Bool_t fFNR_Triggered;
  Bool_t fNormalStop;
  Bool_t fBufferFullStop;
  Bool_t fDoubleHitStop;

  Int_t fNeighbourTriggerIds[8]; // 8 direct neighbours per pad in maximum
  Float_t fPulseShape[45];
  ClassDef(CbmTrdDigi, 4);
};

#endif
