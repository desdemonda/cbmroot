/**
 * \file CbmAnaConversionTomography.cxx
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2014
 * Using the true-MC data to do some tomography studies,
 * via conversion of gammas (gamma -> e+ e-)
 * somewhere in the detector material and plotting
 * the point of conversion.
 **/

#include "CbmAnaConversionTomography.h"

#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include <iostream>
#include <map>


using namespace std;



CbmAnaConversionTomography::CbmAnaConversionTomography()
  : fMcTracks(NULL),
    fHistoList_tomography(),
    fhGammaZ(NULL),
    fhTomography(NULL),
    fhTomography_XZ(NULL),
    fhTomography_YZ(NULL),
    fhTomography_uptoRICH(NULL),
    fhTomography_RICH_complete(NULL),
    fhTomography_RICH_beampipe(NULL),
    fhTomography_STS_end(NULL),
    fhTomography_STS_lastStation(NULL),
    fhTomography_RICH_frontplate(NULL),
    fhTomography_RICH_backplate(NULL),
    fhConversion(NULL),
    fhConversion_inSTS(NULL),
    fhConversion_prob(NULL),
    fhConversion_energy(NULL),
    fhConversion_p(NULL),
    fhTomography_reco(NULL),
    fhTomography_reco_XZ(NULL),
    fhTomography_reco_YZ(NULL),
    fhConversion_reco(NULL),
	electronIDs(),
	electronMotherIDs(),
	conversionsInDetector(),
	//conversionsInDetector_cut(),
	fhConversionsPerDetector(NULL),
	fhConversionsPerDetectorPE(NULL),
	//fhConversionsPerDetectorPE_cut(NULL),
    timer(),
    fTime(0.)
{
}

CbmAnaConversionTomography::~CbmAnaConversionTomography()
{
}


void CbmAnaConversionTomography::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) {Fatal("CbmAnaConversionTomography::Init", "RootManager not instantised!");}

	fMcTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( NULL == fMcTracks) { Fatal("CbmAnaConversion::Init","No MCTrack array!"); }


	InitHistos();
}


