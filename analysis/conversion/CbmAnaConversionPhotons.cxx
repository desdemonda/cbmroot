/**
 * \file CbmAnaConversionPhotons.cxx
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2014
 **/

#include "CbmAnaConversionPhotons.h"

#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "CbmRichPoint.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmRichRing.h"
#include "CbmTrackMatchNew.h"
#include "CbmDrawHist.h"
#include "CbmL1PFFitter.h"
#include "L1Field.h"
//#include "CbmStsKFTrackFitter.h"
#include "../../littrack/cbm/elid/CbmLitGlobalElectronId.h"

#include <algorithm>
#include <map>

#include "TCanvas.h"



using namespace std;



CbmAnaConversionPhotons::CbmAnaConversionPhotons()
  : fRichPoints(NULL),
    fRichRings(NULL),
    fRichRingMatches(NULL),
    fMcTracks(NULL),
    fStsTracks(NULL),
    fStsTrackMatches(NULL),
    fGlobalTracks(NULL),
    fPrimVertex(NULL),
    fKFVertex(),
    fHistoList_photons(),
    fHistoList_EFG(),
	fHistoList_EFG_angle(),
	fHistoList_EFG_invmass(),
	fHistoList_EFG_energy(),
    fPhotons_nofPerEvent(NULL),
	fPhotons_pt(NULL),
	fPhotons_test(NULL),
	fPhotons_nofMC(NULL),
	fPhotons_energyMC(NULL),
	fPhotons_ptMC(NULL),
	fPhotons_ptRapMC(NULL),
	fPhotonsRest_energyMC(NULL),
	fPhotonsRest_pdgMotherMC(NULL),
	fPhotonsRest_ptMC(NULL),
	fPhotonsRest_ptMC_pi0(NULL),
	fPhotonsRest_ptMC_n(NULL),
	fPhotonsRest_ptMC_e(NULL),
	fPhotonsRest_ptMC_eta(NULL),
	fMCTracklist(),
	fMCTracklist_allElectronsFromGamma(),
	fRecoTracklist_allElectronsFromGamma(),
	fRecoTracklist_allElectronsFromGammaMom(),
	fRecoTracklist_allElectronsFromPi0(),
	fRecoTracklist_allElectronsFromPi0Mom(),
	global_nof_photonsMC(0),
	global_nof_photonsReco(0),
	fhGlobalNofDirectPhotons(NULL),
	fhEFG_angle_all(NULL),
	fhEFG_angle_combBack(NULL),
	fhEFG_angle_allSameG(NULL),
	fhEFG_angle_direct(NULL),
	fhEFG_angle_pi0(NULL),
	fhEFG_angle_eta(NULL),
	//fhEFG_angleVSenergy_all(NULL),
	//fhEFG_angleVSenergy_combBack(NULL),
	fhEFG_angleVSenergy_allSameG(NULL),
	fhEFG_angleVSenergy_direct(NULL),
	fhEFG_angleVSenergy_pi0(NULL),
	fhEFG_angleVSenergy_eta(NULL),
	//fhEFG_angleVSpt_all(NULL),
	//fhEFG_angleVSpt_combBack(NULL),
	fhEFG_angleVSpt_allSameG(NULL),
	fhEFG_angleVSpt_direct(NULL),
	fhEFG_angleVSpt_pi0(NULL),
	fhEFG_angleVSpt_eta(NULL),
	fhEFG_invmass_all(NULL),
	fhEFG_invmass_combBack(NULL),
	fhEFG_invmass_allSameG(NULL),
	fhEFG_invmass_direct(NULL),
	fhEFG_invmass_pi0(NULL),
	fhEFG_invmass_eta(NULL),
	fhEFG_energy_all(NULL),
	fhEFG_energy_combBack(NULL),
	fhEFG_energy_allSameG(NULL),
	fhEFG_energy_direct(NULL),
	fhEFG_energy_pi0(NULL),
	fhEFG_energy_eta(NULL),
	fhEFG_angleBelow1GeV_all(NULL),
	fhEFG_angleBelow1GeV_combBack(NULL),
	fhEFG_angleBelow1GeV_allSameG(NULL),
	fhEFG_angleBelow1GeV_direct(NULL),
	fhEFG_angleBelow1GeV_pi0(NULL),
	fhEFG_angleBelow1GeV_eta(NULL),
	fhEFG_angleAbove1GeV_all(NULL),
	fhEFG_angleAbove1GeV_combBack(NULL),
	fhEFG_angleAbove1GeV_allSameG(NULL),
	fhEFG_angleAbove1GeV_direct(NULL),
	fhEFG_angleAbove1GeV_pi0(NULL),
	fhEFG_angleAbove1GeV_eta(NULL),
	fhEFG_momentumPair_all(NULL),
	fhEFG_momentumPair_combBack(NULL),
	fhEFG_momentumPair_allSameG(NULL),
	fhEFG_momentumPair_direct(NULL),
	fhEFG_momentumPair_pi0(NULL),
	fhEFG_momentumPair_eta(NULL),
	//fhEFG_startvertexVSangle_all(NULL),
	//fhEFG_startvertexVSangle_combBack(NULL),
	fhEFG_startvertexVSangle_allSameG(NULL),
	fhEFG_startvertexVSangle_direct(NULL),
	fhEFG_startvertexVSangle_pi0(NULL),
	fhEFG_startvertexVSangle_eta(NULL),
	fhEFG_angle_all_reco(NULL),
	fhEFG_angle_combBack_reco(NULL),
	fhEFG_angle_allSameG_reco(NULL),
	fhEFG_angle_direct_reco(NULL),
	fhEFG_angle_pi0_reco(NULL),
	fhEFG_angle_eta_reco(NULL),
	fhEFG_angleBelow1GeV_all_reco(NULL),
	fhEFG_angleBelow1GeV_combBack_reco(NULL),
	fhEFG_angleBelow1GeV_allSameG_reco(NULL),
	fhEFG_angleBelow1GeV_direct_reco(NULL),
	fhEFG_angleBelow1GeV_pi0_reco(NULL),
	fhEFG_angleBelow1GeV_eta_reco(NULL),
	fhEFG_invmass_all_reco(NULL),
	fhEFG_invmass_combBack_reco(NULL),
	fhEFG_invmass_allSameG_reco(NULL),
	fhEFG_invmass_direct_reco(NULL),
	fhEFG_invmass_pi0_reco(NULL),
	fhEFG_invmass_eta_reco(NULL),
	fhEFG_angleVSpt_allSameG_reco(NULL),
	fhEFG_angleVSpt_direct_reco(NULL),
	fhEFG_angleVSpt_pi0_reco(NULL),
	fhEFG_angleVSpt_eta_reco(NULL),
	fhEFG_momentumResolutionPhoton_reco(NULL),
	fhEFG_momentumResolutionElectrons_reco(NULL),
	fhEFPI0_angle_reco(NULL),
    timer(),
    fTime(0.)
{
}

CbmAnaConversionPhotons::~CbmAnaConversionPhotons()
{
}


void CbmAnaConversionPhotons::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) { Fatal("CbmAnaConversion::Init","RootManager not instantised!"); }

	fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if ( NULL == fRichPoints) { Fatal("CbmAnaConversion::Init","No RichPoint array!"); }

	fMcTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( NULL == fMcTracks) { Fatal("CbmAnaConversion::Init","No MCTrack array!"); }

	fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
	if ( NULL == fStsTracks) { Fatal("CbmAnaConversion::Init","No StsTrack array!"); }

	fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
	if (NULL == fStsTrackMatches) { Fatal("CbmAnaConversion::Init","No StsTrackMatch array!"); }

	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if (NULL == fGlobalTracks) { Fatal("CbmAnaConversion::Init","No GlobalTrack array!"); }

	fPrimVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
	if (NULL == fPrimVertex) { Fatal("CbmAnaConversion::Init","No PrimaryVertex array!"); }

	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if (NULL == fRichRings) { Fatal("CbmAnaConversion::Init","No RichRing array!"); }

	fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if (NULL == fRichRingMatches) { Fatal("CbmAnaConversion::Init","No RichRingMatch array!"); }


	InitHistos();

	global_nof_photonsMC = 0;
	global_nof_photonsReco = 0;
}


