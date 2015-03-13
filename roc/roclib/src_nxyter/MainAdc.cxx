//============================================================================
/*! \file MainAdc.cxx
 *  \author W.F.J.Mueller/GSI, major adaptions
 *  \author Norbert Abel/KIP, original versions
 */
//============================================================================

#include "boost/format.hpp"

#include "nxyter/MainAdc.h"
#include "roc/defines_roc.h"
#include "nxyter/defines_nxyter.h"

using std::endl;
using boost::format;

using roc::Board;

/*!
 * \class nxyter::MainAdc
 * \brief Represents the full interface to a AD9228/AD9219 ADC.
 *
 * This class provides the access to the ROC and SMI interface parts of
 * the main data ADC used on the FEB's. It holds the full address information
 * but no further context.
 */

//----------------------------------------------------------------------------
//! Default constructor
nxyter::MainAdc::MainAdc() :
  roc::Peripheral(),
  fPort(0),
  fMode4nx(0)
{
}


//----------------------------------------------------------------------------
//!Constructor with full addressing path information.
/*!
 * Sets up \c MainAdc object with full addressing information
 * \param board ROC board pointer
 * \param port the \ref glo_roc_port
 * \param mode4nx if \c true configure for \ref glo_feb_feb4nx FEBs
 *
 * \note For \ref glo_feb_feb1nx or glo_feb_feb2nx FEBs the control and data 
 *       interface is on the same port. For \ref glo_feb_feb4nx FEBs the port 
 *       is always 0 (#CON19), but the control interface is on port 
 *       0 (#CON19) while the data interface is at port 1 (#CON20). 
 *       Therefore the \a mode4nx flag is needed to always have a complete 
 *       addressing information available in the object.
 */

nxyter::MainAdc::MainAdc(roc::Board* board, int port, bool mode4nx) :
  roc::Peripheral(board),
  fPort(port),
  fMode4nx(mode4nx)
{
}

//----------------------------------------------------------------------------

nxyter::MainAdc::~MainAdc()
{
}

//----------------------------------------------------------------------------
//! Write to ADC device register with optional readback verification.
/*!
 * Writes the 8bit value \a val to device register \a reg. If \a veri
 * is \c true, the register contents is read back and verified.
 *
 * \param reg device register number
 * \param val value
 * \param veri if \c true readback verification done (default is \c false)
 * \returns see \ref roc_board_opererr
 *
 * \sa getRegister()
 */

int nxyter::MainAdc::setRegister(uint8_t reg, uint8_t val, bool veri)
{
  bool isput[6];
  uint32_t addr[6];
  uint32_t data[6];
  int rc;

  // determine interface addresses depending on port
  int adc_addr = (fPort==CON19) ? ROC_NX_ADC_ADDR   : ROC_NX_ADC_ADDR2;
  int adc_put  = (fPort==CON19) ? ROC_NX_ADC_REG    : ROC_NX_ADC_REG2;
  int adc_get  = (fPort==CON19) ? ROC_NX_ADC_ANSWER : ROC_NX_ADC_ANSWER2;

  isput[0] = true;  addr[0] = adc_addr;  data[0] = reg;
  isput[1] = true;  addr[1] = adc_put;   data[1] = val;
  isput[2] = true;  addr[2] = adc_addr;  data[2] = 255;
  isput[3] = true;  addr[3] = adc_put;   data[3] =   1;
  isput[4] = true;  addr[4] = adc_addr;  data[4] = reg;
  isput[5] = false; addr[5] = adc_get;

  int nop = veri ? 6 : 4;

  rc = board().operGen(isput, addr, data, nop);
  if (veri) {
    if (rc == 0 && val != (data[5]&0xff)) { // check readback value
      rc = Board::kOperVerifyErr;
    }
  }

  return rc;
}

//----------------------------------------------------------------------------
//! Read from ADC device register.
/*!
 * Reads an 8bit value from device register \a reg.
 *
 * \param reg device register number
 * \param val value
 * \returns see \ref roc_board_opererr
 *
 * \sa setRegister()
 */

