//============================================================================
/*! \file Peripheral.h
 *  \author Norbert Abel/KIP
 */
//============================================================================

#ifndef ROC_PERIPHERAL_H
#define ROC_PERIPHERAL_H

#include "roc/Board.h"

namespace roc {

//! Base class to represent ROC peripherals.
/*!
 * Concrete ROC peripherals, like the I2C bus interface, will be represented
 * by classes derived from \c Peripheral.
 * 
 * The class holds the address information needed to reach the \c Peripheral,
 * which at the base class level is just the pointer to the roc::Board which 
 * hosts the \c Peripheral.
 */ 

   class Peripheral {
      protected:
         roc::Board* fBoard;                //!< board pointer
      public:
         Peripheral() : fBoard(0) {}
         Peripheral(roc::Board* board);
         virtual ~Peripheral() {}

         roc::Board& board();

         //! Returns pointer to the roc::Board which hosts the \c Peripheral.
         /*!
          * Note: normally using \c board() is more convenient.
          */
         roc::Board* getBoard() const { return fBoard; }
   };

}

#endif
