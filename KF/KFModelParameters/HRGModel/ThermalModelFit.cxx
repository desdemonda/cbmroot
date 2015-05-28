#include "ThermalModelFit.h"
#include "ThermalModelBase.h"

#include "Minuit2/FCNBase.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MnHesse.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/SimplexMinimizer.h"

//#include "DebugText.h"
#include <fstream>

//#include <QDebug>


using namespace ROOT::Minuit2;

int migraditer = 0;

namespace ThermalModelFitNamespace {
	class RatiosFCN : public FCNBase {

	public:

	  RatiosFCN(ThermalModelFit *thmfit_):THMFit(thmfit_), iter(0) { 
	  }

	  ~RatiosFCN() {}

	  double operator()(const std::vector<double>& par) const {
		//std::cout << ++iter << "\n";
		++migraditer;
		double chi2 = 0.;
        THMFit->model->Parameters.T = par[0];
        THMFit->model->Parameters.muB = par[1];
        THMFit->model->Parameters.gammaS = par[2];
        THMFit->model->Parameters.V = par[3];
        THMFit->model->Parameters.muQ = -par[1] / 50.;
        THMFit->model->Parameters.muS = par[1] / 5.;
        THMFit->model->SetQBgoal(THMFit->QBgoal);
        THMFit->model->FixParameters();
        THMFit->Parameters.muQ.value = THMFit->model->Parameters.muQ;
        THMFit->Parameters.muS.value = THMFit->model->Parameters.muS;
        THMFit->model->CalculateDensities();
		for(unsigned int i=0;i<THMFit->Ratios.size();++i) {
            int id1 = THMFit->model->TPS->PDGtoID[THMFit->Ratios[i].PDGID1];
            int id2 = THMFit->model->TPS->PDGtoID[THMFit->Ratios[i].PDGID2];
            double ModelRatio = THMFit->model->densitiestotal[id1] / THMFit->model->densitiestotal[id2];
			chi2 += (ModelRatio - THMFit->Ratios[i].value) * (ModelRatio - THMFit->Ratios[i].value) / THMFit->Ratios[i].error / THMFit->Ratios[i].error;
		}
        for(unsigned int i=0;i<THMFit->Multiplicities.size();++i) {
            int id1 = THMFit->model->TPS->PDGtoID[THMFit->Multiplicities[i].fPDGID];
            double ModelMult = THMFit->model->densitiestotal[id1] * THMFit->model->Parameters.V;
            chi2 += (ModelMult - THMFit->Multiplicities[i].fValue) * (ModelMult - THMFit->Multiplicities[i].fValue) / THMFit->Multiplicities[i].fError / THMFit->Multiplicities[i].fError;
        }
		return chi2;
	  }

	  double Up() const {return 1.;}
	  
	  RatiosFCN(const RatiosFCN&);
      RatiosFCN& operator=(const RatiosFCN&);

	private:
	  ThermalModelFit *THMFit;
	  int iter;
	};
}

using namespace ThermalModelFitNamespace;

ThermalModelFit::~ThermalModelFit(void)
{
}

