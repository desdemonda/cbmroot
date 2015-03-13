
#include "roc/Gpio.h"
#include "roc/defines_gpio.h"

using roc::Board;

/*!
 * \class roc::Gpio
 * \brief Device class with all GPIO access methods
 *
 */ 

//----------------------------------------------------------------------------
//! Constructor with full addressing path information.
/*!
 * Sets up \c Gpio object with full addressing information
 * \param board ROC board pointer
 */

roc::Gpio::Gpio(roc::Board* board) :
  roc::Peripheral(board)
{
}

//----------------------------------------------------------------------------

roc::Gpio::~Gpio()
{
}

//----------------------------------------------------------------------------
//! Set baud rate for sync message sender or receiver
/*!
 * \param gpio_nr The number of the GPIO port
 *    \li 1 for SYNC_M the sync message sender
 *    \li 2 for SYNC_S0 the sync message receiver channel 0
 *    \li 3 for SYNC_S1 the sync message receiver channel 1
 * \param offset number of 250 MHz clock cycles between the start bit edge 
 *        and the first data capture
 * \param odd number of 250 MHz clock cycles between the (2n + 1) data capture
 *        and the (2n + 2) data capture
 * \param even number of 250 MHz clock cycles between the (2n + 0) data capture
 *        and the (2n + 1) data capture
 * \returns see \ref roc_board_opererr, or -1 of gpio_nr out of range
 *
 * \sa setSyncBaud(int gpio_nr, uint32_t ratediv)
 * \sa getSyncBaud(int gpio_nr, uint32_t& offset, 
 *                 uint32_t& odd, uint32_t& even)
 */

int roc::Gpio::setSyncBaud(int gpio_nr, uint32_t offset,
                           uint32_t odd, uint32_t even)
{
   uint32_t addr_offset[3] = {ROC_SYNC1_BAUD_START,
                              ROC_SYNC2_BAUD_START,
                              ROC_SYNC3_BAUD_START};
   uint32_t addr_odd[3]    = {ROC_SYNC1_BAUD1,
                              ROC_SYNC2_BAUD1,
                              ROC_SYNC3_BAUD1};
   uint32_t addr_even[3]   = {ROC_SYNC1_BAUD2,
                              ROC_SYNC2_BAUD2,
                              ROC_SYNC3_BAUD2};

   if (gpio_nr < 1 || gpio_nr > 3) return -1;

   int rc = board().operPPP(addr_offset[gpio_nr-1], offset-1,
                            addr_odd[gpio_nr-1],    odd-1,
                            addr_even[gpio_nr-1],   even-1);
   return rc;
}

//----------------------------------------------------------------------------
//! Set baud rate for sync message sender or receiver
/*!
 * \param gpio_nr The number of the GPIO port
 *    \li 1 for SYNC_M the sync message sender
 *    \li 2 for SYNC_S0 the sync message receiver channel 0
 *    \li 3 for SYNC_S1 the sync message receiver channel 1
 * \param ratediv rate divider factor
 * \returns see \ref roc_board_opererr, or 
 *   \li -1 if \a gpio_nr out of range
 *   \li -2 if \a ratediv too large (>511)
 *
 * \sa getSyncBaud(int gpio_nr, uint32_t ratediv)
 */

int roc::Gpio::setSyncBaud(int gpio_nr, uint32_t ratediv)
{
   uint32_t offset = ratediv/4;
   uint32_t odd    = ratediv/2;
   uint32_t even   = ratediv-odd;

   if (odd > 255 || even > 255) return -2;

   return setSyncBaud(gpio_nr, offset, odd, even);
}

//----------------------------------------------------------------------------
//! Get baud rate settings for sync message sender or receiver
/*!
 * set setSyncBaud(int gpio_nr, uint32_t& offset, uint32_t& odd, uint32_t& even)
 * for detailed description of the arguments.
 */