int nxyter::MainAdc::getRegister(uint8_t reg, uint8_t& val)
{
  int rc;
  uint32_t data;

  // determine interface addresses depending on port
  int adc_addr = (fPort==CON19) ? ROC_NX_ADC_ADDR   : ROC_NX_ADC_ADDR2;
  int adc_get  = (fPort==CON19) ? ROC_NX_ADC_ANSWER : ROC_NX_ADC_ANSWER2;

  rc = board().operPG(adc_addr, reg,        // put address
                      adc_get,  data);      // get answer
  
  val = uint8_t(data);
  return rc;
}

//----------------------------------------------------------------------------
//! Setup test pattern mode \a mode for ADC channel \a ch.
/*!
 * \param ch ADC channel number (0-3)
 * \param mode ADC test pattern mode (0-12)
 * \returns see \ref roc_board_opererr
 */

int nxyter::MainAdc::setTestMode(int ch, uint8_t mode)
{
  int rc;
  uint8_t oldchmask;
  rc = getRegister(kReg_device_index_A, oldchmask);
  if (rc) return Board::operErrBuild(rc, 0);
  rc = setRegister(kReg_device_index_A, (1<<(ch&0x3)), true);
  if (rc) return Board::operErrBuild(rc, 1);
  rc = setRegister(kReg_test_io, mode);
  if (rc) return Board::operErrBuild(rc, 2);
  rc = setRegister(kReg_device_index_A, oldchmask);
  if (rc) return Board::operErrBuild(rc, 3);
  return 0;
}

//----------------------------------------------------------------------------
//! Setup user test pattern \a pat for all ADC channels.
/*!
 * \param pat user test pattern, 16 bits stored, lower 12 bits used
 * \returns see \ref roc_board_opererr
 */

int nxyter::MainAdc::setUserPattern(uint16_t pat)
{
  int rc;
  rc = setRegister(kReg_user_patt1_lsb, uint8_t(pat & 0xff));
  if (rc) return Board::operErrBuild(rc, 0);
  rc = setRegister(kReg_user_patt1_msb, uint8_t((pat>>8) & 0xff));
  if (rc) return Board::operErrBuild(rc, 1);
  return 0;
}

//----------------------------------------------------------------------------
//! Probe whether an ADC exists.
/*!
 * This method tries to test in a reliable but as much as possible 
 * non-intrusive way whether an ADC chip is accessible under the port setup
 * for the object. The probe procedure uses the four user pattern registers 
 * of the ADC and performs four steps
 * \li read these registers
 * \li write distinct values into these registers
 * \li read back and check the distinct values
 * \li restore the initial settings (saved in first step)
 *
 * The method aborts and returns with a non-zero return code when any of
 * the above operation or tests fail. In case of success the ADC chip
 * is in the same state as before.
 *
 * The probe is \e mostly non-intrusive because only registers used in test
 * modes are used, and should therefore not disput normal operation.
 *
 * \returns 0 on success and non-zero if any step fails. See 
 *    \ref roc_board_opererr for details. The index refers to the 16 get and
 *    set operations and thus allows to precisely pinpoint at what point the
 *    probe aborted.
 */

int nxyter::MainAdc::probe()
{
  int rc;
  uint8_t regs[4] = {kReg_user_patt1_lsb, kReg_user_patt1_msb,
                     kReg_user_patt2_lsb, kReg_user_patt2_msb};
  
  uint8_t save[4];
  uint8_t back[4];
  uint8_t patt[4] = {0xde, 0xed, 0xbe, 0xef}; // 'deadbeaf' ...

  // read and save registers
  for (int i=0; i<4; i++) {
    rc = getRegister(regs[i], save[i]);
    if (rc) return Board::operErrBuild(rc, i);
  }

  // write pattern
  for (int i=0; i<4; i++) {
    rc = setRegister(regs[i], patt[i]);
    if (rc) return Board::operErrBuild(rc, 4+i);
  }

  // write and check pattern
  for (int i=0; i<4; i++) {
    rc = getRegister(regs[i], back[i]);
    if (rc) return Board::operErrBuild(rc, 8+i);
    if (back[i] != patt[i]) 
      return Board::operErrBuild(Board::kOperVerifyErr, 8+i);
  }

  // restore registers
  for (int i=0; i<4; i++) {
    rc = setRegister(regs[i], save[i], true);
    if (rc) return Board::operErrBuild(rc, 12+i);
  }

  return 0;
}

