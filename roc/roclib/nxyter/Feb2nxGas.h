//============================================================================
/*! \file Feb2nxGas.h
 *  \author W.F.J.Mueller/GSI, significant adaptions
 *  \author Norbert Abel/KIP, initial version
 */
//============================================================================

#ifndef NXYTER_FEB2NXGAS_H
#define NXYTER_FEB2NXGAS_H

#include "nxyter/FebBase.h"

namespace nxyter {

   class Feb2nxGas : public nxyter::FebBase {
      public:
         Feb2nxGas() {}
         Feb2nxGas(roc::Board* board, int port);
         virtual ~Feb2nxGas();

   };

}

#endif
