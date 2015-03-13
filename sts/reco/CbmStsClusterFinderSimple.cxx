/** @file CbmClusterFinderSimple.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 16.06.2014
 **/

// --- Include class header
#include <reco/CbmStsClusterFinderSimple.h>

// --- Includes from ROOT
#include "TClonesArray.h"

// --- Includes from FAIRROOT
#include "FairLogger.h"

// --- Includes from CBMROOT
#include "CbmStsAddress.h"
#include "CbmStsDigi.h"
#include "CbmStsCluster.h"

 // --- Includes from STS
#include "CbmStsModule.h"


// -----   Constructor   ---------------------------------------------------
CbmStsClusterFinderSimple::CbmStsClusterFinderSimple() : TObject(),
                                                         fClusters(NULL) {
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsClusterFinderSimple::~CbmStsClusterFinderSimple() {
}
// -------------------------------------------------------------------------


// -----   Cluster finding   -----------------------------------------------
Int_t CbmStsClusterFinderSimple::FindClusters(CbmStsModule* module) {

	// --- Counter
	Int_t nClusters = 0;
	Int_t index = -1;  // dummy

	// --- Loop over module channels
	Int_t clusterStart = -1;
	for (Int_t channel = 0; channel < module->GetNofChannels();
			 channel++) {
		if ( module->GetDigi(channel, index) ) {   // channel is active
			if ( clusterStart == -1 ) clusterStart = channel; // start new cluster
		}
		else {   // channel is inactive
			if (clusterStart != -1)  {   // stop current cluster
				module->CreateCluster(clusterStart, channel-1, fClusters);
				nClusters++;
				clusterStart = -1;  // delete current cluster
			}
		}
	}   // Loop over module channels

	return nClusters;
}
// -------------------------------------------------------------------------





ClassImp(CbmStsClusterFinderSimple)

