//============================================================================
/*! \file I2cDevice.cxx
 *  \author W.F.J.Mueller/GSI, based on earlier work of Norbert Abel/KIP
 */
//============================================================================

#include "roc/I2cDevice.h"
#include "roc/defines_roc.h"
#include "roc/defines_i2c.h"

using roc::Board;

/*!
 * \class roc::I2cDevice
 * \brief Base class to represent a ROC I2C Bus Device.
 *
 * A ROC can have several I2C bus ports. They are served by a common control
 * logic, a switch determines which port is active at a given time. Each bus
 * can have several devices attached, identified by a <em>slave address</em>.
 * 
 * The class holds the addressing information needed to reach the \c I2cDevice:
 * \li the ROC board pointer (in the base class)
 * \li the I2C bus port number
 * \li the I2C device slave address
 */ 

//----------------------------------------------------------------------------
//! Default constructor to make old ROOT hapy
roc::I2cDevice::I2cDevice() :
  roc::Peripheral(), fPort(0), fSlaveAddr(0)
{
}


//----------------------------------------------------------------------------
//! Constructor with full addressing path information.
/*!
 * Sets up \c I2cDevice object with full addressing information
 * \param board ROC board pointer
 * \param port the I2C bus port number
 * \param addr the I2C device slave address
 */

roc::I2cDevice::I2cDevice(roc::Board* board, int port, uint8_t addr) :
  roc::Peripheral(board), fPort(port), fSlaveAddr(addr)
{
}

//----------------------------------------------------------------------------

roc::I2cDevice::~I2cDevice()
{
}

//----------------------------------------------------------------------------
//! Write to device register with optional readback verification.
/*!
 * Writes the 8bit value \a val to device register \a reg. If \a veri
 * is \c true, the register contents is read back and verified.
 * The I2C status is checked for error flags after the write and 
 * optional read cycle.
 *
 * \param reg device register number
 * \param val value
 * \param veri if \c true readback verification done (default is \c false)
 * \returns see \ref roc_board_opererr
 *
 * \sa getRegister(), setRegisterVerify()
 */

int roc::I2cDevice::setRegister(uint8_t reg, uint8_t val, bool veri)
{
  bool isput[7];
  uint32_t addr[7];
  uint32_t data[7];
  int rc;

  isput[0] = true;  addr[0] = ROC_NX_I2C_SWITCH;    data[0] = fPort;
  isput[1] = true;  addr[1] = ROC_NX_I2C_SLAVEADDR; data[1] = fSlaveAddr;
  isput[2] = true;  addr[2] = ROC_NX_I2C_REGISTER;  data[2] = reg;
  isput[3] = true;  addr[3] = ROC_NX_I2C_DATA;      data[3] = val;
  isput[4] = false; addr[4] = ROC_NX_I2C_ERROR;
  isput[5] = false; addr[5] = ROC_NX_I2C_DATA;
  isput[6] = false; addr[6] = ROC_NX_I2C_ERROR;

  int nop = veri ? 7 : 5;

  rc = board().operGen(isput, addr, data, nop);

  if (rc == 0 && data[4] != 0) {            // check I2C bus error on put
    rc = Board::operErrBuild(Board::kOperBusErr, 0);
  }
  if (veri) {
    if (rc == 0 && data[6] != 0) {          // check I2C bus error on readback
      rc = Board::operErrBuild(Board::kOperBusErr, 1);
    }
    if (rc == 0 && val != (data[5]&0xff)) { // check readback value
      rc = Board::kOperVerifyErr;
    }
  }
  
  return rc;
}

//----------------------------------------------------------------------------
//! Read 8 bit from device register.
/*!
 * Reads an 8bit value from device register \a reg and checks
 * I2C status for error flags.
 *
 * \param reg device register number
 * \param val value
 * \returns see \ref roc_board_opererr
 *
 * \sa setRegister()
 */

