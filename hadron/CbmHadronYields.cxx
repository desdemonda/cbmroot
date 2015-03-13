// ------------------------------------------------------------------
// -----                     CbmHadronYields                    -----
// -----              Created 18/09/2006 by D.Kresan            -----
// ------------------------------------------------------------------
#include <iostream>
using namespace std;

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmTrdTrack.h"
#include "CbmTofHit.h"
#include "CbmTofPoint.h"
#include "CbmGlobalTrack.h"
#include "CbmHadronYields.h"

//___________________________________________________________________
//
// CbmHadronYields
//
// Task for calculating the particles' yields
//


// ------------------------------------------------------------------
CbmHadronYields::CbmHadronYields()
  : FairTask("HadronYields"),
    fEvents(0),
    fArrayMCTrack(NULL),
    fArrayStsTrack(NULL),
    fArrayStsTrackM(NULL),
    fArrayTrdTrack(NULL),
    fArrayTrdTrackM(NULL),
    fArrayTofHit(NULL),
    fArrayTofPoint(NULL),
    fArrayGlbTrack(NULL),
    fmap_all(),
    fmap_sts_acc(),
    fmap_trd_acc(),
    fmap_glb_acc(),
    fmap_sts_rec(),
    fmap_trd_rec(),
    fmap_glb_rec()
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmHadronYields::CbmHadronYields(const char* name, Int_t verbose)
  : FairTask(name, verbose),
    fEvents(0),
    fArrayMCTrack(NULL),
    fArrayStsTrack(NULL),
    fArrayStsTrackM(NULL),
    fArrayTrdTrack(NULL),
    fArrayTrdTrackM(NULL),
    fArrayTofHit(NULL),
    fArrayTofPoint(NULL),
    fArrayGlbTrack(NULL),
    fmap_all(),
    fmap_sts_acc(),
    fmap_trd_acc(),
    fmap_glb_acc(),
    fmap_sts_rec(),
    fmap_trd_rec(),
    fmap_glb_rec()
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmHadronYields::~CbmHadronYields()
{
    // Destructor
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
InitStatus CbmHadronYields::Init()
{
    // Task initialisation
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmHadronYields::Init : "
	    << "ROOT manager is not instantiated." << endl;
        return kFATAL;
    }

    fArrayMCTrack = (TClonesArray*) rootMgr->GetObject("MCTrack");
    if(NULL == fArrayMCTrack) {
	cout << "-W- CbmHadronYields::Init : "
            << "no MC track array!" << endl;
    }

    fArrayStsTrack = (TClonesArray*) rootMgr->GetObject("STSTrack");
    if(NULL == fArrayStsTrack) {
	cout << "-W- CbmHadronYields::Init : "
	    << "no STS track array!" << endl;
    }

    fArrayStsTrackM = (TClonesArray*) rootMgr->GetObject("STSTrackMatch");
    if(NULL == fArrayStsTrackM) {
	cout << "-W- CbmHadronYields::Init : "
	    << "no STS track match array!" << endl;
    }

    fArrayTrdTrack = (TClonesArray*) rootMgr->GetObject("TRDTrack");
    if(NULL == fArrayTrdTrack) {
	cout << "-W- CbmHadronYields::Init : "
	    << "no TRD track array!" << endl;
    }

    fArrayTrdTrackM = (TClonesArray*) rootMgr->GetObject("TRDTrackMatch");
    if(NULL == fArrayTrdTrackM) {
	cout << "-W- CbmHadronYields::Init : "
	    << "no TRD track match array!" << endl;
    }

    fArrayTofHit = (TClonesArray*) rootMgr->GetObject("TofHit");
    if(NULL == fArrayTofHit) {
	cout << "-W- CbmHadronYields::Init : "
	    << "no TOF hit array!" << endl;
    }

    fArrayTofPoint = (TClonesArray*) rootMgr->GetObject("TOFPoint");
    if(NULL == fArrayTofPoint) {
	cout << "-W- CbmHadronYields::Init : "
	    << "no TOF point array!" << endl;
    }


    fArrayGlbTrack = (TClonesArray*) rootMgr->GetObject("GlobalTrack");
    if(NULL == fArrayGlbTrack) {
	cout << "-W- CbmHadronYields::Init : "
	    << "no global track array!" << endl;
    }

    cout << "-I- CbmHadronYields::Init : "
        << "initialisation completed." << endl;

    return kSUCCESS;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmHadronYields::Exec(Option_t* option)
{
    // Task execution

    // STS quality
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
	iStsTrack < fArrayStsTrackM->GetEntriesFast();
	iStsTrack++) {
	// Get pointer to the STS track match
	stsTrackMatch = (CbmTrackMatch*) fArrayStsTrackM->
	    At(iStsTrack);
	if(NULL == stsTrackMatch) {
	    mapSts2MC[iStsTrack] = -1;
	    continue;
	}
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
    map<Int_t, Int_t> mapMC2Trd;
    map<Int_t, Int_t> mapTrd2MC;
    map<Int_t, Double_t> mapMCTrdQual;
    map<Int_t, Bool_t> mapTrdGood;

    // Loop over TRD track matches
    CbmTrackMatch* trdTrackMatch;
    for(Int_t iTrdTrack = 0;
	iTrdTrack < fArrayTrdTrackM->GetEntriesFast();
	iTrdTrack++) {
	// Get pointer to the TRD track match
	trdTrackMatch = (CbmTrackMatch*) fArrayTrdTrackM->
	    At(iTrdTrack);
	if(NULL == trdTrackMatch) {
            mapTrd2MC[iTrdTrack] = -1;
	    continue;
	}
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
    }


    // Loop over MC tracks
    CbmMCTrack *mcTrack;
    Int_t pdg;
    for(Int_t iMC = 0; iMC < fArrayMCTrack->GetEntriesFast(); iMC++) {
        // Get pointer to the track
	mcTrack = (CbmMCTrack*) fArrayMCTrack->At(iMC);
	if(NULL == mcTrack) continue;

	// Count all prim. hadrons
        if(-1 != mcTrack->GetMotherId()) continue;
	pdg = mcTrack->GetPdgCode();
	if(211 != abs(pdg) &&
	   321 != abs(pdg) &&
	   2212!= abs(pdg)) continue;
        fmap_all[pdg] += 1;

	// Count accepted prim. hadrons
	if(mcTrack->GetNPoints(kSTS) >= 4) {
	    fmap_sts_acc[pdg] += 1;
	}
	if(mcTrack->GetNPoints(kTRD) >= 12) {
	    fmap_trd_acc[pdg] += 1;
	}
	if(mcTrack->GetNPoints(kSTS) >= 4 &&
	   mcTrack->GetNPoints(kTRD) >= 12 &&
	   mcTrack->GetNPoints(kTOF) >= 1) {
	    fmap_glb_acc[pdg] += 1;
	}

	// Reco in STS
	if(mapMC2Sts.find(iMC) != mapMC2Sts.end()) {
	    fmap_sts_rec[pdg] += 1;
	}
	// Reco in TRD
	if(mapMC2Trd.find(iMC) != mapMC2Trd.end()) {
	    fmap_trd_rec[pdg] += 1;
	}
	// Globaly reco
	if(mapMC2Glb.find(iMC) != mapMC2Glb.end()) {
	    fmap_glb_rec[pdg] += 1;
	}
    }


    if(0 == (fEvents%10)) {
	cout << "-I- CbmHadronYields::Exec : "
            << "event " << fEvents << " processed." << endl;
    }
    fEvents += 1;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmHadronYields::Finish()
{
    // Finish of the task execution
    Int_t n_all_pip = fmap_all[211];
    Int_t n_all_kap = fmap_all[321];
    Int_t n_all_prot = fmap_all[2212];

    Int_t n_sts_acc_pip = fmap_sts_acc[211];
    Int_t n_sts_acc_kap = fmap_sts_acc[321];
    Int_t n_sts_acc_prot = fmap_sts_acc[2212];

    Int_t n_trd_acc_pip = fmap_trd_acc[211];
    Int_t n_trd_acc_kap = fmap_trd_acc[321];
    Int_t n_trd_acc_prot = fmap_trd_acc[2212];

    Int_t n_glb_acc_pip = fmap_glb_acc[211];
    Int_t n_glb_acc_kap = fmap_glb_acc[321];
    Int_t n_glb_acc_prot = fmap_glb_acc[2212];

    Int_t n_sts_rec_pip = fmap_sts_rec[211];
    Int_t n_sts_rec_kap = fmap_sts_rec[321];
    Int_t n_sts_rec_prot = fmap_sts_rec[2212];

    Int_t n_trd_rec_pip = fmap_trd_rec[211];
    Int_t n_trd_rec_kap = fmap_trd_rec[321];
    Int_t n_trd_rec_prot = fmap_trd_rec[2212];

    Int_t n_glb_rec_pip = fmap_glb_rec[211];
    Int_t n_glb_rec_kap = fmap_glb_rec[321];
    Int_t n_glb_rec_prot = fmap_glb_rec[2212];

    cout << endl
        << "-I-------------------------------------------I-" << endl
	<< "-I              CbmHadronYields              I-" << endl
	<< "-I" << endl
	<< "-I" << endl
	<< "-I   All : " << endl
	<< "-I   <pi+> = " << (Double_t)n_all_pip / (Double_t)fEvents << endl
	<< "-I   <K+> = " << (Double_t)n_all_kap / (Double_t)fEvents << endl
	<< "-I   <p> = " << (Double_t)n_all_prot / (Double_t)fEvents << endl
	<< "-I" << endl
	<< "-I" << endl
	<< "-I   Accepted in STS : " << endl
	<< "-I   <pi+> = " << (Double_t)n_sts_acc_pip / (Double_t)fEvents << endl
	<< "-I   <K+> = " << (Double_t)n_sts_acc_kap / (Double_t)fEvents << endl
	<< "-I   <p> = " << (Double_t)n_sts_acc_prot / (Double_t)fEvents << endl
	<< "-I" << endl
	<< "-I   Accepted in TRD : " << endl
	<< "-I   <pi+> = " << (Double_t)n_trd_acc_pip / (Double_t)fEvents << endl
	<< "-I   <K+> = " << (Double_t)n_trd_acc_kap / (Double_t)fEvents << endl
	<< "-I   <p> = " << (Double_t)n_trd_acc_prot / (Double_t)fEvents << endl
	<< "-I" << endl
	<< "-I   Accepted in STS+TRD+TOF : " << endl
	<< "-I   <pi+> = " << (Double_t)n_glb_acc_pip / (Double_t)fEvents << endl
	<< "-I   <K+> = " << (Double_t)n_glb_acc_kap / (Double_t)fEvents << endl
	<< "-I   <p> = " << (Double_t)n_glb_acc_prot / (Double_t)fEvents << endl
	<< "-I" << endl
	<< "-I" << endl
	<< "-I   Reco in STS : " << endl
	<< "-I   <pi+> = " << (Double_t)n_sts_rec_pip / (Double_t)fEvents << endl
	<< "-I   <K+> = " << (Double_t)n_sts_rec_kap / (Double_t)fEvents << endl
	<< "-I   <p> = " << (Double_t)n_sts_rec_prot / (Double_t)fEvents << endl
	<< "-I" << endl
	<< "-I   Reco in TRD : " << endl
	<< "-I   <pi+> = " << (Double_t)n_trd_rec_pip / (Double_t)fEvents << endl
	<< "-I   <K+> = " << (Double_t)n_trd_rec_kap / (Double_t)fEvents << endl
	<< "-I   <p> = " << (Double_t)n_trd_rec_prot / (Double_t)fEvents << endl
	<< "-I" << endl
	<< "-I   Reco in STS+TRD+TOF : " << endl
	<< "-I   <pi+> = " << (Double_t)n_glb_rec_pip / (Double_t)fEvents << endl
	<< "-I   <K+> = " << (Double_t)n_glb_rec_kap / (Double_t)fEvents << endl
	<< "-I   <p> = " << (Double_t)n_glb_rec_prot / (Double_t)fEvents << endl
	<< "-I-------------------------------------------I-" << endl
	<< endl;
}
// ------------------------------------------------------------------


ClassImp(CbmHadronYields);


