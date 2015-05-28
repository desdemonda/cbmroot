#ifndef RAPIDITYFIT_H
#define RAPIDITYFIT_H

#include <string>
#include <cmath>
#include <vector>

struct RapidityFitParameter {
	bool toFit;
	double value;
	double error;
	double xmin, xmax;
	std::string name;
	RapidityFitParameter(std::string name_="", bool toFit_=true, double val=1., double err=0., double vmin=-2., double vmax=2.):
		toFit(toFit_), value(val), error(err), xmin(vmin), xmax(vmax), name("") {
		}
};

struct RapidityFitParameters {
    RapidityFitParameter A, sig, yav;
    double chi2ndf;
    RapidityFitParameters(double A_ = 200., double sig_ = 1., double yav_ = 0.):
		A(RapidityFitParameter("A", true, A_, 100., 0., 2000.)),
		sig(RapidityFitParameter("sig", true, sig_, 0.5, 0., 10.)),
		yav(RapidityFitParameter("yav", true, yav_, 0.3, 0., 10.)),
		chi2ndf(1.)
	{
		// A = RapidityFitParameter("A", true, A_, 100., 0., 2000.);
		// sig = RapidityFitParameter("sig", true, sig_, 0.5, 0., 10.);
		// yav = RapidityFitParameter("yav", true, yav_, 0.3, 0., 10.);
	}
	RapidityFitParameter GetParamater(const std::string& name) const {
		if (A.name==name) return A;
		if (sig.name==name) return sig;
		if (yav.name==name) return yav;
		return RapidityFitParameter();
	}
	void SetParameter(const std::string& name, const RapidityFitParameter& param) {
		if (A.name==name) A = param;
		if (sig.name==name) sig = param;
		if (yav.name==name) yav = param;
	}
	void SetParameter(const std::string& name, double val, double err, double xmin, double xmax) {
		if (A.name==name) A = RapidityFitParameter(name, true, val, err, xmin, xmax);
		if (sig.name==name) sig = RapidityFitParameter(name, true, val, err, xmin, xmax);
		if (yav.name==name) yav = RapidityFitParameter(name, true, val, err, xmin, xmax);
	}
	void SetParameterFitFlag(const std::string& name, bool toFit) {
		if (A.name==name) A.toFit = toFit;
		if (sig.name==name) sig.toFit = toFit;
		if (yav.name==name) yav.toFit = toFit;
	}
	RapidityFitParameters GetRapidityFitParameters() {
        return RapidityFitParameters(A.value, sig.value, yav.value);
	}
};

namespace RapidityFitNamespace {
	double TwoGauss(double y, double A, double sig, double y0);
};

class RapidityFit
{
	std::vector<double> y, dndy, err;
	RapidityFitParameters Parameters;
public:
    RapidityFit(const std::vector<double> &y_, const std::vector<double> &dndy_, const std::vector<double> &err_, double A_ = 200., double sig_ = 1., double yav_ = 0.2):
            y(y_), dndy(dndy_), err(err_), Parameters(A_, sig_, yav_)
    {
    }
	~RapidityFit(void);
	void SetFitFlag(const std::string& name, bool flag) {
		Parameters.SetParameterFitFlag(name, flag);
	}
	void SetData(const std::vector<double> &y_, const std::vector<double> &dndy_, const std::vector<double> &err_) {
		y = y_;
		dndy = dndy_;
		err = err_;
	}

	RapidityFitParameters PerformFit();
};

#endif
