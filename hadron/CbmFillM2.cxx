// ------------------------------------------------------------------
// -----                         CbmFillM2                      -----
// -----               Created 24-09-2004 by D.Kresan           -----
// ------------------------------------------------------------------
#include <iostream>
#include <map>
using namespace std;

#include "TClonesArray.h"
#include "TF1.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TRandom.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairBaseParSet.h"
#include "FairRuntimeDb.h"
#include "FairDetector.h"
#include "CbmMCTrack.h"
#include "CbmTofPoint.h"
#include "CbmHadron.h"
#include "CbmFillM2.h"


//___________________________________________________________________
//
// CbmFillM2
//
// Task for filling the m2 vs momentum container
//


// ------------------------------------------------------------------
CbmFillM2::CbmFillM2()
  : FairTask("FillM2"),
    fRnd(new TRandom(1)),
    fEvents(0),
    fCoutBunch(1),
    fTimeResolution(0.08),
    fLevel(1),
    fNoTrdStations(0),
    fNoTrdPerStation(0),
    fArrayMCTrack(NULL),
    fArrayTofPoint(NULL),
    fArrayHadron(NULL),
    fh_m2mom_hadron_all(NULL),
    fh_m2mom_hadron(NULL),
    fh_m2mom_hadron_true(NULL),
    fh_m2mom_hadron_10m(NULL), 
    fNbinsM2(90),
    fMinM2(-1.5),
    fMaxM2(3.0),
    fBinSizeM2((fMaxM2 - fMinM2) / (Double_t)fNbinsM2),
    fNbinsMom(9),
    fMinMom(1.),
    fMaxMom(10.),
    fNbinsMomCharge(TMath::Nint( 2*(fNbinsMom+fMinMom/fBinSizeMom) )),
    fBinSizeMom((fMaxMom - fMinMom) / (Double_t)fNbinsMom)
{
    CreateHistogramms();
    cout << "Nbins momentum*charge  :  " << fNbinsMomCharge << endl;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmFillM2::CbmFillM2(const char *name, Int_t verbose,
		     Int_t nbinsM2,
		     Double_t minM2, Double_t maxM2,
		     Int_t nbinsMom,
		     Double_t minMom, Double_t maxMom)
  : FairTask(name, verbose),
    fRnd(new TRandom(1)),
    fEvents(0),
    fCoutBunch(10),
    fTimeResolution(0.08),
    fLevel(1),
    fNoTrdStations(0),
    fNoTrdPerStation(0),
    fArrayMCTrack(NULL),
    fArrayTofPoint(NULL),
    fArrayHadron(NULL),
    fh_m2mom_hadron_all(NULL),
    fh_m2mom_hadron(NULL),
    fh_m2mom_hadron_true(NULL),
    fh_m2mom_hadron_10m(NULL), 
    fNbinsM2(nbinsM2),
    fMinM2(minM2),
    fMaxM2(maxM2),
    fBinSizeM2((fMaxM2 - fMinM2) / (Double_t)fNbinsM2),
    fNbinsMom(nbinsMom),
    fMinMom(minMom),
    fMaxMom(maxMom),
    fNbinsMomCharge(TMath::Nint( 2*(fNbinsMom+fMinMom/fBinSizeMom) )),
    fBinSizeMom((fMaxMom - fMinMom) / (Double_t)fNbinsMom)

{
    CreateHistogramms();
    cout << "Nbins momentum*charge  :  " << fNbinsMomCharge << endl;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmFillM2::~CbmFillM2()
{
    // Destructor
    delete fRnd;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
InitStatus CbmFillM2::Init()
{
    // Task initialisation

    // Get pointers to root manager and application
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmFillM2::Init : "
	    << "root manager is not instantiated." << endl;
        return kERROR;
    }

    if(1 == fLevel) {
	fArrayMCTrack = (TClonesArray*) rootMgr->GetObject("MCTrack");
	if(NULL == fArrayMCTrack) {
	    cout << "-W- CbmFillM2::Init : "
		<< "no MC track array!" << endl;
	}
	fArrayTofPoint = (TClonesArray*) rootMgr->GetObject("TofPoint");
	if(NULL == fArrayTofPoint) {
	    cout << "-W- CbmFillM2::Init : "
		<< "no TOF point array!" << endl;
	}
    }
    else {
	fArrayHadron = (TClonesArray*) rootMgr->GetObject("Hadron");
	if(NULL == fArrayHadron) {
	    cout << "-W- CbmFillM2::Init : "
		<< "no Hadron array!" << endl;
	}
    }

    if(1 == fLevel) {
	// Get the pointer to the singleton CbmRunAna object
	FairRunAna* ana = FairRunAna::Instance();
	if(NULL == ana) {
	    cout << "-E- CbmFillM2::Init :"
		<<" no CbmRunAna object!" << endl;
	    return kFATAL;
	}

	// Get the pointer to run-time data base
	FairRuntimeDb* rtdb = ana->GetRuntimeDb();
	if(NULL == rtdb) {
	    cout << "-E- CbmFillM2::Init :"
		<<" no runtime database!" << endl;
	    return kFATAL;
	}

	// Get the pointer to container of base parameters
	FairBaseParSet* baseParSet =
	    (FairBaseParSet*) rtdb->getContainer("FairBaseParSet");
	if(NULL == baseParSet) {
	    cout << "-E- CbmFillM2::Init :"
		<<" no container of base parameters!" << endl;
	    return kFATAL;
	}

	// Get the pointer to detector list
	TObjArray *detList = baseParSet->GetDetList();
	if(NULL == detList) {
	    cout << "-E- CbmFillM2::Init :"
		<< " no detector list!" << endl;
	    return kFATAL;
	}

	// Find TRD detector
	FairDetector* trd = (FairDetector*) detList->FindObject("TRD");
	if(NULL == trd) {
	    cout << "-E- CbmFillM2::Init :"
		<< " no TRD detector!" << endl;
	    return kSUCCESS;
	}

	// Determine the geometry version
	TString name = trd->GetGeometryFileName();
	if(name.Contains("9")) {
	    cout << "-I- CbmFillM2::Init :"
		<< " TRD geometry : 3x3." << endl;
	    fNoTrdStations = 3;
	    fNoTrdPerStation = 3;
	} else if(name.Contains("12")) {
	    cout << "-I- CbmFillM2::Init :"
		<< " TRD geometry : 3x4." << endl;
	    fNoTrdStations = 3;
	    fNoTrdPerStation = 4;
	} else if(name.Contains("6x2")) {
	    cout << "-I- CbmFillM2::Init :"
		<< " TRD geometry : 6x2." << endl;
	    fNoTrdStations = 6;
	    fNoTrdPerStation = 2;
	} else if(name.Contains("new_all")) {
	    cout << "-I- CbmFillM2::Init :"
		<< " TRD geometry : 3x3 (New)." << endl;
	    fNoTrdStations = 3;
	    fNoTrdPerStation = 3;
	} else if(name.Contains("standard")) {
	    cout << "-I- CbmFillM2::Init :"
		<< " TRD geometry : 3x4." << endl;
	    fNoTrdStations = 3;
	    fNoTrdPerStation = 4;
	} else {
	    Fatal("Init", "Unknown TRD geometry!");
	}
    }

    return kSUCCESS;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmFillM2::Exec(Option_t *option)
{
    // Task execution. Fill the m2 vs. momentum histogramm

    Int_t charge = 0;
    Double_t mom = 0.;
    Double_t m2 = 0.;

    if(1 == fLevel) {
	// Declare variables outside the loop
	CbmMCTrack* mcTrack = NULL;
	CbmTofPoint* tofPoint = NULL;
	Int_t mcTrackIndex = 0;
	Int_t pdgCode = 0;
	Double_t time = 0;

	// Process TOF points ----------------------------------
	if(fArrayTofPoint && fArrayMCTrack) {
            Int_t temp = -5;
	    for(Int_t iTofPoint = 0;
		iTofPoint < fArrayTofPoint->GetEntriesFast();
		iTofPoint++) {
		tofPoint = (CbmTofPoint*) fArrayTofPoint->At(iTofPoint);
		if(NULL == tofPoint) continue;

		// Get pointer to the corresponding MC track
		mcTrackIndex = tofPoint->GetTrackID();
		if(mcTrackIndex < 0) continue;
		if(mcTrackIndex == temp) continue;
                temp = mcTrackIndex;
		mcTrack = (CbmMCTrack*) fArrayMCTrack->At(mcTrackIndex);
		if(NULL == mcTrack) continue;

		// Skip secondaries
		if(-1 != mcTrack->GetMotherId()) continue;

		// Process only hadrons
		pdgCode = mcTrack->GetPdgCode();
		if( (211!=TMath::Abs(pdgCode)) &&
		   (321!=TMath::Abs(pdgCode)) &&
		   (2212!=TMath::Abs(pdgCode)) ) continue;

		// Check if accepted in the STS and TRD
		if(mcTrack->GetNPoints(kSTS) < 4) continue;
		if(mcTrack->GetNPoints(kTRD) <
		   (fNoTrdStations*fNoTrdPerStation)) continue;

		// Calculate squared mass from smeared tof
		// and fill histogramms
		charge = pdgCode / TMath::Abs(pdgCode);
		mom = mcTrack->GetP();
		time = tofPoint->GetTime();
                time = fRnd->Gaus(time, fTimeResolution);
		m2 = TMath::Power(mom, 2) *
		    (  TMath::Power( (time*29.9792458)/tofPoint->GetLength(), 2 ) - 1  );

		// Fill tof spectrum
		fh_m2mom_hadron->Fill(mom * charge, m2);
	    }// Loop over TOF hits
	}
	// -----------------------------------------------------
    } else {
	// Process global tracks -------------------------------
	CbmHadron *hadron  = NULL;
	if(fArrayHadron) {
	    for(Int_t iHadron = 0;
		iHadron < fArrayHadron->GetEntriesFast();
		iHadron++) {
		// Get pointer to the global track
		hadron = (CbmHadron*) fArrayHadron->At(iHadron);
		if(NULL == hadron) continue;

		// Calculate squared mass and fill histogramms
		charge = hadron->GetCharge();
		mom = hadron->GetP();
		m2 = hadron->GetMass2();

		// Fill tof spectrum for all hadrons
		fh_m2mom_hadron_all->Fill(mom * charge, m2);

		// Select primaries
                if(hadron->GetB() > 3) continue;

		// Fill tof spectrum for prim. hadrons
		fh_m2mom_hadron->Fill(mom * charge, m2);
		if(kFALSE == hadron->IsGhost() &&
		   kFALSE == hadron->IsTDH()) {
		    fh_m2mom_hadron_true->Fill(mom * charge, m2);
		}
		if(hadron->GetLength() <= 1200 &&
		   hadron->GetLength() >= 990) {
		  fh_m2mom_hadron_10m->Fill(mom * charge, m2);
		}
	    }// Loop over global tracks
	}
	// ----------------------------------------------------
    }

    Reset();

    if(0 == (fEvents % fCoutBunch)) {
	cout << "-I- CbmFillM2::Exec : event number "
	    << fEvents << " processed" << endl;
    }
    fEvents += 1;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmFillM2::Finish()
{
    // Finish the task execution
//    WriteHistogramms();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmFillM2::CreateHistogramms()
{
    // Create histogramms

    // Squared mass vs. momentum distributions
    fh_m2mom_hadron_all  = new TH2F("h_m2mom_hadron_all",
				    "Squared Mass vs. mom. of all hadrons",
				    fNbinsMomCharge, -fMaxMom, fMaxMom,
				    fNbinsM2, fMinM2, fMaxM2);

    // Squared mass vs. momentum distributions
    fh_m2mom_hadron  = new TH2F("h_m2mom_hadron",
				"Squared Mass vs. mom. of prim. hadrons",
				fNbinsMomCharge, -fMaxMom, fMaxMom,
				fNbinsM2, fMinM2, fMaxM2);

    // Squared mass vs. momentum distributions for true tracks
    fh_m2mom_hadron_true  = new TH2F("h_m2mom_hadron_true",
				     "Squared Mass vs. mom. of true hadrons",
				     fNbinsMomCharge, -fMaxMom, fMaxMom,
				     fNbinsM2, fMinM2, fMaxM2);

    // Squared mass vs. momentum distributions for 10m tracks
    fh_m2mom_hadron_10m  = new TH2F("h_m2mom_hadron_10m",
				    "Squared Mass vs. mom. of 10m tracks",
				    fNbinsMomCharge, -fMaxMom, fMaxMom,
				    fNbinsM2, fMinM2, fMaxM2);
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmFillM2::WriteHistogramms()
{
    // Write histogramms
    const Double_t cM2Mom = 1./fBinSizeM2 * 1./fBinSizeMom;
    fh_m2mom_hadron_all->Scale(cM2Mom);
    fh_m2mom_hadron->Scale(cM2Mom);
    fh_m2mom_hadron_true->Scale(cM2Mom);
    fh_m2mom_hadron_10m->Scale(cM2Mom);

    fh_m2mom_hadron_all->Write();
    fh_m2mom_hadron->Write();
    fh_m2mom_hadron_true->Write();
    fh_m2mom_hadron_10m->Write();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmFillM2::Reset()
{
    if(1 == fLevel) {
	fArrayMCTrack->Delete();
	fArrayTofPoint->Delete();
    } else {
	fArrayHadron->Delete();
    }
}
// ------------------------------------------------------------------



ClassImp(CbmFillM2);


