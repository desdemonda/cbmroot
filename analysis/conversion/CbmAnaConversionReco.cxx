/**
 * \file CbmAnaConversionReco.cxx
 *
 * 
 * 
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2014
 **/

#include "CbmAnaConversionReco.h"

// standard includes
#include <iostream>

// includes from ROOT
#include "TRandom3.h"

// included from CbmRoot
#include "FairRootManager.h"
#include "CbmMCTrack.h"


#define M2E 2.6112004954086e-7
using namespace std;



CbmAnaConversionReco::CbmAnaConversionReco()
  : fMcTracks(NULL),
    fMCTracklist_all(),
    fRecoTracklistEPEM(),
    fRecoTracklistEPEM_ids(),
    fRecoMomentum(),
    fRecoRefittedMomentum(),
    fHistoList_MC(),
    fHistoList_reco(),
    fHistoList_reco_mom(),
    fHistoList_gg(),
	fHistoList_gee(),
	fHistoList_eeee(),
	fHistoList_all(),
    fhInvariantMass_MC_all(NULL),
    fhInvariantMass_MC_pi0(NULL),
    fhInvariantMass_MC_pi0_epem(NULL),
    fhInvariantMass_MC_pi0_gepem(NULL),
    fhInvariantMass_MC_pi0_gg(NULL),
    fhInvariantMass_MC_eta(NULL),
    fhInvariantMassReco_pi0(NULL),
    fhMCtest(NULL),
    fhEPEM_invmass_gg_mc(NULL),
	fhEPEM_invmass_gg_refitted(NULL),
	fhEPEM_invmass_gee_mc(NULL),
	fhEPEM_invmass_gee_refitted(NULL),
	fhEPEM_invmass_eeee_mc(NULL),
	fhEPEM_invmass_eeee_refitted(NULL),
	fhEPEM_invmass_all_mc(NULL),
	fhEPEM_invmass_all_refitted(NULL),
	fhEPEM_openingAngle_gg_mc(NULL),
	fhEPEM_openingAngle_gg_refitted(NULL),
	fhEPEM_openingAngle_gee_mc(NULL),
	fhEPEM_openingAngle_gee_refitted(NULL),
    fhInvMass_EPEM_mc(NULL),
    fhInvMass_EPEM_stsMomVec(NULL),
    fhInvMass_EPEM_refitted(NULL),
    fhInvMass_EPEM_error_stsMomVec(NULL),
    fhInvMass_EPEM_error_refitted(NULL),
    fhInvMass_EPEM_openingAngleRef(NULL),
    fhUsedMomenta_stsMomVec(NULL),
    fhUsedMomenta_mc(NULL),
    fhUsedMomenta_error_stsMomVec(NULL),
    fhUsedMomenta_error_refitted(NULL),
    fhUsedMomenta_errorX_stsMomVec(NULL),
    fhUsedMomenta_vsX_stsMomVec(NULL),
    fhUsedMomenta_errorY_stsMomVec(NULL),
    fhUsedMomenta_vsY_stsMomVec(NULL),
    fhUsedMomenta_errorZ_stsMomVec(NULL),
    fhUsedMomenta_vsZ_stsMomVec(NULL),
    fhUsedMomenta_errorX_refitted(NULL),
    fhUsedMomenta_vsX_refitted(NULL),
    fhUsedMomenta_errorY_refitted(NULL),
    fhUsedMomenta_vsY_refitted(NULL),
    fhUsedMomenta_errorZ_refitted(NULL),
    fhUsedMomenta_vsZ_refitted(NULL),
    fhInvariantMass_pi0epem(NULL),
    fhPi0_startvertex(NULL),
    fhPi0_startvertexElectrons(NULL),
    fhInvMassWithFullRecoCuts(NULL),
    timer(),
    fTime(0.)
{
}

CbmAnaConversionReco::~CbmAnaConversionReco()
{
}


void CbmAnaConversionReco::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) { Fatal("CbmAnaConversion::Init","RootManager not instantised!"); }

	fMcTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( NULL == fMcTracks) { Fatal("CbmAnaConversion::Init","No MCTrack array!"); }


	InitHistos();
}