void CbmAnaConversionTomography::InitHistos()
{
	fHistoList_tomography.clear();

	fhGammaZ = new TH1D("fhGammaZ", "fhGammaZ;Z [cm];Entries", 300, 0.0, 300.);
	fHistoList_tomography.push_back(fhGammaZ);
	
	// for all events including gammas and gamma-conversion
	fhTomography		= new TH3D("fhTomography", "Tomography/fhTomography;X [cm];Y [cm];Z [cm]", 200, -200., 200., 200, -200., 200., 500, 0., 1000.);
	fhTomography_XZ		= new TH2D("fhTomography_XZ", "fhTomography_XZ;X [cm];Z [cm]", 2800, -700., 700., 2200, 0., 1100.);
	fhTomography_YZ		= new TH2D("fhTomography_YZ", "fhTomography_YZ;Y [cm];Z [cm]", 2000, -500., 500., 2200, 0., 1100.);
	//fhTomography_XZ_cut	= new TH2D("fhTomography_XZ_cut", "fhTomography_XZ_cut;X [cm];Z [cm]", 2800, -700., 700., 2200, 0., 1100.);
	//fhTomography_YZ_cut	= new TH2D("fhTomography_YZ_cut", "fhTomography_YZ_cut;Y [cm];Z [cm]", 2000, -500., 500., 2200, 0., 1100.);
	fHistoList_tomography.push_back(fhTomography);
	fHistoList_tomography.push_back(fhTomography_XZ);
	fHistoList_tomography.push_back(fhTomography_YZ);
	//fHistoList_tomography.push_back(fhTomography_XZ_cut);
	//fHistoList_tomography.push_back(fhTomography_YZ_cut);
	
	fhTomography_uptoRICH			= new TH2D("fhTomography_uptoRICH", "fhTomography_uptoRICH;X [cm];Y [cm]", 400, -200., 200., 400, -200., 200.);
	fhTomography_RICH_complete		= new TH2D("fhTomography_RICH_complete", "fhTomography_RICH_complete;X [cm];Y [cm]", 400, -200., 200., 400, -200., 200.);
	fhTomography_RICH_beampipe		= new TH2D("fhTomography_RICH_beampipe", "fhTomography_RICH_beampipe;X [cm];Y [cm]", 400, -100., 100., 200, -50., 50.);
	fhTomography_STS_end			= new TH2D("fhTomography_STS_end", "fhTomography_STS_end;X [cm];Y [cm]", 400, -200., 200., 200, -100., 100.);
	fhTomography_STS_lastStation	= new TH2D("fhTomography_STS_lastStation", "fhTomography_STS_lastStation;X [cm];Y [cm]", 200, -100., 100., 200, -100., 100.);
	fhTomography_RICH_frontplate	= new TH2D("fhTomography_RICH_frontplate", "fhTomography_RICH_frontplate;X [cm];Y [cm]", 400, -200., 200., 400, -200., 200.);
	fhTomography_RICH_backplate		= new TH2D("fhTomography_RICH_backplate", "fhTomography_RICH_backplate;X [cm];Y [cm]", 400, -200., 200., 400, -200., 200.);
	fHistoList_tomography.push_back(fhTomography_uptoRICH);
	fHistoList_tomography.push_back(fhTomography_RICH_complete);
	fHistoList_tomography.push_back(fhTomography_RICH_beampipe);
	fHistoList_tomography.push_back(fhTomography_STS_end);
	fHistoList_tomography.push_back(fhTomography_STS_lastStation);
	fHistoList_tomography.push_back(fhTomography_RICH_frontplate);
	fHistoList_tomography.push_back(fhTomography_RICH_backplate);
	
	
	
	fhConversion		= new TH1D("fhConversion", "fhConversion;Z [cm];# conversions", 4800, -0.5, 1199.5);
	fhConversion_inSTS	= new TH1D("fhConversion_inSTS", "fhConversion in STS;Z [cm];# conversions", 800, -0.5, 199.5);
	fhConversion_prob	= new TH1D("fhConversion_prob", "fhConversion_prob;Z [cm];# conversions", 1200, -0.5, 1199.5);
	fHistoList_tomography.push_back(fhConversion);
	fHistoList_tomography.push_back(fhConversion_inSTS);
	fHistoList_tomography.push_back(fhConversion_prob);
	
	fhConversion_energy	= new TH1D("fhConversion_energy", "fhConversion_energy;energy;#", 1000, 0., 100.);
	fhConversion_p		= new TH1D("fhConversion_p", "fhConversion_p;p;#", 1000, 0., 100.);
	fHistoList_tomography.push_back(fhConversion_energy);
	fHistoList_tomography.push_back(fhConversion_p);
	
	fhConversionsPerDetector	= new TH1I("fhConversionsPerDetector", "fhConversionsPerDetector;;#", 5, 0, 5);
	fHistoList_tomography.push_back(fhConversionsPerDetector);
	fhConversionsPerDetector->GetXaxis()->SetBinLabel(1, "magnet");
	fhConversionsPerDetector->GetXaxis()->SetBinLabel(2, "STS");
	fhConversionsPerDetector->GetXaxis()->SetBinLabel(3, "RICH");
	fhConversionsPerDetector->GetXaxis()->SetBinLabel(4, "TRD");
	fhConversionsPerDetector->GetXaxis()->SetBinLabel(5, "TOF");
	
	fhConversionsPerDetectorPE	= new TH2I("fhConversionsPerDetectorPE", "fhConversionsPerDetectorPE;;conversions per event", 5, 0, 5, 1500, 0, 1500);
	fHistoList_tomography.push_back(fhConversionsPerDetectorPE);
	fhConversionsPerDetectorPE->GetXaxis()->SetBinLabel(1, "magnet");
	fhConversionsPerDetectorPE->GetXaxis()->SetBinLabel(2, "STS");
	fhConversionsPerDetectorPE->GetXaxis()->SetBinLabel(3, "RICH");
	fhConversionsPerDetectorPE->GetXaxis()->SetBinLabel(4, "TRD");
	fhConversionsPerDetectorPE->GetXaxis()->SetBinLabel(5, "TOF");
	
	//fhConversionsPerDetectorPE_cut	= new TH2I("fhConversionsPerDetectorPE_cut", "fhConversionsPerDetectorPE_cut;;conversions per event", 5, 0, 5, 1500, 0, 1500);
	//fHistoList_tomography.push_back(fhConversionsPerDetectorPE_cut);
	//fhConversionsPerDetectorPE_cut->GetXaxis()->SetBinLabel(1, "magnet");
	//fhConversionsPerDetectorPE_cut->GetXaxis()->SetBinLabel(2, "STS");
	//fhConversionsPerDetectorPE_cut->GetXaxis()->SetBinLabel(3, "RICH");
	//fhConversionsPerDetectorPE_cut->GetXaxis()->SetBinLabel(4, "TRD");
	//fhConversionsPerDetectorPE_cut->GetXaxis()->SetBinLabel(5, "TOF");
	
	// tomography from reconstructed tracks
	fhTomography_reco		= new TH3D("fhTomography_reco", "fhTomography_reco;X [cm];Y [cm];Z [cm]", 200, -200., 200., 200, -200., 200., 500, 0., 1000.);
	fhTomography_reco_XZ	= new TH2D("fhTomography_reco_XZ", "fhTomography_reco_XZ;X [cm];Z [cm]", 1600, -400., 400., 2400, 0., 1200.);
	fhTomography_reco_YZ	= new TH2D("fhTomography_reco_YZ", "fhTomography_reco_YZ;Y [cm];Z [cm]", 1600, -400., 400., 2400, 0., 1200.);
	fhConversion_reco		= new TH1D("fhConversion_reco", "fhConversion_reco;Z [cm];# conversions", 4800, -0.5, 1199.5);
	fHistoList_tomography.push_back(fhTomography_reco);
	fHistoList_tomography.push_back(fhTomography_reco_XZ);
	fHistoList_tomography.push_back(fhTomography_reco_YZ);
	fHistoList_tomography.push_back(fhConversion_reco);
	
}


