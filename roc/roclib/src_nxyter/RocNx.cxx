#include <cmath>

#include <stdlib.h>

#include "nxyter/RocNx.h"
#include "nxyter/defines_nxyter.h"
#include "roc/defines_roc.h"

//using roc::Board;

/*!
 * \class nxyter::RocNx
 * \brief Device class with all generic nXYTER access methods
 *
 */ 

//----------------------------------------------------------------------------
//! Constructor with full addressing path information.
/*!
 * Sets up \c RocNx object with full addressing information
 * \param board ROC board pointer
 */

nxyter::RocNx::RocNx(roc::Board* board) :
  roc::Peripheral(board)
{
}

//----------------------------------------------------------------------------

nxyter::RocNx::~RocNx()
{
}

//----------------------------------------------------------------------------
//! Set nXYTER specific interface registers in ROC to default values.
/*!
 * This method sets default values for
 * \li ROC number (0)
 * \li nXYTER active mask (all off)
 * \li ADC port selector (values for Feb1nx)
 * \li ADC clock delay (value good for 2-4 segment cables)
 * \li ADC latency (value good for 2-4 segment cables)
 * \li nXYTER delay (value good for iDUR=15 operation)
 */
int nxyter::RocNx::setToDefault()
{
  typedef struct
  {
    uint32_t addr;
    uint32_t val;
  } inititem_t;

  inititem_t initlist[] = {
    ROC_NX_NXACTIVE,          0,              // deactivate all nx
    ROC_NX_FEB4NX,             0,
    ROC_NX_PARITY_CHECK,       1,              // enable nx parity check
    ROC_NX_ADC_PORT_SELECT1,   1,              // adc port select for Feb1nx
    ROC_NX_ADC_PORT_SELECT2,   0,
    ROC_NX_ADC_PORT_SELECT3,   1,
    ROC_NX_ADC_PORT_SELECT4,   0,
    ROC_NX_SR_INIT,       0xfc03,              // adc delay 11
    ROC_NX_BUFG_SELECT,        0,
    ROC_NX_SR_INIT2,      0xfc03,              // adc delay 11
    ROC_NX_BUFG_SELECT2,       0,
    ROC_NX_ADC_LATENCY1,      65,              // good for all cables
    ROC_NX_ADC_LATENCY2,      65,
    ROC_NX_ADC_LATENCY3,      65,
    ROC_NX_ADC_LATENCY4,      65,
    ROC_NX_DELAY_LTS,        512,              // !! the holy default !!
    ROC_NX_DELAY_NX0,        288,              // good for iDUR=15 and all cables
    ROC_NX_DELAY_NX1,        288,
    ROC_NX_DELAY_NX2,        288,
    ROC_NX_DELAY_NX3,        288,
    ROC_NX_DEBUG_MODE,         0,              // make sure it's off
    0xffffffff,    0xffffffff               // end marker
  };

  for (inititem_t* p=initlist; p->addr !=0xffffffff; p++) {
    int rc = board().put(p->addr, p->val);
    if (rc) return roc::Board::operErrBuild(rc, p-initlist);
  }
  return 0;
}


//----------------------------------------------------------------------------
//! Reset nXYTER (and ROC) timestamp system.
/*!
 * This method brings the timestamp system of all connected nXYTERs and the
 * local timestamp counter of the ROC into a consistent state, counting up
 * from almost zero.
 *
 * \returns see \ref roc_board_opererr
 */

int nxyter::RocNx::resetRocNxTs()
{
   roc::OperList lst;

   lst.addPut(ROC_NX_TS_RESET, 1);
   lst.addPut(ROC_NX_TS_RESET, 0);

   return board().operGen(lst);
}


