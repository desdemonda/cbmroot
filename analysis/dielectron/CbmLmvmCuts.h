/**
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2015
 * @version 1.0
 **/


#ifndef CBM_LMVM_CUTS_H
#define CBM_LMVM_CUTS_H


class CbmLmvmCuts {
public:
	CbmLmvmCuts():
		fTrdAnnCut(0.),
		fRichAnnCut(0.),
		fMeanA(0.),
		fMeanB(0.),
		fRmsA(0.),
		fRmsB(0.),
		fRmsCoeff(0.),
		fDistCut(0.),
		fUseRichAnn(kTRUE),
		fMomentumCut(0.),
		fChiPrimCut(0.),
		fPtCut(0.),
		fAngleCut(0.),
		fGammaCut(0.),
		fStCutAngle(0.),
		fStCutPP(0.),
		fTtCutAngle(0.),
		fTtCutPP(0.),
		fRtCutAngle(0.),
		fRtCutPP(0.),
		fMvd1CutP(0.),
		fMvd1CutD(0.),
		fMvd2CutP(0.),
		fMvd2CutD(0.)
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
		fMomentumCut = -1.; // if cut < 0 it is not used

		// analysis cuts
		fPtCut = 0.2;
		fAngleCut = 1.;
		fChiPrimCut = 3.;
		fGammaCut = 0.025;
		fStCutAngle = 1.5;
		fStCutPP = 1.5;
		fTtCutAngle = 0.75;
		fTtCutPP = 4.0;
		fRtCutAngle = 1.25;
		fRtCutPP = 2.0;
		fMvd1CutP = 1.2;
		fMvd1CutD = 0.4;
		fMvd2CutP = 1.5;
		fMvd2CutD = 0.5;
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
				"fPtCut = " << fPtCut << endl <<
				"fAngleCut = " << fAngleCut << endl <<
				"fGammaCut = " << fGammaCut << endl <<
				"fStCut (ang,pp) = (" << fStCutAngle << "," << fStCutPP << ")" << endl <<
				"fRtCut (ang,pp) = (" << fRtCutAngle << "," << fRtCutPP << ")" << endl <<
				"fTtCut (ang,pp) = (" << fTtCutAngle << "," << fTtCutPP << ")" << endl <<
				"fMvd1Cut (p,d) = (" << fMvd1CutP << "," << fMvd1CutD << ")" << endl <<
				"fMvd2Cut (p,d) = (" << fMvd2CutP << "," << fMvd2CutD << ")" << endl <<
			   "fMomentumCut = "<< fMomentumCut <<  endl;
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
	Double_t fMomentumCut; // if cut < 0 then it will not be used

	// Analysis cuts
	Double_t fChiPrimCut;
	Double_t fPtCut;
	Double_t fAngleCut;
	Double_t fGammaCut;
	Double_t fStCutAngle;
	Double_t fStCutPP;
	Double_t fTtCutAngle;
	Double_t fTtCutPP;
	Double_t fRtCutAngle;
	Double_t fRtCutPP;
	Double_t fMvd1CutP;
	Double_t fMvd1CutD;
	Double_t fMvd2CutP;
	Double_t fMvd2CutD;
};

#endif