void CbmAnaConversionTomography::Finish()
{
	//gDirectory->cd("analysis-conversion");
	gDirectory->mkdir("Tomography");
	gDirectory->cd("Tomography");
	for (Int_t i = 0; i < fHistoList_tomography.size(); i++){
		fHistoList_tomography[i]->Write();
	}
	gDirectory->cd("..");
	
	cout << "CbmAnaConversionTomography: Realtime - " << fTime << endl;
	//timer.Print();
}



void CbmAnaConversionTomography::Exec() 
{
	timer.Start();

	electronIDs.clear();
	electronMotherIDs.clear();
	conversionsInDetector[0] = 0;	// magnet
	conversionsInDetector[1] = 0;	// sts
	conversionsInDetector[2] = 0;	// rich
	conversionsInDetector[3] = 0;	// trd
	conversionsInDetector[4] = 0;	// tof

	//conversionsInDetector_cut[0] = 0;	// magnet
	//conversionsInDetector_cut[1] = 0;	// sts
	//conversionsInDetector_cut[2] = 0;	// rich
	//conversionsInDetector_cut[3] = 0;	// trd
	//conversionsInDetector_cut[4] = 0;	// tof

	Int_t nofMcTracks = fMcTracks->GetEntriesFast();
	for (int i = 0; i < nofMcTracks; i++) {
		CbmMCTrack* mctrack = (CbmMCTrack*)fMcTracks->At(i);
		if (mctrack == NULL) continue;

		if (TMath::Abs( mctrack->GetPdgCode())  == 11) {
			int motherId = mctrack->GetMotherId();
			if (motherId == -1) continue;
			CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
			int mcMotherPdg  = -1;
			if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
			
			electronIDs.push_back(i);
			electronMotherIDs.push_back(motherId);
		}   
	}
	
	
	int photoncounter = 0;
	std::multimap<int,int> electronMap;
	for(int i=0; i<electronIDs.size(); i++) {
		electronMap.insert ( std::pair<int,int>(electronMotherIDs[i], electronIDs[i]) );
	}
	
	int check = 0;
	for(std::map<int,int>::iterator it=electronMap.begin(); it!=electronMap.end(); ++it) {
		if(it == electronMap.begin()) {
			TomographyMC(it->second);
		}
		else {
			std::map<int,int>::iterator zwischen = it;
			zwischen--;
			int id = it->first;
			int id_old = zwischen->first;
			if(id == id_old) continue;
			else {
				TomographyMC(it->second);
			}
		}
	}

	fhConversionsPerDetectorPE->Fill(1.0*0, 1.0*conversionsInDetector[0]);
	fhConversionsPerDetectorPE->Fill(1.0*1, 1.0*conversionsInDetector[1]);
	fhConversionsPerDetectorPE->Fill(1.0*2, 1.0*conversionsInDetector[2]);
	fhConversionsPerDetectorPE->Fill(1.0*3, 1.0*conversionsInDetector[3]);
	fhConversionsPerDetectorPE->Fill(1.0*4, 1.0*conversionsInDetector[4]);
	
	//fhConversionsPerDetectorPE_cut->Fill(1.0*0, 1.0*conversionsInDetector_cut[0]);
	//fhConversionsPerDetectorPE_cut->Fill(1.0*1, 1.0*conversionsInDetector_cut[1]);
	//fhConversionsPerDetectorPE_cut->Fill(1.0*2, 1.0*conversionsInDetector_cut[2]);
	//fhConversionsPerDetectorPE_cut->Fill(1.0*3, 1.0*conversionsInDetector_cut[3]);
	//fhConversionsPerDetectorPE_cut->Fill(1.0*4, 1.0*conversionsInDetector_cut[4]);


	timer.Stop();
	fTime += timer.RealTime();
}

