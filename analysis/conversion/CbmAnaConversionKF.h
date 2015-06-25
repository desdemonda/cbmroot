/**
 * \file CbmAnaConversionKF.h
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2014
 **/
 
 
#ifndef CBM_ANA_CONVERSION_KF
#define CBM_ANA_CONVERSION_KF

// included from ROOT
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TClonesArray.h>
#include <TStopwatch.h>

// included from CbmRoot
#include "CbmMCTrack.h"
#include "CbmVertex.h"
#include "CbmKFParticleFinder.h"
#include "CbmKFParticleFinderQA.h"
#include "CbmKFParticle.h"
#include "KFParticle.h"




using namespace std;


class CbmAnaConversionKF
{

public:
	CbmAnaConversionKF();
	virtual ~CbmAnaConversionKF();

	void Init();
	void InitHistos();
	void Finish();
	void Exec();

	//void KFParticle_Analysis();

	void SetKF(CbmKFParticleFinder* kfparticle, CbmKFParticleFinderQA* kfparticleQA);
	void SetSignalIds(std::vector<int> *signalids);
	void SetGhostIds(std::vector<int> *ghostids);


	void test();
	Double_t Invmass_4particles(const CbmMCTrack* mctrack1, const CbmMCTrack* mctrack2, const CbmMCTrack* mctrack3, const CbmMCTrack* mctrack4);

	void test2();
	void Reconstruct();
	Double_t Invmass_4particlesRECO(KFParticle part1, KFParticle part2, KFParticle part3, KFParticle part4);




private:
	TClonesArray* fKFMcParticles;
	TClonesArray* fMcTracks;
	TClonesArray* fStsTracks;
	TClonesArray* fStsTrackMatches;

	CbmKFParticleFinder* fKFparticle;
	CbmKFParticleFinderQA* fKFparticleFinderQA;
	
	const KFParticleTopoReconstructor* fKFtopo;
	vector<int> trackindexarray;
	Int_t particlecounter;
	Int_t particlecounter_2daughters;
	Int_t particlecounter_3daughters;
	Int_t particlecounter_4daughters;
	Int_t particlecounter_all;
	TH1D * fhPi0_NDaughters;

	// for data gained from the KFParticle package
	Int_t fNofGeneratedPi0_allEvents;	// number of generated pi0 summed up over ALL EVENTS
	Int_t fNofPi0_kfparticle_allEvents;	// number of all reconstructed pi0 summed up over ALL EVENTS
	Int_t fNofGeneratedPi0;
	Int_t fNofPi0_kfparticle;
	TH1D * fhPi0Ratio;
	TH1D * fhPi0_mass;


	vector<int> fSignalIds;
	vector<int> fGhostIds;


	vector<TH1*> fHistoList_kfparticle;	// list of all histograms containing results from KFParticle package
	

	vector<KFParticle> particlevector;
	vector<int> electronIDs;
	TH1D * fhInvMassPi0WithFullReco;


	// timer
	TStopwatch timer;
	Double_t fTime;

	CbmAnaConversionKF(const CbmAnaConversionKF&);
	CbmAnaConversionKF operator=(const CbmAnaConversionKF&);

	ClassDef(CbmAnaConversionKF,1)
};

#endif
