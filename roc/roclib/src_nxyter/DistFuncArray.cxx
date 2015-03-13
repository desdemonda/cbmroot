//============================================================================
/*! \file DistFuncArray.cxx
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#include "nxyter/DistFuncArray.h"

/*!
 * \class nxyter::DistFuncArray
 * \brief Array of DistFunc objects
 *
 */

//----------------------------------------------------------------------------
//! Constructor, use \a dim nxyter::DistFunc objects with capacity \a cap.

nxyter::DistFuncArray::DistFuncArray(int dim, int cap) :
  fDist(dim)
{
  for (int i=0; i<dim; i++) fDist[i].setCapacity(cap);
}

//----------------------------------------------------------------------------

nxyter::DistFuncArray::~DistFuncArray()
{
}

//----------------------------------------------------------------------------
//! Set maximum number of entries accepted to \a max.
/*!
 * Calls DistFunc::setMaxEntries() for all array elements.
 */

void nxyter::DistFuncArray::setMaxEntries(int max)
{ 
  for (int i=0; i<fDist.size(); i++) fDist[i].setMaxEntries(max);
}

//----------------------------------------------------------------------------
//! Reserve capacity to hold \a cap entries.
/*!
 * Calls DistFunc::setCapacity() for all array elements.
 */

void nxyter::DistFuncArray::setCapacity(int cap)
{ 
  for (int i=0; i<fDist.size(); i++) fDist[i].setCapacity(cap);
}

//----------------------------------------------------------------------------
//! Drop all entries. 
/*!
 * Calls DistFunc::clear() for all array elements.
 */

void nxyter::DistFuncArray::clear()
{
  for (int i=0; i<fDist.size(); i++) fDist[i].clear();
}

//----------------------------------------------------------------------------
//! Returns total number of entries in all distributions of array.

int nxyter::DistFuncArray::numEntries()
{
  int nent = 0;
  for (int i=0; i<fDist.size(); i++) nent += fDist[i].numEntries();
  return nent;
}

//----------------------------------------------------------------------------
