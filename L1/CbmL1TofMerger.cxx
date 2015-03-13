// ------------------------------------------------------------------
// -----                     CbmL1TofMerger                     -----
// -----             Created 24-01-2006 by D.Kresan             -----
// ------------------------------------------------------------------
#include "CbmL1TofMerger.h"

#include "CbmL1Def.h"

#include "FairRootManager.h"
#include "CbmTofHit.h"
#include "CbmTrdTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmKFTrack.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>
#include <map>
#include <utility>

using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::make_pair;

//___________________________________________________________________
//
// CbmL1TofMerger
//
// Concrete implementation of the global track - TOF hit merging
// algorithm
//


// ------------------------------------------------------------------
CbmL1TofMerger::CbmL1TofMerger():fArrayTrdTrack(0)
{
    // Default constructor
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmL1TofMerger::CbmL1TofMerger(Int_t verbose):fArrayTrdTrack(0)
{
    // Standard constructor
    fVerbose = 1;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmL1TofMerger::~CbmL1TofMerger()
{
    // Destructor
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmL1TofMerger::Init()
{
    // Initialisation
    FairRootManager *rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmL1TofMerger::Init(): "
	    << "ROOT manager is not instantiated!" << endl;
        return;
    }
    fArrayTrdTrack = L1_DYNAMIC_CAST<TClonesArray*>( rootMgr->GetObject("TrdTrack") );
    if(NULL == fArrayTrdTrack) {
	cout << "-W- CbmL1TofMerger::Init: "
            << "no TRD track array" << endl;
    }
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
Int_t CbmL1TofMerger::DoMerge(TClonesArray* glbTracks,
			      TClonesArray* tofHits)
{
    // Implementation of the merging algorithm
    Int_t nMerged = 0;

    // Map from tof hit index to boolean flag
    map<Int_t, Bool_t> mapTofHitUsed;
    mapTofHitUsed.clear();
    // Map from tof hit index to chi2 value
    map<Int_t, Double_t> mapTofHitChi2;
    mapTofHitChi2.clear();
    // Map from tof hit index to track index
    map<Int_t, Int_t> mapTofHitTrack;
    mapTofHitTrack.clear();
    // Map from track index to the boolean flag
    map<Int_t, Bool_t> mapTrack;
    mapTrack.clear();
    for(Int_t iTrack = 0; iTrack < glbTracks->GetEntriesFast();
	iTrack++) {
        mapTrack[iTrack] = kTRUE;
    }
    // Map from track-tof index to boolean flag
    map<pair<Int_t,Int_t>, Bool_t> mapForbidden;
    mapForbidden.clear();

    // Declare variables outside the loop
    CbmGlobalTrack *track;
    Int_t oldTrackIndex;
    CbmGlobalTrack *track2;
    Int_t trdTrackIndex;
    CbmTrdTrack *trdTrack;
    CbmKFTrack kfTrack;
    CbmTofHit *tofHit;
    Double_t zposTof;
    Double_t chi2;
    Double_t chi2min;
    Int_t indexOfClosest;


    while(mapTrack.size() > 0) {

	// Loop over global tracks
	for(Int_t iTrack = 0; iTrack < glbTracks->GetEntriesFast();
	    iTrack++) {

	    // Skip if not in list
            if(mapTrack.find(iTrack) == mapTrack.end()) continue;

	    // Get pointer to the global track
            track = L1_DYNAMIC_CAST<CbmGlobalTrack*>( glbTracks->At(iTrack) );
	    if(NULL == track) {
		mapTrack.erase(iTrack);
		continue;
	    }

	    // Skip if has no STS or TRD track
	    if(track->GetStsTrackIndex() < 0 ||
	       track->GetTrdTrackIndex() < 0) {
		mapTrack.erase(iTrack);
		continue;
	    }

	    // Get TRD track
	    trdTrackIndex = track->GetTrdTrackIndex();
      trdTrack = L1_DYNAMIC_CAST<CbmTrdTrack*>( fArrayTrdTrack->At(trdTrackIndex) );
	    if(NULL == trdTrack) {
		mapTrack.erase(iTrack);
		continue;
	    }

	    // Set track parameters
	    kfTrack.SetTrackParam(*(const_cast<FairTrackParam*>(trdTrack->GetParamLast())));
	    chi2min = 1e16;
	    indexOfClosest = -1;

	    // Loop over TOF hits
	    for(Int_t iTof = 0; iTof < tofHits->GetEntriesFast();
		iTof++) {
		// Check if pair is not forbidden
                if(mapForbidden[make_pair(iTrack,iTof)]) continue;
		// Get TOF hit
                tofHit = L1_DYNAMIC_CAST<CbmTofHit*>( tofHits->At(iTof) );
		if(NULL == tofHit) continue;
		// Get z position of hit
		zposTof = tofHit->GetZ();
		// Extrapolate to current z-plane
		kfTrack.Extrapolate(zposTof);
		// Check for geometrical overlap
		if(kFALSE == Overlap(kfTrack, tofHit)) continue;
		// Get chi2 to hit
		chi2 = GetChi2ToHit(kfTrack, tofHit);
		// Find smallest chi2
		if(chi2 < chi2min) {
		    chi2min = chi2;
		    indexOfClosest = iTof;
		}
	    } // Loop over TOF hits

	    // Save index of TOF hit
	    if(-1 != indexOfClosest) {
		// Check if already used
		if(mapTofHitUsed[indexOfClosest]) {
		    // Check if this track is closer
		    if(chi2min < mapTofHitChi2[indexOfClosest]) {
			// Force previous track to be reprocessed
      oldTrackIndex = mapTofHitTrack[indexOfClosest];
      track2 = L1_DYNAMIC_CAST<CbmGlobalTrack*>( glbTracks->At(oldTrackIndex) );
			track2->SetTofHitIndex(-1);
			mapTrack[oldTrackIndex] = kTRUE;
                        nMerged -= 1;
			// Forbid this combination
                        mapForbidden[make_pair(oldTrackIndex,indexOfClosest)] = kTRUE;
                        // Attach hit to current track
			track->SetTofHitIndex(indexOfClosest);
			mapTofHitChi2[indexOfClosest] = chi2min;
			mapTofHitTrack[indexOfClosest] = iTrack;
			mapTrack.erase(iTrack);
                        nMerged += 1;
		    } else {
			// Forbid this combination
			mapForbidden[make_pair(iTrack,indexOfClosest)] = kTRUE;
		    }
		} else {
		    track->SetTofHitIndex(indexOfClosest);
		    mapTofHitUsed[indexOfClosest] = kTRUE;
		    mapTofHitChi2[indexOfClosest] = chi2min;
		    mapTofHitTrack[indexOfClosest] = iTrack;
		    mapTrack.erase(iTrack);
		    nMerged += 1;
		}
	    } else {
		mapTrack.erase(iTrack);
	    }

	}// Loop over global tracks

    }


    return nMerged;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
Bool_t CbmL1TofMerger::Overlap(CbmKFTrack& track,
			       const CbmTofHit* tofHit)
{
    // Check for geometrical overlap between track extrapolation
    // and TOF hit
    Double_t x1 = track.GetTrack()[0];
    Double_t x2 = tofHit->GetX();
    Double_t dx2 = tofHit->GetDx();
    Bool_t overlap_x = TMath::Abs(x1-x2) <= (5 + 3*dx2);
    Double_t y1 = track.GetTrack()[1];
    Double_t y2 = tofHit->GetY();
    Double_t dy2 = tofHit->GetDy();
    Bool_t overlap_y = TMath::Abs(y1-y2) <= (5 + 3*dy2);
    return (overlap_x && overlap_y);
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
Double_t CbmL1TofMerger::GetChi2ToHit(CbmKFTrack& track,
				      const CbmTofHit* tofHit)
{
    // Get chi2 from the track extrapolation to the TOF hit
    Double_t dx = track.GetTrack()[0] - tofHit->GetX();
    Double_t dy = track.GetTrack()[1] - tofHit->GetY();
/*    Double_t c0 = track.GetCovMatrix()[0] + TMath::Power(tofHit->GetDx(),2);
    Double_t c1 = track.GetCovMatrix()[1];
    Double_t c2 = track.GetCovMatrix()[2] + TMath::Power(tofHit->GetDy(),2);
    Double_t chi2 = 0.5*(dx*dx*c0-2*dx*dy*c1+dy*dy*c2)/(c0*c2-c1*c1);*/
    Double_t chi2 = TMath::Sqrt(dx*dx + dy*dy);
    return chi2;
}
// ------------------------------------------------------------------


ClassImp(CbmL1TofMerger);



