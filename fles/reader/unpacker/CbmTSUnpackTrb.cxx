// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                              CbmTSUnpackTrb                       -----
// -----                    Created 07.11.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
#include "CbmTSUnpackTrb.h"

#include "CbmTrbIterator.h"
#include "CbmRawEvent.h"
#include "CbmRawSubEvent.h"
#include "CbmRichTrbParam.h"
#include "CbmTrbCalibrator.h"
#include "CbmTrbRawMessage.h"

#include "TimesliceReader.hpp"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <bitset>

CbmTSUnpackTrb::CbmTSUnpackTrb()
  : CbmTSUnpack(),
    fTrbRaw(new TClonesArray("CbmTrbRawMessage", 10)),
    fTrbBridge(new TrbBridge()),
    fTrbEventList(),
    fData(),
    fDataSize(0),
    fSynchRefTime(0.),
    fSynchOffsetTimeMap(),
    fTrbIter(NULL),
    fRawEvent(NULL),
    fRawSubEvent(NULL),
    fLink(0),
    fEpochMarker(),
    fTimeBuffer()
{
}

CbmTSUnpackTrb::~CbmTSUnpackTrb()
{
}

Bool_t CbmTSUnpackTrb::Init()
{
  LOG(INFO) << "Initializing" << FairLogger::endl; 

  FairRootManager* ioman = FairRootManager::Instance();
  if (ioman == NULL) {
    LOG(FATAL) << "No FairRootManager instance" << FairLogger::endl;
  }
  ioman->Register("TrbRawMessage", "trb raw data", fTrbRaw, kTRUE);

  return kTRUE;
}

Bool_t CbmTSUnpackTrb::DoUnpack(const fles::Timeslice& ts, size_t component)
{
  LOG(DEBUG) << "Unpacking Trb Data" << FairLogger::endl; 

  fLink = ts.descriptor(component, 0).eq_id;

  Int_t skipMS = 1; //TODO: should be set from macro according to the flib settings
  fTrbEventList = fTrbBridge->extractEventsFromTimeslice(ts, skipMS);

  LOG(DEBUG) << "Found " << fTrbEventList.size() << " TRB events in time slice"
	    << FairLogger::endl;

  // Loop over all TRB events which are in the timeslice 
  while ( !fTrbEventList.empty() ) {  
    fData = fTrbEventList.front();
    fDataSize = fData.size();
    UnpackTrbEvents();
    fTrbEventList.pop_front();
  }

  return kTRUE;
}


void CbmTSUnpackTrb::Reset()
{
  fTrbEventList.clear();
  fTrbRaw->Clear();
}

void CbmTSUnpackTrb::Finish()
{
}

void CbmTSUnpackTrb::UnpackTrbEvents()
{


  fTrbIter = new CbmTrbIterator(static_cast<void*>(fData.data()), fDataSize);

  // Loop over TRB events 
  while (true) {
    
    // Try to extract next event from the Iterator. If no events left - go out of the loop
    fRawEvent = fTrbIter->NextEvent();
    if (fRawEvent == NULL) break;
    if(gLogger->IsLogNeeded(DEBUG)) {
      fRawEvent->Print();    
    }
    fSynchRefTime = -1.;
    
    // Loop over TRB subevents
    while (true){
      fRawSubEvent = fTrbIter->NextSubEvent();
      if (fRawSubEvent == NULL) break;
      if(gLogger->IsLogNeeded(DEBUG)) {
	fRawSubEvent->Print();
      }
      ProcessTdc(fRawSubEvent);
    }
    
  }
  
}

void CbmTSUnpackTrb::ProcessTdc(CbmRawSubEvent* rawSubEvent)
{
  UInt_t tdcDataIndex = 0;
  UInt_t tdcData = 0x0;
  UInt_t trbId = rawSubEvent->SubId();
  
  while (true) {
    tdcData = rawSubEvent->SubDataValue(tdcDataIndex);
    UInt_t tdcNofWords = (tdcData >> 16) & 0xffff;
    UInt_t tdcId = tdcData & 0xffff;
    LOG(DEBUG) << "TDC DATA tdcNofWords = " << tdcNofWords << " , ID=  " 
	      << std::hex << tdcId << std::dec << FairLogger::endl;

    if (tdcId == 0x5555) break; // Fixed word for end of SubSubEvent
 
    // if these ids show up one has to jump to the next word and interpret that one
    // this magic knowledge comes from Jan and Manuel
    if (tdcId == 0x7000 || tdcId == 0x7001 || tdcId == 0x7002 || tdcId == 0x7003){
      tdcDataIndex++;
      continue;
    }

    //read TDC words to array
    UInt_t* dataArray = new UInt_t[tdcNofWords];
    for (UInt_t i = 0; i < tdcNofWords; i++) {
      tdcDataIndex++;
      tdcData = rawSubEvent->SubDataValue(tdcDataIndex);
      dataArray[i] = tdcData;
    }
    DecodeTdcDataNew(dataArray, tdcNofWords, tdcId);
    tdcDataIndex++;
    delete [] dataArray;
  }
}

