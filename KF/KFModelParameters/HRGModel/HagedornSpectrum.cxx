#include "HagedornSpectrum.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "xMath.h"
#include <cmath>

namespace HagedornSpectrumNamespace {
	#include "NumericalIntegration.h"
}

using namespace std;
using namespace HagedornSpectrumNamespace;

const double HagedornSpectrum::GeVtoifm = 1./0.197;//5.06773;

HagedornSpectrum::~HagedornSpectrum(void)
{
}

double HagedornSpectrum::CalculateParticleDensity(double T, double muB, double mass, double dMu) const {
    return xMath::BesselK(2, mass/T) * exp((muB+dMu)/T) / 2. / xMath::Pi() / xMath::Pi() * mass * mass * T * GeVtoifm * GeVtoifm * GeVtoifm;
}

double HagedornSpectrum::CalculateDensity(double T, double muB, int type, double dMu) const {
	double ret = 0.;

	double mMax = 80.;
	int iters = 5000;
	double dM = (mMax-fM0) / iters;
	for(int i=0;i<iters;++i) {
		double x = fM0 + (0.5 + i) * dM;
		if (type==0) ret += fC * pow((x)*(x)+fmo*fmo,-5./4.) * exp((x)/fTH) * CalculateParticleDensity(T, muB, x, dMu);
		if (type==1) ret += fC * pow((x)*(x)+fmo*fmo,-5./4.) * exp((x)/fTH) * CalculateEnergyDensity(T, muB, x, dMu);
		if (type==2) ret += fC * pow((x)*(x)+fmo*fmo,-5./4.) * exp((x)/fTH) * CalculateEntropyDensity(T, muB, x, dMu);
		if (type==3) ret += fC * pow((x)*(x)+fmo*fmo,-5./4.) * exp((x)/fTH) * CalculatePressure(T, muB, x, dMu);
	}
	return ret * dM;
}

double HagedornSpectrum::CalculatePressure(double T, double muB, double mass, double dMu) const {
	return T * CalculateParticleDensity(T, muB, mass, dMu);
}

double HagedornSpectrum::CalculateEntropyDensity(double T, double muB, double mass, double dMu) const {
	double ret = 0.;
	muB /= T;
	dMu /= T;
	double tMu = muB + dMu;
	double tMass = mass / T;
	ret = 0.;

	vector<double> x, w;
	GetCoefsIntegrateLaguerre32(x,w);

	for(int i = 0 ; i < 32 ; i++){
		double x2 = x[i] * x[i];
		double E = sqrt(x[i]*x[i]+tMass*tMass);
        ret += w[i] * x2 * exp(-E + tMu)  * (x2 / 3. / E + E - tMu);
	}

    return ret / 2. / xMath::Pi() / xMath::Pi() * T * T * T * GeVtoifm * GeVtoifm * GeVtoifm;
}

double HagedornSpectrum::CalculateEnergyDensity(double T, double muB, double mass, double dMu) const {
	return (3*T + mass * xMath::BesselK1(mass/T)/xMath::BesselK(2,mass/T)) * CalculateParticleDensity(T, muB, mass, dMu);
	double ret = 0.;
	muB /= T;
	dMu /= T;
	double tMu = muB + dMu;
	double tMass = mass / T;
	ret = 0.;

	vector<double> x, w;
	GetCoefsIntegrateLaguerre32(x,w);

	if (tMu>tMass) return 0.;//cout << "AAAAAAA";

	for(int i = 0 ; i < 32 ; i++) {
		//double x2 = x[i] * x[i];
		double E = sqrt(x[i]*x[i]+tMass*tMass);
        ret += w[i] * x[i] * x[i] * E * exp(-E + tMu);
	}

    return ret / 2. / xMath::Pi() / xMath::Pi() * T * T * T * T * GeVtoifm * GeVtoifm * GeVtoifm;
}