void CbmAnaConversionReco::InitHistos()
{
	fHistoList_MC.clear();
	fHistoList_reco.clear();
	fHistoList_reco_mom.clear();

	fHistoList_gg.clear();
	fHistoList_gee.clear();
	fHistoList_eeee.clear();
	fHistoList_all.clear();


	fhInvariantMass_MC_all			= new TH1D("fhInvariantMass_MC_all", "fhInvariantMass_MC_all;mass [GeV/c^2];#", 2000, 0., 2.);
	fhInvariantMass_MC_pi0			= new TH1D("fhInvariantMass_MC_pi0", "fhInvariantMass_MC_pi0;mass [GeV/c^2];#", 2000, 0., 2.);
	fhInvariantMass_MC_pi0_epem		= new TH1D("fhInvariantMass_MC_pi0_epem", "fhInvariantMass_MC_pi0_epem;mass [GeV/c^2];#", 2000, 0., 2.);
	fhInvariantMass_MC_pi0_gepem	= new TH1D("fhInvariantMass_MC_pi0_gepem", "fhInvariantMass_MC_pi0_gepem;mass [GeV/c^2];#", 2000, 0., 2.);
	fhInvariantMass_MC_pi0_gg		= new TH1D("fhInvariantMass_MC_pi0_gg", "fhInvariantMass_MC_pi0_gg;mass [GeV/c^2];#", 2000, 0., 2.);
	fhInvariantMass_MC_eta			= new TH1D("fhInvariantMass_MC_eta", "fhInvariantMass_MC_eta;mass [GeV/c^2];#", 2000, 0., 2.);
	fHistoList_MC.push_back(fhInvariantMass_MC_all);
	fHistoList_MC.push_back(fhInvariantMass_MC_pi0);
	fHistoList_MC.push_back(fhInvariantMass_MC_pi0_epem);
	fHistoList_MC.push_back(fhInvariantMass_MC_pi0_gepem);
	fHistoList_MC.push_back(fhInvariantMass_MC_pi0_gg);
	fHistoList_MC.push_back(fhInvariantMass_MC_eta);
	
	
	fhMCtest	= new TH1D("fhMCtest", "fhMCtest;mass [GeV/c^2];#", 2000, 0., 2.);
	fHistoList_MC.push_back(fhMCtest);




	fhEPEM_invmass_gg_mc			= new TH1D("fhEPEM_invmass_gg_mc","fhEPEM_invmass_gg_mc;mass [GeV/c^2];#", 400, -0.0025, 1.9975);
	fhEPEM_invmass_gg_refitted		= new TH1D("fhEPEM_invmass_gg_refitted","fhEPEM_invmass_gg_refitted;mass [GeV/c^2];#", 400, -0.0025, 1.9975);
	fhEPEM_invmass_gee_mc			= new TH1D("fhEPEM_invmass_gee_mc","fhEPEM_invmass_gee_mc;mass [GeV/c^2];#", 400, -0.0025, 1.9975);
	fhEPEM_invmass_gee_refitted		= new TH1D("fhEPEM_invmass_gee_refitted","fhEPEM_invmass_gee_refitted;mass [GeV/c^2];#", 400, -0.0025, 1.9975);
	fhEPEM_invmass_eeee_mc			= new TH1D("fhEPEM_invmass_eeee_mc","fhEPEM_invmass_eeee_mc;mass [GeV/c^2];#", 400, -0.0025, 1.9975);
	fhEPEM_invmass_eeee_refitted	= new TH1D("fhEPEM_invmass_eeee_refitted","fhEPEM_invmass_eeee_refitted;mass [GeV/c^2];#", 400, -0.0025, 1.9975);
	fhEPEM_invmass_all_mc			= new TH1D("fhEPEM_invmass_all_mc","fhEPEM_invmass_all_mc;mass [GeV/c^2];#", 400, -0.0025, 1.9975);
	fhEPEM_invmass_all_refitted		= new TH1D("fhEPEM_invmass_all_refitted","fhEPEM_invmass_all_refitted;mass [GeV/c^2];#", 400, -0.0025, 1.9975);
	fHistoList_gg.push_back(fhEPEM_invmass_gg_mc);
	fHistoList_gg.push_back(fhEPEM_invmass_gg_refitted);
	fHistoList_gee.push_back(fhEPEM_invmass_gee_mc);
	fHistoList_gee.push_back(fhEPEM_invmass_gee_refitted);
	fHistoList_eeee.push_back(fhEPEM_invmass_eeee_mc);
	fHistoList_eeee.push_back(fhEPEM_invmass_eeee_refitted);
	fHistoList_all.push_back(fhEPEM_invmass_all_mc);
	fHistoList_all.push_back(fhEPEM_invmass_all_refitted);

	fhEPEM_openingAngle_gg_mc			= new TH1D("fhEPEM_openingAngle_gg_mc","fhEPEM_openingAngle_gg_mc;angle [deg];#", 1010, -0.1, 100.9);
	fhEPEM_openingAngle_gg_refitted		= new TH1D("fhEPEM_openingAngle_gg_refitted","fhEPEM_openingAngle_gg_refitted;angle [deg];#", 1010, -0.1, 100.9);
	fhEPEM_openingAngle_gee_mc			= new TH1D("fhEPEM_openingAngle_gee_mc","fhEPEM_openingAngle_gee_mc;angle [deg];#", 1010, -0.1, 100.9);
	fhEPEM_openingAngle_gee_refitted	= new TH1D("fhEPEM_openingAngle_gee_refitted","fhEPEM_openingAngle_gee_refitted;angle [deg];#", 1010, -0.1, 100.9);
	fHistoList_gg.push_back(fhEPEM_openingAngle_gg_mc);
	fHistoList_gg.push_back(fhEPEM_openingAngle_gg_refitted);
	fHistoList_gee.push_back(fhEPEM_openingAngle_gee_mc);
	fHistoList_gee.push_back(fhEPEM_openingAngle_gee_refitted);





	fhInvMass_EPEM_mc				= new TH1D("fhInvMass_EPEM_mc","fhInvariantMass_recoMomentum1 (mc);mass [GeV/c^2];#", 400, -0.0025, 1.9975);
	fhInvMass_EPEM_stsMomVec		= new TH1D("fhInvMass_EPEM_stsMomVec","fhInvariantMass_recoMomentum2 (stsMomentumVec);mass [GeV/c^2];#", 200, -0.005, 1.995);
	fhInvMass_EPEM_refitted			= new TH1D("fhInvMass_EPEM_refitted","fhInvariantMass_recoMomentum3 (refitted at primary);mass [GeV/c^2];#", 200, -0.005, 1.995);
	fhInvMass_EPEM_error_stsMomVec	= new TH1D("fhInvMass_EPEM_error_stsMomVec","fhInvariantMass_recoMomentum4 (error, stsMomentumVec);(mc-reco)/mc;#", 500, -0.005, 4.995);
	fhInvMass_EPEM_error_refitted	= new TH1D("fhInvMass_EPEM_error_refitted","fhInvariantMass_recoMomentum5 (error, refitted);(mc-reco)/mc;#", 500, -0.005, 4.995);
	fhInvMass_EPEM_openingAngleRef	= new TH1D("fhInvMass_EPEM_openingAngleRef","fhInvariantMass_openingAngleRef;angle [deg];#", 1010, -0.1, 100.9);
	fhUsedMomenta_stsMomVec			= new TH1D("fhUsedMomenta_stsMomVec","fhMomentumtest1;momentum;#", 100, 0., 2.);
	fhUsedMomenta_mc				= new TH1D("fhUsedMomenta_mc","fhMomentumtest2;momentum;#", 100, 0., 2.);
	fhUsedMomenta_error_stsMomVec	= new TH1D("fhUsedMomenta_error_stsMomVec","fhMomentumtest3 (error);(mc-reco)/mc;#", 400, -2.005, 1.995);
	fhUsedMomenta_error_refitted	= new TH1D("fhUsedMomenta_error_refitted","fhMomentumtest4 (error);(mc-reco_refitted)/mc;#", 400, -2.005, 1.995);
	
	fhUsedMomenta_errorX_stsMomVec	= new TH1D("fhUsedMomenta_errorX_stsMomVec","fhMomentumtest5 (error of x-momentum);(mc-reco_reco)/mc;#", 401, -4.01, 4.01);
	fhUsedMomenta_vsX_stsMomVec		= new TH2D("fhUsedMomenta_vsX_stsMomVec","fhMomentumtest5vs (error of x-momentum);mc;reco", 101, -1.01, 1.01, 101, -1.01, 1.01);
	fhUsedMomenta_errorY_stsMomVec	= new TH1D("fhUsedMomenta_errorY_stsMomVec","fhMomentumtest6 (error of y-momentum);(mc-reco_reco)/mc;#", 401, -4.01, 4.01);
	fhUsedMomenta_vsY_stsMomVec		= new TH2D("fhUsedMomenta_vsY_stsMomVec","fhMomentumtest6vs (error of y-momentum);mc;reco", 101, -1.01, 1.01, 101, -1.01, 1.01);
	fhUsedMomenta_errorZ_stsMomVec	= new TH1D("fhUsedMomenta_errorZ_stsMomVec","fhMomentumtest7 (error of z-momentum);(mc-reco_reco)/mc;#", 401, -4.01, 4.01);
	fhUsedMomenta_vsZ_stsMomVec		= new TH2D("fhUsedMomenta_vsZ_stsMomVec","fhMomentumtest7vs (error of z-momentum);mc;reco", 201, -0.01, 4.01, 201, -0.01, 4.01);
	fhUsedMomenta_errorX_refitted	= new TH1D("fhUsedMomenta_errorX_refitted","fhMomentumtest5 (error of x-momentum);(mc-reco_reco)/mc;#", 401, -4.01, 4.01);
	fhUsedMomenta_vsX_refitted		= new TH2D("fhUsedMomenta_vsX_refitted","fhMomentumtest5vs (error of x-momentum);mc;reco", 101, -1.01, 1.01, 101, -1.01, 1.01);
	fhUsedMomenta_errorY_refitted	= new TH1D("fhUsedMomenta_errorY_refitted","fhMomentumtest6 (error of y-momentum);(mc-reco_reco)/mc;#", 401, -4.01, 4.01);
	fhUsedMomenta_vsY_refitted		= new TH2D("fhUsedMomenta_vsY_refitted","fhMomentumtest6vs (error of y-momentum);mc;reco", 101, -1.01, 1.01, 101, -1.01, 1.01);
	fhUsedMomenta_errorZ_refitted	= new TH1D("fhUsedMomenta_errorZ_refitted","fhMomentumtest7 (error of z-momentum);(mc-reco_reco)/mc;#", 401, -4.01, 4.01);
	fhUsedMomenta_vsZ_refitted		= new TH2D("fhUsedMomenta_vsZ_refitted","fhMomentumtest7vs (error of z-momentum);mc;reco", 201, -0.01, 4.01, 201, -0.01, 4.01);
	fHistoList_reco.push_back(fhInvMass_EPEM_mc);
	fHistoList_reco.push_back(fhInvMass_EPEM_stsMomVec);
	fHistoList_reco.push_back(fhInvMass_EPEM_refitted);
	fHistoList_reco.push_back(fhInvMass_EPEM_error_stsMomVec);
	fHistoList_reco.push_back(fhInvMass_EPEM_error_refitted);
	fHistoList_reco.push_back(fhInvMass_EPEM_openingAngleRef);
	fHistoList_reco_mom.push_back(fhUsedMomenta_stsMomVec);
	fHistoList_reco_mom.push_back(fhUsedMomenta_mc);
	fHistoList_reco_mom.push_back(fhUsedMomenta_error_stsMomVec);
	fHistoList_reco_mom.push_back(fhUsedMomenta_error_refitted);
	fHistoList_reco_mom.push_back(fhUsedMomenta_errorX_stsMomVec);		// error of x-component of reconstructed momentum
	fHistoList_reco_mom.push_back(fhUsedMomenta_errorY_stsMomVec);		// error of y-component of reconstructed momentum
	fHistoList_reco_mom.push_back(fhUsedMomenta_errorZ_stsMomVec);		// error of z-component of reconstructed momentum
	fHistoList_reco_mom.push_back(fhUsedMomenta_vsX_stsMomVec);	// x-component of reconstructed momentum vs mc-momentum
	fHistoList_reco_mom.push_back(fhUsedMomenta_vsY_stsMomVec);	// y-component of reconstructed momentum vs mc-momentum
	fHistoList_reco_mom.push_back(fhUsedMomenta_vsZ_stsMomVec);	// z-component of reconstructed momentum vs mc-momentum
	fHistoList_reco_mom.push_back(fhUsedMomenta_errorX_refitted);		// error of x-component of reconstructed momentum
	fHistoList_reco_mom.push_back(fhUsedMomenta_errorY_refitted);		// error of y-component of reconstructed momentum
	fHistoList_reco_mom.push_back(fhUsedMomenta_errorZ_refitted);		// error of z-component of reconstructed momentum
	fHistoList_reco_mom.push_back(fhUsedMomenta_vsX_refitted);	// x-component of reconstructed momentum vs mc-momentum
	fHistoList_reco_mom.push_back(fhUsedMomenta_vsY_refitted);	// y-component of reconstructed momentum vs mc-momentum
	fHistoList_reco_mom.push_back(fhUsedMomenta_vsZ_refitted);	// z-component of reconstructed momentum vs mc-momentum
	
	fhInvariantMass_pi0epem = new TH1D("fhInvariantMass_pi0epem","fhInvariantMass_pi0epem;mass [GeV/c^2];#", 400, 0., 2.);
	fHistoList_reco.push_back(fhInvariantMass_pi0epem);
	
	fhPi0_startvertex = new TH1D("fhPi0_startvertex","fhPi0_startvertex;z[cm];#", 200, -0.25, 99.75);
	fHistoList_reco.push_back(fhPi0_startvertex);
	
	fhPi0_startvertexElectrons = new TH1D("fhPi0_startvertexElectrons","fhPi0_startvertexElectrons;z[cm];#", 400, -0.25, 199.75);
	fHistoList_reco.push_back(fhPi0_startvertexElectrons);

	fhInvMassWithFullRecoCuts = new TH1D("fhInvMassWithFullRecoCuts","fhInvMassWithFullRecoCuts;mass [GeV/c^2];#", 400, 0., 2.);
	fHistoList_reco.push_back(fhInvMassWithFullRecoCuts);

}







