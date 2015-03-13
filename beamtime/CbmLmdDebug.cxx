 /** @file CbmLmdDebug.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17.12.2014
 **/


#include "CbmLmdDebug.h"

#include <iostream>
#include <iomanip>

#include "TStopwatch.h"

using namespace std;




// -----   Constructor   ---------------------------------------------------
CbmLmdDebug::CbmLmdDebug(const char* fileName) :
	fLmdFileName(fileName),
	fRocIter(),
	fCurrentMessage(),
	fCurrentEpoch(),
	fNofHitNxy(),
	fNofSync(),
	fLastMsgType(-1),
	fLastMsgRocId(-1),
	fLastMsgNxyId(-1),
	fLastMsgTime(0),
	fMaxTimeDisorder(0),
	fTimeStart(0),
	fTimeStop(0)
{
	for (Int_t iType = 0; iType < 8; iType++) fNofMsgType[iType] = 0;
	Init();
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmLmdDebug::~CbmLmdDebug() {
}
// -------------------------------------------------------------------------



// -----   Initialisation   ------------------------------------------------
Bool_t CbmLmdDebug::Init() {

	// --- Open input file
  fRocIter = new roc::Iterator(fLmdFileName.Data());

  // --- Get first message
  if ( ! fRocIter->next() ) {
    LOG(ERROR) << GetName() << "::Init: input file does not exist "
	             << "or is empty!" << FairLogger::endl;
    return kFALSE;
  }
  fCurrentMessage = &fRocIter->msg();

  return kTRUE;
}
// -------------------------------------------------------------------------


// -----   Execution   -----------------------------------------------------
Int_t CbmLmdDebug::Run(Int_t maxMessages) {

  // --- Message counter and time
	TStopwatch timer;
	timer.Start();
	Int_t nMessages = 0;

  // --- Signal end of file if there are no more messages
  if ( ! fCurrentMessage ) {
    LOG(INFO) << GetName() << ": End of input file reached."
              << FairLogger::endl;
    return nMessages;
  }

  // --- Message loop
  while ( fCurrentMessage ) {
    nMessages++;

    // --- Message type, ROC and NXYTER ID
    Int_t msgType = fCurrentMessage->getMessageType();
    Int_t rocId   = fCurrentMessage->getRocNumber();
    Int_t nxyId   = fCurrentMessage->getNxNumber();

    // --- If EPOCH message: update current epoch for the ROC
    if ( fCurrentMessage->isEpochMsg() ) {
    	fCurrentEpoch[rocId] = fCurrentMessage->getEpochNumber();
    	LOG(DEBUG2) << "EPOCH " << fCurrentMessage->getEpochNumber()
    			       << " for ROC " << rocId << FairLogger::endl;
    }


    // --- Message type counter
    fNofMsgType[msgType]++;

    // --- Hit counter per NXYTER
    if ( fCurrentMessage->isHitMsg() ) {
    	pair<Int_t, Int_t> rocnx(rocId, nxyId);
    	fNofHitNxy[rocnx]++;
    }

    // --- Sync counter per ROC
    if ( fCurrentMessage->isSyncMsg() ) fNofSync[rocId]++;

    // --- Message full time
    ULong_t msgTime = fCurrentMessage->getMsgFullTime(fCurrentEpoch[rocId]);


    // --- Update run start and stop time
    if ( nMessages == 1 ) fTimeStart = msgTime;
    fTimeStop = TMath::Max(fTimeStop, msgTime);

    // --- Message to screen for proper LOG level
    switch ( msgType ) {
    	case roc::MSG_SYNC:
    		LOG(DEBUG) <<  "SYNC   Message at t = " << msgTime << " ROC " << rocId
          				 << " NXYTER " << nxyId << FairLogger::endl; break;
    	case roc::MSG_SYS:
    		LOG(DEBUG) <<  "SYS    Message at t = " << msgTime << " ROC " << rocId
          				 << " NXYTER " << nxyId << FairLogger::endl; break;
    	case roc::MSG_EPOCH:
      	LOG(DEBUG1) << "EPOCH  Message at t = " << msgTime << " ROC " << rocId
      	            << " NXYTER " << nxyId << " epoch number "
      	            << fCurrentMessage->getEpochNumber() << FairLogger::endl; break;
    	case roc::MSG_HIT:
    		LOG(DEBUG2) << "HIT    Message at t = " << msgTime << " ROC " << rocId
          				  << " NXYTER " << nxyId << FairLogger::endl; break;
    	case roc::MSG_AUX:
    		LOG(DEBUG2) << "AUX    Message at t = " << msgTime << " ROC " << rocId
          				  << " NXYTER " << nxyId << FairLogger::endl; break;
    	default: break;
    }

    // --- Compare time to previous message
    Long64_t tDif = fLastMsgTime - msgTime;
    if ( tDif > 0 ) { // Message is before previous message
    	fMaxTimeDisorder = TMath::Max(fMaxTimeDisorder, tDif);
    	if ( tDif > 1e5 ) {
    		LOG(DEBUG3) << "Time disorder by " << tDif << " ns " << FairLogger::endl;
    		LOG(DEBUG3) << "Previous message: type " << fLastMsgType
    				       << " ROC " << fLastMsgRocId << " NXYTER " << fLastMsgNxyId
    				       << " time " << fLastMsgTime << FairLogger::endl;
    		LOG(DEBUG3) << "Current message: type " << msgType
    				       << " ROC " << rocId << " NXYTER " << nxyId
    				       << " time " << msgTime << FairLogger::endl << FairLogger::endl;
    	}	//? disorder > 100 mus
    } //? message before previous message

    // --- Store message info for comparison
    SetLastMessage();

    // --- Get next ROC message
    if ( fRocIter->next() ) fCurrentMessage = &fRocIter->msg();
    else fCurrentMessage = NULL;

    // --- Stop if required number of messages are read
    if ( maxMessages >= 0 && nMessages >= maxMessages ) break;
  }	// message loop


  timer.Stop();
  LOG(INFO) << "End of run; " << nMessages << " messages read"
  		      << FairLogger::endl;
  LOG(INFO) << "Start time : " << fixed << setprecision(4) << fTimeStart * 1.e-9 << " s  ";
  LOG(INFO) << "Stop  time : " << fTimeStop  * 1.e-9 << " s  ";
  Double_t deltaT = (fTimeStop - fTimeStart) / 1.e9;
  LOG(INFO) << "Duration   : " << deltaT  << " s" << FairLogger::endl;

  LOG(INFO) << "Max. time disorder " << fMaxTimeDisorder << " ns "
  		      << FairLogger::endl;
  PrintStats();
  LOG(INFO) << "CPU time " << timer.CpuTime() << " s, real time "
  		      << timer.RealTime() << " s" << FairLogger::endl;
  return nMessages;
}
// -------------------------------------------------------------------------



/// -----  Statistics   ----------------------------------------------------
void CbmLmdDebug::PrintStats() {

	// ---  Messages per Type
	const char* names[8]
	    = { "NOP", "HIT", "EPOCH", "SYNC", "AUX", "EPOCH2", "GET4", "SYS" };
	LOG(INFO) << FairLogger::endl;
	LOG(INFO) << "Messages per message type: " << FairLogger::endl;
	for (Int_t i=0; i<8; i++)
  LOG(INFO) << setw(6) << names[i] << "  "
            << setw(10) << right << fNofMsgType[i] << FairLogger::endl;

	// --- Hit messages per NXYTER
  LOG(INFO) << FairLogger::endl;
  LOG(INFO) << "Hit messages per ROC/NXYTER: " << FairLogger::endl;
  map<pair<Int_t, Int_t>, UInt_t>::iterator itHit;
  for (itHit = fNofHitNxy.begin(); itHit != fNofHitNxy.end(); itHit++) {
  	Int_t rocId = itHit->first.first;
  	Int_t nxyId = itHit->first.second;
  	UInt_t nMsg = itHit->second;
  	LOG(INFO) << " ROC " << setw(2) << rocId << "  NXYTER " << nxyId
  			      << "  HIT " << setw(12) << right << nMsg
  			      << FairLogger::endl;
  }

	// --- Sync messages per ROC
  LOG(INFO) << FairLogger::endl;
  LOG(INFO) << "Sync messages per ROC: " << FairLogger::endl;
  map<Int_t, UInt_t>::iterator itSync;
  for (itSync = fNofSync.begin(); itSync != fNofSync.end(); itSync++) {
  	Int_t rocId = itSync->first;
  	UInt_t nMsg = itSync->second;
  	LOG(INFO) << " ROC " << setw(2) << rocId
  			      << "  SYNC " << setw(12) << right << nMsg
  			      << FairLogger::endl;
  }
  LOG(INFO) << FairLogger::endl;

}
// -------------------------------------------------------------------------


// -----   Store message info   --------------------------------------------
void CbmLmdDebug::SetLastMessage() {
	fLastMsgType  = fCurrentMessage->getMessageType();
	fLastMsgRocId = fCurrentMessage->getRocNumber();
	fLastMsgNxyId = fCurrentMessage->getNxNumber();
	fLastMsgTime  = fCurrentMessage->getMsgFullTime(fCurrentEpoch[fLastMsgRocId]);
}
// -------------------------------------------------------------------------
