#ifndef THERMALMODELFIT_H
#define THERMALMODELFIT_H

#include <string>
#include "ThermalModelBase.h"
#include <cmath>

struct FitParameter {
	bool toFit;
	double value;
	double error;
	double xmin, xmax;
	std::string name;
	FitParameter(std::string name_="", bool toFit_=true, double val=1., double err=0., double vmin=-2., double vmax=2.):
		toFit(toFit_), value(val), error(err), xmin(vmin), xmax(vmax), name("") {
		}
};

struct ThermalModelFitParametersExtended {
    FitParameter T, muB, muS, muQ, gammaS, V;
    double chi2ndf;
    FitParameter nB, rhoB, rhoQ, en, entropy, pressure, eta;
    ThermalModelFitParametersExtended():
		T(), muB(), muS(), muQ(), gammaS(), V(), chi2ndf(),
		nB(), rhoB(), rhoQ(), en(), entropy(), pressure(), eta()
		{ }
    ThermalModelFitParametersExtended(ThermalModelBase *model):
			T(), muB(), muS(), muQ(), gammaS(), V(), chi2ndf(),
			nB(), rhoB(), rhoQ(), en(), entropy(), pressure(), eta() 
	{
        T.value = model->Parameters.T;
        muB.value = model->Parameters.muB;
        muS.value = model->Parameters.muS;
        muQ.value = model->Parameters.muQ;
        gammaS.value = model->Parameters.gammaS;
        V.value = model->Parameters.V;
        nB.value = model->CalculateHadronDensity();
        rhoB.value = model->CalculateBaryonDensity();
        rhoQ.value = model->CalculateChargeDensity();
        en.value = model->CalculateEnergyDensity();
        entropy.value = model->CalculateEntropyDensity();
        pressure.value = model->CalculatePressure();
        eta.value = model->CalculateShearViscosity() / model->CalculateEntropyDensity();
    }
};

struct ThermalModelFitParameters {
    FitParameter T, muB, muS, muQ, gammaS, V, R;
    double chi2ndf;
    ThermalModelFitParameters(double T_=0.12, double muB_=0.5, double muS_=0.1, double muQ_=-0.01, double gammaS_=1., double V_ = 4000., double R_=1.):
		T(FitParameter("T", true, T_, 0.05, 0.05, 0.18)),
		muB(FitParameter("muB", true, muB_, 0.05, 0., 1.)),
		muS(FitParameter("muS", false, muS_)),
		muQ(FitParameter("muQ", false, muQ_)),
		gammaS(FitParameter("gammaS", false, gammaS_, 0.2, 0.5, 1.5)),
		V(FitParameter("V", true, V_, 2000., 1., 20000.)),
		R(FitParameter("R", false, R_, 0., 0., 100.)),
		chi2ndf(1.)
	{
		// T = FitParameter("T", true, T_, 0.05, 0.05, 0.18);
		// muB = FitParameter("muB", true, muB_, 0.05, 0., 1.);
		// muS = FitParameter("muS", false, muS_);
		// muQ = FitParameter("muQ", false, muQ_);
        // gammaS = FitParameter("gammaS", false, gammaS_, 0.2, 0.5, 1.5);
        // V = FitParameter("V", true, V_, 2000., 1., 20000.);
		// R = FitParameter("R", false, R_, 0., 0., 100.);
	}
	FitParameter GetParamater(const std::string& name) const {
		if (T.name==name) return T;
		if (muB.name==name) return muB;
		if (muS.name==name) return muS;
		if (muQ.name==name) return muQ;
		if (gammaS.name==name) return gammaS;
        if (V.name==name) return V;
		if (R.name==name) return R;
		return FitParameter();
	}
	void SetParameter(const std::string& name, const FitParameter& param) {
		if (T.name==name) T = param;
		if (muB.name==name) muB = param;
		if (muS.name==name) muS = param;
		if (muQ.name==name) muQ = param;
		if (gammaS.name==name) gammaS = param;
        if (V.name==name) V = param;
		if (R.name==name) R = param;
	}
	void SetParameter(const std::string& name, double val, double err, double xmin, double xmax) {
		if (T.name==name) T = FitParameter(name, true, val, err, xmin, xmax);
		if (muB.name==name) muB = FitParameter(name, true, val, err, xmin, xmax);
		if (muS.name==name) muS = FitParameter(name, true, val, err, xmin, xmax);
		if (muQ.name==name) muQ = FitParameter(name, true, val, err, xmin, xmax);
		if (gammaS.name==name) gammaS = FitParameter(name, true, val, err, xmin, xmax);
        if (V.name==name) V = FitParameter(name, true, val, err, xmin, xmax);
		if (R.name==name) R = FitParameter(name, true, val, err, xmin, xmax);
	}
	void SetParameterFitFlag(const std::string& name, bool toFit) {
		if (T.name==name) T.toFit = toFit;
		if (muB.name==name) muB.toFit = toFit;
		if (muS.name==name) muS.toFit = toFit;
		if (muQ.name==name) muQ.toFit = toFit;
		if (gammaS.name==name) gammaS.toFit = toFit;
        if (V.name==name) V.toFit = toFit;
		if (R.name==name) R.toFit = toFit;
	}
	ThermalModelParameters GetThermalModelParameters() {
        return ThermalModelParameters(T.value, muB.value, muS.value, muQ.value, gammaS.value, V.value, R.value);
	}
};

