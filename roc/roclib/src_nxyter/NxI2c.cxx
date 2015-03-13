//============================================================================
/*! \file NxI2c.cxx
 *  \author W.F.J.Mueller/GSI, based on earlier work of Norbert Abel/KIP
 */
//============================================================================

#include "nxyter/NxI2c.h"

using roc::Board;

/*!
 * \class nxyter::NxI2c
 * \brief Represents the I2C interface of an nXYTER.
 *
 * This class represents the I2C interface of an nXYTER. It is derived from
 * roc::I2cDevice and provides
 * \li a probe() method to check whether an xXYTER is connected
 * \li methods to write/read a complete register context to/from an nXYTER,
 *     see setContext() and getContext()
 * \li a method to access the error counters, see getCounters()
 * \li a convenience method to manipulate configuration bits, 
 *       see setTestModes() 
 * \li some convenient methods to write/read the main registers and the
 *       trim-daq (shift) register.
 * 
 * \note The nxyter::NxContext class represents the nXYTER register context, 
 *       while this class mediates the access to the nXYTER.
 */

//----------------------------------------------------------------------------
//! Constructor with full addressing path information.
/*!
 * Sets up \c NxI2c object with full addressing information
 * \param board ROC board pointer
 * \param port the \ref glo_roc_port (equivalent to the I2C bus interface port)
 * \param addr the I2C device slave address
 */

nxyter::NxI2c::NxI2c(roc::Board* board, int port, 
                     uint8_t addr) :
  roc::I2cDevice(board, port, addr)
{
}

//----------------------------------------------------------------------------

nxyter::NxI2c::~NxI2c()
{
}

//----------------------------------------------------------------------------
//! Write the part or full context to nXYTER with optional readback verify.
/*!
 * Updates nXYTER registers with the values given by the context \a cntx.
 * The \a domask allows to enable separately whether the mask, core or
 * trim part of the register set is written.
 * If \a veri is \c true, the context is readback and compared.
 * 
 * \param cntx nXYTER register context container
 * \param domask one or several of the mask bits nxyter::kDoMask, 
 *     nxyter::kDoCore, or nxyter::kDoTrim
 * \param veri if \c true readback verification done (default is \c true)
 * \returns see \ref roc_board_opererr.
 *
 * \note for the readback verification of a write of the mask register part
 *   the test trigger mode will temporarily disabled in case it is active.
 *   This might interfere with data taking and should therefore be avoided
 *   when DAQ is active.
 *
 * \sa getContext()
 */

int nxyter::NxI2c::setContext(NxContext& cntx, int domask, bool veri)
{
  int rc;
  NxContext data;
  
  if (domask & kDoMask) {
    rc = setRegMask(cntx.dataRegisterMain());
    if (rc) return rc;
  }
  if (domask & kDoCore) {
    rc = setRegCore(cntx.dataRegisterMain());
    if (rc) return Board::operErrBuildInc(rc, 16);
  }
  
  if (domask & kDoTrim) {
    rc = setRegTrim(cntx.dataRegisterTrim());
    if (rc) return Board::operErrBuildInc(rc, 128);
  }

  if (veri) {
    rc = getContext(data, domask);
    if (rc) return Board::operErrBuildInc(rc, 1000);
    if (!cntx.equal(data, domask))
      return Board::operErrBuild(Board::kOperVerifyErr,2000);
  }
  
  return 0;
}

//----------------------------------------------------------------------------
//! Read the part or full context from nXYTER.
/*!
 * The \a domask allows to enable separately whether the mask, core or
 * trim part of the register set is written.
 * \param cntx nXYTER register context container
 * \param domask one or several of the mask bits nxyter::kDoMask, 
 *     nxyter::kDoCore, or nxyter::kDoTrim
 * \returns see \ref roc_board_opererr
 *
 * \note the two nXYTER error counters are not accessed, to read them use the
 *   getCounters() method.
 * \note for the read of the mask register part the test trigger mode will 
 *   temporarily disabled in case it is active. This might interfere with 
 *   data taking and should therefore be avoided when DAQ is active.
 *
 * \sa setContext()
 */

