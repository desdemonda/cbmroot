// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                            CbmSourceLmdNew                        -----
// -----                    Created 10.11.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "CbmSourceLmdNew.h"

#include "CbmAuxDigi.h"
#include "CbmStsDigi.h"
#include "CbmMuchBeamTimeDigi.h"
#include "fhodo/CbmFiberHodoDigi.h"

#include "CbmDaqMap.h"
#include "CbmTbDaqBuffer.h"
#include "CbmTbEvent.h"
#include "CbmTbEventBuilder.h"

#include "FairLogger.h"
#include "FairRunOnline.h"
#include "FairEventHeader.h"

#include "TClonesArray.h"

#include <iomanip>

using std::map;

CbmSourceLmdNew::CbmSourceLmdNew()
  : FairSource(),
    fInputFileName(""),
    fInputFileList(),
    fFileCounter(0),
    fReadInTimeStep(1e9),
    fPersistence(kTRUE),
    fAuxOffset(1000),
    fBuffer(CbmTbDaqBuffer::Instance()),
    fDaqMap(NULL),
    fRocIter(NULL),
    fEventBuilder(NULL),
    fTimeStart(0),
    fTimeStop(0),
    fCurrentEpochTime(0),
    fStartEpoch(0),
    fCurrentEpoch(),
    fCurrentBufferTime(0),
    fCurrentMessage(NULL),
    fTimeBufferFill(0),
    fTimeBufferOut(0.),
    fCurrentDigi(NULL),
    fNofMessages(0),
    fNofEpochs(0),
    fNofEvents(0),
    fNofMessType(),
    fNofMessRoc(),
    fNofHitMsg(),
    fNofDigis(),
    fNofBLDigis(),
    fNofDiscardedDigis(),
    fNofAux(0),
    fNofDiscardedAux(0),
    fNofIgnoredMessages(0),
    fBaselineDataFill(kFALSE),
    fBaselineDataRetrieve(kFALSE),
    fBaselineRoc(),
  fTriggeredMode(kFALSE),
  fUnpackers(),
  fNofBaselineDigis(0)
{
  // --- Initialise counters
  for (Int_t iType = 0; iType < 8; iType++) { fNofMessType[iType] = 0; }
  for (Int_t iRoc = 0; iRoc < 20; iRoc++)
    for (Int_t iNx = 0; iNx < 5; iNx++ ) {
      fNofMessRoc[iRoc][iNx] = 0;
    }
}

CbmSourceLmdNew::CbmSourceLmdNew(const char* inFile)
  : FairSource(),
    fInputFileName(inFile),
    fInputFileList(new TObjString(inFile)),
    fFileCounter(0),
    fReadInTimeStep(1e9),
    fPersistence(kTRUE),
    fAuxOffset(1000),
    fBuffer(CbmTbDaqBuffer::Instance()),
    fDaqMap(NULL),
    fRocIter(NULL),
    fEventBuilder(NULL),
    fTimeStart(0),
    fTimeStop(0),
    fCurrentEpochTime(0),
    fStartEpoch(0),
    fCurrentEpoch(),
    fCurrentBufferTime(0),
    fCurrentMessage(NULL),
    fTimeBufferFill(0),
  fTimeBufferOut(0.),
  fCurrentDigi(NULL),
    fNofMessages(0),
    fNofEpochs(0),
    fNofEvents(0),
    fNofMessType(),
    fNofMessRoc(),
    fNofHitMsg(),
    fNofDigis(),
    fNofBLDigis(),
    fNofDiscardedDigis(),
    fNofAux(0),
    fNofDiscardedAux(0),
    fNofIgnoredMessages(0),
    fBaselineDataFill(kFALSE),
    fBaselineDataRetrieve(kFALSE),
    fBaselineRoc(),
  fTriggeredMode(kFALSE),
  fUnpackers(),
  fNofBaselineDigis(0)
{
  // --- Initialise counters
  for (Int_t iType = 0; iType < 8; iType++) { fNofMessType[iType] = 0; }
  for (Int_t iRoc = 0; iRoc < 20; iRoc++)
    for (Int_t iNx = 0; iNx < 5; iNx++ ) {
      fNofMessRoc[iRoc][iNx] = 0;
    }
}


CbmSourceLmdNew::CbmSourceLmdNew(const CbmSourceLmdNew& source)
  : FairSource(source)
{
}


