/*
 *====================================================================
 *
 *  Class for 2D spline function used by CbmModels package
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  2D spline class
 *
 *====================================================================
 */

#ifndef BILINEARSPLINEFUNCTION_H
#define BILINEARSPLINEFUNCTION_H
#include "CbmSplineFunction.h"

class BilinearSplineFunction
{
	vector<double> xs;
	vector<SplineFunction> xspls;
public:
	BilinearSplineFunction(void):xs(), xspls() { xs.resize(0); xspls.resize(0); }
	BilinearSplineFunction(const vector<double> & x, const vector<double> & y, const vector<double> & vals):xs(), xspls()
    {
		setData(x, y, vals);
    }
	void setData(const vector<double> & x, const vector<double> & y, const vector<double> & vals) {
		if (x.size()>0) {
			xs.resize(0);
			xspls.resize(0);
			double cx = -1e50;
			for(unsigned int i=0;i<x.size();++i) {
				if (fabs(x[i]-cx)>1e-6) {
					xspls.push_back(SplineFunction());
					xs.push_back(x[i]);
					xspls[xspls.size()-1].add_val(y[i], vals[i]);
					cx = x[i];
				}
				else {
					xspls[xspls.size()-1].add_val(y[i], vals[i]);
				}
			}
		}
	}
	double Eval(double x, double y) const {
		if (xs.size()<2) return -1.;
		unsigned int indx = 0;
		vector< double >::const_iterator it = lower_bound(xs.begin(), xs.end(), x);
        indx = distance(xs.begin(), it);
		int ind1 = 0, ind2 = 0;
		if (indx==0) {
			ind1 = 0;
			ind2 = 1;
		}
		else if (indx==xs.size()) {
			ind1 = indx - 2;
			ind2 = indx - 1;
		}
		else {
			ind1 = indx - 1;
			ind2 = indx;
		}
		double f1v = xspls[ind1].f(y);
		double f2v = xspls[ind2].f(y);
		return f1v + (x - xs[ind1]) * (f2v - f1v) / (xs[ind2] - xs[ind1]);
	}
	~BilinearSplineFunction(void) { }
};

#endif
