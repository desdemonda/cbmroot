
#ifndef CBM_ANA_JPSI_CUTS_H
#define CBM_ANA_JPSI_CUTS_H

#include "TObject.h"
#include <iostream>

using namespace std;

class CbmAnaJpsiCuts {
public:
	CbmAnaJpsiCuts():
		fTrdAnnCut(0.),
		fRichAnnCut(0.),
		fMeanA(0.),
		fMeanB(0.),
		fRmsA(0.),
		fRmsB(0.),
		fRmsCoeff(0.),
		fDistCut(0.),
		fUseRichAnn(kTRUE),
		fChiPrimCut(0.),
		fPtCut(0.)
	{
		SetDefaultCuts();
	}

	/*
	 * Set default electron ID and analysis cuts.
	 */
	void SetDefaultCuts()
	{
		//electron ID cuts
		fTrdAnnCut = 0.85;
		fRichAnnCut = 0.0;
		fUseRichAnn = true;
		fMeanA = -1.;
		fMeanB = -1.;
		fRmsA = -1.;
		fRmsB = -1.;
		fRmsCoeff = -1.;
		fDistCut = -1.;

		// analysis cuts
		fPtCut = 1.5;
		fChiPrimCut = 3.;
	}

	/*
	 * Print out cuts.
	 */
	void Print()
	{
		cout << "Used cuts:" << endl <<
				"fTrdAnnCut = " << fTrdAnnCut << endl <<
				"fRichAnnCut = " << fRichAnnCut << endl <<
				"fChiPrimCut = " << fChiPrimCut << endl <<
				"fPtCut = " << fPtCut << endl;
	}

public:
	 // ID cuts
	Double_t fTrdAnnCut;
	Double_t fRichAnnCut;
	Double_t fMeanA;
	Double_t fMeanB;
	Double_t fRmsA;
	Double_t fRmsB;
	Double_t fRmsCoeff;
	Double_t fDistCut;
	Bool_t fUseRichAnn;

	// Analysis cuts
	Double_t fChiPrimCut;
	Double_t fPtCut;
};

#endif
