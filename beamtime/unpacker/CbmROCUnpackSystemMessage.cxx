// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                       CbmROCUnpackSystemMessage                   -----
// -----                    Created 07.11.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
#include "CbmROCUnpackSystemMessage.h"

#include "CbmAuxDigi.h"
#include "CbmTbDaqBuffer.h"
#include "CbmSourceLmdNew.h"

#include "FairRunOnline.h"
#include "FairLogger.h"

#include <iomanip>

CbmROCUnpackSystemMessage::CbmROCUnpackSystemMessage()
  : CbmROCUnpack(),
    fBuffer(CbmTbDaqBuffer::Instance()),
    fSource(NULL)
{
}

CbmROCUnpackSystemMessage::~CbmROCUnpackSystemMessage()
{
}


Bool_t CbmROCUnpackSystemMessage::Init()
{
  FairRunOnline* online = FairRunOnline::Instance();
  fSource = static_cast<CbmSourceLmdNew*>(online->GetSource());
  return kTRUE;
}


Bool_t CbmROCUnpackSystemMessage::DoUnpack(roc::Message* message, ULong_t hitTime)
{

  if (message->getSysMesType() == roc::SYSMSG_USER) {
    // --- Get absolute time and Roc number
    CbmAuxDigi* digi;
    Int_t rocId = message->getRocNumber();

    switch(message->getSysMesData()) {
    case roc::SYSMSG_USER_CALIBR_ON:
      {
	if ( !fSource->IsBaselineFill() ) {
	  // --- Create AuxDigi and send it to the buffer
	  digi = new CbmAuxDigi(666, 666, hitTime);
	  fBuffer->InsertData(digi);
	  LOG(INFO) << " Switching now to baseline mode at " << 
	    setprecision(9) << Double_t(hitTime) * 1.e-9 << " s" << FairLogger::endl;
	  fSource->AddBaselineRoc(rocId);
	  LOG(INFO) << "ROC " << rocId << " now in baseline mode" << FairLogger::endl;
	  fSource->SetBaselineFill(kTRUE);
	}
	fSource->AddBaselineRoc(rocId);
	LOG(INFO) << "ROC " << rocId << " now in baseline mode" << FairLogger::endl;
	break;
      }
    case roc::SYSMSG_USER_CALIBR_OFF:
      {
	if ( fSource->IsBaselineFill() ) {
	  Bool_t isEmpty=fSource->RemoveBaselineRoc(rocId);
	  LOG(INFO) << "ROC " << rocId << " now in normal mode" << FairLogger::endl;
	  if ( isEmpty ) {
	    digi = new CbmAuxDigi(999, 999, hitTime);
	    fBuffer->InsertData(digi);
	    fSource->SetBaselineFill(kFALSE);
	    LOG(INFO) << "Switching back to normal mode at " << 
	      setprecision(9) << Double_t(hitTime) * 1.e-9 << " s" << FairLogger::endl;
	  }     
	} 
	break;
      }
    case roc::SYSMSG_USER_RECONFIGURE:
      {
	LOG(DEBUG) << "Found USER_RECONFIGURE at ";// << FairLogger::endl;
	break;
      }
    default:
      {
	LOG(ERROR) << "Found unknown system message at ";// << FairLogger::endl;
	break;
      }
    }
  }
  
  return kTRUE;
}


void CbmROCUnpackSystemMessage::Reset()
{
}


ClassImp(CbmROCUnpackSystemMessage)