int roc::Gpio::getSyncBaud(int gpio_nr, uint32_t& offset,
                           uint32_t& odd, uint32_t& even)
{
   uint32_t addr_offset[3] = {ROC_SYNC1_BAUD_START,
                              ROC_SYNC2_BAUD_START,
                              ROC_SYNC3_BAUD_START};
   uint32_t addr_odd[3]    = {ROC_SYNC1_BAUD1,
                              ROC_SYNC2_BAUD1,
                              ROC_SYNC3_BAUD1};
   uint32_t addr_even[3]   = {ROC_SYNC1_BAUD2,
                              ROC_SYNC2_BAUD2,
                              ROC_SYNC3_BAUD2};

   if (gpio_nr < 1 || gpio_nr > 3) return -1;

   uint32_t offset_raw;
   uint32_t odd_raw;
   uint32_t even_raw;

   int rc = board().operGGG(addr_offset[gpio_nr-1], offset_raw,
                            addr_odd[gpio_nr-1],    odd_raw,
                            addr_even[gpio_nr-1],   even_raw);

   offset = offset_raw + 1;
   odd    = odd_raw    + 1;
   even   = even_raw   + 1;

   return rc;
}

//----------------------------------------------------------------------------
//! Get baud rate divider setting for sync message sender or receiver
/*!
 * see setSyncBaud(int gpio_nr, uint32_t ratediv)
 * for detailed description of the arguments.
 */

int roc::Gpio::getSyncBaud(int gpio_nr, uint32_t& ratediv)
{
   uint32_t offset;
   uint32_t odd;
   uint32_t even;

   int rc = getSyncBaud(gpio_nr, offset, odd, even);
   ratediv = odd + even;
   return rc;
}

//----------------------------------------------------------------------------
//! Sets the synch message sender scale down to 1 to 2** \a val.
/*!
 * The sync message sender sends a sync message either at every epoch or
 * scaled down every every 2nd, or every 4th, or every 2**n'th epoch.
 * This methods sets the scale down exponent, allowed values for \a val
 * are 0 to 21. Since the epoch is 16.384 usec this allows to adjust the
 * sync message period between ~16 usec and 35 sec.
 *
 * \param val sync message scale down exponent, range 0 to 21.
 * \returns see \ref roc_board_opererr
 *
 * \par Table of scale down factors and resulting periods:
 * <table>
 * <tr><td>\a val</td> <td>period</td>       <td>Comment</td></tr>
 * <tr><td>   0  </td> <td>  16.384 usec</td> <td>every epoch</td></tr>
 * <tr><td>   1  </td> <td>  32.768 usec</td> <td>every 2nd epoch</td></tr>
 * <tr><td>   2  </td> <td>  65.536 usec</td> <td>every 4th epoch</td></tr>
 * <tr><td>   3  </td> <td> 131.072 usec</td> <td>every 8th epoch</td></tr>
 * <tr><td>   4  </td> <td> 262.144 usec</td> <td>every 16th epoch</td></tr>
 * <tr><td>   5  </td> <td> 524.288 usec</td> <td>every 32th epoch</td></tr>
 * <tr><td>   6  </td> <td>   ~1.05 msec</td> <td>every 64th epoch</td></tr>
 * <tr><td>   7  </td> <td>   ~2.09 msec</td> <td>every 128th epoch</td></tr>
 * <tr><td>   8  </td> <td>   ~4.19 msec</td> <td>every 256th epoch</td></tr>
 * <tr><td>   9  </td> <td>   ~8.38 msec</td> <td>every 512th epoch</td></tr>
 * <tr><td>  10  </td> <td>  ~16.77 msec</td> <td>every 1024th epoch</td></tr>
 * <tr><td>  11  </td> <td>  ~33.55 msec</td> <td>every 2048th epoch</td></tr>
 * <tr><td>  12  </td> <td>  ~67.11 msec</td> <td>every 4096th epoch</td></tr>
 * <tr><td>  13  </td> <td> ~134.21 msec</td> <td>every 8192th epoch</td></tr>
 * <tr><td>  14  </td> <td> ~268.43 msec</td> <td>every 16384th epoch</td></tr>
 * <tr><td>  15  </td> <td> ~536.69 msec</td> <td>every 32768th epoch</td></tr>
 * <tr><td>  16  </td> <td>   ~1.07 sec</td> <td>every 65536th epoch</td></tr>
 * <tr><td>  17  </td> <td>   ~2.15 sec</td> <td>every 131072th epoch</td></tr>
 * <tr><td>  18  </td> <td>   ~4.29 sec</td> <td>every 262144th epoch</td></tr>
 * <tr><td>  19  </td> <td>   ~8.59 sec</td> <td>every 524288th epoch</td></tr>
 * <tr><td>  20  </td> <td>  ~17.18 sec</td> <td>every 1048576th epoch</td></tr>
 * <tr><td>  21  </td> <td>  ~34.35 sec</td> <td>every 2097152th epoch</td></tr>
 * </table>
 */

