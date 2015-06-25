/**
 * \file CbmAnaConversionRich.cxx
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2014
 **/

#include "CbmAnaConversionRich.h"

#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "CbmRichPoint.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"
#include "CbmRichRing.h"
#include "CbmTrackMatchNew.h"

#include <algorithm>
#include <map>



using namespace std;



CbmAnaConversionRich::CbmAnaConversionRich()
  : fRichPoints(NULL),
    fRichRings(NULL),
    fRichRingMatches(NULL),
    fMcTracks(NULL),
    fStsTracks(NULL),
    fStsTrackMatches(NULL),
    fGlobalTracks(NULL),
    fPrimVertex(NULL),
    fHistoList_richrings(),
    fTest(NULL),
    fRichRings_nofRings(NULL),
    fRichRings_motherpdg(NULL),
    fRichRings_richpdg(NULL),
    fRichRings_electronspE(NULL),
    fRichRings_sourcePI0(NULL),
    fRichRings_test(NULL),
    fRichRings_detectedParticles(NULL),
    fRichRings_detParticlesMother(NULL),
    fRichRings_Aaxis(NULL),
    fRichRings_Aaxis_part1(NULL),
    fRichRings_Aaxis_part2(NULL),
    fRichRings_Aaxis_part3(NULL),
    fRichRings_Baxis(NULL),
    fRichRings_Baxis_part1(NULL),
    fRichRings_Baxis_part2(NULL),
    fRichRings_Baxis_part3(NULL),
    fRichRings_radius(NULL),
    fRichRings_distance(NULL),
    fhRingtest(NULL),
    fhRichRings_AaxisVSmom(NULL),
    fhRichRings_BaxisVSmom(NULL),
    fhRichRings_test1(NULL),
    fhRichRings_test2(NULL),
    fhRichRings_test3(NULL),
    fhRichRings_test4(NULL),
    fhRichRings_test5(NULL),
    fhRichRings_test6(NULL),
    fhRichRings_pos(NULL),
    fhRichRings_protons(NULL),
    fhRichRings_protons2(NULL),
    fhRichRings_start(NULL),
    timer(),
    fTime(0.)
{
}

CbmAnaConversionRich::~CbmAnaConversionRich()
{
}


void CbmAnaConversionRich::Init()
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
}