ThermalModelFitParameters ThermalModelFit::PerformFit() {
    RatiosFCN mfunc(this);
    std::vector<double> params(4, 0.);
	params[0] = Parameters.T.value;
	params[1] = Parameters.muB.value;
	params[2] = Parameters.gammaS.value;
    params[3] = Parameters.V.value;
	MnUserParameters upar;
	upar.Add("T", Parameters.T.value, Parameters.T.error, Parameters.T.xmin, Parameters.T.xmax);
	upar.Add("muB", Parameters.muB.value, Parameters.muB.error, Parameters.muB.xmin, Parameters.muB.xmax);
	upar.Add("gammaS", Parameters.gammaS.value, Parameters.gammaS.error, Parameters.gammaS.xmin, Parameters.gammaS.xmax);
    upar.Add("V", Parameters.V.value, Parameters.V.error, Parameters.V.xmin, Parameters.V.xmax);
    int nparams = 4;
    if (!Parameters.gammaS.toFit) { upar.Fix("gammaS"); nparams--; }
    if (!Parameters.V.toFit || Multiplicities.size()==0) { upar.Fix("V"); nparams--; }

    bool repeat = 0;
    if (model->fUseWidth) {
        repeat = 1;
        model->SetUseWidth(false);
    }

	MnMigrad migrad(mfunc, upar);
	FunctionMinimum min = migrad();
    MnHesse hess;
    hess(mfunc, min);
	ThermalModelFitParameters ret = Parameters;
	ret.T.value = (min.UserParameters()).Params()[0];
	ret.T.error = (min.UserParameters()).Errors()[0];
	ret.muB.value = (min.UserParameters()).Params()[1];
	ret.muB.error = (min.UserParameters()).Errors()[1];
	ret.gammaS.value = (min.UserParameters()).Params()[2];
	ret.gammaS.error = (min.UserParameters()).Errors()[2];
    ret.V.value = (min.UserParameters()).Params()[3];
    ret.V.error = (min.UserParameters()).Errors()[3];

    if (repeat) {
        model->SetUseWidth(true);

        upar.SetValue("T", (min.UserParameters()).Params()[0]);
        upar.SetValue("muB", (min.UserParameters()).Params()[1]);
        upar.SetValue("gammaS", (min.UserParameters()).Params()[2]);
        upar.SetValue("V", (min.UserParameters()).Params()[3]);

        MnMigrad migradd(mfunc, upar);
        min = migradd();

        ret = Parameters;
        ret.T.value = (min.UserParameters()).Params()[0];
        ret.T.error = (min.UserParameters()).Errors()[0];
        ret.muB.value = (min.UserParameters()).Params()[1];
        ret.muB.error = (min.UserParameters()).Errors()[1];
        ret.gammaS.value = (min.UserParameters()).Params()[2];
        ret.gammaS.error = (min.UserParameters()).Errors()[2];
        ret.V.value = (min.UserParameters()).Params()[3];
        ret.V.error = (min.UserParameters()).Errors()[3];
    }

    model->SetParameters(ret.GetThermalModelParameters());
    model->SetQBgoal(QBgoal);
    model->FixParameters();
    ret.muQ.value = model->Parameters.muQ;
    ret.muS.value = model->Parameters.muS;
	Parameters = ret;
	params[0] = ret.T.value;
	params[1] = ret.muB.value;
	params[2] = ret.gammaS.value;
    params[3] = ret.V.value;
	std::cout << "T= " << params[0]*1000. << " MeV " << "muB= " << params[1]*1000. << " MeV " << " gammaS = " << params[2] <<
                 " V = " << params[3] << " fm^3" <<
                 " chi2/ndf = " << mfunc(params) / (Multiplicities.size() + Ratios.size() - nparams) << std::endl;

    ret.chi2ndf = mfunc(params) / (Multiplicities.size() + Ratios.size() - nparams);

    ExtendedParameters = ThermalModelFitParametersExtended(model);

    ExtendedParameters.T.error = ret.T.error;
    ExtendedParameters.muB.error = ret.muB.error;
    ExtendedParameters.gammaS.error = ret.gammaS.error;
    ExtendedParameters.V.error = ret.V.error;

    double Tinit = model->Parameters.T;
    double muBinit = model->Parameters.muB;

    double dT = 0.0005, dmu = 0.0005;
    model->Parameters.T = Tinit + dT;
    model->Parameters.muB = muBinit;
    model->fCalculated = false;
    model->FixParameters();
    model->CalculateDensities();
    ThermalModelFitParametersExtended ParametersdT = ThermalModelFitParametersExtended(model);
    model->Parameters.T = Tinit;
    model->Parameters.muB = muBinit + dmu;
    model->fCalculated = false;
    model->FixParameters();
    model->CalculateDensities();
    ThermalModelFitParametersExtended ParametersdMu = ThermalModelFitParametersExtended(model);
    model->Parameters.T = Tinit;
    model->Parameters.muB = muBinit;
    model->FixParameters();
    model->CalculateDensities();


    double err1 = (min.UserCovariance()).operator ()(0,0);
    double err2 = (min.UserCovariance()).operator ()(1,1);
    double cov = (min.UserCovariance()).operator ()(0,1);
    double deriv1 = (ParametersdT.muQ.value-ExtendedParameters.muQ.value) / dT;
    double deriv2 = (ParametersdMu.muQ.value-ExtendedParameters.muQ.value) / dmu;
    ExtendedParameters.muQ.error = sqrt(deriv1*deriv1*err1 + deriv2*deriv2*err2 + 2.*deriv1*deriv2*cov);
    deriv1 = (ParametersdT.muS.value-ExtendedParameters.muS.value) / dT;
    deriv2 = (ParametersdMu.muS.value-ExtendedParameters.muS.value) / dmu;
    ExtendedParameters.muS.error = sqrt(deriv1*deriv1*err1 + deriv2*deriv2*err2 + 2.*deriv1*deriv2*cov);
    deriv1 = (ParametersdT.nB.value-ExtendedParameters.nB.value) / dT;
    deriv2 = (ParametersdMu.nB.value-ExtendedParameters.nB.value) / dmu;
    ExtendedParameters.nB.error = sqrt(deriv1*deriv1*err1 + deriv2*deriv2*err2 + 2.*deriv1*deriv2*cov);
    deriv1 = (ParametersdT.rhoB.value-ExtendedParameters.rhoB.value) / dT;
    deriv2 = (ParametersdMu.rhoB.value-ExtendedParameters.rhoB.value) / dmu;
    ExtendedParameters.rhoB.error = sqrt(deriv1*deriv1*err1 + deriv2*deriv2*err2 + 2.*deriv1*deriv2*cov);
    deriv1 = (ParametersdT.rhoQ.value-ExtendedParameters.rhoQ.value) / dT;
    deriv2 = (ParametersdMu.rhoQ.value-ExtendedParameters.rhoQ.value) / dmu;
    ExtendedParameters.rhoQ.error = sqrt(deriv1*deriv1*err1 + deriv2*deriv2*err2 + 2.*deriv1*deriv2*cov);
    deriv1 = (ParametersdT.en.value-ExtendedParameters.en.value) / dT;
    deriv2 = (ParametersdMu.en.value-ExtendedParameters.en.value) / dmu;
    ExtendedParameters.en.error = sqrt(deriv1*deriv1*err1 + deriv2*deriv2*err2 + 2.*deriv1*deriv2*cov);
    deriv1 = (ParametersdT.entropy.value-ExtendedParameters.entropy.value) / dT;
    deriv2 = (ParametersdMu.entropy.value-ExtendedParameters.entropy.value) / dmu;
    ExtendedParameters.entropy.error = sqrt(deriv1*deriv1*err1 + deriv2*deriv2*err2 + 2.*deriv1*deriv2*cov);
    deriv1 = (ParametersdT.pressure.value-ExtendedParameters.pressure.value) / dT;
    deriv2 = (ParametersdMu.pressure.value-ExtendedParameters.pressure.value) / dmu;
    ExtendedParameters.pressure.error = sqrt(deriv1*deriv1*err1 + deriv2*deriv2*err2 + 2.*deriv1*deriv2*cov);
    deriv1 = (ParametersdT.eta.value-ExtendedParameters.eta.value) / dT;
    deriv2 = (ParametersdMu.eta.value-ExtendedParameters.eta.value) / dmu;
    ExtendedParameters.eta.error = sqrt(deriv1*deriv1*err1 + deriv2*deriv2*err2 + 2.*deriv1*deriv2*cov);

	return ret;
}

