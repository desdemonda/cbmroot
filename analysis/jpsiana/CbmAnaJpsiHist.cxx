
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



