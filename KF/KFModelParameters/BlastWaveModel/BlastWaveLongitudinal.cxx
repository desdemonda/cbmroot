#include "BlastWaveLongitudinal.h"
#include <iostream>

namespace BlastWaveLongitudinalNamespace {

#include "../NumericalIntegration.h"

}

BlastWaveLongitudinal::BlastWaveLongitudinal(double mass, int PDGID, bool UseAcc, double ymin, double ymax, double ycm, double T):
		xlag32(), wlag32(), xleg32(), wleg32(), xlegeta(), wlegeta(),
		fMass(mass), fPDGID(PDGID), fUseAcceptance(UseAcc), fYmin(ymin), fYmax(ymax), fYcm(ycm), fT(T),
		fNorm(1.), fY2Min(), fY2Max(), fEtaMin(), fEtaMax(), fAcceptance(), fReconstructionEfficiency(), fetaay2(), fNormeta(), fNormeta4pi()
{
	BlastWaveLongitudinalNamespace::GetCoefsIntegrateLegendre32(0., 3., xlag32, wlag32);
	BlastWaveLongitudinalNamespace::GetCoefsIntegrateLegendre32(fYmin, fYmax, xleg32, wleg32);
	{
	    TString work = getenv("VMCWORKDIR");
		TString dir  = work + "/KF/KFModelParameters/common/";
		char spdg[300];
		sprintf(spdg, "%d", fPDGID);
		if (!Acceptance::ReadAcceptanceFunction(fAcceptance, dir + "pty_acc_" + spdg + ".txt")) {
			fUseAcceptance = false;
			return;
		}
		fReconstructionEfficiency = Acceptance::ReconstructionEfficiencyFunction(0.99, 0.98, 0.135);
		std::vector<double> etavec(0), y2acc(0), norm(0), norm4pi(0);
		double deta = 0.01;
		double etamax = ycm;
		for(int i=0;;++i) {
		  double tmpeta = 1.e-5 + deta*(i);
		  if (i==0) { fY2Min = y2Av(tmpeta); fEtaMin = tmpeta; }
		  if (tmpeta>etamax) { fY2Max = y2Av(tmpeta - deta); fEtaMax = tmpeta - deta; break; }
		  etavec.push_back(tmpeta);
		  y2acc.push_back(y2Av(tmpeta));
		  norm.push_back(Normalization(tmpeta));
		  norm4pi.push_back(Normalization4pi(tmpeta));
		  // std::cout << "Calc: " << etavec[i] << " " << y2acc[i] << " " << norm[i] << "\n";
		}
		fetaay2 = TSpline3("fetaay2acc", &y2acc[0], &etavec[0], etavec.size());
		fNormeta = TSpline3("fNormacc", &etavec[0], &norm[0], etavec.size());
		fNormeta4pi = TSpline3("fNorm4pi", &etavec[0], &norm4pi[0], etavec.size());
		for(unsigned int i=0;i<etavec.size();++i) {
		  // std::cout << "ACalc: " << fetaay2.Eval(y2acc[i]) << " " << y2acc[i] << " " << fNormeta.Eval(etavec[i]) << "\n";
		//  std::cout << "Avs: " << mtAv(Tvec[i]) << " " << mtAv2(Tvec[i]) << "\n";
		}
	}
}

double BlastWaveLongitudinal::y2Av(double eta) {
		BlastWaveLongitudinalNamespace::GetCoefsIntegrateLegendre32(-eta, eta, xlegeta, wlegeta);
		double ret1 = 0., ret2 = 0.;
		for(Int_t ie = 0 ; ie < 32 ; ie++){
			for(Int_t i = 0 ; i < 32 ; i++){
				for(Int_t j = 0 ; j < 32 ; j++){
					double tmpf = 0.;
					double ty = xleg32[j] - xlegeta[ie];
					double tp = sqrt(xlag32[i]*xlag32[i] + fMass*fMass) * TMath::CosH(xleg32[j] + fYcm);
					tp = sqrt(tp*tp - fMass*fMass);
					double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
					tmpf = wlegeta[ie] * wlag32[i] * wleg32[j] * xlag32[i] * tmt * TMath::CosH(ty) * TMath::Exp(-tmt * TMath::CosH(ty) / fT);
					if (fUseAcceptance)	tmpf *= fAcceptance.getAcceptance(xleg32[j] + fYcm, xlag32[i]) * fReconstructionEfficiency.f(tp);
					ret1 += tmpf;
					ret2 += tmpf * xleg32[j] * xleg32[j];
				}
			}
		}
	    return ret2 / ret1;
}