CbmSourceLmdNew::~CbmSourceLmdNew()
{
  //  delete fDaqMap;
}


// -----   Fill the buffer up to a given time   -----------------------------
Bool_t CbmSourceLmdNew::FillBuffer(ULong_t time)
{

  // --- Signal end of file if there are no more messages
  if ( ! fCurrentMessage ) {
    LOG(INFO) << GetName() << ": End of input file reached."
              << FairLogger::endl;
    return kFALSE;
  }

  Int_t nMessages = 0;
  while ( fCurrentMessage ) {
    nMessages++;
    
    // Extract needed info from the message
    Int_t msgType = fCurrentMessage->getMessageType();
    Int_t rocId = fCurrentMessage->getRocNumber();
    Int_t nxyId = fCurrentMessage->getNxNumber();
    ULong_t hitTime = fCurrentMessage->getMsgFullTime(fCurrentEpoch[rocId]);
    
    // --- Jump out of loop of hit time is after time limit
    if (hitTime > time) { break; }

    // Ignore all messages with unknown system ID
    Int_t systemId = fDaqMap->GetSystemId(rocId);
    if ( systemId < 0 ) {
    	fNofIgnoredMessages++;
    	LOG(DEBUG) << "ROC id " << rocId << " system ID " << systemId
    			       << ": ignoring message" << FairLogger::endl;
    	GetNextMessage();
    	continue;
    }

    // Count number of messages for the different types
    fNofMessType[msgType]++;
    
    // To define a unique number needed for calling the correct
    // unpacker calculate such a number from the message type (which
    // is 1 for hit messages and the unique system id)
    if ( fCurrentMessage->isHitMsg() ) {
      // --- ROC number defines the system
      fNofHitMsg[systemId]++;
      fNofMessRoc[rocId][nxyId]++;
      if ( systemId >= 0 ) msgType = msgType*10 + systemId;
    }
    
    // call the registered unpacker for the 
    // message type of the current message
    std::map<Int_t, CbmROCUnpack*>::iterator it=fUnpackers.find(msgType);
    if (it == fUnpackers.end()) {
      LOG(ERROR) << "Skipping message with unknown type " 
		 << msgType << " ROC " << rocId << FairLogger::endl;
      continue;
    } else {
      it->second->DoUnpack(fCurrentMessage, hitTime);
    }
    
    GetNextMessage();

  } //- message loop
  
  // --- Update buffer fill time
  fTimeBufferFill  = time;
  
  // --- Set buffer retrieval time to one time step before fill time.
  fTimeBufferOut  = Double_t(time - fReadInTimeStep);
  
  // --- End of input: retrieval time equals fill time
  if ( ! fCurrentMessage ) {
    LOG(INFO) << GetName() << ": End of input reached." << FairLogger::endl;
    fTimeBufferOut = Double_t(time);
  }
  
  // --- Status info
  LOG(INFO) << GetName() << ": Buffer fill time " << fixed
            << setprecision(3) << Double_t(fTimeBufferFill) * 1.e-9
            << " s, retrieval up to " << fTimeBufferOut * 1.e-9
            << " s, " << nMessages << " messages processed"
            << FairLogger::endl;
  fBuffer->PrintStatus();
  fNofMessages += nMessages;
  
  return kTRUE;
}
// --------------------------------------------------------------------------



// -----   Get next data   --------------------------------------------------
CbmDigi* CbmSourceLmdNew::GetNextData()
{

  // --- Retrieve next data from the buffer.
  CbmDigi* digi = fBuffer->GetNextData(fTimeBufferOut);

  // --- If no data: fill the buffer with next time step
  // --- N.b.: fCurrentMessage = NULL means no more data in input and
  // ---       retrieval of buffer up to the fill time.
  while ( ( ! digi )  &&  fCurrentMessage ) {
    FillBuffer(fTimeBufferFill + fReadInTimeStep);
    digi = fBuffer->GetNextData(fTimeBufferOut);
  }

  // --- If the digi pointer is NULL; the input is exhausted and the buffer
  // --- is empty.
  return digi;
}
// --------------------------------------------------------------------------


