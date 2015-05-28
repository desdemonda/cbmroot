/** @file CbmBuildEventsSimple.h
 ** @ athor //Dr.Sys (Mikhail Prokudin) prokudin@cern.ch
 ** @ date 14.06.06
 **/

#ifndef CBMBUILDEVENTSSIMPLE_H
#define CBMBUILDEVENTSSIMPLE_H

#include "FairTask.h"

class CbmTimeSlice;
class TClonesArray;

class CbmBuildEventsSimple : public FairTask
{
public:
  CbmBuildEventsSimple();
  CbmBuildEventsSimple(const char* name, Int_t iVerbose=1);

  virtual InitStatus Init();
  Int_t ConstructEvent();
  Int_t SkipEvents(Int_t nsts);	/** Skip events until fEndT reached **/
  virtual void Exec(Option_t* opt);
  virtual void Finish();

  /** Fills Stsdigis array. STS separate, because start and end end digi is known. **/
  void FillStsInfo(Int_t st, Int_t end);

  // Setters. Time in [ns], events in counts :) Only STS information is used.
  void SetStepLength(Double_t slen=30) {fStepLength=slen;}
  void SetEventStartThreshold(Int_t nev=2000) {fEventStartThreshold=nev;}
  void SetSliceLength(Double_t len=300) {fSliceLength=len;}

  ~CbmBuildEventsSimple();

private:
  CbmTimeSlice* fSlice;	/** Pointer to current timeslce **/
  TClonesArray* fSTSDigi;	/** Array of STS digis **/
  Int_t fNSTSDigis;

  Int_t fEv;		/** Number of current event **/
  Int_t fSliceN;	/** Number of current time slice **/
  Double_t fStepLength;	/** Length of step to find event begin. [ns] **/
  Int_t fEventStartThreshold;	/** Number of hits in step required to hind an event **/
  Double_t fSliceLength;	/** Lenght of time slice. If 2 events inside one slice -> slice is bad. [ns]**/

  Double_t fPrevT;	/** Time of previous hit **/
  Int_t fPrevN;		/** Number of previous hit**/
  Double_t fCurT;	/** Time of current hit **/
  Int_t fCurN;		/** Number of current hit **/
  Double_t fStartT;	/** Start of current event **/
  Double_t fEndT;	/** End of current event **/

  Int_t fOpenEvent;

  /** Prevent usage of copy constructor and assignment operator **/
  CbmBuildEventsSimple(const CbmBuildEventsSimple&);
  CbmBuildEventsSimple& operator =(const CbmBuildEventsSimple&);


  ClassDef(CbmBuildEventsSimple, 0)
};

#endif