void CbmAnaConversionPhotons::InitHistos()
{
	fHistoList_photons.clear();
	fHistoList_EFG.clear();
	fHistoList_EFG_angle.clear();
	fHistoList_EFG_invmass.clear();
	fHistoList_EFG_energy.clear();

	fPhotons_nofPerEvent = new TH1I("fPhotons_nofPerEvent", "fPhotons_nofPerEvent; nof photons per event; #", 30, -0.5, 29.5);
	fHistoList_photons.push_back(fPhotons_nofPerEvent);
	fPhotons_pt = new TH1D("fPhotons_pt", "fPhotons_pt; pt; #", 50, -0.5, 4.5);
	fHistoList_photons.push_back(fPhotons_pt);
	fPhotons_test = new TH1I("fPhotons_test", "fPhotons_test; test; #", 10, -0.5, 9.5);
	fHistoList_photons.push_back(fPhotons_test);

	fPhotons_nofMC = new TH1I("fPhotons_nofMC", "fPhotons_nofMC; test; #", 100, -0.5, 99.5);
	fHistoList_photons.push_back(fPhotons_nofMC);
	fPhotons_energyMC = new TH1D("fPhotons_energyMC", "fPhotons_energyMC; energy; #", 500, -0.5, 49.5);
	fHistoList_photons.push_back(fPhotons_energyMC);
	fPhotons_ptMC = new TH1D("fPhotons_ptMC", "fPhotons_ptMC; pt; #", 500, -0.5, 4.5);
	fHistoList_photons.push_back(fPhotons_ptMC);
	fPhotons_ptRapMC = new TH2D("fPhotons_ptRapMC", "fPhotons_ptRapMC; pt; #", 500, -0.5, 4.5, 1000, -0.5, 9.5);
	fHistoList_photons.push_back(fPhotons_ptRapMC);
	fPhotonsRest_energyMC = new TH1D("fPhotonsRest_energyMC", "fPhotonsRest_energyMC; energy; #", 500, -0.5, 49.5);
	fHistoList_photons.push_back(fPhotonsRest_energyMC);
	fPhotonsRest_pdgMotherMC = new TH1D("fPhotonsRest_pdgMotherMC", "fPhotonsRest_pdgMotherMC; pdg code; #", 5000, -0.5, 4999.5);
	fHistoList_photons.push_back(fPhotonsRest_pdgMotherMC);
	fPhotonsRest_ptMC = new TH1D("fPhotonsRest_ptMC", "fPhotonsRest_ptMC; pt; #", 500, -0.5, 4.5);
	fHistoList_photons.push_back(fPhotonsRest_ptMC);

	fPhotonsRest_ptMC_pi0 = new TH1D("fPhotonsRest_ptMC_pi0", "fPhotonsRest_ptMC_pi0; pt; #", 500, -0.5, 4.5);
	fHistoList_photons.push_back(fPhotonsRest_ptMC_pi0);
	fPhotonsRest_ptMC_n = new TH1D("fPhotonsRest_ptMC_n", "fPhotonsRest_ptMC_n; pt; #", 500, -0.5, 4.5);
	fHistoList_photons.push_back(fPhotonsRest_ptMC_n);
	fPhotonsRest_ptMC_e = new TH1D("fPhotonsRest_ptMC_e", "fPhotonsRest_ptMC_e; pt; #", 500, -0.5, 4.5);
	fHistoList_photons.push_back(fPhotonsRest_ptMC_e);
	fPhotonsRest_ptMC_eta = new TH1D("fPhotonsRest_ptMC_eta", "fPhotonsRest_ptMC_eta; pt; #", 500, -0.5, 4.5);
	fHistoList_photons.push_back(fPhotonsRest_ptMC_eta);

	fhGlobalNofDirectPhotons = new TH1D("fhGlobalNofDirectPhotons", "fhGlobalNofDirectPhotons; ; #", 2, 0., 2.);
	fHistoList_photons.push_back(fhGlobalNofDirectPhotons);
	fhGlobalNofDirectPhotons->GetXaxis()->SetBinLabel(1, "nof global DP");		// number of all direct photons from MC
	fhGlobalNofDirectPhotons->GetXaxis()->SetBinLabel(2, "nof reconstructed DP");		// number of all direct photons which can be reconstructed


	fhEFG_angle_all		= new TH1D("fhEFG_angle_all", "fhEFG_angle_all; angle [deg]; #", 2000, 0., 100.);
	fhEFG_angle_combBack= new TH1D("fhEFG_angle_combBack", "fhEFG_angle_combBack; angle [deg]; #", 2000, 0., 100.);
	fhEFG_angle_allSameG= new TH1D("fhEFG_angle_allSameG", "fhEFG_angle_allSameG; angle [deg]; #", 400, 0., 20.);
	fhEFG_angle_direct	= new TH1D("fhEFG_angle_direct", "fhEFG_angle_direct; angle [deg]; #", 400, 0., 20.);
	fhEFG_angle_pi0		= new TH1D("fhEFG_angle_pi0", "fhEFG_angle_pi0; angle [deg]; #", 400, 0., 20.);
	fhEFG_angle_eta		= new TH1D("fhEFG_angle_eta", "fhEFG_angle_eta; angle [deg]; #", 400, 0., 20.);
	fHistoList_EFG_angle.push_back(fhEFG_angle_all);
	fHistoList_EFG_angle.push_back(fhEFG_angle_combBack);
	fHistoList_EFG_angle.push_back(fhEFG_angle_allSameG);
	fHistoList_EFG_angle.push_back(fhEFG_angle_direct);
	fHistoList_EFG_angle.push_back(fhEFG_angle_pi0);
	fHistoList_EFG_angle.push_back(fhEFG_angle_eta);
	fhEFG_invmass_all		= new TH1D("fhEFG_invmass_all", "fhEFG_invmass_all; invmass [GeV/c^2]; #", 5000, 0., 0.5);
	fhEFG_invmass_combBack	= new TH1D("fhEFG_invmass_combBack", "fhEFG_invmass_combBack; invmass [GeV/c^2]; #", 5000, 0., 0.5);
	fhEFG_invmass_allSameG	= new TH1D("fhEFG_invmass_allSameG", "fhEFG_invmass_allSameG; invmass [GeV/c^2]; #", 1000, 0., 0.1);
	fhEFG_invmass_direct	= new TH1D("fhEFG_invmass_direct", "fhEFG_invmass_direct; invmass [GeV/c^2]; #", 1000, 0., 0.1);
	fhEFG_invmass_pi0		= new TH1D("fhEFG_invmass_pi0", "fhEFG_invmass_pi0; invmass [GeV/c^2]; #", 1000, 0., 0.1);
	fhEFG_invmass_eta		= new TH1D("fhEFG_invmass_eta", "fhEFG_invmass_eta; invmass [GeV/c^2]; #", 1000, 0., 0.1);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_all);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_combBack);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_allSameG);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_direct);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_pi0);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_eta);
	fhEFG_energy_all		= new TH1D("fhEFG_energy_all", "fhEFG_energy_all; energy [GeV]; #", 5000, 0., 50);
	fhEFG_energy_combBack	= new TH1D("fhEFG_energy_combBack", "fhEFG_energy_combBack; energy [GeV]; #", 5000, 0., 50);
	fhEFG_energy_allSameG	= new TH1D("fhEFG_energy_allSameG", "fhEFG_energy_allSameG; energy [GeV]; #", 5000, 0., 50);
	fhEFG_energy_direct		= new TH1D("fhEFG_energy_direct", "fhEFG_energy_direct; energy [GeV]; #", 5000, 0., 50);
	fhEFG_energy_pi0		= new TH1D("fhEFG_energy_pi0", "fhEFG_energy_pi0; energy [GeV]; #", 5000, 0., 50);
	fhEFG_energy_eta		= new TH1D("fhEFG_energy_eta", "fhEFG_energy_eta; energy [GeV]; #", 5000, 0., 50);
	fHistoList_EFG_energy.push_back(fhEFG_energy_all);
	fHistoList_EFG_energy.push_back(fhEFG_energy_combBack);
	fHistoList_EFG_energy.push_back(fhEFG_energy_allSameG);
	fHistoList_EFG_energy.push_back(fhEFG_energy_direct);
	fHistoList_EFG_energy.push_back(fhEFG_energy_pi0);
	fHistoList_EFG_energy.push_back(fhEFG_energy_eta);
	
	// opening angles for photon-energies below 1 GeV
	fhEFG_angleBelow1GeV_all		= new TH1D("fhEFG_angleBelow1GeV_all", "fhEFG_angleBelow1GeV_all; angle [deg]; #", 2000, 0., 100.);
	fhEFG_angleBelow1GeV_combBack	= new TH1D("fhEFG_angleBelow1GeV_combBack", "fhEFG_angleBelow1GeV_combBack; angle [deg]; #", 2000, 0., 100.);
	fhEFG_angleBelow1GeV_allSameG	= new TH1D("fhEFG_angleBelow1GeV_allSameG", "fhEFG_angleBelow1GeV_allSameG; angle [deg]; #", 400, 0., 20.);
	fhEFG_angleBelow1GeV_direct		= new TH1D("fhEFG_angleBelow1GeV_direct", "fhEFG_angleBelow1GeV_direct; angle [deg]; #", 400, 0., 20.);
	fhEFG_angleBelow1GeV_pi0		= new TH1D("fhEFG_angleBelow1GeV_pi0", "fhEFG_angleBelow1GeV_pi0; angle [deg]; #", 400, 0., 20.);
	fhEFG_angleBelow1GeV_eta		= new TH1D("fhEFG_angleBelow1GeV_eta", "fhEFG_angleBelow1GeV_eta; angle [deg]; #", 400, 0., 20.);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_all);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_combBack);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_allSameG);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_direct);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_pi0);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_eta);
	
	// opening angles for photon-energies above 1 GeV
	fhEFG_angleAbove1GeV_all		= new TH1D("fhEFG_angleAbove1GeV_all", "fhEFG_angleAbove1GeV_all; angle [deg]; #", 2000, 0., 100.);
	fhEFG_angleAbove1GeV_combBack	= new TH1D("fhEFG_angleAbove1GeV_combBack", "fhEFG_angleAbove1GeV_combBack; angle [deg]; #", 2000, 0., 100.);
	fhEFG_angleAbove1GeV_allSameG	= new TH1D("fhEFG_angleAbove1GeV_allSameG", "fhEFG_angleAbove1GeV_allSameG; angle [deg]; #", 400, 0., 20.);
	fhEFG_angleAbove1GeV_direct		= new TH1D("fhEFG_angleAbove1GeV_direct", "fhEFG_angleAbove1GeV_direct; angle [deg]; #", 400, 0., 20.);
	fhEFG_angleAbove1GeV_pi0		= new TH1D("fhEFG_angleAbove1GeV_pi0", "fhEFG_angleAbove1GeV_pi0; angle [deg]; #", 400, 0., 20.);
	fhEFG_angleAbove1GeV_eta		= new TH1D("fhEFG_angleAbove1GeV_eta", "fhEFG_angleAbove1GeV_eta; angle [deg]; #", 400, 0., 20.);
	fHistoList_EFG_angle.push_back(fhEFG_angleAbove1GeV_all);
	fHistoList_EFG_angle.push_back(fhEFG_angleAbove1GeV_combBack);
	fHistoList_EFG_angle.push_back(fhEFG_angleAbove1GeV_allSameG);
	fHistoList_EFG_angle.push_back(fhEFG_angleAbove1GeV_direct);
	fHistoList_EFG_angle.push_back(fhEFG_angleAbove1GeV_pi0);
	fHistoList_EFG_angle.push_back(fhEFG_angleAbove1GeV_eta);

	// opening angle vs energy (MC)
	//fhEFG_angleVSenergy_all			= new TH2D("fhEFG_angleVSenergy_all", "fhEFG_angleVSenergy_all;opening angle [deg];energy [GeV]", 2000, 0., 100., 5000, 0., 50.);
	//fhEFG_angleVSenergy_combBack	= new TH2D("fhEFG_angleVSenergy_combBack", "fhEFG_angleVSenergy_combBack;opening angle [deg];energy [GeV]", 2000, 0., 100., 5000, 0., 50.);
	fhEFG_angleVSenergy_allSameG	= new TH2D("fhEFG_angleVSenergy_allSameG", "fhEFG_angleVSenergy_allSameG;energy [GeV];opening angle [deg]", 5000, 0., 50., 400, 0., 20.);
	fhEFG_angleVSenergy_direct		= new TH2D("fhEFG_angleVSenergy_direct", "fhEFG_angleVSenergy_direct;energy [GeV];opening angle [deg]", 5000, 0., 50., 400, 0., 20.);
	fhEFG_angleVSenergy_pi0			= new TH2D("fhEFG_angleVSenergy_pi0", "fhEFG_angleVSenergy_pi0;energy [GeV];opening angle [deg]", 5000, 0., 50., 400, 0., 20.);
	fhEFG_angleVSenergy_eta			= new TH2D("fhEFG_angleVSenergy_eta", "fhEFG_angleVSenergy_eta;energy [GeV];opening angle [deg]", 5000, 0., 50., 400, 0., 20.);
	//fHistoList_EFG_angle.push_back(fhEFG_angleVSenergy_all);
	//fHistoList_EFG_angle.push_back(fhEFG_angleVSenergy_combBack);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSenergy_allSameG);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSenergy_direct);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSenergy_pi0);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSenergy_eta);

	// opening angle vs pt (MC)
	//fhEFG_angleVSpt_all			= new TH2D("fhEFG_angleVSpt_all", "fhEFG_angleVSpt_all;opening angle [deg];pt [GeV/c]", 2000, 0., 100., 500, 0., 5.);
	//fhEFG_angleVSpt_combBack	= new TH2D("fhEFG_angleVSpt_combBack", "fhEFG_angleVSpt_combBack;opening angle [deg];pt [GeV/c]", 2000, 0., 100., 500, 0., 5.);
	fhEFG_angleVSpt_allSameG	= new TH2D("fhEFG_angleVSpt_allSameG", "fhEFG_angleVSpt_allSameG;pt [GeV/c];opening angle [deg]", 500, 0., 5., 400, 0., 20.);
	fhEFG_angleVSpt_direct		= new TH2D("fhEFG_angleVSpt_direct", "fhEFG_angleVSpt_direct;pt [GeV/c];opening angle [deg]", 500, 0., 5., 400, 0., 20.);
	fhEFG_angleVSpt_pi0			= new TH2D("fhEFG_angleVSpt_pi0", "fhEFG_angleVSpt_pi0;pt [GeV/c];opening angle [deg]", 500, 0., 5., 400, 0., 20.);
	fhEFG_angleVSpt_eta			= new TH2D("fhEFG_angleVSpt_eta", "fhEFG_angleVSpt_eta;pt [GeV/c];opening angle [deg]", 500, 0., 5., 400, 0., 20.);
	//fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_all);
	//fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_combBack);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_allSameG);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_direct);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_pi0);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_eta);

	// momentum of the e+e- pair (sum)
	fhEFG_momentumPair_all		= new TH1D("fhEFG_momentumPair_all", "fhEFG_momentumPair_all; energy [GeV]; #", 5000, 0., 50);
	fhEFG_momentumPair_combBack	= new TH1D("fhEFG_momentumPair_combBack", "fhEFG_momentumPair_combBack; energy [GeV]; #", 5000, 0., 50);
	fhEFG_momentumPair_allSameG	= new TH1D("fhEFG_momentumPair_allSameG", "fhEFG_momentumPair_allSameG; energy [GeV]; #", 5000, 0., 50);
	fhEFG_momentumPair_direct	= new TH1D("fhEFG_momentumPair_direct", "fhEFG_momentumPair_direct; energy [GeV]; #", 5000, 0., 50);
	fhEFG_momentumPair_pi0		= new TH1D("fhEFG_momentumPair_pi0", "fhEFG_momentumPair_pi0; energy [GeV]; #", 5000, 0., 50);
	fhEFG_momentumPair_eta		= new TH1D("fhEFG_momentumPair_eta", "fhEFG_momentumPair_eta; energy [GeV]; #", 5000, 0., 50);
	fHistoList_EFG.push_back(fhEFG_momentumPair_all);
	fHistoList_EFG.push_back(fhEFG_momentumPair_combBack);
	fHistoList_EFG.push_back(fhEFG_momentumPair_allSameG);
	fHistoList_EFG.push_back(fhEFG_momentumPair_direct);
	fHistoList_EFG.push_back(fhEFG_momentumPair_pi0);
	fHistoList_EFG.push_back(fhEFG_momentumPair_eta);

	//fhEFG_startvertexVSangle_all		= new TH2D("fhEFG_startvertexVSangle_all", "fhEFG_startvertexVSangle_all; startvertex; angle", 100, -0.01, 0.02, 2000, 0., 100.);
	//fhEFG_startvertexVSangle_combBack	= new TH2D("fhEFG_startvertexVSangle_combBack", "fhEFG_startvertexVSangle_combBack; startvertex; angle", 100, -0.01, 0.02, 2000, 0., 100.);
	fhEFG_startvertexVSangle_allSameG	= new TH2D("fhEFG_startvertexVSangle_allSameG", "fhEFG_startvertexVSangle_allSameG; startvertex [cm]; angle [deg]", 200, -0.02, 0.02, 400, 0., 20.);
	fhEFG_startvertexVSangle_direct		= new TH2D("fhEFG_startvertexVSangle_direct", "fhEFG_startvertexVSangle_direct; startvertex [cm]; angle [deg]", 200, -0.02, 0.02, 400, 0., 20.);
	fhEFG_startvertexVSangle_pi0		= new TH2D("fhEFG_startvertexVSangle_pi0", "fhEFG_startvertexVSangle_pi0; startvertex [cm]; angle [deg]", 200, -0.02, 0.02, 400, 0., 20.);
	fhEFG_startvertexVSangle_eta		= new TH2D("fhEFG_startvertexVSangle_eta", "fhEFG_startvertexVSangle_eta; startvertex [cm]; angle [deg]", 200, -0.02, 0.02, 400, 0., 20.);
	//fHistoList_EFG.push_back(fhEFG_startvertexVSangle_all);
	//fHistoList_EFG.push_back(fhEFG_startvertexVSangle_combBack);
	fHistoList_EFG.push_back(fhEFG_startvertexVSangle_allSameG);
	fHistoList_EFG.push_back(fhEFG_startvertexVSangle_direct);
	fHistoList_EFG.push_back(fhEFG_startvertexVSangle_pi0);
	fHistoList_EFG.push_back(fhEFG_startvertexVSangle_eta);



	// opening angles for all photon-energies (RECO)
	fhEFG_angle_all_reco		= new TH1D("fhEFG_angle_all_reco", "fhEFG_angle_all_reco; angle [deg]; #", 2000, 0., 200.);
	fhEFG_angle_combBack_reco	= new TH1D("fhEFG_angle_combBack_reco", "fhEFG_angle_combBack_reco; angle [deg]; #", 2000, 0., 200.);
	fhEFG_angle_allSameG_reco	= new TH1D("fhEFG_angle_allSameG_reco", "fhEFG_angle_allSameG_reco; angle [deg]; #", 1000, 0., 100.);
	fhEFG_angle_direct_reco		= new TH1D("fhEFG_angle_direct_reco", "fhEFG_angle_direct_reco; angle [deg]; #", 1000, 0., 100.);
	fhEFG_angle_pi0_reco		= new TH1D("fhEFG_angle_pi0_reco", "fhEFG_angle_pi0_reco; angle [deg]; #", 1000, 0., 100.);
	fhEFG_angle_eta_reco		= new TH1D("fhEFG_angle_eta_reco", "fhEFG_angle_eta_reco; angle [deg]; #", 1000, 0., 100.);
	fHistoList_EFG_angle.push_back(fhEFG_angle_all_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angle_combBack_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angle_allSameG_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angle_direct_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angle_pi0_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angle_eta_reco);
	
	// opening angles for photon-energies below 1 GeV (RECO)
	fhEFG_angleBelow1GeV_all_reco		= new TH1D("fhEFG_angleBelow1GeV_all_reco", "fhEFG_angleBelow1GeV_all_reco; angle [deg]; #", 2000, 0., 100.);
	fhEFG_angleBelow1GeV_combBack_reco	= new TH1D("fhEFG_angleBelow1GeV_combBack_reco", "fhEFG_angleBelow1GeV_combBack_reco; angle [deg]; #", 2000, 0., 100.);
	fhEFG_angleBelow1GeV_allSameG_reco	= new TH1D("fhEFG_angleBelow1GeV_allSameG_reco", "fhEFG_angleBelow1GeV_allSameG_reco; angle [deg]; #", 2000, 0., 100.);
	fhEFG_angleBelow1GeV_direct_reco	= new TH1D("fhEFG_angleBelow1GeV_direct_reco", "fhEFG_angleBelow1GeV_direct_reco; angle [deg]; #", 2000, 0., 100.);
	fhEFG_angleBelow1GeV_pi0_reco		= new TH1D("fhEFG_angleBelow1GeV_pi0_reco", "fhEFG_angleBelow1GeV_pi0_reco; angle [deg]; #", 2000, 0., 100.);
	fhEFG_angleBelow1GeV_eta_reco		= new TH1D("fhEFG_angleBelow1GeV_eta_reco", "fhEFG_angleBelow1GeV_eta_reco; angle [deg]; #", 2000, 0., 100.);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_all_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_combBack_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_allSameG_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_direct_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_pi0_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angleBelow1GeV_eta_reco);
	
	fhEFG_invmass_all_reco		= new TH1D("fhEFG_invmass_all_reco", "fhEFG_invmass_all_reco; invmass [GeV/c^2]; #", 5000, 0., 5.);
	fhEFG_invmass_combBack_reco	= new TH1D("fhEFG_invmass_combBack_reco", "fhEFG_invmass_combBack_reco; invmass [GeV/c^2]; #", 5000, 0., 5.);
	fhEFG_invmass_allSameG_reco	= new TH1D("fhEFG_invmass_allSameG_reco", "fhEFG_invmass_allSameG_reco; invmass [GeV/c^2]; #", 5000, 0., 5);
	fhEFG_invmass_direct_reco	= new TH1D("fhEFG_invmass_direct_reco", "fhEFG_invmass_direct_reco; invmass [GeV/c^2]; #", 5000, 0., 5);
	fhEFG_invmass_pi0_reco		= new TH1D("fhEFG_invmass_pi0_reco", "fhEFG_invmass_pi0_reco; invmass [GeV/c^2]; #", 5000, 0., 5);
	fhEFG_invmass_eta_reco		= new TH1D("fhEFG_invmass_eta_reco", "fhEFG_invmass_eta_reco; invmass [GeV/c^2]; #", 5000, 0., 5);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_all_reco);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_combBack_reco);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_allSameG_reco);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_direct_reco);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_pi0_reco);
	fHistoList_EFG_invmass.push_back(fhEFG_invmass_eta_reco);

	// opening angle vs pt (reco)
	//fhEFG_angleVSpt_all			= new TH2D("fhEFG_angleVSpt_all", "fhEFG_angleVSpt_all;opening angle [deg];pt [GeV/c]", 2000, 0., 100., 500, 0., 5.);
	//fhEFG_angleVSpt_combBack	= new TH2D("fhEFG_angleVSpt_combBack", "fhEFG_angleVSpt_combBack;opening angle [deg];pt [GeV/c]", 2000, 0., 100., 500, 0., 5.);
	fhEFG_angleVSpt_allSameG_reco	= new TH2D("fhEFG_angleVSpt_allSameG_reco", "fhEFG_angleVSpt_allSameG_reco;pt [GeV/c];opening angle [deg]", 500, 0., 5., 400, 0., 20.);
	fhEFG_angleVSpt_direct_reco		= new TH2D("fhEFG_angleVSpt_direct_reco", "fhEFG_angleVSpt_direct_reco;pt [GeV/c];opening angle [deg]", 500, 0., 5., 400, 0., 20.);
	fhEFG_angleVSpt_pi0_reco		= new TH2D("fhEFG_angleVSpt_pi0_reco", "fhEFG_angleVSpt_pi0_reco;pt [GeV/c];opening angle [deg]", 500, 0., 5., 400, 0., 20.);
	fhEFG_angleVSpt_eta_reco		= new TH2D("fhEFG_angleVSpt_eta_reco", "fhEFG_angleVSpt_eta_reco;pt [GeV/c];opening angle [deg]", 500, 0., 5., 400, 0., 20.);
	//fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_all);
	//fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_combBack);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_allSameG_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_direct_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_pi0_reco);
	fHistoList_EFG_angle.push_back(fhEFG_angleVSpt_eta_reco);


	fhEFG_momentumResolutionPhoton_reco		= new TH2D("fhEFG_momentumResolutionPhoton_reco", "fhEFG_momentumResolutionPhoton_reco;p [GeV/c]; dp/p [%]", 20, 0., 20., 800, 0., 200.);
	fhEFG_momentumResolutionElectrons_reco	= new TH2D("fhEFG_momentumResolutionElectrons_reco", "fhEFG_momentumResolutionElectrons_reco;p [GeV/c]; dp/p [%]", 20, 0., 20., 1600, 0., 400.);
	fHistoList_EFG.push_back(fhEFG_momentumResolutionPhoton_reco);
	fHistoList_EFG.push_back(fhEFG_momentumResolutionElectrons_reco);


	fhEFPI0_angle_reco	= new TH1D("fhEFPI0_angle_reco", "fhEFPI0_angle_reco; angle [deg]; #", 500, 0., 50.);
	fHistoList_photons.push_back(fhEFPI0_angle_reco);
}