int nxyter::NxI2c::getContext(NxContext& cntx, int domask)
{
  int rc;

  if (domask & kDoMask) {
    rc = getRegMask(cntx.dataRegisterMain());
    if (rc) return rc;
  }
  if (domask & kDoCore) {
    rc = getRegCore(cntx.dataRegisterMain());
    if (rc) return Board::operErrBuildInc(rc, 16);
  }
  if (domask & kDoTrim) {
    rc = getRegTrim(cntx.dataRegisterTrim());
    if (rc) return Board::operErrBuildInc(rc, 128);
  }
  
  return 0;
}

//----------------------------------------------------------------------------
//! Read and clear the error counters from nXYTER.
/*!
 * Returns the values of the two error counters of the nXYTER. Note that they
 * are read-only and the read access is implemened as <em>read and clear</em>.
 * \param overflow Overflow counter
 * \param tokenmiss Missing token counter
 * \returns see \ref roc_board_opererr
 */

int nxyter::NxI2c::getCounters(uint16_t& overflow, uint16_t& tokenmiss)
{
  int rc;
  uint8_t data[4];

  overflow  = 0;
  tokenmiss = 0;

  rc = getRegisterArray(34, data, 4);       // read 34 - 37
  if (rc) return rc;

  overflow  = data[0] + uint16_t(data[1])<<8; // reg 34/35
  tokenmiss = data[2] + uint16_t(data[3])<<8; // reg 36/37

  return 0;
}

//----------------------------------------------------------------------------
//! Setup <em>test pulser</em> and <em>test trigger</em> mode.
/*!
 * The <em>test pulse</em> input of the nXYTER can be used to
 * \li fire a test pulse, coupled into a quarter of the channels
 * \li force a hit on all channels
 *
 * This defines four operation modes, all of which have useful applications.
 * This method sets the kNxC0TestPulsEnable and kNxC0TestTrigEnable bits
 * in register kNxRegConfig0 according to the arguments \a testpuls and
 * \a testtrig. The test pulse polarity, controlled by bit 
 * kNxC0TestPulsPolarity, is set according to the already configured front-end
 * polarity, given by kNxC1FrontEndPolarity in register kNxRegConfig1.
 *
 * The four possible choices of channel group receiving the test pulse are
 * selected by \a calselect and written into the kNxC1CalibSelectMask bits
 * of register kNxRegConfig1.
 *
 * \param testpuls determines whether <em>test pulser</em> mode is enabled
 * \param testtrig determines whether <em>test trigger</em> mode is enabled
 * \param calselect determines which channel group receives the test pulse. Only
 *          the 2 LSBs are used: 
 *          \li 0 will select channels 0,4,8,...,124
 *          \li 1 will select channels 1,5,9,...,125
 *          \li 2 will select channels 2,6,10,...,126
 *          \li 3 will select channels 3,7,11,...,127
 * \returns see \ref roc_board_opererr
 */

int nxyter::NxI2c::setTestModes(bool testpuls, bool testtrig, 
                                int calselect)
{
  int rc;
  uint8_t conf0;
  uint8_t conf1;
  rc = getRegister(kNxRegConfig0, conf0);
  if (rc) return Board::operErrBuild(rc, 0);
  rc = getRegister(kNxRegConfig1, conf1);
  if (rc) return Board::operErrBuild(rc, 1);

  if (testpuls) {
    conf0 |=  kNxC0TestPulsEnable;
  } else {
    conf0 &= ~kNxC0TestPulsEnable;
  }

  if (testtrig) {
    conf0 |=  kNxC0TestTrigEnable;
  } else {
    conf0 &= ~kNxC0TestTrigEnable;
  }
  
  if (conf1 & kNxC1FrontEndPolarity) {
    conf0 |=  kNxC0TestPulsPolarity;
  } else {
    conf0 &= ~kNxC0TestPulsPolarity;
  }
  
  conf1 &= ~kNxC1CalibSelectMask;
  conf1 |= uint8_t(calselect) & kNxC1CalibSelectMask;
  
  rc = setRegister(kNxRegConfig0, conf0, true);
  if (rc) return Board::operErrBuild(rc, 2);
  rc = setRegister(kNxRegConfig1, conf1, true);
  if (rc) return Board::operErrBuild(rc, 3);
  
  return 0;
}

