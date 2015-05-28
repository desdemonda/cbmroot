#include "ThermalModelEVMF.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include "xMath.h"

using namespace std;

namespace ThermalModelEVMFNamespace {
	ThermalModelEVMF *gThM;

	void broyden22(vector<double> &xin, vector<double> (*func)(const vector<double>&)) {
		const double TOLF=1.0e-8, EPS=1.0e-8;
		const int MAXITS=200;
		vector<double> h = xin, xh1 = xin, xh2 = xin;
		for(unsigned int i=0;i<xin.size();++i) { 
			h[i] = EPS*fabs(h[i]); 
			if (h[i]==0.0) h[i] = EPS;
			if (i==0) xh1[i] = xin[i] + h[i]; 
			else xh2[i] = xin[i] + h[i]; 
		}
		vector<double> r1 = func(xin), r21 = func(xh1), r22 = func(xh2);
		if (fabs(r1[0])<TOLF && fabs(r1[1])<TOLF) return;
		double J[2][2];
		J[0][0] = (r21[0]-r1[0]) / h[0];
		J[0][1] = (r22[0]-r1[0]) / h[1];
		J[1][0] = (r21[1]-r1[1]) / h[0];
		J[1][1] = (r22[1]-r1[1]) / h[1];
		double Jinv[2][2];
		double det = J[0][0]*J[1][1] - J[0][1]*J[1][0];
		if (det==0.0) throw("singular Jacobian in broyden22");
		Jinv[0][0] = J[1][1] / det;
		Jinv[0][1] = -J[0][1] / det;
		Jinv[1][0] = -J[1][0] / det;
		Jinv[1][1] = J[0][0] / det;
		vector<double> xold = xin;
		vector<double> rold = r1;
		vector<double> rprevten = r1;
		for(int iter=1;iter<=MAXITS;++iter) {
			xin[0] = xold[0] - Jinv[0][0]*rold[0] - Jinv[0][1]*rold[1];
			xin[1] = xold[1] - Jinv[1][0]*rold[0] - Jinv[1][1]*rold[1];
			r1 = func(xin);
			if (fabs(r1[0])<TOLF && fabs(r1[1])<TOLF) return;
			if (iter%10==0) {
				if (fabs(r1[0]/rprevten[0])>1e-2 && fabs(r1[1]/rprevten[1])>1e-2) throw("broyden is stuck");
				rprevten = r1;
			}
			double JF[2];
			double DF[2];
			double dx[2];
			DF[0] = (r1[0]-rold[0]);
			DF[1] = (r1[1]-rold[1]);
			JF[0] = Jinv[0][0]*DF[0] + Jinv[0][1]*DF[1];
			JF[1] = Jinv[1][0]*DF[0] + Jinv[1][1]*DF[1];
			dx[0] = xin[0] - xold[0];
			dx[1] = xin[1] - xold[1];
			double znam = dx[0]*JF[0] + dx[1]*JF[1];
			if (znam==0.0) throw("singular Jacobian in broyden22");
			double xJ[2];
			JF[0] = dx[0] - JF[0];
			JF[1] = dx[1] - JF[1];
			xJ[0] = dx[0]*Jinv[0][0] + dx[1]*Jinv[1][0];
			xJ[1] = dx[0]*Jinv[0][1] + dx[1]*Jinv[1][1];
			Jinv[0][0] = Jinv[0][0] + JF[0]*xJ[0] / znam;
			Jinv[0][1] = Jinv[0][1] + JF[0]*xJ[1] / znam;
			Jinv[1][0] = Jinv[1][0] + JF[1]*xJ[0] / znam;
			Jinv[1][1] = Jinv[1][1] + JF[1]*xJ[1] / znam;
			xold = xin;
			rold = r1;
		}
		throw("exceed MAXITS in broyden");
	}

	vector<double> function22(const vector<double> &xin) {
		vector<double> ret(2);
		gThM->Parameters.muQ = xin[0];
		gThM->Parameters.muS = xin[1];
		gThM->CalculateDensities();
		double fBd = gThM->CalculateBaryonDensity();
		double fCd = gThM->CalculateChargeDensity();
		double fSd = gThM->CalculateStrangenessDensity();
		double fASd = gThM->CalculateAbsoluteStrangenessDensity();
		ret[0] = (fCd/fBd-gThM->QBgoal)/gThM->QBgoal;
		ret[1] = fSd/fASd;
		return ret;
	}
};

using namespace ThermalModelEVMFNamespace;


ThermalModelEVMF::~ThermalModelEVMF(void)
{
}

