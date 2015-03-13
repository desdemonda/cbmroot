/**
 * \file CbmAnaConversion.cxx
 *
 * Base class for conversion analysis
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2014
 **/

#include "CbmAnaConversion.h"

#include "TH1D.h"
#include "TH1.h"
#include "TH3.h"
#include "TCanvas.h"
#include "TClonesArray.h"

#include "CbmMCTrack.h"
#include "FairTrackParam.h"
#include "CbmRichHit.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmDrawHist.h"
#include "CbmTrackMatchNew.h"
#include "CbmRichPoint.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmRichRing.h"
#include "CbmTrackMatchNew.h"

#include "CbmUtils.h"

#include <iostream>
#include <string>
#include <boost/assign/list_of.hpp>


#include "L1Field.h"
#include "CbmL1PFFitter.h"
#include "CbmStsKFTrackFitter.h"

#include "TRandom3.h"

#include "CbmKFParticleFinder.h"
#include "CbmKFParticleFinderQA.h"
#include "KFParticleTopoReconstructor.h"


#include "CbmAnaConversionTomography.h"
#include "CbmAnaConversionRich.h"
//#include "CbmAnaConversionKF.h"
#include "CbmAnaConversionReco.h"


#define M2E 2.6112004954086e-7

using namespace std;
using boost::assign::list_of;

CbmAnaConversion::CbmAnaConversion()
  : FairTask("CbmAnaConversion"),
    DoTomography(0),
    DoRichAnalysis(0),
    DoKFAnalysis(0),
    DoReconstruction(0),
    fhNofElPrim(NULL),
    fhNofElSec(NULL),
    fhNofElAll(NULL),
    fhElectronSources(NULL),
    fhNofPi0_perEvent(NULL),
    fhNofPi0_perEvent_cut(NULL),
    fhPi0_z(NULL),
    fhPi0_z_cut(NULL),
    fhElectronsFromPi0_z(NULL),
    fhInvariantMass_test(NULL),
    fhInvariantMass_test2(NULL),
    fhInvariantMass_test3(NULL),
    fhInvariantMassReco_test(NULL),
    fhInvariantMassReco_test2(NULL),
    fhInvariantMassReco_test3(NULL),
    fhInvariantMassReco_pi0(NULL),
    fhMomentum_MCvsReco(NULL),     
    fhMomentum_MCvsReco_diff(NULL),   
    fhSearchGammas(NULL),
    fPrimVertex(NULL),
    fKFVertex(),
    fRichPoints(NULL),
    fRichRings(NULL),
    fRichRingMatches(NULL),
    fMcTracks(NULL),
    fStsTracks(NULL),
    fStsTrackMatches(NULL),
    fGlobalTracks(NULL),
    fEventNum(0),
    test(0),
    testint(0),
    fAnalyseMode(0),
    fKFparticle(NULL),
    fKFparticleFinderQA(NULL),
    fKFtopo(NULL),
    trackindexarray(),
    particlecounter(0),
    particlecounter_2daughters(0),
    particlecounter_3daughters(0),
    particlecounter_4daughters(0),
    particlecounter_all(0),
    fNofGeneratedPi0_allEvents(0),
    fNofPi0_kfparticle_allEvents(0),
    fNofGeneratedPi0(0),
    fNofPi0_kfparticle(0),
    fhPi0Ratio(NULL),
    fhPi0_mass(NULL),
    fhPi0_NDaughters(NULL),
    fHistoList(),
    fHistoList_MC(),
    fHistoList_tomography(),
    fHistoList_reco(),
    fHistoList_reco_mom(),
    fHistoList_kfparticle(),
    fHistoList_richrings(),
    fMCTracklist(),
    fMCTracklist_all(),
    fRecoTracklist(),
    fRecoTracklistEPEM(),
    fRecoMomentum(),
    fRecoRefittedMomentum(),
    timer_all(),
    fTime_all(0.),
    timer_mc(),
    fTime_mc(0.),
    timer_rec(),
    fTime_rec(0.),
    fAnaTomography(NULL),
    fAnaRich(NULL),
    fAnaKF(NULL),
    fAnaReco(NULL)
    
{
}

CbmAnaConversion::~CbmAnaConversion()
{

}