struct ExperimentMultiplicity {
    int fPDGID;
    double fValue, fError;
    ExperimentMultiplicity(int PDGID=-211, double value=300., double error=20.):
        fPDGID(PDGID), fValue(value), fError(error) { }
    void addSystematicError(double percent) {
        fError = sqrt(fError*fError + percent*percent*fValue*fValue);
    }
};

struct ExperimentRatio {
    int PDGID1, PDGID2;
	double value, error;
	ExperimentRatio(int PDGID1_ = 211, int PDGID2_ = -211, double value_ = 1., double error_ = 0.1):
		PDGID1(PDGID1_), PDGID2(PDGID2_), value(value_), error(error_) { }
	ExperimentRatio(int PDGID1_, int PDGID2_, double value1, double error1, double value2, double error2):
		PDGID1(PDGID1_), PDGID2(PDGID2_),
		value(value1 / value2),
		error(sqrt(error1*error1 / value2 / value2 + value1 * value1 / value2  / value2  / value2  / value2  * error2 * error2))
		{ 
			// value = value1 / value2;
			// error = sqrt(error1*error1 / value2 / value2 + value1 * value1 / value2  / value2  / value2  / value2  * error2 * error2);
			//error += value * 0.05;
		}
    void addSystematicError(double percent) {
        error = sqrt(error*error + percent*percent*value*value);
    }
};

struct FittedQuantity {
    int type; // 0 - Multiplicity, 1 - Ratio
    ExperimentMultiplicity mult;
    ExperimentRatio ratio;
    FittedQuantity():
		type(0),
		mult(ExperimentMultiplicity(-211, 10., 1.)),
		ratio()
	{
        // type = 0;
        // mult = ExperimentMultiplicity(-211, 10., 1.);
    }
    FittedQuantity(const ExperimentMultiplicity & op):
		type(0), mult(op), ratio()
	{
        // type = 0;
        // mult = op;
    }
    FittedQuantity(const ExperimentRatio & op):
		type(1), mult(), ratio(op)
	{
        // type = 1;
        // ratio = op;
    }
};

class ThermalModelBase;

class ThermalModelFit
{
public:
	ThermalModelBase *model;
	ThermalModelFitParameters Parameters;
    ThermalModelFitParametersExtended ExtendedParameters;
	double QBgoal;
    std::vector<ExperimentMultiplicity> Multiplicities;
	std::vector<ExperimentRatio> Ratios;
    ThermalModelFit(ThermalModelBase *model_, double T=0.12, double muB=0.5, double muS=0., double muQ=0., double gammaS=1., double V=4000., double R=1.):
            model(model_), Parameters(T, muB, muS, muQ, gammaS, V, R),
			ExtendedParameters(),
			QBgoal(0.4),
			Multiplicities(),
			Ratios()
    {
        Multiplicities.resize(0);
        Ratios.resize(0);
        model->SetParameters(Parameters.GetThermalModelParameters());
    }
	~ThermalModelFit(void);
	void SetFitFlag(const std::string& name, bool flag) {
		Parameters.SetParameterFitFlag(name, flag);
        QBgoal = 0.4;
	}
	void SetQBConstraint(double QB) {
		QBgoal = QB;
	}
	void SetRatios(const std::vector<ExperimentRatio> & Ratios_) {
		Ratios = Ratios_;
	}
	void AddRatios(const std::vector<ExperimentRatio> & Ratios_) {
		Ratios.insert(Ratios.end(), Ratios_.begin(), Ratios_.end());
	}
	void AddRatio(const ExperimentRatio& Ratio_) {
		Ratios.push_back(Ratio_);
	}
	void ClearRatios() { Ratios.resize(0); }
	void PrintRatios();

    void SetMultiplicities(const std::vector<ExperimentMultiplicity> & Multiplicities_) {
        Multiplicities = Multiplicities_;
    }
    void AddMultiplicities(const std::vector<ExperimentMultiplicity> & Multiplicities_) {
        Multiplicities.insert(Multiplicities.end(), Multiplicities_.begin(), Multiplicities_.end());
    }
    void AddMultiplicity(const ExperimentMultiplicity& Multiplicity_) {
        Multiplicities.push_back(Multiplicity_);
    }
    void ClearMultiplicities() { Multiplicities.resize(0); }
    void PrintMultiplicities();
    double chi2Ndf(double T, double muB);

	ThermalModelFitParameters PerformFit();
	
	ThermalModelFit(const ThermalModelFit&);
    ThermalModelFit& operator=(const ThermalModelFit&);
};

std::vector<FittedQuantity> loadExpDataFromFile(const std::string & filename);

#endif