void CbmAnaConversionPhotons::Finish()
{
	//gDirectory->cd("analysis-conversion");
	gDirectory->mkdir("Photons");
	gDirectory->cd("Photons");

	gDirectory->mkdir("electrons from gamma");
	gDirectory->cd("electrons from gamma");
	
	gDirectory->mkdir("angle");
	gDirectory->mkdir("invariant mass");
	gDirectory->mkdir("energy");
	
	gDirectory->cd("angle");
	for (Int_t i = 0; i < fHistoList_EFG_angle.size(); i++){
		fHistoList_EFG_angle[i]->Write();
	}
	gDirectory->cd("..");
	
	gDirectory->cd("invariant mass");
	for (Int_t i = 0; i < fHistoList_EFG_invmass.size(); i++){
		fHistoList_EFG_invmass[i]->Write();
	}
	gDirectory->cd("..");
	
	gDirectory->cd("energy");
	for (Int_t i = 0; i < fHistoList_EFG_energy.size(); i++){
		fHistoList_EFG_energy[i]->Write();
	}
	gDirectory->cd("..");
	
	
	for (Int_t i = 0; i < fHistoList_EFG.size(); i++){
		fHistoList_EFG[i]->Write();
	}
	gDirectory->cd("..");
	
	for (Int_t i = 0; i < fHistoList_photons.size(); i++){
		fHistoList_photons[i]->Write();
	}
	gDirectory->cd("..");


	cout << "CbmAnaConversionPhotons: Realtime - " << fTime << endl;
	//timer.Print();

	cout << "CbmAnaConversionPhotons: all MC direct photons: " << global_nof_photonsMC << ", all reconstructible direct photons: " << global_nof_photonsReco << " => " << 1.0*global_nof_photonsReco/(1.0*global_nof_photonsMC)*100 << " % can be reconstructed!" << endl;


	// normalisation of some histograms
	fPhotonsRest_ptMC_pi0->Scale(1.0/(1.0*fPhotonsRest_ptMC_pi0->Integral()));
	fPhotonsRest_ptMC_n->Scale(1.0/fPhotonsRest_ptMC_n->Integral());
	fPhotonsRest_ptMC_e->Scale(1.0/fPhotonsRest_ptMC_e->Integral());
	
	TCanvas* c = new TCanvas();
	c->SetWindowSize(1600, 1600);
	DrawH1(fPhotonsRest_ptMC_pi0);
	DrawH1(fPhotonsRest_ptMC_n);
	DrawH1(fPhotonsRest_ptMC_e);
	DrawH1(fPhotonsRest_ptMC_eta);
}