InitStatus CbmAnaConversion::Init()
{
	cout << "CbmAnaConversion::Init"<<endl;
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

	InitHistograms();
   
	fKFtopo = fKFparticle->GetTopoReconstructor();
	particlecounter = 0;   
   
	testint = 0;
	test = 0;
	
	fNofGeneratedPi0_allEvents = 0;
	fNofPi0_kfparticle_allEvents = 0;
	
	
	DoTomography = 1;
	DoRichAnalysis = 1;
	DoKFAnalysis = 1;
	DoReconstruction = 1;
	
	if(DoTomography) {
		fAnaTomography = new CbmAnaConversionTomography();
		fAnaTomography->Init();
	}
	if(DoRichAnalysis) {
		fAnaRich = new CbmAnaConversionRich();
		fAnaRich->Init();
	}
	if(DoKFAnalysis) {
	//	fAnaKF = new CbmAnaConversionKF();
	//	fAnaKF->SetKF(fKFparticle, fKFparticleFinderQA);
	//	fAnaKF->Init();
	}
	if(DoReconstruction) {
		fAnaReco = new CbmAnaConversionReco();
		fAnaReco->Init();
	}


	return kSUCCESS;
}

void CbmAnaConversion::InitHistograms()
{
	fHistoList.clear();
	fHistoList_kfparticle.clear();
	
	
	fhNofElPrim				= new TH1D("fhNofElPrim", "fhNofElPrim;Nof prim El;Entries", 10., -0.5, 9.5);
	fhNofElSec				= new TH1D("fhNofElSec", "fhNofElSec;Nof Sec El;Entries", 20., -0.5, 19.5);
	fhNofElAll				= new TH1D("fhNofElAll", "fhNofElAll;Nof All El;Entries", 30., -0.5, 29.5);
	fhNofPi0_perEvent		= new TH1D("fhNofPi0_perEvent", "fhNofPi0_perEvent;Nof pi0;Entries", 400., -0.5, 799.5);
	fhNofPi0_perEvent_cut	= new TH1D("fhNofPi0_perEvent_cut", "fhNofPi0_perEvent_cut;Nof pi0;Entries", 400., -0.5, 799.5);
	fhPi0_z					= new TH1D("fhPi0_z", "fhPi0_z;z [cm];Entries", 600., -0.5, 599.5);
	fhPi0_z_cut				= new TH1D("fhPi0_z_cut", "fhPi0_z_cut;z [cm];Entries", 600., -0.5, 599.5);
	fhElectronSources		= new TH1D("fhElectronSources", "fhElectronSources;Source;Entries", 5., 0., 5.);
	fhElectronsFromPi0_z	= new TH1D("fhElectronsFromPi0_z", "fhElectronsFromPi0_z (= pos. of gamma conversion);z [cm];Entries", 600., -0.5, 599.5);
	fHistoList.push_back(fhNofPi0_perEvent);
	fHistoList.push_back(fhNofPi0_perEvent_cut);
	fHistoList.push_back(fhPi0_z);
	fHistoList.push_back(fhPi0_z_cut);
	fHistoList.push_back(fhElectronSources);
	fHistoList.push_back(fhElectronsFromPi0_z);
	
	fhElectronSources->GetXaxis()->SetBinLabel(1, "gamma");
	fhElectronSources->GetXaxis()->SetBinLabel(2, "pi0");
	fhElectronSources->GetXaxis()->SetBinLabel(3, "eta");
	fhElectronSources->GetXaxis()->SetBinLabel(4, "gamma from pi0");
	fhElectronSources->GetXaxis()->SetBinLabel(5, "gamma from eta");
	

	
	// for UrQMD events (invariant mass from pi0 -> gamma + gamma
	fhInvariantMass_test	= new TH1D("fhInvariant", "fhInvariant;mass [GeV/c^2];#", 2000, 0., 2.);
	fhInvariantMass_test2	= new TH1D("fhInvariant2", "fhInvariant2;mass [GeV/c^2];#", 2000, 0., 2.);
	fhInvariantMass_test3	= new TH1D("fhInvariant3", "fhInvariant3;mass [GeV/c^2];#", 2000, 0., 2.);
	fHistoList.push_back(fhInvariantMass_test);
	fHistoList.push_back(fhInvariantMass_test2);
	fHistoList.push_back(fhInvariantMass_test3);
	
	fhInvariantMassReco_test	= new TH1D("fhInvariantReco", "fhInvariantReco;mass [GeV/c^2];#", 2000, 0., 2.);
	fhInvariantMassReco_test2	= new TH1D("fhInvariantReco2", "fhInvariantReco2;mass [GeV/c^2];#", 2000, 0., 2.);
	fhInvariantMassReco_test3	= new TH1D("fhInvariantReco3", "fhInvariantReco3;mass [GeV/c^2];#", 2000, 0., 2.);
	fHistoList.push_back(fhInvariantMassReco_test);
	fHistoList.push_back(fhInvariantMassReco_test2);
	fHistoList.push_back(fhInvariantMassReco_test3);
	
	fhInvariantMassReco_pi0 = new TH1D("fhInvariantReco_pi0", "fhInvariantReco_pi0;mass [GeV/c^2];#", 2000, 0., 2.);
	fHistoList.push_back(fhInvariantMassReco_pi0);
	

	
	// for reconstructed tracks
	fhMomentum_MCvsReco			= new TH2D("fhMomentum_MCvsReco","fhMomentum_MCvsReco;MC;Reco", 400, 0., 40., 400, 0., 40.); 
	fhMomentum_MCvsReco_diff	= new TH1D("fhMomentum_MCvsReco_diff","fhMomentum_MCvsReco_diff;(MC-Reco)/MC", 500, -0.01, 4.); 
	fHistoList.push_back(fhMomentum_MCvsReco);
	fHistoList.push_back(fhMomentum_MCvsReco_diff);

	
	fhSearchGammas = new TH1D("fhSearchGammas","fhSearchGammas;mass;#", 100, -0.005, 0.995);
	fHistoList.push_back(fhSearchGammas);
	
	
	
	// #############################################
	// Histograms related to KFParticle results
	fhPi0_NDaughters	= new TH1D("fhPi0_NDaughters","fhPi0_NDaughters;number of daughers;#", 4, 0.5, 4.5);
	fhPi0Ratio			= new TH1D("fhPi0Ratio", "fhPi0Ratio; ratio;#", 1000, 0., 0.02);
	fhPi0_mass			= new TH1D("fhPi0_mass", "fhPi0_mass;mass;#", 500, 0., 0.5);
	fHistoList_kfparticle.push_back(fhPi0_NDaughters);
	fHistoList_kfparticle.push_back(fhPi0Ratio);
	fHistoList_kfparticle.push_back(fhPi0_mass);

}



