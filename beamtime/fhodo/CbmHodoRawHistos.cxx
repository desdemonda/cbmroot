#include "CbmHodoRawHistos.h"

#include "CbmFiberHodoDigi.h"
#include "CbmFiberHodoAddress.h"

#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH2F.h"
#include "TString.h"

#include <set>

using std::set;

// ---- Default constructor -------------------------------------------
CbmHodoRawHistos::CbmHodoRawHistos()
  : FairTask("CbmHodoRawHistos"),
    fHodoDigi(NULL),
    fHM(new CbmHistManager())
{
}

// ---- Destructor ----------------------------------------------------
CbmHodoRawHistos::~CbmHodoRawHistos()
{
}

// ----  Initialisation  ----------------------------------------------
void CbmHodoRawHistos::SetParContainers()
{
}

// ---- Init ----------------------------------------------------------
InitStatus CbmHodoRawHistos::Init()
{
  LOG(DEBUG) << "Initilization of CbmHodoRawHistos" << FairLogger::endl;

  // Get a handle from the IO manager
  FairRootManager* ioman = FairRootManager::Instance();

  // Get a pointer to the previous already existing data level
  fHodoDigi = (TClonesArray*) ioman->GetObject("HodoDigi");
  if ( ! fHodoDigi ) {
    LOG(ERROR) << "No branch HodoDigi found!" << FairLogger::endl;
    LOG(ERROR) << "Task CbmHodoRawHistos will be inactive" << FairLogger::endl;
    return kERROR;
  }

  // Create the TClonesArray for the output data and register
  // it in the IO manager
  /*
    <OutputDataLevel> = new TClonesArray("OutputDataLevelName", 100);
    ioman->Register("OutputDataLevelName","OutputDataLevelName",<OutputDataLevel>,kTRUE);
  */

  // Do whatever else is needed at the initilization stage
  // Create histograms to be filled
  // initialize variables

  CreateHistograms();

  return kSUCCESS;

}

// ---- ReInit  -------------------------------------------------------
void CbmHodoRawHistos::CreateHistograms()
{
  TString hodo[2] = { "FrontHodo", "RearHodo"}; 
  fHM->Add("ADC_FrontHodo_X", 
	   new TH2F("ADC_FrontHodo_X", 
		    "ADC_FrontHodo_X;channel;ADC value", 64, 0, 63, 4096, 0, 4095));   
  fHM->Add("ADC_RearHodo_X", 
	   new TH2F("ADC_RearHodo_X", 
		    "ADC_RearHodo_X;channel;ADC value", 64, 0, 63, 4096, 0, 4095));   
  fHM->Add("ADC_FrontHodo_Y", 
	   new TH2F("ADC_FrontHodo_Y", 
		    "ADC_FrontHodo_Y;channel;ADC value", 64, 0, 63, 4096, 0, 4095));   
  fHM->Add("ADC_RearHodo_Y", 
	   new TH2F("ADC_RearHodo_Y", 
		    "ADC_RearHodo_Y;channel;ADC value", 64, 0, 63, 4096, 0, 4095));   
}


// ---- ReInit  -------------------------------------------------------
InitStatus CbmHodoRawHistos::ReInit()
{
  return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------
void CbmHodoRawHistos::Exec(Option_t* option)
{

  CbmFiberHodoDigi* digi = NULL;
  Int_t address = -1;
  Int_t station = -1;
  Int_t side = -1;
  Int_t strip = -1;
  Double_t adc = -1.;
  UInt_t nofHodoDigis = fHodoDigi->GetEntriesFast();
  for (UInt_t iDigi=0; iDigi < nofHodoDigis; ++iDigi) {
    digi = static_cast<CbmFiberHodoDigi*>(fHodoDigi->At(iDigi));
    adc = digi->GetCharge();
    address = digi->GetAddress();
    station = CbmFiberHodoAddress::GetStationId(address);
    strip = CbmFiberHodoAddress::GetStripId(address);
    side = CbmFiberHodoAddress::GetSideId(address);
    
    if ( 0 == station ) {
      if ( 0 == side) {
	fHM->H2("ADC_FrontHodo_X")->Fill(strip,adc);
      } else {
	fHM->H2("ADC_FrontHodo_Y")->Fill(strip,adc);
      }
    } else {
      if ( 0 == side) {
	fHM->H2("ADC_RearHodo_X")->Fill(strip,adc);	
      } else {
	fHM->H2("ADC_RearHodo_Y")->Fill(strip,adc);
      }
    }    
  }
  
}

// ---- Finish --------------------------------------------------------
void CbmHodoRawHistos::Finish()
{
  fHM->H2("ADC_FrontHodo_X")->Write();	
  fHM->H2("ADC_RearHodo_X")->Write();	
  fHM->H2("ADC_FrontHodo_Y")->Write();	
  fHM->H2("ADC_RearHodo_Y")->Write();	
}

ClassImp(CbmHodoRawHistos)
