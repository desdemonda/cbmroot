// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                             CbmROCUnpackAux                       -----
// -----                    Created 14.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
#include "CbmROCUnpackAux.h"

#include "CbmAuxDigi.h"
#include "CbmSourceLmdNew.h"
#include "CbmTbDaqBuffer.h"

#include "FairRunOnline.h"
#include "FairLogger.h"

#include "TClonesArray.h"

CbmROCUnpackAux::CbmROCUnpackAux()
  : CbmROCUnpack(),
    fSource(NULL),
    fBuffer(CbmTbDaqBuffer::Instance()),
    fAuxDigis(new TClonesArray("CbmAuxDigi", 10))
{
}
  
CbmROCUnpackAux::~CbmROCUnpackAux()
{
}


Bool_t CbmROCUnpackAux::Init()
{
  FairRunOnline* online = FairRunOnline::Instance();
  fSource = static_cast<CbmSourceLmdNew*>(online->GetSource());

  FairRootManager* ioman = FairRootManager::Instance();
  ioman->Register("AuxDigi", "AUX data", fAuxDigis, fPersistence);

  return kTRUE;

}

Bool_t CbmROCUnpackAux::DoUnpack(roc::Message* Message, ULong_t hitTime)
{
  if (!fSource->IsBaselineFill()) {
    // --- ROC Id and channel number
    Int_t rocId = Message->getRocNumber();
    Int_t channel = Message->getAuxChNum();

    fSource->CheckCurrentEpoch(rocId);
    ULong_t offset = fSource->GetAuxOffset();
    
    // --- Create AuxDigi and send it to the buffer
    CbmAuxDigi* digi = new CbmAuxDigi(rocId, channel, hitTime + offset);
    fBuffer->InsertData(digi);
  } else {
    fSource->AddDiscardedAux();
  }

  return kTRUE;
}

void CbmROCUnpackAux::FillOutput(CbmDigi* digi)
{
  new( (*fAuxDigis)[fAuxDigis->GetEntriesFast()])
    CbmAuxDigi(*(dynamic_cast<CbmAuxDigi*>(digi)));
}


void CbmROCUnpackAux::Reset()
{
  fAuxDigis->Clear();
}

ClassImp(CbmROCUnpackAux)
