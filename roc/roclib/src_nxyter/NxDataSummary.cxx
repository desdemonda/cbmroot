//============================================================================
/*! \file NxDataSummary.cxx
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#include <iostream>

#include "boost/format.hpp"

#include "nxyter/NxDataSummary.h"

using std::endl;
using boost::format;

/*!
 * \class nxyter::NxDataSummary
 * \brief Data container for nXYTER data distribution parameters.
 *
 */

//----------------------------------------------------------------------------
//! Constructor with full addressing path information.

nxyter::NxDataSummary::NxDataSummary() :
  fNumEntries(128),
  fFull(128),
  fMedian(128),
  fWidth50(128),
  fWidth80(128),
  fMin(128),
  fMax(128)
{
}

//----------------------------------------------------------------------------

nxyter::NxDataSummary::~NxDataSummary()
{
}

//----------------------------------------------------------------------------
//! Clear all values.

void nxyter::NxDataSummary::clear()
{
  for (int ch=0; ch<128; ch++) {
    fNumEntries[ch] = 0;
    fFull[ch]       = false;
    fMedian[ch]     = 0.;
    fWidth50[ch]    = 0.;
    fWidth80[ch]    = 0.;
    fMin[ch]        = 0.;
    fMax[ch]        = 0.;
  }
}

//----------------------------------------------------------------------------
//! Analyse the DistFuncArray \a dfa and store the distribution parameters.
/*!
 * This method inspects the DistFuncArray \a dfa and determines for each
 * of the 128 channels
 * \li number of entries
 * \li whether the 'full' flag was set
 * \li median value of distribution
 * \li 25-75 width (FWHM)
 * \li 10-90 width
 * \li lowest value found for channel
 * \li highest value found for channel
 *
 * These parameters can be retrieved with the accessors numEntries(), full(),
 * getMedian(), getWidth50(), getWidth80(), getMin(), and getMax().
 */

void nxyter::NxDataSummary::analyse(DistFuncArray& dfa)
{
  for (int ch=0; ch<128; ch++) {
    fNumEntries[ch] = dfa[ch].numEntries();
    fFull[ch]       = dfa[ch].full();
    fMedian[ch]     = dfa[ch].getMedian();
    fWidth50[ch]    = dfa[ch].getWidth(0.25);
    fWidth80[ch]    = dfa[ch].getWidth(0.40);
    fMin[ch]        = dfa[ch].getMin();
    fMax[ch]        = dfa[ch].getMax();
  }
}

//----------------------------------------------------------------------------
//! Print to stream \a os.

void nxyter::NxDataSummary::print(std::ostream& os, int nxnum) const
{
  os << "nx cha   hits   median  w25-75  w10-90   min   max" << endl;
  int chaseen = 0;
  for (int cha=0; cha<128; cha++) {
    int nent = numEntries(cha);
    if (nent == 0) continue;
    chaseen += 1;
    float med = getMedian(cha);
    float w50 = getWidth50(cha);
    float w80 = getWidth80(cha);
    float min = getMin(cha);
    float max = getMax(cha);
    char cpl  = full(cha) ? '+' : ' ';
    static format fmt(" %1d %3d %6d%c  %6.1f  %6.1f  %6.1f  %4.0f  %4.0f");
    os << fmt % nxnum % cha % nent % cpl % med % w50 % w80 % min % max << endl;
  }
  if (chaseen == 0) {
    os << std::dec << nxnum << " ... --- no hits seen for all channels ---" 
       << endl;
  }
}

//----------------------------------------------------------------------------
