#ifndef THERMALMODELBASE_H
#define THERMALMODELBASE_H
#include "ThermalParticleSystem.h"
#include "HagedornSpectrum.h"

struct ThermalModelParameters {
    double T, muB, muS, muQ, gammaS, V, R;
    ThermalModelParameters(double T_=0.1, double muB_=0.5, double muS_=0., double muQ_=0., double gammaS_=1., double V_ = 4000., double R_=1.):
        T(T_), muB(muB_), muS(muS_), muQ(muQ_), gammaS(gammaS_), V(V_), R(R_)
    {
    }
};

class ThermalModelBase
{
public:
	ThermalParticleSystem *TPS;
    ThermalModelParameters Parameters;
	bool fUseWidth;
    bool fCalculated;
    bool fNormBratio;
    bool fQuantumStats;
	bool fUseHagedorn;
    double QBgoal;
    double fVolume;

    bool useOpenMP;

    std::vector<double> densities;
    std::vector<double> densitiestotal;

	HagedornSpectrum fHag;
	double fHagedornDensity;

    ThermalModelBase(ThermalParticleSystem *TPS_, double T=0.125, double muB=0.45, double muS=0.1, double muQ=-0.01, double gammaS=1., double V = 4000., double R=1.):
        TPS(TPS_),
        Parameters(T,muB,muS,muQ,gammaS,V,R),
        fUseWidth(false),
        fCalculated(false),
        fNormBratio(false),
        fQuantumStats(true),
		fUseHagedorn(false),
		QBgoal(0.4),
		fVolume(V),
		useOpenMP(0),
		densities(),
		densitiestotal(),
		fHag(),
		fHagedornDensity(0.)
        {
            // QBgoal = 0.4;
            // fVolume = V;
            Parameters.muS = muB / 5.;
            Parameters.muQ = -muB / 50.;
			// fUseHagedorn = false;
        }
    ThermalModelBase(/*std::string InputFile="",*/ ThermalParticleSystem *TPS_, const ThermalModelParameters& params):
        TPS(TPS_), 
        Parameters(params),
        fUseWidth(false),
        fCalculated(false),
        fNormBratio(false),
        fQuantumStats(true),
		fUseHagedorn(false),
		QBgoal(0.4),
		fVolume(4000.),
		useOpenMP(0),
		densities(),
		densitiestotal(),
		fHag(),
		fHagedornDensity(0.)
        {
            // QBgoal = 0.4;
        }
    virtual ~ThermalModelBase(void) { }
    void SetUseWidth(bool useWidth) {
        fUseWidth = useWidth;
    }
    void SetNormBratio(bool normBratio) {
        if (normBratio!=fNormBratio) {
            fNormBratio = normBratio;
            if (fNormBratio) TPS->NormalizeBranchingRatios();
            else TPS->RestoreBranchingRatios();
        }
    }
    void SetOMP(bool openMP) {
        useOpenMP = openMP;
    }
	 void SetHagedorn(bool useHagedorn, double M0 = 3., double TH = 0.160, double a = 3., double C = 1.) {
        fUseHagedorn = useHagedorn;
		if (useHagedorn) fHag = HagedornSpectrum(M0, TH, a, C); 
    }
    virtual void SetParameters(double T, double muB, double muS, double muQ, double gammaS, double V, double R) {
        Parameters.T = T;
        Parameters.muB = muB;
        Parameters.muS = muS;
        Parameters.muQ = muQ;
        Parameters.gammaS = gammaS;
        Parameters.V = V;
        Parameters.R = R;
        fVolume = 4000.;
        fCalculated = false;
    }
    virtual void SetParameters(const ThermalModelParameters& params) {
        Parameters = params;
        fCalculated = false;
    }
    virtual void ChangeTPS(ThermalParticleSystem *TPS_) {
        TPS = TPS_;
        fCalculated = false;
    }

    // 0 - Boltzmann, 1 - Quantum
    virtual void SetStatistics(bool stats) {
        fQuantumStats = stats;
        for(unsigned int i=0;i<TPS->fParticles.size();++i)
            TPS->fParticles[i].useStatistics(stats);
    }


    void SetQBgoal(double QB) { QBgoal = QB; }
    void SetVolume(double Volume) { fVolume = Volume; Parameters.V = Volume; }
    virtual void FixParameters() { }
    virtual void FixParameters(double QB) { }	// And zero net strangeness
    virtual void CalculateDensities() { }
    virtual double CalculateHadronDensity() = 0;
    virtual double GetParticlePrimordialDensity(int) = 0;
    virtual double GetParticleTotalDensity(int) = 0;
    virtual double CalculateBaryonDensity() = 0;
    virtual double CalculateChargeDensity() = 0;
    virtual double CalculateStrangenessDensity() = 0;
    virtual double CalculateCharmDensity() = 0;
    virtual double CalculateAbsoluteStrangenessDensity() = 0;
    virtual double CalculateAbsoluteCharmDensity() = 0;
    virtual double CalculateEnergyDensity() = 0;
    virtual double CalculateEntropyDensity() = 0;
    virtual double CalculateBaryonMatterEntropyDensity() = 0;
    virtual double CalculateMesonMatterEntropyDensity() = 0;
    virtual double CalculatePressure() = 0;
    virtual double CalculateShearViscosity() = 0;
	
	ThermalModelBase(const ThermalModelBase&);
    ThermalModelBase& operator=(const ThermalModelBase&);
};

#endif // THERMALMODELBASE_H