void CbmAnaConversionReco::Finish()
{
	//gDirectory->cd("analysis-conversion");
	gDirectory->mkdir("MCreco");
	gDirectory->cd("MCreco");
	for (Int_t i = 0; i < fHistoList_MC.size(); i++){
		fHistoList_MC[i]->Write();
	}
	gDirectory->cd("..");

	gDirectory->mkdir("Reconstruction2");
	gDirectory->cd("Reconstruction2");
	
	gDirectory->mkdir("pi0 -> gg");
	gDirectory->cd("pi0 -> gg");
	for (Int_t i = 0; i < fHistoList_gg.size(); i++){
		fHistoList_gg[i]->Write();
	}
	gDirectory->cd("..");
	
	gDirectory->mkdir("pi0 -> ge+e-");
	gDirectory->cd("pi0 -> ge+e-");
	for (Int_t i = 0; i < fHistoList_gee.size(); i++){
		fHistoList_gee[i]->Write();
	}
	gDirectory->cd("..");
	
	gDirectory->mkdir("pi0 -> e+e-e+e-");
	gDirectory->cd("pi0 -> e+e-e+e-");
	for (Int_t i = 0; i < fHistoList_eeee.size(); i++){
		fHistoList_eeee[i]->Write();
	}
	gDirectory->cd("..");
	
	gDirectory->mkdir("pi0 -> all");
	gDirectory->cd("pi0 -> all");
	for (Int_t i = 0; i < fHistoList_all.size(); i++){
		fHistoList_all[i]->Write();
	}
	gDirectory->cd("..");
	
	for (Int_t i = 0; i < fHistoList_reco.size(); i++){
		fHistoList_reco[i]->Write();
	}
	gDirectory->mkdir("Momenta2");
	gDirectory->cd("Momenta2");
	for (Int_t i = 0; i < fHistoList_reco_mom.size(); i++){
		fHistoList_reco_mom[i]->Write();
	}
	gDirectory->cd("../..");
	
	cout << "CbmAnaConversionReco: Realtime - " << fTime << endl;
	//timer.Print();
}




void CbmAnaConversionReco::SetTracklistMC(vector<CbmMCTrack*> MCTracklist)
{
	fMCTracklist_all = MCTracklist;
}


void CbmAnaConversionReco::SetTracklistReco(vector<CbmMCTrack*> MCTracklist, vector<TVector3> RecoTracklist1, vector<TVector3> RecoTracklist2, vector<int> ids)
{
	fRecoTracklistEPEM = MCTracklist;
	fRecoTracklistEPEM_ids = ids;
	fRecoMomentum = RecoTracklist1;
	fRecoRefittedMomentum = RecoTracklist2;
}






void CbmAnaConversionReco::InvariantMassMC_all() 
// calculation of invariant mass via X -> gamma gamma -> e+ e- e+ e-, only MC data with several cuts (see FillMCTrackslists())
{
	timer.Start();


	cout << "InvariantMassTestMC - Start..." << endl;
	cout << "InvariantMassTestMC - Size of fTracklistMC_all:\t " << fMCTracklist_all.size() << endl;
	if(fMCTracklist_all.size() >= 4) {
		for(int i=0; i<fMCTracklist_all.size(); i++) {
			for(int j=i+1; j<fMCTracklist_all.size(); j++) {
				for(int k=j+1; k<fMCTracklist_all.size(); k++) {
					for(int l=k+1; l<fMCTracklist_all.size(); l++) {
					
						if(fMCTracklist_all[i]->GetPdgCode() + fMCTracklist_all[j]->GetPdgCode() + fMCTracklist_all[k]->GetPdgCode() + fMCTracklist_all[l]->GetPdgCode() != 0) continue;
					
						int motherId1 = fMCTracklist_all[i]->GetMotherId();
						int motherId2 = fMCTracklist_all[j]->GetMotherId();
						int motherId3 = fMCTracklist_all[k]->GetMotherId();
						int motherId4 = fMCTracklist_all[l]->GetMotherId();
						
						// decay pi0 -> e+ e- e+ e-
						if(motherId1 == motherId2 && motherId1 == motherId3 && motherId1 == motherId4) {
							cout << "testxyz" << endl;
							Double_t invmass = Invmass_4particles(fMCTracklist_all[i], fMCTracklist_all[j], fMCTracklist_all[k], fMCTracklist_all[l]);
							fhMCtest->Fill(invmass);
							
							if (motherId1 != -1) {
								int mcMotherPdg1  = -1;
								CbmMCTrack* mother1 = (CbmMCTrack*) fMcTracks->At(motherId1);
								if (NULL != mother1) mcMotherPdg1 = mother1->GetPdgCode();
								if(mcMotherPdg1 == 111) {
									Double_t invmass2 = 0;
									invmass2 = Invmass_4particles(fMCTracklist_all[i], fMCTracklist_all[j], fMCTracklist_all[k], fMCTracklist_all[l]);
									fhMCtest->Fill(invmass2);
									fhInvariantMass_MC_pi0->Fill(invmass2);
									fhInvariantMass_MC_pi0_epem->Fill(invmass2);
									cout << "#######################################  Decay pi0 -> e+e-e+e- detected!\t\t" << invmass2 << endl;
								}
							}
							else {
								continue;
							}
						}


						int grandmotherId1 = -1;
						int grandmotherId2 = -1;
						int grandmotherId3 = -1;
						int grandmotherId4 = -1;

						int mcMotherPdg1  = -1;
						int mcMotherPdg2  = -1;
						int mcMotherPdg3  = -1;
						int mcMotherPdg4  = -1;
						int mcGrandmotherPdg1  = -1;
						int mcGrandmotherPdg2  = -1;
						int mcGrandmotherPdg3  = -1;
						int mcGrandmotherPdg4  = -1;


						if (motherId1 != -1) {
							CbmMCTrack* mother1 = (CbmMCTrack*) fMcTracks->At(motherId1);
							if (NULL != mother1) mcMotherPdg1 = mother1->GetPdgCode();
							grandmotherId1 = mother1->GetMotherId();
							if(grandmotherId1 != -1) {
								CbmMCTrack* grandmother1 = (CbmMCTrack*) fMcTracks->At(grandmotherId1);
								if (NULL != grandmother1) mcGrandmotherPdg1 = grandmother1->GetPdgCode();
							}
						}
						if (motherId2 != -1) {
							CbmMCTrack* mother2 = (CbmMCTrack*) fMcTracks->At(motherId2);
							if (NULL != mother2) mcMotherPdg2 = mother2->GetPdgCode();
							grandmotherId2 = mother2->GetMotherId();
							if(grandmotherId2 != -1) {
								CbmMCTrack* grandmother2 = (CbmMCTrack*) fMcTracks->At(grandmotherId2);
								if (NULL != grandmother2) mcGrandmotherPdg2 = grandmother2->GetPdgCode();
							}
						}
						if (motherId3 != -1) {
							CbmMCTrack* mother3 = (CbmMCTrack*) fMcTracks->At(motherId3);
							if (NULL != mother3) mcMotherPdg3 = mother3->GetPdgCode();
							grandmotherId3 = mother3->GetMotherId();
							if(grandmotherId3 != -1) {
								CbmMCTrack* grandmother3 = (CbmMCTrack*) fMcTracks->At(grandmotherId3);
								if (NULL != grandmother3) mcGrandmotherPdg3 = grandmother3->GetPdgCode();
							}
						}
						if (motherId4 != -1) {
							CbmMCTrack* mother4 = (CbmMCTrack*) fMcTracks->At(motherId4);
							if (NULL != mother4) mcMotherPdg4 = mother4->GetPdgCode();
							grandmotherId4 = mother4->GetMotherId();
							if(grandmotherId4 != -1) {
								CbmMCTrack* grandmother4 = (CbmMCTrack*) fMcTracks->At(grandmotherId4);
								if (NULL != grandmother4) mcGrandmotherPdg4 = grandmother4->GetPdgCode();
							}
						}



						if(motherId1 == motherId2 && motherId1 == motherId3) {}
						if(motherId1 == motherId2 && motherId1 == motherId4) {}
						if(motherId1 == motherId3 && motherId1 == motherId4) {}
						if(motherId2 == motherId3 && motherId2 == motherId4) {}

					//	if(  ((motherId1 == motherId2 && motherId3 == motherId4) && (mcMotherPdg1 == 22 || mcMotherPdg1 == 111) && (mcMotherPdg3 == 22 || mcMotherPdg3 == 111))
					//	  || ((motherId1 == motherId3 && motherId2 == motherId4) && (mcMotherPdg1 == 22 || mcMotherPdg1 == 111) && (mcMotherPdg2 == 22 || mcMotherPdg2 == 111))
					//	  || ((motherId1 == motherId4 && motherId2 == motherId3) && (mcMotherPdg1 == 22 || mcMotherPdg1 == 111) && (mcMotherPdg2 == 22 || mcMotherPdg2 == 111))) {
					
						// decay X -> gg -> e+ e- e+ e-
						if(  ((motherId1 == motherId2 && motherId3 == motherId4) && (mcMotherPdg1 == 22) && (mcMotherPdg3 == 22) && grandmotherId1 == grandmotherId3)
						  || ((motherId1 == motherId3 && motherId2 == motherId4) && (mcMotherPdg1 == 22) && (mcMotherPdg2 == 22) && grandmotherId1 == grandmotherId2)
						  || ((motherId1 == motherId4 && motherId2 == motherId3) && (mcMotherPdg1 == 22) && (mcMotherPdg2 == 22) && grandmotherId1 == grandmotherId2)) {
							Double_t invmass = Invmass_4particles(fMCTracklist_all[i], fMCTracklist_all[j], fMCTracklist_all[k], fMCTracklist_all[l]);
							//fhMCtest->Fill(invmass);
							fhInvariantMass_MC_all->Fill(invmass);
							if(mcGrandmotherPdg1 == 111) {
								fhInvariantMass_MC_pi0->Fill(invmass);
								fhInvariantMass_MC_pi0_gg->Fill(invmass);
							}
							if(mcGrandmotherPdg1 == 221) {
								fhInvariantMass_MC_eta->Fill(invmass);
							}
						}
						
						
						// decay pi0 -> g e+ e- -> e+ e- e+ e-
						if(  ((motherId1 == motherId2 && motherId3 == motherId4) && (mcMotherPdg1 == 22) && (mcMotherPdg3 == 111) && grandmotherId1 == motherId3)
						  || ((motherId1 == motherId2 && motherId3 == motherId4) && (mcMotherPdg1 == 111) && (mcMotherPdg3 == 22) && grandmotherId3 == motherId1)
						  || ((motherId1 == motherId3 && motherId2 == motherId4) && (mcMotherPdg1 == 22) && (mcMotherPdg2 == 111) && grandmotherId1 == motherId2)
						  || ((motherId1 == motherId3 && motherId2 == motherId4) && (mcMotherPdg1 == 111) && (mcMotherPdg2 == 22) && grandmotherId2 == motherId1)
						  || ((motherId1 == motherId4 && motherId2 == motherId3) && (mcMotherPdg1 == 22) && (mcMotherPdg2 == 111) && grandmotherId1 == motherId2)
						  || ((motherId1 == motherId4 && motherId2 == motherId3) && (mcMotherPdg1 == 111) && (mcMotherPdg2 == 22) && grandmotherId2 == motherId1)) {
							Double_t invmass = Invmass_4particles(fMCTracklist_all[i], fMCTracklist_all[j], fMCTracklist_all[k], fMCTracklist_all[l]);
							fhInvariantMass_MC_pi0_gepem->Fill(invmass);
							fhInvariantMass_MC_pi0->Fill(invmass);
						}
						// decay eta -> g e+ e- -> e+ e- e+ e-
						if(  ((motherId1 == motherId2 && motherId3 == motherId4) && (mcMotherPdg1 == 22) && (mcMotherPdg3 == 221) && grandmotherId1 == motherId3)
						  || ((motherId1 == motherId2 && motherId3 == motherId4) && (mcMotherPdg1 == 221) && (mcMotherPdg3 == 22) && grandmotherId3 == motherId1)
						  || ((motherId1 == motherId3 && motherId2 == motherId4) && (mcMotherPdg1 == 22) && (mcMotherPdg2 == 221) && grandmotherId1 == motherId2)
						  || ((motherId1 == motherId3 && motherId2 == motherId4) && (mcMotherPdg1 == 221) && (mcMotherPdg2 == 22) && grandmotherId2 == motherId1)
						  || ((motherId1 == motherId4 && motherId2 == motherId3) && (mcMotherPdg1 == 22) && (mcMotherPdg2 == 221) && grandmotherId1 == motherId2)
						  || ((motherId1 == motherId4 && motherId2 == motherId3) && (mcMotherPdg1 == 221) && (mcMotherPdg2 == 22) && grandmotherId2 == motherId1)) {
							Double_t invmass = Invmass_4particles(fMCTracklist_all[i], fMCTracklist_all[j], fMCTracklist_all[k], fMCTracklist_all[l]);
							fhInvariantMass_MC_eta->Fill(invmass);
						}
					}
				}
			}
		}
	}
	cout << "InvariantMassTestMC - End!" << endl;

	timer.Stop();
	fTime += timer.RealTime();
}