void CbmAnaConversion::Exec(Option_t* option)
{
	//timer_all.Reset();
	timer_all.Start();

	cout << "=======================================================================" << endl;
	cout << "========== CbmAnaConversion, event No. " <<  fEventNum << endl;
	cout << "=======================================================================" << endl;

	fEventNum++;
   
	fNofGeneratedPi0 = 0;
	fNofPi0_kfparticle = 0;


	// arrays of tracks 
	fMCTracklist.clear();
	fMCTracklist_all.clear();
	fRecoTracklist.clear();
	fRecoTracklistEPEM.clear();
	fRecoMomentum.clear();
	fRecoRefittedMomentum.clear();

	// several counters
	int countPrimEl = 0;
	int countSecEl = 0;
	int countAllEl  = 0;
	int countGammaEl = 0;
	int countMothers = 0;
	int countPrimPart = 0;   
	int countPi0MC = 0; 
	int countPi0MC_cut = 0;
	int countPi0MC_fromPrimary = 0;
   
	if (fPrimVertex != NULL){
		fKFVertex = CbmKFVertex(*fPrimVertex);
	} else {
		Fatal("CbmAnaConversion::Exec","No PrimaryVertex array!");
	}

	if(DoKFAnalysis) {
		//fAnaKF->SetSignalIds(fKFparticleFinderQA->GetSignalIds());
		//fAnaKF->SetGhostIds(fKFparticleFinderQA->GetGhostIds());
		//fAnaKF->KFParticle_Analysis();
	}

	if(DoRichAnalysis) {
		fAnaRich->AnalyseRICHdata();
	}

	// ========================================================================================
	// START - Analyse MC tracks
	timer_mc.Start();
	Int_t nofMcTracks = fMcTracks->GetEntriesFast();
	for (int i = 0; i < nofMcTracks; i++) {
		CbmMCTrack* mctrack = (CbmMCTrack*)fMcTracks->At(i);
		if (mctrack == NULL) continue;   
   
		FillMCTracklists(mctrack);	// fill tracklists for further analyses

		if(DoTomography) {
			fAnaTomography->TomographyMC(mctrack);		// analyse gamma-conversions with MC data
		}


		if (mctrack->GetMotherId() == -1) { countPrimPart++; }   
		if (mctrack->GetMotherId() == -1 && TMath::Abs( mctrack->GetPdgCode()) == 11) { countPrimEl++; }   
		if (mctrack->GetMotherId() != -1 && TMath::Abs( mctrack->GetPdgCode()) == 11) { countSecEl++; }   
		if (TMath::Abs( mctrack->GetPdgCode()) == 11) { countAllEl++; }  
		if (mctrack->GetPdgCode() == 111) {
			countPi0MC++;
			TVector3 v;
			mctrack->GetStartVertex(v);
			if(v.Z() <= 10) {
				countPi0MC_cut++;
			}
			fhPi0_z->Fill(v.Z());
			Double_t r2 = v.Z()*v.Z() * tan(25./180*TMath::Pi()) * tan(25./180*TMath::Pi());
			if( (v.X()*v.X() + v.Y()*v.Y()) <= r2) {
				fhPi0_z_cut->Fill(v.Z());
			}
			
			int motherId = mctrack->GetMotherId();
			if (motherId == -1) countPi0MC_fromPrimary++;
		}

		if (TMath::Abs( mctrack->GetPdgCode())  == 11) { 
			AnalyseElectrons(mctrack);
		}
	}
	timer_mc.Stop();
	fTime_mc += timer_mc.RealTime();

	cout << "CbmAnaConversion::Exec - Number of pi0 in MC sample: " << countPi0MC << endl;
	cout << "CbmAnaConversion::Exec - Number of pi0 from primary: " << countPi0MC_fromPrimary << endl;
	fhNofPi0_perEvent->Fill(countPi0MC);
	fhNofPi0_perEvent_cut->Fill(countPi0MC_cut);
	
	fNofGeneratedPi0 = countPi0MC_fromPrimary;
	fNofGeneratedPi0_allEvents += fNofGeneratedPi0;
	fNofPi0_kfparticle_allEvents += fNofPi0_kfparticle;
	fhPi0Ratio->Fill(1.0*fNofPi0_kfparticle/fNofGeneratedPi0);

	InvariantMassTest();
	
	if(DoReconstruction) {
		fAnaReco->SetTracklistMC(fMCTracklist_all);
		fAnaReco->InvariantMassMC_all();
	}

	ReconstructGamma();

   
	fhNofElPrim->Fill(countPrimEl);
	fhNofElSec->Fill(countSecEl);
	fhNofElAll->Fill(countAllEl);


	// END - Analyse MC tracks
	// ========================================================================================
   
   
   
	// ========================================================================================
	// START - Analyse reconstructed tracks
	timer_rec.Start();
	Int_t ngTracks = fGlobalTracks->GetEntriesFast();
	for (Int_t i = 0; i < ngTracks; i++) {
		CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(i);
		if(NULL == gTrack) continue;
		int stsInd = gTrack->GetStsTrackIndex();
		int richInd = gTrack->GetRichRingIndex();
		int trdInd = gTrack->GetTrdTrackIndex();
		int tofInd = gTrack->GetTofHitIndex();

		if (stsInd < 0) continue;
		CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(stsInd);
		if (stsTrack == NULL) continue;
		CbmTrackMatchNew* stsMatch  = (CbmTrackMatchNew*) fStsTrackMatches->At(stsInd);
		if (stsMatch == NULL) continue;
		int stsMcTrackId = stsMatch->GetMatchedLink().GetIndex();
		if (stsMcTrackId < 0) continue;
		CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMcTracks->At(stsMcTrackId);
		if (mcTrack1 == NULL) continue;
		int pdg = TMath::Abs(mcTrack1->GetPdgCode());
		int motherId = mcTrack1->GetMotherId();
		double momentum = mcTrack1->GetP();
		stsMatch->GetTrueOverAllHitsRatio();
       
		if(DoTomography) {
			fAnaTomography->TomographyReco(mcTrack1);
		}
		FillRecoTracklist(mcTrack1);
       
		TVector3 stsMomentumVec;	// momenta as measured by STS
		stsTrack->GetParamFirst()->Momentum(stsMomentumVec);
		Double_t stsMomentum = stsMomentumVec.Mag();
       
		TVector3 mcMomentumVec;		// momenta from true-MC data
		mcTrack1->GetMomentum(mcMomentumVec);
		Double_t mcMomentum = mcMomentumVec.Mag();
		fhMomentum_MCvsReco->Fill(mcMomentum, stsMomentum);
		fhMomentum_MCvsReco_diff->Fill(TMath::Abs(mcMomentum-stsMomentum)/mcMomentum);
       
		TVector3 bothtogether;		// combination of measured (STS) momenta and MC momenta
		bothtogether.SetX(mcMomentumVec.X());
		bothtogether.SetY(stsMomentumVec.Y());
		bothtogether.SetZ(stsMomentumVec.Z());
       

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
       
		// Fill tracklists containing momenta from mc-true, measured in sts, refitted at primary
		FillRecoTracklistEPEM(mcTrack1, stsMomentumVec, refittedMomentum);
	}
	timer_rec.Stop();
	fTime_rec += timer_rec.RealTime();
	
//	InvariantMassTestReco();

	if(DoReconstruction) {
		fAnaReco->SetTracklistReco(fRecoTracklistEPEM, fRecoMomentum, fRecoRefittedMomentum);
		fAnaReco->InvariantMassTest_4epem();
	}

	// END - analyse reconstructed tracks
	// ========================================================================================



// =========================================================================================================================
// ============================================== END - EXEC function ======================================================
// =========================================================================================================================
	timer_all.Stop();
	fTime_all += timer_all.RealTime();
}



