//============================================================================
/*! \file Feb1nxGenD.cxx
 *  \author W.F.J.Mueller/GSI, significant adaptions
 *  \author Norbert Abel/KIP, initial version
 */
//============================================================================

#include "roc/defines_roc.h"
#include "nxyter/Feb1nxGenD.h"

/*!
 * \class nxyter::Feb1nxGenD
 * \brief Represents a FEB Rev D with 1 nXYTER
 *
 */

//----------------------------------------------------------------------------
//! Constructor with full addressing path information.
/*!
 * Sets up \c Feb1nxGenD object with full addressing information
 * \param board ROC board pointer
 * \param port the \ref glo_roc_port
 */

nxyter::Feb1nxGenD::Feb1nxGenD(roc::Board* board, int port) :
  nxyter::FebBase(board, port, kFeb1nxGenD)
{
  fMonAdcDevice.setSlaveAddr(41);
  int nxnum = (port==CON19) ? 0 : 2;
  //      addr, nxnum, adc
  addNxChip( 8, nxnum,   1);                // data taken from 2nd ADC channel
}

//----------------------------------------------------------------------------

nxyter::Feb1nxGenD::~Feb1nxGenD()
{
}

