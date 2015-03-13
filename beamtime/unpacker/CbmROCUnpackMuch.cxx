// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           CbmROCUnpackMuch                         -----
// -----                    Created 13.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
#include "CbmROCUnpackMuch.h"

#include "CbmMuchBeamTimeDigi.h"
#include "CbmTbDaqBuffer.h"
#include "CbmSourceLmdNew.h"
#include "CbmDaqMap.h"

#include "CbmMuchAddress.h"

#include "FairRunOnline.h"
#include "FairLogger.h"

#include "TClonesArray.h"

CbmROCUnpackMuch::CbmROCUnpackMuch()
  : CbmROCUnpack(),
    fBuffer(CbmTbDaqBuffer::Instance()),
    fDaqMap(NULL),
    fSource(NULL),
    fMuchDigis(new TClonesArray("CbmMuchBeamTimeDigi", 10)),
    fMuchBaselineDigis(new TClonesArray("CbmMuchBeamTimeDigi", 10))
{
}
  
CbmROCUnpackMuch::~CbmROCUnpackMuch()
{
}
  
  
Bool_t CbmROCUnpackMuch::Init()
{
  FairRunOnline* online = FairRunOnline::Instance();
  fSource = static_cast<CbmSourceLmdNew*>(online->GetSource());

  fDaqMap = fSource->GetDaqMap();

  FairRootManager* ioman = FairRootManager::Instance();
  ioman->Register("MuchDigi", "MUCH raw data", fMuchDigis, fPersistence);
  ioman->Register("MuchBaselineDigi", "MUCH baseline data", 
		  fMuchBaselineDigis, fPersistence);

  return kTRUE;
}
 
Bool_t CbmROCUnpackMuch::DoUnpack(roc::Message* Message, ULong_t hitTime)
{
  
  // --- Get absolute time, NXYTER and channel number
  Int_t rocId      = Message->getRocNumber();
  Int_t nxyterId   = Message->getNxNumber();
  Int_t nxChannel  = Message->getNxChNum();
  Int_t charge     = Message->getNxAdcValue();
  
  // --- Get detector element from DaqMap
  Int_t station = 0;
  Int_t layer   = 0;
  Int_t sector  = 0;
  Int_t channel = 0;

  fDaqMap->Map(rocId, nxyterId, nxChannel,
  		         station, layer, sector, channel);

  // --- Construct unique address
  UInt_t address = CbmMuchAddress::GetAddress(station, layer, 0,
					      0, sector, channel);
  
  // --- Create digi
  CbmMuchBeamTimeDigi* digi = new CbmMuchBeamTimeDigi(address, charge, hitTime);
  digi->SetPadX(sector);
  digi->SetPadY(channel);
  digi->SetRocId(rocId);
  digi->SetNxId(nxyterId);
  digi->SetNxCh(nxChannel);


  LOG(DEBUG) << "MUCH message: rocId " << rocId << " NXYTER " << nxyterId
  		<< " channel " << nxChannel << " charge " << charge << " time "
  		<< hitTime << FairLogger::endl;
  LOG(DEBUG) << "MUCH digi: station " << CbmMuchAddress::GetElementId(address, kMuchStation)
       << " layer " << CbmMuchAddress::GetElementId(address, kMuchLayer)
  << " layerside " << CbmMuchAddress::GetElementId(address, kMuchLayerSide)
  << " module " << CbmMuchAddress::GetElementId(address, kMuchModule)
  << " sector " << CbmMuchAddress::GetElementId(address, kMuchSector)
  << " channel " << CbmMuchAddress::GetElementId(address, kMuchChannel)
  << FairLogger::endl;
  LOG(DEBUG) << "MUCH digi: pad x " << digi->GetPadX() << " pad y" << digi->GetPadY()
  		<< " ROC " << digi->GetRocId() << " NXYTER " << digi->GetNxId()
  		<< FairLogger::endl;
  LOG(DEBUG) << FairLogger::endl;



  // In case of normal data insert the digi into the buffer.
  // In case of baseline data insert the digi only if the roc 
  // is already in baseline mode.  
  if ( !fSource->IsBaselineFill() ) {
    fBuffer->InsertData(digi);
  } else {
    if ( fSource->IsBaselineFill(rocId) ) { 
      fBuffer->InsertData(digi); 
    } else {
      fSource->AddDiscardedDigi(kMUCH);
    }
  }

  return kTRUE;
}

void CbmROCUnpackMuch::FillOutput(CbmDigi* digi)
{
  if ( !fSource->IsBaselineRetrieve() ) {
    new( (*fMuchDigis)[fMuchDigis->GetEntriesFast()])
      CbmMuchBeamTimeDigi(*(dynamic_cast<CbmMuchBeamTimeDigi*>(digi)));
  } else {
    new( (*fMuchBaselineDigis)[fMuchBaselineDigis->GetEntriesFast()])
      CbmMuchBeamTimeDigi(*(dynamic_cast<CbmMuchBeamTimeDigi*>(digi)));
  }

}

void CbmROCUnpackMuch::Reset()
{
  // The next block is needed do to the problem that the TClonesArray is
  // very large after usage (2.5 M entries). This somehow slows down
  // the Clear of the container even if no entries are inside by 2-3 orders
  // of magnitude which slows down the execution of the whole program by 
  // more then a factor of 10.
  // TODO:
  // It has to be checked if this is a bug in Root by producing a small
  // example program to demonstarete the issue
  if (fMuchBaselineDigis->GetEntriesFast() > 1000) {
    fMuchBaselineDigis->Delete();
    fMuchBaselineDigis->Expand(10);
  } 
  fMuchDigis->Clear();
  fMuchBaselineDigis->Clear();

}

ClassImp(CbmROCUnpackMuch)
