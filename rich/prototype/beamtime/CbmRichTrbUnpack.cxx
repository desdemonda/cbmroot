#include "CbmRichTrbUnpack.h"

#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include "CbmTrbIterator.h"
#include "CbmTrbRawHit.h"
#include "CbmTrbOutputHit.h"
#include "CbmRichTrbParam.h"
#include "CbmTrbCalibrator.h"
#include "FairRootManager.h"
#include "CbmRichHit.h"
#include "CbmRichHitInfo.h"
#include "TClonesArray.h"
#include "CbmDrawHist.h"
#include "TFolder.h"
#include "CbmRichTrbTimeMessage.h"
#include "CbmRichEpicsData.h"

#include "TH1D.h"
#include "TCanvas.h"

#include "FairLogger.h"

CbmRichTrbUnpack::CbmRichTrbUnpack(TString hldFileName) 
  : FairSource(),
    fHldFileName(hldFileName),
    fRichHits(NULL),
    fRichHitInfos(NULL),
    fDataPointer(NULL),
    fDataSize(0),
    fEventNum(0),
    fNofHitsWithOneEdge(0),
    fNofRawEvents(0),
    fNofRawSubEvents(0),
    fNofTimeDataMessages(0),
    fSynchRefTime(0.),
    fSynchOffsetTimeMap(),
    fRawRichHits(),
    fRawEventTimeHits(),
    fRawFingerSciHits(),
    fOutputRichHits(),
    fOutputEventTimeHits(),
    fOutputFingerSciHits(),
    fhChannelEntries(),
    fhEpoch(),
    fhCoarseTime(),
    fhFineTime(),
    fhDeltaT(),
    fhDiffHitTimeEventTime(),
    fhNofRichHitsVsTrbNum(),
    fhDiffHitTimeEventTimeAll(),
    fhDiffFingerSciTimeEventTimeAll(),
    fAnaType(kCbmRichBeamEvent),
    fUseFingerSci(false),
    fDrawHist(kFALSE)
{
}

CbmRichTrbUnpack::~CbmRichTrbUnpack()
{
	;
}

Bool_t CbmRichTrbUnpack::Init()
{
	LOG(INFO) << "CbmRichTrbUnpack::Init()" << FairLogger::endl;
	LOG(INFO) << "Input file name " << fHldFileName << endl;

	ReadInputFileToMemory();

	FairRootManager* fManager = FairRootManager::Instance();

	fRichHits = new TClonesArray("CbmRichHit");
	fManager->Register("RichHit","RICH", fRichHits, kTRUE);

	fRichHitInfos = new TClonesArray("CbmRichHitInfo");
	fManager->Register("RichHitInfo","RICH", fRichHitInfos, kTRUE);

	ReadEvents();

	fhNofRichHitsVsTrbNum = new TH2D("fhNofRichHitsVsTrbNum", "fhNofRichHitsVsTrbNum;TRB number;Nof hits in event", TRB_TDC3_NUMBOARDS, .5, TRB_TDC3_NUMBOARDS + .5, 65, -0.5, 64.5);
	fhDiffHitTimeEventTimeAll = new TH1D("fhDiffHitTimeEventTime", "fhDiffHitTimeEventTime;dT [ns];Entries", 200, -100., 100.);
	fhDiffFingerSciTimeEventTimeAll = new TH1D("fhDiffFingerSciTimeEventTimeAll", "fhDiffFingerSciTimeEventTimeAll;dT [ns];Entries", 200, -100., 100.);

	TString hname, htitle;
	for (Int_t iTrb = 0; iTrb < TRB_TDC3_NUMBOARDS; iTrb++){
		for (Int_t iTdc = 0; iTdc < TRB_TDC3_NUMTDC; iTdc++){
			hname.Form("fhDiffHitTimeEventTime_%d_%d", iTrb+1, iTdc);
			htitle.Form("fhDiffHitTimeEventTime_%d_%d;dT [ns];Entries", iTrb+1, iTdc);
			fhDiffHitTimeEventTime[iTrb][iTdc] = new TH1D( hname.Data(), htitle.Data(), 200, -100., 100.);
		}
	}

	return kTRUE;
}

#ifdef _NewFairSource
Int_t CbmRichTrbUnpack::ReadEvent(UInt_t) 
#else
Int_t CbmRichTrbUnpack::ReadEvent()       
#endif
{
	LOG(DEBUG) << "Event #" << fEventNum << FairLogger::endl;

	fRichHits->Clear();
	fRichHitInfos->Clear();

	if (fOutputEventTimeHits.size() == 0) {
		LOG(ERROR) << "No reference time hits." << FairLogger::endl;
		return 1;
	}

	BuildEvent(fEventNum);

	fEventNum++;

	if (fEventNum < fOutputEventTimeHits.size()){
		return 0; // still some data
	} else {
		return 1; // no more events
	}
}

