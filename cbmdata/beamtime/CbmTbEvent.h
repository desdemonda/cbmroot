/**@file CbmTbEvent.h
 **@date 26.06.2009
 **author V. Friese <v.friese@gsi.de>
 **
 **Event header file for test beam data AUG09
 **Setup: 3 STS + GEM (MUCH) + RICH, 2 beam counters
 **
 **/


#ifndef CBMTBEVENT_H
#define CBMTBEVENT_H 1


#include "CbmDetectorList.h"

#include "FairEventHeader.h"

#include <vector>


class CbmDigi;


class CbmTbEvent : public FairEventHeader
{

 public:

  /** Default constructor **/
  CbmTbEvent();


  /** Destructor (no action) **/
  virtual ~CbmTbEvent() { }


  /** Accessors **/
  Double_t GetTime()         const { return fEventTime; }
  Double_t GetTimeStart()    const { return fTimeStart; }
  Double_t GetTimeStop()     const { return fTimeStop; }
  Double_t GetDuration()     const { return fTimeStop - fTimeStart; }
  Double_t GetEpochTime()    const { return fEpochTime; }
  Int_t    GetNofAux()       const { return fNofAux; }
  Int_t    GetNofData    ()  const { return fNofData; }
  Int_t    GetNofDigis(Int_t systemId)     const { return fNofDigis[systemId]; }
  Int_t    GetNofBeam()      const { return fTimeBc.size(); }
  Double_t GetTimeBc()       const { 
    return (fTimeBc.empty() ? 0. : fTimeBc[0]); }
  Double_t GetTimeBc(Int_t iBc) const { 
    return (fTimeBc.size() > iBc ? fTimeBc[iBc] : 0.); }
  Int_t    GetEventType()    const { return fEventType; }

  void     SetEventType(Int_t type) { fEventType = type; }

  /** Add a digi to the event  **/
  void AddDigi(CbmDigi* digi);


  /** Check for beam signal **/
  Bool_t IsBeam() const { return ( ! fTimeBc.empty() ); }


  /** Check for good beam (only one beam counter signal) **/
  Bool_t IsGoodBeam() const { return ( fTimeBc.size() == 1 ); }


  /** Check for empty event **/
  Bool_t IsEmpty() const { return (fNofData==0 ? kTRUE : kFALSE); }


  /** Clear event **/
  void Clear();


  /** Print event **/
  virtual void Print(Option_t* opt = "");



 private:

  //Double_t fTime;                 /** Absolute event time [ns] **/
  Double_t fTimeStart;            /** Time of first digi in event [ns] **/
  Double_t fTimeStop;             /** Time of last digi in event [ns] **/
  Double_t fEpochTime;            /** Time of epoch marker [ns] **/
  Int_t    fNofData;              ///< Total number of digis in event
  std::vector<Double_t> fTimeBc;  /** Time Of Beam Counter messages **/
  Int_t    fNofDigis[kNOFDETS];   ///< Number of digis per detector system
  Int_t    fNofAux;               ///< Number of AUX signals in event
  Int_t    fEventType;            ///< Type of event 0=normal event,1=baseline

  Double_t fTimeSum;              //! Transient. For calculation of tmean.



  ClassDef(CbmTbEvent,1);

};



#endif