int roc::I2cDevice::getRegister(uint8_t reg, uint8_t& val)
{
  bool isput[5];
  uint32_t addr[5];
  uint32_t data[5];
  int rc;

  isput[0] = true;  addr[0] = ROC_NX_I2C_SWITCH;    data[0] = fPort;
  isput[1] = true;  addr[1] = ROC_NX_I2C_SLAVEADDR; data[1] = fSlaveAddr;
  isput[2] = true;  addr[2] = ROC_NX_I2C_REGISTER;  data[2] = reg;
  isput[3] = false; addr[3] = ROC_NX_I2C_DATA;
  isput[4] = false; addr[4] = ROC_NX_I2C_ERROR;

  rc = board().operGen(isput, addr, data, 5);

  val = uint8_t(data[3]);

  if (rc == 0 && data[4] != 0) {            // check I2C bus error on get
    rc = Board::kOperBusErr;
  }
  
  return rc;
}

//----------------------------------------------------------------------------
//! Read 16 bit from device register.
/*!
 * Reads an 16bit value from device register \a reg and checks
 * I2C status for error flags.
 *
 * \param reg device register number
 * \param val value
 * \returns see \ref roc_board_opererr
 *
 */

int roc::I2cDevice::getRegister16(uint8_t reg, uint16_t& val)
{
  bool isput[7];
  uint32_t addr[7];
  uint32_t data[7];
  int rc;

  isput[0] = true;  addr[0] = ROC_NX_I2C_SWITCH;    data[0] = fPort;
  isput[1] = true;  addr[1] = ROC_NX_I2C_SLAVEADDR; data[1] = fSlaveAddr;
  isput[2] = true;  addr[2] = ROC_NX_I2C_REGISTER;  data[2] = reg;
  isput[3] = true;  addr[3] = ROC_NX_I2C_READ16;    data[3] = 1;
  isput[4] = false; addr[4] = ROC_NX_I2C_DATA;
  isput[5] = false; addr[5] = ROC_NX_I2C_ERROR;
  isput[6] = true;  addr[6] = ROC_NX_I2C_READ16;    data[6] = 0;

  rc = board().operGen(isput, addr, data, 7);

  val = uint16_t(data[4]);

  if (rc == 0 && data[5] != 0) {            // check I2C bus error on get
    rc = Board::kOperBusErr;
  }
  
  return rc;
}

//----------------------------------------------------------------------------
//! Write to device register and return the readback value.
/*!
 * Writes the 8bit value \a valset to device register \a reg, reads
 * it back and returns the readback value in \a valget.
 * The I2C status is checked for error flags after the write and read cycle.
 *
 * \param reg device register number
 * \param valset value to be written
 * \param valget value returned from readback
 * \returns see \ref roc_board_opererr
 *
 * \note The method just returns the readback value and does not test whether
 *       \a valget equals \a valset, thus never returns a Board::kOperVerifyErr
 *       return code.
 *
 * \sa setRegister(), getRegister()
 */

int roc::I2cDevice::setRegisterVerify(uint8_t reg, uint8_t valset, 
                                      uint8_t& valget)
{
  bool isput[7];
  uint32_t addr[7];
  uint32_t data[7];
  int rc;

  isput[0] = true;  addr[0] = ROC_NX_I2C_SWITCH;    data[0] = fPort;
  isput[1] = true;  addr[1] = ROC_NX_I2C_SLAVEADDR; data[1] = fSlaveAddr;
  isput[2] = true;  addr[2] = ROC_NX_I2C_REGISTER;  data[2] = reg;
  isput[3] = true;  addr[3] = ROC_NX_I2C_DATA;      data[3] = valset;
  isput[4] = false; addr[4] = ROC_NX_I2C_ERROR;
  isput[5] = false; addr[5] = ROC_NX_I2C_DATA;
  isput[6] = false; addr[6] = ROC_NX_I2C_ERROR;

  rc = board().operGen(isput, addr, data, 7);

  valget = uint8_t(data[5]);

  if (rc == 0 && data[4] != 0) {            // check I2C bus error on put
    rc = Board::operErrBuild(Board::kOperBusErr, 0);
  }
  if (rc == 0 && data[6] != 0) {            // check I2C bus error on readback
    rc = Board::operErrBuild(Board::kOperBusErr, 1);
  }
  
  return rc;
}

