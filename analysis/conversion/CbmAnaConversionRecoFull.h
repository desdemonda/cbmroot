/**
 * \file CbmAnaConversionRecoFull.h
 *
 * \author Sascha Reinecke <reinecke@uni-wuppertal.de>
 * \date 2015
 **/
 
 
#ifndef CBM_ANA_CONVERSION_RECOFULL
#define CBM_ANA_CONVERSION_RECOFULL

// included from ROOT
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TClonesArray.h>
#include <TStopwatch.h>

// included from CbmRoot
#include "CbmMCTrack.h"
#include "CbmVertex.h"
#include "CbmKFVertex.h"
#include "../../littrack/cbm/elid/CbmLitGlobalElectronId.h"
#include "../dielectron/CbmLmvmKinematicParams.h"




using namespace std;


class CbmAnaConversionRecoFull
{

public:
	CbmAnaConversionRecoFull();
	virtual ~CbmAnaConversionRecoFull();

	void Init();
	void InitHistos();
	void Finish();
	void Exec();

	void CombineElectrons();
	Double_t Invmass_4particlesRECO(const TVector3 part1, const TVector3 part2, const TVector3 part3, const TVector3 part4);

	CbmLmvmKinematicParams CalculateKinematicParamsReco(const TVector3 electron1, const TVector3 electron2);



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

	vector<TH1*> fHistoList_recofull;


	TH1I * fhElectrons;

	CbmLitGlobalElectronId* electronidentifier;

	vector<CbmGlobalTrack*> fElectrons_track;
	vector<TVector3> fElectrons_momenta;

	TH1D * fhElectrons_invmass;
	TH1D * fhElectrons_invmass_cut;


	// timer
	TStopwatch timer;
	Double_t fTime;

	CbmAnaConversionRecoFull(const CbmAnaConversionRecoFull&);
	CbmAnaConversionRecoFull operator=(const CbmAnaConversionRecoFull&);

	ClassDef(CbmAnaConversionRecoFull,1)
};

#endif
