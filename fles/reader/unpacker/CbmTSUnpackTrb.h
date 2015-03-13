// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                              CbmTSUnpackTrb                       -----
// -----                    Created 22.11.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMTSUNPACKTRB_H
#define CBMTSUNPACKTRB_H

#ifndef __CINT__
  #include "Timeslice.hpp"
  #include "Message.hpp"
  #include "TrbBridge.hpp"
#endif


#include "CbmTSUnpack.h"
#include "CbmBeamDefaults.h"
#include "CbmTrbRawMessage.h"

#include "TClonesArray.h"

#include <map>
#include <list>
#include <vector>
 
class CbmRawSubEvent;
class CbmRawEvent;
class CbmRawSubEvent;
class CbmTrbIterator;

class CbmTSUnpackTrb : public CbmTSUnpack
{
 public:
  
  CbmTSUnpackTrb();
  virtual ~CbmTSUnpackTrb();
    
  virtual Bool_t Init();
#ifndef __CINT__
  virtual Bool_t DoUnpack(const fles::Timeslice& ts, size_t component);
#endif
  virtual void Reset();

  virtual void Finish();

 private:

  TClonesArray* fTrbRaw;
#ifndef __CINT__
  TrbBridge* fTrbBridge;
  std::list<std::vector<uint32_t>> fTrbEventList;
  std::vector<uint32_t> fData;
  size_t fDataSize;
#endif

  Double_t fSynchRefTime; // Reference time for synchronization
  std::map<UInt_t, Double_t> fSynchOffsetTimeMap; // first - TDCId, second - time offeset in ns
  CbmTrbIterator* fTrbIter; //!
  CbmRawEvent* fRawEvent;  //!
  CbmRawSubEvent* fRawSubEvent; //!
  Int_t fLink;
  std::map<Int_t, Int_t> fEpochMarker;

  /** Buffer management **/
  std::multimap<Double_t, CbmTrbRawMessage*> fTimeBuffer;


  void LoopOverTrbEvents();
  void UnpackTrbEvents();
  void ProcessTdc(CbmRawSubEvent* rawSubEvent);
  void DecodeTdcData(UInt_t* data, UInt_t size, UInt_t trbId, UInt_t tdcId);
  void DecodeTdcDataNew(UInt_t* data, UInt_t length, UInt_t tdcId);
  Double_t GetFullCoarseTime(UInt_t epoch, UShort_t coarseTime);
  Double_t GetFullTime(UShort_t TRB, UShort_t TDC, UShort_t CH, UInt_t epoch, UShort_t coarseTime, UShort_t fineTime);

  CbmTSUnpackTrb(const CbmTSUnpackTrb&);
  CbmTSUnpackTrb operator=(const CbmTSUnpackTrb&);

  ClassDef(CbmTSUnpackTrb, 1)
};

#endif