//----------------------------------------------------------------------------
//! Set the ADC registers to default values suitable for data taking.
/*!
 * Rewrite all ADC registers which determine operation to the power-up 
 * defaults. Only exception is the \c serial_control register, which is
 * set for 12 bit operation (rather than nominal bitstream). This allows
 * to use also 10 bit ADC's, some early FEB actually did so.
 *
 * In particular:
 * \li all test pattern modes are canceled, measured ADC values will
 *     transmitted (channel mask is first set to b1111, that the test_io
 *     register is set to 0)
 * \li all channels are powered up
 *
 * All writes are verified, so an error return is truely bad news.
 * 
 * \returns see \ref roc_board_opererr
 */

int nxyter::MainAdc::setToDefault()
{
  int rc;
  rc = setRegister(kReg_chip_port_config, 0x18, true);
  if (rc) return Board::operErrBuild(rc, 0);
  rc = setRegister(kReg_device_index_A,   0x0f, true);
  if (rc) return Board::operErrBuild(rc, 1);
  rc = setRegister(kReg_modes,            0x00, true);
  if (rc) return Board::operErrBuild(rc, 2);
  rc = setRegister(kReg_clock,            0x01, true);
  if (rc) return Board::operErrBuild(rc, 3);
  rc = setRegister(kReg_test_io,          0x00, true);
  if (rc) return Board::operErrBuild(rc, 4);
  rc = setRegister(kReg_output_mode,      0x00, true);
  if (rc) return Board::operErrBuild(rc, 5);
  rc = setRegister(kReg_output_adjust,    0x00, true);
  if (rc) return Board::operErrBuild(rc, 6);
  rc = setRegister(kReg_output_phase,    0x03, true);
  if (rc) return Board::operErrBuild(rc, 7);
  rc = setRegister(kReg_serial_control,  0x03, true); // 12 bit mode !!
  if (rc) return Board::operErrBuild(rc, 8);
  rc = setRegister(kReg_serial_ch_stat,  0x00, true);
  if (rc) return Board::operErrBuild(rc, 9);
  return 0;
}

//----------------------------------------------------------------------------
//! Read ADC registers and print to stream \a os.
/*!
 * Produces a nicely formatted listing of all ADC settings with the value
 * given in hex, decimal and also binary representation. An ADC with
 * default settings setup by setToDefault() will produce
\verbatim
Reg( 0) chip_port_config: 0x18  24   0 0 0 1 1 0 0 0
Reg( 1) chip_id         : 0x02   2   0 0 0 0 0 0 1 0
Reg( 2) chip_grade      : 0x11  17   0 0 0 1 0 0 0 1
Reg( 5) device_index_A  : 0x0f  15   0 0 0 0 1 1 1 1
Reg( 8) modes           : 0x00   0   0 0 0 0 0 0 0 0
Reg( 9) clock           : 0x01   1   0 0 0 0 0 0 0 1
Reg(13) test_io         : 0x00   0   0 0 0 0 0 0 0 0
Reg(20) output_mode     : 0x00   0   0 0 0 0 0 0 0 0
Reg(21) output_adjust   : 0x00   0   0 0 0 0 0 0 0 0
Reg(22) output_phase    : 0x03   3   0 0 0 0 0 0 1 1
Reg(25) user_patt1_lsb  : 0x00   0   0 0 0 0 0 0 0 0
Reg(26) user_patt1_msb  : 0x00   0   0 0 0 0 0 0 0 0
Reg(27) user_patt2_lsb  : 0x00   0   0 0 0 0 0 0 0 0
Reg(28) user_patt2_msb  : 0x00   0   0 0 0 0 0 0 0 0
Reg(33) serial_control  : 0x03   3   0 0 0 0 0 0 1 1
Reg(34) serial_ch_stat  : 0x00   0   0 0 0 0 0 0 0 0
\endverbatim
*/

void nxyter::MainAdc::printRegisters(std::ostream& os)
{
  static format fmt("Reg(%|2|) %|-16|: 0x%|02x| %|3|");
  for (int i=0; i<35; i++) {
    const char* regname = nxyter::MainAdc::registerName(i);
    if (regname[0] == 0) continue;
    uint8_t val;
    getRegister(i,val);
    os << fmt % i % regname % uint16_t(val) % uint16_t(val);
    os << "  ";
    for (int j=7; j>=0; j--) os << ((val & (1<<j)) ? " 1" : " 0");
    os << endl;
  }
}