void CbmAnaConversionPhotons::Exec()
{
	timer.Start();


	if (fPrimVertex != NULL){
		fKFVertex = CbmKFVertex(*fPrimVertex);
	} else {
		Fatal("CbmAnaConversion::Exec","No PrimaryVertex array!");
	}

	int nofDirectPhotons = 0;

	fMCTracklist.clear();
	fMCTracklist_allElectronsFromGamma.clear();
	fRecoTracklist_allElectronsFromGamma.clear();
	fRecoTracklist_allElectronsFromGammaMom.clear();
	fRecoTracklist_allElectronsFromPi0.clear();
	fRecoTracklist_allElectronsFromPi0Mom.clear();

	//AnalysePhotons();

	// everything related to MC-true data
	Int_t nofMcTracks = fMcTracks->GetEntriesFast();
	for (int i = 0; i < nofMcTracks; i++) {
		CbmMCTrack* mctrack = (CbmMCTrack*)fMcTracks->At(i);
		if (mctrack == NULL) continue;
		int pdg = TMath::Abs(mctrack->GetPdgCode());
		if(pdg == 11) FillMCTracklist_allElectrons(mctrack);	// filling tracklist with all electrons with mother gamma (in function)

		nofDirectPhotons += CheckMC(mctrack);
	}

	fPhotons_nofPerEvent->Fill(nofDirectPhotons);
	global_nof_photonsMC += nofDirectPhotons;

	AnalyseElectronsFromGammaMC();



	// everything related to reconstructed data
	Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
	for (int iG = 0; iG < nofGlobalTracks; iG++){
		CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(iG);
		if(NULL == gTrack) continue;
		int stsInd = gTrack->GetStsTrackIndex();
		int richInd = gTrack->GetRichRingIndex();
		if (richInd < 0) continue;
		if (stsInd < 0) continue;
		
		CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(stsInd);
		if (stsTrack == NULL) continue;

		CbmTrackMatchNew* stsMatch  = (CbmTrackMatchNew*)fStsTrackMatches->At(stsInd);
		if (stsMatch == NULL) continue;
		int stsMcTrackId = stsMatch->GetMatchedLink().GetIndex();
		if (stsMcTrackId < 0) continue;
		CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMcTracks->At(stsMcTrackId);
		if (mcTrack1 == NULL) continue;

		CbmTrackMatchNew* richMatch  = (CbmTrackMatchNew*)fRichRingMatches->At(richInd);
		if (richMatch == NULL) continue;
		int richMcTrackId = richMatch->GetMatchedLink().GetIndex();
		if (richMcTrackId < 0) continue;
		CbmMCTrack* mcTrack2 = (CbmMCTrack*) fMcTracks->At(richMcTrackId);
		if (mcTrack2 == NULL) continue;

		if(stsMcTrackId != richMcTrackId) continue;

		int pdg = TMath::Abs(mcTrack1->GetPdgCode());


		// calculate refitted momenta at primary vertex
		TVector3 refittedMomentum;
		CbmL1PFFitter fPFFitter;
		vector<CbmStsTrack> stsTracks;
		stsTracks.resize(1);
		stsTracks[0] = *stsTrack;
		vector<L1FieldRegion> vField;
		vector<float> chiPrim;
		fPFFitter.GetChiToVertex(stsTracks, vField, chiPrim, fKFVertex, 3e6);
		//cand.chi2sts = stsTracks[0].GetChiSq() / stsTracks[0].GetNDF();
		//cand.chi2Prim = chiPrim[0];
		const FairTrackParam* vtxTrack = stsTracks[0].GetParamFirst();
		vtxTrack->Momentum(refittedMomentum);



		if(pdg == 11) FillRecoTracklist_allElectrons(mcTrack1, refittedMomentum);

		if(pdg == 11) {
			int motherID = mcTrack1->GetMotherId();
			if(motherID == -1) continue;
			CbmMCTrack* mothermcTrack1 = (CbmMCTrack*) fMcTracks->At(motherID);
			int motherpdg = TMath::Abs(mothermcTrack1->GetPdgCode());
			if(motherpdg != 22) continue;
			int grandmotherID = mothermcTrack1->GetMotherId();
			if(grandmotherID != -1) continue;
			
			//nof_photons++;
			fPhotons_pt->Fill(mothermcTrack1->GetPt());
			FillMCTracklist(mcTrack1);
		}
	}

	CombineElectrons();
	AnalyseElectronsFromGammaReco();
	AnalyseElectronsFromPi0Reco();


	timer.Stop();
	cout << "CbmAnaConversionPhotons: time spent (Exec) " << timer.RealTime() << endl;
	fTime += timer.RealTime();
}




