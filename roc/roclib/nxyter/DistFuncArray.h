//============================================================================
/*! \file DistFuncArray.h
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#ifndef NXYTER_DISTFUNCARRAY_H
#define NXYTER_DISTFUNCARRAY_H

#include <vector>

#include "nxyter/DistFunc.h"

namespace nxyter {     

  class DistFuncArray {
    protected:
      std::vector<DistFunc>   fDist;        //!< array

    public:
      explicit DistFuncArray(int dim, int cap=0);
      virtual ~DistFuncArray();

      void setMaxEntries(int max);
      void setCapacity(int cap);
      void clear();

      //! Adds an entry with value \a val to distribution \a ind.
      /*!
       * \param val value to be added to the distribution
       * \param ind distribution index in range 0 to size()-1
       * \throws std::out_of_range if \a ind out of range
       * \sa DistFunc::addEntry()
       */
      void addEntry(int ind, float val)
      { fDist.at(ind).addEntry(val); }

      int numEntries();

      //! Returns array size (as defined at ctor time).
      int size()
        { return fDist.size(); }

      //! Access DistFunc object with index \a ind.
      /*!
       * \param ind index, allowed range is 0 to size()-1
       * \throws std::out_of_range if \a ind out of range
       */
      DistFunc& operator[](int ind)
        { return fDist.at(ind); }
  };

}

#endif