void CbmAnaConversionRich::InitHistos()
{
	fHistoList_richrings.clear();

	fTest = new TH2D("fTest", "fTest; X; Y", 200, -100, 100, 400, -200, 200);
	fHistoList_richrings.push_back(fTest);


	// #############################################
	// Histograms related to rich rings
	fRichRings_nofRings		= new TH1D("fRichRings_nofRings", "fRichRings_nofRings;nof rings per Event;#", 101, -0.5, 100.5);
	fRichRings_motherpdg	= new TH1D("fRichRings_motherpdg", "fRichRings_motherpdg;pdg code;#", 5002, -1.5, 5000.5);
	fRichRings_richpdg		= new TH1D("fRichRings_richpdg", "fRichRings_richpdg;pdg code;#", 5001, -0.5, 5000.5);
	fRichRings_electronspE	= new TH1D("fRichRings_electronspE", "fRichRings_electronspE;nof electrons per event;#", 60, -0.5, 59.5);
	fRichRings_sourcePI0	= new TH1D("fRichRings_sourcePI0", "fRichRings_sourcePI0;nof electrons from pi0;#", 60, -0.5, 59.5);
	fRichRings_test			= new TH1D("fRichRings_test", "fRichRings_test;nof electrons from pi0;#", 20, -0.5, 19.5);
	fHistoList_richrings.push_back(fRichRings_nofRings);
	fHistoList_richrings.push_back(fRichRings_motherpdg);
	fHistoList_richrings.push_back(fRichRings_richpdg);
	fHistoList_richrings.push_back(fRichRings_electronspE);
	fHistoList_richrings.push_back(fRichRings_sourcePI0);
	fHistoList_richrings.push_back(fRichRings_test);
	
	fRichRings_detectedParticles	= new TH1D("fRichRings_detectedParticles", "fRichRings_detectedParticles;;#", 9, 0., 9.);
	fRichRings_detectedParticles->SetLabelSize(0.06);
	fHistoList_richrings.push_back(fRichRings_detectedParticles);
	fRichRings_detectedParticles->GetXaxis()->SetBinLabel(1, "e^{+}/e^{-}");		// pdg code 11
	fRichRings_detectedParticles->GetXaxis()->SetBinLabel(2, "\\mu^{+}/\\mu^{-}");	// pdg code 13
	fRichRings_detectedParticles->GetXaxis()->SetBinLabel(3, "\\pi^{+}/\\pi^{-}");	// pdg code 211
	fRichRings_detectedParticles->GetXaxis()->SetBinLabel(4, "K^{+}/K^{-}");		// pdg code 321
	fRichRings_detectedParticles->GetXaxis()->SetBinLabel(5, "p");					// pdg code 2212
	fRichRings_detectedParticles->GetXaxis()->SetBinLabel(6, "\\Sigma^-");			// pdg code 3112
	fRichRings_detectedParticles->GetXaxis()->SetBinLabel(7, "\\Sigma^+");			// pdg code 3222
	fRichRings_detectedParticles->GetXaxis()->SetBinLabel(8, "\\Xi^-");				// pdg code 3312
	fRichRings_detectedParticles->GetXaxis()->SetBinLabel(9, "else");				// everything else
	
	fRichRings_detParticlesMother	= new TH1D("fRichRings_detParticlesMother", "fRichRings_detParticlesMother;;#", 22, 0., 22.);
	fRichRings_detParticlesMother->SetLabelSize(0.04);
	fHistoList_richrings.push_back(fRichRings_detParticlesMother);
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(1, "e^{+}/e^{-}");		// pdg code 11
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(2, "\\mu^{+}/\\mu^{-}");	// pdg code 13
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(3, "\\gamma");			// pdg code 22
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(4, "\\pi^{0}");			// pdg code 111
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(5, "K_{L}^{0}");			// pdg code 130
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(6, "\\pi^{+}/\\pi^{-}");	// pdg code 211
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(7, "\\eta");				// pdg code 221
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(8, "K_{S}^{0}");			// pdg code 310
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(9, "K^{+}/K^{-}");		// pdg code 321
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(10, "n");				// pdg code 2112
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(11, "p");				// pdg code 2212
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(12, "\\Sigma^-");		// pdg code 3112
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(13, "\\Lambda^0");		// pdg code 3122
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(14, "\\Sigma^+");		// pdg code 3222
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(15, "\\Xi^-");			// pdg code 3312
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(16, "\\Omega^-");		// pdg code 3334
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(17, "prim.");			// no mother particle, i.e. comes from primary vertex
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(18, "prim. E");			// no mother particle, i.e. comes from primary vertex (e+/e-)
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(19, "prim. M");			// no mother particle, i.e. comes from primary vertex (mu+/mu-)
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(20, "prim. P");			// no mother particle, i.e. comes from primary vertex (pi+/pi-)
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(21, "prim. K");			// no mother particle, i.e. comes from primary vertex (K+/K-)
	fRichRings_detParticlesMother->GetXaxis()->SetBinLabel(22, "prim. else");		// no mother particle, i.e. comes from primary vertex (K+/K-)
	
	
	
	
	fRichRings_Aaxis		= new TH1D("fRichRings_Aaxis", "fRichRings_Aaxis;A-axis;#", 300, 0., 30.);
	fRichRings_Aaxis_part1	= new TH1D("fRichRings_Aaxis_part1", "fRichRings_Aaxis_part1;A-axis;#", 300, 0., 30.);
	fRichRings_Aaxis_part2	= new TH1D("fRichRings_Aaxis_part2", "fRichRings_Aaxis_part2;A-axis;#", 300, 0., 30.);
	fRichRings_Aaxis_part3	= new TH1D("fRichRings_Aaxis_part3", "fRichRings_Aaxis_part3;A-axis;#", 300, 0., 30.);
	fRichRings_Baxis		= new TH1D("fRichRings_Baxis", "fRichRings_Baxis;B-axis;#", 300, 0., 30.);  
	fRichRings_Baxis_part1	= new TH1D("fRichRings_Baxis_part1", "fRichRings_Baxis_part1;B-axis;#", 300, 0., 30.);
	fRichRings_Baxis_part2	= new TH1D("fRichRings_Baxis_part2", "fRichRings_Baxis_part2;B-axis;#", 300, 0., 30.);
	fRichRings_Baxis_part3	= new TH1D("fRichRings_Baxis_part3", "fRichRings_Baxis_part3;B-axis;#", 300, 0., 30.);  
	fRichRings_radius		= new TH1D("fRichRings_radius", "fRichRings_radius;radius;#", 300, 0., 30.);  
	fRichRings_distance		= new TH1D("fRichRings_distance", "fRichRings_distance;distance;#", 500, 0., 5.);  
	fhRingtest				= new TH2D("fhRingtest", "fhRingtest;momentum [GeV/c];radius [cm]", 200, 0., 10., 200, 0., 10.);
	fhRichRings_AaxisVSmom	= new TH2D("fhRichRings_AaxisVSmom", "fhRichRings_AaxisVSmom;momentum [GeV/c];a-axis [cm]", 200, 0., 10., 200, 0., 10.);
	fhRichRings_BaxisVSmom	= new TH2D("fhRichRings_BaxisVSmom", "fhRichRings_BaxisVSmom;momentum [GeV/c];b-axis [cm]", 200, 0., 10., 200, 0., 10.);
	fHistoList_richrings.push_back(fRichRings_Aaxis);
	fHistoList_richrings.push_back(fRichRings_Aaxis_part1);
	fHistoList_richrings.push_back(fRichRings_Aaxis_part2);
	fHistoList_richrings.push_back(fRichRings_Aaxis_part3);
	fHistoList_richrings.push_back(fRichRings_Baxis);
	fHistoList_richrings.push_back(fRichRings_Baxis_part1);
	fHistoList_richrings.push_back(fRichRings_Baxis_part2);
	fHistoList_richrings.push_back(fRichRings_Baxis_part3);
	fHistoList_richrings.push_back(fRichRings_radius);
	fHistoList_richrings.push_back(fRichRings_distance);
	fHistoList_richrings.push_back(fhRingtest);
	fHistoList_richrings.push_back(fhRichRings_AaxisVSmom);
	fHistoList_richrings.push_back(fhRichRings_BaxisVSmom);
	
	fhRichRings_test1		= new TH1D("fRichRings_test1", "fRichRings_test1;A-axis;#", 300, 0., 30.);
	fhRichRings_test2		= new TH1D("fRichRings_test2", "fRichRings_test2;A-axis;#", 300, 0., 30.);
	fhRichRings_test3		= new TH1D("fRichRings_test3", "fRichRings_test3;A-axis;#", 300, 0., 5.);
	fhRichRings_test4		= new TH1D("fRichRings_test4", "fRichRings_test4;A-axis;#", 300, 0., 30.);
	fhRichRings_test5		= new TH1D("fRichRings_test5", "fRichRings_test5;A-axis;#", 300, 0., 30.);
	fhRichRings_test6		= new TH1D("fRichRings_test6", "fRichRings_test6;A-axis;#", 300, 0., 30.);
	fhRichRings_pos			= new TH2D("fRichRings_pos", "fRichRings_pos;x;y", 400, -200., 200., 400, -200., 200.);
	fHistoList_richrings.push_back(fhRichRings_test1);
	fHistoList_richrings.push_back(fhRichRings_test2);
	fHistoList_richrings.push_back(fhRichRings_test3);
	fHistoList_richrings.push_back(fhRichRings_test4);
	fHistoList_richrings.push_back(fhRichRings_test5);
	fHistoList_richrings.push_back(fhRichRings_test6);
	fHistoList_richrings.push_back(fhRichRings_pos);


	// for proton analysis
	fhRichRings_protons	= new TH2D("fhRichRings_protons", "fhRichRings_protons;px;py", 1000, -10, 10, 1000, -10, 10);
	fHistoList_richrings.push_back(fhRichRings_protons);
	fhRichRings_protons2	= new TH1D("fhRichRings_protons2", "fhRichRings_protons2;nof hits;#", 40, -0.5, 39.5);
	fHistoList_richrings.push_back(fhRichRings_protons2);



	fhRichRings_start	= new TH1D("fhRichRings_start", "fhRichRings_start;z;#", 1000, 0, 1000);
	fHistoList_richrings.push_back(fhRichRings_start);
}







