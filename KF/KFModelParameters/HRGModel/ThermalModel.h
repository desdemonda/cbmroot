#ifndef THERMALMODEL_H
#define THERMALMODEL_H
#include "ThermalModelBase.h"

#include <cmath>
//#include <omp.h>


class ThermalModel : public ThermalModelBase
{
public:
    ThermalModel(/*std::string InputFile="",*/ ThermalParticleSystem *TPS_, double T=0.125, double muB=0.45, double muS=0.1, double muQ=-0.01, double gammaS=1., double V=4000., double R=1.):
        ThermalModelBase(TPS_, T, muB, muS, muQ, gammaS, V, R)
		{
			densities.resize(TPS->fParticles.size());
			densitiestotal.resize(TPS->fParticles.size());
		}
	ThermalModel(/*std::string InputFile="",*/ ThermalParticleSystem *TPS_, const ThermalModelParameters& params):
        ThermalModelBase(TPS_, params)
		{
			densities.resize(TPS->fParticles.size());
			densitiestotal.resize(TPS->fParticles.size());
		}
    virtual ~ThermalModel(void);

    virtual void ChangeTPS(ThermalParticleSystem *TPS_) {
        ThermalModelBase::ChangeTPS(TPS_);
        densities.resize(TPS->fParticles.size());
        densitiestotal.resize(TPS->fParticles.size());
    }

    virtual void FixParameters();
    virtual void FixParameters(double QB);	// And zero net strangeness
	void SolveTwoRatios(int PDG11, int PDG12, double R1, int PDG21, int PDG22, double R2, double QB=0.5);	// And zero strangeness
    virtual void CalculateDensities() {
            for(unsigned int i=0;i<TPS->fParticles.size();++i) {
                densities[i] = TPS->fParticles[i].CalculateDensity(Parameters.T, Parameters.muB, Parameters.muS, Parameters.muQ, Parameters.gammaS, 0, fUseWidth);
            }

        //#pragma omp parallel for
		for(int i=0;i<static_cast<int>(TPS->fParticles.size());++i) {
            //if (TPS->fParticles[i].fStable==0) densitiestotal[i] = 0.;
            //else
            {
				densitiestotal[i] = densities[i];
				for(int j=0;j<static_cast<int>(TPS->fParticles[i].ResonanceBR.size());++j)
                    if (i!=TPS->fParticles[i].ResonanceBR[j].second) densitiestotal[i] += TPS->fParticles[i].ResonanceBR[j].first * densities[TPS->fParticles[i].ResonanceBR[j].second];
			}
		}
		fCalculated = true;
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
		for(unsigned int i=0;i<TPS->fParticles.size();++i) ret += TPS->fParticles[i].CalculateDensity(Parameters.T, Parameters.muB, Parameters.muS, Parameters.muQ, Parameters.gammaS, 1, fUseWidth);
		return ret;
	}
    virtual double CalculateEntropyDensity() {
		double ret = 0.;
		for(unsigned int i=0;i<TPS->fParticles.size();++i) ret += TPS->fParticles[i].CalculateDensity(Parameters.T, Parameters.muB, Parameters.muS, Parameters.muQ, Parameters.gammaS, 2, fUseWidth);
		return ret;
	}
    virtual double CalculateBaryonMatterEntropyDensity() {
        double ret = 0.;
        for(unsigned int i=0;i<TPS->fParticles.size();++i)
            if (TPS->fParticles[i].fB!=0)
                ret += TPS->fParticles[i].CalculateDensity(Parameters.T, Parameters.muB, Parameters.muS, Parameters.muQ, Parameters.gammaS, 2, fUseWidth);
        return ret;
    }
    virtual double CalculateMesonMatterEntropyDensity() {
        double ret = 0.;
        for(unsigned int i=0;i<TPS->fParticles.size();++i)
            if (TPS->fParticles[i].fB==0)
                ret += TPS->fParticles[i].CalculateDensity(Parameters.T, Parameters.muB, Parameters.muS, Parameters.muQ, Parameters.gammaS, 2, fUseWidth);
        return ret;
    }
    virtual double CalculatePressure() {
		double ret = 0.;
		for(unsigned int i=0;i<TPS->fParticles.size();++i) ret += TPS->fParticles[i].CalculateDensity(Parameters.T, Parameters.muB, Parameters.muS, Parameters.muQ, Parameters.gammaS, 3, fUseWidth);
		return ret;
	}
    virtual double CalculateShearViscosity() {
        return 0.;
    }
};

#endif
