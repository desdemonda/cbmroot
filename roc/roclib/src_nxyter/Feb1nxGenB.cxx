//============================================================================
/*! \file Feb1nxGenB.cxx
 *  \author W.F.J.Mueller/GSI, clone from Feb1nxGen
 */
//============================================================================

#include "roc/defines_roc.h"
#include "nxyter/Feb1nxGenB.h"

/*!
 * \class nxyter::Feb1nxGenB
 * \brief Represents a FEB Rev B with 1 nXYTER
 *
 */

//----------------------------------------------------------------------------
//! Constructor with full addressing path information.
/*!
 * Sets up \c Feb1nxGenB object with full addressing information
 * \param board ROC board pointer
 * \param port the \ref glo_roc_port
 */

nxyter::Feb1nxGenB::Feb1nxGenB(roc::Board* board, int port) :
  nxyter::FebBase(board, port, kFeb1nxGenB)
{
  int nxnum = (port==CON19) ? 0 : 2;
  //      addr, nxnum, adc
  addNxChip( 8, nxnum,   0);                // data taken from 1st ADC channel
                                            // Note: Rev C is 2nd ADC channel !
}

//----------------------------------------------------------------------------

nxyter::Feb1nxGenB::~Feb1nxGenB()
{
}