//----------------------------------------------------------------------------
//! Probe whether an nXYTER exists.
/*!
 * This method tries to test in a reliable but as much as possible 
 * non-intrusive way whether an nXYTER chip is accessible under the
 * port and slave address setup for the object. The probe procedure
 * uses the registers
 * \li 24: test pulse amplitue
 * \li 38: test pulse delay
 * \li 39: test trigger delay
 *
 * and performs four steps
 * \li read these registers
 * \li write distinct values into these registers
 * \li read back and check the distinct values
 * \li restore the initial settings (saved in first step)
 *
 * The method aborts and returns with a non-zero return code when any of
 * the above operation or tests fail. In case of success the nXYTER chip
 * is in the same state as before.
 *
 * The probe is \e mostly non-intrusive because only registers used in test
 * pulse/trigger modes are used, and should therefore not disput normal
 * operation.
 *
 * \note the obvious simple probe, using the mask registers, doesn't work
 *   because a read to register 0-15 does not return the mask registers when
 *   the chip is in test trigger mode ! Therefore the registers 24,38, and
 *   39 are used.
 *
 * \returns 0 on success and non-zero if any step fails. See 
 *    \ref roc_board_opererr for details. The index refers to the 12 get and
 *    set operations and thus allows to precisely pinpoint at what point the
 *    probe aborted.
 */

int nxyter::NxI2c::probe()
{
  int rc(0), res(0);
  uint8_t regs[3] = {kNxRegcal, kNxRegdelayTestPuls, kNxRegdelayTestTrig};
  uint8_t save[3];
  uint8_t back[3];
  uint8_t patt[3] = {0x55, 0xaa, 0x5f};

  // read and save registers
  for (int i=0; i<3; i++) {
    rc = getRegister(regs[i], save[i]);
    if (rc) return Board::operErrBuild(rc, i);
  }

  // write pattern
  for (int i=0; i<3; i++) {
    rc = setRegister(regs[i], patt[i]);
    if (rc && (res==0)) res = Board::operErrBuild(rc, 3+i);
  }

  // read and check pattern
  for (int i=0; i<3; i++) {
    rc = getRegister(regs[i], back[i]);
    if (rc && (res==0)) res = Board::operErrBuild(rc, 6+i);
    if ((back[i] != patt[i]) && (res==0))
      res = Board::operErrBuild(Board::kOperVerifyErr, 6+i);
  }

  // restore registers in any case
  for (int i=0; i<3; i++)
    setRegister(regs[i], save[i], true);

  return res;
}

//----------------------------------------------------------------------------
//! Write default values into nXYTER registers.
/*!
 * Constructs a context with default values with and writes these values 
 * into the associated nXYTER. See documentation of 
 * NxContext::setToDefault() for description of the parameters
 * \a ispos, \a maskon, and \a poweron.
 * \returns see \ref roc_board_opererr
 */

int nxyter::NxI2c::setToDefault(bool ispos, int maskon, int poweron)
{
  nxyter::NxContext cntx;
  cntx.setToDefault(ispos, maskon, poweron);
  return setContext(cntx);
}

//----------------------------------------------------------------------------
//! Read nXYTER registers and print to stream \a os.
/*!
 * Reads full register context with getContext() and prints them with
 * NxContext::print(). For details consult the documentation of these
 * two methods.
 */

