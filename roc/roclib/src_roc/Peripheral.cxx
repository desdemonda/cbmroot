//============================================================================
/*! \file Peripheral.cxx
 *  \author Norbert Abel/KIP
 */
//============================================================================

#include <stdexcept>
#include "roc/Peripheral.h"


//----------------------------------------------------------------------------
//! Contructor with roc::Board pointer
/*!
 * \c roc::Peripheral should always constructed via this ctor because
 * the \a board pointer to a \c roc::Board instance is always needed.
 * 
 * \param board pointer to the \c roc::Board which hosts the \c Peripheral.
 */
roc::Peripheral::Peripheral(roc::Board* board)
{
  fBoard = board;
}

//----------------------------------------------------------------------------
//! Return reference to \c roc::Board which hosts the \c Peripheral.
/*!
 * \throws std::runtime_error if board pointer not initialized or 0.
 */

roc::Board& roc::Peripheral::board()
{
   if (fBoard==0) throw std::runtime_error("Board object not specified");
   return *fBoard;
}

//----------------------------------------------------------------------------
