/**
 * \file CbmAnaConversionTomography.h
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2014
 * Using the true-MC data to do some tomography studies,
 * via conversion of gammas (gamma -> e+ e-)
 * somewhere in the detector material and plotting
 * the point of conversion.
 **/
 
 
#ifndef CBM_ANA_CONVERSION_TOMOGRAPHY
#define CBM_ANA_CONVERSION_TOMOGRAPHY

// included from ROOT
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TStopwatch.h>
#include <TClonesArray.h>

// included from CbmRoot
#include "CbmMCTrack.h"




using namespace std;


class CbmAnaConversionTomography
{

public:
	CbmAnaConversionTomography();
	virtual ~CbmAnaConversionTomography();

	void Init();
	void InitHistos();
	void Finish();

	void Exec();

	void TomographyMC(int electronID);
	void TomographyReco(CbmMCTrack* mctrack);
	Bool_t GetNPoints(CbmMCTrack* mctrack);







private:
	TClonesArray* fMcTracks;

	vector<TH1*> fHistoList_tomography;	// list of all histograms


	TH1D * fhGammaZ;
	TH3D * fhTomography;
	TH2D * fhTomography_XZ;
	TH2D * fhTomography_YZ;
	//TH2D * fhTomography_XZ_cut;
	//TH2D * fhTomography_YZ_cut;
	TH2D * fhTomography_uptoRICH;
	TH2D * fhTomography_RICH_complete;
	TH2D * fhTomography_RICH_beampipe;
	TH2D * fhTomography_STS_end;
	TH2D * fhTomography_STS_lastStation;
	TH2D * fhTomography_RICH_frontplate;
	TH2D * fhTomography_RICH_backplate;

	TH1D * fhConversion;
	TH1D * fhConversion_inSTS;
	TH1D * fhConversion_prob;
	TH1D * fhConversion_energy;
	TH1D * fhConversion_p;


	TH3D * fhTomography_reco;
	TH2D * fhTomography_reco_XZ;
	TH2D * fhTomography_reco_YZ;
	TH1D * fhConversion_reco;

	vector<int> electronIDs;
	vector<int> electronMotherIDs;
	Int_t conversionsInDetector[5];			// 0 = magnet, 1 = sts, 2 = rich, 3 = trd, 4 = tof
	//Int_t conversionsInDetector_cut[5];		// 0 = magnet, 1 = sts, 2 = rich, 3 = trd, 4 = tof
	TH1I * fhConversionsPerDetector;
	TH2I * fhConversionsPerDetectorPE;
	//TH2I * fhConversionsPerDetectorPE_cut;

	// timer
	TStopwatch timer;
	Double_t fTime;

	CbmAnaConversionTomography(const CbmAnaConversionTomography&);
	CbmAnaConversionTomography operator=(const CbmAnaConversionTomography&);
     
	ClassDef(CbmAnaConversionTomography,1)
};

#endif