void nxyter::NxI2c::printRegisters(std::ostream& os, int domask)
{
  nxyter::NxContext cntx;
  int rc = getContext(cntx, domask);
  if (rc) {
    os << std::dec;
    os << "Failed to read nx context port: " << getPortNumber()
       << " addr: " << std::dec << uint16_t(getSlaveAddr())
       << " " << Board::operErrToString(rc)
       << std::endl;
  } else {
    cntx.print(os, domask);
  }
}

//----------------------------------------------------------------------------
//! Write the mask nXYTER register set.
/*!
 * Writes the mask registers.
 * \param val array with 46 values
 * \returns see \ref roc_board_opererr
 * \note consider to use setContext().
 * \sa getRegMask()
 */

int nxyter::NxI2c::setRegMask(const uint8_t *val)
{
  return setRegisterArray(0, val, 16);    // regs  0 - 15
}

//----------------------------------------------------------------------------
//! Write the core nXYTER register set.
/*!
 * Writes the core registers, this is all except the mask and trim registers
 * and the read-only overflow and missing token counter registers, thus 
 * 16-29,32,33,38,39,43-45.
 * \param val array with 46 values
 * \returns see \ref roc_board_opererr
 * \sa getRegCore()
 * \note consider to use setContext().
 */

int nxyter::NxI2c::setRegCore(const uint8_t *val)
{
  int rc;

  rc = setRegisterArray(16, val+16, 14);    // regs 16 - 29
  if (rc) return rc;
  rc = setRegisterArray(32, val+32,  2);    // regs 32 - 33
  if (rc) return Board::operErrBuildInc(rc, 14);
  rc = setRegisterArray(38, val+38,  2);    // regs 38 - 39
  if (rc) return Board::operErrBuildInc(rc, 16);
  rc = setRegisterArray(43, val+43,  3);    // regs 43 - 45
  if (rc) return Board::operErrBuildInc(rc, 19);
  return 0;
}

//----------------------------------------------------------------------------
//! Write 129 8 bit data values to the trim-daq register 42.
/*!
 * \param val array with 129 values, [0..127] holds channel 0 to 127 
 *              and [128] the test channel value.
 * \returns see \ref roc_board_opererr
 *
 * \note consider to use setContext().
 * \note \a val and nXYTER storage order differ, the test channel data
 *   is in \a val[128] while it is the first value to be written into the chip.
 * \sa getRegTrim()
 */

int nxyter::NxI2c::setRegTrim(const uint8_t *val)
{
  int rc;
  rc = setRegister(kNxRegTrimDAQPower, val[128]);
  if (rc) return Board::operErrBuild(rc, 128);
  rc = setMailboxRegister(kNxRegTrimDAQPower, val, 128);
  if (rc) return Board::operErrBuildInc(rc, 1);
  return 0;
}

//----------------------------------------------------------------------------
//! Read the mask nXYTER register set.
/*!
 * Reads the mask registers. 
 * \param val array with 46 values
 * \returns see \ref roc_board_opererr
 * \note consider to use getContext().
 * \note for the read of the mask register part the test trigger mode will 
 *   temporarily disabled in case it is active. This might interfere with 
 *   data taking and should therefore be avoided when DAQ is active. To
 *   retrieve the mask bits directly use getRegisterArray().
 * \sa setRegMask()
 */

int nxyter::NxI2c::getRegMask(uint8_t *val)
{
  int rc;
  uint8_t conf0;
  rc = getRegister(kNxRegConfig0, conf0);
  if (rc) return rc;
  if (conf0 & kNxC0TestTrigEnable) {        // temporary test trigger disable
    rc = setRegister(kNxRegConfig0, (conf0 & ~kNxC0TestTrigEnable));
    if (rc) return rc;
  }

  rc = getRegisterArray(0, val, 16);    // regs  0 - 15
  if (rc) return rc;

  if (conf0 & kNxC0TestTrigEnable) {        // reenable test trigger
    rc = setRegister(kNxRegConfig0, conf0);
    if (rc) return rc;
  }
  
  return 0;
}

