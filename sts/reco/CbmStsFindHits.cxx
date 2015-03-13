/** @file CbmStsFindHits.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 16.06.2014
 **/

// --- Include class header
#include "reco/CbmStsFindHits.h"

// --- Includes from C++
#include <iostream>

// --- Includes from ROOT
#include "TClonesArray.h"

// --- Includes from FAIRROOT
#include "FairEventHeader.h"
#include "FairRunAna.h"

// --- Includes from STS
#include "setup/CbmStsSetup.h"

using namespace std;


// -----   Constructor   ---------------------------------------------------
CbmStsFindHits::CbmStsFindHits()
    : FairTask("StsFindHits", 1)
    , fClusters(NULL)
    , fHits(NULL)
    , fSetup(NULL)
    , fTimer()
    , fNofEvents(0.)
    , fNofClustersTot(0.)
    , fNofHitsTot(0.)
    , fTimeTot(0.)
    , fActiveModules()
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmStsFindHits::~CbmStsFindHits() {
}
// -------------------------------------------------------------------------



// -----   Task execution   ------------------------------------------------
void CbmStsFindHits::Exec(Option_t* opt) {

	// --- Event number
	Int_t iEvent =
			FairRunAna::Instance()->GetEventHeader()->GetMCEntryNumber();

	// Start timer and counter
	fTimer.Start();

  // --- Clear output arrays
	fHits->Delete();

	// --- Sort clusters into modules
	Int_t nClusters = SortClusters();

	// --- Find hits in modules
	Int_t nHits = 0;
	set<CbmStsModule*>::iterator it;
	for (it = fActiveModules.begin(); it != fActiveModules.end(); it++) {
		CbmStsModule* module = (*it);
		if ( ! module ) {
			LOG(FATAL) << GetName() << ": Non-valid module pointer in vector!"
					       << FairLogger::endl;
			continue;
		}
		Int_t nModuleHits = module->FindHits(fHits);
		LOG(DEBUG1) << GetName() << ": Module " << module->GetName()
    			      << ", clusters: " << module->GetNofClusters()
   		          << ", hits: " << nModuleHits << FairLogger::endl;
		nHits += nModuleHits;
	}

  // --- Counters
  fTimer.Stop();
  fNofEvents++;
  fNofClustersTot += nClusters;
  fNofHitsTot     += nHits;
  fTimeTot        += fTimer.RealTime();

  LOG(INFO) << "+ " << setw(20) << GetName() << ": Event " << setw(6)
  		      << right << iEvent << ", time " << fixed << setprecision(6)
  		      << fTimer.RealTime() << " s, clusters: " << nClusters
  		      << ", hits: " << nHits << FairLogger::endl;
}
// -------------------------------------------------------------------------



// -----   End-of-run action   ---------------------------------------------
void CbmStsFindHits::Finish() {
	std::cout << std::endl;
	LOG(INFO) << "=====================================" << FairLogger::endl;
	LOG(INFO) << GetName() << ": Run summary" << FairLogger::endl;
	LOG(INFO) << "Events processed  : " << fNofEvents << FairLogger::endl;
	LOG(INFO) << "Clusters / event  : "
			      << fNofClustersTot / Double_t(fNofEvents) << FairLogger::endl;
	LOG(INFO) << "Hits / event      : "
			      << fNofHitsTot / Double_t(fNofEvents)
			      << FairLogger::endl;
	LOG(INFO) << "Hits per cluster  : " << fNofHitsTot / fNofClustersTot
			      << FairLogger::endl;
	LOG(INFO) << "Time per event    : " << fTimeTot / Double_t(fNofEvents)
			      << " " << FairLogger::endl;
	LOG(INFO) << "=====================================" << FairLogger::endl;

}
// -------------------------------------------------------------------------


// -----   End-of-event action   -------------------------------------------
void CbmStsFindHits::FinishEvent() {

	// --- Clear cluster sets for all active modules
	Int_t nModules = 0;
	set<CbmStsModule*>::iterator it;
	for (it = fActiveModules.begin(); it != fActiveModules.end(); it++) {
		(*it)->ClearClusters();
		nModules++;
	}
	fActiveModules.clear();

	LOG(DEBUG) << GetName() << ": Cleared clusters in " << nModules
			       << " modules. " << FairLogger::endl;
}
// -------------------------------------------------------------------------


// -----   Initialisation   ------------------------------------------------
InitStatus CbmStsFindHits::Init()
{
    // --- Check IO-Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
    	LOG(ERROR) << GetName() << ": No FairRootManager!"
    			       << FairLogger::endl;
    	return kFATAL;
    }

    // --- Get input array (StsDigis)
    fClusters = (TClonesArray*)ioman->GetObject("StsCluster");
    if (NULL == fClusters) {
    	LOG(ERROR) << GetName() << ": No StsCluster array!" << FairLogger::endl;
    	return kERROR;
    }

    // --- Register output array
    fHits = new TClonesArray("CbmStsHit", 10000);
    ioman->Register("StsHit", "Hits in STS", fHits, kTRUE);

    // --- Get STS setup
    fSetup = CbmStsSetup::Instance();

    LOG(INFO) << GetName() << ": Initialisation successful"
    		      << FairLogger::endl;
    return kSUCCESS;
}
// -------------------------------------------------------------------------




// ----- Sort digis into module digi maps   --------------------------------
Int_t CbmStsFindHits::SortClusters() {

	// --- Counters
	Int_t nClusters = 0;

	// --- Loop over clusters in input array
	for (Int_t iCluster = 0;
			 iCluster < fClusters->GetEntriesFast(); iCluster++) {
		CbmStsCluster* cluster = static_cast<CbmStsCluster*> (fClusters->At(iCluster));
		UInt_t address = cluster->GetAddress();
		cluster->SetIndex(iCluster);
		CbmStsModule* module =
				static_cast<CbmStsModule*>(fSetup->GetElement(address, kStsModule));

	  // --- Update set of active modules
		fActiveModules.insert(module);

		// --- Assign cluster to module
		module->AddCluster(cluster);
		nClusters++;

	}  // Loop over cluster array

	// --- Debug output
	LOG(DEBUG) << GetName() << ": sorted " << nClusters << " clusters into "
			       << fActiveModules.size() << " module(s)." << FairLogger::endl;
	if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG3) ) {
		set <CbmStsModule*>::iterator it;
		for (it = fActiveModules.begin(); it != fActiveModules.end() ; it++) {
			CbmStsModule* module = (*it);
			LOG(DEBUG3) << GetName() << ": Module " << module->GetName()
						      << ", clusters " << module->GetNofClusters()
						      << FairLogger::endl;
		}  // active module loop
	}  //? DEBUG 3

	return nClusters;
}
// -------------------------------------------------------------------------

ClassImp(CbmStsFindHits)