//----------------------------------------------------------------------------
//! Fire a test pulse sequence.
/*!
 * The ROC has a build-in test pulse generator which can produce pulse trains
 * with programmable period, width and number of cycles. This method causes 
 * the ROC to generate a test pulse sequence with a period of \a period * 4 ns,
 * a pulse width of \a width * 4 ns, and \a number rising test pulse edges.
 * If \a number if 0 the generator produces test pulses continuously up to 
 * the next fireTestPulse(uint32_t, int32_t, uint32_t) or
 * fireTestPulse(uint32_t, uint32_t, int32_t uint32_t) call. 
 * To stop a continuous sequence simply call fireTestPulse(2,1,1).
 *
 * \param period controls period, rising edges occur every \a period * 4 ns.
 *          If \c period is 0, a currently running test pulse train is aborted.
 *          In this case \c width and \c number are ignored and can be
 *          specified as 0.
 * \param width controls the pulse shape. If \a width > 0, the "H" time is
 *          \a width * 4 ns and the "L" time is \a period - \a width * 4 ns.
 *          If \a width < 0, the "L" time is \a width * 4 ns and the "H" time 
 *          is \a period - \a width * 4 ns.
 * \param number controls number of generated pulses. Only the lower 15 LSB
 *          are used, maximal pulse train length is 32767 pulses. A 0 causes
 *          a continuous pulse train.
 * \returns see \ref roc_board_opererr or -1 if the arguments are out-of-range.
 *
 * \note The "L" time and the "H" time is limited to 4 * 2^24 = 67108864 ns.
 *       Usually \a width is quite small, so in practice the minimal
 *       frequency is about 14.9 Hz.
 *
 * \sa fireTestPulse(uint32_t delay, uint32_t period, int32_t width, 
 *                   uint32_t number)
 */

int nxyter::RocNx::fireTestPulse(uint32_t period, int32_t width, 
                                 uint32_t number)
{
   uint32_t llow, lhigh;

   if (period == 0) {                       // setup abort sequence
     period = 6;
     width  = 3;
     number = 1;
   }

   if (width > 0) {
     llow  = period - width;
     lhigh = width;
   } else {
     llow  = -width;
     lhigh = period + width;
   }

   if (number > 0x00008fff || width == 0 || labs(width) >= period || 
       llow > 0x00ffffff || lhigh > 0x00ffffff) {
     return -1;
   }

   int rc = board().operPPPP(ROC_NX_TESTPULSE_LENGTH,  llow - 1,
                             ROC_NX_TESTPULSE_LENGTH2, lhigh - 1,
                             ROC_NX_TESTPULSE_NUMBER, number * 2,
                             ROC_NX_TESTPULSE_START, 1);
   return rc;
}

//----------------------------------------------------------------------------
//! Fire a test pulse sequence after next nXYTER/ROC timestamp reset.
/*!
 * The ROC test pulse generator can produce a pulse train with programmable
 * period, width and number of cycles which start with a defined delay after a
 * nXYTER/ROC timestamp reset. This allows to produce nXYTER hits with
 * controlled timestamp and is used for calibrations of delay parameters.
 *
 * This method will cause a nXYTER/ROC timestamp reset (with resetRocNxTs())
 * followed a test pulse train with a period of \a period * 4 ns,
 * a pulse width of \a width * 4 ns, and \a number rising test pulse edges
 * starting \a delay * 4 ns after the reset pulse to the nXYTER.
 *
 * \param delay controls delay, first edge after delay * 4 ns after reset.
 *          Only the 16 LSB are used, the maximal delay is 4 * 65535 =
 *          262140 ns.
 * \param period controls period, rising edges occur every \a period * 4 ns.
 * \param width controls the pulse shape. If \a width > 0, the "H" time is
 *          \a width * 4 ns and the "L" time is \a period - \a width * 4 ns.
 *          If \a width < 0, the "L" time is \a width * 4 ns and the "H" time 
 *          is \a period - \a width * 4 ns.
 * \param number controls number of generated pulses. Only the lower 16 LSB
 *          are used, maximal pulse train length is 32767 pulses. A 0 causes
 *          a continuous pulse train.
 * \returns see \ref roc_board_opererr or -1 if the arguments are out-of-range.
 *
 * \note The "L" time and the "H" time is limited to 4 * 2^24 = 67108864 ns.
 *       Usually \a width is quite small, so in practice the minimal
 *       frequency is about 14.9 Hz.
 *
 * \sa fireTestPulse(uint32_t delay, uint32_t period, int32_t width)
 */