int roc::Gpio::setSyncScale(uint32_t val)
{
   return board().put(ROC_SYNC1_SCALEDOWN, val);
}

//----------------------------------------------------------------------------
//! Get the synch message sender scale down.
/*!
 * \param val sync message scale down exponent
 * \returns see \ref roc_board_opererr
 */

int roc::Gpio::getSyncScale(uint32_t& val)
{
   return board().get(ROC_SYNC1_SCALEDOWN, val);
}

//----------------------------------------------------------------------------
//! Read GPIO configuration register.
/*!
 * \param mask read value of GPIO configuration register
 * \returns see \ref roc_board_opererr
 * \sa setConfig(), unpackConfig(), packConfig()
 */

int roc::Gpio::getConfig(uint32_t& mask)
{
   return board().get(ROC_GPIO_CONFIG, mask);
}

//----------------------------------------------------------------------------
//! Write GPIO configuration register.
/*!
 * \param mask new content of the GPIO configuration register
 * \returns see \ref roc_board_opererr
 * \sa getConfig(), unpackConfig(), packConfig()
 */

int roc::Gpio::setConfig(uint32_t mask)
{
   return board().put(ROC_GPIO_CONFIG, mask);
}

//----------------------------------------------------------------------------
//! Configure one GPIO port.
/*!
 * \param gpio_nr The number of the GPIO port (2 - 7 for  SYNC_S0, SYNC_S1,
 *                  AUX0, AUX1, AUX2, AUX3)
 * \param riseedge if \c true, SYNC is enable or AUX records rising edges
 * \param falledge if \c true, SYNC is enable or AUX records falling edges
 * \param throttled if \c true, the channel is sensitive to throttling, data
 *    is dropped when ROC is throttling. If \c false, port sends data even when
 *    ROC is throttling.
 * \param extrafunc if \c true enables additional functionality associated 
 *    with port:
 *          \li for SYNC_S0: loop-back from SYNC_M
 *          \li for SYNC_S1: loop-back from SYNC_M
 *          \li for AUX2: TP_in active, ROC will send AUX2 to nXYTER TESTPULSE
 *          \li for AUX3: THR_in active, ROC will throttle when AUX3=1
 * \param altin if \c true, switches an AUX to an alternate input source:
 *          \li for AUX1: PULSER_ACTIVE (test pulse generator monitor)
 *          \li for AUX3: THROTTLE_OUT (throttle loopback)
 * \returns see \ref roc_board_opererr or -1 if gpio_nr out of range.
 *
 * \sa getConfig(), setConfig(), unpackConfig(), 
 *     packConfig()
 */

int roc::Gpio::setConfig(int gpio_nr, bool riseedge, bool falledge,
                         bool throttled, bool extrafunc, bool altin)
{
   uint32_t mask;
   int rc = getConfig(mask);
   if (rc) return rc;
   if (!packConfig(mask, gpio_nr, riseedge, falledge, throttled, 
                   extrafunc, altin)) return -1;
   rc = setConfig(mask);
   if (rc)  Board::operErrBuildInc(rc,1);
   return 0;
}

//----------------------------------------------------------------------------
//! Update part of GPIO configuration mask.
/*!
 * Updates in the GPIO configuration mask \a mask the bits corresponding to 
 * GPIO port \a gpio_nr accoding to the attributes \a riseedge, \a falledge,
 * \a throttled, \a extrafunc and \a altin.
 * For detailed description of attributes see setRocGpioConfig().
 * Unsupported attributes are ignored. Returns \c false if gpio_nr is
 * outside range 2 to 7.
 */