Bool_t CbmSourceLmdNew::GetNextMessage() {

  if ( fRocIter->next() ) fCurrentMessage = &fRocIter->msg();

  else  { // No new message

    // Check if there is another file in the list of files
    // and open this file if one exits
    fFileCounter += 1;
    if ( fInputFileList.GetSize() > fFileCounter ) {
    	Bool_t success = OpenInputFileAndGetFirstMessage();
      if (!success) {
      	fCurrentMessage = NULL;
      	return kFALSE;
      }
    }
    else fCurrentMessage = NULL;

  } //? No new message
  return kTRUE;



}
// --------------------------------------------------------------------------



// -----   Initialisation   -------------------------------------------------
Bool_t CbmSourceLmdNew::Init()
{

  LOG(INFO) << FairLogger::endl;
  LOG(INFO) << "====================================================="
            << FairLogger::endl;
  LOG(INFO) << GetName() << ": Initialising ..." << FairLogger::endl;

  if ( NULL == fEventBuilder ) {
    LOG(FATAL) << "No event builder defined." << FairLogger::endl;
  }

  fEventBuilder->Init();

  if ( NULL == fDaqMap ) {
    LOG(FATAL) << "No DaqMap defined." << FairLogger::endl;
  }

  // Call the init of all registered unpackers
  for (std::map<Int_t, CbmROCUnpack*>::iterator it=fUnpackers.begin(); it!=fUnpackers.end(); ++it) {
    LOG(INFO) << "Initialize " << it->second->GetName() << 
      " for message type " << it->first << FairLogger::endl;
    it->second->Init();
    it->second->SetPersistence(fPersistence);
  }

  Bool_t success=OpenInputFileAndGetFirstMessage();
  if (!success) return kFALSE;

  if ( fCurrentMessage->getMessageType() != roc::MSG_EPOCH ) {
    LOG(ERROR) << GetName() << ": First message is not epoch marker!"
               << FairLogger::endl;
    return kFALSE;
  }

  // --- Get start time and set initial buffer fill time
  Int_t epoch  = fCurrentMessage->getEpochNumber();
  ULong_t time = fCurrentMessage->getMsgFullTime(epoch);
  LOG(INFO) << GetName() << ": First message: epoch " << epoch << "  time "
            << setprecision(9) << Double_t(time) * 1.e-9 << " s"
            << FairLogger::endl;

  // --- Set initial buffer fill time (rounded to next read-in time step)
  fTimeBufferFill = ULong_t( Double_t(time) / Double_t(fReadInTimeStep) )
                    * fReadInTimeStep;
  fTimeBufferOut  = fTimeBufferFill - fReadInTimeStep;

  // --- Read the first digi from the buffer
  fCurrentDigi = GetNextData();
  if ( ! fCurrentDigi ) {
    LOG(ERROR) << GetName() << ": No hit data in input!" << FairLogger::endl;
    return kFALSE;
  }

  LOG(INFO) << GetName() << ": Initialisation done. " << FairLogger::endl;
  LOG(INFO) << "====================================================="
            << FairLogger::endl;
  LOG(INFO) << FairLogger::endl;

  return kTRUE;
}
// --------------------------------------------------------------------------