Double_t CbmAnaConversionReco::Invmass_4particles(const CbmMCTrack* mctrack1, const CbmMCTrack* mctrack2, const CbmMCTrack* mctrack3, const CbmMCTrack* mctrack4)
// calculation of invariant mass from four electrons/positrons
{
/*    TVector3 mom1;
    mctrack1->GetMomentum(mom1);
    TVector3 tempmom1;
    tempmom1.SetX(SmearValue(mom1.X()));
    tempmom1.SetY(SmearValue(mom1.Y()));
    tempmom1.SetZ(SmearValue(mom1.Z()));
    Double_t energy1 = TMath::Sqrt(tempmom1.Mag2() + M2E);
    TLorentzVector lorVec1(tempmom1, energy1);

    TVector3 mom2;
    mctrack2->GetMomentum(mom2);
    TVector3 tempmom2;
    tempmom2.SetX(SmearValue(mom2.X()));
    tempmom2.SetY(SmearValue(mom2.Y()));
    tempmom2.SetZ(SmearValue(mom2.Z()));
    Double_t energy2 = TMath::Sqrt(tempmom2.Mag2() + M2E);
    TLorentzVector lorVec2(tempmom2, energy2);

    TVector3 mom3;
    mctrack3->GetMomentum(mom3);
    TVector3 tempmom3;
    tempmom3.SetX(SmearValue(mom3.X()));
    tempmom3.SetY(SmearValue(mom3.Y()));
    tempmom3.SetZ(SmearValue(mom3.Z()));
    Double_t energy3 = TMath::Sqrt(tempmom3.Mag2() + M2E);
    TLorentzVector lorVec3(tempmom3, energy3);

    TVector3 mom4;
    mctrack4->GetMomentum(mom4);
    TVector3 tempmom4;
    tempmom4.SetX(SmearValue(mom4.X()));
    tempmom4.SetY(SmearValue(mom4.Y()));
    tempmom4.SetZ(SmearValue(mom4.Z()));
    Double_t energy4 = TMath::Sqrt(tempmom4.Mag2() + M2E);
    TLorentzVector lorVec4(tempmom4, energy4);
*/    
    TLorentzVector lorVec1;
    mctrack1->Get4Momentum(lorVec1);
    
    TLorentzVector lorVec2;
    mctrack2->Get4Momentum(lorVec2);
    
    TLorentzVector lorVec3;
    mctrack3->Get4Momentum(lorVec3);
    
    TLorentzVector lorVec4;
    mctrack4->Get4Momentum(lorVec4);
    
    
    TLorentzVector sum;
    sum = lorVec1 + lorVec2 + lorVec3 + lorVec4;
    cout << "mc: \t" << sum.Px() << " / " << sum.Py() << " / " << sum.Pz() << " / " << sum.E() << "\t => mag = " << sum.Mag() << endl;
    

	return sum.Mag();
}


Double_t CbmAnaConversionReco::SmearValue(Double_t value) 
{
	TRandom3 generator(0);
	Double_t result = 0;
	Double_t smear = 0;
	Int_t plusminus = 0;
	while (plusminus == 0) { // should be either 1 or -1, not 0
		plusminus = generator.Uniform(-2, 2);
	}
	Double_t gaus = generator.Gaus(1,1);
	smear = gaus * plusminus;
//	result = value * (1. + 1.0*smear/100);		//smearing as wished
	
	result = value;		// -> no smearing
	
	return result;
}




Double_t CbmAnaConversionReco::Invmass_4particlesRECO(const TVector3 part1, const TVector3 part2, const TVector3 part3, const TVector3 part4)
// calculation of invariant mass from four electrons/positrons
{
    Double_t energy1 = TMath::Sqrt(part1.Mag2() + M2E);
    TLorentzVector lorVec1(part1, energy1);

    Double_t energy2 = TMath::Sqrt(part2.Mag2() + M2E);
    TLorentzVector lorVec2(part2, energy2);

    Double_t energy3 = TMath::Sqrt(part3.Mag2() + M2E);
    TLorentzVector lorVec3(part3, energy3);

    Double_t energy4 = TMath::Sqrt(part4.Mag2() + M2E);
    TLorentzVector lorVec4(part4, energy4);
    
    TLorentzVector sum;
    sum = lorVec1 + lorVec2 + lorVec3 + lorVec4;
    //cout << "reco: \t" << sum.Px() << " / " << sum.Py() << " / " << sum.Pz() << " / " << sum.E() << "\t => mag = " << sum.Mag() << endl;
    

	return sum.Mag();
}







