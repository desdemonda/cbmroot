#include "CbmRichTrbUnpack2.h"

//TODO check which headers are really needed

#include <iostream>
#include <map>

#include <TClonesArray.h>

#include "FairLogger.h"
#include "FairRootManager.h"

#include "CbmTrbIterator.h"
#include "CbmTrbCalibrator.h"
#include "CbmRichTrbParam.h"

#include "CbmRichTrbDefines.h"

#include "CbmTrbRawMessage.h"

// Uncoment if you want to have excessive printout (do not exectue on many events, may produce Gb's of output)
//#define DEBUGPRINT

CbmRichTrbUnpack2::CbmRichTrbUnpack2() 
: FairSource(),
  fHldFiles(),
  fNofRawEvents(0),
  fMaxNofRawEvents(2000000000),
  fDataPointer(NULL),
  fDataSize(0),
  fTrbIter(NULL),
  fTrbRawHits(new TClonesArray("CbmTrbRawMessage", 10))
{
}

CbmRichTrbUnpack2::CbmRichTrbUnpack2(TString hldFileName) 
: FairSource(),
  fHldFiles(),
  fNofRawEvents(0),
  fMaxNofRawEvents(2000000000),
  fDataPointer(NULL),
  fDataSize(0),
  fTrbIter(NULL),
  fTrbRawHits(new TClonesArray("CbmTrbRawMessage", 10))
{
   fHldFiles.push_back(hldFileName);
}

CbmRichTrbUnpack2::~CbmRichTrbUnpack2()
{
}

Bool_t CbmRichTrbUnpack2::Init()
{
	LOG(INFO) << "CbmRichTrbUnpack::Init()" << FairLogger::endl;

   FairRootManager* ioman = FairRootManager::Instance();
   if (ioman == NULL) {
      LOG(FATAL) << "No FairRootManager instance" << FairLogger::endl;
   }
   ioman->Register("CbmTrbRawMessage", "TRB raw messages", fTrbRawHits, kTRUE);

   // Read the first file
	ReadNextInputFileToMemory();

	return kTRUE;
}

void CbmRichTrbUnpack2::Close()
{
}

void CbmRichTrbUnpack2::Reset()
{
   fTrbRawHits->Clear();
}

UInt_t CbmRichTrbUnpack2::ReadNextInputFileToMemory()
{
   if (fHldFiles.size() > 0) {
      TString hldFileName = fHldFiles.front();
      fHldFiles.pop_front();

	   std::streampos size;
	   std::ifstream file (hldFileName.Data(), std::ios::in|std::ios::binary|std::ios::ate);
	   if (file.is_open()) {
		   size = file.tellg();

         if (fDataPointer) delete [] fDataPointer;
		   fDataPointer = new Char_t[size];

		   file.seekg (0, std::ios::beg);
		   file.read (fDataPointer, size);
		   file.close();
		   LOG(INFO) << "The entire file " << hldFileName << " content is in memory (" << size/(1024*1024) << " MB)" << FairLogger::endl;
	   } else {
		   LOG(FATAL) << "Unable to open file " << hldFileName << FairLogger::endl;
	   }
	   fDataSize = (UInt_t) size;

      if (fTrbIter) delete fTrbIter;
      fTrbIter = new CbmTrbIterator((void*)fDataPointer, fDataSize);

      return 0; // There is data available
   } else {
      if (fDataPointer) delete [] fDataPointer;
      if (fTrbIter) delete fTrbIter;
      return 1; // no more data
   }
}

#ifdef _NewFairSource
Int_t CbmRichTrbUnpack2::ReadEvent(UInt_t) 
#else
Int_t CbmRichTrbUnpack2::ReadEvent()       
#endif
{
   // Try to extract next event from the Iterator. If no events left - go out of the loop
	CbmRawEvent* rawEvent = fTrbIter->NextEvent();
	if (rawEvent == NULL) {  // no more data
      if (this->ReadNextInputFileToMemory() == 1) {
         return 1;
      } else {
         rawEvent = fTrbIter->NextEvent();
      }
   }

//   if (rawEvent->Size() > 8000) continue; // TODO uncomment?

	// End run if exceed the limit of total number of raw hits
	if (fNofRawEvents >= fMaxNofRawEvents) return 1; // no more data

	if (fNofRawEvents % 1000 == 0) LOG(INFO) << "Raw event # " << fNofRawEvents << FairLogger::endl;
   #ifdef DEBUGPRINT
   rawEvent->Print();
   #endif

	// Loop over subevents
	while (true)
   {
		CbmRawSubEvent* rawSubEvent = fTrbIter->NextSubEvent();
		if (rawSubEvent == NULL) break;
      #ifdef DEBUGPRINT
		rawSubEvent->PrintHeaderOnly();  // PrintHeaderOnly() or Print()
      #endif
      ProcessSubEvent(rawSubEvent);

	}

	fNofRawEvents++;

   return 0; // still some data
}