//----------------------------------------------------------------------------
//! Setup association of nXYTER chips and ADC channels.
/*!
 * The ROC can serve one or two FEBs, each with 4 channel ADC, and a total
 * or four nXYTER chips. An ADC channel mux allows to associate every
 * nXYTER data channel with any of the 4 possible ADC data channels of the
 * associated port. This method controls the setting of this multiplexer.
 * 
 * \param nxnum  see \ref glo_nxyter_nxnum
 * \param val    ADC channel. Only 2 LSB are used, valid values are
 *               \li 0 use data from connector pin A17/B17
 *               \li 1 use data from connector pin A18/B18
 *               \li 2 use data from connector pin A20/B20
 *               \li 3 use data from connector pin A21/B21
 * \returns see \ref roc_board_opererr, or -1 if \a nxnum out-of-range
 *
 * \note The ADC data for \a nxnum = 0 and 1 is taken from port 0 (#CON19) 
 * for \ref glo_feb_feb1nx and \ref glo_feb_feb2nx FEBs and from 
 * port 1 (#CON20) for \ref glo_feb_feb4nx FEBs. The ADC data for
 * \a nxnum = 2 and 3 is always taken from port 1 (#CON20).
 */

int nxyter::MainAdc::setChannelMux(int nxnum, uint32_t val)
{
   int rc = -1;
   if (nxnum==0) rc = board().put(ROC_NX_ADC_PORT_SELECT1, val);
   if (nxnum==1) rc = board().put(ROC_NX_ADC_PORT_SELECT2, val);
   if (nxnum==2) rc = board().put(ROC_NX_ADC_PORT_SELECT3, val);
   if (nxnum==3) rc = board().put(ROC_NX_ADC_PORT_SELECT4, val);
   return rc;
}

//----------------------------------------------------------------------------
//! Set delay of ADC data relative to nXYTER data.
/*!
 * \param nxnum  see \ref glo_nxyter_nxnum
 * \param val    delay of ADC data relative to nXYTER data in 250MHz clocks
 * \returns see \ref roc_board_opererr, or -1 if \a nxnum out-of-range
 * \sa getChannelLatency()
 */

int nxyter::MainAdc::setChannelLatency(int nxnum, uint32_t val)
{
   int rc = -1;
   if (nxnum==0) rc = board().put(ROC_NX_ADC_LATENCY1, val);
   if (nxnum==1) rc = board().put(ROC_NX_ADC_LATENCY2, val);
   if (nxnum==2) rc = board().put(ROC_NX_ADC_LATENCY3, val);
   if (nxnum==3) rc = board().put(ROC_NX_ADC_LATENCY4, val);
   return rc;
}

//----------------------------------------------------------------------------
//! Returns delay of ADC data relative to nXYTER data.
/*!
 * \param nxnum see \ref glo_nxyter_nxnum
 * \param val    delay of ADC data relative to nXYTER data in 250MHz clocks
 * \returns see \ref roc_board_opererr, or -1 if \a nxnum out-of-range
 * \sa getChannelLatency()
 */

int nxyter::MainAdc::getChannelLatency(int nxnum, uint32_t& val)
{
   int rc = -1;
   if (nxnum==0) rc = board().get(ROC_NX_ADC_LATENCY1, val);
   if (nxnum==1) rc = board().get(ROC_NX_ADC_LATENCY2, val);
   if (nxnum==2) rc = board().get(ROC_NX_ADC_LATENCY3, val);
   if (nxnum==3) rc = board().get(ROC_NX_ADC_LATENCY4, val);
   return rc;
}

//----------------------------------------------------------------------------
//! Set the relative phase between the nXYTER and the ADC clock
/*!
 * The ADC clock can be shifted relative to the nXYTER clock to adjust the
 * proper data sampling timing on the FEB. The clock shift is implemented
 * on the ROC in a two stage fashion, leading to two internal setup
 * parameters. This method takes a desired \a delay in units of ns as input,
 * calculates these parameters, and calls the low level methods
 * setClockDelaySrInit() and setClockDelayBufg() to transmit them to the ROC.
 *
 * \note The mapping of \a delay to the hardware settings is done such that
 *       a larger value of \a delay will cause a later sampling of the
 *       nXYTER data by the ADC. This way the time direction used by this
 *       method and by setChannelLatency() is the same.
 *
 * \param delay desired delay on units of 1 ns
 * \returns see \ref roc_board_opererr
 */