void CbmAnaConversionReco::InvariantMassTest_4epem()
// Calculating invariant mass of 4 ep/em, using MC data AND reconstructed momentum
{
	timer.Start();


	cout << "InvariantMassTest_4epem - Start..." << endl;
	cout << "InvariantMassTest_4epem - " << fRecoTracklistEPEM.size() << "\t" << fRecoMomentum.size() << endl;
	int fill = 0;
	for(int i=0; i<fRecoTracklistEPEM.size(); i++) {
		for(int j=i+1; j<fRecoTracklistEPEM.size(); j++) {
			for(int k=j+1; k<fRecoTracklistEPEM.size(); k++) {
				for(int l=k+1; l<fRecoTracklistEPEM.size(); l++) {
					if(fRecoTracklistEPEM[i]->GetPdgCode() + fRecoTracklistEPEM[j]->GetPdgCode() + fRecoTracklistEPEM[k]->GetPdgCode() + fRecoTracklistEPEM[l]->GetPdgCode() != 0) continue;
					
					if(fRecoTracklistEPEM.size() != fRecoMomentum.size() || fRecoTracklistEPEM.size() != fRecoRefittedMomentum.size()) {
						cout << "InvariantMassTest_4epem - not matching number of entries!" << endl;
						continue;
					}
					
					
					int motherId1 = fRecoTracklistEPEM[i]->GetMotherId();
					int motherId2 = fRecoTracklistEPEM[j]->GetMotherId();
					int motherId3 = fRecoTracklistEPEM[k]->GetMotherId();
					int motherId4 = fRecoTracklistEPEM[l]->GetMotherId();
					
					// decay pi0 -> e+ e- e+ e-
					if(motherId1 == motherId2 && motherId1 == motherId3 && motherId1 == motherId4) {
						if (motherId1 != -1) {
							int mcMotherPdg1  = -1;
							CbmMCTrack* mother1 = (CbmMCTrack*) fMcTracks->At(motherId1);
							if(NULL != mother1) mcMotherPdg1 = mother1->GetPdgCode();
							if( (mcMotherPdg1 == 111 || mcMotherPdg1 == 221) ) { // && NofDaughters(motherId1) == 4) {
								Double_t invmass1 = 0;
								Double_t invmass2 = 0;	// momenta from stsMomentumVec
								Double_t invmass3 = 0;
							//	invmass2 = Invmass_4particlesRECO(fRecoMomentum[i], fRecoMomentum[j], fRecoMomentum[k], fRecoMomentum[l]);
								invmass1 = Invmass_4particles(fRecoTracklistEPEM[i], fRecoTracklistEPEM[j], fRecoTracklistEPEM[k], fRecoTracklistEPEM[l]);	// true MC values
								invmass3 = Invmass_4particlesRECO(fRecoRefittedMomentum[i], fRecoRefittedMomentum[j], fRecoRefittedMomentum[k], fRecoRefittedMomentum[l]);
							//	fhInvariantMass_pi0epem->Fill(invmass2);
								cout << "Decay pi0 -> e+e-e+e- detected!\t\t mc mass: " << invmass1 << "\t, reco mass: " << invmass3 << endl;
								cout << "motherids: " << motherId1 << "/" << motherId2 << "/" << motherId3 << "/" << motherId4 << "\t motherpdg: " << mcMotherPdg1 << "\t mctrack mass: " << mother1->GetMass() << "\t nofdaughters: " << NofDaughters(motherId1) << endl;
								cout << "pdgs " << fRecoTracklistEPEM[i]->GetPdgCode() << "/" << fRecoTracklistEPEM[j]->GetPdgCode() << "/" << fRecoTracklistEPEM[k]->GetPdgCode() << "/" << fRecoTracklistEPEM[l]->GetPdgCode() << endl;
								TVector3 start1;
								fRecoTracklistEPEM[i]->GetStartVertex(start1);
								TVector3 start2;
								fRecoTracklistEPEM[j]->GetStartVertex(start2);
								TVector3 start3;
								fRecoTracklistEPEM[k]->GetStartVertex(start3);
								TVector3 start4;
								fRecoTracklistEPEM[l]->GetStartVertex(start4);
								cout << "start: " << start1.Z() << "/" << start2.Z() << "/" << start3.Z() << "/" << start4.Z() << endl;
								
								fhEPEM_invmass_eeee_mc->Fill(invmass1);
								fhEPEM_invmass_eeee_refitted->Fill(invmass3);
								fhEPEM_invmass_all_mc->Fill(invmass1);
								fhEPEM_invmass_all_refitted->Fill(invmass3);
							}
						}
						else {	// all 4 particles come directly from the vertex
							continue;
						}
					}
					
					if( (motherId1 == motherId2 && motherId3 == motherId4) ||
						(motherId1 == motherId3 && motherId2 == motherId4) ||
						(motherId1 == motherId4 && motherId2 == motherId3) ) {


						int grandmotherId1 = -1;
						int grandmotherId2 = -1;
						int grandmotherId3 = -1;
						int grandmotherId4 = -1;

						int mcMotherPdg1  = -1;
						int mcMotherPdg2  = -1;
						int mcMotherPdg3  = -1;
						int mcMotherPdg4  = -1;
						int mcGrandmotherPdg1  = -1;
						int mcGrandmotherPdg2  = -1;
						int mcGrandmotherPdg3  = -1;
						int mcGrandmotherPdg4  = -1;

						CbmMCTrack* grandmother1;

						if (motherId1 != -1) {
							CbmMCTrack* mother1 = (CbmMCTrack*) fMcTracks->At(motherId1);
							if (NULL != mother1) mcMotherPdg1 = mother1->GetPdgCode();
							grandmotherId1 = mother1->GetMotherId();
							if(grandmotherId1 != -1) {
								grandmother1 = (CbmMCTrack*) fMcTracks->At(grandmotherId1);
								if (NULL != grandmother1) mcGrandmotherPdg1 = grandmother1->GetPdgCode();
							}
						}
						if (motherId2 != -1) {
							CbmMCTrack* mother2 = (CbmMCTrack*) fMcTracks->At(motherId2);
							if (NULL != mother2) mcMotherPdg2 = mother2->GetPdgCode();
							grandmotherId2 = mother2->GetMotherId();
							if(grandmotherId2 != -1) {
								CbmMCTrack* grandmother2 = (CbmMCTrack*) fMcTracks->At(grandmotherId2);
								if (NULL != grandmother2) mcGrandmotherPdg2 = grandmother2->GetPdgCode();
							}
						}
						if (motherId3 != -1) {
							CbmMCTrack* mother3 = (CbmMCTrack*) fMcTracks->At(motherId3);
							if (NULL != mother3) mcMotherPdg3 = mother3->GetPdgCode();
							grandmotherId3 = mother3->GetMotherId();
							if(grandmotherId3 != -1) {
								CbmMCTrack* grandmother3 = (CbmMCTrack*) fMcTracks->At(grandmotherId3);
								if (NULL != grandmother3) mcGrandmotherPdg3 = grandmother3->GetPdgCode();
							}
						}
						if (motherId4 != -1) {
							CbmMCTrack* mother4 = (CbmMCTrack*) fMcTracks->At(motherId4);
							if (NULL != mother4) mcMotherPdg4 = mother4->GetPdgCode();
							grandmotherId4 = mother4->GetMotherId();
							if(grandmotherId4 != -1) {
								CbmMCTrack* grandmother4 = (CbmMCTrack*) fMcTracks->At(grandmotherId4);
								if (NULL != grandmother4) mcGrandmotherPdg4 = grandmother4->GetPdgCode();
							}
						}

						if(grandmotherId1 == -1) continue;

						if(motherId1 == motherId2 && motherId3 == motherId4) {
							if(NofDaughters(motherId1) != 2 || NofDaughters(motherId3) != 2) continue;
							if( (grandmotherId1 == motherId3 && mcGrandmotherPdg1 == 111) || (motherId1 == grandmotherId3 && mcMotherPdg1 == 111)) {
								cout << "HURRAY! .-.-.-.-.-.-..-.-.-.-.-.-.-.-.-.-.-.-." << endl;
								Double_t invmass1 = 0;
								Double_t invmass3 = 0;
								invmass1 = Invmass_4particles(fRecoTracklistEPEM[i], fRecoTracklistEPEM[j], fRecoTracklistEPEM[k], fRecoTracklistEPEM[l]);	// true MC values
								invmass3 = Invmass_4particlesRECO(fRecoRefittedMomentum[i], fRecoRefittedMomentum[j], fRecoRefittedMomentum[k], fRecoRefittedMomentum[l]);
								fhInvariantMass_pi0epem->Fill(invmass1);
								
								fhEPEM_invmass_gee_mc->Fill(invmass1);
								fhEPEM_invmass_gee_refitted->Fill(invmass3);
								fhEPEM_invmass_all_mc->Fill(invmass1);
								fhEPEM_invmass_all_refitted->Fill(invmass3);
								
								if(mcGrandmotherPdg1 == 111) {
									Double_t opening_angle1_mc = 0;
									Double_t opening_angle1_refitted = 0;
									opening_angle1_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[i], fRecoTracklistEPEM[j]);
									opening_angle1_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[i], fRecoRefittedMomentum[j]);
									fhEPEM_openingAngle_gee_mc->Fill(opening_angle1_mc);
									fhEPEM_openingAngle_gee_refitted->Fill(opening_angle1_refitted);
								}
								
								if(mcMotherPdg1 == 111) {
									Double_t opening_angle1_mc = 0;
									Double_t opening_angle1_refitted = 0;
									opening_angle1_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[k], fRecoTracklistEPEM[l]);
									opening_angle1_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[k], fRecoRefittedMomentum[l]);
									fhEPEM_openingAngle_gee_mc->Fill(opening_angle1_mc);
									fhEPEM_openingAngle_gee_refitted->Fill(opening_angle1_refitted);
								}
							}
						}
						if(motherId1 == motherId3 && motherId2 == motherId4) {
							if(NofDaughters(motherId1) != 2 || NofDaughters(motherId2) != 2) continue;
							if( (grandmotherId1 == motherId2 && mcGrandmotherPdg1 == 111) || (motherId1 == grandmotherId2 && mcMotherPdg1 == 111)) {
								cout << "HURRAY! .-.-.-.-.-.-..-.-.-.-.-.-.-.-.-.-.-.-." << endl;
								Double_t invmass1 = 0;
								Double_t invmass3 = 0;
								invmass1 = Invmass_4particles(fRecoTracklistEPEM[i], fRecoTracklistEPEM[j], fRecoTracklistEPEM[k], fRecoTracklistEPEM[l]);	// true MC values
								invmass3 = Invmass_4particlesRECO(fRecoRefittedMomentum[i], fRecoRefittedMomentum[j], fRecoRefittedMomentum[k], fRecoRefittedMomentum[l]);
								fhInvariantMass_pi0epem->Fill(invmass1);
								
								fhEPEM_invmass_gee_mc->Fill(invmass1);
								fhEPEM_invmass_gee_refitted->Fill(invmass3);
								fhEPEM_invmass_all_mc->Fill(invmass1);
								fhEPEM_invmass_all_refitted->Fill(invmass3);
								
								if(mcGrandmotherPdg1 == 111) {
									Double_t opening_angle1_mc = 0;
									Double_t opening_angle1_refitted = 0;
									opening_angle1_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[i], fRecoTracklistEPEM[k]);
									opening_angle1_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[i], fRecoRefittedMomentum[k]);
									fhEPEM_openingAngle_gee_mc->Fill(opening_angle1_mc);
									fhEPEM_openingAngle_gee_refitted->Fill(opening_angle1_refitted);
								}
								
								if(mcMotherPdg1 == 111) {
									Double_t opening_angle1_mc = 0;
									Double_t opening_angle1_refitted = 0;
									opening_angle1_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[j], fRecoTracklistEPEM[l]);
									opening_angle1_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[j], fRecoRefittedMomentum[l]);
									fhEPEM_openingAngle_gee_mc->Fill(opening_angle1_mc);
									fhEPEM_openingAngle_gee_refitted->Fill(opening_angle1_refitted);
								}
							}
						}
						if(motherId1 == motherId4 && motherId2 == motherId3) {
							if(NofDaughters(motherId1) != 2 || NofDaughters(motherId2) != 2) continue;
							if( (grandmotherId1 == motherId2 && mcGrandmotherPdg1 == 111) || (motherId1 == grandmotherId2 && mcMotherPdg1 == 111)) {
								cout << "HURRAY! .-.-.-.-.-.-..-.-.-.-.-.-.-.-.-.-.-.-." << endl;
								Double_t invmass1 = 0;
								Double_t invmass3 = 0;
								invmass1 = Invmass_4particles(fRecoTracklistEPEM[i], fRecoTracklistEPEM[j], fRecoTracklistEPEM[k], fRecoTracklistEPEM[l]);	// true MC values
								invmass3 = Invmass_4particlesRECO(fRecoRefittedMomentum[i], fRecoRefittedMomentum[j], fRecoRefittedMomentum[k], fRecoRefittedMomentum[l]);
								fhInvariantMass_pi0epem->Fill(invmass1);
								
								fhEPEM_invmass_gee_mc->Fill(invmass1);
								fhEPEM_invmass_gee_refitted->Fill(invmass3);
								fhEPEM_invmass_all_mc->Fill(invmass1);
								fhEPEM_invmass_all_refitted->Fill(invmass3);
								
								if(mcGrandmotherPdg1 == 111) {
									Double_t opening_angle1_mc = 0;
									Double_t opening_angle1_refitted = 0;
									opening_angle1_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[i], fRecoTracklistEPEM[l]);
									opening_angle1_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[i], fRecoRefittedMomentum[l]);
									fhEPEM_openingAngle_gee_mc->Fill(opening_angle1_mc);
									fhEPEM_openingAngle_gee_refitted->Fill(opening_angle1_refitted);
								}
								
								if(mcMotherPdg1 == 111) {
									Double_t opening_angle1_mc = 0;
									Double_t opening_angle1_refitted = 0;
									opening_angle1_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[j], fRecoTracklistEPEM[k]);
									opening_angle1_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[j], fRecoRefittedMomentum[k]);
									fhEPEM_openingAngle_gee_mc->Fill(opening_angle1_mc);
									fhEPEM_openingAngle_gee_refitted->Fill(opening_angle1_refitted);
								}
							}
						}




					// ===================================================================================================
					// HERE DECAY pi0 -> gamma gamma -> e+e- e+e-
					if(grandmotherId1 == grandmotherId2 && grandmotherId1 == grandmotherId3 && grandmotherId1 == grandmotherId4) {
				//		if(mcGrandmotherPdg1 != 111 && mcGrandmotherPdg1 != 221) continue; // 111 = pi0, 221 = eta
						if(mcGrandmotherPdg1 != 111) continue; // 111 = pi0, 221 = eta

						TVector3 pi0start;
						grandmother1->GetStartVertex(pi0start);
						fhPi0_startvertex->Fill(pi0start.Z());
						
						TVector3 pi0start_i;
						fRecoTracklistEPEM[i]->GetStartVertex(pi0start_i);
						fhPi0_startvertexElectrons->Fill(pi0start_i.Z());
						TVector3 pi0start_j;
						fRecoTracklistEPEM[j]->GetStartVertex(pi0start_j);
						fhPi0_startvertexElectrons->Fill(pi0start_j.Z());
						TVector3 pi0start_k;
						fRecoTracklistEPEM[k]->GetStartVertex(pi0start_k);
						fhPi0_startvertexElectrons->Fill(pi0start_k.Z());
						TVector3 pi0start_l;
						fRecoTracklistEPEM[l]->GetStartVertex(pi0start_l);
						fhPi0_startvertexElectrons->Fill(pi0start_l.Z());
						
						if(pi0start_i.Z() > 1 || pi0start_j.Z() > 1 || pi0start_k.Z() > 1 || pi0start_l.Z() > 1) continue;

						Double_t invmass1 = 0;	// true MC values
						invmass1 = Invmass_4particles(fRecoTracklistEPEM[i], fRecoTracklistEPEM[j], fRecoTracklistEPEM[k], fRecoTracklistEPEM[l]);
						Double_t invmass2 = 0;	// momenta from stsMomentumVec
						invmass2 = Invmass_4particlesRECO(fRecoMomentum[i], fRecoMomentum[j], fRecoMomentum[k], fRecoMomentum[l]);
						Double_t invmass3 = 0;	// momenta from refitted at primary vertex
						invmass3 = Invmass_4particlesRECO(fRecoRefittedMomentum[i], fRecoRefittedMomentum[j], fRecoRefittedMomentum[k], fRecoRefittedMomentum[l]);

						cout << "######################################################################" << endl;
						cout << fRecoMomentum[i].X() << "\t" << fRecoRefittedMomentum[i].X() << endl;
						cout << fRecoMomentum[i].Y() << "\t" << fRecoRefittedMomentum[i].Y() << endl;
						cout << fRecoMomentum[i].Z() << "\t" << fRecoRefittedMomentum[i].Z() << endl;

						if(fRecoTracklistEPEM_ids.size() != fRecoTracklistEPEM.size()) {
							cout << "size mismatch! " << fRecoTracklistEPEM_ids.size() << "/" << fRecoTracklistEPEM.size() << endl;
						}

						cout << "######################################################################" << endl;
						cout << "index: " << i << "\t" << j << "\t" << k << "\t" << l << endl;
						cout << "mc id: " << fRecoTracklistEPEM_ids[i] << "\t" << fRecoTracklistEPEM_ids[j] << "\t" << fRecoTracklistEPEM_ids[k] << "\t" << fRecoTracklistEPEM_ids[l] << endl;
						cout << "motherid: " << motherId1 << "\t" << motherId2 << "\t" << motherId3 << "\t" << motherId4 << endl;
						cout << "motherpdg: " << mcMotherPdg1 << "\t" << mcMotherPdg2 << "\t" << mcMotherPdg3 << "\t" << mcMotherPdg4 << endl;
						cout << "grandmotherid: " << grandmotherId1 << "\t" << grandmotherId2 << "\t" << grandmotherId3 << "\t" << grandmotherId4 << endl;
						cout << "pdg: " << fRecoTracklistEPEM[i]->GetPdgCode() << "\t" << fRecoTracklistEPEM[j]->GetPdgCode() << "\t" << fRecoTracklistEPEM[k]->GetPdgCode() << "\t" << fRecoTracklistEPEM[l]->GetPdgCode() << endl;
						cout << "invmass reco: " << invmass2 << "\t invmass mc: " << invmass1 << endl;
					
						fhInvMass_EPEM_mc->Fill(invmass1);
						fhInvMass_EPEM_stsMomVec->Fill(invmass2);
						fhInvMass_EPEM_refitted->Fill(invmass3);
						fhInvMass_EPEM_error_stsMomVec->Fill(1.0*TMath::Abs(invmass1-invmass2)/invmass1);
						fhInvMass_EPEM_error_refitted->Fill(1.0*TMath::Abs(invmass1-invmass3)/invmass1);
					
						fhUsedMomenta_stsMomVec->Fill(fRecoMomentum[i].Mag());
						fhUsedMomenta_stsMomVec->Fill(fRecoMomentum[j].Mag());
						fhUsedMomenta_stsMomVec->Fill(fRecoMomentum[k].Mag());
						fhUsedMomenta_stsMomVec->Fill(fRecoMomentum[l].Mag());
					
						fhUsedMomenta_mc->Fill(fRecoTracklistEPEM[i]->GetP());
						fhUsedMomenta_mc->Fill(fRecoTracklistEPEM[j]->GetP());
						fhUsedMomenta_mc->Fill(fRecoTracklistEPEM[k]->GetP());
						fhUsedMomenta_mc->Fill(fRecoTracklistEPEM[l]->GetP());
					
						fhUsedMomenta_error_stsMomVec->Fill(TMath::Abs(fRecoTracklistEPEM[i]->GetP() - fRecoMomentum[i].Mag())/fRecoTracklistEPEM[i]->GetP());
						fhUsedMomenta_error_stsMomVec->Fill(TMath::Abs(fRecoTracklistEPEM[j]->GetP() - fRecoMomentum[j].Mag())/fRecoTracklistEPEM[j]->GetP());
						fhUsedMomenta_error_stsMomVec->Fill(TMath::Abs(fRecoTracklistEPEM[k]->GetP() - fRecoMomentum[k].Mag())/fRecoTracklistEPEM[k]->GetP());
						fhUsedMomenta_error_stsMomVec->Fill(TMath::Abs(fRecoTracklistEPEM[l]->GetP() - fRecoMomentum[l].Mag())/fRecoTracklistEPEM[l]->GetP());
					
					//	fhUsedMomenta_error_refitted->Fill(TMath::Abs(fRecoRefittedMomentum[i].Mag() - fRecoMomentum[i].Mag())/fRecoRefittedMomentum[i].Mag());
					//	fhUsedMomenta_error_refitted->Fill(TMath::Abs(fRecoRefittedMomentum[j].Mag() - fRecoMomentum[j].Mag())/fRecoRefittedMomentum[j].Mag());
					//	fhUsedMomenta_error_refitted->Fill(TMath::Abs(fRecoRefittedMomentum[k].Mag() - fRecoMomentum[k].Mag())/fRecoRefittedMomentum[k].Mag());
					//	fhUsedMomenta_error_refitted->Fill(TMath::Abs(fRecoRefittedMomentum[l].Mag() - fRecoMomentum[l].Mag())/fRecoRefittedMomentum[l].Mag());
		
						fhUsedMomenta_error_refitted->Fill(TMath::Abs(fRecoTracklistEPEM[i]->GetP() - fRecoRefittedMomentum[i].Mag())/fRecoTracklistEPEM[i]->GetP());
						fhUsedMomenta_error_refitted->Fill(TMath::Abs(fRecoTracklistEPEM[j]->GetP() - fRecoRefittedMomentum[j].Mag())/fRecoTracklistEPEM[j]->GetP());
						fhUsedMomenta_error_refitted->Fill(TMath::Abs(fRecoTracklistEPEM[k]->GetP() - fRecoRefittedMomentum[k].Mag())/fRecoTracklistEPEM[k]->GetP());
						fhUsedMomenta_error_refitted->Fill(TMath::Abs(fRecoTracklistEPEM[l]->GetP() - fRecoRefittedMomentum[l].Mag())/fRecoTracklistEPEM[l]->GetP());


						TVector3 momentumtest5a;
						fRecoTracklistEPEM[i]->GetMomentum(momentumtest5a);
						fhUsedMomenta_errorX_stsMomVec->Fill(TMath::Abs(momentumtest5a.X() - fRecoMomentum[i].X())/momentumtest5a.X());
						fhUsedMomenta_errorY_stsMomVec->Fill(TMath::Abs(momentumtest5a.Y() - fRecoMomentum[i].Y())/momentumtest5a.Y());
						fhUsedMomenta_errorZ_stsMomVec->Fill(TMath::Abs(momentumtest5a.Z() - fRecoMomentum[i].Z())/momentumtest5a.Z());
						fhUsedMomenta_vsX_stsMomVec->Fill(momentumtest5a.X(), fRecoMomentum[i].X());
						fhUsedMomenta_vsY_stsMomVec->Fill(momentumtest5a.Y(), fRecoMomentum[i].Y());
						fhUsedMomenta_vsZ_stsMomVec->Fill(momentumtest5a.Z(), fRecoMomentum[i].Z());
						fhUsedMomenta_errorX_refitted->Fill(TMath::Abs(momentumtest5a.X() - fRecoRefittedMomentum[i].X())/momentumtest5a.X());
						fhUsedMomenta_errorY_refitted->Fill(TMath::Abs(momentumtest5a.Y() - fRecoRefittedMomentum[i].Y())/momentumtest5a.Y());
						fhUsedMomenta_errorZ_refitted->Fill(TMath::Abs(momentumtest5a.Z() - fRecoRefittedMomentum[i].Z())/momentumtest5a.Z());
						fhUsedMomenta_vsX_refitted->Fill(momentumtest5a.X(), fRecoRefittedMomentum[i].X());
						fhUsedMomenta_vsY_refitted->Fill(momentumtest5a.Y(), fRecoRefittedMomentum[i].Y());
						fhUsedMomenta_vsZ_refitted->Fill(momentumtest5a.Z(), fRecoRefittedMomentum[i].Z());

						TVector3 momentumtest5b;
						fRecoTracklistEPEM[j]->GetMomentum(momentumtest5b);
						fhUsedMomenta_errorX_stsMomVec->Fill(TMath::Abs(momentumtest5b.X() - fRecoMomentum[j].X())/momentumtest5b.X());
						fhUsedMomenta_errorY_stsMomVec->Fill(TMath::Abs(momentumtest5b.Y() - fRecoMomentum[j].Y())/momentumtest5b.Y());
						fhUsedMomenta_errorZ_stsMomVec->Fill(TMath::Abs(momentumtest5b.Z() - fRecoMomentum[j].Z())/momentumtest5b.Z());
						fhUsedMomenta_vsX_stsMomVec->Fill(momentumtest5b.X(), fRecoMomentum[j].X());
						fhUsedMomenta_vsY_stsMomVec->Fill(momentumtest5b.Y(), fRecoMomentum[j].Y());
						fhUsedMomenta_vsZ_stsMomVec->Fill(momentumtest5b.Z(), fRecoMomentum[j].Z());
						fhUsedMomenta_errorX_refitted->Fill(TMath::Abs(momentumtest5b.X() - fRecoRefittedMomentum[j].X())/momentumtest5b.X());
						fhUsedMomenta_errorY_refitted->Fill(TMath::Abs(momentumtest5b.Y() - fRecoRefittedMomentum[j].Y())/momentumtest5b.Y());
						fhUsedMomenta_errorZ_refitted->Fill(TMath::Abs(momentumtest5b.Z() - fRecoRefittedMomentum[j].Z())/momentumtest5b.Z());
						fhUsedMomenta_vsX_refitted->Fill(momentumtest5b.X(), fRecoRefittedMomentum[j].X());
						fhUsedMomenta_vsY_refitted->Fill(momentumtest5b.Y(), fRecoRefittedMomentum[j].Y());
						fhUsedMomenta_vsZ_refitted->Fill(momentumtest5b.Z(), fRecoRefittedMomentum[j].Z());

						TVector3 momentumtest5c;
						fRecoTracklistEPEM[k]->GetMomentum(momentumtest5c);
						fhUsedMomenta_errorX_stsMomVec->Fill(TMath::Abs(momentumtest5c.X() - fRecoMomentum[k].X())/momentumtest5c.X());
						fhUsedMomenta_errorY_stsMomVec->Fill(TMath::Abs(momentumtest5c.Y() - fRecoMomentum[k].Y())/momentumtest5c.Y());
						fhUsedMomenta_errorZ_stsMomVec->Fill(TMath::Abs(momentumtest5c.Z() - fRecoMomentum[k].Z())/momentumtest5c.Z());
						fhUsedMomenta_vsX_stsMomVec->Fill(momentumtest5c.X(), fRecoMomentum[k].X());
						fhUsedMomenta_vsY_stsMomVec->Fill(momentumtest5c.Y(), fRecoMomentum[k].Y());
						fhUsedMomenta_vsZ_stsMomVec->Fill(momentumtest5c.Z(), fRecoMomentum[k].Z());
						fhUsedMomenta_errorX_refitted->Fill(TMath::Abs(momentumtest5c.X() - fRecoRefittedMomentum[k].X())/momentumtest5c.X());
						fhUsedMomenta_errorY_refitted->Fill(TMath::Abs(momentumtest5c.Y() - fRecoRefittedMomentum[k].Y())/momentumtest5c.Y());
						fhUsedMomenta_errorZ_refitted->Fill(TMath::Abs(momentumtest5c.Z() - fRecoRefittedMomentum[k].Z())/momentumtest5c.Z());
						fhUsedMomenta_vsX_refitted->Fill(momentumtest5c.X(), fRecoRefittedMomentum[k].X());
						fhUsedMomenta_vsY_refitted->Fill(momentumtest5c.Y(), fRecoRefittedMomentum[k].Y());
						fhUsedMomenta_vsZ_refitted->Fill(momentumtest5c.Z(), fRecoRefittedMomentum[k].Z());

						TVector3 momentumtest5d;
						fRecoTracklistEPEM[l]->GetMomentum(momentumtest5d);
						fhUsedMomenta_errorX_stsMomVec->Fill(TMath::Abs(momentumtest5d.X() - fRecoMomentum[l].X())/momentumtest5d.X());
						fhUsedMomenta_errorY_stsMomVec->Fill(TMath::Abs(momentumtest5d.Y() - fRecoMomentum[l].Y())/momentumtest5d.Y());
						fhUsedMomenta_errorZ_stsMomVec->Fill(TMath::Abs(momentumtest5d.Z() - fRecoMomentum[l].Z())/momentumtest5d.Z());
						fhUsedMomenta_vsX_stsMomVec->Fill(momentumtest5d.X(), fRecoMomentum[l].X());
						fhUsedMomenta_vsY_stsMomVec->Fill(momentumtest5d.Y(), fRecoMomentum[l].Y());
						fhUsedMomenta_vsZ_stsMomVec->Fill(momentumtest5d.Z(), fRecoMomentum[l].Z());
						fhUsedMomenta_errorX_refitted->Fill(TMath::Abs(momentumtest5d.X() - fRecoRefittedMomentum[l].X())/momentumtest5d.X());
						fhUsedMomenta_errorY_refitted->Fill(TMath::Abs(momentumtest5d.Y() - fRecoRefittedMomentum[l].Y())/momentumtest5d.Y());
						fhUsedMomenta_errorZ_refitted->Fill(TMath::Abs(momentumtest5d.Z() - fRecoRefittedMomentum[l].Z())/momentumtest5d.Z());
						fhUsedMomenta_vsX_refitted->Fill(momentumtest5d.X(), fRecoRefittedMomentum[l].X());
						fhUsedMomenta_vsY_refitted->Fill(momentumtest5d.Y(), fRecoRefittedMomentum[l].Y());
						fhUsedMomenta_vsZ_refitted->Fill(momentumtest5d.Z(), fRecoRefittedMomentum[l].Z());
						
						
						Double_t opening_angle1_mc = 0;
						Double_t opening_angle2_mc = 0;
						Double_t opening_angle1_refitted = 0;
						Double_t opening_angle2_refitted = 0;
						
						if(motherId1 == motherId2) {
							opening_angle1_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[i], fRecoTracklistEPEM[j]);
							opening_angle2_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[k], fRecoTracklistEPEM[l]);
							opening_angle1_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[i], fRecoRefittedMomentum[j]);
							opening_angle2_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[k], fRecoRefittedMomentum[l]);
						}
						if(motherId1 == motherId3) {
							opening_angle1_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[i], fRecoTracklistEPEM[k]);
							opening_angle2_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[j], fRecoTracklistEPEM[l]);
							opening_angle1_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[i], fRecoRefittedMomentum[k]);
							opening_angle2_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[j], fRecoRefittedMomentum[l]);
						}
						if(motherId1 == motherId4) {
							opening_angle1_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[i], fRecoTracklistEPEM[l]);
							opening_angle2_mc = CalculateOpeningAngleMC(fRecoTracklistEPEM[j], fRecoTracklistEPEM[k]);
							opening_angle1_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[i], fRecoRefittedMomentum[l]);
							opening_angle2_refitted = CalculateOpeningAngleReco(fRecoRefittedMomentum[j], fRecoRefittedMomentum[k]);
						}
						
						
						fhInvMass_EPEM_openingAngleRef->Fill(opening_angle1_refitted);
						fhInvMass_EPEM_openingAngleRef->Fill(opening_angle2_refitted);
						
						
						
						fhEPEM_invmass_gg_mc->Fill(invmass1);
						fhEPEM_invmass_gg_refitted->Fill(invmass3);
						fhEPEM_invmass_all_mc->Fill(invmass1);
						fhEPEM_invmass_all_refitted->Fill(invmass3);
						fhEPEM_openingAngle_gg_mc->Fill(opening_angle1_mc);
						fhEPEM_openingAngle_gg_mc->Fill(opening_angle2_mc);
						fhEPEM_openingAngle_gg_refitted->Fill(opening_angle1_refitted);
						fhEPEM_openingAngle_gg_refitted->Fill(opening_angle2_refitted);
						

						cout << "reco/mc: " << fRecoMomentum[i].Mag() << " / " << fRecoTracklistEPEM[i]->GetP() << " ### "  << fRecoMomentum[j].Mag() << " / " << fRecoTracklistEPEM[j]->GetP() << " ### "  << fRecoMomentum[k].Mag() << " / " << fRecoTracklistEPEM[k]->GetP() << " ### "  << fRecoMomentum[l].Mag() << " / " << fRecoTracklistEPEM[l]->GetP() << endl;

						fill++;
					}
					}
				}
			}
		}
	}
	cout << "InvariantMassTest_4epem - Filled events: " << fill << endl;
	cout << "InvariantMassTest_4epem - End!" << endl;


	timer.Stop();
	fTime += timer.RealTime();
}



