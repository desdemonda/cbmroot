/** @file CbmStsFindClusters.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 16.06.2014
 **/

// --- Include class header
#include "reco/CbmStsFindClusters.h"

// --- Includes from C++
#include <iostream>

// --- Includes from ROOT
#include "TClonesArray.h"

// --- Includes from FAIRROOT
#include "FairEventHeader.h"
#include "FairRunAna.h"

// --- Includes from STS
#include "reco/CbmStsClusterFinderSimple.h"
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSetup.h"

using namespace std;


// -----   Constructor   ---------------------------------------------------
CbmStsFindClusters::CbmStsFindClusters()
    : FairTask("StsFindClusters", 1)
    , fDigis(NULL)
    , fClusters(NULL)
    , fSetup(NULL)
    , fFinder(NULL)
    , fTimer()
    , fNofEvents(0.)
    , fNofDigisTot(0.)
    , fNofClustersTot(0.)
    , fTimeTot(0.)
    , fActiveModules()
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmStsFindClusters::~CbmStsFindClusters() {
	if ( fFinder ) delete fFinder;
}
// -------------------------------------------------------------------------



// -----   Task execution   ------------------------------------------------
void CbmStsFindClusters::Exec(Option_t* opt) {

	// --- Event number
	Int_t iEvent =
			FairRunAna::Instance()->GetEventHeader()->GetMCEntryNumber();

	// Start timer and counter
	fTimer.Start();
	Int_t nOfClusters = 0;

	// --- Clear output array
	fClusters->Delete();

	// --- Sort digis into modules
	Int_t nOfDigis = SortDigis();

	// --- Find clusters in modules
	set<CbmStsModule*>::iterator it;
	for (it = fActiveModules.begin(); it != fActiveModules.end(); it++) {
		Int_t nClusters = fFinder->FindClusters(*it);
		LOG(DEBUG1) << GetName() << ": Module " << (*it)->GetName()
    			      << ", digis: " << (*it)->GetNofDigis()
   		          << ", clusters " << nClusters << FairLogger::endl;
		nOfClusters += nClusters;
	}

  // --- Counters
  fTimer.Stop();
  fNofEvents++;
  fNofDigisTot    += nOfDigis;
  fNofClustersTot += nOfClusters;
  fTimeTot        += fTimer.RealTime();

  LOG(INFO) << "+ " << setw(20) << GetName() << ": Event " << setw(6)
  		      << right << iEvent << ", time " << fixed << setprecision(6)
  		      << fTimer.RealTime() << " s, digis: " << nOfDigis
  		      << ", clusters: " << nOfClusters << FairLogger::endl;
}
// -------------------------------------------------------------------------



// -----   End-of-run action   ---------------------------------------------
void CbmStsFindClusters::Finish() {
	std::cout << std::endl;
	LOG(INFO) << "=====================================" << FairLogger::endl;
	LOG(INFO) << GetName() << ": Run summary" << FairLogger::endl;
	LOG(INFO) << "Events processed  : " << fNofEvents << FairLogger::endl;
	LOG(INFO) << "Digis / event     : " << fNofDigisTot / Double_t(fNofEvents)
			      << FairLogger::endl;
	LOG(INFO) << "Clusters / event  : "
			      << fNofClustersTot / Double_t(fNofEvents)
			      << FairLogger::endl;
	LOG(INFO) << "Digis per cluster : " << fNofDigisTot / fNofClustersTot
			      << FairLogger::endl;
	LOG(INFO) << "Time per event    : " << fTimeTot / Double_t(fNofEvents)
			      << " s " << FairLogger::endl;
	LOG(INFO) << "=====================================" << FairLogger::endl;

}
// -------------------------------------------------------------------------



// -----   End-of-event action   -------------------------------------------
void CbmStsFindClusters::FinishEvent() {

	// --- Clear digi maps for all active modules
	Int_t nModules = 0;
	set<CbmStsModule*>::iterator it;
	for (it = fActiveModules.begin(); it != fActiveModules.end(); it++) {
		(*it)->ClearDigis();
		nModules++;
	}
	fActiveModules.clear();

	LOG(DEBUG) << GetName() << ": Cleared digis in " << nModules
			       << " modules. " << FairLogger::endl;
}
// -------------------------------------------------------------------------



// -----   Initialisation   ------------------------------------------------
InitStatus CbmStsFindClusters::Init()
{
    // --- Check IO-Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
    	LOG(ERROR) << GetName() << ": No FairRootManager!"
    			       << FairLogger::endl;
    	return kFATAL;
    }

    // --- Get input array (StsDigis)
    fDigis = (TClonesArray*)ioman->GetObject("StsDigi");
    if (NULL == fDigis) {
    	LOG(ERROR) << GetName() << ": No StsDigi array!" << FairLogger::endl;
    	return kERROR;
    }

    // --- Register output array
    fClusters = new TClonesArray("CbmStsCluster", 10000);
    ioman->Register("StsCluster", "Cluster in STS", fClusters, kTRUE);

    // --- Get STS setup
    fSetup = CbmStsSetup::Instance();

    // --- Create cluster finder
    fFinder = new CbmStsClusterFinderSimple();
    fFinder->SetOutputArray(fClusters);

    LOG(INFO) << GetName() << ": Initialisation successful"
    		      << FairLogger::endl;
    return kSUCCESS;
}
// -------------------------------------------------------------------------




// ----- Sort digis into module digi maps   --------------------------------
Int_t CbmStsFindClusters::SortDigis() {

	// --- Counters
	Int_t nDigis   = 0;

	// --- Loop over digis in input array
	for (Int_t iDigi = 0; iDigi < fDigis->GetEntriesFast(); iDigi++) {
		CbmStsDigi* digi = static_cast<CbmStsDigi*> (fDigis->At(iDigi));
		if ( ! digi ) {
			LOG(FATAL) << GetName() << ": Invalid digi pointer!"
					       << FairLogger::endl;
			continue;
		}

		// --- Get the module
		UInt_t address = digi->GetAddress();
		CbmStsModule* module =
				static_cast<CbmStsModule*>(fSetup->GetElement(address, kStsModule));
		if ( ! module ) {
			LOG(FATAL) << GetName() << ": Module " << address
					       << " not present in STS setup!" << FairLogger::endl;
			continue;
		}

		// --- Add module to list of active modules, if not yet present.
		fActiveModules.insert(module);

		// --- Add the digi to the module
		module->AddDigi(digi, iDigi);
		nDigis++;

	}  // Loop over digi array

	// --- Debug output
	LOG(DEBUG) << GetName() << ": sorted " << nDigis << " digis into "
			       << fActiveModules.size() << " module(s)." << FairLogger::endl;
	if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG3) ) {
		set<CbmStsModule*>::iterator it;
		for (it = fActiveModules.begin(); it != fActiveModules.end() ; it++) {
				LOG(DEBUG3) << GetName() << ": Module " << (*it)->GetName()
						        << ", digis " << (*it)->GetNofDigis()
						        << FairLogger::endl;
		}
	}

	return nDigis;
}
// -------------------------------------------------------------------------

ClassImp(CbmStsFindClusters)
