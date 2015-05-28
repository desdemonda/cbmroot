#pragma once
#ifndef THERMALMODELEVMF_H
#define THERMALMODELEVMF_H
#include "ThermalModelBase.h"
#include <cmath>

#ifndef PI
#define PI 3.141592653589793
#endif


class ThermalModelEVMF : public ThermalModelBase
{
public:
	std::vector<double> densitiesid;
	double fSuppression;
	double fDensity;
	double RHad;
	int fMode;
    ThermalModelEVMF(ThermalParticleSystem *TPS_, double T=0.16, double muB=0.23, double muS=0.1, double muQ=-0.01, double gammaS=1., double V=4000., double R=1., int mode=0):
        ThermalModelBase(TPS_, T, muB, muS, muQ, gammaS, V, R),
		densitiesid(),
		fSuppression(1.),
		fDensity(0.),
        RHad(R),
		fMode(mode)
			{
				densities.resize(TPS->fParticles.size());
				densitiesid.resize(TPS->fParticles.size());
				densitiestotal.resize(TPS->fParticles.size());
			}
	ThermalModelEVMF(ThermalParticleSystem *TPS_, const ThermalModelParameters& params, double RHad_ = 0., int mode=0):
        ThermalModelBase(TPS_, params),
		densitiesid(),
		fSuppression(1.),
		fDensity(0.),
		RHad(RHad_),
		fMode(mode)
		{
			densities.resize(TPS->fParticles.size());
			densitiesid.resize(TPS->fParticles.size());
			densitiestotal.resize(TPS->fParticles.size());
            if (TPS->fParticles.size()>0) RHad = TPS->fParticles[0].fRadius;
            fVolume = 4000.;
		}
    virtual ~ThermalModelEVMF(void);
    virtual void SetParameters(double T, double muB, double muS, double muQ, double gammaS, double V, double R) {
		Parameters.T = T;
		Parameters.muB = muB;
		Parameters.muS = muS;
		Parameters.muQ = muQ;
		Parameters.gammaS = gammaS;
		Parameters.R = R;
        Parameters.V = V;
		fCalculated = false;
        RHad = R;
        fVolume = V;
	}
	void setRadius(double rad) {
		Parameters.R = rad;
		RHad = rad;
	}
	void setMode(int mode) {
		fMode = mode;
	}
    virtual void SetParameters(const ThermalModelParameters& params) {
		Parameters = params;
		fCalculated = false;
        RHad = params.R;
	}
    virtual void ChangeTPS(ThermalParticleSystem *TPS_) {
        ThermalModelBase::ChangeTPS(TPS_);
        densitiesid.resize(TPS->fParticles.size());
        densities.resize(TPS->fParticles.size());
        densitiestotal.resize(TPS->fParticles.size());
    }
    virtual void FixParameters();
    virtual void FixParameters(double QB);	// And zero net strangeness
    virtual void CalculateDensities();
	double Density(double n);
	double UVdW(double n, double T, double vo) {
		// van der Waals
		if (fMode==0) return T * vo * n / (1. - vo * n) - T * log(1. - vo * n);
		
		// Directly excluded
		if (fMode==1) return -T * log(1. - 2.*vo * n);
		//return T * 2. * vo * n;

		// Carnahan-Starling
		if (fMode==2) {
			double tmp = vo * n - 4.;
			return -T * (3. + 16. * (-vo * n + 12.) / tmp / tmp / tmp);
		}
		
		// Pade approximation for hard spheres
		double x = vo*n;
		double b1 = 0.063597, b2 = 0.017329, b3 = 0.561493, b4 = 0.081313;
		double u = 1 + b1*x + b2*x*x;
		double d = 1 - b3*x + b4*x*x;
		double c1 = b2 / b4, c2 = (b1*b4 + b2*b3) / 2. / b4 / b4, c3 = 1. + (b1*b3*b4 + b2*b3*b3 - 2.*b2*b4)/2./b4/b4,
			c4 = 1./sqrt(4.*b4-b3*b3);
		return T * (x * (c1 + u/d) + c2*log(d) + 2.*c3*c4*(atan(c4*(2.*b4*x-b3))+atan(c4*b3)));

		// Virial expansion for hard spheres
		double b[8];
		b[2] = 4 * vo, b[3] = 10. * vo * vo, b[4] = 18.365 * vo * vo * vo, b[5] = 28.24 * vo * vo * vo * vo, b[6] = 39.5 * vo * vo * vo * vo * vo, b[7] = 56.5 * vo * vo * vo * vo * vo * vo;
		double ret = 0.;
		for(int i=7;i>=2;--i) {
			ret = ret * n + i * b[i] / (i-1.) * n;
		}
		return ret * T;
	}
	double PVdW(double n, double T, double vo) {
		// van der Waals
		if (fMode==0) return T * n * vo * n / (1. - vo * n);

		// Directly excluded
		if (fMode==1) return -T * (n  + log(1. - 2.*vo*n) / vo/2.);
		//return T * vo * n * n;

		// Carnahan-Starling
		if (fMode==2) {
			double tmp = (1 - vo * n / 4.);
			return T * n * (vo * n - (vo * n * vo * n) / 8.) / tmp / tmp / tmp;
		}

		// Pade approximation
		double x = vo*n;
		double b1 = 0.063597, b2 = 0.017329, b3 = 0.561493, b4 = 0.081313;
		double u = 1 + b1*x + b2*x*x;
		double d = 1 - b3*x + b4*x*x;
		return T * x * x / vo * u / d;

		// Virial expansion
		double b[8];
		b[2] = 4 * vo, b[3] = 10. * vo * vo, b[4] = 18.365 * vo * vo * vo, b[5] = 28.24 * vo * vo * vo * vo, b[6] = 39.5 * vo * vo * vo * vo * vo, b[7] = 56.5 * vo * vo * vo * vo * vo * vo;
		double ret = 0.;
		for(int i=7;i>=2;--i) {
			ret = ret * n + b[i] * n;
		}
		return ret * T * n;
	}

