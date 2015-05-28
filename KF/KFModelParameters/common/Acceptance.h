#ifndef ACCEPTANCE_H
#define ACCEPTANCE_H

#include "TMath.h"
#include <vector>
#include "../CbmBilinearSplineFunction.h"

namespace Acceptance {

	struct AcceptanceFunction {
	  Double_t dy, dpt;
	  vector<Double_t> ys, pts, probs;
	  BilinearSplineFunction sfunc;
	  void setSpline() { sfunc.setData(ys, pts, probs); }
	  Double_t getAcceptance(const Double_t & y, const Double_t & pt) const;
	  AcceptanceFunction(): dy(), dpt(), ys(), pts(), probs(), sfunc() { }
	};
		
	struct ReconstructionEfficiencyFunction {
	  Double_t p1, p2, p3;
	  ReconstructionEfficiencyFunction(double p1_=1., double p2_=0., double p3_=1.):p1(p1_), p2(p2_), p3(p3_) { }
	  Double_t f(double p) { 
		return p1 - p2 * TMath::Exp(-p*p/2./p3/p3); 
	  }
	};

	int ReadAcceptanceFunction(AcceptanceFunction & func, TString filename);
}

#endif