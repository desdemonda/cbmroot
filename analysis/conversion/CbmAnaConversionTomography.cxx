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
	fhTomography	= new TH3D("fhTomography", "Tomography/fhTomography;X [cm];Y [cm];Z [cm]", 200, -200., 200., 200, -200., 200., 500, 0., 1000.);
	fhTomography_XZ	= new TH2D("fhTomography_XZ", "fhTomography_XZ;X [cm];Z [cm]", 2400, -600., 600., 2200, 0., 1100.);
	fhTomography_YZ	= new TH2D("fhTomography_YZ", "fhTomography_YZ;Y [cm];Z [cm]", 2000, -500., 500., 2200, 0., 1100.);
	fHistoList_tomography.push_back(fhTomography);
	fHistoList_tomography.push_back(fhTomography_XZ);
	fHistoList_tomography.push_back(fhTomography_YZ);
	
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
	
	
	
	// tomography from reconstructed tracks
	fhTomography_reco		= new TH3D("fhTomography_reco", "fhTomography_reco;X [cm];Y [cm];Z [cm]", 200, -200., 200., 200, -200., 200., 500, 0., 1000.);
	fhTomography_reco_XZ	= new TH2D("fhTomography_reco_XZ", "fhTomography_reco_XZ;X [cm];Z [cm]", 1600, -400., 400., 2400, 0., 1200.);
	fhTomography_reco_YZ	= new TH2D("fhTomography_reco_YZ", "fhTomography_reco_YZ;Y [cm];Z [cm]", 1600, -400., 400., 2400, 0., 1200.);
	fHistoList_tomography.push_back(fhTomography_reco);
	fHistoList_tomography.push_back(fhTomography_reco_XZ);
	fHistoList_tomography.push_back(fhTomography_reco_YZ);
	
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
	timer.Print();
}



void CbmAnaConversionTomography::DoTomography() 
{

}





void CbmAnaConversionTomography::TomographyMC(CbmMCTrack* mctrack)
// doing tomography from gamma -> e+ e- decays, MC DATA
{
	timer.Start();

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

	timer.Stop();
	fTime += timer.RealTime();
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
			}
		}
	}
}





