//============================================================================
/*! \file Feb1nxGenC.h
 *  \author W.F.J.Mueller/GSI, significant adaptions
 *  \author Norbert Abel/KIP, initial version
 */
//============================================================================

#ifndef NXYTER_FEB1NXGEND_H
#define NXYTER_FEB1NXGEND_H

#include "nxyter/FebBase.h"

namespace nxyter {

   class Feb1nxGenD : public nxyter::FebBase {
      public:
         Feb1nxGenD() {}
         Feb1nxGenD(roc::Board* board, int port);
         virtual ~Feb1nxGenD();

   };

}

#endif
