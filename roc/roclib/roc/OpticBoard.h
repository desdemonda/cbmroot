#ifndef ROC_OPTICBOARD_H
#define ROC_OPTICBOARD_H

#include "roc/Board.h"

#include "roc/defines_optic.h"

namespace roc {

   class OpticBoard : public roc::Board {

      protected:

         OpticBoard();
         virtual ~OpticBoard();

      public:

         virtual int getTransportKind() const { return roc::kind_ABB; }
   };

}

#endif
