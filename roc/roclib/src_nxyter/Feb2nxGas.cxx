//============================================================================
/*! \file Feb2nxGas.cxx
 *  \author W.F.J.Mueller/GSI, significant adaptions
 *  \author Norbert Abel/KIP, initial version
 */
//============================================================================

#include "roc/defines_roc.h"
#include "nxyter/Feb2nxGas.h"

/*!
 * \class nxyter::Feb2nxGas
 * \brief Represents a 'PANDA GEM TPC' FEB with 2 nXYTER
 *
 */

//----------------------------------------------------------------------------
//! Constructor with full addressing path information.
/*!
 * Sets up \c Feb2nxGas object with full addressing information
 * \param board ROC board pointer
 * \param port the \ref glo_roc_port
 * \todo MainAdc::setChannelMux() setup pure guess, never verified
 */

nxyter::Feb2nxGas::Feb2nxGas(roc::Board* board, int port) :
  nxyter::FebBase(board, port, kFeb2nxGas)
{
  int nxnum0 = (port==CON19) ? 0 : 2;
  int nxnum1 = (port==CON19) ? 1 : 3;

  //      addr, nxnum, adc
  addNxChip(40, nxnum0,  0);
  addNxChip(48, nxnum1,  1);
}

//----------------------------------------------------------------------------

nxyter::Feb2nxGas::~Feb2nxGas()
{
}