void CbmTSUnpackTrb::DecodeTdcDataNew(UInt_t* data, UInt_t length, UInt_t tdcId)
{

  // ignore MBS-Triggern+ Billboard and CTS for the time beeing
  if ( 0x7005 == tdcId || 0x112 == tdcId ) return;

  for (UInt_t i = 0; i < length; i++){
    LOG(DEBUG) << "Data word: " << std::hex << data[i] << std::dec << FairLogger::endl;
    std::bitset<32> value(data[i]);
    LOG(DEBUG) << "Data word: " << value << FairLogger::endl;

    UInt_t tdcData = data[i];

    UInt_t tdcTimeDataMarker = (data[i] >> 31) & 0x1; //1 bit
    if (tdcTimeDataMarker == 0x1) { //TIME DATA
      LOG(DEBUG) << "Found tdc time data for tdc " << tdcId << FairLogger::endl;  
      UInt_t chNum = (tdcData >> 22) & 0x7f; // 7bits
      UInt_t fineTime = (tdcData >> 12) & 0x3ff; // 10 bits //0x3ff
      UInt_t edge = (tdcData >> 11) & 0x1; // 1bit
      UInt_t coarseTime = (tdcData) & 0x7ff; // 1bits

      Int_t epochMarker = 0;
      auto it=fEpochMarker.find(tdcId);
      if (it != fEpochMarker.end()) {
	epochMarker = it->second;
      }

      LOG(DEBUG) << "Found " << fineTime << ", " << edge << ", " << coarseTime 
		<< " fine, edge and coarse " 
		<< " for TDC " << tdcId << ", channel " << chNum << FairLogger::endl;   

      Double_t fullTime = GetFullCoarseTime(epochMarker, coarseTime);
      LOG(INFO) << "FullTime: " << fullTime
		<< FairLogger::endl;

      new( (*fTrbRaw)[fTrbRaw->GetEntriesFast()] )
	CbmTrbRawMessage(fLink, tdcId, chNum, epochMarker, coarseTime, fineTime, edge);


      if( 110 == tdcId ) {
	pair<Double_t, CbmTrbRawMessage*> 
	  value (fullTime, static_cast<CbmTrbRawMessage*>(fTrbRaw->At(fTrbRaw->GetEntriesFast())));
	  fTimeBuffer.insert(value);



      }
    } else {
      LOG(DEBUG) << "No tdc time data for tdc " << tdcId << FairLogger::endl;  
      UInt_t tdcMarker = (tdcData >> 29) & 0x7; //3 bits
      if (tdcMarker == 0x1) {// TDC header
	UInt_t randomCode = (tdcData >> 16) & 0xff; // 8bits
	UInt_t errorBits = (tdcData) & 0xffff; //16 bits
        LOG(DEBUG) << Form("TDC HEADER randomCode:0x%02x, errorBits:0x%04x\n", 
			   randomCode, errorBits) 
		   << FairLogger::endl;
      } else if (tdcMarker == 0x2) {// DEBUG
	UInt_t debugMode = (tdcData >> 24) & 0x1f; //5
	UInt_t debugBits = (tdcData) & 0xffffff;//24 bits
	LOG(DEBUG) << Form("DEBUG debugMode:%i, debugBits:0x%06x\n", 
			   debugMode, debugBits) 
		   << FairLogger::endl;
      } else if (tdcMarker == 0x3){ // EPOCH counter
	UInt_t curEpochCounter = (tdcData) & 0xfffffff; //28 bits

	auto it=fEpochMarker.find(tdcId);
	if (it == fEpochMarker.end()) {
	  fEpochMarker.insert( std::pair<Int_t,Int_t>(tdcId,curEpochCounter) );    
	} else {
	  it->second = curEpochCounter;
	}

	LOG(DEBUG) << Form("EPOCH COUNTER epochCounter:0x%07x\n", 
			   curEpochCounter)
		   << FairLogger::endl;
      }
    }

  }
}

