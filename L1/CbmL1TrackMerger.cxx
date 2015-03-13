// ------------------------------------------------------------------
// -----                    CbmL1TrackMerger                    -----
// -----             Created 2006-01-24 by D.Kresan             -----
// ------------------------------------------------------------------
#include "CbmL1TrackMerger.h"

#include "CbmL1Def.h"

#include "FairRootManager.h"
#include "CbmTrackMatch.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmKFTrack.h"

#include "TClonesArray.h"
#include "TH2F.h"
#include "TMath.h"

#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::map;

//___________________________________________________________________
//
// CbmL1TrackMerger
//
// Concrete implementation of the STS-TRD track merging.
// Two methods are availible:
// 1) Merging based on the value of CbmTrdTrack::GetStsTrackIndex().
//    Selected by ::SetMethod(1). To be used for STS-based TRD track
//    finder
// 2) Merging at the first TRD plane. Selected by ::SetMethod(2). To
//    be used for standalone TRD track finder
//


// ------------------------------------------------------------------
CbmL1TrackMerger::CbmL1TrackMerger():
fMethod(1),         // Merging method: 1 - based on StsTrackIndex from TRD track
fArrayStsTrackM(NULL), // Array of STS track matches
fArrayTrdTrackM(NULL), // Array of TRD track matches
fh_dx_true(0),      // Control histogramm
fh_dx_false(0),     // Control histogramm
fh_dy_true(0),      // Control histogramm
fh_dy_false(0),     // Control histogramm
fh_dtx_true(0),     // Control histogramm
fh_dtx_false(0),    // Control histogramm
fh_dty_true(0),     // Control histogramm
fh_dty_false(0)    // Control histogramm
{
    // Default constructor
    fVerbose = 1;
    CreateHistogramms();
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmL1TrackMerger::CbmL1TrackMerger(Int_t verbose):
    fMethod(1),         // Merging method: 1 - based on StsTrackIndex from TRD track
fArrayStsTrackM(NULL), // Array of STS track matches
fArrayTrdTrackM(NULL), // Array of TRD track matches
fh_dx_true(0),      // Control histogramm
fh_dx_false(0),     // Control histogramm
fh_dy_true(0),      // Control histogramm
fh_dy_false(0),     // Control histogramm
fh_dtx_true(0),     // Control histogramm
fh_dtx_false(0),    // Control histogramm
fh_dty_true(0),     // Control histogramm
fh_dty_false(0)    // Control histogramm
{
    // Standard constructor
    fVerbose = verbose;
    CreateHistogramms();
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmL1TrackMerger::~CbmL1TrackMerger()
{
    // Destructor
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmL1TrackMerger::Init()
{
    // Initialisation
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmL1TrackMerger::Init : "
	    << "FairRootManager is not instantiated!" << endl;
        return;
    }
    fArrayStsTrackM = L1_DYNAMIC_CAST<TClonesArray*>( rootMgr->GetObject("StsTrackMatch") );
    if(NULL == fArrayStsTrackM) {
	cout << "-W- CbmL1TrackMerger::Init : "
            << "no STS track match array" << endl;
    }
    fArrayTrdTrackM = L1_DYNAMIC_CAST<TClonesArray*>( rootMgr->GetObject("TrdTrackMatch") );
    if(NULL == fArrayTrdTrackM) {
	cout << "-W- CbmL1TrackMerger::Init : "
            << "no TRD track match array" << endl;
    }
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
Int_t CbmL1TrackMerger::DoMerge(TClonesArray* stsTracks,
				TClonesArray* trdTracks,
				TClonesArray* glbTracks)
{
    // Implementation of the merging algorithm
    if(1 == fMethod) {
	return MergeSimple(stsTracks, trdTracks, glbTracks);
    } else if(2 == fMethod) {
	return MergeImPlane(stsTracks, trdTracks, glbTracks);
    } else {
	cout << "-E- CbmL1TrackMerger::DoMerge : "
	    << "unknown method " << fMethod << endl;
    }
    return 0;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
Int_t CbmL1TrackMerger::MergeSimple(TClonesArray* stsTracks,
				    TClonesArray* trdTracks,
				    TClonesArray* glbTracks)
{
    // Simple merging algorithm. To be used for STS based TRD track
    // finding
    Int_t nGlb = 0;

    CbmTrdTrack* trdTrack = NULL;
    CbmGlobalTrack* glbTrack = NULL;

    map<Int_t, Int_t> mapStsTrackUsed;

    // Loop over TRD tracks, create corresponding global track
    // and attach STS track
    for(Int_t iTrdTrack = 0; iTrdTrack < trdTracks->GetEntriesFast(); iTrdTrack++) {
	// Get pointer to the TRD track
      trdTrack = L1_DYNAMIC_CAST<CbmTrdTrack*>( trdTracks->At(iTrdTrack) );
	if(NULL == trdTrack) continue;
	// Create global track
	glbTrack = new ((*glbTracks)[nGlb]) CbmGlobalTrack();
	nGlb += 1;
	// Set STS and TRD track indices
	glbTrack->SetStsTrackIndex(trdTrack->GetPreviousTrackId());
	glbTrack->SetTrdTrackIndex(iTrdTrack);
	// Mark STS track as used
        mapStsTrackUsed[trdTrack->GetPreviousTrackId()] = kTRUE;
	if(fVerbose > 1) {
	    cout << "-I- CbmL1TrackMerger::MergeSimple" << endl
		<< "                      global track : " << nGlb-1
		<< "                         sts track : "
		<< trdTrack->GetPreviousTrackId() << endl
		<< "                         trd track : " << iTrdTrack
                << endl;
	}
    }// Loop over TRD tracks

    // Loop over STS tracks
    for(Int_t iSts = 0; iSts < stsTracks->GetEntriesFast(); iSts++) {
        // Skip, if already used
        if(mapStsTrackUsed[iSts]) continue;
	// Create global track
	glbTrack = new ((*glbTracks)[nGlb]) CbmGlobalTrack();
        nGlb += 1;
        glbTrack->SetStsTrackIndex(iSts);
	if(fVerbose > 1) {
	    cout << "-I- CbmL1TrackMerger::MergeSimple" << endl
		<< "                      global track : " << nGlb-1
		<< "                         sts track : " << iSts
		<< "                         trd track : " << -1
                << endl;
	}
    }

    return nGlb;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
Int_t CbmL1TrackMerger::MergeImPlane(TClonesArray* stsTracks,
				     TClonesArray* trdTracks,
				     TClonesArray* glbTracks)
{
    // Real merging algorithm. To be used for standalone
    // TRD track finders
    if(!stsTracks || !trdTracks || !glbTracks) return 0;
    if(!fArrayStsTrackM || !fArrayTrdTrackM) return 0;

    CbmStsTrack* stsTrack;
    CbmKFTrack kfTrack;
    CbmTrdTrack* trdTrack;
    CbmGlobalTrack* glbTrack;
    CbmTrackMatch* stsTrackM;
    CbmTrackMatch* trdTrackM;
    Int_t nGlb = 0;
    Int_t nMerged = 0;
    Double_t dx;
    Double_t dy;
    Double_t dtx;
    Double_t dty;
    Double_t theta;
    Double_t chi2XY;
    Double_t dx_cut;
    Double_t dy_cut;
    Double_t dtx_cut;

    // Map from the TRD track index to the boolean flag
    map<Int_t, Bool_t> mapTrdTrackUsed;

    // Loop over STS tracks
    for(Int_t iStsTrack = 0; iStsTrack < stsTracks->GetEntriesFast(); iStsTrack++) {
	// Get pointer to the STS track and track match
      stsTrack = L1_DYNAMIC_CAST<CbmStsTrack*>( stsTracks->At(iStsTrack) );
	if(NULL == stsTrack) continue;
  stsTrackM = L1_DYNAMIC_CAST<CbmTrackMatch*>( fArrayStsTrackM->At(iStsTrack) );
        if(NULL == stsTrackM) continue;

	// Create global track
	new ((*glbTracks)[nGlb]) CbmGlobalTrack();
  glbTrack = L1_DYNAMIC_CAST<CbmGlobalTrack*>( glbTracks->At(nGlb) );
        if(NULL == glbTrack) continue;
	nGlb += 1;
	// Set STS track index
	glbTrack->SetStsTrackIndex(iStsTrack);
	// Merge with TRD
        kfTrack.SetTrackParam(*stsTrack->GetParamLast());
	// Loop over TRD tracks
	Double_t minChi2XY = 1e16;
        Int_t indexOfClosest = -1;
	for(Int_t iTrdTrack = 0; iTrdTrack < trdTracks->GetEntriesFast(); iTrdTrack++) {
	    // Skip if already merged
            if(mapTrdTrackUsed[iTrdTrack]) continue;
	    // Get pointer to the TRD track and track match
            trdTrack = L1_DYNAMIC_CAST<CbmTrdTrack*>( trdTracks->At(iTrdTrack) );
	    if(NULL == trdTrack) continue;
      trdTrackM = L1_DYNAMIC_CAST<CbmTrackMatch*>( fArrayTrdTrackM->At(iTrdTrack) );
            if(NULL == trdTrackM) continue;
	    // Extrapolate STS track to the first plane of TRD track
	    kfTrack.Extrapolate(trdTrack->GetParamFirst()->GetZ());

	    // Fill histogramms
            dx = kfTrack.GetTrack()[0] - trdTrack->GetParamFirst()->GetX();
	    dy = kfTrack.GetTrack()[1] - trdTrack->GetParamFirst()->GetY();
            dtx = kfTrack.GetTrack()[2] - trdTrack->GetParamFirst()->GetTx();
            dty = kfTrack.GetTrack()[3] - trdTrack->GetParamFirst()->GetTy();
	    theta = 180./TMath::Pi() *
		TMath::ACos(1./TMath::Sqrt(1+
					   TMath::Power(kfTrack.GetTrack()[2],2)+
					   TMath::Power(kfTrack.GetTrack()[3],2)));
	    if(stsTrackM->GetMCTrackId() == trdTrackM->GetMCTrackId()) {
                fh_dx_true->Fill(dx, theta);
	    } else {
                fh_dx_false->Fill(dx, theta);
	    }

	    // Cut on dx
	    if(theta < 5) {
		dx_cut = 1;
	    } else if(theta < 10) {
                dx_cut = 4;
	    } else if(theta < 15) {
                dx_cut = 4;
	    } else if(theta < 20) {
                dx_cut = 4;
	    } else if(theta < 25) {
                dx_cut = 4;
	    } else if(theta < 30) {
                dx_cut = 4;
	    } else if(theta < 35) {
                dx_cut = 4;
	    } else if(theta < 40) {
                dx_cut = 2;
	    } else {
                dx_cut = 0.;
	    }
	    if(TMath::Abs(dx) > dx_cut) continue;
	    // Fill histogramms
	    if(stsTrackM->GetMCTrackId() == trdTrackM->GetMCTrackId()) {
                fh_dy_true->Fill(dy, theta);
	    } else {
                fh_dy_false->Fill(dy, theta);
	    }

	    // Cut on dy
	    if(theta < 5) {
                dy_cut = 1;
	    } else if(theta < 10) {
                dy_cut = 2;
	    } else if(theta < 15) {
                dy_cut = 2;
	    } else if(theta < 20) {
                dy_cut = 3;
	    } else if(theta < 25) {
                dy_cut = 3;
	    } else if(theta < 30) {
                dy_cut = 3;
	    } else if(theta < 35) {
                dy_cut = 3;
	    } else {
                dy_cut = 0.;
	    }
	    if(TMath::Abs(dy) > dy_cut) continue;
            // Fill histogramms
	    if(stsTrackM->GetMCTrackId() == trdTrackM->GetMCTrackId()) {
                fh_dtx_true->Fill(dtx, theta);
	    } else {
                fh_dtx_false->Fill(dtx, theta);
	    }

	    // Cut on dtx
	    if(theta < 10) {
                dtx_cut = 0.001;
	    } else if(theta < 15) {
                dtx_cut = 0.002;
	    } else if(theta < 20) {
                dtx_cut = 0.003;
	    } else if(theta < 25) {
                dtx_cut = 0.004;
	    } else if(theta < 30) {
                dtx_cut = 0.005;
	    } else if(theta < 35) {
                dtx_cut = 0.013;
	    } else {
                dtx_cut = 0.;
	    }
            dtx_cut *= 10;
	    if(TMath::Abs(dtx) > dtx_cut) continue;
            // Fill histogramms
	    if(stsTrackM->GetMCTrackId() == trdTrackM->GetMCTrackId()) {
                fh_dty_true->Fill(dty, theta);
	    } else {
                fh_dty_false->Fill(dty, theta);
	    }

	    if(TMath::Abs(dty) > dtx_cut) continue;

	    // Get chi2
	    chi2XY = GetChi2XY(kfTrack, const_cast<FairTrackParam*>(trdTrack->GetParamFirst()));
            // Choose the closest
	    if(chi2XY < minChi2XY) {
		minChi2XY = chi2XY;
                indexOfClosest = iTrdTrack;
	    }
	}
	if(indexOfClosest < 0) continue;
	// Attach TRD track
	glbTrack->SetTrdTrackIndex(indexOfClosest);
        mapTrdTrackUsed[indexOfClosest] = kTRUE;
        nMerged += 1;
    }

    // Loop over free TRD tracks
    for(Int_t iTrdTrack = 0; iTrdTrack < trdTracks->GetEntriesFast(); iTrdTrack++) {
	// Skip if already merged
	if(mapTrdTrackUsed[iTrdTrack]) continue;
	// Create global track
	new ((*glbTracks)[nGlb]) CbmGlobalTrack();
  glbTrack = L1_DYNAMIC_CAST<CbmGlobalTrack*>( glbTracks->At(nGlb) );
        if(NULL == glbTrack) continue;
	nGlb += 1;
	// Set TRD track index
        glbTrack->SetTrdTrackIndex(iTrdTrack);
    }

    return nMerged;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
Double_t CbmL1TrackMerger::GetChi2XY(CbmKFTrack& kfTrack, FairTrackParam* trackParam)
{
    // Get the chi2 from track extrapolation to TRD track parameters
    Double_t dx = kfTrack.GetTrack()[0] - trackParam->GetX();
    Double_t dy = kfTrack.GetTrack()[1] - trackParam->GetY();
    Double_t c0 = kfTrack.GetCovMatrix()[0] + trackParam->GetCovariance(0, 0);
    Double_t c1 = kfTrack.GetCovMatrix()[1] + trackParam->GetCovariance(0, 1);
    Double_t c2 = kfTrack.GetCovMatrix()[2] + trackParam->GetCovariance(1, 1);
    Double_t chi2 = 0.5*(dx*dx*c0-2*dx*dy*c1+dy*dy*c2)/(c0*c2-c1*c1);
//    Double_t chi2 = TMath::Sqrt(dx*dx + dy*dy);
    return chi2;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmL1TrackMerger::CreateHistogramms()
{
    // Create control histogramms
    fh_dx_true = new TH2F("h_dx_true", "",
			  200, -10., 10.,
			  9, 0., 45.);
    fh_dx_false = new TH2F("h_dx_false", "",
			   200, -10., 10.,
			   9, 0., 45.);

    fh_dy_true = new TH2F("h_dy_true", "",
			  200, -10., 10.,
			  9, 0., 45.);
    fh_dy_false = new TH2F("h_dy_false", "",
			   200, -10., 10.,
			   9, 0., 45.);

    fh_dtx_true = new TH2F("h_dtx_true", "",
			   200, -0.1, 0.1,
			   9, 0., 45.);
    fh_dtx_false = new TH2F("h_dtx_false", "",
			    200, -0.1, 0.1,
			    9, 0., 45.);

    fh_dty_true = new TH2F("h_dty_true", "",
			   200, -0.1, 0.1,
			   9, 0., 45.);
    fh_dty_false = new TH2F("h_dty_false", "",
			    200, -0.1, 0.1,
			    9, 0., 45.);
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmL1TrackMerger::WriteHistogramms()
{
    // Write control histogramms to the file
    fh_dx_true->Write();
    fh_dx_false->Write();
    fh_dy_true->Write();
    fh_dy_false->Write();

    fh_dtx_true->Write();
    fh_dtx_false->Write();
    fh_dty_true->Write();
    fh_dty_false->Write();
}
// ------------------------------------------------------------------


ClassImp(CbmL1TrackMerger);