void CbmAnaConversionRich::Finish()
{
	//gDirectory->cd("analysis-conversion");
	gDirectory->mkdir("RichRings");
	gDirectory->cd("RichRings");
	for (Int_t i = 0; i < fHistoList_richrings.size(); i++){
		fHistoList_richrings[i]->Write();
	}
	gDirectory->cd("..");
	
	cout << "CbmAnaConversionRich: Realtime - " << fTime << endl;
	//timer.Print();
}




void CbmAnaConversionRich::AnalyseRICHdata()
{
	timer.Start();
	
	Int_t nofMC = fMcTracks->GetEntriesFast();
	cout << "CbmAnaConversionRich: nof mc tracks " << nofMC << endl;

	// analyse RICH points
	Int_t nofPoints = fRichPoints->GetEntriesFast();
	for (int i = 0; i < nofPoints; i++) {
		CbmRichPoint* point = (CbmRichPoint*)fRichPoints->At(i);
		if(point == NULL) continue;
		//cout << point->GetX() << "\t" << point->GetY() << endl;
		fTest->Fill(point->GetX(), point->GetY());
	}

	// analyse RICH rings
	if(fRichRings != NULL) {
		Bool_t testbool = true;
		Int_t nofElectrons = 0;
		Int_t sourcePI0 = 0;
		vector<int> pi0ids;
		pi0ids.clear();
		
		Int_t primEl = 0;
		Int_t primMu = 0;
		Int_t primPi = 0;
		Int_t primK = 0;
		
		Int_t nofRings = fRichRings->GetEntriesFast();
		fRichRings_nofRings->Fill(nofRings);
		for (int i = 0; i < nofRings; i++) {
			CbmRichRing* richRing = (CbmRichRing*)fRichRings->At(i);
			if(richRing == NULL) continue;
			fRichRings_Aaxis->Fill(richRing->GetAaxis());
			fRichRings_Baxis->Fill(richRing->GetBaxis());
			fRichRings_distance->Fill(richRing->GetDistance());
			fRichRings_radius->Fill(richRing->GetRadius());
		}

		Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
		cout << "CbmAnaConversionRich: nof global tracks " << nofGlobalTracks << endl;
		for (int iG = 0; iG < nofGlobalTracks; iG++){
			CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(iG);
			if(NULL == gTrack) continue;
			int stsInd = gTrack->GetStsTrackIndex();
			int richInd = gTrack->GetRichRingIndex();
			if (richInd < 0) continue; // no RICH segment -> no ring

			//int trdInd = gTrack->GetTrdTrackIndex();
			//int tofInd = gTrack->GetTofHitIndex();

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

			// get start vertex of track
			TVector3 start;
			mcTrack2->GetStartVertex(start);
			fhRichRings_start->Fill(start.Z());


/*			Int_t index = richRing->GetTrackID();
			if (index < 0) continue;
			CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(index);
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
*/
			int motherId = mcTrack2->GetMotherId();
			int motherpdg = 0;
			CbmMCTrack* mothermcTrack1;

			if(motherId != -1) {
				mothermcTrack1 = (CbmMCTrack*) fMcTracks->At(motherId);
				motherpdg = TMath::Abs(mothermcTrack1->GetPdgCode());
			}
			if(motherId == -1) {
				motherpdg = -1;
				if(pdg == 11) primEl++;
				if(pdg == 13) primMu++;
				if(pdg == 211) primPi++;
				if(pdg == 321) primK++;
			}

			if(pdg == 2212) {
				Protons(mcTrack1);
				//fhRichRings_protons2->Fill(richRing->GetNofHits());
			}

			fRichRings_motherpdg->Fill(motherpdg);
			fRichRings_richpdg->Fill(pdg);

			if(pdg == 11) { // && stsMcTrackId == richMcTrackId) {
				nofElectrons++;
				int grandmotherpdg = 0;
				int grandmotherId = 0;
				if(motherpdg == 22) {
					grandmotherId = mothermcTrack1->GetMotherId();
					if(grandmotherId != -1) {
						CbmMCTrack* grandmothermcTrack1 = (CbmMCTrack*) fMcTracks->At(grandmotherId);
						grandmotherpdg = TMath::Abs(grandmothermcTrack1->GetPdgCode());
					}
					if(grandmotherId == -1) {
						grandmotherpdg = -1;
					}
				}
				if(motherpdg == 111 || grandmotherpdg == 111) {
					sourcePI0++;
					if(motherpdg == 111) {
						pi0ids.push_back(motherId);
					}
					else if(grandmotherpdg == 111) {
						pi0ids.push_back(grandmotherId);
					}
				}
			}
			FillAdditionalPDGhisto(pdg, motherpdg);
		}

		fRichRings_electronspE->Fill(nofElectrons);
		fRichRings_sourcePI0->Fill(sourcePI0);

		TH1I* zwischenhisto = new TH1I("zwischenhisto", "zwischenhisto", 1000000, 0, 1000000);
		for(int i=0; i<pi0ids.size(); i++) {
			zwischenhisto->Fill(pi0ids[i]);
		}
		fRichRings_test->Fill(zwischenhisto->GetMaximum());
		if(zwischenhisto->GetMaximum() >= 4) {
			CbmMCTrack* mcTrack2 = (CbmMCTrack*) fMcTracks->At(zwischenhisto->GetMaximumBin()-1);
			int pdg = mcTrack2->GetPdgCode();
			cout << "CbmAnaConversionRich: MAXIMUM BIN: " << zwischenhisto->GetMaximum() << "\t" << "bin id: " << zwischenhisto->GetMaximumBin()-1 << endl;
			cout << "CbmAnaConversionRich: pdg code: " << pdg << endl;
			cout << "CbmAnaConversionRich: \t";
			
			std::sort(pi0ids.begin(), pi0ids.end());
			for(int i=0; i<pi0ids.size(); i++) {
				cout << pi0ids[i] << "\t";
			}
			
			cout << endl;
			cout << "CbmAnaConversionRich: number of electrons: " << nofElectrons << "\t e from pi0: " << sourcePI0 << "\t pi0ids size: " << pi0ids.size() << endl;
		}
		zwischenhisto->Delete();
		
		
		int photoncounter = 0;
		std::multimap<int,int> electronMap;
		for(int i=0; i<pi0ids.size(); i++) {
			electronMap.insert ( std::pair<int,int>(pi0ids[i], i) );
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
					if(check > 3) {
						cout << "CbmAnaConversionRich: richmap - photon found " << id << endl;
						photoncounter++;
					}
				}
				else check=1;
			}
		}


	}


	// plot momentum vs ring radius/Axis+Baxis
	Int_t nGTracks = fGlobalTracks->GetEntriesFast();
	for (Int_t i = 0; i < nGTracks; i++){
		CbmGlobalTrack* gTrack  = (CbmGlobalTrack*) fGlobalTracks->At(i);
		if(NULL == gTrack) continue;
		Int_t richInd = gTrack->GetRichRingIndex();
		if (richInd < 0) continue;
		CbmRichRing* richRing = (CbmRichRing*) fRichRings->At(richInd);

		CbmTrackMatchNew* richMatch  = (CbmTrackMatchNew*) fRichRingMatches->At(richInd);
		if (richMatch == NULL) continue;
		Int_t richMcTrackId = richMatch->GetMatchedLink().GetIndex();
		
		CbmMCTrack* mctrack = (CbmMCTrack*)fMcTracks->At(richMcTrackId);
		if (mctrack == NULL) continue;   
		
		Double_t momentum = mctrack->GetP();
		Double_t radius = richRing->GetRadius();
		Double_t axisA = richRing->GetAaxis();
		Double_t axisB = richRing->GetBaxis();
		
		fhRingtest->Fill(momentum, radius);
		fhRichRings_AaxisVSmom->Fill(momentum, axisA);
		fhRichRings_BaxisVSmom->Fill(momentum, axisB);
		
		if(richRing->GetPhi() <= 5) {
			fhRichRings_test1->Fill(axisA);
		}
		
		Double_t ringX = richRing->GetCenterX();
		Double_t ringY = richRing->GetCenterY();
		
		if(sqrt(ringX*ringX + (abs(ringY)-105)*(abs(ringY)-105)) <= 50) {
			fhRichRings_test2->Fill(axisA);
		}
		fhRichRings_test3->Fill(richRing->GetAngle());
		fhRichRings_pos->Fill(ringX, ringY);
		
		if(abs(ringX) <= 30 && abs(ringY) <= 130) {
			fRichRings_Aaxis_part1->Fill(axisA);
			fRichRings_Baxis_part1->Fill(axisB);
		}
		if(abs(ringX) > 30 && abs(ringY) > 130 && abs(ringX) <= 70 && abs(ringY) <= 155) {
			fRichRings_Aaxis_part2->Fill(axisA);
			fRichRings_Baxis_part2->Fill(axisB);
		}
		if(abs(ringX) > 70 && abs(ringY) > 155) {
			fRichRings_Aaxis_part3->Fill(axisA);
			fRichRings_Baxis_part3->Fill(axisB);
		}
	}

	CheckMC();

	timer.Stop();
	fTime += timer.RealTime();
}




