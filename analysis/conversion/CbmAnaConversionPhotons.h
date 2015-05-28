/**
 * \file CbmAnaConversionPhotons.h
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2014
 **/
 
 
#ifndef CBM_ANA_CONVERSION_PHOTONS
#define CBM_ANA_CONVERSION_PHOTONS

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
#include "CbmKFVertex.h"




using namespace std;


class CbmAnaConversionPhotons
{

public:
	CbmAnaConversionPhotons();
	virtual ~CbmAnaConversionPhotons();

	void Init();
	void InitHistos();
	void Finish();
	void Exec();


	void AnalysePhotons();
	void FillMCTracklist(CbmMCTrack* mctrack);
	void FillMCTracklist_allElectrons(CbmMCTrack* mctrack);
	void FillRecoTracklist_allElectrons(CbmMCTrack* mctrack, TVector3 refittedMom);
	void CombineElectrons();
	int CheckMC(CbmMCTrack* mctrack);

	CbmLmvmKinematicParams CalculateKinematicParams(const CbmMCTrack* mctrackP, const CbmMCTrack* mctrackM);
	CbmLmvmKinematicParams CalculateKinematicParamsReco(const TVector3 electron1, const TVector3 electron2);


	void AnalyseElectronsFromGammaMC();
	void AnalyseElectronsFromGammaReco();
	void AnalyseElectronsFromPi0Reco();



private:
	TClonesArray* fRichPoints;
	TClonesArray* fRichRings;
	TClonesArray* fRichRingMatches;
	TClonesArray* fMcTracks;
	TClonesArray* fStsTracks;
	TClonesArray* fStsTrackMatches;
	TClonesArray* fGlobalTracks;
	CbmVertex *fPrimVertex;
	CbmKFVertex fKFVertex;

	vector<TH1*> fHistoList_photons;	// list of all histograms related to rich rings
	vector<TH1*> fHistoList_EFG;
	vector<TH1*> fHistoList_EFG_angle;
	vector<TH1*> fHistoList_EFG_invmass;
	vector<TH1*> fHistoList_EFG_energy;


	TH1I * fPhotons_nofPerEvent;
	TH1D * fPhotons_pt;
	TH1I * fPhotons_test;
	TH1I * fPhotons_nofMC;
	TH1D * fPhotons_energyMC;
	TH1D * fPhotons_ptMC;
	TH2D * fPhotons_ptRapMC;
	TH1D * fPhotonsRest_energyMC;
	TH1D * fPhotonsRest_pdgMotherMC;
	TH1D * fPhotonsRest_ptMC;

	TH1D * fPhotonsRest_ptMC_pi0;
	TH1D * fPhotonsRest_ptMC_n;
	TH1D * fPhotonsRest_ptMC_e;
	TH1D * fPhotonsRest_ptMC_eta;

	vector<CbmMCTrack*>	fMCTracklist;
	vector<CbmMCTrack*>	fMCTracklist_allElectronsFromGamma;
	vector<CbmMCTrack*>	fRecoTracklist_allElectronsFromGamma;
	vector<TVector3>	fRecoTracklist_allElectronsFromGammaMom;
	vector<CbmMCTrack*>	fRecoTracklist_allElectronsFromPi0;
	vector<TVector3>	fRecoTracklist_allElectronsFromPi0Mom;

	Int_t global_nof_photonsMC;
	Int_t global_nof_photonsReco;
	TH1D * fhGlobalNofDirectPhotons;

	// distributions of opening angles from MC-true data
	TH1D * fhEFG_angle_all;
	TH1D * fhEFG_angle_combBack;
	TH1D * fhEFG_angle_allSameG;
	TH1D * fhEFG_angle_direct;
	TH1D * fhEFG_angle_pi0;
	TH1D * fhEFG_angle_eta;

	// opening angle vs energy from MC-true data
	//TH2D * fhEFG_angleVSenergy_all;
	//TH2D * fhEFG_angleVSenergy_combBack;
	TH2D * fhEFG_angleVSenergy_allSameG;
	TH2D * fhEFG_angleVSenergy_direct;
	TH2D * fhEFG_angleVSenergy_pi0;
	TH2D * fhEFG_angleVSenergy_eta;

	// opening angle vs pt from MC-true data
	//TH2D * fhEFG_angleVSpt_all;
	//TH2D * fhEFG_angleVSpt_combBack;
	TH2D * fhEFG_angleVSpt_allSameG;
	TH2D * fhEFG_angleVSpt_direct;
	TH2D * fhEFG_angleVSpt_pi0;
	TH2D * fhEFG_angleVSpt_eta;

