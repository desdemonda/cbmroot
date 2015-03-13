/**
 * \file CbmAnaConversionKF.cxx
 *
 * This class only extract some results from the KFParticle package,
 * which are relevant for the conversion analysis.
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2014
 **/

#include "CbmAnaConversionKF.h"


// included from CbmRoot
#include "FairRootManager.h"
#include "CbmKFParticleFinder.h"
#include "CbmKFParticleFinderQA.h"
#include "KFParticleTopoReconstructor.h"



using namespace std;



CbmAnaConversionKF::CbmAnaConversionKF()
 : fKFparticle(NULL),
   fKFparticleFinderQA(NULL),
   fKFtopo(NULL),
   trackindexarray(),
   particlecounter(0),
   particlecounter_2daughters(0),
   particlecounter_3daughters(0),
   particlecounter_4daughters(0),
   particlecounter_all(0),
   fhPi0_NDaughters(NULL),
   fNofGeneratedPi0_allEvents(0),
   fNofPi0_kfparticle_allEvents(0),
   fNofGeneratedPi0(0),
   fNofPi0_kfparticle(0),
   fhPi0Ratio(NULL),
   fhPi0_mass(NULL),
   fSignalIds(),
   fGhostIds(),
   fHistoList_kfparticle()
{
}

CbmAnaConversionKF::~CbmAnaConversionKF()
{
}


void CbmAnaConversionKF::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) { Fatal("CbmAnaConversion::Init","RootManager not instantised!"); }


	fKFtopo = fKFparticle->GetTopoReconstructor();

	InitHistos();
}


void CbmAnaConversionKF::InitHistos()
{
	fHistoList_kfparticle.clear();

	// #############################################
	// Histograms related to KFParticle results
	fhPi0_NDaughters	= new TH1D("fhPi0_NDaughters","fhPi0_NDaughters;number of daughers;#", 4, 0.5, 4.5);
	fhPi0Ratio			= new TH1D("fhPi0Ratio", "fhPi0Ratio; ratio;#", 1000, 0., 0.02);
	fhPi0_mass			= new TH1D("fhPi0_mass", "fhPi0_mass;mass;#", 500, 0., 0.5);
	fHistoList_kfparticle.push_back(fhPi0_NDaughters);
	fHistoList_kfparticle.push_back(fhPi0Ratio);
	fHistoList_kfparticle.push_back(fhPi0_mass);


}







void CbmAnaConversionKF::Finish()
{
	//gDirectory->cd("analysis-conversion");
	gDirectory->mkdir("KF");
	gDirectory->cd("KF");
	for (Int_t i = 0; i < fHistoList_kfparticle.size(); i++){
		fHistoList_kfparticle[i]->Write();
	}
	gDirectory->cd("..");
}



void CbmAnaConversionKF::SetKF(CbmKFParticleFinder* kfparticle, CbmKFParticleFinderQA* kfparticleQA)
{
	fKFparticle = kfparticle;
	fKFparticleFinderQA = kfparticleQA;
	if(fKFparticle) {
		cout << "CbmAnaConversionKF: kf works" << endl;
	}
	else {
		cout << "CbmAnaConversionKF: kf works not" << endl;
	}
}


void CbmAnaConversionKF::SetSignalIds(std::vector<int> *signalids) 
{  
	fSignalIds.clear();
	fSignalIds = *signalids;
}

void CbmAnaConversionKF::SetGhostIds(std::vector<int> *ghostids) 
{
	fGhostIds.clear();
	fGhostIds = *ghostids;
}