int nxyter::MainAdc::setClockDelay(uint32_t delay)
{
   uint32_t sr_init;
   uint32_t bufg_select;

   calcClockDelayRegs(delay, sr_init, bufg_select);

   board().Debug(1, "SR_INIT:     %8.8x", sr_init);
   board().Debug(1, "BUFG_SELECT: %8.8x", bufg_select);

   int rc;
   rc = setClockDelaySrInit(sr_init);
   if (rc) return rc;
   rc = setClockDelayBufg(bufg_select);
   if (rc) return Board::operErrBuild(rc, 1);
   return 0;
}


//----------------------------------------------------------------------------
//! Calculates registers values for specified ADC clock delay

void nxyter::MainAdc::calcClockDelayRegs(uint32_t delay, uint32_t& sr_init, uint32_t& bufg_select)
{
   delay = delay & 0x1f;                    // only 5 bits significant
   delay = 31 - delay;                      // mirror time axis

   bufg_select = delay & 0x01;
   delay = delay >> 1;

   sr_init = 255;
   sr_init = sr_init << delay;

   sr_init += sr_init >> 16;
   sr_init &= 0xffff;
}

//----------------------------------------------------------------------------
//! Finds clock delay value, which is correspond to specified regs. Returns 0xffff if fails

uint32_t nxyter::MainAdc::findClockDelay(uint32_t sr_init, uint32_t bufg_select)
{
   for (uint32_t delay=0; delay<=31; delay++) {
      uint32_t sr_init1;
      uint32_t bufg_select1;

      calcClockDelayRegs(delay, sr_init1, bufg_select1);

      if ((sr_init1==sr_init) && (bufg_select1==bufg_select)) return delay;
   }

   return 0xffff;
}

//----------------------------------------------------------------------------
//! Set nXYTER to ADC clock phase, 'SR_INIT' part
/*! 
 * Low level method to setup one of of two parameters controling the relative 
 * phase between the nXYTER and the ADC clock. At the user level always call 
 * setClockDelay().
 *
 * The 'SR_INIT' setting determines the initial value of the ADC-Clock 
 * generating shift register.
 *
 * \param val new initial shift register setting (16 bit)
 * \returns see \ref roc_board_opererr
 *
 * \sa getClockDelaySrInit()
 */

int nxyter::MainAdc::setClockDelaySrInit(uint32_t val)
{
   return board().put((fPort==CON19) ? ROC_NX_SR_INIT : ROC_NX_SR_INIT2, val);
}

//----------------------------------------------------------------------------
//! Returns 'SR_INIT' part of nXYTER to ADC clock phase adjust.
/*!
 * \param val initial shift register setting (16 bit)
 * \returns see \ref roc_board_opererr
 *
 * \sa setClockDelaySrInit()
 */

int nxyter::MainAdc::getClockDelaySrInit(uint32_t& val)
{
   return board().get((fPort==CON19) ? ROC_NX_SR_INIT : ROC_NX_SR_INIT2, val);
}

//----------------------------------------------------------------------------
//! Set nXYTER to ADC clock phase, 'BUFG' part
/*! 
 * Low level method to setup one of of two parameters controling the relative 
 * phase between the nXYTER and the ADC clock. At the user level always call 
 * setClockDelay().
 *
 * The 'BUFG' setting determines whether the 250 MHz clock is used directly
 * (\a val = 0) or a clock shifted by 90 degree is used (\a val = 1). This
 * changes the relative clock phase between nXYTER and ADC on the FEB by 1 ns.
 *
 * \param val new clock phase mux setting (1 bit)
 * \returns see \ref roc_board_opererr
 *
 * \sa getClockDelayBufg()
 */

int nxyter::MainAdc::setClockDelayBufg(uint32_t val)
{
   return board().put((fPort==CON19) ? ROC_NX_BUFG_SELECT : ROC_NX_BUFG_SELECT2, val);
}