Int_t CbmAnaConversionReco::NofDaughters(Int_t motherId) 
{
	Int_t nofDaughters = 0;
	for(int i=0; i<fRecoTracklistEPEM.size(); i++) {
		Int_t motherId_temp = fRecoTracklistEPEM[i]->GetMotherId();
		if(motherId == motherId_temp) nofDaughters++;
	}


	return nofDaughters;
}


Double_t CbmAnaConversionReco::CalculateOpeningAngleReco(TVector3 electron1, TVector3 electron2)
{
	Double_t energyP = TMath::Sqrt(electron1.Mag2() + M2E);
	TLorentzVector lorVecP(electron1, energyP);

	Double_t energyM = TMath::Sqrt(electron2.Mag2() + M2E);
	TLorentzVector lorVecM(electron2, energyM);

	Double_t anglePair = lorVecM.Angle(lorVecP.Vect());
	Double_t theta = 180.*anglePair/TMath::Pi();

	return theta;
}


Double_t CbmAnaConversionReco::CalculateOpeningAngleMC(CbmMCTrack* mctrack1, CbmMCTrack* mctrack2)
{
	TVector3 electron1;
	mctrack1->GetMomentum(electron1);
	Double_t energyP = TMath::Sqrt(electron1.Mag2() + M2E);
	TLorentzVector lorVecP(electron1, energyP);

	TVector3 electron2;
	mctrack2->GetMomentum(electron2);
	Double_t energyM = TMath::Sqrt(electron2.Mag2() + M2E);
	TLorentzVector lorVecM(electron2, energyM);

	Double_t anglePair = lorVecM.Angle(lorVecP.Vect());
	Double_t theta = 180.*anglePair/TMath::Pi();

	return theta;
}



