//============================================================================
/*! \file Feb1nxGenB.h
 *  \author W.F.J.Mueller/GSI, clone from Feb1nxGen
 */
//============================================================================

#ifndef NXYTER_FEB1NXGENB_H
#define NXYTER_FEB1NXGENB_H

#include "nxyter/FebBase.h"

namespace nxyter {

   class Feb1nxGenB : public nxyter::FebBase {
      public:
         Feb1nxGenB() {}
         Feb1nxGenB(roc::Board* board, int port);
         virtual ~Feb1nxGenB();

   };

}

#endif
