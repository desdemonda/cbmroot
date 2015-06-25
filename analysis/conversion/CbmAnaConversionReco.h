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
#include "../dielectron/CbmLmvmKinematicParams.h"




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
	void SetTracklistReco(vector<CbmMCTrack*> MCTracklist, vector<TVector3> RecoTracklist1, vector<TVector3> RecoTracklist2, vector<int> ids);
	void InvariantMassMC_all();
	Double_t Invmass_4particles(const CbmMCTrack* mctrack1, const CbmMCTrack* mctrack2, const CbmMCTrack* mctrack3, const CbmMCTrack* mctrack4);
	Double_t SmearValue(Double_t value);
	Double_t Invmass_4particlesRECO(const TVector3 part1, const TVector3 part2, const TVector3 part3, const TVector3 part4);
	void InvariantMassTest_4epem();
	Int_t NofDaughters(Int_t motherId);
	Double_t CalculateOpeningAngleReco(TVector3 electron1, TVector3 electron2);
	Double_t CalculateOpeningAngleMC(CbmMCTrack* mctrack1, CbmMCTrack* mctrack2);

	void CalculateInvMassWithFullRecoCuts();
	CbmLmvmKinematicParams CalculateKinematicParamsReco(const TVector3 electron1, const TVector3 electron2);







private:
	TClonesArray* fMcTracks;


	vector<CbmMCTrack*>	fMCTracklist_all;
	vector<CbmMCTrack*>	fRecoTracklistEPEM;
	vector<int>			fRecoTracklistEPEM_ids;
	vector<TVector3>	fRecoMomentum;
	vector<TVector3>	fRecoRefittedMomentum;
	

	vector<TH1*> fHistoList_MC;			// list of all histograms generated with MC data
	vector<TH1*> fHistoList_reco;		// list of all histograms generated with reconstructed data
	vector<TH1*> fHistoList_reco_mom;	// list of all histograms of reconstruction data (used momenta)
	vector<TH1*> fHistoList_gg;
	vector<TH1*> fHistoList_gee;
	vector<TH1*> fHistoList_eeee;
	vector<TH1*> fHistoList_all;


	TH1D * fhInvariantMass_MC_all;
	TH1D * fhInvariantMass_MC_pi0;
	TH1D * fhInvariantMass_MC_pi0_epem;
	TH1D * fhInvariantMass_MC_pi0_gepem;
	TH1D * fhInvariantMass_MC_pi0_gg;
	TH1D * fhInvariantMass_MC_eta;
   
	TH1D * fhInvariantMassReco_pi0;

	TH1D * fhMCtest;


	// histograms for procedure "InvariantMassTest_4epem()"
	TH1D * fhEPEM_invmass_gg_mc;
	TH1D * fhEPEM_invmass_gg_refitted;
	TH1D * fhEPEM_invmass_gee_mc;
	TH1D * fhEPEM_invmass_gee_refitted;
	TH1D * fhEPEM_invmass_eeee_mc;
	TH1D * fhEPEM_invmass_eeee_refitted;
	TH1D * fhEPEM_invmass_all_mc;
	TH1D * fhEPEM_invmass_all_refitted;
	
	TH1D * fhEPEM_openingAngle_gg_mc;
	TH1D * fhEPEM_openingAngle_gg_refitted;
	TH1D * fhEPEM_openingAngle_gee_mc;
	TH1D * fhEPEM_openingAngle_gee_refitted;


	TH1D * fhInvMass_EPEM_mc;
	TH1D * fhInvMass_EPEM_stsMomVec;
	TH1D * fhInvMass_EPEM_refitted;
	TH1D * fhInvMass_EPEM_error_stsMomVec;
	TH1D * fhInvMass_EPEM_error_refitted;
	TH1D * fhInvMass_EPEM_openingAngleRef;	// refitted 
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
	TH1D * fhPi0_startvertexElectrons;

	TH1D * fhInvMassWithFullRecoCuts;


	// timer
	TStopwatch timer;
	Double_t fTime;

	CbmAnaConversionReco(const CbmAnaConversionReco&);
	CbmAnaConversionReco operator=(const CbmAnaConversionReco&);

	ClassDef(CbmAnaConversionReco,1)
};

#endif