void CbmRichTrbUnpack::Close()
{
   if (fDrawHist) {
		CreateAndDrawQa();
		CreateAndDrawEventBuildDisplay();
	}
	ClearAllBuffers();
}

void CbmRichTrbUnpack::Reset()
{

}

void CbmRichTrbUnpack::ReadInputFileToMemory()
{
	streampos size;
	ifstream file (fHldFileName.Data(), ios::in|ios::binary|ios::ate);
	if (file.is_open()) {
		size = file.tellg();
		fDataPointer = new Char_t[size];
		file.seekg (0, ios::beg);
		file.read (fDataPointer, size);
		file.close();
		LOG(INFO) << "The entire file content is in memory (" << size/(1024*1024) << " MB)" << FairLogger::endl;
	} else {
		LOG(FATAL) << "Unable to open file " << FairLogger::endl;
	}
	fDataSize = (UInt_t) size;
}

void CbmRichTrbUnpack::ReadEvents()
{
	CbmTrbIterator* trbIter = new CbmTrbIterator((void*)fDataPointer, fDataSize);
	Int_t maxNofRawEvents = 2000000000;
	
	// Loop through events
	while (true) {
	   // Try to extract next event from the Iterator. If no events left - go out of the loop
		CbmRawEvent* rawEvent = trbIter->NextEvent();
		if (rawEvent == NULL) break;
		fNofRawEvents++;

		if (fNofRawEvents % 10000 == 0) LOG(INFO) << "Raw event # " << fNofRawEvents << FairLogger::endl;
		//rawEvent->Print();
		fSynchRefTime = -1.;
		// Loop over subevents
		while (true){
			CbmRawSubEvent* rawSubEvent = trbIter->NextSubEvent();
			if (rawSubEvent == NULL) break;
			//rawSubEvent->Print();
			ProcessTdc(rawSubEvent);
			fNofRawSubEvents++;
		}
		// Go out if exceed the limit of total number of raw hits
		if (fNofRawEvents >= maxNofRawEvents){
			break;
		}
	}

	CreateOutputHits();
	delete[] fDataPointer;
}

void CbmRichTrbUnpack::ProcessTdc(CbmRawSubEvent* rawSubEvent)
{
	UInt_t tdcDataIndex = 0;
	UInt_t tdcData = 0x0;
	UInt_t trbId = rawSubEvent->SubId();

	while (true) {
		tdcData = rawSubEvent->SubDataValue(tdcDataIndex);
		UInt_t tdcNofWords = (tdcData >> 16) & 0xffff;
		UInt_t tdcId = tdcData & 0xffff;
		//printf("TDC DATA tdcNofWords = %i, ID = 0x%04x\n", tdcNofWords, tdcId);
		if (tdcId == 0x5555) break;
		if (tdcId == 0x7000 || tdcId == 0x7001 || tdcId == 0x7002 || tdcId == 0x7003){
			tdcDataIndex++;
			continue;
		}

		//read TDC words to array
		UInt_t dataArray[tdcNofWords];
		for (UInt_t i = 0; i < tdcNofWords; i++) {
			tdcDataIndex++;
			tdcData = rawSubEvent->SubDataValue(tdcDataIndex);
			dataArray[i] = tdcData;
		} // for tdcNofWords
		DecodeTdcData(dataArray, tdcNofWords, trbId, tdcId);
		tdcDataIndex++;
	}
}

