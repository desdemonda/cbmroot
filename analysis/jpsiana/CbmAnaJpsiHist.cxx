
#include "CbmAnaJpsiHist.h"

#include "TColor.h"
#include <boost/assign/list_of.hpp>

using boost::assign::list_of;

const vector<string> CbmAnaJpsiHist::fSourceTypes =
      list_of("signal")("bg")("pi0")("gamma");

const vector<string> CbmAnaJpsiHist::fSourceTypesLatex =
      list_of("S")("BG")("#pi^{0}")("#gamma");

const vector<int> CbmAnaJpsiHist::fSourceTypesColor =
      list_of(kRed)(kBlue)(kGreen)(kOrange);

const vector<string> CbmAnaJpsiHist::fAnaSteps =
      list_of("mc")("acc")("reco")("chi2prim")("elid")("PtCut");

const vector<string> CbmAnaJpsiHist::fAnaStepsLatex =
      list_of("MC")("ACC")("REC")("#chi^{2}_{prim}")("ID")("p_{t}");

const vector<int> CbmAnaJpsiHist::fAnaStepsColor =
      list_of(kGreen+3)(kOrange+3)(kBlack)(kOrange+7)(kRed)(kPink-6);