//----------------------------------------------------------------------------
//! Read the core nXYTER register set.
/*!
 * Reads the core registers, this is all except the mask and trim registers
 * and the read-only overflow and missing token counter registers, thus 
 * 16-29,32,33,38,39,43-45.
 * \param val array with 46 values
 * \returns see \ref roc_board_opererr
 * \sa setRegCore()
 * \note consider to use getContext().
 */

int nxyter::NxI2c::getRegCore(uint8_t *val)
{
  int rc;

  rc = getRegisterArray(16, val+16, 14);    // regs 16 - 29
  if (rc) return rc;
  rc = getRegisterArray(32, val+32,  2);    // regs 32 - 33
  if (rc) return Board::operErrBuildInc(rc, 14);
  rc = getRegisterArray(38, val+38,  2);    // regs 38 - 39
  if (rc) return Board::operErrBuildInc(rc, 16);
  rc = getRegisterArray(43, val+43,  3);    // regs 43 - 45
  if (rc) return Board::operErrBuildInc(rc, 19);
  return 0;
}

//----------------------------------------------------------------------------
//! Read 129 8 bit data values from the trim-daq register 42.
/*!
 * \param val array with 129 values, [0..127] holds channel 0 to 127 
 *              and [128] the test channel value.
 * \returns see \ref roc_board_opererr
 * \bug This implementation is currently not atomic ! Two concurrent executions
 *      of this method will destroy the content of the nXYTER trim-daq
 *      register !
 *
 * \note consider to use getContext().
 * \note \a val and nXYTER storage order differ, the test channel data
 *   is in \a val[128] while it is the first value to be read from the chip.
 * \sa setRegTrim()
 */

int nxyter::NxI2c::getRegTrim(uint8_t *val)
{
  int rc;

  // The trimdaq register is a 129 stage shift register
  // Reg 42 must be read and rewritten 129 times to retrieve the contents
  // The code below uses setRegisterVerify() to combine a write and read
  // into a single transaction and gain thus some speed.

  rc = getRegister(kNxRegTrimDAQPower, val[128]);
  if (rc) return Board::operErrBuild(rc, 128);

  uint8_t valrewrite = val[128];

  for (int i=0; i<128; i++) {
    rc = setRegisterVerify(kNxRegTrimDAQPower, valrewrite, val[i]);
    valrewrite = val[i];
    if (rc) return Board::operErrBuild(rc, i);
  }
  rc = setRegister(kNxRegTrimDAQPower, valrewrite);
  if (rc) return Board::operErrBuild(rc, 127);
  return 0;
}

//----------------------------------------------------------------------------
//! Returns name for register \a reg (or "" if undefined).

const char* nxyter::NxI2c::registerName(int reg) 
{
  static const char* names[46] = {"mask_000_007", // reg  0
                                  "mask_008_015",
                                  "mask_016_023",
                                  "mask_024_031",
                                  "mask_032_039",
                                  "mask_040_047",
                                  "mask_048_055",
                                  "mask_056_063",
                                  "mask_064_071", // reg  7
                                  "mask_072_079",
                                  "mask_080_087",
                                  "mask_088_095",
                                  "mask_096_103",
                                  "mask_104_111",
                                  "mask_112_119",
                                  "mask_120_127",
                                  "Vcg",            // reg 16
                                  "Icgfoll",        // reg 17
                                  "Vth",            // reg 18
                                  "Vbfb",           // reg 19
                                  "VbiasF",         // reg 20
                                  "VbiasS",         // reg 21
                                  "VbiasS2",        // reg 22
                                  "Vcm",            // reg 23
                                  "cal",            // reg 24
                                  "iCOMP",          // reg 25
                                  "iDUR",           // reg 26
                                  "iINV",           // reg 27
                                  "iPDH",           // reg 28
                                  "iTWC",           // reg 29
                                  "",               // reg 30
                                  "",               // reg 31
                                  "Config0",        // reg 32
                                  "Config1",        // reg 33
                                  "OverflowLSB",  // reg 34
                                  "OverflowMSB",  // reg 35
                                  "MissTokenLSB", // reg 36
                                  "MissTokenMSB", // reg 37
                                  "delayTestPuls",// reg 38
                                  "delayTestTrig",// reg 39
                                  "",               // reg 40
                                  "",               // reg 41
                                  "TrimDAQ-Power",// reg 42
                                  "delayClock1",  // reg 43
                                  "delayClock2",  // reg 44
                                  "delayClock3"   // reg 45
  };
  
  if (reg < 0 || reg >=46) return "";
  return names[reg];
}