void CbmRichTrbUnpack::DecodeTdcData(
		UInt_t* data,
		UInt_t size,
		UInt_t trbId,
		UInt_t tdcId)
{
	// decode epics data
	if (tdcId == 0x0112){
		DecodeEpicsData(data, size);
		return;
	}
	//printf("TRBID=0x%04x, TDCID=0x%04x\n", trbId, tdcId);

	CbmRichTrbParam* param = CbmRichTrbParam::Instance();
	//Bool_t isPmtHit = CbmRichTrbParam::Instance()->IsPmtTrb(trbId);
	Bool_t isPmtHit = CbmRichTrbParam::Instance()->IsPmtTdc(tdcId);
	UInt_t curEpochCounter = 0;
	queue<CbmRichTrbTimeMessage> mesQ;

	for (UInt_t i = 0; i < size; i++){
		UInt_t tdcData = data[i];
		UInt_t tdcTimeDataMarker = (tdcData >> 31) & 0x1; //1 bit

		if (tdcTimeDataMarker == 0x1) { //TIME DATA
			fNofTimeDataMessages++;
			UInt_t chNum = (tdcData >> 22) & 0x7f; // 7bits
			UInt_t fineTime = (tdcData >> 12) & 0x3ff; // 10 bits //0x3ff
			UInt_t edge = (tdcData >> 11) & 0x1; // 1bit
			UInt_t coarseTime = (tdcData) & 0x7ff; // 1bits

			// Give the calibrator the read fine time so that it was taken into account
			if ((trbId != 0x7005)) CbmTrbCalibrator::Instance()->AddFineTime(trbId, tdcId, chNum, fineTime);

			LOG(DEBUG) << "TIMEDATA chNum:" << chNum << ", fineTime:" << fineTime << ", edge:" << edge << ", coarseTime:" << coarseTime
								<< ", fullTime:" << fixed << GetFullTime(tdcId, chNum, curEpochCounter, coarseTime, fineTime) << FairLogger::endl;

			if ( param->IsSyncChannel(chNum) ) { // synchronization channel
				Double_t time = GetFullTime(tdcId, chNum, curEpochCounter, coarseTime, fineTime);
				if (fSynchRefTime == -1. ) {
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
					if  ( (fAnaType == kCbmRichBeamEvent && chNum == 5) ||           // hodoscope (beam trigger)
						  (fAnaType == kCbmRichLaserPulserEvent && chNum == 7) ||    // UV LED
						  (fAnaType == kCbmRichLedPulserEvent && chNum == 15) ){     // Laser

						CbmTrbRawHit* rawHitRef = new CbmTrbRawHit(tdcId, chNum, curEpochCounter, coarseTime, fineTime, 0, 0, 0, 0);
						fRawEventTimeHits.push_back(rawHitRef);
					}
					if (fUseFingerSci && chNum == 1) {
						CbmTrbRawHit* rawFingerScif = new CbmTrbRawHit(tdcId, chNum, curEpochCounter, coarseTime, fineTime, 0, 0, 0, 0);
						fRawFingerSciHits.push_back(rawFingerScif);
					}
				} else if ( isPmtHit ) {
					if ( param->IsLeadingEdgeChannel(chNum) ) { // leading edge
						mesQ.push( CbmRichTrbTimeMessage(chNum, curEpochCounter, coarseTime, fineTime) );
					} else { // trailing edge
						if (!mesQ.empty()) {
							CbmRichTrbTimeMessage tMes = mesQ.front();
							mesQ.pop();
							if (chNum - tMes.fChannelNum != 1) {
								//LOG(ERROR) << "Leading edge channel number - trailing edge channel number != 1. tdcId=" << hex << tdcId << dec <<
								//										", chNum=" << chNum <<  ", prevChNum=" << tMes.fChannelNum << FairLogger::endl;
								fNofHitsWithOneEdge++;
								tMes = mesQ.front();
								mesQ.pop();
							}
							if (chNum - tMes.fChannelNum == 1) {
								CbmTrbRawHit* rawHit = new CbmTrbRawHit(tdcId, tMes.fChannelNum, tMes.fEpochCounter, tMes.fCoarseTime, tMes.fFineTime,
										chNum, curEpochCounter, coarseTime, fineTime);
								fRawRichHits.push_back(rawHit);
							} else {
								LOG(ERROR) << "STILL Leading edge channel number - trailing edge channel number != 1. tdcId=" << hex << tdcId << dec <<
											", chNum=" << chNum <<  ", prevChNum=" << tMes.fChannelNum << FairLogger::endl;
								// remove all elements from queue
								queue<CbmRichTrbTimeMessage> empty;
								swap( mesQ, empty );
							}
						} else { // message queue is empty
							fNofHitsWithOneEdge++;
							//LOG(ERROR) << "MESSAGE QUEUE is empty" << FairLogger::endl;
						}
					} // is trailing edge
				} //isPmtTrb
			}//NOT SYNCH channel

			if (fineTime == 0x3ff) LOG(DEBUG) << "-ERROR- Dummy fine time registered: " << fineTime << FairLogger::endl;
		}// if TIME DATA

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

void CbmRichTrbUnpack::DecodeEpicsData(UInt_t* data, UInt_t size)
{

	if (size <= 4) return;
	CbmRichEpicsData ed;
	for (UInt_t i = 0; i < size; i++){
		if (i <= 3) continue; // MBS data and header
		UInt_t tdcData = data[i];

		if (i == 4) { // 0
			ed.fCBM_BMP180_GetTemp = (tdcData & 0x1ff) / 10.; //9 bits
			ed.fCBM_BMP180_GetPressure = (tdcData << 9) & 0x1fffff; // 21 bits
			ed.fDataFormatVersion = (tdcData << 30) & 0x3; // 2 bits
		} else if (i == 5) { // 1
			ed.fCBM_PWRSWITCH_GetCurrent00 = tdcData & 0xffff;
			ed.fCBM_PWRSWITCH_GetCurrent01 = (tdcData << 16) & 0xffff;
		} else if (i == 6) { // 2
			ed.fCBM_PWRSWITCH_GetCurrent02 = tdcData & 0xffff;
			ed.fCBM_PWRSWITCH_GetCurrent03 = (tdcData << 16) & 0xffff;
		} else if (i == 7) { // 3
			ed.fCBM_PWRSWITCH_GetCurrent04 = tdcData & 0xffff;
			ed.fCBM_PWRSWITCH_GetCurrent05 = (tdcData << 16) & 0xffff;
		} else if (i == 8) { // 4
			ed.fCBM_PWRSWITCH_GetCurrent06 = tdcData & 0xffff;
			ed.fCBM_PWRSWITCH_GetCurrent07 = (tdcData << 16) & 0xffff;
		} else if (i == 9) { // 5
			ed.fCBM_PWRSWITCH_GetCurrent08 = tdcData & 0xffff;
			ed.fCBM_PWRSWITCH_GetCurrent09 = (tdcData << 16) & 0xffff;
		} else if (i == 10) { // 6
			ed.fCBM_PWRSWITCH_GetCurrent0A = tdcData & 0xffff;
			ed.fCBM_PWRSWITCH_GetCurrent0B = (tdcData << 16) & 0xffff;
		} else if (i == 11) { // 7
			ed.fCBM_PWRSWITCH_GetCurrent0C = tdcData & 0xffff;
			ed.fCBM_PWRSWITCH_GetCurrent0D = (tdcData << 16) & 0xffff;
		} else if (i == 12) { // 8
			ed.fCBM_PWRSWITCH_GetCurrent0E = tdcData & 0xffff;
			ed.fCBM_PWRSWITCH_GetCurrent0F = (tdcData << 16) & 0xffff;
		} else if (i == 13) { // 9
			ed.fTimestampThresholdFile = tdcData;
		} else if (i == 14) { // 10
			ed.fOUTPUT_TERMINAL_VOLTAGE_U0 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U0 = (tdcData << 16) & 0xffff;
		} else if (i == 15) { // 11
			ed.fOUTPUT_TERMINAL_VOLTAGE_U1 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U1 = (tdcData << 16) & 0xffff;
		} else if (i == 16) { // 12
			ed.fOUTPUT_TERMINAL_VOLTAGE_U2 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U2 = (tdcData << 16) & 0xffff;
		} else if (i == 17) { // 13
			ed.fOUTPUT_TERMINAL_VOLTAGE_U3 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U3 = (tdcData << 16) & 0xffff;
		} else if (i == 18) { // 14
			ed.fOUTPUT_TERMINAL_VOLTAGE_U4 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U4 = (tdcData << 16) & 0xffff;
		} else if (i == 19) { // 15
			ed.fOUTPUT_TERMINAL_VOLTAGE_U5 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U5 = (tdcData << 16) & 0xffff;
		} else if (i == 20) { // 16
			ed.fOUTPUT_TERMINAL_VOLTAGE_U6 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U6 = (tdcData << 16) & 0xffff;
		} else if (i == 21) { // 17
			ed.fOUTPUT_TERMINAL_VOLTAGE_U7 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U7 = (tdcData << 16) & 0xffff;
		} else if (i == 22) { // 18
			ed.fOUTPUT_TERMINAL_VOLTAGE_U8 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U8 = (tdcData << 16) & 0xffff;
		} else if (i == 23) { // 19
			ed.fOUTPUT_TERMINAL_VOLTAGE_U9 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U9 = (tdcData << 16) & 0xffff;
		} else if (i == 24) { // 20
			ed.fOUTPUT_TERMINAL_VOLTAGE_U10 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U10 = (tdcData << 16) & 0xffff;
		} else if (i == 25) { // 21
			ed.fOUTPUT_TERMINAL_VOLTAGE_U11 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U11 = (tdcData << 16) & 0xffff;
		} else if (i == 26) { // 22
			ed.fOUTPUT_TERMINAL_VOLTAGE_U12 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U12 = (tdcData << 16) & 0xffff;
		} else if (i == 27) { // 23
			ed.fOUTPUT_TERMINAL_VOLTAGE_U13 = (tdcData & 0xffff) / 100.;
			ed.fMEASUREMENT_CURRENT_U13 = (tdcData << 16) & 0xffff;
		} else if (i == 28) { // 24
			ed.fOUTPUT_TERMINAL_VOLTAGE_U14 = (tdcData & 0xffff) / 100.;
		    ed.fMEASUREMENT_CURRENT_U14 = (tdcData << 16) & 0xffff;
		} else if (i == 29) { // 25
			ed.fOUTPUT_TERMINAL_VOLTAGE_U15 = (tdcData & 0xffff) / 100.;
		    ed.fMEASUREMENT_CURRENT_U15 = (tdcData << 16) & 0xffff;
		} else if (i == 30) { // 26
			ed.fCBM_RICH_Mirror_Pos_ActualPosition1 = (tdcData & 0xffff) / 100.;
			ed.fCBM_RICH_Mirror_Pos_ActualPosition2 = ((tdcData << 16) & 0xffff) / 100.;
		} else if (i == 31) { // 27
			ed.fCBM_RICH_Gas_O2 = (tdcData & 0xffff) / 32.;
			ed.fCBM_RICH_Gas_H2O = ((tdcData << 16) & 0xffff) / 32.;
		} else if (i == 32) { // 28
			ed.fCBM_RICH_Gas_PT_1 = (tdcData & 0xffff) / 12800.;
		    ed.fCBM_RICH_Gas_PTB = ((tdcData << 16) & 0xffff) / 32.;
		} else if (i == 33) { // 29
			ed.fCBM_RICH_Gas_PT_2 = (tdcData & 0xffff) / 32.;
			ed.fCBM_RICH_Gas_PT_3 = ((tdcData << 16) & 0xffff) / 32.;
		} else if (i == 34) { // 30
			ed.fCBM_RICH_Gas_TT_1 = (tdcData & 0xffff) / 128.;
			ed.fCBM_RICH_Gas_TT_2 = ((tdcData << 16) & 0xffff) / 128.;
		} else if (i == 35) { // 31
			ed.fCBM_RICH_Gas_PT_4 = (tdcData & 0xffff) / 16384.;
			ed.fCBM_RICH_Gas_FM_1 = ((tdcData << 16) & 0xffff) / 512.;
		} else if (i == 36) { // 32
			ed.fCBM_RICH_Gas_RefrIndex = tdcData / 128.0e6;
		} else if (i == 37) { // 33
			ed.fPadiwaThresholdOffset = (tdcData & 0xffff) - 32768;
		}
		//ed.Print();
	}//
}

CbmTrbOutputHit* CbmRichTrbUnpack::CreateOutputHit(CbmTrbRawHit* h)
{
	Double_t lFullTime = this->GetFullTime(h->GetTdc(), h->GetLChannel(), h->GetLEpoch(), h->GetLCTime(), h->GetLFTime());
	Double_t tFullTime = this->GetFullTime(h->GetTdc(), h->GetTChannel(), h->GetTEpoch(), h->GetTCTime(), h->GetTFTime());
	return new CbmTrbOutputHit(h->GetTdc(), h->GetLChannel(), lFullTime, h->GetTChannel(), tFullTime);
}

Double_t CbmRichTrbUnpack::GetFullTime(UShort_t TDC, UShort_t CH, UInt_t epoch, UShort_t coarseTime, UShort_t fineTime)
{
	Double_t coarseUnit = 5.;
	Double_t epochUnit = coarseUnit * 0x800;
    UInt_t trb_index = (TDC >> 4) & 0x00FF - 1;
    UInt_t tdc_index = (TDC & 0x000F);
	Double_t time = epoch * epochUnit + coarseTime * coarseUnit - CbmTrbCalibrator::Instance()->GetFineTimeCalibrated(trb_index, tdc_index, CH, fineTime);

	if (CH != 0){
		if (fSynchOffsetTimeMap[TDC] > 150) {
			LOG(ERROR) << "CbmRichTrbUnpack::GetFullTime Synch Offset" << fSynchOffsetTimeMap[TDC] << "ns > 150 ns for TDC" << std::hex << TDC << std::dec << FairLogger::endl;
		} else {
			time = time + fSynchOffsetTimeMap[TDC];
		}
	}

	return time;
}

void CbmRichTrbUnpack::CreateOutputHits()
{
	Int_t nofRichHits = fRawRichHits.size();
	fOutputRichHits.resize(nofRichHits);
	for (int i = 0; i < nofRichHits; i++){
		fOutputRichHits[i] = CreateOutputHit(fRawRichHits[i]);
	}

	Int_t nofRefHits = fRawEventTimeHits.size();
	fOutputEventTimeHits.resize(nofRefHits);
	for (Int_t i = 0; i < nofRefHits; i++) {
		fOutputEventTimeHits[i] = CreateOutputHit(fRawEventTimeHits[i]);
	}

	if (fUseFingerSci) {
		Int_t nofFingSciHits = fRawFingerSciHits.size();
		fOutputFingerSciHits.resize(nofFingSciHits);
		for (Int_t i = 0; i < nofFingSciHits; i++) {
			fOutputFingerSciHits[i] = CreateOutputHit(fRawFingerSciHits[i]);
		}
		std::sort(fOutputFingerSciHits.begin(), fOutputFingerSciHits.end(), CbmTrbOutputHitLeadingFullTimeComparatorLess());
	}


	std::sort(fOutputRichHits.begin(), fOutputRichHits.end(), CbmTrbOutputHitLeadingFullTimeComparatorLess());
}

void CbmRichTrbUnpack::BuildEvent(Int_t refHitIndex)
{
	CbmRichTrbParam* param = CbmRichTrbParam::Instance();
	Int_t indmin, indmax;
	Double_t eventTime = fOutputEventTimeHits[refHitIndex]->GetLFullTime();

	// check finger-scintillator for event triggering
	if (fUseFingerSci) {
		FindMinMaxIndex(eventTime, EVENT_TIME_WINDOW, fOutputFingerSciHits,&indmin, &indmax);
		Int_t size = indmax - indmin + 1;
		if (size == 0){
			return;
		} else {
			CbmTrbOutputHit* h = fOutputFingerSciHits[indmin];
			fhDiffFingerSciTimeEventTimeAll->Fill(eventTime - h->GetLFullTime());
		}
	}

	FindMinMaxIndex(eventTime, EVENT_TIME_WINDOW, fOutputRichHits,&indmin, &indmax);
	Int_t size = indmax - indmin + 1;
	UInt_t nofHitsTrb[TRB_TDC3_NUMBOARDS];
	for (UInt_t i = 0; i < TRB_TDC3_NUMBOARDS; i++) {
		nofHitsTrb[i] = 0;
	}
	for (Int_t iH = indmin; iH <= indmax; iH++) {
		CbmTrbOutputHit* h = fOutputRichHits[iH];
		CbmRichHitInfo* data = param->GetRichHitInfo(h->GetTdc(), h->GetLChannel());

		UShort_t trbInd = ( (h->GetTdc() >> 4) & 0x00FF ) - 1;
		nofHitsTrb[trbInd]++;

		AddRichHitToOutputArray(trbInd, data);

		fhDiffHitTimeEventTimeAll->Fill(eventTime - h->GetLFullTime());

		//UShort_t trbInd = ( (h->GetTrb() >> 4) & 0x00FF ) - 1;
	    UShort_t tdcInd = (h->GetTdc() & 0x000F);
		fhDiffHitTimeEventTime[trbInd][tdcInd]->Fill(eventTime - h->GetLFullTime());

		LOG(DEBUG2) << data->GetX() << " " << data->GetY() << FairLogger::endl;
		LOG(DEBUG2) <<fixed << iH << " " << hex << h->GetTdc() << dec << " " << h->GetLChannel() << " " << h->GetLFullTime() << FairLogger::endl;
	}

	for (UInt_t i = 0; i < TRB_TDC3_NUMBOARDS - 1; i++) {
		fhNofRichHitsVsTrbNum->Fill(i+1, nofHitsTrb[i]);
	}
}

void CbmRichTrbUnpack::AddRichHitToOutputArray(UShort_t trbId, CbmRichHitInfo* hitInfo)
{
	UInt_t counter1 = fRichHits->GetEntries();
	new((*fRichHits)[counter1]) CbmRichHit();
	CbmRichHit* hit = static_cast<CbmRichHit*>(fRichHits->At(counter1));
	hit->SetX(hitInfo->GetX());
	hit->SetY(hitInfo->GetY());
	hit->SetPmtId(hitInfo->GetPmtNum());

	UInt_t counter2 = fRichHitInfos->GetEntries();
	new((*fRichHitInfos)[counter2]) CbmRichHitInfo();
	CbmRichHitInfo* newHitInfo = static_cast<CbmRichHitInfo*>(fRichHitInfos->At(counter2));
	newHitInfo->Copy(hitInfo);

	if (fRichHits->GetEntries() != fRichHitInfos->GetEntries()){
		LOG(ERROR) << "CbmRichTrbUnpack::AddRichHitToOutputArray: fRichHits->Entries() =" <<
				fRichHits->GetEntries() << "NOT EQUAL fRichHitInfos->Entries() = " << fRichHitInfos->GetEntries() << FairLogger:: endl;
	}
}

void CbmRichTrbUnpack::FindMinMaxIndex(
		Double_t refTime,
		Double_t windowT,
		vector<CbmTrbOutputHit*>& hits,
		Int_t *indmin,
		Int_t *indmax)
{
	Double_t windowTime = 0.5 * windowT;
	CbmTrbOutputHit* mpnt = new CbmTrbOutputHit();
	vector<CbmTrbOutputHit*>::iterator itmin, itmax;

	mpnt->SetLeadingFullTime(refTime - windowTime);
	itmin = std::lower_bound(hits.begin(), hits.end(), mpnt, CbmTrbOutputHitLeadingFullTimeComparatorLess());

	mpnt->SetLeadingFullTime(refTime + windowTime);
	itmax = std::lower_bound(hits.begin(), hits.end(), mpnt, CbmTrbOutputHitLeadingFullTimeComparatorLess()) - 1;

	*indmin = itmin - hits.begin();
	*indmax = itmax - hits.begin();
}

void CbmRichTrbUnpack::ClearAllBuffers()
{
	for (Int_t i = 0; i < fRawRichHits.size(); i++) {
		delete fRawRichHits[i];
	}
	fRawRichHits.clear();

	for (Int_t i = 0; i < fRawEventTimeHits.size(); i++) {
		delete fRawEventTimeHits[i];
	}
	fRawEventTimeHits.clear();

	for (Int_t i = 0; i < fRawFingerSciHits.size(); i++) {
		delete fRawFingerSciHits[i];
	}
	fRawFingerSciHits.clear();

	for (Int_t i = 0; i < fOutputRichHits.size(); i++) {
		delete fOutputRichHits[i];
	}
	fOutputRichHits.clear();

	for (Int_t i = 0; i < fOutputEventTimeHits.size(); i++) {
		delete fOutputEventTimeHits[i];
	}
	fOutputEventTimeHits.clear();

	for (Int_t i = 0; i < fOutputFingerSciHits.size(); i++) {
		delete fOutputFingerSciHits[i];
	}
	fOutputFingerSciHits.clear();
}

void CbmRichTrbUnpack::FillRawHitHist(CbmTrbRawHit* rh)
{
	UShort_t trbInd = ( (rh->GetTdc() >> 4) & 0x00FF ) - 1;
	UShort_t tdcInd = (rh->GetTdc() & 0x000F);

	fhChannelEntries[trbInd][tdcInd]->Fill(rh->GetLChannel());
	fhChannelEntries[trbInd][tdcInd]->Fill(rh->GetTChannel());
	fhEpoch[trbInd][tdcInd]->Fill(rh->GetLEpoch());
	fhEpoch[trbInd][tdcInd]->Fill(rh->GetTEpoch());
	fhCoarseTime[trbInd][tdcInd]->Fill(rh->GetLCTime());
	fhCoarseTime[trbInd][tdcInd]->Fill(rh->GetTCTime());
	fhFineTime[trbInd][tdcInd]->Fill(rh->GetLFTime());
	fhFineTime[trbInd][tdcInd]->Fill(rh->GetTFTime());
}

void CbmRichTrbUnpack::FillOutputHitHist(CbmTrbOutputHit* outHit)
{
	UShort_t trbInd = ( (outHit->GetTdc() >> 4) & 0x00FF ) - 1;
	UShort_t tdcInd = (outHit->GetTdc() & 0x000F);
	fhDeltaT[trbInd][tdcInd]->Fill(outHit->GetDeltaT());
}

void CbmRichTrbUnpack::CreateAndDrawQa()
{
	LOG(INFO) << "Number of raw RICH hits = " << fRawRichHits.size() << FairLogger::endl;
	LOG(INFO) << "Number of output RICH hits = " << fOutputRichHits.size() << FairLogger::endl;
	LOG(INFO) << "Number of raw reference time hits = " << fRawEventTimeHits.size() << FairLogger::endl;
	LOG(INFO) << "Number of output reference time hits = " << fOutputEventTimeHits.size() << FairLogger::endl;
	LOG(INFO) << "Number of raw finger-sci  hits = " << fRawFingerSciHits.size() << FairLogger::endl;
	LOG(INFO) << "Number of output finger-sci hits = " << fOutputFingerSciHits.size() << FairLogger::endl;
	LOG(INFO) << "Number of hits with one edge = " << fNofHitsWithOneEdge << ", " << 100. * fNofHitsWithOneEdge / fRawRichHits.size() <<  "%"<< FairLogger::endl;
	LOG(INFO) << "Number of raw HLD events = " << fNofRawEvents << FairLogger::endl;
	LOG(INFO) << "Number of raw HLD sub events = " << fNofRawSubEvents << FairLogger:: endl;
	LOG(INFO) << "Number of time data messages = " << fNofTimeDataMessages << FairLogger::endl;

	TString hname, htitle;
	for (Int_t iTrb = 0; iTrb < TRB_TDC3_NUMBOARDS; iTrb++){
		for (Int_t iTdc = 0; iTdc < TRB_TDC3_NUMTDC; iTdc++){
			hname.Form("hChannelEntries_%d_%d", iTrb+1, iTdc);
			htitle.Form("hChannelEntries_%d_%d;Channel number;Entries", iTrb+1, iTdc);
			fhChannelEntries[iTrb][iTdc] = new TH1D( hname.Data(), htitle.Data(), 33, -0.5, 32.5);

			hname.Form("hEpoch_%d_%d", iTrb+1, iTdc);
			htitle.Form("hEpoch_%d_%d;Epoch counter;Entries", iTrb+1, iTdc);
			fhEpoch[iTrb][iTdc] = new TH1D(hname, htitle, 1000, 0, 0);

			hname.Form("hCoraseTime_%d_%d", iTrb+1, iTdc);
			htitle.Form("hCoraseTime_%d_%d;Coarse time;Entries", iTrb+1, iTdc);
			fhCoarseTime[iTrb][iTdc] = new TH1D(hname, htitle, 2048, -0.5, 2047.5);

			hname.Form("hFineTime_%d_%d", iTrb+1, iTdc);
			htitle.Form("hFineTime_%d_%d;Fine time;Entries", iTrb+1, iTdc);
			fhFineTime[iTrb][iTdc] = new TH1D(hname, htitle, 1024, -0.5, 1023.5);

			hname.Form("hDeltaT_%d_%d", iTrb+1, iTdc);
			htitle.Form("hDeltaT_%d_%d;DeltaT [ns];Entries", iTrb+1, iTdc);
			fhDeltaT[iTrb][iTdc] = new TH1D(hname, htitle, 200, -20, 40);
		}
	}

	for (int i = 0; i < fRawRichHits.size(); i++) {
		FillRawHitHist(fRawRichHits[i]);
	}

	for (int i = 0; i < fOutputRichHits.size(); i++) {
		FillOutputHitHist(fOutputRichHits[i]);
	}

	TFolder* rootHistFolder = gROOT->GetRootFolder()->AddFolder("rich_trb_unpack_debug", "rich_trb_unpack_debug");
	TString cname;
	for (Int_t iTrb = 0; iTrb < TRB_TDC3_NUMBOARDS; iTrb++){
		cname.Form("rich_trb_unpack_debug_trb%d", iTrb+1);
		TCanvas* c = new TCanvas(cname.Data(), cname.Data(), 1500, 1000);
		c->Divide(6, TRB_TDC3_NUMTDC);
		for (Int_t iTdc = 0; iTdc < TRB_TDC3_NUMTDC; iTdc++)
      {
			c->cd(6*iTdc + 1);
			DrawH1(fhChannelEntries[iTrb][iTdc]);
			c->cd(6*iTdc + 2);
			DrawH1(fhEpoch[iTrb][iTdc]);
			c->cd(6*iTdc + 3);
			DrawH1(fhCoarseTime[iTrb][iTdc]);
			c->cd(6*iTdc + 4);
			DrawH1(fhFineTime[iTrb][iTdc]);
			c->cd(6*iTdc + 5);
			DrawH1(fhDeltaT[iTrb][iTdc], kLinear, kLog);
			c->cd(6*iTdc + 6);
			DrawH1(fhDiffHitTimeEventTime[iTrb][iTdc], kLinear, kLog);

			//rootHistFolder->Add(c);

         //rootHistFolder->Add(fhChannelEntries[iTrb][iTdc]);
         //rootHistFolder->Add(fhEpoch[iTrb][iTdc]);
         //rootHistFolder->Add(fhCoarseTime[iTrb][iTdc]);
         //rootHistFolder->Add(fhFineTime[iTrb][iTdc]);
         rootHistFolder->Add(fhDeltaT[iTrb][iTdc]);

		}
	}

	TCanvas* c1 = new TCanvas("rich_trb_unpack_debug_rich_hits_vs_trb_num", "rich_trb_unpack_debug_rich_hits_vs_trb_num", 1200, 800);
	DrawH2(fhNofRichHitsVsTrbNum);

	TCanvas* c2 = new TCanvas("rich_trb_unpack_debug_diff_hittime_eventtime", "rich_trb_unpack_debug_diff_hittime_eventtime", 800, 800);
	DrawH1(fhDiffHitTimeEventTimeAll);

	if (fUseFingerSci){
		TCanvas* c = new TCanvas("rich_trb_unpack_debug_diff_fingerscitime_eventtime", "rich_trb_unpack_debug_diff_fingerscitime_eventtime", 800, 800);
		DrawH1(fhDiffFingerSciTimeEventTimeAll, kLinear, kLog);
	}

	rootHistFolder->Add(fhNofRichHitsVsTrbNum);
	rootHistFolder->Add(fhDiffHitTimeEventTimeAll);
	rootHistFolder->Write();
}

void CbmRichTrbUnpack::CreateAndDrawEventBuildDisplay()
{
	Double_t minT = fOutputEventTimeHits[0]->GetLFullTime();
	Double_t maxT = fOutputEventTimeHits[ fOutputEventTimeHits.size() - 1 ]->GetLFullTime();
	TH1D* hRefTime = new TH1D("hRefTime", "hRefTime;Time [ns];Entries", 10000, 0., maxT - minT);
	TH1D* hHitTime = new TH1D("hHitTime", "hHitTime;Time [ns];Entries", 10000, 0., maxT - minT);
	TH1D* hDiffRefTime = new TH1D("hDiffRefTime", "hDiffRefTime;dT [ns];Entries", 2000, -10., 200000);
	std::sort(fOutputEventTimeHits.begin(), fOutputEventTimeHits.end(), CbmTrbOutputHitLeadingFullTimeComparatorLess());
	for (int i = 0; i < fOutputEventTimeHits.size(); i++) {
		hRefTime->Fill(fOutputEventTimeHits[i]->GetLFullTime() - minT);

		if (i != 0) hDiffRefTime->Fill( fOutputEventTimeHits[i]->GetLFullTime() - fOutputEventTimeHits[i-1]->GetLFullTime() );
	}
	for (int i = 0; i < fOutputRichHits.size(); i++) {
		hHitTime->Fill(fOutputRichHits[i]->GetLFullTime() - minT);
	}

	TCanvas* c = new TCanvas("rich_trb_unpack_debug_ref_time", "rich_trb_unpack_debug_ref_time", 1600, 600);
	DrawH1(hRefTime);
	TCanvas* c2 = new TCanvas("rich_trb_unpack_debug_rich_hit_time", "rich_trb_unpack_debug_rich_hit_time", 1600, 600);
	DrawH1(hHitTime);
	TCanvas* c3 = new TCanvas("rich_trb_unpack_debug_delta_ref_time", "rich_trb_unpack_debug_delta_ref_time", 800, 800);
	DrawH1(hDiffRefTime);
}

ClassImp(CbmRichTrbUnpack)