double BlastWaveLongitudinal::y2Av2(double eta) {
		double ret1 = 0., ret2 = 0.;
		for(Int_t j = 0 ; j < 32 ; j++) {
			ret1 += wleg32[j] * xleg32[j] * xleg32[j] * fy(xleg32[j], eta);
			ret2 += wleg32[j] * fy(xleg32[j], eta);
			}
		return ret1 / ret2;
}

double BlastWaveLongitudinal::fy(double y, double eta) {
		double ret = 0.;
		BlastWaveLongitudinalNamespace::GetCoefsIntegrateLegendre32(-eta, eta, xlegeta, wlegeta);
		for(Int_t ie = 0 ; ie < 32 ; ie++){
			for(Int_t i = 0 ; i < 32 ; i++){
				double tmpf = 0.;
				double ty = y - xlegeta[ie];
				double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
				double tp = tmt * TMath::CosH(y + fYcm);
				tp = sqrt(tp*tp - fMass*fMass);
				tmpf = wlegeta[ie] * wlag32[i] * xlag32[i] * tmt * TMath::CosH(ty) * TMath::Exp(-tmt * TMath::CosH(ty) / fT);
				if (fUseAcceptance)	tmpf *= fAcceptance.getAcceptance(y + fYcm, xlag32[i]) * fReconstructionEfficiency.f(tp);
				ret += tmpf;
			}
		}
		return ret / fNormeta.Eval(eta);
}

double BlastWaveLongitudinal::Normalization(double eta) {
	double ret = 0.;
	//if (fUseAcceptance) 
	{
		BlastWaveLongitudinalNamespace::GetCoefsIntegrateLegendre32(-eta, eta, xlegeta, wlegeta);
		for(Int_t ie = 0 ; ie < 32 ; ie++){
			for(Int_t i = 0 ; i < 32 ; i++){
				for(Int_t j = 0 ; j < 32 ; j++){
					double tmpf = 0.;
					double ty = xleg32[j] - xlegeta[ie];
					double tp = sqrt(xlag32[i]*xlag32[i] + fMass*fMass) * TMath::CosH(xleg32[j] + fYcm);
					tp = sqrt(tp*tp - fMass*fMass);
					double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
					tmpf = wlegeta[ie] * wlag32[i] * wleg32[j] * xlag32[i] * tmt * TMath::CosH(ty) * TMath::Exp(-tmt * TMath::CosH(ty) / fT);
					if (fUseAcceptance)	tmpf *= fAcceptance.getAcceptance(xleg32[j] + fYcm, xlag32[i]) * fReconstructionEfficiency.f(tp);
					ret += tmpf;
				}
			}
		}
	}
	return ret;
}

double BlastWaveLongitudinal::Normalization4pi(double eta) {
	double ret = 0.;
	//if (fUseAcceptance) 
	{
		BlastWaveLongitudinalNamespace::GetCoefsIntegrateLegendre32(-eta, eta, xlegeta, wlegeta);
		for(Int_t ie = 0 ; ie < 32 ; ie++){
			for(Int_t i = 0 ; i < 32 ; i++){
				for(Int_t j = 0 ; j < 32 ; j++){
					double tmpf = 0.;
					double ty = xleg32[j] - xlegeta[ie];
					double tp = sqrt(xlag32[i]*xlag32[i] + fMass*fMass) * TMath::CosH(xleg32[j] + fYcm);
					tp = sqrt(tp*tp - fMass*fMass);
					double tmt = sqrt(xlag32[i]*xlag32[i] + fMass*fMass);
					tmpf = wlegeta[ie] * wlag32[i] * wleg32[j] * xlag32[i] * tmt * TMath::CosH(ty) * TMath::Exp(-tmt * TMath::CosH(ty) / fT);
					ret += tmpf;
				}
			}
		}
	}
	return ret;
}