void CbmTSUnpackTrb::DecodeTdcData(UInt_t* data, UInt_t size, UInt_t trbId, UInt_t tdcId)
{
  Bool_t isPmtTrb = CbmRichTrbParam::Instance()->IsPmtTrb(trbId);
  UInt_t curEpochCounter = 0;
  UInt_t prevChNum[5] = {0, 0, 0, 0, 0};
  UInt_t prevEpochCounter[5] = {0, 0, 0, 0, 0};
  UInt_t prevCoarseTime[5] = {0, 0, 0, 0, 0};
  UInt_t prevFineTime[5] = {0, 0, 0, 0, 0};
  UInt_t prevCounter = 0;
  UInt_t prevNof = 0;
  for (UInt_t i = 0; i < size; i++){
    UInt_t tdcData = data[i];
    
    UInt_t tdcTimeDataMarker = (tdcData >> 31) & 0x1; //1 bit
    if (tdcTimeDataMarker == 0x1) { //TIME DATA
      
      UInt_t chNum = (tdcData >> 22) & 0x7f; // 7bits
      UInt_t fineTime = (tdcData >> 12) & 0x3ff; // 10 bits //0x3ff
      UInt_t edge = (tdcData >> 11) & 0x1; // 1bit
      UInt_t coarseTime = (tdcData) & 0x7ff; // 1bits
      
      // Give the calibrator the read fine time so that it was taken into account
      if ((trbId != 0x7005)) CbmTrbCalibrator::Instance()->AddFineTime(trbId, tdcId, chNum, fineTime);
      
      if (chNum == 0 ) {
	Double_t time = GetFullTime(trbId, tdcId, chNum, curEpochCounter, coarseTime, fineTime);
	//cout << "CHANNEL0: " << fixed << fSynchRefTime << " " << time << "  "<<fSynchRefTime - time << " " <<hex << "     TRB " << trbId << "    TDC " << tdcId << dec << endl;
	if (fSynchRefTime == -1. ) {//&& (isPmtTrb || tdcId == 0x0110)
	  fSynchRefTime = time;
	}
	if (fSynchRefTime != -1){
	  fSynchOffsetTimeMap[tdcId] = fSynchRefTime - time;
	} else{
	  fSynchOffsetTimeMap[tdcId] = 0.;
	}
      } else {
	if (tdcId == 0x7005) { //CTS
	  
	} else if (tdcId == 0x0110) { // reference time TDC for event building
	  //cout << fSynchOffsetTimeMap[0x0110] << endl;
	  if  (chNum == 5) { // ||           // hodoscope (beam trigger)
	      //		(fAnaType == kCbmRichLaserPulserEvent && chNum == 7) ||    // UV LED
	      //		(fAnaType == kCbmRichLedPulserEvent && chNum == 15) ){     // Laser


	    /*
	new( (*fTrbRaw)[fTrbRaw->GetEntriesFast()] )      	    
	    CbmTrbRawMessage(trbId, tdcId, chNum, curEpochCounter, coarseTime, fineTime, 0, 0, 0, 0);
	    */
	    //	    fRawEventTimeHits.push_back(rawHitRef);
	  }
	} else if ( isPmtTrb ) {
	  if (chNum == prevChNum[prevCounter]) {
	    LOG(DEBUG) << " DOUBLE HIT DETECTED TIMEDATA chNum:" << chNum << ", fineTime:" << fineTime << ", edge:" << edge << ", coarseTime:" << coarseTime
		       << ", fullTime:" << fixed << GetFullTime(trbId, tdcId, chNum, curEpochCounter, coarseTime, fineTime) << FairLogger::endl;
	    //fNofDoubleHits++;
	    continue;
	  }
	  if (chNum%2 == 1) { // leading edge
	    if (chNum == prevChNum[prevCounter]) {
	      prevCounter++;
	    }
	    prevChNum[prevCounter] = chNum;
	    prevEpochCounter[prevCounter] = curEpochCounter;
	    prevCoarseTime[prevCounter] = coarseTime;
	    prevFineTime[prevCounter] = fineTime;
	  } else { // trailing edge
	    //prevNof = prevCounter;
	    prevCounter = 0;
	    if (chNum == prevChNum[prevCounter]) {
	      prevCounter++;
	    }
	    if (chNum - prevChNum[prevCounter] == 1) {
	      /*
	      new( (*fTrbRaw)[fTrbRaw->GetEntriesFast()] )      	    
		CbmTrbRawHit(trbId, tdcId, prevChNum[prevCounter],
			     prevEpochCounter[prevCounter], prevCoarseTime[prevCounter],
			     prevFineTime[prevCounter], chNum, curEpochCounter,
			     coarseTime, fineTime);
	      */
	      prevChNum[prevCounter] = chNum;
	      prevEpochCounter[prevCounter] = 0;
	      prevCoarseTime[prevCounter] = 0;
	      prevFineTime[prevCounter] = 0;
	    } else {
	      LOG(DEBUG) << "Leading edge channel number - trailing edge channel number != 1. tdcId=" << hex << tdcId << dec <<
		", chNum=" << chNum <<  ", prevChNum=" << prevChNum[prevCounter] << FairLogger::endl;
	    }
	  }
	} //isPmtTrb
      }// if chNum!=0
      LOG(DEBUG) << "TIMEDATA chNum:" << chNum << ", fineTime:" << fineTime << ", edge:" << edge << ", coarseTime:" << coarseTime
		 << ", fullTime:" << fixed << GetFullTime(trbId, tdcId, chNum, curEpochCounter, coarseTime, fineTime) << FairLogger::endl;
      if (fineTime == 0x3ff) LOG(DEBUG) << "-ERROR- Dummy fine time registered: " << fineTime << FairLogger::endl;
    }//if TIME DATA
    
    UInt_t tdcMarker = (tdcData >> 29) & 0x7; //3 bits
    if (tdcMarker == 0x1) {// TDC header
      UInt_t randomCode = (tdcData >> 16) & 0xff; // 8bits
      UInt_t errorBits = (tdcData) & 0xffff; //16 bits
      //printf("TDC HEADER randomCode:0x%02x, errorBits:0x%04x\n", randomCode, errorBits);
    } else if (tdcMarker == 0x2) {// DEBUG
      UInt_t debugMode = (tdcData >> 24) & 0x1f; //5
      UInt_t debugBits = (tdcData) & 0xffffff;//24 bits
      //printf("DEBUG debugMode:%i, debugBits:0x%06x\n", debugMode, debugBits);
    } else if (tdcMarker == 0x3){ // EPOCH counter
      curEpochCounter = (tdcData) & 0xfffffff; //28 bits
      //printf("EPOCH COUNTER epochCounter:0x%07x\n", curEpochCounter);
    }
  }// for loop
}