bool roc::Gpio::packConfig(uint32_t& mask, int gpio_nr, 
                           bool riseedge, bool falledge, 
                           bool throttled, bool extrafunc, 
                           bool altin)
{
   if (gpio_nr <2 || gpio_nr>7) return false;
  
   if (gpio_nr==2 || gpio_nr==3) {          // SYNC
     riseedge = riseedge | falledge;
     falledge = false;
   }

   mask = mask & (~(0xf<<(4*gpio_nr)));
   uint32_t submask = 0;
   if (riseedge)  submask |= 0x1;
   if (falledge)  submask |= 0x2;
   if (throttled) submask |= 0x4;
   if (extrafunc) submask |= 0x8;
   mask = mask | ( submask << (4*gpio_nr));

   if (gpio_nr >= 4) {                      // AUX
     uint32_t bitmask = 1<<(gpio_nr-4);
     mask = mask & (~bitmask);
     if (altin) mask |= bitmask;
   }

   return true;
}

//----------------------------------------------------------------------------
//! Retrieve attribute flags from GPIO configuration mask.
/*!
 * Retrieves from the GPIO configuration mask \a mask the bits corresponding to 
 * GPIO port \a gpio_nr accoding to the attributes \a riseedge, \a falledge,
 * \a throttled, \a extrafunc and \a altin.
 * For detailed description of attributes see setRocGpioConfig().
 * Returns \c false if gpio_nr is outside range 2 to 7.
 */

bool roc::Gpio::unpackConfig(uint32_t mask, int gpio_nr, 
                             bool& riseedge, bool& falledge,
                             bool& throttled, bool& extrafunc, 
                             bool& altin)
{
   if (gpio_nr <2 || gpio_nr>7) return false;

   uint32_t submask = (mask >> (4*gpio_nr)) & 0xf;

   riseedge  = (submask & 0x1) != 0;
   falledge  = (submask & 0x2) != 0;
   throttled = (submask & 0x4) != 0;
   extrafunc = (submask & 0x8) != 0;

   if ((gpio_nr==2)||(gpio_nr==3)) {
      riseedge = riseedge | falledge;
      falledge = false;
   }

   altin = false;

   if (gpio_nr >= 4) {                      // AUX
     uint32_t bitmask = 1<<(gpio_nr-4);
     altin = (mask & bitmask) != 0;
   }

   return true;
}

//----------------------------------------------------------------------------
//! Add the GPIO interface address set to the address mapping tables.

void roc::Gpio::addAddrMap(roc::Board* board)
{
  board->addRocAddrMapping("ROC_SYNC1_SCALEDOWN", ROC_SYNC1_SCALEDOWN);
  board->addRocAddrMapping("ROC_SYNC1_BAUD_START", ROC_SYNC1_BAUD_START);
  board->addRocAddrMapping("ROC_SYNC1_BAUD1", ROC_SYNC1_BAUD1);
  board->addRocAddrMapping("ROC_SYNC1_BAUD2", ROC_SYNC1_BAUD2);
  board->addRocAddrMapping("ROC_SYNC2_BAUD_START", ROC_SYNC2_BAUD_START);
  board->addRocAddrMapping("ROC_SYNC2_BAUD1", ROC_SYNC2_BAUD1);
  board->addRocAddrMapping("ROC_SYNC2_BAUD2", ROC_SYNC2_BAUD2);
  board->addRocAddrMapping("ROC_SYNC3_BAUD_START", ROC_SYNC3_BAUD_START);
  board->addRocAddrMapping("ROC_SYNC3_BAUD1", ROC_SYNC3_BAUD1);
  board->addRocAddrMapping("ROC_SYNC3_BAUD2", ROC_SYNC3_BAUD2);
  board->addRocAddrMapping("ROC_GPIO_CONFIG", ROC_GPIO_CONFIG);
}

//----------------------------------------------------------------------------


const char* roc::Gpio::name(int n)
{
   static const char* gpio_names[NumGpio] = { "SYNC_M", "SYNC_S0", "SYNC_S1", "AUX0", "AUX1", "AUX2", "AUX3" };

   if ((n<0) || (n>=NumGpio)) return 0;

   return gpio_names[n];
}
