//============================================================================
/*! \file Feb1nxGenC.h
 *  \author W.F.J.Mueller/GSI, significant adaptions
 *  \author Norbert Abel/KIP, initial version
 */
//============================================================================

#ifndef NXYTER_FEB1NXGENC_H
#define NXYTER_FEB1NXGENC_H

#include "nxyter/FebBase.h"

namespace nxyter {

   class Feb1nxGenC : public nxyter::FebBase {
      public:
         Feb1nxGenC() {}
         Feb1nxGenC(roc::Board* board, int port);
         virtual ~Feb1nxGenC();

   };

}

#endif