Double_t CbmTSUnpackTrb::GetFullCoarseTime(UInt_t epoch, UShort_t coarseTime)
{
  Double_t coarseUnit = 5.; // 5 ns
  Double_t epochUnit = coarseUnit * 0x800; // ~10.3 mus
  
  Double_t time = epoch * epochUnit + coarseTime * coarseUnit;

  return time;
}


Double_t CbmTSUnpackTrb::GetFullTime(UShort_t TRB, UShort_t TDC, UShort_t CH, UInt_t epoch, UShort_t coarseTime, UShort_t fineTime)
{
  Double_t coarseUnit = 5.;
  Double_t epochUnit = coarseUnit * 0x800;
  
  uint32_t trb_index = (TRB >> 4) & 0x00FF - 1;
  uint32_t tdc_index = (TDC & 0x000F);
  
  Double_t time = epoch * epochUnit + coarseTime * coarseUnit -
    CbmTrbCalibrator::Instance()->GetFineTimeCalibrated(trb_index, tdc_index, CH, fineTime);
  if (CH != 0){
    if (fSynchOffsetTimeMap[TDC] > 150) {
      LOG(ERROR) << "CbmRichTrbUnpack::GetFullTime Synch Offset > 150 ns for TDC" << TDC << FairLogger::endl;
    } else {
      time = time + fSynchOffsetTimeMap[TDC];
    }
  }
  return time;
}

/*
void CbmTSUnpackTrb::Register()
{
}
*/


ClassImp(CbmTSUnpackTrb)
