#include "BoltzmannDistribution.h"
#include <iostream>

namespace BoltzmannDistributionNamespace {

#include "../NumericalIntegration.h"

}

BoltzmannDistribution::BoltzmannDistribution(double mass, int PDGID, bool UseAcc, double ymin, double ymax, double ycm, double width):
		xlag32(), wlag32(), xleg32(), wleg32(),
		fMass(mass), fPDGID(PDGID), fUseAcceptance(UseAcc), fYmin(ymin), fYmax(ymax), fYcm(ycm), fWidth(width),
		fNorm(1.), fAcceptance(), fReconstructionEfficiency(), fTamt(), fNormT(), fNormT4pi()
{
	//InverseSlopeNamespace::GetCoefsIntegrateLaguerre32(xlag32, wlag32);
	fWidth = 1.;
	BoltzmannDistributionNamespace::GetCoefsIntegrateLegendre32(0., 3., xlag32, wlag32);
	BoltzmannDistributionNamespace::GetCoefsIntegrateLegendre32(fYmin, fYmax, xleg32, wleg32);
	//if (fUseAcceptance) 
	{
	    TString work = getenv("VMCWORKDIR");
		TString dir  = work + "/KF/KFModelParameters/common/";//ThermalModel.mT-t.sts_v13.txt";
		char spdg[300];
		sprintf(spdg, "%d", fPDGID);
		if (!Acceptance::ReadAcceptanceFunction(fAcceptance, dir + "pty_acc_" + spdg + ".txt")) {
			fUseAcceptance = false;
			//std::cout << dir + "pty_acc_" + spdg + ".txt" << "\nAcceptance not OK!\n" << "\n";
			return;
		}
		//std::cout << "Acceptance OK!\n" << "\n";
		fReconstructionEfficiency = Acceptance::ReconstructionEfficiencyFunction(0.99, 0.98, 0.135);
		std::vector<double> Tvec(0), mtacc(0), norm(0), norm4pi(0);
		double dT = 0.002;
		double Tmax = 0.5;
		//std::cout << ymin << "<" << "y" << "<" << ymax << "\n";
		for(int i=0;;++i) {
		  double tmpT = 0.01 + dT*(0.5+i);
		  if (tmpT>Tmax) break;
		  Tvec.push_back(tmpT);
		  mtacc.push_back(mtAv(tmpT));
		  norm.push_back(Normalization(tmpT));
		  norm4pi.push_back(Normalization4pi(tmpT));
		  //std::cout << "Calc: " << Tvec[i] << " " << mtacc[i] << " " << norm[i] << "\n";
		}
		fTamt = TSpline3("fTamtacc", &mtacc[0], &Tvec[0], Tvec.size());
		fNormT = TSpline3("fNormacc", &Tvec[0], &norm[0], Tvec.size());
		fNormT4pi = TSpline3("fNorm4pi", &Tvec[0], &norm4pi[0], Tvec.size());
		//for(int i=0;i<Tvec.size();++i) {
		//  std::cout << "ACalc: " << fTamt.Eval(mtacc[i]) << " " << mtacc[i] << " " << fNormT.Eval(Tvec[i]) << "\n";
		//  std::cout << "Avs: " << mtAv(Tvec[i]) << " " << mtAv2(Tvec[i]) << "\n";
		//}
	}
}

double BoltzmannDistribution::mtAv(double T) {
		double ret1 = 0., ret2 = 0.;
		for(Int_t i = 0 ; i < 32 ; i++){
			for(Int_t j = 0 ; j < 32 ; j++){
				double tmpf = 0.;
				double tp = sqrt(xlag32[i]*xlag32[i] + fMass*fMass) * TMath::CosH(xleg32[j] + fYcm);
				tp = sqrt(tp*tp - fMass*fMass);
				double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
				tmpf = wlag32[i] * wleg32[j] * xlag32[i] * tmt * TMath::CosH(xleg32[j]) * TMath::Exp(-tmt * TMath::CosH(xleg32[j]) / T);
				if (fUseAcceptance)	tmpf *= fAcceptance.getAcceptance(xleg32[j] + fYcm, xlag32[i]) * fReconstructionEfficiency.f(tp);
				ret1 += tmpf;
				ret2 += tmpf * tmt;
			}
		  }
	    return ret2 / ret1;
}