void CbmAnaConversionPhotons::AnalysePhotons() 
{
	Int_t nof_photons = 0;

	Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
	for (int iG = 0; iG < nofGlobalTracks; iG++){
		CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(iG);
		if(NULL == gTrack) continue;
		int stsInd = gTrack->GetStsTrackIndex();
		int richInd = gTrack->GetRichRingIndex();
		if (richInd < 0) continue; // no RICH segment -> no ring
		if (stsInd < 0) continue;

		CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(stsInd);
		if (stsTrack == NULL) continue;

		CbmTrackMatchNew* stsMatch  = (CbmTrackMatchNew*)fStsTrackMatches->At(stsInd);
		if (stsMatch == NULL) continue;
		int stsMcTrackId = stsMatch->GetMatchedLink().GetIndex();
		if (stsMcTrackId < 0) continue;
		CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMcTracks->At(stsMcTrackId);
		if (mcTrack1 == NULL) continue;

		CbmTrackMatchNew* richMatch  = (CbmTrackMatchNew*)fRichRingMatches->At(richInd);
		if (richMatch == NULL) continue;
		int richMcTrackId = richMatch->GetMatchedLink().GetIndex();
		if (richMcTrackId < 0) continue;
		CbmMCTrack* mcTrack2 = (CbmMCTrack*) fMcTracks->At(richMcTrackId);
		if (mcTrack2 == NULL) continue;

		// stsMcTrackId == richMcTrackId -> track was reconstructed in STS and made a ring in RICH, track matching was correct
		// in case they are not equal, the ring comes either from a secondary particle or STS track was not reconstructed
		if(stsMcTrackId != richMcTrackId) continue;

		int pdg = TMath::Abs(mcTrack2->GetPdgCode());	// extract pdg code of particle directly from rich ring
		
		if(pdg == 11) {
			int motherID = mcTrack1->GetMotherId();
			if(motherID == -1) continue;
			CbmMCTrack* mothermcTrack1 = (CbmMCTrack*) fMcTracks->At(motherID);
			int motherpdg = TMath::Abs(mothermcTrack1->GetPdgCode());
			if(motherpdg != 22) continue;
			int grandmotherID = mothermcTrack1->GetMotherId();
			if(grandmotherID != -1) continue;
			
			nof_photons++;
			fPhotons_pt->Fill(mothermcTrack1->GetPt());
			FillMCTracklist(mcTrack1);
		}
	}
	
	//fPhotons_nofPerEvent->Fill(nof_photons);

	CombineElectrons();
}



