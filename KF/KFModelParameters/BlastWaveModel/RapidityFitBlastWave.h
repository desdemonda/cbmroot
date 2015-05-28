#ifndef RAPIDITYFIT_H
#define RAPIDITYFIT_H

#include <string>
#include <cmath>
#include <vector>

struct RapidityFitBlastWaveParameter {
	bool toFit;
	double value;
	double error;
	double xmin, xmax;
	std::string name;
	RapidityFitBlastWaveParameter(std::string name_="", bool toFit_=true, double val=1., double err=0., double vmin=-2., double vmax=2.):
		toFit(toFit_), value(val), error(err), xmin(vmin), xmax(vmax), name("") {
		}
};

struct RapidityFitBlastWaveParameters {
    RapidityFitBlastWaveParameter A, eta;
    double chi2ndf;
    RapidityFitBlastWaveParameters(double A_ = 200., double eta_ = 0.3):
		A(RapidityFitBlastWaveParameter("A", true, A_, 1000., 0., 200000.)),
		eta(RapidityFitBlastWaveParameter("eta", true, eta_, 0.2, 0., 10.)),
		chi2ndf(1.)
	{
		// A = RapidityFitBlastWaveParameter("A", true, A_, 1000., 0., 200000.);
		// eta = RapidityFitBlastWaveParameter("eta", true, eta_, 0.2, 0., 10.);
	}
	RapidityFitBlastWaveParameter GetParamater(const std::string& name) const {
		if (A.name==name) return A;
		if (eta.name==name) return eta;
		return RapidityFitBlastWaveParameter();
	}
	void SetParameter(const std::string& name, const RapidityFitBlastWaveParameter& param) {
		if (A.name==name) A = param;
		if (eta.name==name) eta = param;
	}
	void SetParameter(const std::string& name, double val, double err, double xmin, double xmax) {
		if (A.name==name) A = RapidityFitBlastWaveParameter(name, true, val, err, xmin, xmax);
		if (eta.name==name) eta = RapidityFitBlastWaveParameter(name, true, val, err, xmin, xmax);
	}
	void SetParameterFitFlag(const std::string& name, bool toFit) {
		if (A.name==name) A.toFit = toFit;
		if (eta.name==name) eta.toFit = toFit;
	}
};

namespace RapidityFitBlastWaveNamespace {
	double dndy(double y, double A, double eta, double T, double mass);
};

class RapidityFitBlastWave
{
	std::vector<double> y, dndy, err;
	RapidityFitBlastWaveParameters Parameters;
	double fT, fMass;
public:
    RapidityFitBlastWave(const std::vector<double> &y_, const std::vector<double> &dndy_, const std::vector<double> &err_, double T, double mass, double A_ = 200., double eta_ = 0.3):
            y(y_), dndy(dndy_), err(err_), Parameters(A_, eta_), fT(T), fMass(mass)
    {
    }
	~RapidityFitBlastWave(void);
	void SetFitFlag(const std::string& name, bool flag) {
		Parameters.SetParameterFitFlag(name, flag);
	}
	void SetData(const std::vector<double> &y_, const std::vector<double> &dndy_, const std::vector<double> &err_) {
		y = y_;
		dndy = dndy_;
		err = err_;
	}

	RapidityFitBlastWaveParameters PerformFit();
};

#endif
