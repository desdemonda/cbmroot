#include "RapidityFitBlastWave.h"

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


namespace RapidityFitBlastWaveNamespace {
	double dndy(double y, double A, double eta, double T, double mass) {
		double ret = 0.;
		int iters = 100;
		double deta = 2. * eta / iters;
		for(int i=0;i<iters;++i)
		{
			double et = -eta + (0.5 + i) * deta;
			ret += (mass*mass/T/T + mass / T * 2. / TMath::CosH(y - et) + 2. / TMath::CosH(y - et) / TMath::CosH(y - et) ) * TMath::Exp(-mass / T * TMath::CosH(y - et));
		}
		return ret * deta * A * T * T * T;
	  }
	  
	class dndyFCN : public FCNBase {

	public:

	  dndyFCN(const std::vector<double> &y_, const std::vector<double> &dndy_, const std::vector<double> &err_, double T, double mass):
	  fPositions(y_), fMeasurements(dndy_), fErrors(err_), fT(T), fMass(mass), iter(0)
	  { 
	  }

	  ~dndyFCN() {}

	  double operator()(const std::vector<double>& par) const {
		double chi2 = 0.;
        for(unsigned int i=0;i<fPositions.size();++i) {
			double val = dndy(fPositions[i], par[0], par[1], fT, fMass);
			chi2 += (fMeasurements[i] - val) * (fMeasurements[i] - val) / fErrors[i] / fErrors[i];
		}
		return chi2;
	  }

	  double Up() const {return 1.;}

	private:
	  std::vector<double> fPositions;
	  std::vector<double> fMeasurements;
	  std::vector<double> fErrors;	  
	  double fT, fMass;
	  int iter;
	};
}

using namespace RapidityFitBlastWaveNamespace;

RapidityFitBlastWave::~RapidityFitBlastWave(void)
{
}

RapidityFitBlastWaveParameters RapidityFitBlastWave::PerformFit() {
    dndyFCN mfunc(y, dndy, err, fT, fMass);
    std::vector<double> params(2, 0.);
	params[0] = Parameters.A.value;
	params[1] = Parameters.eta.value;
	//params[2] = Parameters.yav.value;
	MnUserParameters upar;
	upar.Add("A", Parameters.A.value, Parameters.A.error, Parameters.A.xmin, Parameters.A.xmax);
	upar.Add("eta", Parameters.eta.value, Parameters.eta.error, Parameters.eta.xmin, Parameters.eta.xmax);
	//upar.Add("yav", Parameters.yav.value, Parameters.yav.error, Parameters.yav.xmin, Parameters.yav.xmax);
    int nparams = 2;

	MnMigrad migrad(mfunc, upar);
	//std::cout<<"start migrad "<<std::endl;
	FunctionMinimum min = migrad();
    MnHesse hess;
    hess(mfunc, min);
	RapidityFitBlastWaveParameters ret = Parameters;
	ret.A.value = (min.UserParameters()).Params()[0];
	ret.A.error = (min.UserParameters()).Errors()[0];
	ret.eta.value = (min.UserParameters()).Params()[1];
	ret.eta.error = (min.UserParameters()).Errors()[1];
	//ret.yav.value = (min.UserParameters()).Params()[2];
	//ret.yav.error = (min.UserParameters()).Errors()[2];

	params[0] = ret.A.value;
	params[1] = ret.eta.value;
	//params[2] = ret.yav.value;
	std::cout << "A = " << params[0] << " " << " eta = " << params[1] << " " << 
                 " chi2/ndf = " << mfunc(params) / (y.size() - nparams) << std::endl;

    ret.chi2ndf = mfunc(params) / (y.size() - nparams);

	return ret;
}