// -----   Event loop   -----------------------------------------------------
// Action: Digis are retrieved from the buffer one by one.
// If their time is within the association window, they are added to the
// event. If not, the event loop is stopped.
Int_t CbmSourceLmdNew::ReadEvent()
{
  
  // The TClonesArrays and everything else is cleared when FairRunOnline
  // calls the Reset function right before calling ReadEvent
  
  // Loop over digis
  while ( kTRUE ) {
    
    // here one should call a more generic function from a separate
    // class which defines if a digi belonges to the event or not

    if ( fEventBuilder->IsInEvent(fCurrentDigi) ) {

      // --- Copy digi to output array
      Int_t systemId = fCurrentDigi->GetSystemId();
      Int_t msgType = -1;
      if ( 999 == systemId ) { // hardcoded number
	Int_t val = dynamic_cast<CbmAuxDigi*>(fCurrentDigi)->GetRocId();
	if ( val == 666 ) {
	  CbmTbEvent* CurrentEvent = 
	    dynamic_cast<CbmTbEvent*> (FairRunOnline::Instance()->GetEventHeader());

	  LOG(INFO) << "Event type is now: " 
		    << CurrentEvent->GetEventType() << FairLogger::endl;
	  CurrentEvent->SetEventType(1);
	  LOG(INFO) << "Event type is now: " 
		    << CurrentEvent->GetEventType() << FairLogger::endl;
	  fBaselineDataRetrieve = kTRUE; 
	  LOG(INFO) << "In Old Aux RocId: "<< val<<FairLogger::endl;
	  //	  FillBaselineDataContainer();
	  fNofEvents++;
          fNofBaselineDigis = fNofDigis[kTutDet];
          // remove special aux digi which should not end up in output
	  delete fCurrentDigi;
	  fCurrentDigi = GetNextData();
	  return 0;
	} else if ( val  == 999 ) {
	  fBaselineDataRetrieve = kFALSE; 
	  LOG(INFO) << "Aux RocId: "<< val<<FairLogger::endl;
	  LOG(INFO) << "Filling " 
		    << ( fNofDigis[kTutDet] - fNofBaselineDigis ) 
		    << " for this baseline event" 
		    << FairLogger::endl;
          // remove special aux digi which should not end up in output
	  delete fCurrentDigi;
	  fCurrentDigi = GetNextData();
	  return 0;
	} else {
	  msgType = 4; // This is an Aux digi
	  fNofAux++;
	}
      } else {
	msgType = systemId + 10; // to get the correct unpacker
      }
      
      std::map<Int_t, CbmROCUnpack*>::iterator it=fUnpackers.find(msgType);
      if (it == fUnpackers.end()) {
	LOG(ERROR) << "Skipping digi with unknown id " 
		   << msgType << FairLogger::endl;
	continue;
      } else {
	it->second->FillOutput(fCurrentDigi);
        if ( kTRUE == fBaselineDataRetrieve ) {
	  fNofDigis[kTutDet]++;
	  fNofBLDigis[systemId]++;
	} else {
	  fNofDigis[systemId]++;
	}
      }
  
      
      // --- Delete current digi from memory and get next one
      delete fCurrentDigi;
      fCurrentDigi = GetNextData();
      if ( ! fCurrentDigi ) {
	LOG(INFO) << "No more input data. End the run." << FairLogger::endl;
	return 1;  // no more data; trigger end of run
      }
    }   //? Digi belongs to event?
    
    // --- If digi does not belong to current event: stop event loop.
    else {
      if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG1) ) {
	fEventBuilder->PrintCurrentEvent();
      }
      fNofEvents++;
      return 0;
    }

  } //- event loop over digis

  LOG(INFO) << "===== Leaving Exec. " << FairLogger::endl;

  return 0;
}
// --------------------------------------------------------------------------

void CbmSourceLmdNew::Close()
{

  // Call the finish function for all registered unpackers
  // write histos, etc.
  for (std::map<Int_t, CbmROCUnpack*>::iterator it=fUnpackers.begin(); it!=fUnpackers.end(); ++it) {
    it->second->Finish();
  }

  LOG(INFO) << FairLogger::endl;
  LOG(INFO) << "====================================================="
            << FairLogger::endl;
  fBuffer->PrintStatus();
  LOG(INFO) << GetName() << ": Run Summary" << FairLogger::endl;

  LOG(INFO) << FairLogger::endl;
  LOG(INFO) << "Messages in input: " << fNofMessages << FairLogger::endl;
  Double_t deltaT = (fTimeStop - fTimeStart) / 1.e9;
  LOG(INFO) << "Start time : " << fTimeStart * 1.e-9 << " s  ";
  LOG(INFO) << "Stop  time : " << fTimeStop  * 1.e-9 << " s  ";
  LOG(INFO) << "Duration   : " << deltaT  << " s" << FairLogger::endl;
  LOG(INFO) << "Ignored messages: " << fNofIgnoredMessages << FairLogger::endl;


  const char* names[8] = { "NOP", "HIT", "EPOCH", "SYNC", "AUX", "EPOCH2", "GET4", "SYS" };
  LOG(INFO) << FairLogger::endl;
  LOG(INFO) << "Messages per message type: " << FairLogger::endl;
  for (Int_t i=0; i<8; i++)
    LOG(INFO) << setw(6) << names[i] << "  "
              << setw(10) << right << fNofMessType[i] << FairLogger::endl;

  LOG(INFO) << FairLogger::endl;
  LOG(INFO) << "Hit messages per ROC/NXYTER: " << FairLogger::endl;
  for (Int_t iRoc = 0; iRoc < 13; iRoc ++)
    for (Int_t iNx = 0; iNx < 3; iNx +=2 ) {
      LOG(INFO) << " ROC " << setw(2) << iRoc << "  NXYTER " << iNx << "  Messages "
                << setw(12) << right << fNofMessRoc[iRoc][iNx] << "  Rate "
                << setw(12) << setprecision(4) << fixed  << right
                << Double_t(fNofMessRoc[iRoc][iNx]) / deltaT
                << " /s " << FairLogger::endl;
    }

  LOG(INFO) << FairLogger::endl;
  LOG(INFO) << "Hit messages and digis per system: " << FairLogger::endl;
  for (Int_t iSys = 0; iSys < kNOFDETS; iSys++) {
    TString sysName;
    CbmDetectorList::GetSystemNameCaps(iSys, sysName);
    LOG(INFO) << setw(5) << sysName << ": Messages " << fNofHitMsg[iSys]
              << ", Digis " << fNofDigis[iSys] 
	      << ", BaselineDigis " << fNofBLDigis[iSys] 
	      << ", discarded Digis " << fNofDiscardedDigis[iSys] << FairLogger::endl;
  }
  LOG(INFO) << "AUX  : Messages " << fNofMessType[roc::MSG_AUX] << ", Digis "
            << fNofAux << ", discarded Digis " << fNofDiscardedAux 
	    << FairLogger::endl;


  LOG(INFO) << "Total number of events: " << fNofEvents << FairLogger::endl;

  delete fRocIter;
}