	double GetEffectiveVO(double n, double T, double vo) {
		//return 1./n * (1. - (1.-vo*n)*exp(-vo*n/(1.-vo*n)));
		return 1./n * (1. - exp(-UVdW(n,T,vo)/T));
	}

    virtual double GetParticlePrimordialDensity(int part) {
        if (!fCalculated) CalculateDensities();
        if (part>=static_cast<int>(densities.size())) return 0.;
        return densities[part];
    }
    virtual double GetParticleTotalDensity(int part) {
        if (!fCalculated) CalculateDensities();
        if (part>=static_cast<int>(densitiestotal.size())) return 0.;
        return densitiestotal[part];
    }

    virtual double CalculateHadronDensity() {
		if (!fCalculated) CalculateDensities();
		double ret = 0.;
		for(unsigned int i=0;i<TPS->fParticles.size();++i)
			ret += densities[i];
		if (fUseHagedorn) ret += fHagedornDensity;
		return ret;
	}
    virtual double CalculateBaryonDensity() {
		if (!fCalculated) CalculateDensities();
		double ret = 0.;
		for(unsigned int i=0;i<TPS->fParticles.size();++i)
			ret += TPS->fParticles[i].fB * densities[i];
		return ret;
	}
    virtual double CalculateChargeDensity() {
		if (!fCalculated) CalculateDensities();
		double ret = 0.;
		for(unsigned int i=0;i<TPS->fParticles.size();++i)
			ret += TPS->fParticles[i].fC * densities[i];
		return ret;
	}
    virtual double CalculateStrangenessDensity() {
		if (!fCalculated) CalculateDensities();
		double ret = 0.;
		for(unsigned int i=0;i<TPS->fParticles.size();++i)
			ret += TPS->fParticles[i].fS * densities[i];
		return ret ;
	}
    virtual double CalculateCharmDensity() {
        if (!fCalculated) CalculateDensities();
        double ret = 0.;
        for(unsigned int i=0;i<TPS->fParticles.size();++i)
            ret += TPS->fParticles[i].fCharm * densities[i];
        return ret ;
    }
    virtual double CalculateAbsoluteStrangenessDensity() {
		if (!fCalculated) CalculateDensities();
		double ret = 0.;
		for(unsigned int i=0;i<TPS->fParticles.size();++i)
			ret += TPS->fParticles[i].fAbsS * densities[i];
		return ret;
	}
    virtual double CalculateAbsoluteCharmDensity() {
        if (!fCalculated) CalculateDensities();
        double ret = 0.;
        for(unsigned int i=0;i<TPS->fParticles.size();++i)
            ret += TPS->fParticles[i].fAbsC * densities[i];
        return ret;
    }
    virtual double CalculateEnergyDensity() {
		double ret = 0.;
		double vo = 0.;
		vo = 4. * 4. * PI / 3. * RHad * RHad * RHad;
		double dMu = -UVdW(fDensity, Parameters.T, vo);
		for(unsigned int i=0;i<TPS->fParticles.size();++i) {
			ret += TPS->fParticles[i].CalculateDensity(Parameters.T, Parameters.muB, Parameters.muS, Parameters.muQ, Parameters.gammaS, 1, fUseWidth, dMu);
		}
		if (fUseHagedorn) ret += fHag.CalculateDensity(Parameters.T,  Parameters.muB, 1, dMu);
		return ret;// / fSuppression;
	}
    virtual double CalculateEntropyDensity() {
		return (1./Parameters.T) * (CalculateEnergyDensity() + CalculatePressure() - Parameters.muB * CalculateBaryonDensity() - Parameters.muS * CalculateStrangenessDensity()  - Parameters.muQ * CalculateChargeDensity());
	}

    // Dummy
    virtual double CalculateBaryonMatterEntropyDensity() {
        double ret = 0.;
        return ret;
    }
    virtual double CalculateMesonMatterEntropyDensity() {
        double ret = 0.;
        return ret;
    }

    virtual double CalculatePressure() {
		if (!fCalculated) CalculateDensities();
		double ret = 0.;
		double vo = 0.;
		vo = 4. * 4. * PI / 3. * RHad * RHad * RHad;
		double dMu = -UVdW(fDensity, Parameters.T, vo);
		for(unsigned int i=0;i<TPS->fParticles.size();++i) {
			ret += TPS->fParticles[i].CalculateDensity(Parameters.T, Parameters.muB, Parameters.muS, Parameters.muQ, Parameters.gammaS, 3, fUseWidth, dMu);
		}
		if (fUseHagedorn) ret += fHag.CalculateDensity(Parameters.T,  Parameters.muB, 3, dMu);
		return ret + PVdW(fDensity, Parameters.T, vo);
	}

    virtual double CalculateShearViscosity();
};

#endif

