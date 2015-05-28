#ifndef MULTISCATTERINGMODEL_H
#define MULTISCATTERINGMODEL_H

//#include <cmath>
#include "TMath.h"

//#include <omp.h>


class MultiscatteringModel
{
	std::vector<double> xlag32, wlag32;
public:
    MultiscatteringModel();

    virtual ~MultiscatteringModel(void) { }
	
	double GetSigt(double apt) {
		return TMath::Sqrt(2./TMath::Pi()) * apt;
	}
	
	double GetSigz(double apz2, double apz4) {
		double ret = 3./2.*apz2*apz2 - apz4/2.;
		if (ret<0.) return TMath::Sqrt( apz2 );
		else return TMath::Sqrt( apz2 - TMath::Sqrt(ret) );
	}
	
	double GetQz(double apz2, double apz4) {
		double ret = 3./2.*apz2*apz2 - apz4/2.;
		if (ret<0.) return 0.;
		return TMath::Power(ret, 1./4.);
	}
	
	double fpt(double pt, double sigt) {
		return pt / sigt / sigt * TMath::Exp(-pt*pt/2./sigt/sigt);
	}
	
	double fpz(double pz, double sigz, double qz) {
		return 1. / 2. / TMath::Sqrt(2.*TMath::Pi()) / sigz * ( TMath::Exp(-(pz-qz)*(pz-qz)/2./sigz/sigz) + TMath::Exp(-(pz+qz)*(pz+qz)/2./sigz/sigz));
	}
	
	double dndy(double y, double m, double sigt, double sigz, double qz) {
		double ret = 0.;
		for(int i = 0 ; i < 32 ; i++){
			double mt = TMath::Sqrt(m*m + xlag32[i]*xlag32[i]);
			ret += wlag32[i] * mt * fpt(xlag32[i], sigt) * fpz(mt * TMath::SinH(y), sigz, qz);
		}
		return ret * TMath::CosH(y);
	}
	
	double fypt(double y, double pt, double m, double sigt, double sigz, double qz) {
		double mt = TMath::Sqrt(m*m + pt*pt);
		return mt * TMath::CosH(y) * fpt(pt, sigt) * fpz(mt * TMath::SinH(y), sigz, qz);
	}
};

#endif
