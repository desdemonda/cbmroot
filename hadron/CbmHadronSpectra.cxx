// ------------------------------------------------------------------
// -----                     CbmHadronSpectra                   -----
// -----              Created 06/02/2006 by D.Kresan            -----
// ------------------------------------------------------------------
#include <iostream>
using namespace std;

#include "TClonesArray.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TRandom.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "CbmTofPoint.h"
#include "CbmHadron.h"
#include "CbmHadronSpectra.h"

#define M2PI 0.019479835
#define M2KA 0.24371698
#define M2PROT 0.88035435


//___________________________________________________________________
//
// CbmHadronSpectra
//
// Task for analysis of hadron spectra
//


// ------------------------------------------------------------------
CbmHadronSpectra::CbmHadronSpectra()
  : FairTask("HadronSpectra"),
    fEvents(0),
    fBeamMomentum(25),
    fPdfFileName(""),
    fPdfMinMom(0.),
    fPdfMaxMom(0.),
    fPdfMomBinSize(0.),
    fh_m2min_pi(NULL),
    fh_m2max_pi(NULL),
    fh_m2min_ka(NULL),
    fh_m2max_ka(NULL),
    fh_m2min_prot(NULL),
    fh_m2max_prot(NULL),
    fh_pur_pi(NULL),
    fh_pur_ka(NULL),
    fh_pur_prot(NULL),
    fArrayMCTrack(NULL),
    fArrayTofPoint(NULL),
    fArrayHadron(NULL),
    fh_mom_all_pip(NULL),
    fh_mom_all_kap(NULL),
    fh_mom_all_prot(NULL),
    fh_mom_all_pim(NULL),
    fh_mom_all_kam(NULL),
    fh_mom_acc_pip(NULL),
    fh_mom_acc_kap(NULL),
    fh_mom_acc_prot(NULL),
    fh_mom_acc_pim(NULL),
    fh_mom_acc_kam(NULL),
    fh_mom_pid_pip(NULL),
    fh_mom_pid_kap(NULL),
    fh_mom_pid_prot(NULL),
    fh_mom_pid_pim(NULL),
    fh_mom_pid_kam(NULL),
    fh_mom_rpid_pip(NULL),
    fh_mom_rpid_kap(NULL),
    fh_mom_rpid_prot(NULL),
    fh_mom_rpid_pim(NULL),
    fh_mom_rpid_kam(NULL),
    fh_pt_all_pip(NULL),
    fh_pt_all_kap(NULL),
    fh_pt_all_prot(NULL),
    fh_pt_all_pim(NULL),
    fh_pt_all_kam(NULL),
    fh_pt_acc_pip(NULL),
    fh_pt_acc_kap(NULL),
    fh_pt_acc_prot(NULL),
    fh_pt_acc_pim(NULL),
    fh_pt_acc_kam(NULL),
    fh_pt_rpid_pip(NULL),
    fh_pt_rpid_kap(NULL),
    fh_pt_rpid_prot(NULL),
    fh_pt_rpid_pim(NULL),
    fh_pt_rpid_kam(NULL),
    fh_pty_all_pip(NULL),
    fh_pty_all_kap(NULL),
    fh_pty_all_prot(NULL),
    fh_pty_all_pim(NULL),
    fh_pty_all_kam(NULL),
    fh_pty_acc_pip(NULL),
    fh_pty_acc_kap(NULL),
    fh_pty_acc_prot(NULL),
    fh_pty_acc_pim(NULL),
    fh_pty_acc_kam(NULL),
    fh_pty_pid_pip(NULL),
    fh_pty_pid_kap(NULL),
    fh_pty_pid_prot(NULL),
    fh_pty_pid_pim(NULL),
    fh_pty_pid_kam(NULL),
    fh_pty_rpid_pip(NULL),
    fh_pty_rpid_kap(NULL),
    fh_pty_rpid_prot(NULL),
    fh_pty_rpid_pim(NULL),
    fh_pty_rpid_kam(NULL),
    fh_mty_all_pip(NULL),
    fh_mty_all_kap(NULL),
    fh_mty_all_prot(NULL),
    fh_mty_all_pim(NULL), 
    fh_mty_all_kam(NULL),
    fh_mty_acc_pip(NULL),
    fh_mty_acc_kap(NULL),
    fh_mty_acc_prot(NULL),
    fh_mty_acc_pim(NULL),
    fh_mty_acc_kam(NULL),  
    fh_mty_rpid_pip(NULL),
    fh_mty_rpid_kap(NULL),
    fh_mty_rpid_prot(NULL),
    fh_mty_rpid_pim(NULL),
    fh_mty_rpid_kam(NULL)
{
    CreateHistogramms();
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmHadronSpectra::CbmHadronSpectra(const char* name, Int_t verbose)
  : FairTask(name, verbose),
    fEvents(0),
    fBeamMomentum(25),
    fPdfFileName(""),
    fPdfMinMom(0.),
    fPdfMaxMom(0.),
    fPdfMomBinSize(0.),
    fh_m2min_pi(NULL),
    fh_m2max_pi(NULL),
    fh_m2min_ka(NULL),
    fh_m2max_ka(NULL),
    fh_m2min_prot(NULL),
    fh_m2max_prot(NULL),
    fh_pur_pi(NULL),
    fh_pur_ka(NULL),
    fh_pur_prot(NULL),
    fArrayMCTrack(NULL),
    fArrayTofPoint(NULL),
    fArrayHadron(NULL),
    fh_mom_all_pip(NULL),
    fh_mom_all_kap(NULL),
    fh_mom_all_prot(NULL),
    fh_mom_all_pim(NULL),
    fh_mom_all_kam(NULL),
    fh_mom_acc_pip(NULL),
    fh_mom_acc_kap(NULL),
    fh_mom_acc_prot(NULL),
    fh_mom_acc_pim(NULL),
    fh_mom_acc_kam(NULL),
    fh_mom_pid_pip(NULL),
    fh_mom_pid_kap(NULL),
    fh_mom_pid_prot(NULL),
    fh_mom_pid_pim(NULL),
    fh_mom_pid_kam(NULL),
    fh_mom_rpid_pip(NULL),
    fh_mom_rpid_kap(NULL),
    fh_mom_rpid_prot(NULL),
    fh_mom_rpid_pim(NULL),
    fh_mom_rpid_kam(NULL),
    fh_pt_all_pip(NULL),
    fh_pt_all_kap(NULL),
    fh_pt_all_prot(NULL),
    fh_pt_all_pim(NULL),
    fh_pt_all_kam(NULL),
    fh_pt_acc_pip(NULL),
    fh_pt_acc_kap(NULL),
    fh_pt_acc_prot(NULL),
    fh_pt_acc_pim(NULL),
    fh_pt_acc_kam(NULL),
    fh_pt_rpid_pip(NULL),
    fh_pt_rpid_kap(NULL),
    fh_pt_rpid_prot(NULL),
    fh_pt_rpid_pim(NULL),
    fh_pt_rpid_kam(NULL),
    fh_pty_all_pip(NULL),
    fh_pty_all_kap(NULL),
    fh_pty_all_prot(NULL),
    fh_pty_all_pim(NULL),
    fh_pty_all_kam(NULL),
    fh_pty_acc_pip(NULL),
    fh_pty_acc_kap(NULL),
    fh_pty_acc_prot(NULL),
    fh_pty_acc_pim(NULL),
    fh_pty_acc_kam(NULL),
    fh_pty_pid_pip(NULL),
    fh_pty_pid_kap(NULL),
    fh_pty_pid_prot(NULL),
    fh_pty_pid_pim(NULL),
    fh_pty_pid_kam(NULL),
    fh_pty_rpid_pip(NULL),
    fh_pty_rpid_kap(NULL),
    fh_pty_rpid_prot(NULL),
    fh_pty_rpid_pim(NULL),
    fh_pty_rpid_kam(NULL),
    fh_mty_all_pip(NULL),
    fh_mty_all_kap(NULL),
    fh_mty_all_prot(NULL),
    fh_mty_all_pim(NULL), 
    fh_mty_all_kam(NULL),
    fh_mty_acc_pip(NULL),
    fh_mty_acc_kap(NULL),
    fh_mty_acc_prot(NULL),
    fh_mty_acc_pim(NULL),
    fh_mty_acc_kam(NULL),  
    fh_mty_rpid_pip(NULL),
    fh_mty_rpid_kap(NULL),
    fh_mty_rpid_prot(NULL),
    fh_mty_rpid_pim(NULL),
    fh_mty_rpid_kam(NULL)
{
    CreateHistogramms();
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmHadronSpectra::~CbmHadronSpectra()
{
    // Destructor
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
InitStatus CbmHadronSpectra::Init()
{
    // Task initialisation
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmHadronSpectra::Init : "
	    << "ROOT manager is not instantiated." << endl;
        return kFATAL;
    }

    fArrayMCTrack = (TClonesArray*) rootMgr->GetObject("MCTrack");
    if(NULL == fArrayMCTrack) {
	cout << "-W- CbmHadronSpectra::Init : "
            << "no MC track array!" << endl;
    }

    fArrayTofPoint = (TClonesArray*) rootMgr->GetObject("TOFPoint");
    if(NULL == fArrayTofPoint) {
	cout << "-W- CbmHadronSpectra::Init : "
	    << "no TOF point array!" << endl;
    }

    fArrayHadron = (TClonesArray*) rootMgr->GetObject("Hadron");
    if(NULL == fArrayHadron) {
	cout << "-W- CbmHadronSpectra::Init : "
	    << "no Hadron array!" << endl;
    }

    InitStatus status = ReadPdfFile();
    if(kSUCCESS != status) {
        return status;
    }

    cout << "-I- CbmHadronSpectra::Init : "
        << "initialisation completed." << endl;

    return kSUCCESS;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmHadronSpectra::Exec(Option_t* option)
{
    // Task execution

    // Declare variables outside the loop
    CbmMCTrack *mcTrack;
    CbmTofPoint *tofPoint;
    CbmHadron *hadron;
    Int_t mcTrackIndex;
    Int_t pdgCode;
    Double_t mom;
    Double_t pt;
    Double_t pz;
    Double_t energy;
    Double_t y;
    Double_t mt;
    Double_t tof;
    Double_t l;
    Int_t charge;
    Double_t m2;
    Int_t momChargeBin;
    Double_t m2min_pi;
    Double_t m2max_pi;
    Double_t m2min_ka;
    Double_t m2max_ka;
    Double_t m2min_prot;
    Double_t m2max_prot;
    Int_t pid;
    Double_t weight_pi;
    Double_t weight_ka;
    Double_t weight_prot;


    // Loop over MC tracks
    if(fArrayMCTrack) {
	for(Int_t iMCTrack = 0;
	    iMCTrack < fArrayMCTrack->GetEntriesFast();
	    iMCTrack++) {

	    // Get pointer to the MC track
	    mcTrack = (CbmMCTrack*) fArrayMCTrack->At(iMCTrack);
	    if(NULL == mcTrack) continue;

	    // Skip secondaries
	    if(-1 != mcTrack->GetMotherId()) continue;

	    // Process only hadrons
	    pdgCode = mcTrack->GetPdgCode();
	    if(211 != TMath::Abs(pdgCode) &&
	       321 != TMath::Abs(pdgCode) &&
	       2212 != TMath::Abs(pdgCode)) continue;

	    // Get momentum
	    mom = mcTrack->GetP();
	    pt = mcTrack->GetPt();
	    pz = mcTrack->GetPz();

	    // Fill histogramms
	    switch(pdgCode) {
	    case 211 : {
		energy = TMath::Sqrt(mom*mom + M2PI);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2PI);
		fh_mom_all_pip->Fill(mom);
                fh_pt_all_pip->Fill(pt);
		fh_pty_all_pip->Fill(y, pt);
		fh_mty_all_pip->Fill(y, mt);
                break;
	    };
	    case 321 : {
		energy = TMath::Sqrt(mom*mom + M2KA);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2KA);
		fh_mom_all_kap->Fill(mom);
                fh_pt_all_kap->Fill(pt);
		fh_pty_all_kap->Fill(y, pt);
                fh_mty_all_kap->Fill(y, mt);
                break;
	    };
	    case 2212 : {
		energy = TMath::Sqrt(mom*mom + M2PROT);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2PROT);
		fh_mom_all_prot->Fill(mom);
                fh_pt_all_prot->Fill(pt);
		fh_pty_all_prot->Fill(y, pt);
                fh_mty_all_prot->Fill(y, mt);
                break;
	    };
	    case -211 : {
		energy = TMath::Sqrt(mom*mom + M2PI);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2PI);
		fh_mom_all_pim->Fill(mom);
                fh_pt_all_pim->Fill(pt);
		fh_pty_all_pim->Fill(y, pt);
                fh_mty_all_pim->Fill(y, mt);
                break;
	    };
	    case -321 : {
		energy = TMath::Sqrt(mom*mom + M2KA);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2KA);
		fh_mom_all_kam->Fill(mom);
                fh_pt_all_kam->Fill(pt);
		fh_pty_all_kam->Fill(y, pt);
                fh_mty_all_kam->Fill(y, mt);
                break;
	    };
	    };
	}
    }


    // Loop over TOF points
    if(fArrayTofPoint && fArrayMCTrack) {
	Int_t temp = -5;
	for(Int_t iTofPoint = 0;
	    iTofPoint < fArrayTofPoint->GetEntriesFast();
	    iTofPoint++) {

	    // Get pointer to the TOF point
	    tofPoint = (CbmTofPoint*) fArrayTofPoint->At(iTofPoint);
	    if(NULL == tofPoint) continue;

	    // Get pointer to the MC track
	    mcTrackIndex = tofPoint->GetTrackID();
	    if(mcTrackIndex < 0) continue;
	    if(temp == mcTrackIndex) continue;
	    temp = mcTrackIndex;
	    mcTrack = (CbmMCTrack*) fArrayMCTrack->
		At(mcTrackIndex);
	    if(NULL == mcTrack) continue;

	    // Skip secondaries
	    if(-1 != mcTrack->GetMotherId()) continue;

	    // Process only hadrons
	    pdgCode = mcTrack->GetPdgCode();
	    if(211 != TMath::Abs(pdgCode) &&
	       321 != TMath::Abs(pdgCode) &&
	       2212 != TMath::Abs(pdgCode)) continue;

	    // Check if accepted in STS and TRD
	    if(mcTrack->GetNPoints(kSTS) < 4) continue;
//	    if(mcTrack->GetNPoints(kTRD) < 12) continue;

	    // Get momentum
	    mom = mcTrack->GetP();
	    pt = mcTrack->GetPt();
	    pz = mcTrack->GetPz();

	    // Fill histogramms
	    switch(pdgCode) {
	    case 211 : {
		energy = TMath::Sqrt(mom*mom + M2PI);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2PI);
		fh_mom_acc_pip->Fill(mom);
                fh_pt_acc_pip->Fill(pt);
		fh_pty_acc_pip->Fill(y, pt);
                fh_mty_acc_pip->Fill(y, mt);
                break;
	    };
	    case 321 : {
		energy = TMath::Sqrt(mom*mom + M2KA);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2KA);
		fh_mom_acc_kap->Fill(mom);
                fh_pt_acc_kap->Fill(pt);
		fh_pty_acc_kap->Fill(y, pt);
                fh_mty_acc_kap->Fill(y, mt);
                break;
	    };
	    case 2212 : {
		energy = TMath::Sqrt(mom*mom + M2PROT);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2PROT);
		fh_mom_acc_prot->Fill(mom);
                fh_pt_acc_prot->Fill(pt);
		fh_pty_acc_prot->Fill(y, pt);
                fh_mty_acc_prot->Fill(y, mt);
                break;
	    };
	    case -211 : {
		energy = TMath::Sqrt(mom*mom + M2PI);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2PI);
		fh_mom_acc_pim->Fill(mom);
                fh_pt_acc_pim->Fill(pt);
		fh_pty_acc_pim->Fill(y, pt);
                fh_mty_acc_pim->Fill(y, mt);
                break;
	    };
	    case -321 : {
		energy = TMath::Sqrt(mom*mom + M2KA);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2KA);
		fh_mom_acc_kam->Fill(mom);
                fh_pt_acc_kam->Fill(pt);
		fh_pty_acc_kam->Fill(y, pt);
                fh_mty_acc_kam->Fill(y, mt);
                break;
	    };
	    };

	    // Calculate m2
	    tof = gRandom->Gaus(tofPoint->GetTime(), 0.08);
	    l = tofPoint->GetLength();
	    m2 = TMath::Power(mom, 2) *
		(  TMath::Power( (tof*29.9792458)/l, 2 ) - 1  );
	    charge = pdgCode/TMath::Abs(pdgCode);

	    // Make identification
	    if(2 == fBeamMomentum) {
		if(m2 < 0.15) {       // pion
                    pdgCode = charge*211;
		} else if(m2 < 0.4) { // kaon
                    pdgCode = charge*321;
		} else {              // proton
                    pdgCode = charge*2212;
		}
	    } else {
		if(mom > 5) {
		    if(m2 < 0.4) { // pion
			pdgCode = Int_t(charge * 211);
		    } else { // proton
			pdgCode = Int_t(charge * 2212);
		    }
		} else {
		    momChargeBin = TMath::Nint((mom*charge+fPdfMaxMom) / fPdfMomBinSize - 0.5);
		    m2min_pi = fh_m2min_pi->GetBinContent(momChargeBin+1);
		    m2max_pi = fh_m2max_pi->GetBinContent(momChargeBin+1);
		    m2min_ka = fh_m2min_ka->GetBinContent(momChargeBin+1);
		    m2max_ka = fh_m2max_ka->GetBinContent(momChargeBin+1);
		    m2min_prot = fh_m2min_prot->GetBinContent(momChargeBin+1);
		    m2max_prot = fh_m2max_prot->GetBinContent(momChargeBin+1);
		    weight_pi = fh_pur_pi->GetBinContent(momChargeBin+1);
		    weight_ka = fh_pur_ka->GetBinContent(momChargeBin+1);
		    weight_prot = fh_pur_prot->GetBinContent(momChargeBin+1);
		    if(m2 < m2max_ka && m2 > m2min_ka) {
			pdgCode = (Int_t)charge*321;
		    } else if(m2 < m2max_pi && m2 > m2min_pi) {
			pdgCode = (Int_t)charge*211;
		    } else if(m2 < m2max_prot && m2 > m2min_prot) {
			pdgCode = (Int_t)charge*2212;
		    } else {
			pdgCode = 0;
		    }
		}
	    }

	    // Fill histogramms
	    switch(pdgCode) {
	    case 211 : {
		energy = TMath::Sqrt(mom*mom + M2PI);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		fh_mom_pid_pip->Fill(mom);
		fh_pty_pid_pip->Fill(y, pt);
                break;
	    };
	    case 321 : {
		energy = TMath::Sqrt(mom*mom + M2KA);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		fh_mom_pid_kap->Fill(mom);
		fh_pty_pid_kap->Fill(y, pt);
                break;
	    };
	    case 2212 : {
		energy = TMath::Sqrt(mom*mom + M2PROT);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		fh_mom_pid_prot->Fill(mom);
		fh_pty_pid_prot->Fill(y, pt);
                break;
	    };
	    case -211 : {
		energy = TMath::Sqrt(mom*mom + M2PI);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		fh_mom_pid_pim->Fill(mom);
		fh_pty_pid_pim->Fill(y, pt);
                break;
	    };
	    case -321 : {
		energy = TMath::Sqrt(mom*mom + M2KA);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		fh_mom_pid_kam->Fill(mom);
		fh_pty_pid_kam->Fill(y, pt);
                break;
	    };
	    };
	}
    }


    // Loop over hadrons
    if(fArrayHadron) {
	for(Int_t iHadron = 0;
	    iHadron < fArrayHadron->GetEntriesFast();
	    iHadron++) {

	    // Get pointer to the global track
	    hadron = (CbmHadron*) fArrayHadron->At(iHadron);
	    if(NULL == hadron) continue;

	    // Select primaries
            if(hadron->GetB() > 3) continue;

	    // Get momentum
	    mom = hadron->GetP();
	    pt = hadron->GetPt();
	    pz = hadron->GetPz();
	    charge = hadron->GetCharge();
	    m2 = hadron->GetMass2();

	    if(mom > 5) {
		if(m2 < 0.4) { // pion
		    pid = Int_t(charge * 211);
		} else { // proton
		    pid = Int_t(charge * 2212);
		}
	    } else {
		momChargeBin = TMath::Nint((mom*charge+fPdfMaxMom) / fPdfMomBinSize - 0.5);
		m2min_pi = fh_m2min_pi->GetBinContent(momChargeBin+1);
		m2max_pi = fh_m2max_pi->GetBinContent(momChargeBin+1);
		m2min_ka = fh_m2min_ka->GetBinContent(momChargeBin+1);
		m2max_ka = fh_m2max_ka->GetBinContent(momChargeBin+1);
		m2min_prot = fh_m2min_prot->GetBinContent(momChargeBin+1);
		m2max_prot = fh_m2max_prot->GetBinContent(momChargeBin+1);
                weight_pi = fh_pur_pi->GetBinContent(momChargeBin+1);
                weight_ka = fh_pur_ka->GetBinContent(momChargeBin+1);
                weight_prot = fh_pur_prot->GetBinContent(momChargeBin+1);
		if(m2 < m2max_ka && m2 > m2min_ka) {
		    pid = (Int_t)charge*321;
		} else if(m2 < m2max_pi && m2 > m2min_pi) {
		    pid = (Int_t)charge*211;
		} else if(m2 < m2max_prot && m2 > m2min_prot) {
                    pid = (Int_t)charge*2212;
		} else {
                    pid = 0;
		}
	    }

	    // Fill histogramms
	    switch(pid) {
	    case 211 : {
		energy = TMath::Sqrt(mom*mom + M2PI);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2PI);
		fh_mom_rpid_pip->Fill(mom, weight_pi);
                fh_pt_rpid_pip->Fill(pt, weight_pi);
		fh_pty_rpid_pip->Fill(y, pt, weight_pi);
                fh_mty_rpid_pip->Fill(y, mt, weight_pi);
                break;
	    };
	    case 321 : {
		energy = TMath::Sqrt(mom*mom + M2KA);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2KA);
		fh_mom_rpid_kap->Fill(mom, weight_ka);
                fh_pt_rpid_kap->Fill(pt, weight_ka);
		fh_pty_rpid_kap->Fill(y, pt, weight_ka);
                fh_mty_rpid_kap->Fill(y, mt, weight_ka);
                break;
	    };
	    case 2212 : {
		energy = TMath::Sqrt(mom*mom + M2PROT);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2PROT);
		fh_mom_rpid_prot->Fill(mom, weight_prot);
                fh_pt_rpid_prot->Fill(pt, weight_prot);
		fh_pty_rpid_prot->Fill(y, pt, weight_prot);
                fh_mty_rpid_prot->Fill(y, mt, weight_prot);
                break;
	    };
	    case -211 : {
		energy = TMath::Sqrt(mom*mom + M2PI);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2PI);
		fh_mom_rpid_pim->Fill(mom, weight_pi);
                fh_pt_rpid_pim->Fill(pt, weight_pi);
		fh_pty_rpid_pim->Fill(y, pt, weight_pi);
                fh_mty_rpid_pim->Fill(y, mt, weight_pi);
                break;
	    };
	    case -321 : {
		energy = TMath::Sqrt(mom*mom + M2KA);
		y = 0.5*TMath::Log((energy+pz) / (energy-pz));
		mt = TMath::Sqrt(pt*pt + M2KA);
		fh_mom_rpid_kam->Fill(mom, weight_ka);
                fh_pt_rpid_kam->Fill(pt, weight_ka);
		fh_pty_rpid_kam->Fill(y, pt, weight_ka);
                fh_mty_rpid_kam->Fill(y, mt, weight_ka);
                break;
	    };
	    };
	}
    }

    if(0 == (fEvents%10)) {
	cout << "-I- CbmHadronSpectra::Exec : "
            << "event " << fEvents << " processed." << endl;
    }
    fEvents += 1;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmHadronSpectra::Finish()
{
    // Finish of the task execution
//    WriteHistogramms();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmHadronSpectra::CreateHistogramms()
{
    // Create histogramms
    fh_mom_all_pip = new TH1F("h_mom_all_pip",
			      "Momentum of all #pi^{+}",
			      40, 0., 10.);
    fh_mom_all_kap = new TH1F("h_mom_all_kap",
			      "Momentum of all K^{+}",
			      40, 0., 10.);
    fh_mom_all_prot = new TH1F("h_mom_all_prot",
			       "Momentum of all p",
			       40, 0., 10.);
    fh_mom_all_pim = new TH1F("h_mom_all_pim",
			      "Momentum of all #pi^{-}",
			      40, 0., 10.);
    fh_mom_all_kam = new TH1F("h_mom_all_kam",
			      "Momentum of all K^{-}",
			      40, 0., 10.);

    fh_mom_acc_pip = new TH1F("h_mom_acc_pip",
			      "Momentum of accepted #pi^{+}",
			      40, 0., 10.);
    fh_mom_acc_kap = new TH1F("h_mom_acc_kap",
			      "Momentum of accepted K^{+}",
			      40, 0., 10.);
    fh_mom_acc_prot = new TH1F("h_mom_acc_prot",
			       "Momentum of accepted p",
			       40, 0., 10.);
    fh_mom_acc_pim = new TH1F("h_mom_acc_pim",
			      "Momentum of accepted #pi^{-}",
			      40, 0., 10.);
    fh_mom_acc_kam = new TH1F("h_mom_acc_kam",
			      "Momentum of accepted K^{-}",
			      40, 0., 10.);

    fh_mom_pid_pip = new TH1F("h_mom_pid_pip",
			      "Momentum of pid #pi^{+}",
			      40, 0., 10.);
    fh_mom_pid_kap = new TH1F("h_mom_pid_kap",
			      "Momentum of pid K^{+}",
			      40, 0., 10.);
    fh_mom_pid_prot = new TH1F("h_mom_pid_prot",
			       "Momentum of pid p",
			       40, 0., 10.);
    fh_mom_pid_pim = new TH1F("h_mom_pid_pim",
			      "Momentum of pid #pi^{-}",
			      40, 0., 10.);
    fh_mom_pid_kam = new TH1F("h_mom_pid_kam",
			      "Momentum of pid K^{-}",
			      40, 0., 10.);

    fh_mom_rpid_pip = new TH1F("h_mom_rpid_pip",
				  "Momentum of identified #pi^{+}",
				  40, 0., 10.);
    fh_mom_rpid_kap = new TH1F("h_mom_rpid_kap",
				  "Momentum of identified K^{+}",
				  40, 0., 10.);
    fh_mom_rpid_prot = new TH1F("h_mom_rpid_prot",
				   "Momentum of identified p",
				   40, 0., 10.);
    fh_mom_rpid_pim = new TH1F("h_mom_rpid_pim",
				  "Momentum of identified #pi^{-}",
				  40, 0., 10.);
    fh_mom_rpid_kam = new TH1F("h_mom_rpid_kam",
				  "Momentum of identified K^{-}",
				  40, 0., 10.);


    fh_pt_all_pip = new TH1F("h_pt_all_pip",
			     "pt of all #pi^{+}",
			     20, 0., 2.);
    fh_pt_all_kap = new TH1F("h_pt_all_kap",
			     "pt of all K^{+}",
			     20, 0., 2.);
    fh_pt_all_prot = new TH1F("h_pt_all_prot",
			      "pt of all p",
			      20, 0., 2.);
    fh_pt_all_pim = new TH1F("h_pt_all_pim",
			     "pt of all #pi^{-}",
			     20, 0., 2.);
    fh_pt_all_kam = new TH1F("h_pt_all_kam",
			     "pt of all K^{-}",
			     20, 0., 2.);

    fh_pt_acc_pip = new TH1F("h_pt_acc_pip",
			     "pt of accepted #pi^{+}",
			     20, 0., 2.);
    fh_pt_acc_kap = new TH1F("h_pt_acc_kap",
			     "pt of accepted K^{+}",
			     20, 0., 2.);
    fh_pt_acc_prot = new TH1F("h_pt_acc_prot",
			      "pt of accepted p",
			      20, 0., 2.);
    fh_pt_acc_pim = new TH1F("h_pt_acc_pim",
			     "pt of accepted #pi^{-}",
			     20, 0., 2.);
    fh_pt_acc_kam = new TH1F("h_pt_acc_kam",
			     "pt of accepted K^{-}",
			     20, 0., 2.);

    fh_pt_rpid_pip = new TH1F("h_pt_rpid_pip",
				 "pt of identified #pi^{+}",
				 20, 0., 2.);
    fh_pt_rpid_kap = new TH1F("h_pt_rpid_kap",
				 "pt of identified K^{+}",
				 20, 0., 2.);
    fh_pt_rpid_prot = new TH1F("h_pt_rpid_prot",
				  "pt of identified p",
				  20, 0., 2.);
    fh_pt_rpid_pim = new TH1F("h_pt_rpid_pim",
				 "pt of identified #pi^{-}",
				 20, 0., 2.);
    fh_pt_rpid_kam = new TH1F("h_pt_rpid_kam",
				 "pt of identified K^{-}",
				 20, 0., 2.);


    fh_pty_all_pip = new TH2F("h_pty_all_pip",
			      "pt-y of all #pi^{+}",
			      40, 0., 4.,
			      20, 0., 2.);
    fh_pty_all_kap = new TH2F("h_pty_all_kap",
			      "pt-y of all K^{+}",
			      40, 0., 4.,
                              20, 0., 2.);
    fh_pty_all_prot = new TH2F("h_pty_all_prot",
			       "pt-y of all p",
			       40, 0., 4.,
			       20, 0., 2.);
    fh_pty_all_pim = new TH2F("h_pty_all_pim",
			      "pt-y of all #pi^{-}",
			      40, 0., 4.,
                              20, 0., 2.);
    fh_pty_all_kam = new TH2F("h_pty_all_kam",
			      "pt-y of all K^{-}",
			      40, 0., 4.,
                              20, 0., 2.);

    fh_pty_acc_pip = new TH2F("h_pty_acc_pip",
			      "pt-y of accepted #pi^{+}",
			      40, 0., 4.,
                              20, 0., 2.);
    fh_pty_acc_kap = new TH2F("h_pty_acc_kap",
			      "pt-y of accepted K^{+}",
			      40, 0., 4.,
                              20, 0., 2.);
    fh_pty_acc_prot = new TH2F("h_pty_acc_prot",
			       "pt-y of accepted p",
			       40, 0., 4.,
			       20, 0., 2.);
    fh_pty_acc_pim = new TH2F("h_pty_acc_pim",
			      "pt-y of accepted #pi^{-}",
			      40, 0., 4.,
                              20, 0., 2.);
    fh_pty_acc_kam = new TH2F("h_pty_acc_kam",
			      "pt-y of accepted K^{-}",
			      40, 0., 4.,
                              20, 0., 2.);

    fh_pty_pid_pip = new TH2F("h_pty_pid_pip",
			      "pt-y of pid #pi^{+}",
			      40, 0., 4.,
                              20, 0., 2.);
    fh_pty_pid_kap = new TH2F("h_pty_pid_kap",
			      "pt-y of pid K^{+}",
			      40, 0., 4.,
                              20, 0., 2.);
    fh_pty_pid_prot = new TH2F("h_pty_pid_prot",
			       "pt-y of pid p",
			       40, 0., 4.,
			       20, 0., 2.);
    fh_pty_pid_pim = new TH2F("h_pty_pid_pim",
			      "pt-y of pid #pi^{-}",
			      40, 0., 4.,
                              20, 0., 2.);
    fh_pty_pid_kam = new TH2F("h_pty_pid_kam",
			      "pt-y of pid K^{-}",
			      40, 0., 4.,
                              20, 0., 2.);

    fh_pty_rpid_pip = new TH2F("h_pty_rpid_pip",
				  "pt-y of identified #pi^{+}",
				  40, 0., 4.,
				  20, 0., 2.);
    fh_pty_rpid_kap = new TH2F("h_pty_rpid_kap",
				  "pt-y of identified K^{+}",
				  40, 0., 4.,
				  20, 0., 2.);
    fh_pty_rpid_prot = new TH2F("h_pty_rpid_prot",
				   "pt-y of identified p",
				   40, 0., 4.,
				   20, 0., 2.);
    fh_pty_rpid_pim = new TH2F("h_pty_rpid_pim",
				  "pt-y of identified #pi^{-}",
				  40, 0., 4.,
				  20, 0., 2.);
    fh_pty_rpid_kam = new TH2F("h_pty_rpid_kam",
				  "pt-y of identified K^{-}",
				  40, 0., 4.,
				  20, 0., 2.);


    fh_mty_all_pip = new TH2F("h_mty_all_pip",
			      "mt-y of all #pi^{+}",
			      40, 0., 4.,
			      30, 0., 3.);
    fh_mty_all_kap = new TH2F("h_mty_all_kap",
			      "mt-y of all K^{+}",
			      40, 0., 4.,
                              30, 0., 3.);
    fh_mty_all_prot = new TH2F("h_mty_all_prot",
			       "mt-y of all p",
			       40, 0., 4.,
			       30, 0., 3.);
    fh_mty_all_pim = new TH2F("h_mty_all_pim",
			      "mt-y of all #pi^{-}",
			      40, 0., 4.,
                              30, 0., 3.);
    fh_mty_all_kam = new TH2F("h_mty_all_kam",
			      "mt-y of all K^{-}",
			      40, 0., 4.,
                              30, 0., 3.);

    fh_mty_acc_pip = new TH2F("h_mty_acc_pip",
			      "mt-y of accepted #pi^{+}",
			      40, 0., 4.,
                              30, 0., 3.);
    fh_mty_acc_kap = new TH2F("h_mty_acc_kap",
			      "mt-y of accepted K^{+}",
			      40, 0., 4.,
                              30, 0., 3.);
    fh_mty_acc_prot = new TH2F("h_mty_acc_prot",
			       "mt-y of accepted p",
			       40, 0., 4.,
			       30, 0., 3.);
    fh_mty_acc_pim = new TH2F("h_mty_acc_pim",
			      "mt-y of accepted #pi^{-}",
			      40, 0., 4.,
                              30, 0., 3.);
    fh_mty_acc_kam = new TH2F("h_mty_acc_kam",
			      "mt-y of accepted K^{-}",
			      40, 0., 4.,
                              30, 0., 3.);

    fh_mty_rpid_pip = new TH2F("h_mty_rpid_pip",
				  "mt-y of identified #pi^{+}",
				  40, 0., 4.,
				  30, 0., 3.);
    fh_mty_rpid_kap = new TH2F("h_mty_rpid_kap",
				  "mt-y of identified K^{+}",
				  40, 0., 4.,
				  30, 0., 3.);
    fh_mty_rpid_prot = new TH2F("h_mty_rpid_prot",
				   "mt-y of identified p",
				   40, 0., 4.,
				   30, 0., 3.);
    fh_mty_rpid_pim = new TH2F("h_mty_rpid_pim",
				  "mt-y of identified #pi^{-}",
				  40, 0., 4.,
				  30, 0., 3.);
    fh_mty_rpid_kam = new TH2F("h_mty_rpid_kam",
				  "mt-y of identified K^{-}",
				  40, 0., 4.,
				  30, 0., 3.);
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
InitStatus CbmHadronSpectra::ReadPdfFile()
{
    // Open PDF file and get histogramms for PID
    if(fPdfFileName.IsNull()) return kSUCCESS;
    TFile* pdfFile = new TFile(fPdfFileName);
    if(NULL == pdfFile) {
	cout << "-E- CbmHadronSpectra::ReadPdfFile : "
	    << "file " << fPdfFileName << " does not exist!" << endl;
        return kSUCCESS;
    }
    fh_m2min_pi = (TH1F*) pdfFile->Get("h_m2min_pi");
    fh_m2max_pi = (TH1F*) pdfFile->Get("h_m2max_pi");
    fh_m2min_ka = (TH1F*) pdfFile->Get("h_m2min_ka");
    fh_m2max_ka = (TH1F*) pdfFile->Get("h_m2max_ka");
    fh_m2min_prot = (TH1F*) pdfFile->Get("h_m2min_prot");
    fh_m2max_prot = (TH1F*) pdfFile->Get("h_m2max_prot");
    fh_pur_pi = (TH1F*) pdfFile->Get("h_pur_pi");
    fh_pur_ka = (TH1F*) pdfFile->Get("h_pur_ka");
    fh_pur_prot = (TH1F*) pdfFile->Get("h_pur_prot");
    TH1F* h_sm2 = (TH1F*) pdfFile->Get("h_sm2");
    fPdfMinMom = h_sm2->GetXaxis()->GetXmin();
    fPdfMaxMom = h_sm2->GetXaxis()->GetXmax();
    fPdfMomBinSize = (fPdfMaxMom-fPdfMinMom) /
        (Double_t)h_sm2->GetXaxis()->GetNbins();
    return kSUCCESS;
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void CbmHadronSpectra::WriteHistogramms()
{
    // Write histogramms to the file

    // Scale histogramms
    Double_t cMom = 40./10. / (Double_t)fEvents;
    Double_t cPt = 20./2. / (Double_t)fEvents;
    Double_t cPty = 40./4. * 20./2. / (Double_t)fEvents;
    Double_t cMty = 40./4. * 30./3. / (Double_t)fEvents;

    fh_mom_all_pip->Scale(cMom);
    fh_mom_all_kap->Scale(cMom);
    fh_mom_all_prot->Scale(cMom);
    fh_mom_all_pim->Scale(cMom);
    fh_mom_all_kam->Scale(cMom);

    fh_mom_acc_pip->Scale(cMom);
    fh_mom_acc_kap->Scale(cMom);
    fh_mom_acc_prot->Scale(cMom);
    fh_mom_acc_pim->Scale(cMom);
    fh_mom_acc_kam->Scale(cMom);

    fh_mom_pid_pip->Scale(cMom);
    fh_mom_pid_kap->Scale(cMom);
    fh_mom_pid_prot->Scale(cMom);
    fh_mom_pid_pim->Scale(cMom);
    fh_mom_pid_kam->Scale(cMom);

    fh_mom_rpid_pip->Scale(cMom);
    fh_mom_rpid_kap->Scale(cMom);
    fh_mom_rpid_prot->Scale(cMom);
    fh_mom_rpid_pim->Scale(cMom);
    fh_mom_rpid_kam->Scale(cMom);

    fh_pt_all_pip->Scale(cPt);
    fh_pt_all_kap->Scale(cPt);
    fh_pt_all_prot->Scale(cPt);
    fh_pt_all_pim->Scale(cPt);
    fh_pt_all_kam->Scale(cPt);

    fh_pt_acc_pip->Scale(cPt);
    fh_pt_acc_kap->Scale(cPt);
    fh_pt_acc_prot->Scale(cPt);
    fh_pt_acc_pim->Scale(cPt);
    fh_pt_acc_kam->Scale(cPt);

    fh_pt_rpid_pip->Scale(cPt);
    fh_pt_rpid_kap->Scale(cPt);
    fh_pt_rpid_prot->Scale(cPt);
    fh_pt_rpid_pim->Scale(cPt);
    fh_pt_rpid_kam->Scale(cPt);

    fh_pty_all_pip->Scale(cPty);
    fh_pty_all_kap->Scale(cPty);
    fh_pty_all_prot->Scale(cPty);
    fh_pty_all_pim->Scale(cPty);
    fh_pty_all_kam->Scale(cPty);

    fh_pty_acc_pip->Scale(cPty);
    fh_pty_acc_kap->Scale(cPty);
    fh_pty_acc_prot->Scale(cPty);
    fh_pty_acc_pim->Scale(cPty);
    fh_pty_acc_kam->Scale(cPty);

    fh_pty_pid_pip->Scale(cPty);
    fh_pty_pid_kap->Scale(cPty);
    fh_pty_pid_prot->Scale(cPty);
    fh_pty_pid_pim->Scale(cPty);
    fh_pty_pid_kam->Scale(cPty);

    fh_pty_rpid_pip->Scale(cPty);
    fh_pty_rpid_kap->Scale(cPty);
    fh_pty_rpid_prot->Scale(cPty);
    fh_pty_rpid_pim->Scale(cPty);
    fh_pty_rpid_kam->Scale(cPty);

    fh_mty_all_pip->Scale(cMty);
    fh_mty_all_kap->Scale(cMty);
    fh_mty_all_prot->Scale(cMty);
    fh_mty_all_pim->Scale(cMty);
    fh_mty_all_kam->Scale(cMty);

    fh_mty_acc_pip->Scale(cMty);
    fh_mty_acc_kap->Scale(cMty);
    fh_mty_acc_prot->Scale(cMty);
    fh_mty_acc_pim->Scale(cMty);
    fh_mty_acc_kam->Scale(cMty);

    fh_mty_rpid_pip->Scale(cMty);
    fh_mty_rpid_kap->Scale(cMty);
    fh_mty_rpid_prot->Scale(cMty);
    fh_mty_rpid_pim->Scale(cMty);
    fh_mty_rpid_kam->Scale(cMty);


    // Write histogramms
    fh_mom_all_pip->Write();
    fh_mom_all_kap->Write();
    fh_mom_all_prot->Write();
    fh_mom_all_pim->Write();
    fh_mom_all_kam->Write();

    fh_mom_acc_pip->Write();
    fh_mom_acc_kap->Write();
    fh_mom_acc_prot->Write();
    fh_mom_acc_pim->Write();
    fh_mom_acc_kam->Write();

    fh_mom_pid_pip->Write();
    fh_mom_pid_kap->Write();
    fh_mom_pid_prot->Write();
    fh_mom_pid_pim->Write();
    fh_mom_pid_kam->Write();

    fh_mom_rpid_pip->Write();
    fh_mom_rpid_kap->Write();
    fh_mom_rpid_prot->Write();
    fh_mom_rpid_pim->Write();
    fh_mom_rpid_kam->Write();

    fh_pt_all_pip->Write();
    fh_pt_all_kap->Write();
    fh_pt_all_prot->Write();
    fh_pt_all_pim->Write();
    fh_pt_all_kam->Write();

    fh_pt_acc_pip->Write();
    fh_pt_acc_kap->Write();
    fh_pt_acc_prot->Write();
    fh_pt_acc_pim->Write();
    fh_pt_acc_kam->Write();

    fh_pt_rpid_pip->Write();
    fh_pt_rpid_kap->Write();
    fh_pt_rpid_prot->Write();
    fh_pt_rpid_pim->Write();
    fh_pt_rpid_kam->Write();

    fh_pty_all_pip->Write();
    fh_pty_all_kap->Write();
    fh_pty_all_prot->Write();
    fh_pty_all_pim->Write();
    fh_pty_all_kam->Write();

    fh_pty_acc_pip->Write();
    fh_pty_acc_kap->Write();
    fh_pty_acc_prot->Write();
    fh_pty_acc_pim->Write();
    fh_pty_acc_kam->Write();

    fh_pty_pid_pip->Write();
    fh_pty_pid_kap->Write();
    fh_pty_pid_prot->Write();
    fh_pty_pid_pim->Write();
    fh_pty_pid_kam->Write();

    fh_pty_rpid_pip->Write();
    fh_pty_rpid_kap->Write();
    fh_pty_rpid_prot->Write();
    fh_pty_rpid_pim->Write();
    fh_pty_rpid_kam->Write();

    fh_mty_all_pip->Write();
    fh_mty_all_kap->Write();
    fh_mty_all_prot->Write();
    fh_mty_all_pim->Write();
    fh_mty_all_kam->Write();

    fh_mty_acc_pip->Write();
    fh_mty_acc_kap->Write();
    fh_mty_acc_prot->Write();
    fh_mty_acc_pim->Write();
    fh_mty_acc_kam->Write();

    fh_mty_rpid_pip->Write();
    fh_mty_rpid_kap->Write();
    fh_mty_rpid_prot->Write();
    fh_mty_rpid_pim->Write();
    fh_mty_rpid_kam->Write();
}
// ------------------------------------------------------------------


ClassImp(CbmHadronSpectra);


