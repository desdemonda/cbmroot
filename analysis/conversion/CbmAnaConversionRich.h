/**
 * \file CbmAnaConversionRich.h
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2014
 **/
 
 
#ifndef CBM_ANA_CONVERSION_RICH
#define CBM_ANA_CONVERSION_RICH

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


class CbmAnaConversionRich
{

public:
	CbmAnaConversionRich();
	virtual ~CbmAnaConversionRich();

	void Init();
	void InitHistos();
	void Finish();

	void AnalyseRICHdata();
	void FillAdditionalPDGhisto(Int_t pdg, Int_t motherpdg);
	void Protons(CbmMCTrack* mcTrack);






private:
	TClonesArray* fRichPoints;
	TClonesArray* fRichRings;
	TClonesArray* fRichRingMatches;
	TClonesArray* fMcTracks;
	TClonesArray* fStsTracks;
	TClonesArray* fStsTrackMatches;
	TClonesArray* fGlobalTracks;
	CbmVertex *fPrimVertex;

	vector<TH1*> fHistoList_richrings;	// list of all histograms related to rich rings



	TH2D * fTest;

	// histograms containing data about rich rings
	TH1D * fRichRings_nofRings;
	TH1D * fRichRings_motherpdg;
	TH1D * fRichRings_richpdg;
	TH1D * fRichRings_electronspE;
	TH1D * fRichRings_sourcePI0;
	TH1D * fRichRings_test;
	
	TH1D * fRichRings_detectedParticles;
	TH1D * fRichRings_detParticlesMother;
	
	TH1D * fRichRings_Aaxis;
	TH1D * fRichRings_Aaxis_part1;
	TH1D * fRichRings_Aaxis_part2;
	TH1D * fRichRings_Aaxis_part3;
	TH1D * fRichRings_Baxis;
	TH1D * fRichRings_Baxis_part1;
	TH1D * fRichRings_Baxis_part2;
	TH1D * fRichRings_Baxis_part3;
	TH1D * fRichRings_radius;
	TH1D * fRichRings_distance;
	TH2D * fhRingtest;
	TH2D * fhRichRings_AaxisVSmom;
	TH2D * fhRichRings_BaxisVSmom;

	TH1D * fhRichRings_test1;
	TH1D * fhRichRings_test2;
	TH1D * fhRichRings_test3;
	TH1D * fhRichRings_test4;
	TH1D * fhRichRings_test5;
	TH1D * fhRichRings_test6;
	TH2D * fhRichRings_pos;


	TH2D * fhRichRings_protons;
	TH1D * fhRichRings_protons2;



	// timer
	TStopwatch timer;
	Double_t fTime;

	CbmAnaConversionRich(const CbmAnaConversionRich&);
	CbmAnaConversionRich operator=(const CbmAnaConversionRich&);

	ClassDef(CbmAnaConversionRich,1)
};

#endif
