#include "Acceptance.h"
#include "TString.h"
#include <fstream>

int Acceptance::ReadAcceptanceFunction(Acceptance::AcceptanceFunction & func, TString filename)
{
  double ymin = 0., ymax = 6.;
  double ptmin = 0., ptmax = 2.5;
  func.ys.resize(0);
  func.pts.resize(0);
  func.probs.resize(0);
  ifstream fin(filename.Data());
  if (!fin.is_open()) return 0;
  fin >> func.dy >> func.dpt;
  double ty, tpt, prob;
  func.ys.resize(0);
  func.pts.resize(0);
  func.probs.resize(0);
  while (fin >> ty >> tpt >> prob) {
    if (tpt<ptmin || tpt>ptmax || ty<ymin || ty>ymax) continue;
    func.ys.push_back(ty);
    func.pts.push_back(tpt);
    func.probs.push_back(prob);
  }
  func.setSpline();
  fin.close();
  return 1;
}

Double_t Acceptance::AcceptanceFunction::getAcceptance(const Double_t & y, const Double_t & pt) const {
		double ret = sfunc.Eval(y, pt);
		if (ret<0.) ret = 0.;
		if (ret>1.) ret = 1.;
		return ret;
	}