//============================================================================
/*! \file NxDataSummary.h
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#ifndef NXYTER_NXDATASUMMARY_H
#define NXYTER_NXDATASUMMARY_H

#include <iostream>
#include <vector>

#include "nxyter/DistFuncArray.h"

namespace nxyter {

   class NxDataSummary {
      protected:
         std::vector<int>    fNumEntries;     //!< number entries
         std::vector<bool>   fFull;           //!< df full flag was set
         std::vector<float>  fMedian;         //!< median
         std::vector<float>  fWidth50;        //!< width 25-75
         std::vector<float>  fWidth80;        //!< width 10-90
         std::vector<float>  fMin;            //!< min entry
         std::vector<float>  fMax;            //!< max entry

      public:
         NxDataSummary();
         virtual ~NxDataSummary();

         void clear();
         void analyse(DistFuncArray& fda);

         //! Returns number of entries for channel \a cha.
         int numEntries(int cha) const
           { return fNumEntries.at(cha); }

         //! Returns 'full' flag for channel \a cha.
         int full(int cha) const
           { return fFull.at(cha); }

         //! Returns median for channel \a cha.
         float getMedian(int cha) const
           { return fMedian.at(cha); }

         //! Returns 25-75 width (FWHM) for channel \a cha.
         float getWidth50(int cha) const
           { return fWidth50.at(cha); }

         //! Returns 10-90 width for channel \a cha.
         float getWidth80(int cha) const
           { return fWidth80.at(cha); }

         //! Returns lowest value found for channel \a cha.
         float getMin(int cha) const
           { return fMin.at(cha); }

         //! Returns highest value found for channel \a cha.
         float getMax(int cha) const
           { return fMax.at(cha); }

         void print(std::ostream& os, int nxnum=0) const;

   };

}

#endif