//----------------------------------------------------------------------------
//! Returns descriptive string for configuration bit \a bit.
/*!
 * The nXYTER has two configuration registers (32 and 33). For the purpose
 * of method the bits are number through, bit 0 is LSB of register 32,
 * bit 8 is LSB of register 33.
 */

const char* nxyter::NxI2c::configurationBitName(int bit) 
{
  static const char* names[12] = {"test pulse enable",        // 0-0 
                                  "test pulse synchronize",   // 0-1
                                  "test pulse polarity",      // 0-2
                                  "test trigger enable",      // 0-3
                                  "test trigger synchronize", // 0-4
                                  "disable 32MHz clock",      // 0-5
                                  "disable 128 MHz clock",    // 0-6
                                  "TS LSB clock select",      // 0-7
                                  "calibration select bit 0", // 1-0
                                  "calibration select bit 1", // 1-1
                                  "front-end polarity",       // 1-2
                                  "readout clock select"      // 1-3
  };
  
  if (bit < 0 || bit >=12) return "";
  return names[bit];
}

//----------------------------------------------------------------------------
//! Convert delay to a nXYTER delay buffer setting
/*!
 * The nXYTER delay buffer settings represent the switch settings of the
 * internal delay elements. This method converts a nominal delay value,
 * given as an integer number in the range 0 to 80, into a delay buffer
 * setting suitable to be written into an nXYTER register.
 * \param delay delay value, between 0 and 80
 * \returns 8 bit setting, ready to be written to delay buffer register
 * \sa settingToDelay()
 * \note data taken from nXYTER Manual version: 1.40 (DRAFT) May 25, 2009
 */

uint8_t nxyter::NxI2c::delayToSetting(uint8_t delay)
{
  static const uint8_t d2s[81] = {
      0,   1,   3,   4,  16,  64,  48,   5,  12,  17, 
    192,  65,   7,  19,  49,  67,  68,  13,  51,  20, 
    193, 195,  80, 196,  15,  69,  52,  76,  21,  71, 
     81, 112, 208, 197,  23, 204,  53,  83,  77,  28, 
    199,  55, 113,  84, 209,  79, 205, 240, 115, 211, 
     29,  60, 207,  85, 212,  31, 116, 241,  87,  61, 
    243,  92, 213, 117,  63, 244, 215, 119,  93, 220, 
    124, 245,  95, 221, 247, 125, 223, 252, 127, 253, 
    255
  };
  
  return (delay <= 80) ? d2s[delay] : 0xff;
}

//----------------------------------------------------------------------------
//! Convert nXYTER delay buffer setting into a delay
/*!
 * The nXYTER delay buffer settings represent the switch settings of the
 * internal delay elements. This method converts this setting into a number
 * which should correspond to the delay. It is the inverse of delayToSetting().
 * It will return 0xff for settings not produced by delayToSetting().
 * \param val 8 bit setting, as read from delay buffer register
 * \returns delay value, or 0xff of non-recommended setting seen.
 * \sa delayToSetting()
 */

uint8_t nxyter::NxI2c::settingToDelay(uint8_t val)
{
  for (uint8_t delay=0; delay<=80; delay++) {
    if (delayToSetting(delay) == val) return delay;
  }
  return 0xff;
}

//----------------------------------------------------------------------------