	// distribution of invariant masses from MC-true data
	TH1D * fhEFG_invmass_all;
	TH1D * fhEFG_invmass_combBack;
	TH1D * fhEFG_invmass_allSameG;
	TH1D * fhEFG_invmass_direct;
	TH1D * fhEFG_invmass_pi0;
	TH1D * fhEFG_invmass_eta;

	// distribution of energies from MC-true data
	TH1D * fhEFG_energy_all;
	TH1D * fhEFG_energy_combBack;
	TH1D * fhEFG_energy_allSameG;
	TH1D * fhEFG_energy_direct;
	TH1D * fhEFG_energy_pi0;
	TH1D * fhEFG_energy_eta;

	// distribution of opening angle for energies below 1 GeV from MC-true data
	TH1D * fhEFG_angleBelow1GeV_all;
	TH1D * fhEFG_angleBelow1GeV_combBack;
	TH1D * fhEFG_angleBelow1GeV_allSameG;
	TH1D * fhEFG_angleBelow1GeV_direct;
	TH1D * fhEFG_angleBelow1GeV_pi0;
	TH1D * fhEFG_angleBelow1GeV_eta;

	// distribution of opening angle for energies above 1 GeV from MC-true data
	TH1D * fhEFG_angleAbove1GeV_all;
	TH1D * fhEFG_angleAbove1GeV_combBack;
	TH1D * fhEFG_angleAbove1GeV_allSameG;
	TH1D * fhEFG_angleAbove1GeV_direct;
	TH1D * fhEFG_angleAbove1GeV_pi0;
	TH1D * fhEFG_angleAbove1GeV_eta;

	// distribution of pair-wise momenta from MC-true data
	TH1D * fhEFG_momentumPair_all;
	TH1D * fhEFG_momentumPair_combBack;
	TH1D * fhEFG_momentumPair_allSameG;
	TH1D * fhEFG_momentumPair_direct;
	TH1D * fhEFG_momentumPair_pi0;
	TH1D * fhEFG_momentumPair_eta;

	// startvertex (= point of conversion) on z-axis versus opening angle from MC-true data
	//TH2D * fhEFG_startvertexVSangle_all;
	//TH2D * fhEFG_startvertexVSangle_combBack;
	TH2D * fhEFG_startvertexVSangle_allSameG;
	TH2D * fhEFG_startvertexVSangle_direct;
	TH2D * fhEFG_startvertexVSangle_pi0;
	TH2D * fhEFG_startvertexVSangle_eta;

	// distribution of opening angles from reconstructed momenta
	TH1D * fhEFG_angle_all_reco;
	TH1D * fhEFG_angle_combBack_reco;
	TH1D * fhEFG_angle_allSameG_reco;
	TH1D * fhEFG_angle_direct_reco;
	TH1D * fhEFG_angle_pi0_reco;
	TH1D * fhEFG_angle_eta_reco;

	// distribution of opening angles for photon-energies below 1GeV from reconstructed momenta
	TH1D * fhEFG_angleBelow1GeV_all_reco;
	TH1D * fhEFG_angleBelow1GeV_combBack_reco;
	TH1D * fhEFG_angleBelow1GeV_allSameG_reco;
	TH1D * fhEFG_angleBelow1GeV_direct_reco;
	TH1D * fhEFG_angleBelow1GeV_pi0_reco;
	TH1D * fhEFG_angleBelow1GeV_eta_reco;

	// distribution of invariant masses from reconstructed momenta
	TH1D * fhEFG_invmass_all_reco;
	TH1D * fhEFG_invmass_combBack_reco;
	TH1D * fhEFG_invmass_allSameG_reco;
	TH1D * fhEFG_invmass_direct_reco;
	TH1D * fhEFG_invmass_pi0_reco;
	TH1D * fhEFG_invmass_eta_reco;

	// opening angle vs pt from reconstructed data
	//TH2D * fhEFG_angleVSpt_all_reco;
	//TH2D * fhEFG_angleVSpt_combBack_reco;
	TH2D * fhEFG_angleVSpt_allSameG_reco;
	TH2D * fhEFG_angleVSpt_direct_reco;
	TH2D * fhEFG_angleVSpt_pi0_reco;
	TH2D * fhEFG_angleVSpt_eta_reco;

	// momentum resolution of photons from reconstructed data
	TH2D * fhEFG_momentumResolutionPhoton_reco;
	TH2D * fhEFG_momentumResolutionElectrons_reco;

	// opening angles for e+e- pairs from pi0 dalitz decays (i.e. e+e- pairs directly stemming from pi0)
	TH1D * fhEFPI0_angle_reco;


	// timer
	TStopwatch timer;
	Double_t fTime;

	CbmAnaConversionPhotons(const CbmAnaConversionPhotons&);
	CbmAnaConversionPhotons operator=(const CbmAnaConversionPhotons&);

	ClassDef(CbmAnaConversionPhotons,1)
};

#endif
