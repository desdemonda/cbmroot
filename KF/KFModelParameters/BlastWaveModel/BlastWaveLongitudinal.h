#ifndef BLASTWAVELONGITUDINAL_H
#define BLASTWAVELONGITUDINAL_H

//#include <cmath>
#include "TMath.h"
#include "../common/Acceptance.h"
#include "TSpline.h"
#include <iostream>
//#include <omp.h>


class BlastWaveLongitudinal
{
	std::vector<double> xlag32, wlag32;
	std::vector<double> xleg32, wleg32;
	std::vector<double> xlegeta, wlegeta;
	double fMass;
	int fPDGID;
	bool fUseAcceptance;
	double fYmin, fYmax, fYcm, fT;
	double fNorm;
	double fY2Min, fY2Max;
	double fEtaMin, fEtaMax;
	Acceptance::AcceptanceFunction fAcceptance;//fAcceptanceSTS, fAcceptanceSTSTOF;
	Acceptance::ReconstructionEfficiencyFunction fReconstructionEfficiency;
	TSpline3 fetaay2;
	TSpline3 fNormeta, fNormeta4pi;
public:
    BlastWaveLongitudinal(double mass = 0.938, int PDGID = 2212, bool fUseAcc = false, double ymin = -3., double ymax = 3., double ycm = 2., double T = 0.140);

    virtual ~BlastWaveLongitudinal(void) { }
	
	
	double Geteta(double ay2) {
		if (ay2<fY2Min) return fEtaMin;
		else if (ay2>fY2Max) return fEtaMax;
		return fetaay2.Eval(ay2);
	}
	
	double GetA(double multiplicity, double eta) {
		if (!fUseAcceptance) return multiplicity;
		else return multiplicity * fNormeta4pi.Eval(eta) / fNormeta.Eval(eta);
	}
	
	double GetAerror(double multiplicity, double eta, double multerr, double etaerr) {
		if (!fUseAcceptance) return multerr;
		else return TMath::Sqrt(fNormeta4pi.Eval(eta) / fNormeta.Eval(eta) * fNormeta4pi.Eval(eta) / fNormeta.Eval(eta) * multerr * multerr
						+ multiplicity * multiplicity * (fNormeta4pi.Derivative(eta)/fNormeta.Eval(eta) - fNormeta4pi.Eval(eta)*fNormeta.Derivative(eta)/fNormeta.Eval(eta)/fNormeta.Eval(eta)) * (fNormeta4pi.Derivative(eta)/fNormeta.Eval(eta) - fNormeta4pi.Eval(eta)*fNormeta.Derivative(eta)/fNormeta.Eval(eta)/fNormeta.Eval(eta)) * etaerr * etaerr);
	}
	
	double fy(double y, double eta);
	
	double detady2(double ay2) {
		if (ay2<fY2Min) return 0.;
		else if (ay2>fY2Max) return 0.;
		return fetaay2.Derivative(ay2);
	}

	double y2Av(double eta);
	double y2Av2(double eta);
	double Normalization(double eta);
	double Normalization4pi(double eta);

};

#endif