//----------------------------------------------------------------------------
//! Write to device register array with optional readback verification.
/*!
 * Writes the \a nreg 8bit values from array \a val to an array of device 
 * registers starting at register number \a reg. When \a veri is \c true,
 * each value is readback and verified.
 * The I2C error flags are checked for each transfer, writing is stopped
 * when the first error is detected.
 *
 * \param reg first device register number
 * \param val value array
 * \param nreg number of registers to write
 * \param veri if \c true readback verification done (default is \c false)
 * \returns see \ref roc_board_opererr
 *
 * \sa getRegisterArray()
 */

int roc::I2cDevice::setRegisterArray(uint8_t reg, const uint8_t *val, int nreg,
                                     bool veri)
{
  for (int i=0; i<nreg; i++) {
    int rc = setRegister(reg+i, val[i], veri);
    if (rc) return Board::operErrBuild(rc, i);
  }
  return 0;
}

//----------------------------------------------------------------------------
//! Read from device register array.
/*!
 * Reads \a nreg 8bit values from an array of device registers starting at 
 * register number \a reg. 
 * The I2C error flags are checked for each transfer, reading is stopped 
 * when the first error is detected.
 *
 * \param reg first device register number
 * \param val value array
 * \param nreg number of registers to read
 * \returns see \ref roc_board_opererr
 *
 * \sa setRegisterArray()
 */

int roc::I2cDevice::getRegisterArray(uint8_t reg, uint8_t *val, int nreg)
{
  for (int i=0; i<nreg; i++) {
    int rc = getRegister(reg+i, val[i]);
    if (rc) return Board::operErrBuild(rc, i);
  }
  return 0;
}

//----------------------------------------------------------------------------
//! Write to device mailbox register
/*!
 * Writes the \a nreg 8bit values from array \a val to a single device 
 * register \a reg. This method is useful for Fifo's or mailbox type
 * registers.
 * The I2C error flags are checked for each transfer, writing is stopped when 
 * the first error is detected.
 *
 * \param reg first device register number
 * \param val value array
 * \param nval number of values to write
 * \returns see \ref roc_board_opererr
 *
 * \sa getMailboxRegister()
 */

int roc::I2cDevice::setMailboxRegister(uint8_t reg, const uint8_t *val, int nval)
{
  for (int i=0; i<nval; i++) {
    int rc = setRegister(reg, val[i]);
    if (rc) return Board::operErrBuild(rc, i);
  }
  return 0;
}

//----------------------------------------------------------------------------
//! Read from device mailbox register.
/*!
 * Reads \a nreg 8bit values from a single device register \a reg. 
 * This method is useful for Fifo's or mailbox type registers.
 * The I2C error flags are checked for each transfer, reading is stopped when 
 * the first error is detected.
 *
 * \param reg first device register number
 * \param val value array
 * \param nval number of values to read
 * \returns see \ref roc_board_opererr
 *
 * \sa setMailboxRegister()
 */

int roc::I2cDevice::getMailboxRegister(uint8_t reg, uint8_t *val, int nval)
{
  for (int i=0; i<nval; i++) {
    int rc = getRegister(reg, val[i]);
    if (rc) return Board::operErrBuild(rc, i);
  }
  return 0;
}

//----------------------------------------------------------------------------
//! Add the I2C interface address set to the address mapping tables.

void roc::I2cDevice::addAddrMap(roc::Board* board)
{
  board->addRocAddrMapping("ROC_NX_I2C_DATA", ROC_NX_I2C_DATA);
  board->addRocAddrMapping("ROC_NX_I2C_SLAVEADDR", ROC_NX_I2C_SLAVEADDR);
  board->addRocAddrMapping("ROC_NX_I2C_REGISTER", ROC_NX_I2C_REGISTER);
  board->addRocAddrMapping("ROC_NX_I2C_ERROR", ROC_NX_I2C_ERROR);
  board->addRocAddrMapping("ROC_NX_I2C_SWITCH", ROC_NX_I2C_SWITCH);
  board->addRocAddrMapping("ROC_NX_I2C_READ16", ROC_NX_I2C_READ16);
}

//----------------------------------------------------------------------------
