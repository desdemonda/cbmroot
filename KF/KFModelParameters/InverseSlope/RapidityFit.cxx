#include "RapidityFit.h"

#include "Minuit2/FCNBase.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MnHesse.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/SimplexMinimizer.h"
#include "TMath.h"

#include <fstream>


using namespace ROOT::Minuit2;


namespace RapidityFitNamespace {
	double TwoGauss(double y, double A, double sig, double y0) {
		return A / sqrt(2. * TMath::Pi()) / sig / 2. * (TMath::Exp(-(y-y0)*(y-y0)/2./sig/sig) + TMath::Exp(-(y+y0)*(y+y0)/2./sig/sig) );
	  }
	  
	class DoubleGaussFCN : public FCNBase {

	public:

	  DoubleGaussFCN(const std::vector<double> &y_, const std::vector<double> &dndy_, const std::vector<double> &err_):
	  fPositions(y_), fMeasurements(dndy_), fErrors(err_), iter(0)
	  { 
	  }

	  ~DoubleGaussFCN() {}

	  double operator()(const std::vector<double>& par) const {
		double chi2 = 0.;
        for(unsigned int i=0;i<fPositions.size();++i) {
			double val = TwoGauss(fPositions[i], par[0], par[1], par[2]);
			chi2 += (fMeasurements[i] - val) * (fMeasurements[i] - val) / fErrors[i] / fErrors[i];
		}
		return chi2;
	  }

	  double Up() const {return 1.;}

	private:
	  std::vector<double> fPositions;
	  std::vector<double> fMeasurements;
	  std::vector<double> fErrors;
	  int iter;
	};
}

using namespace RapidityFitNamespace;

RapidityFit::~RapidityFit(void)
{
}

RapidityFitParameters RapidityFit::PerformFit() {
    DoubleGaussFCN mfunc(y, dndy, err);
    std::vector<double> params(3, 0.);
	params[0] = Parameters.A.value;
	params[1] = Parameters.sig.value;
	params[2] = Parameters.yav.value;
	MnUserParameters upar;
	upar.Add("A", Parameters.A.value, Parameters.A.error, Parameters.A.xmin, Parameters.A.xmax);
	upar.Add("sig", Parameters.sig.value, Parameters.sig.error, Parameters.sig.xmin, Parameters.sig.xmax);
	upar.Add("yav", Parameters.yav.value, Parameters.yav.error, Parameters.yav.xmin, Parameters.yav.xmax);
    int nparams = 3;

	MnMigrad migrad(mfunc, upar);
	FunctionMinimum min = migrad();
    MnHesse hess;
    hess(mfunc, min);
	RapidityFitParameters ret = Parameters;
	ret.A.value = (min.UserParameters()).Params()[0];
	ret.A.error = (min.UserParameters()).Errors()[0];
	ret.sig.value = (min.UserParameters()).Params()[1];
	ret.sig.error = (min.UserParameters()).Errors()[1];
	ret.yav.value = (min.UserParameters()).Params()[2];
	ret.yav.error = (min.UserParameters()).Errors()[2];

	params[0] = ret.A.value;
	params[1] = ret.sig.value;
	params[2] = ret.yav.value;
	std::cout << "A = " << params[0] << " " << " sig = " << params[1] << " " << " y0 = " << params[2] << " " <<
                 " chi2/ndf = " << mfunc(params) / (y.size() - nparams) << std::endl;

    ret.chi2ndf = mfunc(params) / (y.size() - nparams);

	return ret;
}
