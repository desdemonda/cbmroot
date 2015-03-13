#ifndef CBM_LMVM_HIST_H
#define CBM_LMVM_HIST_H

#include <vector>
#include <string>

using namespace std;

/*
 * \brief Enumeration for different sources.
 */
enum CbmLmvmSourceTypes {
  kSignal = 0,
  kBg = 1,
  kPi0 = 2,
  kGamma = 3
};

/*
 * \brief Enumeration for analysis steps.
 */
enum CbmLmvmAnalysisSteps {
  kMc = 0,
  kAcc = 1,
  kReco = 2,
  kChi2Prim = 3,
  kElId = 4,
  kGammaCut = 5,
  kMvd1Cut = 6,
  kMvd2Cut = 7,
  kStCut = 8,
  kTtCut = 9,
  kPtCut = 10
};

/*
 * \brief Enumeration for different sources of BG pairs
 */
enum CbmLmvmBgPairSource {
  kGG = 0, // gamma-gamma
  kPP = 1, // pi0-pi0
  kOO = 2, // other-other
  kGP = 3, // gamma-pi0
  kGO = 4, // gamma-other
  kPO = 5 // pi0-other
};

class CbmLmvmHist
{
public:

   const static int fNofSourceTypes = 4;
   const static vector<string> fSourceTypes;
   const static vector<string> fSourceTypesLatex;
   const static vector<int> fSourceTypesColor;

   const static int fNofAnaSteps = 11;
   const static vector<string> fAnaSteps;
   const static vector<string> fAnaStepsLatex;
   const static vector<int> fAnaStepsColor;

   const static vector<string> fSignalNames;

   const static int fNofBgPairSources = 6;
   const static vector<string> fBgPairSourceLatex;
};

#endif