void CbmAnaConversion::Finish()
{
	cout << "\n\n ############### FINISHING ############" << endl;
/*	
	TCanvas* c = new TCanvas();
	c->SetWindowSize(800, 1600);
	DrawH1(fTest);
	//fhGammaZ->Write();
*/
	
	// Write histograms to a file
	gDirectory->mkdir("analysis-conversion");
	gDirectory->cd("analysis-conversion");


	gDirectory->mkdir("KFParticle");
	gDirectory->cd("KFParticle");
	for (Int_t i = 0; i < fHistoList_kfparticle.size(); i++){
		fHistoList_kfparticle[i]->Write();
	}
	gDirectory->cd("..");


	if(DoTomography)		{ fAnaTomography->Finish(); }
	if(DoRichAnalysis)		{ fAnaRich->Finish(); }
	if(DoKFAnalysis)		{ fAnaKF->Finish(); }
	if(DoReconstruction)	{ fAnaReco->Finish(); }

	for (Int_t i = 0; i < fHistoList.size(); i++){
		fHistoList[i]->Write();
	}
	gDirectory->cd("..");
	
	
	
	cout << endl;
	cout << "############### FINISH ##############" << endl;
	cout << "Particlecounter: " << particlecounter << endl;
	cout << "Particlecounter (2 daughters): " << particlecounter_2daughters << endl;
	cout << "Particlecounter (3 daughters): " << particlecounter_3daughters << endl;
	cout << "Particlecounter (4 daughters): " << particlecounter_4daughters << endl;
	cout << "Particlecounter_all: " << particlecounter_all << endl;
	cout << "#####################################" << endl;
	cout << "Number of generated pi0 (all events): " << fNofGeneratedPi0_allEvents << endl;
	cout << "Number of reconstructed pi0 (all events): " << fNofPi0_kfparticle_allEvents << "\t - fraction: " << 1.0*fNofPi0_kfparticle_allEvents/fNofGeneratedPi0_allEvents << endl;
	cout << "#####################################" << endl;
	cout << "############### TIMER ###############" << endl;
	cout << "Complete time: " << fTime_all << endl;
	timer_all.Print();
	cout << "mc tme: " << fTime_mc << "\t time rec: " << fTime_rec << endl;
// =========================================================================================================================
// ============================================== END - FINISH function ====================================================
// =========================================================================================================================
}





