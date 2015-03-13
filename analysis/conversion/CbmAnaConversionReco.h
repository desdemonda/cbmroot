/**
 * \file CbmAnaConversionReco.h
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2014
 **/
 
 
#ifndef CBM_ANA_CONVERSION_RECO
#define CBM_ANA_CONVERSION_RECO

// included from ROOT
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TClonesArray.h>
#include <TStopwatch.h>

// included from CbmRoot
#include "CbmMCTrack.h"
#include "CbmVertex.h"




using namespace std;


class CbmAnaConversionReco
{

public:
	CbmAnaConversionReco();
	virtual ~CbmAnaConversionReco();

	void Init();
	void InitHistos();
	void Finish();

	void SetTracklistMC(vector<CbmMCTrack*> MCTracklist);
	void SetTracklistReco(vector<CbmMCTrack*> MCTracklist, vector<TVector3> RecoTracklist1, vector<TVector3> RecoTracklist2);
	void InvariantMassMC_all();
	Double_t Invmass_4particles(const CbmMCTrack* mctrack1, const CbmMCTrack* mctrack2, const CbmMCTrack* mctrack3, const CbmMCTrack* mctrack4);
	Double_t SmearValue(Double_t value);
	Double_t Invmass_4particlesRECO(const TVector3 part1, const TVector3 part2, const TVector3 part3, const TVector3 part4);
	void InvariantMassTest_4epem();
	Int_t NofDaughters(Int_t motherId);







private:
	TClonesArray* fMcTracks;


	vector<CbmMCTrack*> fMCTracklist_all;
	vector<CbmMCTrack*> fRecoTracklistEPEM;
	vector<TVector3> fRecoMomentum;
	vector<TVector3> fRecoRefittedMomentum;
	

	vector<TH1*> fHistoList_MC;			// list of all histograms generated with MC data
	vector<TH1*> fHistoList_reco;		// list of all histograms generated with reconstructed data
	vector<TH1*> fHistoList_reco_mom;	// list of all histograms of reconstruction data (used momenta)


	TH1D * fhInvariantMass_MC_all;
	TH1D * fhInvariantMass_MC_pi0;
	TH1D * fhInvariantMass_MC_pi0_epem;
	TH1D * fhInvariantMass_MC_pi0_gepem;
	TH1D * fhInvariantMass_MC_pi0_gg;
	TH1D * fhInvariantMass_MC_eta;
   
	TH1D * fhInvariantMassReco_pi0;

	TH1D * fhMCtest;


	TH1D * fhInvMass_EPEM_mc;
	TH1D * fhInvMass_EPEM_stsMomVec;
	TH1D * fhInvMass_EPEM_refitted;
	TH1D * fhInvMass_EPEM_error_stsMomVec;
	TH1D * fhInvMass_EPEM_error_refitted;
	TH1D * fhUsedMomenta_stsMomVec;
	TH1D * fhUsedMomenta_mc;
	TH1D * fhUsedMomenta_error_stsMomVec;
	TH1D * fhUsedMomenta_error_refitted;
	TH1D * fhUsedMomenta_errorX_stsMomVec;
	TH2D * fhUsedMomenta_vsX_stsMomVec;
	TH1D * fhUsedMomenta_errorY_stsMomVec;
	TH2D * fhUsedMomenta_vsY_stsMomVec;
	TH1D * fhUsedMomenta_errorZ_stsMomVec;
	TH2D * fhUsedMomenta_vsZ_stsMomVec;
	TH1D * fhUsedMomenta_errorX_refitted;
	TH2D * fhUsedMomenta_vsX_refitted;
	TH1D * fhUsedMomenta_errorY_refitted;
	TH2D * fhUsedMomenta_vsY_refitted;
	TH1D * fhUsedMomenta_errorZ_refitted;
	TH2D * fhUsedMomenta_vsZ_refitted;
   
	TH1D * fhInvariantMass_pi0epem;
	TH1D * fhPi0_startvertex;


	// timer
	TStopwatch timer[8];
	Double_t fTime;

	CbmAnaConversionReco(const CbmAnaConversionReco&);
	CbmAnaConversionReco operator=(const CbmAnaConversionReco&);

	ClassDef(CbmAnaConversionReco,1)
};

#endif
