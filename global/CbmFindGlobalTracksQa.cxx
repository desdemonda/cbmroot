// ------------------------------------------------------------------
// -----                  CbmFindGlobalTracksQa                 -----
// -----             Created 26-01-2006 by D.Kresan             -----
// ------------------------------------------------------------------
#include "CbmFindGlobalTracksQa.h"

#include "CbmGlobalTrack.h"
#include "CbmMCTrack.h"
#include "CbmTofPoint.h"
#include "CbmTofHit.h"
#include "CbmTrackMatch.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TH1.h"
#include "TDirectory.h"

#include <iostream>
#include <map>
#include <set>

using std::cout;
using std::endl;
using std::set;
using std::map;

//___________________________________________________________________
//
// CbmFindGlobalTracksQa
//
// Task for performance analysis of the global track finding. Number
// of required STS points is set using SetMinStsPoints() method. To
// include/exclude TOF use SetRequireTof(kTRUE/kFALSE) method.
//


// ------------------------------------------------------------------
CbmFindGlobalTracksQa::CbmFindGlobalTracksQa()
  : FairTask(),
    fMinStsPoints(4),
    fMinTrdPoints(12),
    fRequireTof(kTRUE),
    fArrayMCTrack(NULL),
    fArrayTofPoint(NULL),
    fArrayTofHit(NULL),  
    fArrayGlbTrack(NULL),
    fArrayStsTrackMatch(NULL),
    fArrayTrdTrackMatch(NULL),
    fh_mom_acc(NULL),        
    fh_mom_acc2(NULL),       
    fh_mom_rec(NULL),        
    fh_mom_ghost(NULL),      
    fh_mom_eff(NULL),        
    fh_mom_eff_ghost(NULL),  
    fh_mom_eff_tof(NULL),    
    fh_mom_eff_tof_ghost(NULL)
{
    CreateHistogramms();
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmFindGlobalTracksQa::CbmFindGlobalTracksQa(const char* name,
					     Int_t verbose)
  : FairTask(name, verbose),
    fMinStsPoints(4),
    fMinTrdPoints(12),
    fRequireTof(kTRUE),
    fArrayMCTrack(NULL),
    fArrayTofPoint(NULL),
    fArrayTofHit(NULL),  
    fArrayGlbTrack(NULL),
    fArrayStsTrackMatch(NULL),
    fArrayTrdTrackMatch(NULL),
    fh_mom_acc(NULL),        
    fh_mom_acc2(NULL),       
    fh_mom_rec(NULL),        
    fh_mom_ghost(NULL),      
    fh_mom_eff(NULL),        
    fh_mom_eff_ghost(NULL),  
    fh_mom_eff_tof(NULL),    
    fh_mom_eff_tof_ghost(NULL)
{
    CreateHistogramms();
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmFindGlobalTracksQa::~CbmFindGlobalTracksQa()
{
    // Destructor
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
InitStatus CbmFindGlobalTracksQa::Init()
{
    // Initialisation of the task

    // Get pointer to the ROOT I/O manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmFindGlobalTracksQa::Init :"
	    << " ROOT manager is not instantiated" << endl;
        return kFATAL;
    }

    // Get MC information
    fArrayMCTrack = (TClonesArray*) rootMgr->GetObject("MCTrack");
    if(NULL == fArrayMCTrack) {
	cout << "-W- CbmFindGlobalTracksQa::Init :"
            << " no MC track array!" << endl;
    }
    if(fRequireTof) {
	fArrayTofPoint = (TClonesArray*) rootMgr->GetObject("TofPoint");
	if(NULL == fArrayTofPoint) {
	    cout << "-W- CbmFindGlobalTracksQa::Init :"
		<< " no TOF point array!" << endl;
	}
    }

    // Get hit arrays
    if(fRequireTof) {
	fArrayTofHit = (TClonesArray*) rootMgr->GetObject("TofHit");
	if(NULL == fArrayTofHit) {
	    cout << "-W- CbmFindGlobalTracksQa::Init :"
		<< " no TOF hit array" << endl;
	}
    }

    // Get tracks
    fArrayGlbTrack = (TClonesArray*) rootMgr->GetObject("GlobalTrack");
    if(NULL == fArrayGlbTrack) {
	cout << "-W- CbmFindGlobalTracksQa::Init :"
            << " no global track array" << endl;
    }
    fArrayStsTrackMatch = (TClonesArray*) rootMgr->GetObject("StsTrackMatch");
    if(NULL == fArrayStsTrackMatch) {
	cout << "-W- CbmFindGlobalTracksQa::Init :"
	    << " no STS track match array" << endl;
    }
    fArrayTrdTrackMatch = (TClonesArray*) rootMgr->GetObject("TrdTrackMatch");
    if(NULL == fArrayTrdTrackMatch) {
	cout << "-W- CbmFindGlobalTracksQa::Init :"
	    << " no TRD track match array" << endl;
    }

    return kSUCCESS;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmFindGlobalTracksQa::Exec(Option_t* option)
{
    // Task execution
    if(NULL==fArrayMCTrack || NULL==fArrayGlbTrack ||
       NULL==fArrayStsTrackMatch || NULL==fArrayTrdTrackMatch) return;
    if(fRequireTof) {
        if(NULL==fArrayTofPoint || NULL==fArrayTofHit) return;
    }

    // STS quality
    set<Int_t> matchSts;
    map<Int_t, Int_t> mapMC2Sts;
    map<Int_t, Int_t> mapSts2MC;
    map<Int_t, Double_t> mapMCStsQual;
    map<Int_t, Bool_t> mapStsGood;

    // Loop over STS track matches
    CbmTrackMatch* stsTrackMatch;
    Double_t qual;
    Double_t old_qual;
    Int_t mcTrackIndex;
    for(Int_t iStsTrack = 0;
	iStsTrack < fArrayStsTrackMatch->GetEntriesFast();
	iStsTrack++) {
	// Get pointer to the STS track match
	stsTrackMatch = (CbmTrackMatch*) fArrayStsTrackMatch->
	    At(iStsTrack);
	if(NULL == stsTrackMatch) {
	    mapSts2MC[iStsTrack] = -1;
	    continue;
	}
	// Mark this MC track as reco in STS
        matchSts.insert(stsTrackMatch->GetMCTrackId());
	// Get quality
	qual = (Double_t) stsTrackMatch->GetNofTrueHits() /
	    (Double_t) (stsTrackMatch->GetNofTrueHits() +
			stsTrackMatch->GetNofWrongHits() +
			stsTrackMatch->GetNofFakeHits());
	if(qual < 0.7) {
            // Ghost
	    mapSts2MC[iStsTrack] = -1;
            continue;
	}
	// Get MC track index
        mcTrackIndex = stsTrackMatch->GetMCTrackId();
	// Look for another STS track mathed with this MC track
	if(mapMC2Sts.find(mcTrackIndex) == mapMC2Sts.end()) {
	    mapMC2Sts[mcTrackIndex] = iStsTrack;
	    mapSts2MC[iStsTrack] = mcTrackIndex;
	    mapMCStsQual[mcTrackIndex] = qual;
            mapStsGood[iStsTrack] = kTRUE;
	} else {
	    old_qual = mapMCStsQual[mcTrackIndex];
	    if(qual < old_qual) {
                // Clone
		mapSts2MC[iStsTrack] = -1;
		continue;
	    } else {
		// Previous track is clone
		mapStsGood[mapMC2Sts[mcTrackIndex]] = kFALSE;
		mapSts2MC[mapMC2Sts[mcTrackIndex]] = -1;

		mapMC2Sts[mcTrackIndex] = iStsTrack;
		mapSts2MC[iStsTrack] = mcTrackIndex;
		mapMCStsQual[mcTrackIndex] = qual;
		mapStsGood[iStsTrack] = kTRUE;
	    }
	}
    }

    // TRD quality
    set<Int_t> matchTrd;
    map<Int_t, Int_t> mapMC2Trd;
    map<Int_t, Int_t> mapTrd2MC;
    map<Int_t, Double_t> mapMCTrdQual;
    map<Int_t, Bool_t> mapTrdGood;

    // Loop over TRD track matches
    CbmTrackMatch* trdTrackMatch;
    for(Int_t iTrdTrack = 0;
	iTrdTrack < fArrayTrdTrackMatch->GetEntriesFast();
	iTrdTrack++) {
	// Get pointer to the TRD track match
	trdTrackMatch = (CbmTrackMatch*) fArrayTrdTrackMatch->
	    At(iTrdTrack);
	if(NULL == trdTrackMatch) {
            mapTrd2MC[iTrdTrack] = -1;
	    continue;
	}
	// Mark this MC track as reco in TRD
        matchTrd.insert(trdTrackMatch->GetMCTrackId());
	// Get quality
	qual = (Double_t) trdTrackMatch->GetNofTrueHits() /
	    (Double_t) (trdTrackMatch->GetNofTrueHits() +
			trdTrackMatch->GetNofWrongHits() +
			trdTrackMatch->GetNofFakeHits());
	if(qual < 0.7) {
	    // Ghost
            mapTrd2MC[iTrdTrack] = -1;
	    continue;
	}
	// Get MC track index
        mcTrackIndex = trdTrackMatch->GetMCTrackId();
	// Look for another TRD track mathed with this MC track
	if(mapMC2Trd.find(mcTrackIndex) == mapMC2Trd.end()) {
	    mapMC2Trd[mcTrackIndex] = iTrdTrack;
	    mapTrd2MC[iTrdTrack] = mcTrackIndex;
	    mapMCTrdQual[mcTrackIndex] = qual;
            mapTrdGood[iTrdTrack] = kTRUE;
	} else {
	    old_qual = mapMCTrdQual[mcTrackIndex];
	    if(qual < old_qual) {
		// Clone
		mapTrd2MC[iTrdTrack] = -1;
		continue;
	    } else {
		// Previous track is clone
		mapTrdGood[mapMC2Trd[mcTrackIndex]] = kFALSE;
		mapTrd2MC[mapMC2Trd[mcTrackIndex]] = -1;

		mapMC2Trd[mcTrackIndex] = iTrdTrack;
		mapTrd2MC[iTrdTrack] = mcTrackIndex;
		mapMCTrdQual[mcTrackIndex] = qual;
		mapTrdGood[iTrdTrack] = kTRUE;
	    }
	}
    }

    // Loop over global tracks
    map<Int_t, Int_t> mapMC2Glb;

    Int_t stsTrackIndex;
    Int_t trdTrackIndex;
    Int_t tofHitIndex;
    Int_t pointIndex;
    CbmTofPoint* tofPoint;
    CbmTofHit* tofHit;
    CbmGlobalTrack* glbTrack;
    for(Int_t iGlbTrack = 0;
	iGlbTrack < fArrayGlbTrack->GetEntriesFast();
	iGlbTrack++) {
	// Get pointer to the global track
	glbTrack = (CbmGlobalTrack*) fArrayGlbTrack->At(iGlbTrack);
	if(NULL == glbTrack) continue;

	// Check STS and TRD
	stsTrackIndex = glbTrack->GetStsTrackIndex();
	trdTrackIndex = glbTrack->GetTrdTrackIndex();
	if(stsTrackIndex < 0 || trdTrackIndex < 0) continue;

        // Check STS and TRD quality and STS - TRD correspondance
	if(mapSts2MC[stsTrackIndex] != mapTrd2MC[trdTrackIndex]) {
	    // Ghost
            continue;
	}

        // Store MC track index
        mcTrackIndex = mapSts2MC[stsTrackIndex];

	// Check for TOF hit
	if(fRequireTof) {
	    tofHitIndex = glbTrack->GetTofHitIndex();
	    if(tofHitIndex < 0) {
		continue;
	    } else {
		tofHit = (CbmTofHit*) fArrayTofHit->At(tofHitIndex);
		if(NULL != tofHit) {
		    // Check for double hit
		    if(1 == tofHit->GetFlag()) {
			// Check for MC track ID
			pointIndex = tofHit->GetRefId();
			if(pointIndex >= 0) {
			    tofPoint = (CbmTofPoint*) fArrayTofPoint->At(pointIndex);
			    if(NULL != tofPoint) {
				if(tofPoint->GetTrackID() == mcTrackIndex) {
				    // Reconstructed MC track
				    mapMC2Glb[mcTrackIndex] = iGlbTrack;
				}
			    }
			}
		    }
		}
	    }
	} else { // No TOF requested
            // Reconstructed MC track
	    mapMC2Glb[mcTrackIndex] = iGlbTrack;
	}
    }




    // Counters
    Int_t n_acc = 0;
    Int_t n_acc2 = 0;
    Int_t n_rec = 0;
    Int_t n_ghost = 0;

    // Loop over MC tracks
    CbmMCTrack* mcTrack;
    Float_t mom;
    for(Int_t iMCTrack = 0;
	iMCTrack < fArrayMCTrack->GetEntriesFast();
	iMCTrack++) {
	// Get pointer to the MC track
	mcTrack = (CbmMCTrack*) fArrayMCTrack->At(iMCTrack);
	if(NULL == mcTrack) continue;
	// Cut on number of STS points
	if( mcTrack->GetNPoints(kSTS) < fMinStsPoints ) continue;
	// Cut on number of TRD points
	if( mcTrack->GetNPoints(kTRD) < fMinTrdPoints ) continue;
	// Cut on number of TOF points, if requested
	if(fRequireTof) {
	    if( mcTrack->GetNPoints(kTOF) < 1 ) continue;
	}

	// Get momentum value
	mom = mcTrack->GetP();

	// This track is accepted
	n_acc += 1;
	fh_mom_acc->Fill(mom);

	// Check if reco in STS and TRD
        if(matchSts.find(iMCTrack) == matchSts.end()) continue;
	if(matchTrd.find(iMCTrack) == matchTrd.end()) continue;

	// This track is accepted and reco in STS and TRD
	n_acc2 += 1;
        fh_mom_acc2->Fill(mom);

	// Check if matched to the global track
	if(mapMC2Glb.find(iMCTrack) == mapMC2Glb.end()) {
	    // Ghost
	    n_ghost += 1;
            fh_mom_ghost->Fill(mom);
	} else {
	    n_rec += 1;
            fh_mom_rec->Fill(mom);
	}
    }

    if(fVerbose > 0) {
	cout << "-I-          CbmFindGlobalTracksQa          -I-" << endl
	    << "Number of acc MC tracks                  :  " << n_acc << endl
	    << "Number of acc MC tracks (reco in STS TRD):  " << n_acc2 << endl
	    << "Number of good global tracks             :  " << n_rec << endl
	    << "Number of ghost global tracks            :  " << n_ghost << endl;
    }
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmFindGlobalTracksQa::Finish()
{
    // Finish of the task execution
    WriteHistogramms();
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmFindGlobalTracksQa::CreateHistogramms()
{
    // Create control histogramms
    fh_mom_acc = new TH1F("h_mom_acc",
			  "Momentum of acc MC tracks",
			  40, 0., 10.);
    fh_mom_acc2 = new TH1F("h_mom_acc2",
			   "Momentum of acc MC tracks, which are reco in STS and TRD",
			   40, 0., 10.);
    fh_mom_rec = new TH1F("h_mom_rec",
			  "Momentum of good global tracks",
			  40, 0., 10.);
    fh_mom_ghost = new TH1F("h_mom_ghost",
			    "Momentum of ghost global tracks",
			    40, 0., 10.);
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmFindGlobalTracksQa::WriteHistogramms()
{
    // Write control histogramms to file

    TDirectory *olddir = gDirectory;
    TDirectory *dir = new TDirectory("FindGlobalTracksQa",
				     "Performance of global tracking");
    dir->cd();

    fh_mom_acc->Write();
    fh_mom_acc2->Write();
    fh_mom_rec->Write();
    fh_mom_ghost->Write();


    fh_mom_eff = new TH1F("h_mom_eff",
			  "", 40, 0., 10.);
    fh_mom_eff->Sumw2();
    fh_mom_eff->Divide(fh_mom_rec, fh_mom_acc,
		       1, 1, "B");
    fh_mom_eff->Write();


    fh_mom_eff_ghost = new TH1F("h_mom_eff_ghost",
				"", 40, 0., 10.);
    fh_mom_eff_ghost->Sumw2();
    fh_mom_eff_ghost->Divide(fh_mom_ghost, fh_mom_acc,
			     1, 1, "B");
    fh_mom_eff_ghost->Write();


    fh_mom_eff_tof = new TH1F("h_mom_eff_tof",
			      "", 40, 0., 10.);
    fh_mom_eff_tof->Sumw2();
    fh_mom_eff_tof->Divide(fh_mom_rec, fh_mom_acc2,
			   1, 1, "B");
    fh_mom_eff_tof->Write();


    fh_mom_eff_tof_ghost = new TH1F("h_mom_eff_tof_ghost",
				    "", 40, 0., 10.);
    fh_mom_eff_tof_ghost->Sumw2();
    fh_mom_eff_tof_ghost->Divide(fh_mom_ghost, fh_mom_acc2,
				 1, 1, "B");
    fh_mom_eff_tof_ghost->Write();


    Double_t eff = 100 * fh_mom_eff->Integral() / 40.;
    Double_t ghost = 100 * fh_mom_eff_ghost->Integral() / 40.;
    Double_t eff_tof = 100 * fh_mom_eff_tof->Integral() / 40.;
    Double_t ghost_tof = 100 * fh_mom_eff_tof_ghost->Integral() / 40.;


    cout << endl
	<< "-I- CbmFindGlobalTracksQa:           reco: " << eff << " %" << endl
	<< "-I- CbmFindGlobalTracksQa:          ghost: " << ghost << " %" << endl
	<< "-I- CbmFindGlobalTracksQa:      track-tof: " << eff_tof << " %" << endl
	<< "-I- CbmFindGlobalTracksQa: fake track-tof: " << ghost_tof << " %" << endl
	<< endl;

    olddir->cd();
}
// ------------------------------------------------------------------


ClassImp(CbmFindGlobalTracksQa);



