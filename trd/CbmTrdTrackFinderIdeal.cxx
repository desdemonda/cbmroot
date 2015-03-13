// -------------------------------------------------------------------------
// -----                CbmTrdTrackFinderIdeal source file             -----
// -----                  Created 28/11/05  by V. Friese               -----
// -----              according to the CbmStsTrackFinderIdeal          -----
// -------------------------------------------------------------------------
// CBM includes
#include "CbmTrdTrackFinderIdeal.h"

#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairDetector.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmMatch.h"

// ROOT includes
#include "TClonesArray.h"
#include "TObjArray.h"

// C++ includes
#include <iostream>
#include <map>
using std::cout;
using std::endl;
using std::map;


CbmTrdTrackFinderIdeal::CbmTrdTrackFinderIdeal()
  : CbmTrdTrackFinder(),
    fMcTracks(NULL),
    fTrdPoints(NULL),
    fTrdHitMatches(NULL),
    fTrdHitProducerType(""),
    fEventNum(-1)
{
}


CbmTrdTrackFinderIdeal::~CbmTrdTrackFinderIdeal()
{
}


void CbmTrdTrackFinderIdeal::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) Fatal("-E- CbmTrdTrackFinderIdeal::Init","RootManager not instantised!");

	fMcTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( NULL == fMcTracks) Fatal("-E- CbmTrdTrackFinderIdeal::Init", "No MCTrack array!");

	fTrdPoints  = (TClonesArray*) ioman->GetObject("TrdPoint");
	if ( NULL == fTrdPoints) Fatal("-E- CbmTrdTrackFinderIdeal::Init", "No TrdPoint array!");

	TClonesArray* digis = (TClonesArray*) ioman->GetObject("TrdDigi");
	if (NULL == digis) {
		fTrdHitProducerType = "smearing";
	} else {
		fTrdHitProducerType = "digi";
		fTrdHitMatches = (TClonesArray*) ioman->GetObject("TrdHitMatch");
		if ( NULL == fTrdHitMatches) Fatal("-E- CbmTrdTrackFinderIdeal::Init", "No TrdHitMatch array!");
	}

	fEventNum = 1;
}

Int_t CbmTrdTrackFinderIdeal::DoFind(TClonesArray* trdHits,
                                     TClonesArray* trdTracks)
{
	cout << "CbmTrdTrackFinderIdeal, event no. " << fEventNum << " " << fTrdHitProducerType << endl;
	fEventNum++;
	if (NULL == trdHits) Fatal("-E- CbmTrdTrackFinderIdeal::DoFind", "No TrdHitArray!");
	if (NULL == trdTracks) Fatal("-E- CbmTrdTrackFinderIdeal::DoFind", "No TrdTrackArray!");

	Int_t nofTrdHits = trdHits->GetEntriesFast();

	// STL map from MCTrack index to CbmTrdTrack
	map<Int_t, CbmTrdTrack*> mcTrackToTrdTrack;
	for(Int_t iHit = 0; iHit < nofTrdHits; iHit++) {
		CbmTrdHit* trdHit = static_cast<CbmTrdHit*>(trdHits->At(iHit));
		if(NULL == trdHit) continue;

		int trdPointInd = -1;
		if (fTrdHitProducerType == "smearing") {
			trdPointInd = trdHit->GetRefId();
		} else if (fTrdHitProducerType == "digi") {
			const CbmMatch* trdHitMatch = static_cast<const CbmMatch*>(fTrdHitMatches->At(iHit));
			trdPointInd = trdHitMatch->GetMatchedLink().GetIndex();
		}

		if(trdPointInd < 0) continue; // fake or background hit
		FairMCPoint* trdPoint = static_cast<FairMCPoint*>(fTrdPoints->At(trdPointInd));
		if(NULL == trdPoint) continue;

		int mcTrackInd = trdPoint->GetTrackID();
		if (NULL == mcTrackToTrdTrack[mcTrackInd]) mcTrackToTrdTrack[mcTrackInd] = new CbmTrdTrack();
		mcTrackToTrdTrack[mcTrackInd]->AddHit(iHit, kTRDHIT);
	}

	int trackCount = 0;
	for (map<int,CbmTrdTrack*>::iterator it=mcTrackToTrdTrack.begin(); it!=mcTrackToTrdTrack.end(); ++it){
	    new ((*trdTracks)[trackCount]) CbmTrdTrack(*it->second);
	    trackCount++;
	}

	return mcTrackToTrdTrack.size();
}

ClassImp(CbmTrdTrackFinderIdeal)
