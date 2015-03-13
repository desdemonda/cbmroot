// ------------------------------------------------------------------
// -----                      CbmEbefluctPYR                    -----
// -----             Created 2004-Sep-23 by D.Kresan            -----
// ------------------------------------------------------------------
#include <iostream>
using namespace std;

#include "TFile.h"
#include "TDirectory.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TRandom.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "CbmTofPoint.h"
#include "CbmHadron.h"
#include "CbmEbefluctPYR.h"


//___________________________________________________________________
//
// CbmEbefluctPYR
//
// Task for analysis of the event-by-event particle yield ratio
//


// ------------------------------------------------------------------
CbmEbefluctPYR::CbmEbefluctPYR()
  : FairTask("ebeFluctPYR"),
    fLevel(0),
    fEvents(0),
    fCoutBunch(1),
    fNbinsRatio(100),
    fArrayMCTrack(NULL),
    fArrayTofPoint(NULL),
    fArrayHadron(NULL),
    fh_all_mom_kap(NULL),
    fh_acc_mom_kap(NULL),
    fh_pid_mom_kap(NULL),
    fh_rpid_mom_kap(NULL),
    fh_all_nka(NULL),
    fh_acc_nka(NULL),
    fh_pid_nka(NULL),
    fh_rpid_nka(NULL),
    fh_rpid_nka_true(NULL),
    fn_ka_all(0.),
    fn_ka_true(0.),
    fh_all_ka2pi(NULL),
    fh_acc_ka2pi(NULL),
    fh_pid_ka2pi(NULL),
    fh_rpid_ka2pi(NULL),
    fh_rpid_ka2pi_true(NULL),
    fh_rpid_ka2pi_true_10(NULL),
    fh_rpid_ka2pi_true_9(NULL),
    fh_rpid_ka2pi_true_8(NULL),
    fh_rpid_ka2pi_true_7(NULL),
    fh_rpid_ka2pi_true_6(NULL),
    fh_rpid_ka2pi_true_5(NULL),
    fh_rpid_ka2pi_true_4(NULL),
    fh_rpid_ka2pi_true_3(NULL),
    fh_rpid_ka2pi_true_2(NULL),
    fh_rpid_ka2pi_true_475(NULL),
    fh_rpid_ka2pi_true_45(NULL),
    fh_rpid_ka2pi_true_35(NULL),
    fh_rpid_ka2pi_true_34(NULL),
    fh_rpid_ka2pi_true_325(NULL),
    fh_rpid_ka2pi_true_22(NULL),
    fh_all_p2pi(NULL),
    fh_acc_p2pi(NULL),
    fh_rpid_p2pi(NULL),
    fh_rpid_p2pi_true(NULL),
    fh_all_kap2pip(NULL),
    fh_acc_kap2pip(NULL),
    fh_rpid_kap2pip(NULL),
    fh_rpid_kap2pip_true(NULL),
    fh_all_kam2pip(NULL),
    fh_acc_kam2pip(NULL),
    fh_rpid_kam2pip(NULL),
    fh_rpid_kam2pip_true(NULL),
    fh_all_kap2pim(NULL),
    fh_acc_kap2pim(NULL),
    fh_rpid_kap2pim(NULL),
    fh_rpid_kap2pim_true(NULL),
    fh_all_kam2pim(NULL),
    fh_acc_kam2pim(NULL),
    fh_rpid_kam2pim(NULL),
    fh_rpid_kam2pim_true(NULL),
    fh_all_ka2p(NULL),
    fh_acc_ka2p(NULL),
    fh_rpid_ka2p(NULL),
    fh_rpid_ka2p_true(NULL),
    fPdfFileName(""),
    fPdfFile(NULL),
    fh_m2min_pi(NULL),
    fh_m2max_pi(NULL),
    fh_m2min_ka(NULL),
    fh_m2max_ka(NULL),
    fh_m2min_prot(NULL),
    fh_m2max_prot(NULL),
    fh_pur_pi(NULL),
    fh_pur_ka(NULL),
    fh_pur_prot(NULL),
    fh_pdf_pi(NULL),
    fh_pdf_ka(NULL),
    fh_pdf_protaprot(NULL),
    fh_pdf_pip(NULL),
    fh_pdf_kap(NULL),
    fh_pdf_prot(NULL),
    fPdfNbinsM2(900),
    fPdfMinM2(-1.5),
    fPdfMaxM2(3.0),
    fPdfBinSizeM2( (fPdfMaxM2 - fPdfMinM2) / (Double_t)fPdfNbinsM2 ),
    fPdfNbinsMom(90),
    fPdfMinMom(1.),
    fPdfMaxMom(10.),
    fPdfBinSizeMom( (fPdfMaxMom - fPdfMinMom) / (Double_t)fPdfNbinsMom ),
    fPdfNbinsMomCharge(200)
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmEbefluctPYR::CbmEbefluctPYR(const char* name,
                               Int_t verbose,
			       Int_t nBinsRatio)
  : FairTask(name, verbose),
    fLevel(0),
    fEvents(0),
    fCoutBunch(1),
    fNbinsRatio(nBinsRatio),
    fArrayMCTrack(NULL),
    fArrayTofPoint(NULL),
    fArrayHadron(NULL),
    fh_all_mom_kap(NULL),
    fh_acc_mom_kap(NULL),
    fh_pid_mom_kap(NULL),
    fh_rpid_mom_kap(NULL),
    fh_all_nka(NULL),
    fh_acc_nka(NULL),
    fh_pid_nka(NULL),
    fh_rpid_nka(NULL),
    fh_rpid_nka_true(NULL),
    fn_ka_all(0.),
    fn_ka_true(0.),
    fh_all_ka2pi(NULL),
    fh_acc_ka2pi(NULL),
    fh_pid_ka2pi(NULL),
    fh_rpid_ka2pi(NULL),
    fh_rpid_ka2pi_true(NULL),
    fh_rpid_ka2pi_true_10(NULL),
    fh_rpid_ka2pi_true_9(NULL),
    fh_rpid_ka2pi_true_8(NULL),
    fh_rpid_ka2pi_true_7(NULL),
    fh_rpid_ka2pi_true_6(NULL),
    fh_rpid_ka2pi_true_5(NULL),
    fh_rpid_ka2pi_true_4(NULL),
    fh_rpid_ka2pi_true_3(NULL),
    fh_rpid_ka2pi_true_2(NULL),
    fh_rpid_ka2pi_true_475(NULL),
    fh_rpid_ka2pi_true_45(NULL),
    fh_rpid_ka2pi_true_35(NULL),
    fh_rpid_ka2pi_true_34(NULL),
    fh_rpid_ka2pi_true_325(NULL),
    fh_rpid_ka2pi_true_22(NULL),
    fh_all_p2pi(NULL),
    fh_acc_p2pi(NULL),
    fh_rpid_p2pi(NULL),
    fh_rpid_p2pi_true(NULL),
    fh_all_kap2pip(NULL),
    fh_acc_kap2pip(NULL),
    fh_rpid_kap2pip(NULL),
    fh_rpid_kap2pip_true(NULL),
    fh_all_kam2pip(NULL),
    fh_acc_kam2pip(NULL),
    fh_rpid_kam2pip(NULL),
    fh_rpid_kam2pip_true(NULL),
    fh_all_kap2pim(NULL),
    fh_acc_kap2pim(NULL),
    fh_rpid_kap2pim(NULL),
    fh_rpid_kap2pim_true(NULL),
    fh_all_kam2pim(NULL),
    fh_acc_kam2pim(NULL),
    fh_rpid_kam2pim(NULL),
    fh_rpid_kam2pim_true(NULL),
    fh_all_ka2p(NULL),
    fh_acc_ka2p(NULL),
    fh_rpid_ka2p(NULL),
    fh_rpid_ka2p_true(NULL),
    fPdfFileName(""),
    fPdfFile(NULL),
    fh_m2min_pi(NULL),
    fh_m2max_pi(NULL),
    fh_m2min_ka(NULL),
    fh_m2max_ka(NULL),
    fh_m2min_prot(NULL),
    fh_m2max_prot(NULL),
    fh_pur_pi(NULL),
    fh_pur_ka(NULL),
    fh_pur_prot(NULL),
    fh_pdf_pi(NULL),
    fh_pdf_ka(NULL),
    fh_pdf_protaprot(NULL),
    fh_pdf_pip(NULL),
    fh_pdf_kap(NULL),
    fh_pdf_prot(NULL),
    fPdfNbinsM2(900),
    fPdfMinM2(-1.5),
    fPdfMaxM2(3.0),
    fPdfBinSizeM2( (fPdfMaxM2 - fPdfMinM2) / (Double_t)fPdfNbinsM2 ),
    fPdfNbinsMom(90),
    fPdfMinMom(1.),
    fPdfMaxMom(10.),
    fPdfBinSizeMom( (fPdfMaxMom - fPdfMinMom) / (Double_t)fPdfNbinsMom ),
    fPdfNbinsMomCharge(200)
{
    // Standard constructor
    fn_ka_all          = 0;
    fn_ka_true         = 0;
    fEvents            = 0;
    fCoutBunch         = 1;
    fPdfNbinsMom       = 90;
    fPdfMinMom         = 1;
    fPdfMaxMom         = 10;
    fPdfBinSizeMom     = (fPdfMaxMom - fPdfMinMom) / (Double_t)fPdfNbinsMom;
    fPdfNbinsMomCharge = 200;
    fPdfNbinsM2        = 900;
    fPdfMinM2          = -1.5;
    fPdfMaxM2          = 3.0;
    fPdfBinSizeM2      = (fPdfMaxM2 - fPdfMinM2) / (Double_t)fPdfNbinsM2;
    fNbinsRatio        = nBinsRatio;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmEbefluctPYR::~CbmEbefluctPYR()
{
    // Destructor
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
InitStatus CbmEbefluctPYR::Init()
{
    // Task initialisation
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmEbefluctPYR::Init : "
	    << "ROOT manager is not instantiated!" << endl;
        return kERROR;
    }

    if(1 == fLevel || 2 == fLevel) {
	fArrayMCTrack = (TClonesArray*) rootMgr->GetObject("MCTrack");
	if(NULL == fArrayMCTrack) {
	    cout << "-E- CbmEbefluctPYR::Init : "
		<< "no MC track array!" << endl;
	}
	fArrayTofPoint = (TClonesArray*) rootMgr->GetObject("TOFPoint");
	if(NULL == fArrayTofPoint) {
	    cout << "-E- CbmEbefluctPYR::Init : "
		<< "no TOF point array!" << endl;
	}
    } else {
	fArrayHadron = (TClonesArray*) rootMgr->GetObject("Hadron");
	if(NULL == fArrayHadron) {
	    cout << "-E- CbmEbefluctPYR::Init : "
		<< "no Hadron array!" << endl;
	}
    }

    OpenPdfFile();

    CreateHistogramms();

    cout << "-I- CbmEbefluctPYR : "
	<< "initialization completed." << endl;

    return kSUCCESS;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmEbefluctPYR::Exec(Option_t*)
{
    // Task execution. Measure yield ratio in the event and fill
    // histogramms

    if(1 == fLevel || 2 == fLevel) {
        if(NULL == fArrayMCTrack || NULL == fArrayTofPoint) return;

	Double_t n_all_pip = 0.;
	Double_t n_all_kap = 0.;
        Double_t n_all_prot = 0.;
	Double_t n_all_pim = 0.;
	Double_t n_all_kam = 0.;
        Double_t n_all_aprot = 0.;

	Double_t n_acc_pip = 0.;
	Double_t n_acc_kap = 0.;
        Double_t n_acc_prot = 0.;
	Double_t n_acc_pim = 0.;
	Double_t n_acc_kam = 0.;
        Double_t n_acc_aprot = 0.;

	Double_t n_pid_pip = 0;
	Double_t n_pid_kap = 0;
        Double_t n_pid_prot = 0.;
	Double_t n_pid_pim = 0;
        Double_t n_pid_kam = 0;
        Double_t n_pid_aprot = 0.;

	CbmMCTrack *mcTrack;
	Int_t pdgCode;
        Double_t mom;
	CbmTofPoint *tofPoint;
	Int_t trackID;

	Int_t charge;
        Double_t tof;
	Double_t m2;
	Double_t mom_charge;
        Int_t index;
	Double_t m2_min_pi;
	Double_t m2_max_pi;
	Double_t m2_min_ka;
	Double_t m2_max_ka;
	Double_t m2_min_prot;
	Double_t m2_max_prot;
        Int_t pdg;

	for(Int_t itrack = 0;
	    itrack < fArrayMCTrack->GetEntriesFast();
	    itrack++ ) {
	    // Get pointer to the track
	    mcTrack = (CbmMCTrack*) fArrayMCTrack->At(itrack);
	    if(NULL == mcTrack) continue;
	    // Skip secondaries
	    if(-1 != mcTrack->GetMotherId()) continue;
	    // Process only hadrons
	    pdgCode = mcTrack->GetPdgCode();
            mom = mcTrack->GetP();
	    switch(pdgCode) {
	    case 211 : {
		n_all_pip += 1;
		break;
	    };
	    case 321 : {
		n_all_kap += 1;
                fh_all_mom_kap->Fill(mom);
		break;
	    };
	    case 2212 : {
		n_all_prot += 1;
                break;
	    };
	    case -211 : {
		n_all_pim += 1;
		break;
	    };
	    case -321 : {
		n_all_kam += 1;
		break;
	    };
	    case -2212 : {
		n_all_aprot += 1;
                break;
	    };
	    };
	} // Loop over tracks

        fh_all_nka->Fill((n_all_kap+n_all_kam));

	fh_all_ka2pi->Fill( (n_all_kap+n_all_kam) / (n_all_pip+n_all_pim) );
	fh_all_p2pi->Fill( (n_all_prot+n_all_aprot) / (n_all_pip+n_all_pim) );
	fh_all_kap2pip->Fill( n_all_kap / n_all_pip );
	fh_all_kam2pip->Fill( n_all_kam / n_all_pip );
	fh_all_kap2pim->Fill( n_all_kap / n_all_pim );
	fh_all_kam2pim->Fill( n_all_kam / n_all_pim );
	fh_all_ka2p->Fill( (n_all_kap+n_all_kam) / (n_all_prot+n_all_aprot) );

	Int_t temp = -3;
	for(Int_t ipoint = 0;
	    ipoint < fArrayTofPoint->GetEntriesFast();
	    ipoint++) {
	    // Get pointer to the curent point
	    tofPoint = (CbmTofPoint*) fArrayTofPoint->At(ipoint);
	    if(NULL == tofPoint) continue;
	    // Get track id
	    trackID = tofPoint->GetTrackID();
	    if(trackID < 0) continue;
	    if(trackID == temp) continue;
	    temp = trackID;
	    // Get pointer to the particle
	    mcTrack = (CbmMCTrack*) fArrayMCTrack->At(trackID);
	    if(NULL == mcTrack) continue;
	    // Skip secondaries
	    if(-1 != mcTrack->GetMotherId()) continue;
	    // Process only hadrons
	    pdgCode = mcTrack->GetPdgCode();
            mom = mcTrack->GetP();
	    // Fill histogramms for accepted particles
	    switch(pdgCode) {
	    case 211 : {
		n_acc_pip += 1;
		break;
	    };
	    case 321 : {
		n_acc_kap += 1;
		fh_acc_mom_kap->Fill(mom);
		break;
	    };
	    case 2212 : {
		n_acc_prot += 1;
		break;
	    };
	    case -211 : {
		n_acc_pim += 1;
		break;
	    };
	    case -321 : {
		n_acc_kam += 1;
		break;
	    };
	    case -2212 : {
		n_acc_aprot += 1;
		break;
	    };
	    };

	    charge = pdgCode / TMath::Abs(pdgCode);
	    tof = gRandom->Gaus(tofPoint->GetTime(), 0.08);
	    Double_t val = (tof*29.9792458)/tofPoint->GetLength();
	    m2 = mom*mom*(val*val-1);
	    if(mom > 5) {
		if(m2 < 0.4) {
                    pdg = (Int_t)charge*211;
		} else {
                    pdg = (Int_t)charge*2212;
		}
	    } else {
		// Get window for PID
		mom_charge = mom * charge;
		index = TMath::Nint((mom_charge+fPdfMaxMom)/fPdfBinSizeMom);
                m2_min_pi = fh_m2min_pi->GetBinContent(index+1);
		m2_max_pi = fh_m2max_pi->GetBinContent(index+1);
                m2_min_ka = fh_m2min_ka->GetBinContent(index+1);
		m2_max_ka = fh_m2max_ka->GetBinContent(index+1);
                m2_min_prot = fh_m2min_prot->GetBinContent(index+1);
		m2_max_prot = fh_m2max_prot->GetBinContent(index+1);

		// Make PID
		if(m2 < m2_max_ka && m2 > m2_min_ka) {
		    pdg = (Int_t)charge*321;
		} else if(m2 < m2_max_pi && m2 > m2_min_pi) {
		    pdg = (Int_t)charge*211;
		} else if(m2 < m2_max_prot && m2 > m2_min_prot) {
                    pdg = (Int_t)charge*2212;
		} else {
                    pdg = 0;
		}
	    }

            // Increase counters
	    switch(pdg) {
	    case 211 : {
		n_pid_pip += 1;
		break;
	    };
	    case 321 : {
		n_pid_kap += 1;
                fh_pid_mom_kap->Fill(mom);
		break;
	    };
	    case 2212 : {
		n_pid_prot += 1;
		break;
	    };
	    case -211 : {
		n_pid_pim += 1;
		break;
	    };
	    case -321 : {
		n_pid_kam += 1;
		break;
	    };
	    case -2212 : {
		n_pid_aprot += 1;
		break;
	    };
	    };
	} // loop over Tof points

        fh_acc_nka->Fill((n_acc_kap+n_acc_kam));

	fh_acc_ka2pi->Fill( (n_acc_kap+n_acc_kam) / (n_acc_pip+n_acc_pim) );
	fh_acc_p2pi->Fill( (n_acc_prot+n_acc_aprot) / (n_acc_pip+n_acc_pim) );
	fh_acc_kap2pip->Fill( n_acc_kap / n_acc_pip );
	fh_acc_kam2pip->Fill( n_acc_kam / n_acc_pip );
	fh_acc_kap2pim->Fill( n_acc_kap / n_acc_pim );
	fh_acc_kam2pim->Fill( n_acc_kam / n_acc_pim );
	fh_acc_ka2p->Fill( (n_acc_kap+n_acc_kam) / (n_acc_prot+n_acc_aprot) );

	fh_pid_nka->Fill((n_pid_kap+n_pid_kam));

	fh_pid_ka2pi->Fill( (n_pid_kap+n_pid_kam) / (n_pid_pip+n_pid_pim) );
    } else {
	if(NULL == fArrayHadron) return;

	Double_t n_rpid_pip = 0;
	Double_t n_rpid_kap = 0;
	Double_t n_rpid_prot = 0;
	Double_t n_rpid_pim = 0;
        Double_t n_rpid_kam = 0;
	Double_t n_rpid_aprot = 0;
	Double_t n_rpid_true_pip = 0;
	Double_t n_rpid_true_kap = 0;
	Double_t n_rpid_true_prot = 0;
	Double_t n_rpid_true_pim = 0;
        Double_t n_rpid_true_kam = 0;
	Double_t n_rpid_true_aprot = 0;

	Double_t n_rpid_true_ka_10 = 0;
	Double_t n_rpid_true_ka_9 = 0;
	Double_t n_rpid_true_ka_8 = 0;
	Double_t n_rpid_true_ka_7 = 0;
	Double_t n_rpid_true_ka_6 = 0;
	Double_t n_rpid_true_ka_5 = 0;
	Double_t n_rpid_true_ka_4 = 0;
	Double_t n_rpid_true_ka_3 = 0;
	Double_t n_rpid_true_ka_2 = 0;
	Double_t n_rpid_true_ka_475 = 0;
	Double_t n_rpid_true_ka_45 = 0;
	Double_t n_rpid_true_ka_35 = 0;
	Double_t n_rpid_true_ka_34 = 0;
	Double_t n_rpid_true_ka_325 = 0;
	Double_t n_rpid_true_ka_22 = 0;

	CbmHadron *hadron;
	Int_t charge;
	Double_t mom;
	Double_t m2;
	Double_t mom_charge;
        Int_t index;
	Double_t m2_min_pi;
	Double_t m2_max_pi;
	Double_t m2_min_ka;
	Double_t m2_max_ka;
	Double_t m2_min_prot;
	Double_t m2_max_prot;
        Int_t pdg;

        // Loop over hadrons
	for(Int_t iHadron = 0;
	    iHadron < fArrayHadron->GetEntriesFast();
	    iHadron++) {
	    // Get pointer to the hadron
	    hadron = (CbmHadron*) fArrayHadron->At(iHadron);
	    if(NULL == hadron) continue;

	    // Select primaries
	    if(hadron->GetB() > 3) continue;

	    // Get charge, mom and m2
	    charge = hadron->GetCharge();
	    mom = hadron->GetP();
	    m2 = hadron->GetMass2();
	    if(mom > 5) {
		if(m2 < 0.4) {
		    pdg = (Int_t)charge*211;
		} else {
		    pdg = (Int_t)charge*2212;
		}
	    } else {
		// Get window for PID
		mom_charge = mom * charge;
		index = TMath::Nint((mom_charge+fPdfMaxMom)/fPdfBinSizeMom);
                m2_min_pi = fh_m2min_pi->GetBinContent(index+1);
		m2_max_pi = fh_m2max_pi->GetBinContent(index+1);
                m2_min_ka = fh_m2min_ka->GetBinContent(index+1);
		m2_max_ka = fh_m2max_ka->GetBinContent(index+1);
                m2_min_prot = fh_m2min_prot->GetBinContent(index+1);
		m2_max_prot = fh_m2max_prot->GetBinContent(index+1);

		// Make PID
		if(m2 < m2_max_ka && m2 > m2_min_ka) {
		    pdg = (Int_t)charge*321;
		} else if(m2 < m2_max_pi && m2 > m2_min_pi) {
		    pdg = (Int_t)charge*211;
		} else if(m2 < m2_max_prot && m2 > m2_min_prot) {
                    pdg = (Int_t)charge*2212;
		} else {
                    pdg = 0;
		}
	    }

	    // Calculate purity
	    if(321 == TMath::Abs(pdg)) {
                fn_ka_all += 1;
		if(321 == TMath::Abs(hadron->GetPdg())) {
                    fn_ka_true += 1;
		}
	    }

	    // Increase counters
	    switch(pdg) {
	    case 211 : {
		n_rpid_pip += 1;
		break;
	    };
	    case 321 : {
		n_rpid_kap += 1;
                fh_rpid_mom_kap->Fill(mom);
		break;
	    };
	    case 2212 : {
		n_rpid_prot += 1;
		break;
	    };
	    case -211 : {
		n_rpid_pim += 1;
		break;
	    };
	    case -321 : {
		n_rpid_kam += 1;
		break;
	    };
	    case -2212 : {
		n_rpid_aprot += 1;
		break;
	    };
	    };

	    // Use true pdg
	    pdg = hadron->GetPdg();

	    // Increase counters
	    switch(pdg) {
	    case 211 : {
		n_rpid_true_pip += 1;
		break;
	    };
	    case 321 : {
		n_rpid_true_kap += 1;
		break;
	    };
	    case 2212 : {
		n_rpid_true_prot += 1;
		break;
	    };
	    case -211 : {
		n_rpid_true_pim += 1;
		break;
	    };
	    case -321 : {
		n_rpid_true_kam += 1;
		break;
	    };
	    case -2212 : {
		n_rpid_true_aprot += 1;
		break;
	    };
	    };

	    if(321 != TMath::Abs(pdg)) continue;
	    if(mom > 10) continue;
	    n_rpid_true_ka_10 += 1;
            if(mom > 9) continue;
	    n_rpid_true_ka_9 += 1;
            if(mom > 8) continue;
	    n_rpid_true_ka_8 += 1;
            if(mom > 7) continue;
	    n_rpid_true_ka_7 += 1;
            if(mom > 6) continue;
	    n_rpid_true_ka_6 += 1;
	    if(mom > 5) continue;
	    n_rpid_true_ka_5 += 1;
	    if(mom > 4.75) continue;
            n_rpid_true_ka_475 += 1;
	    if(mom > 4.5) continue;
            n_rpid_true_ka_45 += 1;
            if(mom > 4) continue;
	    n_rpid_true_ka_4 += 1;
	    if(mom > 3.5) continue;
            n_rpid_true_ka_35 += 1;
	    if(mom > 3.4) continue;
            n_rpid_true_ka_34 += 1;
	    if(mom > 3.25) continue;
            n_rpid_true_ka_325 += 1;
            if(mom > 3) continue;
	    n_rpid_true_ka_3 += 1;
            if(mom > 2.2) continue;
	    n_rpid_true_ka_22 += 1;
            if(mom > 2) continue;
	    n_rpid_true_ka_2 += 1;
	}

        fh_rpid_nka->Fill((n_rpid_kap+n_rpid_kam));

	fh_rpid_ka2pi->Fill( (n_rpid_kap+n_rpid_kam) / (n_rpid_pip+n_rpid_pim) );
	fh_rpid_p2pi->Fill( (n_rpid_prot+n_rpid_aprot) / (n_rpid_pip+n_rpid_pim) );
	fh_rpid_kap2pip->Fill( n_rpid_kap / n_rpid_pip );
	fh_rpid_kam2pip->Fill( n_rpid_kam / n_rpid_pip );
	fh_rpid_kap2pim->Fill( n_rpid_kap / n_rpid_pim );
	fh_rpid_kam2pim->Fill( n_rpid_kam / n_rpid_pim );
	fh_rpid_ka2p->Fill( (n_rpid_kap+n_rpid_kam) / (n_rpid_prot+n_rpid_aprot) );

	fh_rpid_nka_true->Fill((n_rpid_true_kap+n_rpid_true_kam));

	fh_rpid_ka2pi_true->Fill( (n_rpid_true_kap+n_rpid_true_kam) /
				 (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_p2pi_true->Fill( (n_rpid_true_prot+n_rpid_true_aprot) /
				(n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_kap2pip_true->Fill( n_rpid_true_kap / n_rpid_true_pip );
	fh_rpid_kam2pip_true->Fill( n_rpid_true_kam / n_rpid_true_pip );
	fh_rpid_kap2pim_true->Fill( n_rpid_true_kap / n_rpid_true_pim );
	fh_rpid_kam2pim_true->Fill( n_rpid_true_kam / n_rpid_true_pim );
	fh_rpid_ka2p_true->Fill( (n_rpid_true_kap+n_rpid_true_kam) /
				(n_rpid_true_prot+n_rpid_true_aprot) );
	fh_rpid_ka2pi_true_10->Fill( n_rpid_true_ka_10 /
				    (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_9->Fill( n_rpid_true_ka_9 /
				   (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_8->Fill( n_rpid_true_ka_8 /
				   (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_7->Fill( n_rpid_true_ka_7 /
				   (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_6->Fill( n_rpid_true_ka_6 /
				   (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_5->Fill( n_rpid_true_ka_5 /
				   (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_4->Fill( n_rpid_true_ka_4 /
				   (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_3->Fill( n_rpid_true_ka_3 /
				   (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_2->Fill( n_rpid_true_ka_2 /
				   (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_475->Fill( n_rpid_true_ka_475 /
				     (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_45->Fill( n_rpid_true_ka_45 /
				    (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_35->Fill( n_rpid_true_ka_35 /
				    (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_34->Fill( n_rpid_true_ka_34 /
				    (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_325->Fill( n_rpid_true_ka_325 /
				     (n_rpid_true_pip+n_rpid_true_pim) );
	fh_rpid_ka2pi_true_22->Fill( n_rpid_true_ka_22 /
				    (n_rpid_true_pip+n_rpid_true_pim) );
    }


    if(0 == (fEvents%fCoutBunch)) {
	cout << "-I- CbmEbefluctPYR::Exec : "
            << "event " << fEvents << " is processed" << endl;
    }


    fEvents += 1;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmEbefluctPYR::Finish()
{
    // Finish of the task execution
    //WriteHistogramms();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmEbefluctPYR::DynamicalFluctuations(TH1F *h_data, TH1F *h_mix,
					   Double_t &s_data, Double_t &error_s_data,
					   Double_t &s_mix,  Double_t &error_s_mix,
					   Double_t &s_dyn,  Double_t &error_s_dyn)
{
    // Calculate dynamical fluctuations
    s_data = h_data->GetRMS() / h_data->GetMean();
    error_s_data = s_data / TMath::Sqrt( 2*h_data->GetEntries() );
    s_mix = h_mix->GetRMS() / h_mix->GetMean();
    error_s_mix = s_mix / TMath::Sqrt( 2*h_mix->GetEntries() );
    if(s_data > s_mix) {
	s_dyn = TMath::Sqrt( TMath::Power(s_data, 2) - TMath::Power(s_mix, 2) );
    } else {
	s_dyn = -TMath::Sqrt(-TMath::Power(s_data, 2) + TMath::Power(s_mix, 2) );
    }
    error_s_dyn = TMath::Sqrt(TMath::Power(s_data/s_dyn*error_s_data, 2) +
			      TMath::Power(s_mix/s_dyn*error_s_mix, 2));
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmEbefluctPYR::ExperimentalResolution(Double_t &s_true, Double_t &error_s_true,
					    Double_t &s_exp,  Double_t &error_s_exp,
					    Double_t &resol,  Double_t &error_resol)
{
    // Calculate experimental resolution contribution
    if(s_exp > s_true) {
	resol = TMath::Sqrt( TMath::Power(s_exp, 2) - TMath::Power(s_true, 2) );
    } else {
	resol = -TMath::Sqrt(-TMath::Power(s_exp, 2) + TMath::Power(s_true, 2) );
    }
    error_resol = TMath::Sqrt(TMath::Power(s_exp/resol*error_s_exp, 2) +
			      TMath::Power(s_true/resol*error_s_true, 2));
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmEbefluctPYR::OpenPdfFile()
{
    // Open PDF file and get necessary histogramms
    TDirectory *olddir = gDirectory;

    fPdfFile = new TFile( fPdfFileName );

    fh_m2min_pi = (TH1F*) fPdfFile->Get("h_m2min_pi");
    fh_m2max_pi = (TH1F*) fPdfFile->Get("h_m2max_pi");
    fh_m2min_ka = (TH1F*) fPdfFile->Get("h_m2min_ka");
    fh_m2max_ka = (TH1F*) fPdfFile->Get("h_m2max_ka");
    fh_m2min_prot = (TH1F*) fPdfFile->Get("h_m2min_prot");
    fh_m2max_prot = (TH1F*) fPdfFile->Get("h_m2max_prot");
    fh_pur_pi = (TH1F*) fPdfFile->Get("h_pur_pi");
    fh_pur_ka = (TH1F*) fPdfFile->Get("h_pur_ka");
    fh_pur_prot = (TH1F*) fPdfFile->Get("h_pur_prot");

    fh_pdf_pi = (TH2F*) fPdfFile->Get("h_pdf_pi");
    fh_pdf_ka = (TH2F*) fPdfFile->Get("h_pdf_ka");
    fh_pdf_protaprot = (TH2F*) fPdfFile->Get("h_pdf_protaprot");

    fh_pdf_pip = (TH2F*) fPdfFile->Get("h_pdf_pip");
    fh_pdf_kap = (TH2F*) fPdfFile->Get("h_pdf_kap");
    fh_pdf_prot = (TH2F*) fPdfFile->Get("h_pdf_prot");

    TH1F *h_sm2 = (TH1F*) fPdfFile->Get("h_sm2");

    olddir->cd();

    fPdfNbinsMom = h_sm2->GetNbinsX();
    fPdfMinMom = h_sm2->GetXaxis()->GetXmin();
    fPdfMaxMom = h_sm2->GetXaxis()->GetXmax();
    fPdfBinSizeMom = (fPdfMaxMom - fPdfMinMom) / (Double_t)fPdfNbinsMom;
    fPdfNbinsMomCharge = fh_pdf_pi->GetNbinsX();

    fPdfNbinsM2 = fh_pdf_pi->GetNbinsY();
    fPdfMinM2 = fh_pdf_pi->GetYaxis()->GetXmin();
    fPdfMaxM2 = fh_pdf_pi->GetYaxis()->GetXmax();
    fPdfBinSizeM2 = (fPdfMaxM2 - fPdfMinM2) / (Double_t)fPdfNbinsM2;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmEbefluctPYR::CreateHistogramms()
{
    fh_all_mom_kap = new TH1F("h_all_mom_kap", "",
			      100, 0., 10.);
    fh_acc_mom_kap = new TH1F("h_acc_mom_kap", "",
			      100, 0., 10.);
    fh_pid_mom_kap = new TH1F("h_pid_mom_kap", "",
			      100, 0., 10.);
    fh_rpid_mom_kap = new TH1F("h_rpid_mom_kap", "",
			       100, 0., 10.);

    fh_all_nka = new TH1F("h_all_nka", "", 500, -0.5, 499.5);
    fh_acc_nka = new TH1F("h_acc_nka", "", 500, -0.5, 499.5);
    fh_pid_nka = new TH1F("h_pid_nka", "", 500, -0.5, 499.5);
    fh_rpid_nka = new TH1F("h_rpid_nka", "", 500, -0.5, 499.5);
    fh_rpid_nka_true = new TH1F("h_rpid_nka_true", "", 500, -0.5, 499.5);

    fh_all_ka2pi = new TH1F("h_all_ka2pi",
			    "<K>/<#pi>, all particles",
			    fNbinsRatio, 0., 0.2);
    fh_acc_ka2pi = new TH1F("h_acc_ka2pi",
			    "<K>/<#pi>, accepted particles",
			    fNbinsRatio, 0., 0.2);
    fh_pid_ka2pi = new TH1F("h_pid_ka2pi",
			    "<K>/<#pi>, identified particles",
			    fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi = new TH1F("h_rpid_ka2pi",
			     "<K>/<#pi>, reco + PID",
			     fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true = new TH1F("h_rpid_ka2pi_true",
				  "<K>/<#pi>, reco + PID, true",
				  fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_10 = new TH1F("h_rpid_ka2pi_true_10",
				     "",
				     fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_9 = new TH1F("h_rpid_ka2pi_true_9",
				    "",
				    fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_8 = new TH1F("h_rpid_ka2pi_true_8",
				    "",
				    fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_7 = new TH1F("h_rpid_ka2pi_true_7",
				    "",
				    fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_6 = new TH1F("h_rpid_ka2pi_true_6",
				    "",
				    fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_5 = new TH1F("h_rpid_ka2pi_true_5",
				    "",
				    fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_4 = new TH1F("h_rpid_ka2pi_true_4",
				    "",
				    fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_3 = new TH1F("h_rpid_ka2pi_true_3",
				    "",
				    fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_2 = new TH1F("h_rpid_ka2pi_true_2",
				    "",
				    fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_475 = new TH1F("h_rpid_ka2pi_true_475",
				      "",
				      fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_45 = new TH1F("h_rpid_ka2pi_true_45",
				     "",
				     fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_35 = new TH1F("h_rpid_ka2pi_true_35",
				     "",
				     fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_34 = new TH1F("h_rpid_ka2pi_true_34",
				     "",
				     fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_325 = new TH1F("h_rpid_ka2pi_true_325",
				      "",
				      fNbinsRatio, 0., 0.2);
    fh_rpid_ka2pi_true_22 = new TH1F("h_rpid_ka2pi_true_22",
				     "",
				     fNbinsRatio, 0., 0.2);

    fh_all_p2pi = new TH1F("h_all_p2pi",
			   "",
			   fNbinsRatio, 0., 1.);
    fh_acc_p2pi = new TH1F("h_acc_p2pi",
			   "",
			   fNbinsRatio, 0., 1.);
    fh_rpid_p2pi = new TH1F("h_rpid_p2pi",
			    "",
			    fNbinsRatio, 0., 1.);
    fh_rpid_p2pi_true = new TH1F("h_rpid_p2pi_true",
				 "",
				 fNbinsRatio, 0., 1.);

    fh_all_kap2pip = new TH1F("h_all_kap2pip",
			      "",
			      fNbinsRatio, 0., 0.5);
    fh_acc_kap2pip = new TH1F("h_acc_kap2pip",
			      "",
			      fNbinsRatio, 0., 0.5);
    fh_rpid_kap2pip = new TH1F("h_rpid_kap2pip",
			       "",
			       fNbinsRatio, 0., 0.5);
    fh_rpid_kap2pip_true = new TH1F("h_rpid_kap2pip_true",
				    "",
				    fNbinsRatio, 0., 0.5);

    fh_all_kam2pip = new TH1F("h_all_kam2pip",
			      "",
			      fNbinsRatio, 0., 0.5);
    fh_acc_kam2pip = new TH1F("h_acc_kam2pip",
			      "",
			      fNbinsRatio, 0., 0.5);
    fh_rpid_kam2pip = new TH1F("h_rpid_kam2pip",
			       "",
			       fNbinsRatio, 0., 0.5);
    fh_rpid_kam2pip_true = new TH1F("h_rpid_kam2pip_true",
				    "",
				    fNbinsRatio, 0., 0.5);

    fh_all_kap2pim = new TH1F("h_all_kap2pim",
			      "",
			      fNbinsRatio, 0., 0.5);
    fh_acc_kap2pim = new TH1F("h_acc_kap2pim",
			      "",
			      fNbinsRatio, 0., 0.5);
    fh_rpid_kap2pim = new TH1F("h_rpid_kap2pim",
			       "",
			       fNbinsRatio, 0., 0.5);
    fh_rpid_kap2pim_true = new TH1F("h_rpid_kap2pim_true",
				    "",
				    fNbinsRatio, 0., 0.5);

    fh_all_kam2pim = new TH1F("h_all_kam2pim",
			      "",
			      fNbinsRatio, 0., 0.5);
    fh_acc_kam2pim = new TH1F("h_acc_kam2pim",
			      "",
			      fNbinsRatio, 0., 0.5);
    fh_rpid_kam2pim = new TH1F("h_rpid_kam2pim",
			       "",
			       fNbinsRatio, 0., 0.5);
    fh_rpid_kam2pim_true = new TH1F("h_rpid_kam2pim_true",
				    "",
				    fNbinsRatio, 0., 0.5);

    fh_all_ka2p = new TH1F("h_all_ka2p",
			   "",
			   fNbinsRatio, 0., 1.);
    fh_acc_ka2p = new TH1F("h_acc_ka2p",
			   "",
			   fNbinsRatio, 0., 1.);
    fh_rpid_ka2p = new TH1F("h_rpid_ka2p",
			    "",
			    fNbinsRatio, 0., 1.);
    fh_rpid_ka2p_true = new TH1F("h_rpid_ka2p_true",
				 "",
				 fNbinsRatio, 0., 1.);
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmEbefluctPYR::WriteHistogramms()
{
    // Write histogramms to file

    cout << "Integrated kaon purity: "
	<< (fn_ka_true/fn_ka_all)*100 << " %" << endl;

    fh_all_mom_kap->Write();
    fh_acc_mom_kap->Write();
    fh_pid_mom_kap->Write();
    fh_rpid_mom_kap->Write();

    fh_all_nka->Write();
    fh_acc_nka->Write();
    fh_pid_nka->Write();
    fh_rpid_nka->Write();
    fh_rpid_nka_true->Write();

    // Write particles yield ratio distributions
    fh_all_ka2pi->Write();
    fh_acc_ka2pi->Write();
    fh_pid_ka2pi->Write();
    fh_rpid_ka2pi->Write();
    fh_rpid_ka2pi_true->Write();
    fh_rpid_ka2pi_true_10->Write();
    fh_rpid_ka2pi_true_9->Write();
    fh_rpid_ka2pi_true_8->Write();
    fh_rpid_ka2pi_true_7->Write();
    fh_rpid_ka2pi_true_6->Write();
    fh_rpid_ka2pi_true_5->Write();
    fh_rpid_ka2pi_true_4->Write();
    fh_rpid_ka2pi_true_3->Write();
    fh_rpid_ka2pi_true_2->Write();
    fh_rpid_ka2pi_true_475->Write();
    fh_rpid_ka2pi_true_45->Write();
    fh_rpid_ka2pi_true_35->Write();
    fh_rpid_ka2pi_true_34->Write();
    fh_rpid_ka2pi_true_325->Write();
    fh_rpid_ka2pi_true_22->Write();

    fh_all_p2pi->Write();
    fh_acc_p2pi->Write();
    fh_rpid_p2pi->Write();
    fh_rpid_p2pi_true->Write();

    fh_all_kap2pip->Write();
    fh_acc_kap2pip->Write();
    fh_rpid_kap2pip->Write();
    fh_rpid_kap2pip_true->Write();

    fh_all_kam2pip->Write();
    fh_acc_kam2pip->Write();
    fh_rpid_kam2pip->Write();
    fh_rpid_kam2pip_true->Write();

    fh_all_kap2pim->Write();
    fh_acc_kap2pim->Write();
    fh_rpid_kap2pim->Write();
    fh_rpid_kap2pim_true->Write();

    fh_all_kam2pim->Write();
    fh_acc_kam2pim->Write();
    fh_rpid_kam2pim->Write();
    fh_rpid_kam2pim_true->Write();

    fh_all_ka2p->Write();
    fh_acc_ka2p->Write();
    fh_rpid_ka2p->Write();
    fh_rpid_ka2p_true->Write();
}
// ------------------------------------------------------------------



ClassImp(CbmEbefluctPYR);


