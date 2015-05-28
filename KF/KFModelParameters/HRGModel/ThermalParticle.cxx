#include "ThermalParticle.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "xMath.h"
#include <cmath>


namespace ThermalParticleNamespace {
	#include "NumericalIntegration.h"
}

using namespace std;
using namespace ThermalParticleNamespace;

const double ThermalParticle::GeVtoifm = 1./0.197;//5.06773;

ThermalParticle::~ThermalParticle(void)
{
}

void ThermalParticle::ReadDecays(string filename) {
	fDecays.resize(0);
	ifstream fin(filename.c_str());
	if (fin.is_open()) {
		char cc[400];
		double tmpbr;
		while (fin >> tmpbr) {
			ParticleDecay decay;
			decay.fBratio = tmpbr / 100.;
			fin.getline(cc, 350);
			stringstream ss;
			ss << cc;
			int tmpid;
			while (ss >> tmpid) {
				decay.fDaughters.push_back(tmpid);
			}
			fDecays.push_back(decay);
		}
	}
}

void ThermalParticle::NormalizeBranchingRatios() {
	double sum = 0.;
	for(unsigned int i=0;i<fDecays.size();++i) {
		sum += fDecays[i].fBratio;
	}
	for(unsigned int i=0;i<fDecays.size();++i) {
		fDecays[i].fBratio *= 1./sum;
	}
}

double ThermalParticle::CalculateParticleDensity(double T, double muB, double muS, double muQ, double gammaS, double mass, double dMu) const {
	if (fStatistics==0) {
		if (fabs(gammaS-1.)<1e-5)
			return fSpinDegeneracy * xMath::BesselKexp(2, mass/T) * exp((fB*muB+fS*muS+fC*muQ+dMu-mass)/T) / 2. / xMath::Pi() / xMath::Pi() * mass * mass * T * GeVtoifm * GeVtoifm * GeVtoifm;
		else 
			return fSpinDegeneracy * xMath::BesselKexp(2, mass/T) * pow(gammaS, fAbsS) * exp((fB*muB+fS*muS+fC*muQ+dMu-mass)/T) / 2. / xMath::Pi() / xMath::Pi() * mass * mass * T * GeVtoifm * GeVtoifm * GeVtoifm;
	}
	double eps = 1e-2;
	double ret = 0.;
	double cur = 0.;//prev = 0., cur = 0.;
	if (mass<0.) mass = fMass;
	muB /= T;
	muS /= T;
	muQ /= T;
	dMu /= T;
	double tMu = fB*muB + fS*muS + fC*muQ + dMu;
	double tMass = mass / T;
	double Sign = 1.;
	if (tMu>tMass) return 0.;
	ret = 0.;
    if (tMass>70.) return 0.;
	for(int i=1;i<10;++i) {
        cur = xMath::BesselK(2, i*tMass) * exp(tMu*i - log(pow(gammaS, -fAbsS))*i) / i;
		ret += Sign*cur;
        if (i!=1 && cur/ret<eps) break;
		//prev = cur;
		if (fStatistics==1) Sign *= -1.;
		if (fStatistics==0) break;
	}
    return ret * fSpinDegeneracy / 2. / xMath::Pi() / xMath::Pi() * mass * mass * T * GeVtoifm * GeVtoifm * GeVtoifm;
}

double ThermalParticle::CalculateDensity(double T, double muB, double muS, double muQ, double gammaS, int type, bool useWidth, double dMu) const {
	if (!useWidth || fWidth/fMass<1e-2) {
		if (type==0) return CalculateParticleDensity(T, muB, muS, muQ, gammaS, fMass, dMu);
		if (type==1) return CalculateEnergyDensity(T, muB, muS, muQ, gammaS, fMass, dMu);
		if (type==2) return CalculateEntropyDensity(T, muB, muS, muQ, gammaS, fMass, dMu);
		if (type==3) return CalculatePressure(T, muB, muS, muQ, gammaS, fMass, dMu);
		return 0;
	}
	vector<double> x, w;
	double a = max(fThreshold, fMass - 2.*fWidth), b = fMass + 2.*fWidth;
	int ind = 5;
	if (fWidth/fMass<1e-1) { ind = 10; GetCoefsIntegrateLegendre10(a, b, x, w); }
	else { ind = 32; GetCoefsIntegrateLegendre32(a, b, x, w); }
	double ret1 = 0., ret2 = 0., tmp = 0.;

	for(int i = 0 ; i < ind ; i++) {
		tmp = w[i] * fMass * fWidth * x[i] / ( (x[i]*x[i]-fMass*fMass)*(x[i]*x[i]-fMass*fMass) + fMass*fMass*fWidth*fWidth );
		double dens = 0.;
		if (type==0) dens = CalculateParticleDensity(T, muB, muS, muQ, gammaS, x[i], dMu);
		if (type==1) dens = CalculateEnergyDensity(T, muB, muS, muQ, gammaS, x[i], dMu);
		if (type==2) dens = CalculateEntropyDensity(T, muB, muS, muQ, gammaS, x[i], dMu);
		if (type==3) dens = CalculatePressure(T, muB, muS, muQ, gammaS, x[i], dMu);
		ret1 += tmp * dens;
		ret2 += tmp;
	}
	return ret1 / ret2;
}

