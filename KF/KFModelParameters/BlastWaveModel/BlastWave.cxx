#include "BlastWave.h"
#include <iostream>

namespace BlastWaveNamespace {

#include "../NumericalIntegration.h"

}

BlastWave::BlastWave(double mass, int PDGID, bool UseAcc, double ymin, double ymax, double ycm, double etam):
		xlag32(), wlag32(), xleg32(), wleg32(), xlegeta(), wlegeta(),
		fMass(mass), fPDGID(PDGID), fUseAcceptance(UseAcc), fYmin(ymin), fYmax(ymax), fYcm(ycm), fEtaMax(etam),
		fNorm(1.), fAcceptance(), fReconstructionEfficiency(), fTamt(), fNormT(), fNormT4pi()
{
	if (fabs(fEtaMax)<1e-5) fEtaMax = 1.e-5;
	BlastWaveNamespace::GetCoefsIntegrateLegendre32(0., 3., xlag32, wlag32);
	BlastWaveNamespace::GetCoefsIntegrateLegendre32(fYmin, fYmax, xleg32, wleg32);
	BlastWaveNamespace::GetCoefsIntegrateLegendre32(-fEtaMax, fEtaMax, xlegeta, wlegeta);
	//if (fUseAcceptance) 
	{
	    TString work = getenv("VMCWORKDIR");
		TString dir  = work + "/KF/KFModelParameters/common/";//ThermalModel.mT-t.sts_v13.txt";
		char spdg[300];
		sprintf(spdg, "%d", fPDGID);
		if (!Acceptance::ReadAcceptanceFunction(fAcceptance, dir + "pty_acc_" + spdg + ".txt")) {
			fUseAcceptance = false;
			return;
		}
		fReconstructionEfficiency = Acceptance::ReconstructionEfficiencyFunction(0.99, 0.98, 0.135);
		std::vector<double> Tvec(0), mtacc(0), norm(0), norm4pi(0);
		double dT = 0.002;
		double Tmax = 0.5;
		for(int i=0;;++i) {
		  double tmpT = 0.01 + dT*(0.5+i);
		  if (tmpT>Tmax) break;
		  Tvec.push_back(tmpT);
		  mtacc.push_back(mtAv(tmpT));
		  norm.push_back(Normalization(tmpT));
		  norm4pi.push_back(Normalization4pi(tmpT));
		}
		fTamt = TSpline3("fTamtacc", &mtacc[0], &Tvec[0], Tvec.size());
		fNormT = TSpline3("fNormacc", &Tvec[0], &norm[0], Tvec.size());
		fNormT4pi = TSpline3("fNorm4pi", &Tvec[0], &norm4pi[0], Tvec.size());
	}
}

double BlastWave::mtAv(double T) {
		double ret1 = 0., ret2 = 0.;
		for(Int_t ie = 0 ; ie < 32 ; ie++){
			for(Int_t i = 0 ; i < 32 ; i++){
				for(Int_t j = 0 ; j < 32 ; j++){
					double tmpf = 0.;
					double ty = xleg32[j] - xlegeta[ie];
					double tp = sqrt(xlag32[i]*xlag32[i] + fMass*fMass) * TMath::CosH(xleg32[j] + fYcm);
					tp = sqrt(tp*tp - fMass*fMass);
					double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
					tmpf = wlegeta[ie] * wlag32[i] * wleg32[j] * xlag32[i] * tmt * TMath::CosH(ty) * TMath::Exp(-tmt * TMath::CosH(ty) / T);
					if (fUseAcceptance)	tmpf *= fAcceptance.getAcceptance(xleg32[j] + fYcm, xlag32[i]) * fReconstructionEfficiency.f(tp);
					ret1 += tmpf;
					ret2 += tmpf * tmt;
				}
			}
		}
	    return ret2 / ret1;
}

double BlastWave::mtAv2(double T) {
		double ret = 0.;
		for(Int_t i = 0 ; i < 32 ; i++){
			double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
			ret += wlag32[i] * xlag32[i] * fmt(tmt, T);
		}
		return ret;
}

double BlastWave::fmt(double mt, double T) {
		double ret = 0.;
		double tpt = sqrt(mt*mt - fMass*fMass);
		for(Int_t ie = 0 ; ie < 32 ; ie++){
			for(Int_t j = 0 ; j < 32 ; j++){
				double tmpf = 0.;
				double tp = mt * TMath::CosH(xleg32[j] + fYcm);
				double ty = xleg32[j] - xlegeta[ie];
				tp = sqrt(tp*tp - fMass*fMass);
				tmpf = wlegeta[ie] * wleg32[j] * tpt * mt * TMath::CosH(ty) * TMath::Exp(-mt * TMath::CosH(ty) / T);
				if (fUseAcceptance)	tmpf *= fAcceptance.getAcceptance(xleg32[j] + fYcm, tpt) * fReconstructionEfficiency.f(tp);
				ret += tmpf;
			}
		}
		return ret * mt / tpt / fNormT.Eval(T);
}

double BlastWave::Normalization(double T) {
	double ret = 0.;
	{
		for(Int_t ie = 0 ; ie < 32 ; ie++){
			for(Int_t i = 0 ; i < 32 ; i++){
				for(Int_t j = 0 ; j < 32 ; j++){
					double tmpf = 0.;
					double ty = xleg32[j] - xlegeta[ie];
					double tp = sqrt(xlag32[i]*xlag32[i] + fMass*fMass) * TMath::CosH(xleg32[j] + fYcm);
					tp = sqrt(tp*tp - fMass*fMass);
					double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
					tmpf = wlegeta[ie] * wlag32[i] * wleg32[j] * xlag32[i] * tmt * TMath::CosH(ty) * TMath::Exp(-tmt * TMath::CosH(ty) / T);
					if (fUseAcceptance)	tmpf *= fAcceptance.getAcceptance(xleg32[j] + fYcm, xlag32[i]) * fReconstructionEfficiency.f(tp);
					ret += tmpf;
				}
			}
		}
	}
	return ret;
}

double BlastWave::Normalization4pi(double T) {
	double ret = 0.;
	{
		for(Int_t ie = 0 ; ie < 32 ; ie++){
			for(Int_t i = 0 ; i < 32 ; i++){
				for(Int_t j = 0 ; j < 32 ; j++){
					double tmpf = 0.;
					double ty = xleg32[j] - xlegeta[ie];
					double tp = sqrt(xlag32[i]*xlag32[i] + fMass*fMass) * TMath::CosH(xleg32[j] + fYcm);
					tp = sqrt(tp*tp - fMass*fMass);
					double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
					tmpf = wlegeta[ie] * wlag32[i] * wleg32[j] * xlag32[i] * tmt * TMath::CosH(ty) * TMath::Exp(-tmt * TMath::CosH(ty) / T);
					ret += tmpf;
				}
			}
		}
	}
	return ret;
}