int nxyter::RocNx::fireTestPulse(uint32_t delay, uint32_t period, int32_t width,
                                  uint32_t number)
{
   int rc;

   uint32_t llow, lhigh;
   if (width > 0) {
     llow  = period - width;
     lhigh = width;
   } else {
     llow  = -width;
     lhigh = period + width;
   }
   if (number > 0x00008fff || width == 0 || labs(width) >= period || 
       delay > 0x0000ffff ||
       llow > 0x00ffffff || lhigh > 0x00ffffff) {
     return -1;
   }

   rc = board().operPPPP(ROC_NX_TESTPULSE_RESET_DELAY, delay  - 1,
                         ROC_NX_TESTPULSE_LENGTH,  llow - 1,
                         ROC_NX_TESTPULSE_LENGTH2, lhigh - 1,
                         ROC_NX_TESTPULSE_NUMBER, number * 2);
   if (rc) return rc;
   rc = resetRocNxTs();
   if (rc) return roc::Board::operErrBuild(rc, 4);
   return 0;
}

//----------------------------------------------------------------------------
//! setNxActive
/*!
 * Actiavte/deactive nXYTERs, setting on/off for each nxyter
 */

int nxyter::RocNx::setNxActive(int nx0, int nx1, int nx2, int nx3)
{
   int rc = setNxActive((nx0 ? 1 : 0) | (nx1 ? 2 : 0) |
                        (nx2 ? 4 : 0) | (nx3 ? 8 : 0));
   return rc;
}

//----------------------------------------------------------------------------
//! setRocNxActive
/*!
 * Actiavte/deactive nXYTERs, setting on/off for each nxyter
 */

int nxyter::RocNx::setNxActive(uint32_t mask)
{
   return board().put(ROC_NX_NXACTIVE, mask & 0xf);
}

//----------------------------------------------------------------------------
//! NX_getActivate
/*!
 * Read status mask which nXYTERs is on/off
 */

int nxyter::RocNx::getNxActive(uint32_t& mask)
{
   return board().get(ROC_NX_NXACTIVE, mask);
}

//----------------------------------------------------------------------------
//! setRocLTSDelay
/*!
 * Sets the global LTS Delay.
 * Please, only use this function, if know what you're doing!
 */

int nxyter::RocNx::setLTSDelay(uint32_t val)
{
   return board().put(ROC_NX_DELAY_LTS, val);
}

//----------------------------------------------------------------------------
//! Sets nXYTER parity check flag
/*!
 * Activates/Deactivates the nXYTER parity checking.
 * If parity checking is activated, the broken nXYTER hit message is replaced
 * by a nXYTER parity error system message.
 * \param val Is parity checking on? (1 is yes, 0 is no)
 * \returns see \ref roc_board_opererr
 */

int nxyter::RocNx::setParityCheck(uint32_t val)
{
   return board().put(ROC_NX_PARITY_CHECK, val);
}

//----------------------------------------------------------------------------
//! Sets debug mode readout flag in ROC
/*!
 * \param val debug mode enable flag (1 is yes, 0 is no)
 * \returns see \ref roc_board_opererr
 */

int nxyter::RocNx::setDebugMode(uint32_t val)
{
   return board().put(ROC_NX_DEBUG_MODE, val);
}

//----------------------------------------------------------------------------
//! Retrieves FIFO full state from ROC (1=fifo full)

int nxyter::RocNx::getFifoFull(uint32_t& val)
{
   return board().get(ROC_NX_FIFO_FULL, val);
}

//----------------------------------------------------------------------------
//! Retrieves FIFO empty state from ROC (1=fifo empty)

int nxyter::RocNx::getFifoEmpty(uint32_t& val)
{
   return board().get(ROC_NX_FIFO_EMPTY, val);
}

//----------------------------------------------------------------------------
//! Try to acquire \a nmsg messages from 'debug' port.
/*!
 * Uses the DAQ 'debug' port to read up to \a nmsg messages via ROC control
 * space accesses. ROC must be in 'debug' mode. The method will append
 * messages to \a data until either the maximal message count \a nmsg is
 * reached or the 'wide FIFO' is empty.
 *
 * \param datvec retrieved message are appended to the vector, the vector is
 *    not initially cleared by this method
 * \param nmsg maximal number of messages read
 * \returns number of message appended to \a data or -2 in case of 
 *    get/put/operGen errors or -1 if ROC not in debug mode. A 0 is
 *    returned if the 'wide FIFO' is empty when getDataDebug() is called.
 *
 * \sa setDebugMode(), getRocFifoEmpty()
 */