double ThermalParticle::CalculateParticleDensityLaguerre(double T, double muB, double muS, double muQ, double gammaS, double mass, double dMu) const {
	if (fStatistics==0) 
		return CalculateParticleDensity(T, muB, muS, muQ, gammaS, mass);
	double ret = 0.;
	if (mass<0.) mass = fMass;
	muB /= T;
	muS /= T;
	muQ /= T;
	dMu /= T;
	double tMu = fB*muB + fS*muS + fC*muQ + dMu;
	double tMass = mass / T;
	ret = 0.;

	vector<double> x, w;
	GetCoefsIntegrateLaguerre32(x,w);

	if (tMu>tMass) return 0.;

	for(int i = 0 ; i < 32 ; i++){
        ret += w[i] * x[i] * x[i] / (exp(sqrt(x[i]*x[i]+tMass*tMass) - tMu) * pow(gammaS, -fAbsS) + fStatistics);
	}

    return ret * fSpinDegeneracy / 2. / xMath::Pi() / xMath::Pi() * T * T * T * GeVtoifm * GeVtoifm * GeVtoifm;

}

double ThermalParticle::CalculatePressure(double T, double muB, double muS, double muQ, double gammaS, double mass, double dMu) const {
	if (fStatistics==0) 
		return T * CalculateParticleDensity(T, muB, muS, muQ, mass, dMu);
	double ret = 0.;
	if (mass<0.) mass = fMass;
	muB /= T;
	muS /= T;
	muQ /= T;
	dMu /= T;
	double tMu = fB*muB + fS*muS + fC*muQ + dMu;
	double tMass = mass / T;
	//double Sign = 1.;
	ret = 0.;

	vector<double> x, w;
	GetCoefsIntegrateLaguerre32(x,w);
	//double ret = 0.;

	if (tMu>tMass) return 0.;//cout << "AAAAAAA";

	for(int i = 0 ; i < 32 ; i++) {
		double x2 = x[i] * x[i];
		double E = sqrt(x[i]*x[i]+tMass*tMass);
        ret += w[i] * x2 * x2 / (exp(E - tMu) * pow(gammaS, -fAbsS) + fStatistics) / E;
	}

    return ret * fSpinDegeneracy / 6. / xMath::Pi() / xMath::Pi() * T * T * T * T * GeVtoifm * GeVtoifm * GeVtoifm;
}

double ThermalParticle::CalculateEntropyDensity(double T, double muB, double muS, double muQ, double gammaS, double mass, double dMu) const {
	if (fStatistics==0)
		return ( CalculateEnergyDensity(T, muB, muS, muQ, gammaS, mass, dMu) 
		+ CalculatePressure(T, muB, muS, muQ, gammaS, mass, dMu) 
		- (fB*muB+fS*muS+fC*muQ+dMu) * CalculateParticleDensity(T, muB, muS, muQ, gammaS, mass, dMu) ) / T;
	double ret = 0.;
	if (mass<0.) mass = fMass;
	muB /= T;
	muS /= T;
	muQ /= T;
	dMu /= T;
	double tMu = fB*muB + fS*muS + fC*muQ + dMu;
	double tMass = mass / T;
	//double Sign = 1.;
	ret = 0.;

	vector<double> x, w;
	GetCoefsIntegrateLaguerre32(x,w);
	//double ret = 0.;

	if (tMu>tMass) return 0.;//cout << "AAAAAAA";

	for(int i = 0 ; i < 32 ; i++){
		double x2 = x[i] * x[i];
		double E = sqrt(x[i]*x[i]+tMass*tMass);
        ret += w[i] * x2 / (exp(E - tMu) * pow(gammaS, -fAbsS) + fStatistics) * (x2 / 3. / E + E - tMu);
	}

    return ret * fSpinDegeneracy / 2. / xMath::Pi() / xMath::Pi() * T * T * T * GeVtoifm * GeVtoifm * GeVtoifm;
}

double ThermalParticle::CalculateEnergyDensity(double T, double muB, double muS, double muQ, double gammaS, double mass, double dMu) const {
	if (fStatistics==0)
		return (3*T + mass * xMath::BesselK1exp(mass/T) / xMath::BesselKexp(2,mass/T)) * CalculateParticleDensity(T, muB, muS, muQ, mass, dMu);
	double ret = 0.;
	if (mass<0.) mass = fMass;
	muB /= T;
	muS /= T;
	muQ /= T;
	dMu /= T;
	double tMu = fB*muB + fS*muS + fC*muQ + dMu;
	double tMass = mass / T;
	ret = 0.;

	vector<double> x, w;
	GetCoefsIntegrateLaguerre32(x,w);

	if (tMu>tMass) return 0.;

	for(int i = 0 ; i < 32 ; i++) {
		//double x2 = x[i] * x[i];
		double E = sqrt(x[i]*x[i]+tMass*tMass);
        ret += w[i] * x[i] * x[i] * E / (exp(E - tMu) * pow(gammaS, -fAbsS) + fStatistics);
	}

    return ret * fSpinDegeneracy / 2. / xMath::Pi() / xMath::Pi() * T * T * T * T * GeVtoifm * GeVtoifm * GeVtoifm;
}