void CbmAnaConversionReco::CalculateInvMassWithFullRecoCuts()
{
	Int_t nof = fRecoTracklistEPEM.size();
	if(nof >= 4) {
		for(int a=0; a<nof-3; a++) {
			for(int b=a; b<nof-2; b++) {
				for(int c=b; c<nof-1; c++) {
					for(int d=c; d<nof; d++) {
						Int_t check1 = (fRecoTracklistEPEM[a]->GetPdgCode() > 0);
						Int_t check2 = (fRecoTracklistEPEM[b]->GetPdgCode() > 0);
						Int_t check3 = (fRecoTracklistEPEM[c]->GetPdgCode() > 0);
						Int_t check4 = (fRecoTracklistEPEM[d]->GetPdgCode() > 0);
						Int_t test = check1 + check2 + check3 + check4;
						if(test != 2) continue;		// need two electrons and two positrons
						
						
						Double_t invmass = Invmass_4particlesRECO(fRecoRefittedMomentum[a], fRecoRefittedMomentum[b], fRecoRefittedMomentum[c], fRecoRefittedMomentum[d]);
						//fhElectrons_invmass->Fill(invmass);
						
						
						CbmLmvmKinematicParams params1 = CalculateKinematicParamsReco(fRecoRefittedMomentum[a], fRecoRefittedMomentum[b]);
						CbmLmvmKinematicParams params2 = CalculateKinematicParamsReco(fRecoRefittedMomentum[a], fRecoRefittedMomentum[c]);
						CbmLmvmKinematicParams params3 = CalculateKinematicParamsReco(fRecoRefittedMomentum[a], fRecoRefittedMomentum[d]);
						CbmLmvmKinematicParams params4 = CalculateKinematicParamsReco(fRecoRefittedMomentum[b], fRecoRefittedMomentum[c]);
						CbmLmvmKinematicParams params5 = CalculateKinematicParamsReco(fRecoRefittedMomentum[b], fRecoRefittedMomentum[d]);
						CbmLmvmKinematicParams params6 = CalculateKinematicParamsReco(fRecoRefittedMomentum[c], fRecoRefittedMomentum[d]);
						
						Double_t openingAngleCut = 1;
						Int_t IsPhoton_openingAngle1 = (params1.fAngle < openingAngleCut);
						Int_t IsPhoton_openingAngle2 = (params2.fAngle < openingAngleCut);
						Int_t IsPhoton_openingAngle3 = (params3.fAngle < openingAngleCut);
						Int_t IsPhoton_openingAngle4 = (params4.fAngle < openingAngleCut);
						Int_t IsPhoton_openingAngle5 = (params5.fAngle < openingAngleCut);
						Int_t IsPhoton_openingAngle6 = (params6.fAngle < openingAngleCut);
						
						Double_t invMassCut = 0.03;
						Int_t IsPhoton_invMass1 = (params1.fMinv < invMassCut);
						Int_t IsPhoton_invMass2 = (params2.fMinv < invMassCut);
						Int_t IsPhoton_invMass3 = (params3.fMinv < invMassCut);
						Int_t IsPhoton_invMass4 = (params4.fMinv < invMassCut);
						Int_t IsPhoton_invMass5 = (params5.fMinv < invMassCut);
						Int_t IsPhoton_invMass6 = (params6.fMinv < invMassCut);
						
						if(IsPhoton_openingAngle1 && IsPhoton_openingAngle6 && IsPhoton_invMass1 && IsPhoton_invMass6 && (check1 + check2 == 1) && (check3 + check4 == 1)) {
							fhInvMassWithFullRecoCuts->Fill(invmass);
						}
						if(IsPhoton_openingAngle2 && IsPhoton_openingAngle5 && IsPhoton_invMass2 && IsPhoton_invMass5 && (check1 + check3 == 1) && (check2 + check4 == 1)) {
							fhInvMassWithFullRecoCuts->Fill(invmass);
						}
						if(IsPhoton_openingAngle3 && IsPhoton_openingAngle4 && IsPhoton_invMass3 && IsPhoton_invMass4 && (check1 + check4 == 1) && (check2 + check3 == 1)) {
							fhInvMassWithFullRecoCuts->Fill(invmass);
						}
					}
				}
			}
		}
	}


}



CbmLmvmKinematicParams CbmAnaConversionReco::CalculateKinematicParamsReco(const TVector3 electron1, const TVector3 electron2)
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


