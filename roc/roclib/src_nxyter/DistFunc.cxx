//============================================================================
/*! \file DistFunc.cxx
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#include <cmath>
#include <algorithm>
#include <iostream>

#include "nxyter/DistFunc.h"

/*!
 * \class nxyter::DistFunc
 * \brief Stores a set of values in the form of a distribution function
 *
 */

//----------------------------------------------------------------------------
//! Constructor, reserves an inital capacity of \a cap entries.
/*!
 * \param cap initial capacity of the internal data. If more than \a cap
 *   entries will be added, the capacity will be automatically increased.
 *   \a cap should be specified when the total number of entries is known
 *   at ctor time, this avoids reallocations and improves performance.
 */

nxyter::DistFunc::DistFunc(int cap) :
  fMaxEntries(0),
  fFull(false),
  fSorted(false)
{
  fData.reserve(cap);
}

//----------------------------------------------------------------------------

nxyter::DistFunc::~DistFunc()
{
}

//----------------------------------------------------------------------------
//! Set maximum number of entries accepted to \a max.
/*!
 * When addEntry() is called more than \a max times, the 'full' flag is
 * set and further entries are discarded. Using a \a max of 0 will
 * disable the 'full' flag logic.
 *
 * \param max maximum number of entries
 * \sa full().
 */

void nxyter::DistFunc::setMaxEntries(int max)
{
  fMaxEntries = max;
  fFull = false;
  if (max > 0) fFull = fData.size() > max;
}

//----------------------------------------------------------------------------
//! Adds an entry with value \a val to distribution.
/*!
 * \param val value to be added to the distribution
 */

void nxyter::DistFunc::addEntry(float val)
{
  if (fMaxEntries>0 && fData.size() >= fMaxEntries) fFull = true;
  if (fFull) return;
  fData.push_back(val);
  fSorted = false;
}

//----------------------------------------------------------------------------
//! Returns inverse of distribution function for probability \a prob.

float nxyter::DistFunc::operator()(float prob)
{
  if (fData.size() == 0) return 0.;
  
  sort();

  if (prob < 0.) prob = 0.;
  if (prob > 1.) prob = 1.;
  
  float ind_f = float(fData.size()-1) * prob;
  int   ind_i = int(std::floor(ind_f));
  float w_i1 = ind_f - ind_i;
  float w_i0 = 1. - w_i1;

  float res;
  if (ind_i == fData.size()-1 || ind_f == 0.) {
    res = fData[ind_i];
  } else {
    res = w_i0 * fData[ind_i] + w_i1 * fData[ind_i+1];
  }

#ifdef never
  printf("+++1");
  for (int i=0; i<fData.size(); i++) printf(" %6.0f", fData[i]);
  printf("\n");

  printf("+++2 p=%6.3f n=%5d i_f=%6.1f i_i=%5d d[i]=%4.0f d[+1]=%4.0f res=%6.1f\n",
         prob, int(fData.size()), ind_f, ind_i, fData[ind_i],
         (ind_i<fData.size()-1) ? fData[ind_i+1] : 0, res);
#endif

  return res;

}

//----------------------------------------------------------------------------
//! Returns entry with index \a ind.
/*!
 * \param ind entry number, between 0 and numEntries()-1
 * \throws std::out_of_range if \a ind out of range
 */

float nxyter::DistFunc::operator[](int ind)
{
  sort();
  return fData.at(ind);
}

//----------------------------------------------------------------------------
//! Returns median of distribution.

float nxyter::DistFunc::getMedian()
{
  return (*this)(0.5);
}

//----------------------------------------------------------------------------
//! Returns width of distribution at +- \a cut level.

float nxyter::DistFunc::getWidth(float cut)
{
  return (*this)(0.5+cut) - (*this)(0.5-cut);
}

//----------------------------------------------------------------------------
//! Returns lowest entry value.

float nxyter::DistFunc::getMin()
{
  if (fData.size() == 0) return 0.;
  return (*this)[0];
}

//----------------------------------------------------------------------------
//! Returns hightest entry value.

float nxyter::DistFunc::getMax()
{
  if (fData.size() == 0) return 0.;
  return (*this)[fData.size()-1];
}

//----------------------------------------------------------------------------
//! Sorts the entries array.  if (fData.size() == 0) return 0.;
  
void nxyter::DistFunc::sort()
{
  if (!fSorted) {
    std::sort(fData.begin(), fData.end());
    fSorted = true;
  }
}

