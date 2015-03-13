// ------------------------------------------------------------------
// -----                   CbmTofMergerIdeal                    -----
// -----            Created 06/03/2006 by D.Kresan              -----
// ------------------------------------------------------------------
#include "CbmTofMergerIdeal.h"

#include "CbmGlobalTrack.h"

#include "FairRootManager.h"
#include "CbmTofPoint.h"
#include "CbmTofHit.h"
#include "CbmTrackMatch.h"

#include "TClonesArray.h"

#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::map;


//___________________________________________________________________
//
// CbmTofMergerIdeal
//
// Concrete implementation of global track - TOF merging algorithm,
// based on MC information
//


// ------------------------------------------------------------------
CbmTofMergerIdeal::CbmTofMergerIdeal()
  : CbmTofMerger(),
    fArrayTofPoint(NULL),
    fArrayStsTrackMatch(NULL),
    fArrayTrdTrackMatch(NULL)
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmTofMergerIdeal::~CbmTofMergerIdeal()
{
    // Destructor
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmTofMergerIdeal::Init()
{
    // Initialisation

    // Get pointer to the ROOT manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmTofMergerIdeal::Init : "
	    << "CBM ROOT manager is not instantiated!"
	    << endl;
        return;
    }
    // Activate TOF point array
    fArrayTofPoint = (TClonesArray*) rootMgr->GetObject("TofPoint");
    if(NULL == fArrayTofPoint) {
	cout << "-W- CbmTofMergerIdeal::Init : "
	    << "no TOF point array"
	    << endl;
    }
    // Activate STS track match array
    fArrayStsTrackMatch = (TClonesArray*) rootMgr->GetObject("StsTrackMatch");
    if(NULL == fArrayStsTrackMatch) {
	cout << "-W- CbmTofMergerIdeal::Init : "
	    << "no STS track match array"
	    << endl;
    }
    // Activate TRD track match array
    fArrayTrdTrackMatch = (TClonesArray*) rootMgr->GetObject("TrdTrackMatch");
    if(NULL == fArrayTrdTrackMatch) {
	cout << "-W- CbmTofMergerIdeal::Init : "
	    << "no TRD track match array"
	    << endl;
    }
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
Int_t CbmTofMergerIdeal::DoMerge(TClonesArray* glbTracks,
				 TClonesArray* tofHits)
{
    // Implementation of the merging algorithm
    if(NULL==glbTracks || NULL==tofHits ||
       NULL==fArrayTofPoint || NULL==fArrayStsTrackMatch ||
       NULL==fArrayTrdTrackMatch) return 0;

    // Declare variables outside the loop
    CbmTofPoint* tofPoint;
    CbmTofHit* tofHit;
    CbmTrackMatch* stsMatch;
    CbmTrackMatch* trdMatch;
    CbmGlobalTrack* glbTrack;
    Int_t tofPointIndex;
    Int_t mcTrackIndex;
    Int_t stsTrackIndex;
    Int_t trdTrackIndex;
    Int_t tofHitIndex;

    // Map from MC track index to the TOF hit index
    map<Int_t, Int_t> mapMCTof;

    // Counters
    Int_t nGlbTracks = glbTracks->GetEntriesFast();
    Int_t nTofHits = tofHits->GetEntriesFast();
    Int_t nMerged = 0;

    // Loop over TOF hits
    for(Int_t iTofHit = 0; iTofHit < nTofHits; iTofHit++) {
	// Get pointer to the TOF hit
	tofHit = (CbmTofHit*) tofHits->At(iTofHit);
	if(NULL == tofHit) continue;
	// Get pointer to the TOF point
	tofPointIndex = tofHit->GetRefId();
	if(tofPointIndex < 0) continue;
	tofPoint = (CbmTofPoint*) fArrayTofPoint->At(tofPointIndex);
	if(NULL == tofPoint) continue;
	// Get MC track index
	mcTrackIndex = tofPoint->GetTrackID();
        mapMCTof[mcTrackIndex] = iTofHit;
    }

    // Loop over global tracks
    for(Int_t iGlbTrack = 0; iGlbTrack < nGlbTracks; iGlbTrack++) {
	// Get pointer to the global track
	glbTrack = (CbmGlobalTrack*) glbTracks->At(iGlbTrack);
	if(NULL == glbTrack) continue;
	// Get pointer to the STS or TRD track match
	stsTrackIndex = glbTrack->GetStsTrackIndex();
	if(stsTrackIndex >= 0) {
	    stsMatch = (CbmTrackMatch*) fArrayStsTrackMatch->
		At(stsTrackIndex);
	    if(NULL == stsMatch) continue;
            mcTrackIndex = stsMatch->GetMCTrackId();
	} else {
	    trdTrackIndex = glbTrack->GetTrdTrackIndex();
	    if(trdTrackIndex < 0) continue;
	    trdMatch = (CbmTrackMatch*) fArrayTrdTrackMatch->
		At(trdTrackIndex);
	    if(NULL == trdMatch) continue;
            mcTrackIndex = trdMatch->GetMCTrackId();
	}
	// Get TOF hit index
        if(mapMCTof.find(mcTrackIndex)==mapMCTof.end()) continue;
	tofHitIndex = mapMCTof[mcTrackIndex];
	// Attach TOF hit to the global track
	glbTrack->SetTofHitIndex(tofHitIndex);
        nMerged += 1;
    }

    return nMerged;
}
// ------------------------------------------------------------------


ClassImp(CbmTofMergerIdeal);