/*
void CbmAnaConversionTomography::TomographyMC(CbmMCTrack* mctrack)
// doing tomography from gamma -> e+ e- decays, MC DATA
{
	if (TMath::Abs( mctrack->GetPdgCode())  == 11) {
		int motherId = mctrack->GetMotherId();
		if (motherId != -1) {
			CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
			int mcMotherPdg  = -1;
			if (NULL != mother) mcMotherPdg = mother->GetPdgCode();

			if (mcMotherPdg == 22) {
				TVector3 v;
				mctrack->GetStartVertex(v);
				fhGammaZ->Fill(v.Z());


				fhTomography->Fill(v.X(), v.Y(), v.Z());
				fhTomography_XZ->Fill(v.X(), v.Z());
				fhTomography_YZ->Fill(v.Y(), v.Z());
				fhConversion_energy->Fill(mctrack->GetEnergy());
				fhConversion_p->Fill(mctrack->GetP());

				if(v.z() >= 0 && v.Z() <= 170) { // only in region before the RICH
					fhTomography_uptoRICH->Fill(v.X(), v.Y());
				}
				if(v.z() >= 170 && v.Z() <= 400) { // only in region of the RICH detector
					fhTomography_RICH_complete->Fill(v.X(), v.Y());
				}
				if(v.z() >= 220 && v.Z() <= 280) { // only in region of RICH beampipe, without being distorted by mirrors or camera
					fhTomography_RICH_beampipe->Fill(v.X(), v.Y());
				}
				if(v.z() >= 100 && v.Z() <= 170) { // only in a downstream part of STS and magnet
					fhTomography_STS_end->Fill(v.X(), v.Y());
				}
				if(v.z() >= 98 && v.Z() <= 102) { // only in a downstream part of STS and magnet, only last STS station
					fhTomography_STS_lastStation->Fill(v.X(), v.Y());
				}
				if(v.z() >= 179 && v.Z() <= 181) { // only in region of RICH frontplate
					fhTomography_RICH_frontplate->Fill(v.X(), v.Y());
				}
				if(v.z() >= 369 && v.Z() <= 371) { // only in region of RICH backplate
					fhTomography_RICH_backplate->Fill(v.X(), v.Y());
				}

				fhConversion->Fill(v.Z());

				if( (TMath::Abs(v.X()) <= 100 && TMath::Abs(v.X()) > 3) && (TMath::Abs(v.Y()) <= 50 && TMath::Abs(v.Y()) > 3) && TMath::Abs(v.Z()) <= 169) {
					fhConversion_inSTS->Fill(v.Z());
				}
			}
		}
	}
}
*/