void CbmAnaConversion::AnalyseElectrons(CbmMCTrack* mctrack)
{
	int motherId = mctrack->GetMotherId();
	if (motherId == -1) return;
	CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
	int mcMotherPdg  = -1;
	if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
	//cout << mcMotherPdg << endl;

	if(mcMotherPdg == 22) {
		fhElectronSources->Fill(0);
		int grandmotherId = mother->GetMotherId();
		if (grandmotherId == -1) return;
		CbmMCTrack* grandmother = (CbmMCTrack*) fMcTracks->At(grandmotherId);
		int mcGrandmotherPdg  = -1;
		if (NULL != grandmother) mcGrandmotherPdg = grandmother->GetPdgCode();
		if(mcGrandmotherPdg == 111) fhElectronSources->Fill(3);
		if(mcGrandmotherPdg == 221) fhElectronSources->Fill(4);

		if(mcGrandmotherPdg == 111) {
			TVector3 v;
			mctrack->GetStartVertex(v);
			fhElectronsFromPi0_z->Fill(v.Z());
		}

	}
	if(mcMotherPdg == 111) fhElectronSources->Fill(1);
	if(mcMotherPdg == 221) fhElectronSources->Fill(2);

	if (mcMotherPdg == 22) {
		TVector3 v;
		mctrack->GetStartVertex(v);
	//	fhGammaZ->Fill(v.Z());
	//	countGammaEl++;
	}
}






CbmLmvmKinematicParams CbmAnaConversion::CalculateKinematicParams(const CbmMCTrack* mctrackP, const CbmMCTrack* mctrackM)
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