void CbmAnaConversionRich::FillAdditionalPDGhisto(Int_t pdg, Int_t motherpdg) 
{
	if(pdg == 11)	fRichRings_detectedParticles->Fill(0);
	if(pdg == 13)	fRichRings_detectedParticles->Fill(1);
	if(pdg == 211)	fRichRings_detectedParticles->Fill(2);
	if(pdg == 321)	fRichRings_detectedParticles->Fill(3);
	if(pdg == 2212)	fRichRings_detectedParticles->Fill(4);
	if(pdg == 3112)	fRichRings_detectedParticles->Fill(5);
	if(pdg == 3222)	fRichRings_detectedParticles->Fill(6);
	if(pdg == 3312)	fRichRings_detectedParticles->Fill(7);
	if(pdg != 11 && pdg != 13 && pdg != 211 && pdg != 321 && pdg != 2212 && pdg != 3112 && pdg != 3222 && pdg != 3312) fRichRings_detectedParticles->Fill(8);

	if(motherpdg == 11)		fRichRings_detParticlesMother->Fill(0);
	if(motherpdg == 13)		fRichRings_detParticlesMother->Fill(1);
	if(motherpdg == 22)		fRichRings_detParticlesMother->Fill(2);
	if(motherpdg == 111)	fRichRings_detParticlesMother->Fill(3);
	if(motherpdg == 130)	fRichRings_detParticlesMother->Fill(4);
	if(motherpdg == 211)	fRichRings_detParticlesMother->Fill(5);
	if(motherpdg == 221)	fRichRings_detParticlesMother->Fill(6);
	if(motherpdg == 310)	fRichRings_detParticlesMother->Fill(7);
	if(motherpdg == 321)	fRichRings_detParticlesMother->Fill(8);
	if(motherpdg == 2112)	fRichRings_detParticlesMother->Fill(9);
	if(motherpdg == 2212)	fRichRings_detParticlesMother->Fill(10);
	if(motherpdg == 3112)	fRichRings_detParticlesMother->Fill(11);
	if(motherpdg == 3122)	fRichRings_detParticlesMother->Fill(12);
	if(motherpdg == 3222)	fRichRings_detParticlesMother->Fill(13);
	if(motherpdg == 3312)	fRichRings_detParticlesMother->Fill(14);
	if(motherpdg == 3334)	fRichRings_detParticlesMother->Fill(15);
	if(motherpdg == -1) {
		fRichRings_detParticlesMother->Fill(16);
		if(pdg == 11)	fRichRings_detParticlesMother->Fill(17);
		if(pdg == 13)	fRichRings_detParticlesMother->Fill(18);
		if(pdg == 211)	fRichRings_detParticlesMother->Fill(19);
		if(pdg == 321)	fRichRings_detParticlesMother->Fill(20);
		//if(pdg != 11 && pdg != 13 && pdg != 211 && pdg != 321 && pdg != 2112 && pdg != 2212 && pdg != 3112 && pdg != 3122 && pdg != 3222 && pdg != 3312 && pdg != 3334) fRichRings_detParticlesMother->Fill(21);
		if(pdg != 11 && pdg != 13 && pdg != 211 && pdg != 321) fRichRings_detParticlesMother->Fill(21);
	}
	
	
}