//----------------------------------------------------------------------------
//! Returns 'BUFG' part of nXYTER to ADC clock phase adjust.
/*!
 * \sa setClockDelayBufg()
 */

int nxyter::MainAdc::getClockDelayBufg(uint32_t& val)
{
   return board().get((fPort==CON19) ? ROC_NX_BUFG_SELECT : ROC_NX_BUFG_SELECT2, val);
}

//----------------------------------------------------------------------------
//! Direct access to ADC values.
/*!
 * In normal operation the ADC values are only inspected when a nXYTER
 * data valid is processed, in this case the ADC reading is placed as
 * part of the hit message into the data stream. For tests it is sometimes
 * desireable to directly read the ADC values. This methods provides this
 * access, and returns the current ADC value for channel \a num.
 *
 * Note that the ADC values are updated at 32 MHz. Note also, that in
 * quiecent state the nXYTER tends to produce a reading close to ADC 
 * midrange, completely independent of the nXYTER baseline settings.
 *
 * \param num ADC channel number (0-3)
 * \param val ADC data (12 bit)
 * \returns see \ref roc_board_opererr, or -1 if \a num out-of-range
 */

int nxyter::MainAdc::getAdcDirect(int num, uint32_t& val)
{
   // Note: for 1nx and 2nx the fPort denotes the control and the data interface
   //       for 4nx fPort=CON19(0) denotes the control interface, while the data
   //       interface is on CON20(1). This is handled by the line below.
   int port = fMode4nx ? CON20 : fPort;

   int rc = -1;
   if (port==CON19) {
     if (num==ADC_PORT_A) rc = board().get(ROC_NX_ADC_DIRECT_1a, val);
     if (num==ADC_PORT_B) rc = board().get(ROC_NX_ADC_DIRECT_1b, val);
     if (num==ADC_PORT_C) rc = board().get(ROC_NX_ADC_DIRECT_1c, val);
     if (num==ADC_PORT_D) rc = board().get(ROC_NX_ADC_DIRECT_1d, val);
   } else {
     if (num==ADC_PORT_A) rc = board().get(ROC_NX_ADC_DIRECT_2a, val);
     if (num==ADC_PORT_B) rc = board().get(ROC_NX_ADC_DIRECT_2b, val);
     if (num==ADC_PORT_C) rc = board().get(ROC_NX_ADC_DIRECT_2c, val);
     if (num==ADC_PORT_D) rc = board().get(ROC_NX_ADC_DIRECT_2d, val);
   }

   return rc;
}

//----------------------------------------------------------------------------
//! Returns name for register \a reg (or "" if undefined).

const char* nxyter::MainAdc::registerName(int reg) 
{
  static const char* names[35] = {"chip_port_config",   // reg 0x00
                                  "chip_id",            // reg 0x01
                                  "chip_grade",         // reg 0x02
                                  "",                   // reg 0x03
                                  "",                   // reg 0x04
                                  "device_index_A",     // reg 0x05
                                  "",                   // reg 0x06
                                  "",                   // reg 0x07
                                  "modes",              // reg 0x08
                                  "clock",              // reg 0x09
                                  "",                   // reg 0x0a
                                  "",                   // reg 0x0b
                                  "",                   // reg 0x0c
                                  "test_io",            // reg 0x0d
                                  "",                   // reg 0x0e
                                  "",                   // reg 0x0f
                                  "",                   // reg 0x10
                                  "",                   // reg 0x11
                                  "",                   // reg 0x12
                                  "",                   // reg 0x13
                                  "output_mode",        // reg 0x14
                                  "output_adjust",      // reg 0x15
                                  "output_phase",       // reg 0x16
                                  "",                   // reg 0x17
                                  "",                   // reg 0x18
                                  "user_patt1_lsb",     // reg 0x19
                                  "user_patt1_msb",     // reg 0x1a
                                  "user_patt2_lsb",     // reg 0x1b
                                  "user_patt2_msb",     // reg 0x1c
                                  "",                   // reg 0x1d
                                  "",                   // reg 0x1e
                                  "",                   // reg 0x1f
                                  "",                   // reg 0x20
                                  "serial_control",     // reg 0x21
                                  "serial_ch_stat"      // reg 0x22
  };
  
  if (reg == 255) return "device_update";
  if (reg < 0 || reg >=35) return "";
  return names[reg];
}
