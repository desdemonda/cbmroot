#ifndef CBM_ANA_JPSI_HIST_H
#define CBM_ANA_JPSI_HIST_H

#include <vector>
#include <string>

using namespace std;

/*
 * \brief Enumeration for different sources.
 */
enum CbmAnaJpsiSourceTypes {
  kJpsiSignal = 0,
  kJpsiBg = 1,
  kJpsiPi0 = 2,
  kJpsiGamma = 3
};

/*
 * \brief Enumeration for analysis steps.
 */
enum CbmAnaJpsiAnalysisSteps {
  kJpsiMc = 0,
  kJpsiAcc = 1,
  kJpsiReco = 2,
  kJpsiChi2Prim = 3,
  kJpsiElId = 4,
  kJpsiPtCut = 5
};

class CbmAnaJpsiHist
{
public:

   const static int fNofSourceTypes = 4;
   const static vector<string> fSourceTypes;
   const static vector<string> fSourceTypesLatex;
   const static vector<int> fSourceTypesColor;

   const static int fNofAnaSteps = 6;
   const static vector<string> fAnaSteps;
   const static vector<string> fAnaStepsLatex;
   const static vector<int> fAnaStepsColor;

};

#endif