void CbmRichTrbUnpack2::ProcessSubEvent(CbmRawSubEvent* subEvent)
{
	UInt_t tdcDataIndex = 0;
	UInt_t tdcData = 0x0;
	UInt_t subEventId = subEvent->SubId();

   // Loop over subsubevents
	while (true)
   {
		tdcData = subEvent->SubDataValue(tdcDataIndex);
		UInt_t tdcNofWords = (tdcData >> 16) & 0xffff;
		UInt_t subSubEventId = tdcData & 0xffff;
      #ifdef DEBUGPRINT
		printf("%08x - ==SUBSUBEVENT HEADER    tdcNofWords = %i, ID = 0x%04x\n", tdcData, tdcNofWords, subSubEventId);
      #endif

		if (subSubEventId == 0x5555) break;

		if (  subSubEventId == 0x7000
         || subSubEventId == 0x7001
         || subSubEventId == 0x7002
         || subSubEventId == 0x7003)
      {
			tdcDataIndex++;
			continue;
		}

		// read TDC words to array
		UInt_t dataArray[tdcNofWords];
		for (UInt_t i = 0; i < tdcNofWords; i++)
      {
			tdcDataIndex++;
			tdcData = subEvent->SubDataValue(tdcDataIndex);
			dataArray[i] = tdcData;
		}

      if (subSubEventId != 0x7005 && subSubEventId != 0x0112) {
 		   ProcessSubSubEvent(dataArray, tdcNofWords, subSubEventId);
      }

		tdcDataIndex++;
	}

}

void CbmRichTrbUnpack2::ProcessSubSubEvent(UInt_t* data, UInt_t size, UInt_t subEventId)
{
	CbmRichTrbParam* param = CbmRichTrbParam::Instance();

   UInt_t tdcId = subEventId;

   UInt_t curEpochCounter = 0;

	for (UInt_t i=0; i<size; i++)
   {
		UInt_t tdcData = data[i];
      UInt_t tdcTimeDataMarker = (tdcData >> 31) & 0x1;

		if (tdcTimeDataMarker == 0x1)  // TIME DATA
      {
         #ifdef DEBUGPRINT
         printf("%08x - TIME DATA", tdcData);
         #endif
			UInt_t channel = (tdcData >> 22) & 0x7f;
         UInt_t fine = (tdcData >> 12) & 0x3ff;
         UInt_t edge = (tdcData >> 11) & 0x1;
         UInt_t coarse = (tdcData) & 0x7ff;
         UInt_t epoch = curEpochCounter;

         new( (*fTrbRawHits)[fTrbRawHits->GetEntriesFast()] )
            CbmTrbRawMessage(0, tdcId, channel, epoch, coarse, fine, edge);

         if (param->IsSyncChannel(channel)) {                   // SYNCH MESSAGE PROCESSING
            //CbmTrbCalibrator::Instance()->AddFineTime(tdcId, tdcId, channel, fine);    //TODO check - needed or not?
            #ifdef DEBUGPRINT
            printf("   --- SYNC - tdc %x ch %d edge %d epoch %08x coarse %08x fine %08x\n", tdcId, channel, edge, epoch, coarse, fine);
            #endif
         } else if (param->IsLeadingEdgeChannel(channel)) {     // LEADING EDGE PROCESSING
            CbmTrbCalibrator::Instance()->AddFineTime(tdcId, tdcId, channel, fine);
            #ifdef DEBUGPRINT
            printf("   --- LEAD - tdc %x ch %d edge %d epoch %08x coarse %08x fine %08x\n", tdcId, channel, edge, epoch, coarse, fine);
            #endif
         } else {                                              // TRAILING EDGE PROCESSING
            CbmTrbCalibrator::Instance()->AddFineTime(tdcId, tdcId, channel, fine);
            #ifdef DEBUGPRINT
            printf("   --- TRAIL- tdc %x ch %d edge %d epoch %08x coarse %08x fine %08x\n", tdcId, channel, edge, epoch, coarse, fine);
            #endif
         }

		} // if TIME DATA

		UInt_t tdcMarker = (tdcData >> 29) & 0x7; //3 bits
		if (tdcMarker == 0x1) {// TDC header
			UInt_t randomCode = (tdcData >> 16) & 0xff; // 8bits
			UInt_t errorBits = (tdcData) & 0xffff; //16 bits
         #ifdef DEBUGPRINT
         printf("%08x - TDC HEADER randomCode:0x%02x, errorBits:0x%04x\n", tdcData, randomCode, errorBits);
         #endif
		} else if (tdcMarker == 0x2) {// DEBUG
			UInt_t debugMode = (tdcData >> 24) & 0x1f; //5 bits
			UInt_t debugBits = (tdcData) & 0xffffff;//24 bits
         #ifdef DEBUGPRINT
			printf("%08x - DEBUG debugMode:%i, debugBits:0x%06x\n", tdcData, debugMode, debugBits);
         #endif
		} else if (tdcMarker == 0x3){ // EPOCH counter
			curEpochCounter = (tdcData) & 0xfffffff; //28 bits
         #ifdef DEBUGPRINT
         printf("%08x - EPOCH COUNTER epochCounter:0x%07x\n", tdcData, curEpochCounter);
         #endif
      } else {
         #ifdef DEBUGPRINT
         if (tdcTimeDataMarker != 0x1) printf("%08x\n", tdcData);
         #endif
		}

	} // for loop

}

ClassImp(CbmRichTrbUnpack2)