void CbmAnaConversionPhotons::FillMCTracklist(CbmMCTrack* mctrack)
{
	fMCTracklist.push_back(mctrack);
	int motherID = mctrack->GetMotherId();
	CbmMCTrack* mothermctrack = (CbmMCTrack*) fMcTracks->At(motherID);
	int motherpdg = mothermctrack->GetPdgCode();
	cout << "CbmAnaConversionPhotons: filling tracklist, pdg " << mctrack->GetPdgCode() << " - motherpdg " << motherpdg << " - motherID " << motherID << endl;
}


void CbmAnaConversionPhotons::FillMCTracklist_allElectrons(CbmMCTrack* mctrack)
{
	int motherID = mctrack->GetMotherId();
	TVector3 startvertex;
	mctrack->GetStartVertex(startvertex);
	if(startvertex.Z() > 0.1) return;
	if(motherID == -1) return;
	else {
		CbmMCTrack* mothermctrack = (CbmMCTrack*) fMcTracks->At(motherID);
		int motherpdg = mothermctrack->GetPdgCode();
		if(motherpdg == 22) {
			fMCTracklist_allElectronsFromGamma.push_back(mctrack);
		}
	}
}


void CbmAnaConversionPhotons::FillRecoTracklist_allElectrons(CbmMCTrack* mctrack, TVector3 refittedMom)
{
	int motherID = mctrack->GetMotherId();
	TVector3 startvertex;
	mctrack->GetStartVertex(startvertex);
	if(startvertex.Z() > 0.1) return;
	if(motherID == -1) return;
	else {
		CbmMCTrack* mothermctrack = (CbmMCTrack*) fMcTracks->At(motherID);
		int motherpdg = mothermctrack->GetPdgCode();
		if(motherpdg == 22) {
			fRecoTracklist_allElectronsFromGamma.push_back(mctrack);
			fRecoTracklist_allElectronsFromGammaMom.push_back(refittedMom);
		}
		if(motherpdg == 111) {
			fRecoTracklist_allElectronsFromPi0.push_back(mctrack);
			fRecoTracklist_allElectronsFromPi0Mom.push_back(refittedMom);
		}
	}
}


void CbmAnaConversionPhotons::CombineElectrons()
{
	TH1I* zwischenhisto = new TH1I("zwischenhisto", "zwischenhisto", 1000000, 0, 1000000);
	for(int i=0; i<fMCTracklist.size(); i++) {
		zwischenhisto->Fill(fMCTracklist[i]->GetMotherId());
	}
	fPhotons_test->Fill(zwischenhisto->GetMaximum());
	if(zwischenhisto->GetMaximum() >= 2) {
		cout << "CbmAnaConversionPhotons: photon found, mother ids " << zwischenhisto->GetMaximumBin()-1 << endl;
		CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMcTracks->At(zwischenhisto->GetMaximumBin()-1);
		cout << "CbmAnaConversionPhotons: additional data: pdg " << mcTrack1->GetPdgCode() << " motherId " << mcTrack1->GetMotherId() << endl;
	}
	zwischenhisto->Delete();


	int photoncounter = 0;
	std::multimap<int,int> electronMap;
	for(int i=0; i<fMCTracklist.size(); i++) {
		electronMap.insert ( std::pair<int,int>(fMCTracklist[i]->GetMotherId(), i) );
	}
	
	int check = 0;
	for(std::map<int,int>::iterator it=electronMap.begin(); it!=electronMap.end(); ++it) {
		if(it == electronMap.begin()) check = 1;
		if(it != electronMap.begin()) {
			std::map<int,int>::iterator zwischen = it;
			zwischen--;
			int id = it->first;
			int id_old = zwischen->first;
			if(id == id_old) {
				check++;
				if(check > 1) {
					cout << "CbmAnaConversionPhotons: map - photon found " << id << endl;
					photoncounter++;
					fhGlobalNofDirectPhotons->Fill(1);
				}
			}
			else check=1;
		}
	}
	//fPhotons_nofPerEvent->Fill(photoncounter);

	global_nof_photonsReco += photoncounter;
}



int CbmAnaConversionPhotons::CheckMC(CbmMCTrack* mctrack)
{
	int nofDirectPhotons = 0;

//	Int_t nofMcTracks = fMcTracks->GetEntriesFast();
//	for (int i = 0; i < nofMcTracks; i++) {
//		CbmMCTrack* mctrack = (CbmMCTrack*)fMcTracks->At(i);
//		if (mctrack == NULL) continue;  
		
		int pdg = mctrack->GetPdgCode();
		int motherId = mctrack->GetMotherId();
		Double_t energy = mctrack->GetEnergy();
		Double_t pt = mctrack->GetPt();
		Double_t rapidity = mctrack->GetRapidity();
		
		if(pdg == 22 && motherId == -1) {	// only direct photons
			nofDirectPhotons++;
			fhGlobalNofDirectPhotons->Fill(0);
			fPhotons_energyMC->Fill(energy);
			fPhotons_ptMC->Fill(pt);
			fPhotons_ptRapMC->Fill(pt, rapidity);
		}
		if(pdg == 22 && motherId != -1) {	// only secondary photons
			fPhotonsRest_energyMC->Fill(energy);
			fPhotonsRest_ptMC->Fill(pt);
			
			CbmMCTrack* mothermctrack = (CbmMCTrack*)fMcTracks->At(motherId);
			int motherpdg = mothermctrack->GetPdgCode();
			fPhotonsRest_pdgMotherMC->Fill(motherpdg);
			
			if(motherpdg == 111) fPhotonsRest_ptMC_pi0->Fill(pt);
			if(motherpdg == 2112) fPhotonsRest_ptMC_n->Fill(pt);
			if(motherpdg == 11) fPhotonsRest_ptMC_e->Fill(pt);
			if(motherpdg == 221) fPhotonsRest_ptMC_eta->Fill(pt);
		}
//	}

//	fPhotons_nofMC->Fill(nofDirectPhotons);

//	global_nof_photonsMC += nofDirectPhotons;
	return nofDirectPhotons;
}