Double_t CbmAnaConversion::Invmass_2gammas(const CbmMCTrack* mctrack1, const CbmMCTrack* mctrack2) 
// calculation of invariant mass from two gammas (m=0)
{
    TVector3 mom1;
    mctrack1->GetMomentum(mom1);
    Double_t energy1 = TMath::Sqrt(mom1.Mag2());
    TLorentzVector lorVec1(mom1, energy1);

    TVector3 mom2;
    mctrack2->GetMomentum(mom2);
    Double_t energy2 = TMath::Sqrt(mom2.Mag2());
    TLorentzVector lorVec2(mom2, energy2);
    
    TLorentzVector sum = lorVec1 + lorVec2;
	return sum.Mag();
}




Double_t CbmAnaConversion::Invmass_2particles(const CbmMCTrack* mctrack1, const CbmMCTrack* mctrack2) 
// calculation of invariant mass from two electrons/positrons
{
    TVector3 mom1;
    mctrack1->GetMomentum(mom1);
    Double_t energy1 = TMath::Sqrt(mom1.Mag2() + M2E);
    TLorentzVector lorVec1(mom1, energy1);

    TVector3 mom2;
    mctrack2->GetMomentum(mom2);
    Double_t energy2 = TMath::Sqrt(mom2.Mag2() + M2E);
    TLorentzVector lorVec2(mom2, energy2);
    
    TLorentzVector sum = lorVec1 + lorVec2;
	return sum.Mag();
}



Double_t CbmAnaConversion::Invmass_4particles(const CbmMCTrack* mctrack1, const CbmMCTrack* mctrack2, const CbmMCTrack* mctrack3, const CbmMCTrack* mctrack4)
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




// =========================================================================================================================
// ============================================== UP TO HERE: Invariant mass calculation ===================================
// =========================================================================================================================





void CbmAnaConversion::FillMCTracklists(CbmMCTrack* mctrack)
// fill all relevant tracklists containing MC tracks
{
	Bool_t electrons = true;
	Bool_t gammas = true;
	
	// fill gamma tracklist
	if(TMath::Abs( mctrack->GetPdgCode()) == 22 && gammas) {
		int motherId = mctrack->GetMotherId();
		if (motherId != -1) {
			CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
			int mcMotherPdg  = -1;
			if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
			if (mcMotherPdg == 111) {	// pdg code 111 = pi0
				fMCTracklist.push_back(mctrack);
			}
		}
	}

	// fill electron tracklists
	if(TMath::Abs( mctrack->GetPdgCode()) == 11 && electrons) { 
		TVector3 v;
		mctrack->GetStartVertex(v);
		if(v.Z() <= 70) {
			int motherId = mctrack->GetMotherId();
			if (motherId != -1 || motherId == -1) {
				CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
				int mcMotherPdg  = -1;
				if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
				if (mcMotherPdg == 22 || mcMotherPdg == 111 || mcMotherPdg == 221) {
					int grandmotherId = mother->GetMotherId();
					if(grandmotherId != -1 || grandmotherId == -1) {
					//	CbmMCTrack* grandmother = (CbmMCTrack*) fMcTracks->At(grandmotherId);
					//	int mcGrandmotherPdg = -1;
					//	if (NULL != grandmother) mcGrandmotherPdg = grandmother->GetPdgCode();
					//	if(mcGrandmotherPdg == 111) {
							fMCTracklist_all.push_back(mctrack);
					//	}
					}
				}
			}
		}
	}
}





void CbmAnaConversion::FillRecoTracklist(CbmMCTrack* mctrack) 
{
	if (TMath::Abs( mctrack->GetPdgCode())  == 11) { 
		int motherId = mctrack->GetMotherId();
		if (motherId != -1) {
		CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
		int mcMotherPdg  = -1;
		if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
		if (mcMotherPdg == 111 || mcMotherPdg == 22) {	// pdg code 111 = pi0, 22 = gamma
			fRecoTracklist.push_back(mctrack);
		//	cout << "pdg " << mctrack->GetPdgCode() << "\t motherid " << motherId << endl;
			test++;
		}
		}
	}
}




void CbmAnaConversion::FillRecoTracklistEPEM(CbmMCTrack* mctrack, TVector3 stsMomentum, TVector3 refittedMom) 
{
	if (TMath::Abs( mctrack->GetPdgCode())  == 11) { 
		int motherId = mctrack->GetMotherId();
		if (motherId != -1) {
			fRecoTracklistEPEM.push_back(mctrack);
			fRecoMomentum.push_back(stsMomentum);
			fRecoRefittedMomentum.push_back(refittedMom);
		}
	}
}




