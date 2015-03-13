//============================================================================
/*! \file NxChip.cxx
 *  \author W.F.J.Mueller/GSI, minor adaptions
 *  \author Norbert Abel/KIP, original versions
 */
//============================================================================

#include "nxyter/NxChip.h"
#include "nxyter/RocNx.h"
#include "nxyter/defines_nxyter.h"

using roc::Board;

/*!
 * \class nxyter::NxChip
 * \brief Represents the full interface of an nXYTER.
 *
 * This class provides the access to the ROC and I2C interface parts of an
 * nXYTER. It holds the full address information but no further context.
 *
 * The I2C interface is accessible via a separate nxyter::NxI2c object
 * which can be accessed via the i2c() method.
 * The ROC part of the interface control the parameters specific to one
 * nXYTER chip.
 */

//----------------------------------------------------------------------------
//!Constructor with full addressing path information.
/*!
 * Sets up \c NxChip object with full addressing information
 * \param board ROC board pointer
 * \param port the \ref glo_roc_port (equivalent to the I2C bus interface port)
 * \param addr the I2C device slave address
 * \param nxnum see \ref glo_nxyter_nxnum
 */

nxyter::NxChip::NxChip(roc::Board* board, int port, int addr, int nxnum) :
   roc::Peripheral(board),
   fI2c(board, port, addr),
   fNxNumber(nxnum)

{
}

//----------------------------------------------------------------------------

nxyter::NxChip::~NxChip()
{
}

//----------------------------------------------------------------------------
//! Set the \ref glo_nxyter_nxnum.
/*!
 * Sets the \ref glo_nxyter_nxnum, used to address nXYYER specific ROC 
 * resources.
 * \param nxnum see \ref glo_nxyter_nxnum
 * \sa getNxNumber()
 */

void nxyter::NxChip::setNxNumber(int nxnum)
{
  fNxNumber = nxnum & 0x3;                      // accept only 2 bits
  if (nxnum<0 | nxnum>3) board().Debug(-1, "Not a valid Nx-Number %d, trimmed",
                                       nxnum);
}

//----------------------------------------------------------------------------
//! Activate data taking of nXYTER.
/*!
 * This method enables the acceptance of <em>data valid</em> bits comming
 * from the nXYTER. 
 *
 * \returns see \ref roc_board_opererr.
 *
 * \sa deactivate()
 */

int nxyter::NxChip::activate()
{
   nxyter::RocNx rocnx(getBoard());
   uint32_t mask;
   int rc = rocnx.getNxActive(mask);
   if (rc) return rc;
   mask = mask | (1<<fNxNumber);
   rc = rocnx.setNxActive(mask);
   if (rc) return Board::operErrBuildInc(rc,1);
   return 0;
}

//----------------------------------------------------------------------------
//! Deactivate data taking of nXYTER.
/*!
 * This method enables the acceptance of <em>data valid</em> bits comming
 * from the nXYTER, no further hit messages will be generated.
 *
 * \returns see \ref roc_board_opererr.
 *
 * \sa activate()
 */

int nxyter::NxChip::deactivate()
{
   nxyter::RocNx rocnx(getBoard());
   uint32_t mask;
   int rc = rocnx.getNxActive(mask);
   if (rc) return rc;
   mask = mask & (~(1<<fNxNumber));
   rc = rocnx.setNxActive(mask);
   if (rc) return Board::operErrBuildInc(rc,1);
   return 0;
}

//----------------------------------------------------------------------------
//! Return actual active state of nXYTER
bool nxyter::NxChip::isactive()
{
   nxyter::RocNx rocnx(getBoard());
   uint32_t mask;
   int rc = rocnx.getNxActive(mask);
   if (rc) return false;
   return mask & (1<<fNxNumber);
}

//----------------------------------------------------------------------------
//! Set the delay compensation between nXYTER and ROC.

int nxyter::NxChip::setChannelDelay(uint32_t val)
{
   int rc = -1;
   if (fNxNumber==0) rc = board().put(ROC_NX_DELAY_NX0, val);
   if (fNxNumber==1) rc = board().put(ROC_NX_DELAY_NX1, val);
   if (fNxNumber==2) rc = board().put(ROC_NX_DELAY_NX2, val);
   if (fNxNumber==3) rc = board().put(ROC_NX_DELAY_NX3, val);
   return rc;
}

//----------------------------------------------------------------------------
//! Returns the delay compensation between nXYTER and ROC.

int nxyter::NxChip::getChannelDelay(uint32_t& val)
{
   int rc = -1;
   if (fNxNumber==0) rc = board().get(ROC_NX_DELAY_NX0, val);
   if (fNxNumber==1) rc = board().get(ROC_NX_DELAY_NX1, val);
   if (fNxNumber==2) rc = board().get(ROC_NX_DELAY_NX2, val);
   if (fNxNumber==3) rc = board().get(ROC_NX_DELAY_NX3, val);
   return rc;
}

//----------------------------------------------------------------------------