void CbmAnaConversionRich::Protons(CbmMCTrack* mcTrack)
{
	fhRichRings_protons->Fill(mcTrack->GetPx(), mcTrack->GetPy());
	

}


void CbmAnaConversionRich::CheckMC() 
{
	vector<int> ids;
	vector<int> electronids;
	ids.clear();
	electronids.clear();

	Int_t nofGlobalTracks = fGlobalTracks->GetEntriesFast();
	for (int iG = 0; iG < nofGlobalTracks; iG++){
		CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(iG);
		if(NULL == gTrack) continue;
		int stsInd = gTrack->GetStsTrackIndex();
		int richInd = gTrack->GetRichRingIndex();
	//	if (richInd < 0) continue; // no RICH segment -> no ring
		if (stsInd < 0) continue;

		CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(stsInd);
		if (stsTrack == NULL) continue;
		CbmTrackMatchNew* stsMatch  = (CbmTrackMatchNew*)fStsTrackMatches->At(stsInd);
		if (stsMatch == NULL) continue;
		int stsMcTrackId = stsMatch->GetMatchedLink().GetIndex();
		if (stsMcTrackId < 0) continue;
		CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMcTracks->At(stsMcTrackId);
		if (mcTrack1 == NULL) continue;
		
	/*	CbmTrackMatchNew* richMatch  = (CbmTrackMatchNew*)fRichRingMatches->At(richInd);
		if (richMatch == NULL) continue;
		int richMcTrackId = richMatch->GetMatchedLink().GetIndex();
		if (richMcTrackId < 0) continue;
		CbmMCTrack* mcTrack2 = (CbmMCTrack*) fMcTracks->At(richMcTrackId);
		if (mcTrack2 == NULL) continue;
			// stsMcTrackId == richMcTrackId -> track was reconstructed in STS and made a ring in RICH, track matching was correct
			// in case they are not equal, the ring comes either from a secondary particle or STS track was not reconstructed
	//	if(stsMcTrackId != richMcTrackId) continue;
*/
		int pdg = TMath::Abs(mcTrack1->GetPdgCode());	// extract pdg code of particle directly from rich ring
	//	int pdg2 = TMath::Abs(mcTrack2->GetPdgCode());

	//	if(pdg != pdg2) {
	//		cout << "PDGs not matching!" << endl;
	//	}

		int motherId = mcTrack1->GetMotherId();
	//	int motherId2 = mcTrack2->GetMotherId();
		
	//	if(motherId != motherId2) {
	//		cout << "MotherIDs not matching!" << endl;
	//	}

		int motherpdg = 0;
		int grandmotherpdg = 0;
		CbmMCTrack* mothermcTrack1;
		CbmMCTrack* grandmothermcTrack1;
		int grandmotherId = 0;

		if(motherId != -1) {
			mothermcTrack1 = (CbmMCTrack*) fMcTracks->At(motherId);
			motherpdg = TMath::Abs(mothermcTrack1->GetPdgCode());
		}
		else if(motherId == -1) {
			motherpdg = -1;
		}

		if(pdg == 11) {
			if(motherpdg == 22) {
				grandmotherId = mothermcTrack1->GetMotherId();
				if(grandmotherId != -1) {
					grandmothermcTrack1 = (CbmMCTrack*) fMcTracks->At(grandmotherId);
					grandmotherpdg = TMath::Abs(grandmothermcTrack1->GetPdgCode());
				}
				if(grandmotherId == -1) {
					grandmotherpdg = -1;
				}
			}
		
			if(motherpdg == 111) {
				ids.push_back(motherId);
				electronids.push_back(stsMcTrackId);
			}
			if(motherpdg == 22 && grandmotherpdg == 111) {
				ids.push_back(grandmotherId);
				electronids.push_back(stsMcTrackId);
			}
		}
	}

	cout << "CbmAnaConversionRich: vector ids contains " << ids.size() << " entries." << endl;
	cout << "CbmAnaConversionRich: ids entries: ";
	for(int i=0; i<ids.size(); i++) {
		cout << ids[i] << " / ";
	}
	cout << endl;	
	cout << "CbmAnaConversionRich: electronids entries: ";
	for(int i=0; i<electronids.size(); i++) {
		cout << electronids[i] << " / ";
	}
	cout << endl;

	int photoncounter = 0;
	std::multimap<int,int> electronMap;
	for(int i=0; i<ids.size(); i++) {
		electronMap.insert ( std::pair<int,int>(ids[i], i) );
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
				if(check > 3) {
					cout << "CbmAnaConversionRich: richmap - photon found " << id << endl;
					photoncounter++;
				}
			}
			else check=1;
		}
	}
}




