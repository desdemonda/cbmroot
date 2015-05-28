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


class CbmAnaJpsiHist
{
public:

   const static int fNofSourceTypes = 4;
   const static vector<string> fSourceTypes;
   const static vector<string> fSourceTypesLatex;
   const static vector<int> fSourceTypesColor;
};

#endif