void ThermalModelEVMF::FixParameters() {
	if (fabs(Parameters.muB)<1e-6) {
		Parameters.muS = Parameters.muQ = 0.;
		return;
	}
	vector<double> x22(2);
	x22[0] = Parameters.muQ;
	x22[1] = Parameters.muS;
	vector<double> x2(2), xinit(2);
	xinit[0] = x2[0] = Parameters.muQ;
    xinit[1] = x2[1] = Parameters.muS;
	ThermalModelEVMFNamespace::gThM = this;
	int iter = 0, iterMAX = 10;
	while (iter<iterMAX) {
		try{
			broyden22(x22, ThermalModelEVMFNamespace::function22);
		}
		catch (...) {
		}
		if (fabs(Parameters.muB/Parameters.muS)>1. && fabs(Parameters.muB/Parameters.muQ)>1.) break;
		xinit[0] = x2[0] = xinit[0] / 10.;
		xinit[1] = x2[1] = xinit[1] / 10.;
		iter++;
	}
	ThermalModelEVMFNamespace::gThM = NULL;
}


void ThermalModelEVMF::FixParameters(double QB) {
	QBgoal = QB;
	FixParameters();
}

double ThermalModelEVMF::Density(double n) {
	double ret = 0.;

        for(unsigned int i=0;i<TPS->fParticles.size();++i) {
            double vo = 0.;
            vo = 4. * 4. * xMath::Pi() / 3. * RHad * RHad * RHad;
            double dMu = -UVdW(n, Parameters.T, vo);
            ret += TPS->fParticles[i].CalculateDensity(Parameters.T, Parameters.muB, Parameters.muS, Parameters.muQ, Parameters.gammaS, 0, fUseWidth, dMu);
        }

	if (fUseHagedorn) {
		double vo = 0.;
        vo = 4. * 4. * xMath::Pi() / 3. * RHad * RHad * RHad;
        double dMu = -UVdW(n, Parameters.T, vo);
		ret += fHag.CalculateDensity(Parameters.T, Parameters.muB, 0, dMu);
	}
	return ret;
}

void ThermalModelEVMF::CalculateDensities() {
	fDensity = 0.;
	{
        double vo = 4. * 4. * xMath::Pi() / 3. * RHad * RHad * RHad;
		if (fDensity*vo>1.) fDensity = 1. / vo / 2.;
		const double TOLF=1.0e-8, EPS=1.0e-8;
		const int MAXITS=200;
		double x = fDensity;
		double h = x;
		h = EPS*fabs(h); 
		if (h==0.0) h = EPS;
		double xh = x + h;
		double r1 = x - Density(x);
		double r2 = xh - Density(xh);
		double Jinv = h / (r1 - r2);
		double xold = x, rold = r1;
		for(int iter=1;iter<=MAXITS;++iter) {
			x = xold - Jinv*rold;
			if (fMode==0 && x*vo>1.) x = 1./vo * (rand()/static_cast<double>(RAND_MAX));
			if (fMode==1 && x*vo>0.5) x = 0.5 / vo * (rand()/static_cast<double>(RAND_MAX));
			r1 = x - Density(x);
			Jinv = (x - xold) / (r1 - rold);
			if (fabs(r1)<TOLF) break;
			xold = x;
			rold = r1;
		}
		fDensity = x;
	}
        for(unsigned int i=0;i<TPS->fParticles.size();++i) {
            double vo = 0.;
            vo = 4. * 4. * xMath::Pi() / 3. * RHad * RHad * RHad;
            double dMu = -UVdW(fDensity, Parameters.T, vo);
            densitiesid[i] = TPS->fParticles[i].CalculateDensity(Parameters.T, Parameters.muB, Parameters.muS, Parameters.muQ, Parameters.gammaS, 0, fUseWidth, dMu);
        }

	if (fUseHagedorn) {
		double vo = 0.;
        vo = 4. * 4. * xMath::Pi() / 3. * RHad * RHad * RHad;
        double dMu = -UVdW(fDensity, Parameters.T, vo);
		fHagedornDensity = fHag.CalculateDensity(Parameters.T, Parameters.muB, 0, dMu);
	}

	for(unsigned int i=0;i<TPS->fParticles.size();++i) densities[i] = densitiesid[i];// / fSuppression;
    for(int i=0;i<static_cast<int>(TPS->fParticles.size());++i) {
        {
            densitiestotal[i] = densities[i];
            for(int j=0;j<static_cast<int>(TPS->fParticles[i].ResonanceBR.size());++j)
                if (i!=TPS->fParticles[i].ResonanceBR[j].second) densitiestotal[i] += TPS->fParticles[i].ResonanceBR[j].first * densities[TPS->fParticles[i].ResonanceBR[j].second];
        }
    }
	fCalculated = true;
}

double ThermalModelEVMF::CalculateShearViscosity() {
    if (!fCalculated) CalculateDensities();
    double ret = 0.;
    double TotalDensity = 0.;
    for(unsigned int i=0;i<TPS->fParticles.size();++i) {
        TotalDensity += densities[i];
        ret += sqrt(TPS->fParticles[i].fMass) * xMath::BesselK(5./2., TPS->fParticles[i].fMass/Parameters.T) / xMath::BesselK(2., TPS->fParticles[i].fMass/Parameters.T) * densities[i];
    }

    return ret * 5. * sqrt(Parameters.T) / 64. / RHad / RHad / sqrt(xMath::Pi()) * 5.06 / TotalDensity;
}