CbmLmvmKinematicParams CbmAnaConversionPhotons::CalculateKinematicParams(const CbmMCTrack* mctrackP, const CbmMCTrack* mctrackM)
{
	CbmLmvmKinematicParams params;

    TVector3 momP;  //momentum e+
    mctrackP->GetMomentum(momP);
    Double_t energyP = TMath::Sqrt(momP.Mag2() + M2E);
    TLorentzVector lorVecP(momP, energyP);

    TVector3 momM;  //momentum e-
    mctrackM->GetMomentum(momM);
    Double_t energyM = TMath::Sqrt(momM.Mag2() + M2E);
    TLorentzVector lorVecM(momM, energyM);

    TVector3 momPair = momP + momM;
    Double_t energyPair = energyP + energyM;
    Double_t ptPair = momPair.Perp();
    Double_t pzPair = momPair.Pz();
    Double_t yPair = 0.5*TMath::Log((energyPair+pzPair)/(energyPair-pzPair));
    Double_t anglePair = lorVecM.Angle(lorVecP.Vect());
    Double_t theta = 180.*anglePair/TMath::Pi();
    Double_t minv = 2.*TMath::Sin(anglePair / 2.)*TMath::Sqrt(momM.Mag()*momP.Mag());

    params.fMomentumMag = momPair.Mag();
    params.fPt = ptPair;
    params.fRapidity = yPair;
    params.fMinv = minv;
    params.fAngle = theta;
    return params;
}


CbmLmvmKinematicParams CbmAnaConversionPhotons::CalculateKinematicParamsReco(const TVector3 electron1, const TVector3 electron2)
{
	CbmLmvmKinematicParams params;

    Double_t energyP = TMath::Sqrt(electron1.Mag2() + M2E);
    TLorentzVector lorVecP(electron1, energyP);

    Double_t energyM = TMath::Sqrt(electron2.Mag2() + M2E);
    TLorentzVector lorVecM(electron2, energyM);

    TVector3 momPair = electron1 + electron2;
    Double_t energyPair = energyP + energyM;
    Double_t ptPair = momPair.Perp();
    Double_t pzPair = momPair.Pz();
    Double_t yPair = 0.5*TMath::Log((energyPair+pzPair)/(energyPair-pzPair));
    Double_t anglePair = lorVecM.Angle(lorVecP.Vect());
    Double_t theta = 180.*anglePair/TMath::Pi();
    Double_t minv = 2.*TMath::Sin(anglePair / 2.)*TMath::Sqrt(electron1.Mag()*electron2.Mag());

    params.fMomentumMag = momPair.Mag();
    params.fPt = ptPair;
    params.fRapidity = yPair;
    params.fMinv = minv;
    params.fAngle = theta;
    return params;
}




void CbmAnaConversionPhotons::AnalyseElectronsFromGammaMC()
{
	int electronnumber = fMCTracklist_allElectronsFromGamma.size();
	cout << "CbmAnaConversionPhotons: array size in AnalyseElectronsFromGammaMC(): " << electronnumber << endl;
	for(int i=0; i<electronnumber-1; i++) {
		for(int j=i; j<electronnumber; j++) {
			if(fMCTracklist_allElectronsFromGamma[i]->GetPdgCode() + fMCTracklist_allElectronsFromGamma[j]->GetPdgCode() != 0) continue;	// only 1 electron and 1 positron allowed
			
			int motherID_i = fMCTracklist_allElectronsFromGamma[i]->GetMotherId();
			int motherID_j = fMCTracklist_allElectronsFromGamma[j]->GetMotherId();
			
			CbmLmvmKinematicParams paramSet = CalculateKinematicParams( fMCTracklist_allElectronsFromGamma[i], fMCTracklist_allElectronsFromGamma[j] );
			
			CbmMCTrack* mothermctrack_i = (CbmMCTrack*)fMcTracks->At(motherID_i);
			int grandmotherID_i = mothermctrack_i->GetMotherId();
			
			
			
			// fill all combinations of e+e-, even if they dont come from the same origin
			fhEFG_angle_all->Fill(paramSet.fAngle);
			fhEFG_invmass_all->Fill(paramSet.fMinv);
			fhEFG_momentumPair_all->Fill(paramSet.fMomentumMag);
			fhEFG_energy_all->Fill(mothermctrack_i->GetEnergy());
			
			
			// fill only combinations of e+e-, which do not have the same mother gamma -> combinatorial background
			if(motherID_i != motherID_j) {
				fhEFG_angle_combBack->Fill(paramSet.fAngle);
				fhEFG_invmass_combBack->Fill(paramSet.fMinv);
				fhEFG_momentumPair_combBack->Fill(paramSet.fMomentumMag);
				fhEFG_energy_combBack->Fill(mothermctrack_i->GetEnergy());
				continue;
			}
			
			TVector3 startvertex;
			fMCTracklist_allElectronsFromGamma[i]->GetStartVertex(startvertex);
			
			
			// fill all combinations of e+e- with the same origin
			fhEFG_angle_allSameG->Fill(paramSet.fAngle);
			fhEFG_angleVSenergy_allSameG->Fill(paramSet.fAngle, mothermctrack_i->GetEnergy());
			fhEFG_angleVSpt_allSameG->Fill(paramSet.fAngle, mothermctrack_i->GetPt());
			fhEFG_invmass_allSameG->Fill(paramSet.fMinv);
			fhEFG_momentumPair_allSameG->Fill(paramSet.fMomentumMag);
			fhEFG_energy_allSameG->Fill(mothermctrack_i->GetEnergy());
			fhEFG_startvertexVSangle_allSameG->Fill(startvertex.Z(), paramSet.fAngle);
			if(mothermctrack_i->GetEnergy() <= 1) fhEFG_angleBelow1GeV_allSameG->Fill(paramSet.fAngle);
			if(mothermctrack_i->GetEnergy() > 1) fhEFG_angleAbove1GeV_allSameG->Fill(paramSet.fAngle);
			
			
			// combinations of e+e- from the same gamma, gamma = direct gamma
			if(grandmotherID_i == -1) {
				fhEFG_angle_direct->Fill(paramSet.fAngle);
				fhEFG_angleVSenergy_direct->Fill(paramSet.fAngle, mothermctrack_i->GetEnergy());
				fhEFG_angleVSpt_direct->Fill(paramSet.fAngle, mothermctrack_i->GetPt());
				fhEFG_invmass_direct->Fill(paramSet.fMinv);
				fhEFG_momentumPair_direct->Fill(paramSet.fMomentumMag);
				fhEFG_energy_direct->Fill(mothermctrack_i->GetEnergy());
				fhEFG_startvertexVSangle_direct->Fill(startvertex.Z(), paramSet.fAngle);
				if(mothermctrack_i->GetEnergy() <= 1) fhEFG_angleBelow1GeV_direct->Fill(paramSet.fAngle);
				if(mothermctrack_i->GetEnergy() > 1) fhEFG_angleAbove1GeV_direct->Fill(paramSet.fAngle);
			}
			// combinations of e+e- from the same gamma, with gamma estimating from another particle (pi0/eta/...)
			else {
				CbmMCTrack* grandmothermctrack_i = (CbmMCTrack*)fMcTracks->At(grandmotherID_i);
				int grandmotherpdg_i = grandmothermctrack_i->GetPdgCode();
				if(grandmotherpdg_i == 111) {	// pi0
					fhEFG_angle_pi0->Fill(paramSet.fAngle);
					fhEFG_angleVSenergy_pi0->Fill(paramSet.fAngle, mothermctrack_i->GetEnergy());
					fhEFG_angleVSpt_pi0->Fill(paramSet.fAngle, mothermctrack_i->GetPt());
					fhEFG_invmass_pi0->Fill(paramSet.fMinv);
					fhEFG_momentumPair_pi0->Fill(paramSet.fMomentumMag);
					fhEFG_energy_pi0->Fill(mothermctrack_i->GetEnergy());
					fhEFG_startvertexVSangle_pi0->Fill(startvertex.Z(), paramSet.fAngle);
					if(mothermctrack_i->GetEnergy() <= 1) fhEFG_angleBelow1GeV_pi0->Fill(paramSet.fAngle);
					if(mothermctrack_i->GetEnergy() > 1) fhEFG_angleAbove1GeV_pi0->Fill(paramSet.fAngle);
				}
				if(grandmotherpdg_i == 221) {	// eta
					fhEFG_angle_eta->Fill(paramSet.fAngle);
					fhEFG_angleVSenergy_eta->Fill(paramSet.fAngle, mothermctrack_i->GetEnergy());
					fhEFG_angleVSpt_eta->Fill(paramSet.fAngle, mothermctrack_i->GetPt());
					fhEFG_invmass_eta->Fill(paramSet.fMinv);
					fhEFG_momentumPair_eta->Fill(paramSet.fMomentumMag);
					fhEFG_energy_eta->Fill(mothermctrack_i->GetEnergy());
					fhEFG_startvertexVSangle_eta->Fill(startvertex.Z(), paramSet.fAngle);
					if(mothermctrack_i->GetEnergy() <= 1) fhEFG_angleBelow1GeV_eta->Fill(paramSet.fAngle);
					if(mothermctrack_i->GetEnergy() > 1) fhEFG_angleAbove1GeV_eta->Fill(paramSet.fAngle);
				}
			}
		}
	}
}