void CbmSourceLmdNew::Reset()
{
  // Call the init of all registered unpackers
  for (std::map<Int_t, CbmROCUnpack*>::iterator it=fUnpackers.begin(); it!=fUnpackers.end(); ++it) {
    it->second->Reset();
  }
  
  fEventBuilder->Reset();
}


void CbmSourceLmdNew::AddBaselineRoc(Int_t rocNr) {
  // fBaselineData is set to kTRUE after the first call of this function
  // so the two rocs which are triggered should be set to basline mode
  if ( fTriggeredMode && !fBaselineDataFill ) {
    fBaselineRoc.insert(11);
    fBaselineRoc.insert(12);
    LOG(INFO) << "Triggered mode" << FairLogger::endl;
    LOG(INFO) << "ROC 11" << " now in baseline mode" << FairLogger::endl;
    LOG(INFO) << "ROC 12" << " now in baseline mode" << FairLogger::endl;
  }
  fBaselineRoc.insert(rocNr);
}

Bool_t CbmSourceLmdNew::RemoveBaselineRoc(Int_t rocNr)
{
  // Triggered mode and triggered rocs still in set
  if (fTriggeredMode && fBaselineRoc.find(11) != fBaselineRoc.end()) {
    fBaselineRoc.erase(11);
    fBaselineRoc.erase(12);
    LOG(INFO) << "Triggered mode" << FairLogger::endl;
    LOG(INFO) << "ROC 11" << " now in normal mode" << FairLogger::endl;
    LOG(INFO) << "ROC 12" << " now in normal mode" << FairLogger::endl;
  }
  fBaselineRoc.erase(rocNr);
  Bool_t empty = fBaselineRoc.empty();  
  return empty;  
}

Bool_t CbmSourceLmdNew::IsBaselineFill(Int_t rocNr)
{
  std::set<Int_t>::iterator it;
  if ( fBaselineRoc.find(rocNr) == fBaselineRoc.end() ) {
    return kFALSE;
  } else {
    return kTRUE;
  }
}

Bool_t CbmSourceLmdNew::OpenInputFileAndGetFirstMessage()
{
  TObjString* tmp =
    dynamic_cast<TObjString*>(fInputFileList.At(fFileCounter));
  fInputFileName = tmp->GetString();
  LOG(INFO) << GetName() << ": Opening file "
	    << fInputFileName
	    << FairLogger::endl;
  // Normally one should delete the old object before, but this
  // doesn't work and result in a crash when creating a new iterator
  fRocIter = new roc::Iterator(fInputFileName.Data());
  if ( ! fRocIter->next() ) {
    LOG(ERROR) << GetName() << "::Init: input file does not exist "
	       << "or is empty!" << FairLogger::endl;
    return kFALSE;
  }
  fCurrentMessage = &fRocIter->msg();

  return kTRUE;
}  


ClassImp(CbmSourceLmdNew)
