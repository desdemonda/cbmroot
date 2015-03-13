// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          CbmROCUnpackHodo                         -----
// -----                    Created 14.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
#include "CbmROCUnpackHodo.h"

#include "CbmFiberHodoDigi.h"
#include "CbmFiberHodoAddress.h"

#include "CbmTbDaqBuffer.h"
#include "CbmDaqMap.h"
#include "CbmSourceLmdNew.h"
#include "CbmDetectorList.h"

#include "FairRunOnline.h"
#include "FairLogger.h"
#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TDirectory.h"


CbmROCUnpackHodo::CbmROCUnpackHodo()
  : CbmROCUnpack(),
    fBuffer(CbmTbDaqBuffer::Instance()),
    fDaqMap(NULL),
    fSource(NULL),
    fHodoDigis(new TClonesArray("CbmFiberHodoDigi", 10)),
    fHodoBaselineDigis(new TClonesArray("CbmFiberHodoDigi", 10)),
    fHM(new CbmHistManager())
{
}
  
CbmROCUnpackHodo::~CbmROCUnpackHodo()
{
}
  
  
Bool_t CbmROCUnpackHodo::Init()
{
  FairRunOnline* online = FairRunOnline::Instance();
  fSource = static_cast<CbmSourceLmdNew*>(online->GetSource());

  fDaqMap = fSource->GetDaqMap();

  FairRootManager* ioman = FairRootManager::Instance();
  ioman->Register("HodoDigi", "HODO raw data", fHodoDigis, fPersistence);
  ioman->Register("HodoBaselineDigi", "HODO baseline data", 
		  fHodoBaselineDigis, fPersistence);

  CreateHistograms();

  return kTRUE;

}

void CbmROCUnpackHodo::CreateHistograms()
{
  fHM->Add("Raw_ADC_FrontHodo", 
	   new TH2F("Raw_ADC_FrontHodo", 
		    "Raw_ADC_FrontHodo;channel;ADC value", 128, 0, 127, 4096, 0, 4095));   
  fHM->Add("Raw_ADC_RearHodo", 
	   new TH2F("Raw_ADC_RearHodo", 
		    "Raw_ADC_RearHodo;channel;ADC value", 128, 0, 127, 4096, 0, 4095));   
}
 
Bool_t CbmROCUnpackHodo::DoUnpack(roc::Message* Message, ULong_t hitTime)
{

  // --- Get absolute time, NXYTER and channel number
  Int_t rocId      = Message->getRocNumber();
  Int_t nxyterId   = Message->getNxNumber();
  Int_t nxChannel  = Message->getNxChNum();
 
  Int_t charge     = Message->getNxAdcValue();
 
  Int_t station = fDaqMap->GetFiberHodoStation(rocId);
  
  if ( 0 == station ) {
    fHM->H2("Raw_ADC_FrontHodo")->Fill(nxChannel,charge);
  } else {
    fHM->H2("Raw_ADC_RearHodo")->Fill(nxChannel,charge);
  } 

  // --- Check for epoch marker for this ROC
  fSource->CheckCurrentEpoch(rocId);

  Int_t iStation;
  Int_t iSector;
  Int_t iPlane;
  Int_t iFiber;

  fDaqMap->Map(rocId, nxyterId, nxChannel, iStation, iSector, iPlane, iFiber);
  Int_t address = CbmFiberHodoAddress::GetAddress(iStation, iPlane, iFiber);
  
  // --- Create a HODO digi and send it to the buffer
  CbmFiberHodoDigi* digi = new CbmFiberHodoDigi(address, charge, hitTime);
  

  // In case of normal data insert the digi into the buffer.
  // In case of baseline data insert the digi only if the roc 
  // is already in baseline mode.  
  if ( !fSource->IsBaselineFill() ) {
    fBuffer->InsertData(digi);
  } else {
    if ( fSource->IsBaselineFill(rocId) ) { 
      fBuffer->InsertData(digi); 
    } else {
      fSource->AddDiscardedDigi(kFHODO);
    }
  }

  return kTRUE;
}

void CbmROCUnpackHodo::FillOutput(CbmDigi* digi)
{
  if ( !fSource->IsBaselineRetrieve() ) {
    new( (*fHodoDigis)[fHodoDigis->GetEntriesFast()])
      CbmFiberHodoDigi(*(dynamic_cast<CbmFiberHodoDigi*>(digi)));
  } else {
    new( (*fHodoBaselineDigis)[fHodoBaselineDigis->GetEntriesFast()])
      CbmFiberHodoDigi(*(dynamic_cast<CbmFiberHodoDigi*>(digi)));
  }
  
}

void CbmROCUnpackHodo::Reset()
{
  // The next block is needed do to the problem that the TClonesArray is
  // very large after usage (2.5 M entries). This somehow slows down
  // the Clear of the container even if no entries are inside by 2-3 orders
  // of magnitude which slows down the execution of the whole program by 
  // more then a factor of 10.
  // TODO:
  // It has to be checked if this is a bug in Root by producing a small
  // example program to demonstarete the issue
  if (fHodoBaselineDigis->GetEntriesFast() > 1000) {
    fHodoBaselineDigis->Delete();
    fHodoBaselineDigis->Expand(10);
  } 

  // --- Clear output arrays
  fHodoDigis->Clear();
  fHodoBaselineDigis->Clear();
}

void CbmROCUnpackHodo::Finish()
{
  gDirectory->mkdir("Hodo_Raw");
  gDirectory->cd("Hodo_Raw");
  fHM->H2("Raw_ADC_FrontHodo")->Write();	
  fHM->H2("Raw_ADC_RearHodo")->Write();	
  gDirectory->cd("..");
}

ClassImp(CbmROCUnpackHodo)