int nxyter::RocNx::getDataDebug(std::vector<nxyter::DataDebug>& datvec, 
                                int nmsg)
{
  bool isput[5];
  uint32_t addr[5];
  uint32_t data[5];
  int rc;

  isput[0] = false;  addr[0] = ROC_NX_DEBUG_MODE; 
  isput[1] = false;  addr[1] = ROC_NX_FIFO_EMPTY;

  rc = board().operGen(isput, addr, data, 2);

  if (rc) return -2;                        // io error
  if (data[0] == 0) return -1;              // not in debug mode
  if (data[1] != 0) return 0;               // fifo empty, quit

  isput[0] = false;  addr[0] = ROC_NX_ADC_DATA;
  isput[1] = false;  addr[1] = ROC_NX_NX_DATA;
  isput[2] = false;  addr[2] = ROC_NX_LT_LOW;
  isput[3] = false;  addr[3] = ROC_NX_LT_HIGH;
  isput[4] = false;  addr[4] = ROC_NX_FIFO_EMPTY;

  int nget = 0;
  while(nget < nmsg) {
    rc = board().operGen(isput, addr, data, 5);
    if (rc) return -2;                      // io error
    datvec.push_back(nxyter::DataDebug(data[0], data[1], data[2], data[3]));
    nget += 1;
    if (data[4] != 0) break;             // fifo empty, quit
  }
  return nget;
}

//----------------------------------------------------------------------------
//! Add the nXYTER interface address set to the address mapping tables.

