#// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           CbmROCUnpackSts                         -----
// -----                    Created 13.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
#include "CbmROCUnpackSts.h"

#include "CbmStsDigi.h"
#include "CbmTbDaqBuffer.h"
#include "CbmSourceLmdNew.h"
#include "CbmDaqMap.h"

#include "CbmStsAddress.h"

#include "FairRunOnline.h"
#include "FairLogger.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

CbmROCUnpackSts::CbmROCUnpackSts()
: CbmROCUnpack(),
  fBuffer(CbmTbDaqBuffer::Instance()),
  fDaqMap(NULL),
  fSource(NULL),
  fStsDigis(new TClonesArray("CbmStsDigi", 10)),
  fStsBaselineDigis(new TClonesArray("CbmStsDigi", 10))
{
}
  
CbmROCUnpackSts::~CbmROCUnpackSts()
{
}
  
  
Bool_t CbmROCUnpackSts::Init()
{
  FairRunOnline* online = FairRunOnline::Instance();
  fSource = static_cast<CbmSourceLmdNew*>(online->GetSource());

  fDaqMap = fSource->GetDaqMap();

  FairRootManager* ioman = FairRootManager::Instance();
  ioman->Register("StsDigi", "STS raw data", fStsDigis, fPersistence);
  ioman->Register("StsBaselineDigi", "STS baseline data",
                  fStsBaselineDigis, fPersistence);

  return kTRUE;
}
 
Bool_t CbmROCUnpackSts::DoUnpack(roc::Message* Message, ULong_t hitTime)
{
  // --- Get absolute time, NXYTER and channel number
  Int_t rocId        = Message->getRocNumber();
  //  ULong64_t hitTime  = fCurrentMessage->getMsgFullTime(fCurrentEpoch[rocId]);
  Int_t nxyterId     = Message->getNxNumber();
  Int_t nxChannel    = Message->getNxChNum();
  Int_t charge       = Message->getNxAdcValue();

  // --- Check for epoch marker for this ROC
  fSource->CheckCurrentEpoch(rocId);

  // --- Get detector element from DaqMap
  Int_t station = fDaqMap->GetStsStation(rocId);
  Int_t sector  = 0;
  Int_t side    = fDaqMap->GetStsSensorSide(rocId);
  Int_t channel = fDaqMap->GetStsChannel(rocId, nxyterId, nxChannel);


  Int_t iStation;
  Int_t iSector;
  Int_t iPlane;
  Int_t iFiber;

  fDaqMap->Map(rocId, nxyterId, nxChannel, iStation, iSector, iPlane, iFiber);

  if( station != iStation ||  sector != iSector ||
      side != iPlane || channel != iFiber ) {
    LOG(INFO) << "Roc, Nxyter: " << rocId << ", " << nxyterId << FairLogger::endl;
    LOG(INFO) << "Station: " << station << ", " << iStation << FairLogger::endl;
    LOG(INFO) << "Sector: " << sector << ", " << iSector << FairLogger::endl;
    LOG(INFO) << "Side: " << side << ", " << iPlane << FairLogger::endl;
    LOG(INFO) << "Channel: " << channel << ", " << iFiber << FairLogger::endl;
  } 
  // --- Create a STS digi and send it to the buffer
  UInt_t address = CbmStsAddress::GetAddress(station,
                   0,   // ladder
                   0,   // halfladder
                   0,   // module
                   0,   // sensor
                   side,
                   channel);
  CbmStsDigi* digi = new CbmStsDigi(address, hitTime, charge);


  // In case of normal data insert the digi into the buffer.
  // In case of baseline data insert the digi only if the roc 
  // is already in baseline mode.  
  if ( !fSource->IsBaselineFill() ) {
    fBuffer->InsertData(digi);
  } else {
    if ( fSource->IsBaselineFill(rocId) ) { 
      fBuffer->InsertData(digi); 
    } else {
      fSource->AddDiscardedDigi(kSTS);
    }
  }

  return kTRUE;
}

void CbmROCUnpackSts::FillOutput(CbmDigi* digi)
{
  if ( !fSource->IsBaselineRetrieve() ) {
    new( (*fStsDigis)[fStsDigis->GetEntriesFast()])
      CbmStsDigi(*(dynamic_cast<CbmStsDigi*>(digi)));
  } else {
      new( (*fStsBaselineDigis)[fStsBaselineDigis->GetEntriesFast()])
      CbmStsDigi(*(dynamic_cast<CbmStsDigi*>(digi)));
  }

}

void CbmROCUnpackSts::Reset()
{
  // The next block is needed do to the problem that the TClonesArray is
  // very large after usage (2.5 M entries). This somehow slows down
  // the Clear of the container even if no entries are inside by 2-3 orders
  // of magnitude which slows down the execution of the whole program by 
  // more then a factor of 10.
  // TODO:
  // It has to be checked if this is a bug in Root by producing a small
  // example program to demonstarete the issue
  if (fStsBaselineDigis->GetEntriesFast() > 1000) {
    fStsBaselineDigis->Delete();
    fStsBaselineDigis->Expand(10);
  } 
  fStsDigis->Clear();
  fStsBaselineDigis->Clear();
}

ClassImp(CbmROCUnpackSts)
