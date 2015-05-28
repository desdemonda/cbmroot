#include "ThermalModel.h"

using namespace std;



namespace ThermalModelNamespace {
	ThermalModel *gThM;

    void broyden2(vector<double> &xin, vector<double> (*func)(const vector<double>&, ThermalModel*), ThermalModel *ThM) {
		const double TOLF=1.0e-8, EPS=1.0e-12;
		const int MAXITS=200;
		vector<double> h = xin, xh1 = xin, xh2 = xin;
		for(unsigned int i=0;i<xin.size();++i) { 
			h[i] = EPS*abs(h[i]); 
			if (h[i]==0.0) h[i] = EPS;
			if (i==0) xh1[i] = xin[i] + h[i]; 
			else xh2[i] = xin[i] + h[i]; 
		}
        vector<double> r1 = func(xin, ThM), r21 = func(xh1, ThM), r22 = func(xh2, ThM);
		if (fabs(r1[0])<TOLF && fabs(r1[1])<TOLF) return;
		double J[2][2];
		J[0][0] = (r21[0]-r1[0]) / h[0];
		J[0][1] = (r22[0]-r1[0]) / h[1];
		J[1][0] = (r21[1]-r1[1]) / h[0];
		J[1][1] = (r22[1]-r1[1]) / h[1];
		double Jinv[2][2];
		double det = J[0][0]*J[1][1] - J[0][1]*J[1][0];
		//cout << xin[0] << " " << h[0] << " " << xin[1] << " " << h[1] << "\n";
		//cout << J[0][0] << " " << J[0][1] << " " << J[1][0] << " " << J[1][1] << "\n";
		if (det==0.0) throw("singular Jacobian in broyden22");
		Jinv[0][0] = J[1][1] / det;
		Jinv[0][1] = -J[0][1] / det;
		Jinv[1][0] = -J[1][0] / det;
		Jinv[1][1] = J[0][0] / det;
		vector<double> xold = xin;
		vector<double> rold = r1;
		vector<double> rprevten = r1;
		for(int iter=1;iter<=MAXITS;++iter) {
			//std::cout << iter << " " << xin[0] << " " << xin[1] << " " << r1[0] << " " << r1[1] << "\n";
			xin[0] = xold[0] - Jinv[0][0]*rold[0] - Jinv[0][1]*rold[1];
			xin[1] = xold[1] - Jinv[1][0]*rold[0] - Jinv[1][1]*rold[1];
            r1 = func(xin, ThM);
            if (fabs(r1[0])<TOLF && fabs(r1[1])<TOLF) {
                //dbgstrm << "Broyden iterations = " << iter << endl;
                return;
            }
			//if (iter%10==0) {
			//	if (abs(r1[0]/rprevten[0])>1e-2 && abs(r1[1]/rprevten[1])>1e-2) throw("broyden is stuck");
			//	rprevten = r1;
			//}
			//if (abs(r1[0])>10*abs(rold[0]) || abs(r1[1])>10*abs(rold[1])) throw("broyden doesn't work");
			double JF[2];
			double DF[2];
			double dx[2];
			DF[0] = (r1[0]-rold[0]);
			DF[1] = (r1[1]-rold[1]);
			/*if (abs(DF[0]/rold[0])>1e1 && abs(DF[1]/rold[1])>1e1 && !fl) { 
				xin[0] = 0.;
				xin[1] = 0.;
				broyden22(xin, func);
				return;
			}*/
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
		//std::cout << r1[0] << " " << r1[1] << "\n";
        //dbgstrm << "Broyden iterations = " << MAXITS << endl;
		throw("exceed MAXITS in broyden");
	}


    vector<double> function2(const vector<double> &xin, ThermalModel *ThM) {
		vector<double> ret(2);
        ThM->Parameters.muQ = xin[0];
        ThM->Parameters.muS = xin[1];
        ThM->CalculateDensities();
        double fBd = ThM->CalculateBaryonDensity();
        double fCd = ThM->CalculateChargeDensity();
        double fSd = ThM->CalculateStrangenessDensity();
        double fASd = ThM->CalculateAbsoluteStrangenessDensity();
        ret[0] = (fCd/fBd-ThM->QBgoal)/ThM->QBgoal;
		ret[1] = fSd/fASd;
		return ret;
	}
}

using namespace ThermalModelNamespace;

ThermalModel::~ThermalModel(void)
{
}

void ThermalModel::FixParameters() {
	if (fabs(Parameters.muB)<1e-6) {
		Parameters.muS = Parameters.muQ = 0.;
		return;
	}
    vector<double> x2(2), xinit(2);
    xinit[0] = x2[0] = Parameters.muQ;
    xinit[1] = x2[1] = Parameters.muS;
	ThermalModelNamespace::gThM = this;
	int iter = 0, iterMAX = 10;
	while (iter<iterMAX) {
		try{
			broyden2(x2, ThermalModelNamespace::function2, this);
		}
		catch (...) {
		}
		if (fabs(Parameters.muB/Parameters.muS)>1. && fabs(Parameters.muB/Parameters.muQ)>1.) break;
		xinit[0] = x2[0] = xinit[0] / 10.;
		xinit[1] = x2[1] = xinit[1] / 10.;
		iter++;
	}
	ThermalModelNamespace::gThM = NULL;
}


void ThermalModel::FixParameters(double QB) {
	QBgoal = QB;
	FixParameters();
}
