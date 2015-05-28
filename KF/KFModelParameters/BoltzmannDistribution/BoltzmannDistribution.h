#ifndef BOLTZMANNDISTRIBUTION_H
#define BOLTZMANNDISTRIBUTION_H

//#include <cmath>
#include "TMath.h"
#include "../common/Acceptance.h"
#include "TSpline.h"
#include <iostream>
//#include <omp.h>


class BoltzmannDistribution
{
	std::vector<double> xlag32, wlag32;
	std::vector<double> xleg32, wleg32;
	double fMass;
	int fPDGID;
	bool fUseAcceptance;
	double fYmin, fYmax, fYcm, fWidth;
	double fNorm;
	Acceptance::AcceptanceFunction fAcceptance;//fAcceptanceSTS, fAcceptanceSTSTOF;
	Acceptance::ReconstructionEfficiencyFunction fReconstructionEfficiency;
	TSpline3 fTamt;
	TSpline3 fNormT, fNormT4pi;
public:
    BoltzmannDistribution(double mass = 0.938, int PDGID = 2212, bool fUseAcc = false, double ymin = -3., double ymax = 3., double ycm = 2., double width = 1.);

    virtual ~BoltzmannDistribution(void) { }
	
	double GetT(double amt) {
		return fTamt.Eval(amt);
	}
	
	double GetA(double multiplicity, double T) {
		if (!fUseAcceptance) return multiplicity;
		else return multiplicity * fNormT4pi.Eval(T) / fNormT.Eval(T);
	}
	
	double GetAerror(double multiplicity, double T, double multerr, double Terr) {
		if (!fUseAcceptance) return multerr;
		else return TMath::Sqrt(fNormT4pi.Eval(T) / fNormT.Eval(T) * fNormT4pi.Eval(T) / fNormT.Eval(T) * multerr * multerr
						+ multiplicity * multiplicity / fNormT.Eval(T) / fNormT.Eval(T) * fNormT4pi.Derivative(T) * fNormT4pi.Derivative(T) * Terr * Terr
						+ multiplicity * multiplicity / fNormT.Eval(T) / fNormT.Eval(T) / fNormT.Eval(T) / fNormT.Eval(T) * fNormT4pi.Eval(T) * fNormT4pi.Eval(T) * fNormT.Derivative(T) * fNormT.Derivative(T) * Terr * Terr);
	}
	
	double fmt(double amt, double T);
	
	double dTdmt(double amt) {
		return fTamt.Derivative(amt);
	}
	
	double mtAv(double T);
	double mtAv2(double T);
	double Normalization(double T);
	double Normalization4pi(double T);
	
	double dndy(double y, double A, double T);

};

#endif