void CbmAnaConversionTomography::TomographyMC(int electronID)
// doing tomography from gamma -> e+ e- decays, MC DATA
{

	CbmMCTrack* mctrack = (CbmMCTrack*) fMcTracks->At(electronID);
			
	TVector3 v;
	mctrack->GetStartVertex(v);
	fhGammaZ->Fill(v.Z());


	fhTomography->Fill(v.X(), v.Y(), v.Z());
	fhTomography_XZ->Fill(v.X(), v.Z());
	fhTomography_YZ->Fill(v.Y(), v.Z());
	//if(GetNPoints(mctrack)) {
	//	fhTomography_XZ_cut->Fill(v.X(), v.Z());
	//	fhTomography_YZ_cut->Fill(v.Y(), v.Z());
	//}
	fhConversion_energy->Fill(mctrack->GetEnergy());
	fhConversion_p->Fill(mctrack->GetP());

	if(v.z() >= 0 && v.Z() <= 170) { // only in region before the RICH
		fhTomography_uptoRICH->Fill(v.X(), v.Y());
	}
	if(v.z() >= 170 && v.Z() <= 400) { // only in region of the RICH detector
		fhTomography_RICH_complete->Fill(v.X(), v.Y());
	}
	if(v.z() >= 220 && v.Z() <= 280) { // only in region of RICH beampipe, without being distorted by mirrors or camera
		fhTomography_RICH_beampipe->Fill(v.X(), v.Y());
	}
	if(v.z() >= 100 && v.Z() <= 170) { // only in a downstream part of STS and magnet
		fhTomography_STS_end->Fill(v.X(), v.Y());
	}
	if(v.z() >= 98 && v.Z() <= 102) { // only in a downstream part of STS and magnet, only last STS station
		fhTomography_STS_lastStation->Fill(v.X(), v.Y());
	}
	if(v.z() >= 179 && v.Z() <= 181) { // only in region of RICH frontplate
		fhTomography_RICH_frontplate->Fill(v.X(), v.Y());
	}
	if(v.z() >= 369 && v.Z() <= 371) { // only in region of RICH backplate
		fhTomography_RICH_backplate->Fill(v.X(), v.Y());
	}

	fhConversion->Fill(v.Z());

	if( (TMath::Abs(v.X()) <= 100 && TMath::Abs(v.X()) > 3) && (TMath::Abs(v.Y()) <= 50 && TMath::Abs(v.Y()) > 3) && TMath::Abs(v.Z()) <= 169) {
		fhConversion_inSTS->Fill(v.Z());
	}
	
	
	if(v.z() >= 170 && v.Z() <= 400) { // only in region of the RICH detector
		conversionsInDetector[2]++;
		fhConversionsPerDetector->Fill(2);
		//if(GetNPoints(mctrack)) { conversionsInDetector_cut[2]++; }
	}
	if(v.z() >= 405 && v.Z() <= 870) { // only in region of the TRD detector
		conversionsInDetector[3]++;
		fhConversionsPerDetector->Fill(3);
		//if(GetNPoints(mctrack)) { conversionsInDetector_cut[3]++; }
	}
	if(v.z() > 870 && v.Z() <= 1010) { // only in region of the TOF detector
		conversionsInDetector[4]++;
		fhConversionsPerDetector->Fill(4);
		//if(GetNPoints(mctrack)) { conversionsInDetector_cut[4]++; }
	}
	if( (TMath::Abs(v.X()) <= 100 && TMath::Abs(v.X()) > 3) && (TMath::Abs(v.Y()) <= 65 && TMath::Abs(v.Y()) > 3) && TMath::Abs(v.Z()) <= 105) {	// STS
		conversionsInDetector[1]++;
		fhConversionsPerDetector->Fill(1);
		//if(GetNPoints(mctrack)) { conversionsInDetector_cut[1]++; }
	}
	if( (TMath::Abs(v.X()) > 100 || TMath::Abs(v.Y()) > 65) && TMath::Abs(v.Z()) <= 169) {		// magnet
		conversionsInDetector[0]++;
		fhConversionsPerDetector->Fill(0);
		//if(GetNPoints(mctrack)) { conversionsInDetector_cut[0]++; }
	}
}




void CbmAnaConversionTomography::TomographyReco(CbmMCTrack* mctrack)
// doing tomography from gamma -> e+ e- decays, RECONSTRUCTED TRACKS WITH MC DATA
{
	if (TMath::Abs( mctrack->GetPdgCode())  == 11) {
		int motherId = mctrack->GetMotherId();
		if (motherId != -1) {
			CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
			int mcMotherPdg  = -1;
			if (NULL != mother) mcMotherPdg = mother->GetPdgCode();

			if (mcMotherPdg == 22) {
				TVector3 v;
				mctrack->GetStartVertex(v);
			  
				fhTomography_reco->Fill(v.X(), v.Y(), v.Z());
				fhTomography_reco_XZ->Fill(v.X(), v.Z());
				fhTomography_reco_YZ->Fill(v.Y(), v.Z());
				fhConversion_reco->Fill(v.Z());
			}
		}
	}
}


Bool_t CbmAnaConversionTomography::GetNPoints(CbmMCTrack* mctrack)
{
	Double_t np_sts = mctrack->GetNPoints(kSTS);
	Double_t np_rich = mctrack->GetNPoints(kRICH);
	Double_t np_trd = mctrack->GetNPoints(kTRD);
	Double_t np_tof = mctrack->GetNPoints(kTOF);
	
	Bool_t result = false;
	if(np_sts > 0 || np_rich > 0 || np_trd > 0 || np_tof > 0) {
		result = true;
	}
	
	return result;
}