void ThermalModelFit::PrintRatios() {
    model->SetParameters(Parameters.GetThermalModelParameters());
    model->SetQBgoal(QBgoal);
    model->FixParameters();
    model->CalculateDensities();
	for(unsigned int i=0;i<Ratios.size();++i) {
        int ind1 = model->TPS->PDGtoID[Ratios[i].PDGID1];
        int ind2 = model->TPS->PDGtoID[Ratios[i].PDGID2];
        std::cout << model->TPS->fParticles[ind1].fName << "/" << model->TPS->fParticles[ind2].fName << " = " <<
            model->densitiestotal[ind1] / model->densitiestotal[ind2] << " " << Ratios[i].value << " " << Ratios[i].error << "\n";
	}
}

void ThermalModelFit::PrintMultiplicities() {
    model->SetParameters(Parameters.GetThermalModelParameters());
    model->SetQBgoal(QBgoal);
    model->FixParameters();
    model->CalculateDensities();
    for(unsigned int i=0;i<Multiplicities.size();++i) {
        int ind1 = model->TPS->PDGtoID[Multiplicities[i].fPDGID];
        std::cout << "<" << model->TPS->fParticles[ind1].fName << "> = " <<
            model->densitiestotal[ind1] * model->Parameters.V << " " << Multiplicities[i].fValue << " " << Multiplicities[i].fError << "\n";
    }
}

using namespace std;

std::vector<FittedQuantity> loadExpDataFromFile(const std::string & filename) {
    std::vector<FittedQuantity> ret(0);
    ifstream fin(filename.c_str());
    //fin.open(filename);
    if (fin.is_open()) {
        string tmp;
        char tmpc[500];
        fin.getline(tmpc, 500);
        tmp = string(tmpc);
        while (1) {
            vector<string> fields = split(tmp, ';');
            if (fields.size()<4) break;
            int type = atoi(fields[0].c_str());
            int pdgid1 = atoi(fields[1].c_str()), pdgid2 = 0;
            double value, error;
            if (type) {
                pdgid2 = atoi(fields[2].c_str());
                value = atof(fields[3].c_str());
				error = 1.;
                if (fields.size()>=4) error = atof(fields[4].c_str());
            }
            else {
                value = atof(fields[2].c_str());
                error = atof(fields[3].c_str());
            }

            if (type) ret.push_back(FittedQuantity(ExperimentRatio(pdgid1, pdgid2, value, error)));
            else ret.push_back(FittedQuantity(ExperimentMultiplicity(pdgid1, value, error)));

            fin.getline(tmpc, 500);
            tmp = string(tmpc);
        }
        fin.close();
    }
    return ret;
}

double ThermalModelFit::chi2Ndf(double T, double muB) {
    double Tinit = T, muBinit = muB;
    RatiosFCN mfunc(this);
    int nparams = 4;
    if (!Parameters.gammaS.toFit) nparams--;
    if (!Parameters.V.toFit || Multiplicities.size()==0) nparams--;
    std::vector<double> params(4, 0.);
    params[0] = T;
    params[1] = muB;
    params[2] = model->Parameters.gammaS;
    params[3] = model->Parameters.V;
    model->Parameters.muS = muB / 5.;
    model->Parameters.muQ = -muB / 50.;
    double ret = mfunc(params);
    model->Parameters.T = Tinit;
    model->Parameters.muB = muBinit;
    return ret / (Multiplicities.size() + Ratios.size() - nparams);
}