void CbmAnaConversion::CalculateInvMass_MC_2particles()
{
	for(int i=0; i<fMCTracklist_all.size(); i++) {
		for(int j=i+1; j<fMCTracklist_all.size(); j++) {
			Double_t invmass = Invmass_2particles(fMCTracklist_all[i],fMCTracklist_all[j]);
			
			int motherId_i = fMCTracklist_all[i]->GetMotherId();
			int motherId_j = fMCTracklist_all[j]->GetMotherId();
			
			CbmMCTrack* mother_i = (CbmMCTrack*) fMcTracks->At(motherId_i);
			int mcMotherPdg_i  = -1;
			if (NULL != mother_i) mcMotherPdg_i = mother_i->GetPdgCode();
		
			CbmMCTrack* mother_j = (CbmMCTrack*) fMcTracks->At(motherId_j);
			int mcMotherPdg_j  = -1;
			if (NULL != mother_j) mcMotherPdg_j = mother_j->GetPdgCode();
			
			if(motherId_i == motherId_j && ( (fMCTracklist_all[i]->GetPdgCode() == 11 && fMCTracklist_all[j]->GetPdgCode() == -11) || (fMCTracklist_all[i]->GetPdgCode() == -11 && fMCTracklist_all[j]->GetPdgCode() == 11))) {
			//	fhInvariantMass_MC_all->Fill(invmass);
				//cout << "e+e- decay detected! MotherId " << motherId_i << "\t invariant mass: " << invmass << endl;
				
				if(mcMotherPdg_i == 111) {
					//fhInvariantMass_MC_pi0->Fill(invmass);
					cout << "-- mother particle of decay: pi0" << endl;
				}
			}
		}
	}



}



void CbmAnaConversion::InvariantMassTest() 
// calculation of invariant mass via pi0-> gamma gamma, ONLY FROM MC DATA!
{
	for(int i=0; i<fMCTracklist.size(); i++) {
		for(int j=i+1; j<fMCTracklist.size(); j++) {
			
			//if(fMCTracklist[i]->GetPx() != fMCTracklist[j]->GetPx() && fMCTracklist[i]->GetPy() != fMCTracklist[j]->GetPy()) {
			Double_t invmass = Invmass_2gammas(fMCTracklist[i],fMCTracklist[j]);
			fhInvariantMass_test->Fill(invmass);
			TVector3 vi;
			fMCTracklist[i]->GetStartVertex(vi);
			TVector3 vj;
			fMCTracklist[j]->GetStartVertex(vj);
			
			int motherId_i = fMCTracklist[i]->GetMotherId();
			int motherId_j = fMCTracklist[j]->GetMotherId();
			
			if(motherId_i == motherId_j) {
				fhInvariantMass_test2->Fill(invmass);
				
				if(invmass < 0.001) {
				//	cout << "gamma1 " << fMCTracklist[i]->GetPx() << "\t" << fMCTracklist[i]->GetPy() << "\t" << fMCTracklist[i]->GetPz() << endl;
				//	cout << "gamma2 " << fMCTracklist[j]->GetPx() << "\t" << fMCTracklist[j]->GetPy() << "\t" << fMCTracklist[j]->GetPz() << endl;
				}
			}
			if(vi.Z() == vj.Z() && vi.Y() == vj.Y() && vi.X() == vj.X()) {
				fhInvariantMass_test3->Fill(invmass);
			}
			//}
		}
	}
}