void CbmAnaConversionKF::KFParticle_Analysis()
{
	int testkf = fKFtopo->NPrimaryVertices();
	cout << "KFParticle_Analysis - test kf NPrimaryVertices: " << testkf << endl;

	const KFPTrackVector* kftrackvector;
	kftrackvector = fKFtopo->GetTracks();
	cout << "KFParticle_Analysis - trackvector: " << kftrackvector->Size() << endl;
	KFParticle testparticle;
	KFPTrack testtrack;
	const int bla = 2;
	kftrackvector->GetTrack(testtrack, bla);
	cout << "KFParticle_Analysis - test p: " << testtrack.GetP() << endl;
	kfvector_int pdgvector;
	pdgvector = kftrackvector->PDG();
	//cout << "pdg tests: " << pdgvector[1] << "\t" << pdgvector[2] << "\t" << pdgvector[3] << "\t" << pdgvector[4] << endl;
	
	
	int nofpions = 0;
	//nofpions = kftrackvector->NPions();
	cout << "KFParticle_Analysis - number of pions: " << nofpions << endl;
	
	int testpi = 0;
	for(int i=0; i<kftrackvector->Size(); i++) {
		if(TMath::Abs(pdgvector[i]) == 211) testpi++;
	}
	cout << "KFParticle_Analysis - testpi: " << testpi << endl;
	
	//kftrackvector->PrintTracks();
	
	
	vector<KFParticle> particlevector;
	particlevector = fKFtopo->GetParticles();
	

	cout << "KFParticle_Analysis - particlevector size: " << particlevector.size() << "\t";
	for(int i=0; i<particlevector.size(); i++) {
		if(particlevector[i].NDaughters() > 3) {
			cout << particlevector[i].NDaughters() << "\t";
		}
		if(particlevector[i].GetPDG() == 111) {
			cout << "blubb" << particlevector[i].NDaughters() << "\t";
			if(particlevector[i].NDaughters() > 2) {
				particlecounter++;
			}
			
			std::vector<int> daughterid;
			daughterid = particlevector[i].DaughterIds();
			cout << "daughterids: ";
			for(int j=0; j<daughterid.size(); j++) {
				cout << daughterid[j] << "/";
				cout << "pdg-" << particlevector[daughterid[j]].GetPDG() << "-";
			}
			particlecounter_all++;
		}
	}
	cout << endl;
	
	
	
	cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	cout << "KFParticle_Analysis - SignalIds size: " << fSignalIds.size();
	for(int i=0; i<fSignalIds.size(); i++) {
		Int_t pdg = (fKFtopo->GetParticles()[fSignalIds[i]]).GetPDG();
		Int_t daughters = (fKFtopo->GetParticles()[fSignalIds[i]]).NDaughters();
		float mass = 0;
		float mass_sigma = 0;
		(fKFtopo->GetParticles()[fSignalIds[i]]).GetMass(mass, mass_sigma);
		if(pdg == 111) {
			cout << "pi0 found!\t";
			particlecounter++;
			if(daughters == 2) { particlecounter_2daughters++; }
			if(daughters == 3) { particlecounter_3daughters++; }
			if(daughters == 4) { particlecounter_4daughters++; }
			fhPi0_NDaughters->Fill(daughters);
			fhPi0_mass->Fill(mass);
			fNofPi0_kfparticle++;
		}
	}
	
	cout << endl;
	cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	cout << "KFParticle_Analysis - GhostIds size: " << fGhostIds.size();
	for(int i=0; i<fGhostIds.size(); i++) {
		Int_t pdg = (fKFtopo->GetParticles()[fGhostIds[i]]).GetPDG();
		Int_t daughters = (fKFtopo->GetParticles()[fGhostIds[i]]).NDaughters();
		if(pdg == 111) {
			cout << "pi0 found!\t";
			particlecounter++;
			if(daughters == 2) { particlecounter_2daughters++; }
			if(daughters == 3) { particlecounter_3daughters++; }
			if(daughters == 4) { particlecounter_4daughters++; }
			//fhPi0_NDaughters->Fill(daughters);
		}
	}
	
	cout << endl;
	cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	
	
	
/*
	trackindexarray.clear();
	trackindexarray = fKFtopo->GetPVTrackIndexArray();
	cout << "trackindexarray: " << trackindexarray.size() << endl;
	
	KFVertex vertex;
	vertex = fKFtopo->GetPrimKFVertex();
	int testnof = 0;
	testnof = vertex.GetNContributors();
	cout << "nof contributors: " << testnof << endl;
	
	KFParticle particle;
	particle = fKFtopo->GetPrimVertex();
	
	
	for(int i=0; i < kftrackvector->Size(); i++) {
		//const kfvector_int& tempPDG;
		//tempPDG = kftrackvector->PDG;
		//fKFVertex->At(trackindexarray[i]);
	
	}
*/



}