double BoltzmannDistribution::mtAv2(double T) {
		double ret = 0.;
		for(Int_t i = 0 ; i < 32 ; i++){
			double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
			ret += wlag32[i] * xlag32[i] * fmt(tmt, T);
		}
		return ret;
}

double BoltzmannDistribution::fmt(double mt, double T) {
		double ret = 0.;
		double tpt = sqrt(mt*mt - fMass*fMass);
		for(Int_t j = 0 ; j < 32 ; j++){
			double tmpf = 0.;
			double tp = mt * TMath::CosH(xleg32[j] + fYcm);
			tp = sqrt(tp*tp - fMass*fMass);
			tmpf = wleg32[j] * tpt * mt * TMath::CosH(xleg32[j]) * TMath::Exp(-mt * TMath::CosH(xleg32[j]) / T);
			if (fUseAcceptance)	tmpf *= fAcceptance.getAcceptance(xleg32[j] + fYcm, tpt) * fReconstructionEfficiency.f(tp);
			ret += tmpf;
		}
		return ret * mt / tpt / fNormT.Eval(T);
}

double BoltzmannDistribution::Normalization(double T) {
	double ret = 0.;
	//if (fUseAcceptance) 
	{
		for(Int_t i = 0 ; i < 32 ; i++){
			for(Int_t j = 0 ; j < 32 ; j++){
				double tmpf = 0.;
				double tp = sqrt(xlag32[i]*xlag32[i] + fMass*fMass) * TMath::CosH(xleg32[j] + fYcm);
				tp = sqrt(tp*tp - fMass*fMass);
				double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
				tmpf = wlag32[i] * wleg32[j] * xlag32[i] * tmt * TMath::CosH(xleg32[j]) * TMath::Exp(-tmt * TMath::CosH(xleg32[j]) / T);
				if (fUseAcceptance)	tmpf *= fAcceptance.getAcceptance(xleg32[j] + fYcm, xlag32[i]) * fReconstructionEfficiency.f(tp);
				ret += tmpf;
			}
		  }
	}
	return ret;
}

double BoltzmannDistribution::Normalization4pi(double T) {
	double ret = 0.;
	//if (fUseAcceptance) 
	{
		for(Int_t i = 0 ; i < 32 ; i++){
			for(Int_t j = 0 ; j < 32 ; j++){
				double tmpf = 0.;
				double tp = sqrt(xlag32[i]*xlag32[i] + fMass*fMass) * TMath::CosH(xleg32[j] + fYcm);
				tp = sqrt(tp*tp - fMass*fMass);
				double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
				tmpf = wlag32[i] * wleg32[j] * xlag32[i] * tmt * TMath::CosH(xleg32[j]) * TMath::Exp(-tmt * TMath::CosH(xleg32[j]) / T);
				ret += tmpf;
			}
		  }
	}
	return ret;
}

double BoltzmannDistribution::dndy(double y, double A, double T) {
	double ret = 0.;
	for(Int_t i = 0 ; i < 32 ; i++){
		//for(Int_t j = 0 ; j < 32 ; j++){
			double tmpf = 0.;
			double tp = sqrt(xlag32[i]*xlag32[i] + fMass*fMass) * TMath::CosH(y + fYcm);
			tp = sqrt(tp*tp - fMass*fMass);
			double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
			tmpf = wlag32[i] * xlag32[i] * tmt * TMath::CosH(y) * TMath::Exp(-tmt * TMath::CosH(y) / T);
			ret += tmpf;
		//}
	}
	return ret / fNormT4pi.Eval(T) * A;
}