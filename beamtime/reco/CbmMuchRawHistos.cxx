#include "CbmMuchRawHistos.h"

#include "CbmMuchBeamTimeDigi.h"
#include "CbmMuchAddress.h"
#include "CbmHistManager.h"

#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH2F.h"
#include "TString.h"

#include <set>

using std::set;

// ---- Default constructor -------------------------------------------
CbmMuchRawHistos::CbmMuchRawHistos()
  : FairTask("CbmMuchRawHistos"),
    fDigis(NULL),
    fHM(new CbmHistManager()),
    fNofEvents(0)
{
}

// ---- Destructor ----------------------------------------------------
CbmMuchRawHistos::~CbmMuchRawHistos()
{
}

// ----  Initialisation  ----------------------------------------------
void CbmMuchRawHistos::SetParContainers()
{
}

// ---- Init ----------------------------------------------------------
InitStatus CbmMuchRawHistos::Init()
{
	LOG(INFO) << GetName() << ": Initialising..." << FairLogger::endl;

  // Get a handle from the IO manager
  FairRootManager* ioman = FairRootManager::Instance();

  // Get the TClonesArray
  fDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
  if ( ! fDigis ) {
    LOG(ERROR) << "No branch MuchDigi found!" << FairLogger::endl;
    LOG(ERROR) << "Task CbmMuchRawHistos will be inactive" << FairLogger::endl;
    return kERROR;
  }

  CreateHistograms();

  return kSUCCESS;

}

// -----   Create histograms   ---------------------------------------------
void CbmMuchRawHistos::CreateHistograms()
{

	TString name;
	TString title;

	name = "hits_much";
	title = name + ";padX;padY";
	fHM->Add(name.Data(), new TH2F(name, title, 23, -0.5, 22.5, 78, -0.5, 77.5));

	name = "MUCH ADC vs channel";
	title = name + ";channel;ADC";
	fHM->Add(name.Data(), new TH2F(name, title, 128, -0.5, 127.5, 4096, -0.5, 4095.5));
}
// -------------------------------------------------------------------------


// ---- ReInit  -------------------------------------------------------
InitStatus CbmMuchRawHistos::ReInit()
{
  return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------
void CbmMuchRawHistos::Exec(Option_t* option)
{


	CbmMuchBeamTimeDigi* digi = NULL;
	Int_t adc = -1;

	Int_t nDigis = fDigis->GetEntriesFast();
	for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
		digi = static_cast<CbmMuchBeamTimeDigi*> (fDigis->At(iDigi));
		if ( ! digi ) continue;
		fHM->H2("hits_much")->Fill(Double_t(digi->GetPadX()),
				                       Double_t(digi->GetPadY()));

	}

	fNofEvents++;
	if ( ! (fNofEvents%1000) )
		LOG(INFO) << GetName() << ": event " << fNofEvents << ", digis "
		          << nDigis << FairLogger::endl;
	else
		LOG(DEBUG) << GetName() << ": event " << fNofEvents << ", digis "
	             << nDigis << FairLogger::endl;
  
}

// ---- Finish --------------------------------------------------------
void CbmMuchRawHistos::Finish()
{

  fHM->H2("hits_much")->Write();
}

ClassImp(CbmMuchRawHistos)