void CbmAnaConversionPhotons::AnalyseElectronsFromGammaReco()
{
	int electronnumber = fRecoTracklist_allElectronsFromGamma.size();
	if(fRecoTracklist_allElectronsFromGamma.size() != fRecoTracklist_allElectronsFromGammaMom.size()) {
		cout << "CbmAnaConversionPhotons::AnalyseElectronsFromGammaReco() - array sizes dont fit!" << endl;
		return;
	}
	cout << "CbmAnaConversionPhotons: array size in AnalyseElectronsFromGammaReco(): " << electronnumber << endl;
	for(int i=0; i<electronnumber; i++) {
		for(int j=i; j<electronnumber; j++) {
			if(fRecoTracklist_allElectronsFromGamma[i]->GetPdgCode() + fRecoTracklist_allElectronsFromGamma[j]->GetPdgCode() != 0) continue;	// only 1 electron and 1 positron allowed
			
			
			CbmLmvmKinematicParams paramSet = CalculateKinematicParamsReco( fRecoTracklist_allElectronsFromGammaMom[i], fRecoTracklist_allElectronsFromGammaMom[j] );
			
			fhEFG_angle_all_reco->Fill(paramSet.fAngle);		// fill all combinations of e+e-, even if they dont come from the same origin
			fhEFG_invmass_all_reco->Fill(paramSet.fMinv);
			
			int motherID_i = fRecoTracklist_allElectronsFromGamma[i]->GetMotherId();
			int motherID_j = fRecoTracklist_allElectronsFromGamma[j]->GetMotherId();
			
			if(motherID_i != motherID_j) {
				fhEFG_angle_combBack_reco->Fill(paramSet.fAngle);
				fhEFG_invmass_combBack_reco->Fill(paramSet.fMinv);
				continue;
			}
			
			fhEFG_angle_allSameG_reco->Fill(paramSet.fAngle);		// all combinations of e+e- with the same origin
			fhEFG_invmass_allSameG_reco->Fill(paramSet.fMinv);
			fhEFG_angleVSpt_allSameG_reco->Fill(paramSet.fPt, paramSet.fAngle);
			
			
			CbmMCTrack* mothermctrack_i = (CbmMCTrack*)fMcTracks->At(motherID_i);
			int grandmotherID_i = mothermctrack_i->GetMotherId();
			
			fhEFG_momentumResolutionPhoton_reco->Fill(paramSet.fMomentumMag, TMath::Abs(mothermctrack_i->GetP() - paramSet.fMomentumMag)/paramSet.fMomentumMag*100);	// dp/p in percent
			fhEFG_momentumResolutionElectrons_reco->Fill(fRecoTracklist_allElectronsFromGammaMom[i].Mag(), TMath::Abs(fRecoTracklist_allElectronsFromGammaMom[i].Mag() - fRecoTracklist_allElectronsFromGamma[i]->GetP())/fRecoTracklist_allElectronsFromGammaMom[i].Mag()*100);
			fhEFG_momentumResolutionElectrons_reco->Fill(fRecoTracklist_allElectronsFromGammaMom[j].Mag(), TMath::Abs(fRecoTracklist_allElectronsFromGammaMom[j].Mag() - fRecoTracklist_allElectronsFromGamma[j]->GetP())/fRecoTracklist_allElectronsFromGammaMom[j].Mag()*100);
			
			
			if(grandmotherID_i == -1) {		// combinations of e+e- from the same gamma, gamma = direct gamma
				fhEFG_angle_direct_reco->Fill(paramSet.fAngle);
				fhEFG_invmass_direct_reco->Fill(paramSet.fMinv);
				fhEFG_angleVSpt_direct_reco->Fill(paramSet.fPt, paramSet.fAngle);
			}
			else {		// combinations of e+e- from the same gamma, with gamma estimating from another particle (pi0/eta/...)
				CbmMCTrack* grandmothermctrack_i = (CbmMCTrack*)fMcTracks->At(grandmotherID_i);
				int grandmotherpdg_i = grandmothermctrack_i->GetPdgCode();
				
				if(grandmotherpdg_i == 111) {
					fhEFG_angle_pi0_reco->Fill(paramSet.fAngle);
					fhEFG_invmass_pi0_reco->Fill(paramSet.fMinv);
					fhEFG_angleVSpt_pi0_reco->Fill(paramSet.fPt, paramSet.fAngle);
				}
				if(grandmotherpdg_i == 221) {
					fhEFG_angle_eta_reco->Fill(paramSet.fAngle);
					fhEFG_invmass_eta_reco->Fill(paramSet.fMinv);
					fhEFG_angleVSpt_eta_reco->Fill(paramSet.fPt, paramSet.fAngle);
				}
			}
		}
	}
}




void CbmAnaConversionPhotons::AnalyseElectronsFromPi0Reco()
{
	int electronnumber = fRecoTracklist_allElectronsFromPi0.size();
	if(fRecoTracklist_allElectronsFromPi0.size() != fRecoTracklist_allElectronsFromPi0Mom.size()) {
		cout << "CbmAnaConversionPhotons::AnalyseElectronsFromGammaPi0() - array sizes dont fit!" << endl;
		return;
	}
	cout << "CbmAnaConversionPhotons: array size in AnalyseElectronsFromPi0Reco(): " << electronnumber << endl;
	for(int i=0; i<electronnumber; i++) {
		for(int j=i; j<electronnumber; j++) {
			if(fRecoTracklist_allElectronsFromPi0[i]->GetPdgCode() + fRecoTracklist_allElectronsFromPi0[j]->GetPdgCode() != 0) continue;	// only 1 electron and 1 positron allowed
			
			
			CbmLmvmKinematicParams paramSet = CalculateKinematicParamsReco( fRecoTracklist_allElectronsFromPi0Mom[i], fRecoTracklist_allElectronsFromPi0Mom[j] );
			
			int motherID_i = fRecoTracklist_allElectronsFromPi0[i]->GetMotherId();
			int motherID_j = fRecoTracklist_allElectronsFromPi0[j]->GetMotherId();
			
			if(motherID_i != motherID_j) {
				continue;
			}
			fhEFPI0_angle_reco->Fill(paramSet.fAngle);
		}
	}
}