void CbmAnaConversion::InvariantMassTestReco() 
// calculation of invariant mass via pi0 -> .. -> e+ e- e+ e-, ONLY FROM RECONSTRUCTED TRACKS!
{
//	cout << "InvariantMassTestReco - Start..." << endl;
//	cout << "InvariantMassTestReco - Size of fRecoTracklist:\t " << fRecoTracklist.size() << endl;
	if(fRecoTracklist.size() >= 4) {
		for(int i=0; i<fRecoTracklist.size(); i++) {
			for(int j=i+1; j<fRecoTracklist.size(); j++) {
				for(int k=j+1; k<fRecoTracklist.size(); k++) {
					for(int l=k+1; l<fRecoTracklist.size(); l++) {
					
						if(fRecoTracklist[i]->GetPdgCode() + fRecoTracklist[j]->GetPdgCode() + fRecoTracklist[k]->GetPdgCode() + fRecoTracklist[l]->GetPdgCode() != 0) continue;
					
						int motherId1 = fRecoTracklist[i]->GetMotherId();
						int motherId2 = fRecoTracklist[j]->GetMotherId();
						int motherId3 = fRecoTracklist[k]->GetMotherId();
						int motherId4 = fRecoTracklist[l]->GetMotherId();
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
					
					
						if(motherId1 == motherId2 && motherId2 == motherId3 && motherId3 == motherId4) {
							Double_t invmass = Invmass_4particles(fRecoTracklist[i], fRecoTracklist[j], fRecoTracklist[k], fRecoTracklist[l]);
							fhInvariantMassReco_pi0->Fill(invmass);
					//		cout << "Decay pi0 -> e+ e- e+ e- detected!" << endl;
						}
						
						if(motherId1 == motherId2 && motherId1 == motherId3) {
						
						}
						if(motherId1 == motherId2 && motherId1 == motherId4) {
						
						}
						if(motherId1 == motherId3 && motherId1 == motherId4) {
						
						}
						if(motherId2 == motherId3 && motherId2 == motherId4) {
							
						}
						
					//	if(  ((motherId1 == motherId2 && motherId3 == motherId4) && (mcMotherPdg1 == 22 || mcMotherPdg1 == 111) && (mcMotherPdg3 == 22 || mcMotherPdg3 == 111))
					//	  || ((motherId1 == motherId3 && motherId2 == motherId4) && (mcMotherPdg1 == 22 || mcMotherPdg1 == 111) && (mcMotherPdg2 == 22 || mcMotherPdg2 == 111))
					//	  || ((motherId1 == motherId4 && motherId2 == motherId3) && (mcMotherPdg1 == 22 || mcMotherPdg1 == 111) && (mcMotherPdg2 == 22 || mcMotherPdg2 == 111))) {
						if(  ((motherId1 == motherId2 && motherId3 == motherId4) && (mcMotherPdg1 == 22) && (mcMotherPdg3 == 22) && grandmotherId1 == grandmotherId3)
						  || ((motherId1 == motherId3 && motherId2 == motherId4) && (mcMotherPdg1 == 22) && (mcMotherPdg2 == 22) && grandmotherId1 == grandmotherId2)
						  || ((motherId1 == motherId4 && motherId2 == motherId3) && (mcMotherPdg1 == 22) && (mcMotherPdg2 == 22) && grandmotherId1 == grandmotherId2)) {
							Double_t invmass = Invmass_4particles(fRecoTracklist[i], fRecoTracklist[j], fRecoTracklist[k], fRecoTracklist[l]);
							fhInvariantMassReco_pi0->Fill(invmass);
					//		cout << "Decay pi0 -> 2gamma -> e+ e-! \t MotherId " << motherId1 << "\t" << motherId2 << "\t" << motherId3 << "\t" << motherId4 <<
					//			 "\t GrandmotherId " << grandmotherId1 << "\t" << grandmotherId2 << "\t" << grandmotherId3 << "\t" << grandmotherId4 << endl;
						
						}
					}
				}
			}
		}
	}
//	cout << "InvariantMassTestReco - End!" << endl;
}






int CbmAnaConversion::GetTest()
{
	return testint;
}


int CbmAnaConversion::GetNofEvents()
{
	return fEventNum;
}


void CbmAnaConversion::SetMode(int mode = 0)
	// mode 1 = tomography
	// mode 2 = urqmd
	// mode 3 = pluto
{
	fAnalyseMode = mode;
}




void CbmAnaConversion::ReconstructGamma()
{
	if(fMCTracklist_all.size() >= 2) {
		for(int i=0; i<fMCTracklist_all.size(); i++) {
			for(int j=i+1; j<fMCTracklist_all.size(); j++) {
				if(fMCTracklist_all[i]->GetPdgCode() + fMCTracklist_all[j]->GetPdgCode() != 0) continue;
				
				int motherId1 = fMCTracklist_all[i]->GetMotherId();
				int motherId2 = fMCTracklist_all[j]->GetMotherId();
				
				if(motherId1 == motherId2) {
					Double_t invmass = Invmass_2particles(fMCTracklist_all[i], fMCTracklist_all[j]);
					fhSearchGammas->Fill(invmass);
				}
			}
		}
	}
}



void CbmAnaConversion::SetKF(CbmKFParticleFinder* kfparticle, CbmKFParticleFinderQA* kfparticleQA)
{

	fKFparticle = kfparticle;
	fKFparticleFinderQA = kfparticleQA;
	if(fKFparticle) {
		cout << "kf works" << endl;
	}
	else {
		cout << "kf works not" << endl;
	}

}







ClassImp(CbmAnaConversion)

