//============================================================================
/*! \file Feb4nxBT.cxx
 *  \author W.F.J.Mueller/GSI, significant adaptions
 *  \author Norbert Abel/KIP, initial version
 */
//============================================================================

#include "roc/defines_roc.h"
#include "nxyter/Feb4nxBT.h"

/*!
 * \class nxyter::Feb4nxBT
 * \brief Represents a 'CBM Beam Tracker' FEB with 4 nXYTER
 *
 */

//----------------------------------------------------------------------------
//! Constructor with full addressing path information.
/*!
 * Sets up \c Feb4nxBT object with full addressing information
 * \param board ROC board pointer
 */

nxyter::Feb4nxBT::Feb4nxBT(roc::Board* board) :
  nxyter::FebBase(board, 0, kFeb4nxBT)
{
  //      addr, nxnum, adc
  addNxChip(72,     0,   1);
  addNxChip(82,     1,   0);
  addNxChip(84,     2,   3);
  addNxChip(88,     3,   2);
}

//----------------------------------------------------------------------------

nxyter::Feb4nxBT::~Feb4nxBT()
{
}

