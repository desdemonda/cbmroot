//============================================================================
/*! \file DistFunc.h
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#ifndef NXYTER_DISTFUNC_H
#define NXYTER_DISTFUNC_H

#include <vector>

namespace nxyter {     

  class DistFunc {
    protected:
      std::vector<float> fData;             //!< data array
      int                fMaxEntries;       //!< max number of entries
      bool               fFull;             //!< max number of entries reached
      bool               fSorted;           //!< is sorted flag

    public:
      explicit DistFunc(int cap=0);
      virtual ~DistFunc();

      void setMaxEntries(int max);

      //! Reserve capacity to hold \a cap entries.
      /*!
       * Increases the size of the internal data area such that \a cap
       * entries can be stored. If more are added, the capacity will be
       * automatically increased. This method improves performance and
       * should be used when the total number of expected entries is known
       * before data accumulation starts.
       *
       * \note Don't confuse with setMaxEntries(), which sets the maximum
       *   number of entries which will be accepted.
       */
      void setCapacity(int cap)
        { fData.reserve(cap); }

      //! Drop all entries. numEntries() will be 0 afterwards.
      void clear()
      { fData.clear(); fSorted = false; fFull = false; }

      void addEntry(float val);

      //! Returns number of entries.
      int numEntries()
        { return fData.size(); }

      //! Returns the 'full' flag.
      /*!
       * If \c true is returned, addEntry() was called more often than
       * the limit setup by setMaxEntries() and these additional entries
       * were discarded.
       * \sa setMaxEntries()
       */
      bool full()
        { return fFull; }

      float operator()(float prob);
      float operator[](int ind);

      float getMedian();
      float getWidth(float cut);
      float getMin();
      float getMax();

    protected:
      void sort();
      
  };

}

#endif