void nxyter::RocNx::addAddrMap(roc::Board* board)
{
  board->addRocAddrMapping("ROC_NX_HWV", ROC_NX_HWV);
  board->addRocAddrMapping("ROC_NX_FIFO_RESET", ROC_NX_FIFO_RESET);
  board->addRocAddrMapping("ROC_NX_THROTTLE", ROC_NX_THROTTLE);
  board->addRocAddrMapping("ROC_NX_INIT", ROC_NX_INIT);
  board->addRocAddrMapping("ROC_NX_TS_RESET", ROC_NX_TS_RESET);
  board->addRocAddrMapping("ROC_NX_BURST1", ROC_NX_BURST1);
  board->addRocAddrMapping("ROC_NX_BURST2", ROC_NX_BURST2);
  board->addRocAddrMapping("ROC_NX_BURST3", ROC_NX_BURST3);
  board->addRocAddrMapping("ROC_NX_TESTPULSE_RESET_DELAY", ROC_NX_TESTPULSE_RESET_DELAY);
  board->addRocAddrMapping("ROC_NX_TESTPULSE_LENGTH", ROC_NX_TESTPULSE_LENGTH);
  board->addRocAddrMapping("ROC_NX_TESTPULSE_LENGTH2", ROC_NX_TESTPULSE_LENGTH2);
  board->addRocAddrMapping("ROC_NX_TESTPULSE_NUMBER", ROC_NX_TESTPULSE_NUMBER);
  board->addRocAddrMapping("ROC_NX_TESTPULSE_START", ROC_NX_TESTPULSE_START);
  board->addRocAddrMapping("ROC_NX_TESTPULSE_RUNNING", ROC_NX_TESTPULSE_RUNNING);
  board->addRocAddrMapping("ROC_NX_NXACTIVE", ROC_NX_NXACTIVE);
  board->addRocAddrMapping("ROC_NX_FEB4NX", ROC_NX_FEB4NX);
  board->addRocAddrMapping("ROC_NX_PARITY_CHECK", ROC_NX_PARITY_CHECK);
  board->addRocAddrMapping("ROC_NX_I2C_RESET", ROC_NX_I2C_RESET);
  board->addRocAddrMapping("ROC_NX_I2C_REGRESET", ROC_NX_I2C_REGRESET);
  board->addRocAddrMapping("ROC_NX_ADC_DIRECT_1a", ROC_NX_ADC_DIRECT_1a);
  board->addRocAddrMapping("ROC_NX_ADC_DIRECT_1b", ROC_NX_ADC_DIRECT_1b);
  board->addRocAddrMapping("ROC_NX_ADC_DIRECT_1c", ROC_NX_ADC_DIRECT_1c);
  board->addRocAddrMapping("ROC_NX_ADC_DIRECT_1d", ROC_NX_ADC_DIRECT_1d);
  board->addRocAddrMapping("ROC_NX_ADC_DIRECT_2a", ROC_NX_ADC_DIRECT_2a);
  board->addRocAddrMapping("ROC_NX_ADC_DIRECT_2b", ROC_NX_ADC_DIRECT_2b);
  board->addRocAddrMapping("ROC_NX_ADC_DIRECT_2c", ROC_NX_ADC_DIRECT_2c);
  board->addRocAddrMapping("ROC_NX_ADC_DIRECT_2d", ROC_NX_ADC_DIRECT_2d);
  board->addRocAddrMapping("ROC_NX_ADC_REG", ROC_NX_ADC_REG);
  board->addRocAddrMapping("ROC_NX_ADC_ADDR", ROC_NX_ADC_ADDR);
  board->addRocAddrMapping("ROC_NX_ADC_ANSWER", ROC_NX_ADC_ANSWER);
  board->addRocAddrMapping("ROC_NX_ADC_REG2", ROC_NX_ADC_REG2);
  board->addRocAddrMapping("ROC_NX_ADC_ADDR2", ROC_NX_ADC_ADDR2);
  board->addRocAddrMapping("ROC_NX_ADC_ANSWER2", ROC_NX_ADC_ANSWER2);
  board->addRocAddrMapping("ROC_NX_SR_INIT", ROC_NX_SR_INIT);
  board->addRocAddrMapping("ROC_NX_BUFG_SELECT", ROC_NX_BUFG_SELECT);
  board->addRocAddrMapping("ROC_NX_SR_INIT2", ROC_NX_SR_INIT2);
  board->addRocAddrMapping("ROC_NX_BUFG_SELECT2", ROC_NX_BUFG_SELECT2);
  board->addRocAddrMapping("ROC_NX_DELAY_LTS", ROC_NX_DELAY_LTS);
  board->addRocAddrMapping("ROC_NX_DELAY_NX0", ROC_NX_DELAY_NX0);
  board->addRocAddrMapping("ROC_NX_DELAY_NX1", ROC_NX_DELAY_NX1);
  board->addRocAddrMapping("ROC_NX_DELAY_NX2", ROC_NX_DELAY_NX2);
  board->addRocAddrMapping("ROC_NX_DELAY_NX3", ROC_NX_DELAY_NX3);
  board->addRocAddrMapping("ROC_NX_ADC_LATENCY1", ROC_NX_ADC_LATENCY1);
  board->addRocAddrMapping("ROC_NX_ADC_LATENCY2", ROC_NX_ADC_LATENCY2);
  board->addRocAddrMapping("ROC_NX_ADC_LATENCY3", ROC_NX_ADC_LATENCY3);
  board->addRocAddrMapping("ROC_NX_ADC_LATENCY4", ROC_NX_ADC_LATENCY4);
  board->addRocAddrMapping("ROC_NX_ADC_PORT_SELECT1", ROC_NX_ADC_PORT_SELECT1);
  board->addRocAddrMapping("ROC_NX_ADC_PORT_SELECT2", ROC_NX_ADC_PORT_SELECT2);
  board->addRocAddrMapping("ROC_NX_ADC_PORT_SELECT3", ROC_NX_ADC_PORT_SELECT3);
  board->addRocAddrMapping("ROC_NX_ADC_PORT_SELECT4", ROC_NX_ADC_PORT_SELECT4);
  board->addRocAddrMapping("ROC_NX_DEBUG_MODE", ROC_NX_DEBUG_MODE);
  board->addRocAddrMapping("ROC_NX_FIFO_EMPTY", ROC_NX_FIFO_EMPTY);
  board->addRocAddrMapping("ROC_NX_FIFO_FULL", ROC_NX_FIFO_FULL);
  board->addRocAddrMapping("ROC_NX_LT_LOW", ROC_NX_LT_LOW);
  board->addRocAddrMapping("ROC_NX_LT_HIGH", ROC_NX_LT_HIGH);
  board->addRocAddrMapping("ROC_NX_ADC_DATA", ROC_NX_ADC_DATA);
  board->addRocAddrMapping("ROC_NX_NX_DATA", ROC_NX_NX_DATA);
}

//----------------------------------------------------------------------------
