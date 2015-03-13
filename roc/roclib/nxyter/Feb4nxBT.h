//============================================================================
/*! \file Feb4nxBT.h
 *  \author W.F.J.Mueller/GSI, significant adaptions
 *  \author Norbert Abel/KIP, initial version
 */
//============================================================================

#ifndef NXYTER_FEB4NXBT_H
#define NXYTER_FEB4NXBT_H

#include "nxyter/FebBase.h"

namespace nxyter {

   class Feb4nxBT : public nxyter::FebBase {
      public:
         Feb4nxBT() {}
         Feb4nxBT(roc::Board* board);
         virtual ~Feb4nxBT();

   };

}

#endif
