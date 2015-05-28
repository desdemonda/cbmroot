#ifndef IMPACTPARAMETERMODEL_H
#define IMPACTPARAMETERMODEL_H

//#include <cmath>
#include "TMath.h"
#include <string>

//#include <omp.h>


class ImpactParameterModel
{
	std::vector<double> bfromN;
	std::vector<double> bfromNerror;
public:
    ImpactParameterModel(std::string filename="");

    virtual ~ImpactParameterModel(void) { }
	
	double getB(int Nchg) {
		if (Nchg<=0. || Nchg==0) return 15.;
		else if (Nchg>=static_cast<int>(bfromN.size())) return 0.;
		else return bfromN[Nchg];
	}
	
	double getBerror(int Nchg) {
		if (Nchg<=0. || Nchg==0) return 2.;
		else if (Nchg>=static_cast<int>(bfromN.size())) return 1.;
		else return bfromNerror[Nchg];
	}
	
	double getB(double Nchg) {
		if (Nchg<=0. || Nchg==0.) return 15.;
		else if (Nchg>=bfromN.size()) return 0.;
		else {
			int Nchg1 = static_cast<int>(Nchg);
			int Nchg2 = Nchg1 + 1;
			double w = (Nchg - Nchg1);
			return (1.-w) * bfromN[Nchg1] + w * bfromN[Nchg2];
		}
	}
	
	double getBerror(double Nchg) {
		if (Nchg<=0. || Nchg==0.) return 2.;
		else if (Nchg>=bfromN.size()) return 1.;
		else {
			int Nchg1 = static_cast<int>(Nchg);
			int Nchg2 = Nchg1 + 1;
			double w = (Nchg - Nchg1);
			return (1.-w) * bfromNerror[Nchg1] + w * bfromNerror[Nchg2];
		}
	}
};

#endif
