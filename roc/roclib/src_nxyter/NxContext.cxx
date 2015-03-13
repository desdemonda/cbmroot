//============================================================================
/*! \file NxContext.cxx
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#include <stdexcept>

#include "boost/format.hpp"

#include "nxyter/NxContext.h"
#include "nxyter/NxI2c.h"

using std::endl;
using boost::format;

/*!
 * \class nxyter::NxContext
 * \brief Container for complete I2C register context of a nXYTER
 *
 * This class holds the information of all implemented registers of the
 * the nXYTER, with the exception of the two error counters (overflow and
 * token miss). Several methods allow convenient access to individual 
 * fields of the data. The nxyter::NxI2c class has methods to
 * write/read a full context to/from an xXYTER (see 
 * nxyter::NxI2c::setContext() and nxyter::NxI2c::getContext()).
 */

//----------------------------------------------------------------------------
//! Constructor, clears all values

nxyter::NxContext::NxContext()
{
  for (int i=0; i<dimRegMain; i++) fRegMain[i] = 0;
  for (int i=0; i<dimRegTrim; i++) fRegTrim[i] = 0;
}

//----------------------------------------------------------------------------

nxyter::NxContext::~NxContext()
{
}

//----------------------------------------------------------------------------
//! Set value of register \a reg to \a val.
/*!
 * In many cases it is more convenient to use
 * setChannelMaskBit(int channel, bool isoff)
 * to access the channel masks in registers 0 to 15.
 * 
 * \throws throw std::out_of_range if \a reg not in range
 * \sa getRegister()
 */

void nxyter::NxContext::setRegister(int reg, uint8_t val)
{
  if (reg<0 || reg>=dimRegMain) 
    throw std::out_of_range("NxContext::setRegister(): reg out of range");
  fRegMain[reg] = val;
}

//----------------------------------------------------------------------------
//! Return value of register \a reg.
/*!
 * In many cases it is more convenient to use
 * getChannelMaskBit(int channel)
 * to access the channel masks in registers 0 to 15.
 *
 * \throws throw std::out_of_range if \a reg not in range
 * \sa setRegister()
 */

uint8_t nxyter::NxContext::getRegister(int reg) const
{
  if (reg<0 || reg>=dimRegMain) 
    throw std::out_of_range("NxContext::getRegister(): reg out of range");
  return fRegMain[reg];
}

//----------------------------------------------------------------------------
//! Set value of shift register slot \a channel to \a val.
/*!
 * Gives access to all bits of trim-daq register data. In many
 * cases it is more convenient to use setThresholdTrim() and
 * setPowerOffMaskBit() to access the components individually.
 *
 * \param channel nx channel number (range 0-128, 128 is test channel)
 * \param val new value (6 bit, MSB is power-down flag, 5LSB trim daq value)
 * \throws throw std::out_of_range if \a channel not in range
 * \sa getTrimRegister()
 */

void nxyter::NxContext::setTrimRegister(int channel, uint8_t val)
{
  if (channel<0 || channel>=dimRegTrim) 
    throw std::out_of_range("NxContext::setTrimRegister():channel out of range");
  fRegTrim[channel] = val & 0x3f;               // only 6 bits !!
}

//----------------------------------------------------------------------------
//! Return value of shift register slot \a channel.
/*!
 * Gives access to all bits of trim-daq register data. In many
 * cases it is more convenient to use getThresholdTrim() and
 * getPowerOffMaskBit() to access the components individually.
 *
 * \param channel nx channel number (range 0-128, 128 is test channel)
 * \throws throw std::out_of_range if \a channel not in range
 * \sa setTrimRegister()
 */

uint8_t nxyter::NxContext::getTrimRegister(int channel) const
{
  if (channel<0 || channel>=dimRegTrim) 
    throw std::out_of_range("NxContext::getTrimRegister(): channel out of range");
  return fRegTrim[channel];
}

//----------------------------------------------------------------------------
//! Set the channel mask bit for channel \a channel to \a isoff.
/*!
 * \param channel nx channel number
 * \param isoff new channel-off flag (0 is channel-on, 1 is channel-off)
 * \throws throw std::out_of_range if \a channel not in range
 * \sa getChannelMaskBit()
 */

void nxyter::NxContext::setChannelMaskBit(int channel, bool isoff)
{
  if (channel<0 || channel>=128) 
    throw std::out_of_range("NxContext::setChannelMaskBit(): channel out of range");

  int reg = (channel>>3) & 0x1f;
  uint8_t mask = 1<<(channel&0x7);
  if (isoff) {
    fRegMain[reg] |=  mask;
  } else {
    fRegMain[reg] &= ~mask;
  }
}

//----------------------------------------------------------------------------
//! Set the channel mask bit \a isoff for channels \a chbeg to \a chend.
/*!
 * \param chbeg begin of channel range
 * \param chend end of channel range
 * \param isoff new channel-off flag (0 is channel-on, 1 is channel-off)
 * \param chstep increment for channel stepping. e.g. 2 will set every 2nd
 *    channel, convenient when all odd or even channels are to be touched.
 * \throws throw std::out_of_range if \a channel not in range
 * \sa setChannelMaskBit(int, bool), getChannelMaskBit()
 */

void nxyter::NxContext::setChannelMaskBit(int chbeg, int chend, bool isoff,
                                          int chstep)
{
  if (chstep <= 0) return;
  for (int ch=chbeg; ch<=chend; ch+=chstep) setChannelMaskBit(ch, isoff);
}

//----------------------------------------------------------------------------
//! Return the channel mask bit for channel \a channel.
/*!
 * \param channel nx channel number
 * \throws throw std::out_of_range if \a channel not in range
 * \sa setChannelMaskBit(int, bool), setChannelMaskBit(int, int, bool)
 */

bool nxyter::NxContext::getChannelMaskBit(int channel) const
{
  if (channel<0 || channel>=128) 
    throw std::out_of_range("NxContext::getChannelMaskBit(): channel out of range");

  int reg = (channel>>3) & 0x1f;
  uint8_t mask = 1<<(channel&0x7);
  
  return fRegMain[reg] & mask;
}

//----------------------------------------------------------------------------
//! Set the threshold trim value \a channel to \a val.
/*!
 * \param channel nx channel number (range 0-128, 128 is test channel)
 * \param val new value for threshold trim (5bit, range 0-31)
 * \throws throw std::out_of_range if \a channel not in range
 * \sa setThresholdTrim(int, int, uint8_t), getThresholdTrim()
 */

void nxyter::NxContext::setThresholdTrim(int channel, uint8_t val)
{
  if (channel<0 || channel>=dimRegTrim) 
    throw std::out_of_range("NxContext::setThresholdTrim(): channel out of range");

  fRegTrim[channel] &= ~0x1f;               // erease lower 5 bit
  fRegTrim[channel] |= val & 0x1f;          // fill in lower 5 bit
}

//----------------------------------------------------------------------------
//! Set the threshold trim value to \a val for channels \a chbeg to \a chend.
/*!
 * The allowed channel range is 0 to 128, 128 is the test channel.
 * \param chbeg begin of channel range
 * \param chend end of channel range
 * \param val new value for threshold trim (5bit, range 0-31)
 * \param chstep increment for channel stepping. e.g. 2 will set every 2nd
 *    channel, convenient when all odd or even channels are to be touched.
 * \throws throw std::out_of_range if \a channel not in range
 * \sa setThresholdTrim(int, uint8_t), getThresholdTrim()
 */

void nxyter::NxContext::setThresholdTrim(int chbeg, int chend, uint8_t val,
                                         int chstep)
{
  if (chstep <= 0) return;
  for (int ch=chbeg; ch<=chend; ch+=chstep) setThresholdTrim(ch, val);
}

//----------------------------------------------------------------------------
//! Return the threshold trim value \a channel.
/*!
 * \param channel nx channel number (range 0-128, 128 is test channel)
 * \throws throw std::out_of_range if \a channel not in range
 * \sa setThresholdTrim(int, uint8_t), setThresholdTrim(int, int, uint8_t)
 */

uint8_t nxyter::NxContext::getThresholdTrim(int channel) const
{
  if (channel<0 || channel>=dimRegTrim) 
    throw std::out_of_range("NxContext::getThresholdTrim(): channel out of range");

  return fRegTrim[channel] & 0x1f;          // return lower 5 bit
}

//----------------------------------------------------------------------------
//! Set the power off mask bit for \a channel to \a isoff.
/*!
 * \param channel nx channel number (range 0-128, 128 is test channel)
 * \param isoff new power-down flag (0 is power-on, 1 is power-off)
 * \throws throw std::out_of_range if \a channel not in range
 * \sa setPowerOffMaskBit(int, int, bool), getPowerOffMaskBit()
 */

void nxyter::NxContext::setPowerOffMaskBit(int channel, bool isoff)
{
  if (channel<0 || channel>=dimRegTrim) 
    throw std::out_of_range("NxContext::setPowerOffMaskBit(): channel out of range");

  if (isoff) {
    fRegTrim[channel] |=  0x20;
  } else {
    fRegTrim[channel] &= ~0x20;                     // erease bit 6
  }
}

//----------------------------------------------------------------------------
//! Set the power off mask bit to \a isoff for channels \a chbeg to \a chend.
/*!
 * The allowed channel range is 0 to 128, 128 is the test channel.
 * \param chbeg begin of channel range
 * \param chend end of channel range
 * \param isoff new power-down flag (0 is power-on, 1 is power-off)
 * \param chstep increment for channel stepping. e.g. 2 will set every 2nd
 *    channel, convenient when all odd or even channels are to be touched.
 * \throws throw std::out_of_range if a non-existing channel is touched
 * \sa setPowerOffMaskBit(int, bool), getPowerOffMaskBit()
 */

void nxyter::NxContext::setPowerOffMaskBit(int chbeg, int chend, bool isoff,
                                           int chstep)
{
  if (chstep <= 0) return;
  for (int ch=chbeg; ch<=chend; ch+=chstep) setPowerOffMaskBit(ch, isoff);
}

//----------------------------------------------------------------------------
//! Return the power off mask bit for \a channel.
/*!
 * \param channel nx channel number (range 0-128, 128 is test channel)
 * \throws throw std::out_of_range if \a channel not in range
 * \sa setPowerOffMaskBit(int, bool), setPowerOffMaskBit(int, int, bool)
 */

bool nxyter::NxContext::getPowerOffMaskBit(int channel) const
{
  if (channel<0 || channel>=dimRegTrim) 
    throw std::out_of_range("NxContext::getPowerOffMaskBit(): channel out of range");

  return fRegTrim[channel] & 0x20;
}


//----------------------------------------------------------------------------
//! Return configuration bit from regs 32-33
/*!
 * \param n from range 0-11
*/

bool nxyter::NxContext::getConfigurationBit(int n)
{
   if ((n<0) || (n>=12)) return false;
   if (n<8) return (fRegMain[32] & (1 << n)) != 0;
   return (fRegMain[33] & (1<<(n-8))) != 0;
}

//----------------------------------------------------------------------------
//! Set configuration bit from regs 32-33
/*!
 * \param n from range 0-11
*/

void nxyter::NxContext::setConfigurationBit(int n, bool on)
{
   if ((n<0) || (n>=12)) return;
   if (n<8)
      fRegMain[32] = on ? (fRegMain[32] | (1 << n)) : (fRegMain[32] & ~(1 << n));
   else
      fRegMain[33] = on ? (fRegMain[33] | (1 << (n-8))) : (fRegMain[33] & ~(1 << (n-8)));
}

//----------------------------------------------------------------------------
//! Set nXYTER context to default values.
/*!
 * All registers are initialized with default values
 * \li the configuration registers are set for <em>standard FEB</em>
 *     operation.
 * \li the bias registers are set to average values found to be acceptable
 *     for several chips. The proper settings for register 21 (VBiasS) and 
 *     22 (VBiasF) are very temperature dependent, the default values might
 *     thus not always bring the chip even into operating range.
 *     The values depend partially on the desired signal polarity specified
 *     with the \a ispos argument.
 * \li the channel masks are set depending on \a maskon, the channels 0 to
 *     \a maskon are enabled, the rest is disabled.
 * \li the power down masks are set depending on \a poweron, the channels 0
 *     to \a poweron are powered, the rest is powered down. The test channel
 *     is always powered.
 * \li the threshold trims are set to 16.
 *
 * \param ispos if \c false setup for negative pulses, if \c true, for 
 *          positive pulses.
 * \param maskon channels with a number less or equal \a maskon will be
 *          enable in the channel mask (mask bit = 0), the rest disabled.
 * \param poweron channels with a number less or equal \a poweron will be
 *          powered, the rest has the power-down flag set.
 */

void nxyter::NxContext::setToDefault(bool ispos, int maskon, int poweron)
{
  // real clean...
  for (int i=0; i<dimRegMain; i++) fRegMain[i] = 0;
  for (int i=0; i<dimRegTrim; i++) fRegTrim[i] = 0;

  for (int i=0; i<128; i++) {
    setChannelMaskBit(i, i>=maskon);        // mask=1  means channel off
    setPowerOffMaskBit(i, i>=poweron);      // isoff=1 means channel power off
    setThresholdTrim(i, 16);
  }
  setPowerOffMaskBit(128, false);
  setThresholdTrim(128, 16);

  // taken from nxyter::I2Cbus::testsetup()
  setRegister(kNxRegVcg,     160);
  setRegister(kNxRegIcgfoll, 255);
  setRegister(kNxRegVth,      35);
  setRegister(kNxRegVbfb,      6);
  setRegister(kNxRegVbiasF,   95);
  setRegister(kNxRegVbiasS,   87);
  setRegister(kNxRegVbiasS2, 100);
  setRegister(kNxRegVcm,     137);
  setRegister(kNxRegcal,     255);
  setRegister(kNxRegiCOMP,    69);
  setRegister(kNxRegiDUR,     15);
  setRegister(kNxRegiINV,     54);
  setRegister(kNxRegiPDH,     92);
  setRegister(kNxRegiTWC,     69);
  setRegister(kNxRegConfig0,   0);
  setRegister(kNxRegConfig1,  kNxC1ReadClockSelect | 0x3);
  
  // now handle what is different for positive polarity
  // 1. Front end polarity bit set
  // 2. test puls polarity bit set
  // 3. VBiasF default 65 rather than 95
  if (ispos) {
    uint8_t val = getRegister(kNxRegConfig1);
    setRegister(kNxRegConfig1, val | kNxC1FrontEndPolarity);
    setRegister(kNxRegConfig0, kNxC0TestPulsPolarity);
    setRegister(kNxRegVbiasF,   65);
  }
}

//----------------------------------------------------------------------------
//! Compare \c this contet with context \a rhs.
/*!
 * The compare can be enabled separately for mask, core and trim part of the
 * context with the \a domask when the bits kDoMask, kDoCore, or kDoTrim
 * are set.
 * The whole context is compared when all three are \c true, which is the 
 * default.
 * \param rhs second context to compare against
 * \param domask one or several of the mask bits kDoMask, kDoCore, or kDoTrim
 * \returns \c false if any enabled compare fails, \c true otherwise
 */

bool nxyter::NxContext::equal(const NxContext& rhs, int domask) const
{
  if (domask & kDoMask) {
    for (int i=0; i<16; i++) {
      if (fRegMain[i] != rhs.fRegMain[i]) return false;
    }
  }
  if (domask & kDoCore) {
    for (int i=16; i<dimRegMain; i++) {
      if (i==30 || i==31) continue;         // skip unimplemented regs
      if (i>=34 && i<=37) continue;         // skip error counters 
      if (i>=40 && i<=42) continue;         // skip unimplemented regs and reg42
      if (fRegMain[i] != rhs.fRegMain[i]) return false;
    }
  }
  if (domask & kDoTrim) {
    for (int i=0; i<dimRegTrim; i++) {
      if (fRegTrim[i] != rhs.fRegTrim[i]) return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------
//! Print context to stream \a os.
/*!
 * Produces a nicely formatted listing of all or part of the nXYTER settings.
 * The print can be enabled separately for mask, core and trim part of the
 * context with the \a domask when the bits kDoMask, kDoCore, or kDoTrim
 * are set.
 * 
 * The output has the form (here full listing with kDoAll):
\verbatim
Reg( 0) mask_000_007  : 0xff 255 :  ch   7-  0: o o o o o o o o   o=active
Reg( 1) mask_008_015  : 0xff  15 :  ch  15-  8: o o o o d d d d   d=inactive
...
Reg(15) mask_120_127  : 0x00   0 :  ch 127-120: o o o o o o o o
Reg(16) Vcg           : 0xa0 160
Reg(17) Icgfoll       : 0xff 255
...
Reg(28) iPDH          : 0x5c  92
Reg(29) iTWC          : 0x45  69
Reg(32) Config0       : 0x00   0 : bit(0) = 0 : test pulse enable
                                 : bit(1) = 0 : test pulse synchronize
                                 : bit(2) = 0 : test pulse polarity
                                 : bit(3) = 0 : test trigger enable
                                 : bit(4) = 0 : test trigger synchronize
                                 : bit(5) = 0 : disable 32MHz clock
                                 : bit(6) = 0 : disable 128 MHz clock
                                 : bit(7) = 0 : TS LSB clock select
Reg(33) Config1       : 0x0b  11 : bit(0) = 1 : calibration select bit 0
                                 : bit(1) = 1 : calibration select bit 1
                                 : bit(2) = 0 : front-end polarity
                                 : bit(3) = 1 : readout clock select
Reg(38) delayTestPuls : 0x00   0 : delay=  0
...
Reg(45) delayClock3   : 0x00   0 : delay=  0
Reg(42) TrimDAQ-Power : ch   0:  u 16  u 16  u 16  u 16  u 16  u 16  u 16  u 16
Reg(42) TrimDAQ-Power : ch   8:  u 16  u 16  u 16  u 16  u 16  u 16  u 16  u 16
...
Reg(42) TrimDAQ-Power : ch 120:  d 16  d 16  d 16  d 16  d 16  d 16  d 16  d 16
Reg(42) TrimDAQ-Power : ch 128:  u 16
\endverbatim
*
* \note The delay settings are displayed as raw values (in hex and decimal(
* and also as <em>effective delay values</em> translated by the 
* NxI2c::delayToSetting() method.
*/

void nxyter::NxContext::print(std::ostream& os, int domask) const
{
  static format fmt_reg("Reg(%|2|) %|-14|: 0x%|02x| %|3|");
  // print registers
  for (int i=0; i<46; i++) {
    if ((domask & kDoMask)==0 && i<16) continue;
    if ((domask & kDoCore)==0 && i>=16) continue;
    
    if (i>=34 && i<=37) continue;
    if (i==42) continue;
    const char* regname = nxyter::NxI2c::registerName(i);
    if (regname[0] == 0) continue;
    os << fmt_reg % i % regname % uint16_t(fRegMain[i]) %
                   uint16_t(fRegMain[i]);

    if (i<16) {                             // mask registers
      static format fmt(" ch %|3|-%|3|:");
      os << " : " << fmt % ((8*i)+7) % (8*i);
      for (int j=7; j>=0; j--) os << ((fRegMain[i] & (1<<j)) ? " d" : " o");
      if (i==0) os << "   o=active";
      if (i==1) os << "   d=inactive";
    }
    
    if (i==32 || i==33) {
      for (int j=0; j<8; j++) {
        if (j!=0) os << "                                ";
        static format fmt(" : bit(%|1|) = %|1| : ");
        os << fmt % j % ((fRegMain[i]&(1<<j))!=0);
        os << nxyter::NxI2c::configurationBitName(8*(i-32)+j);
        if (i==32 && j!=7) os << endl;
        if (i==33 && j!=3) os << endl;  
        if (i==33 && j==3) break;
      }
    }

    if (i==38 || i==39 || i>=43) {          // delay regs
      static format fmt(" : delay=%|3|");
      os << fmt % uint16_t(nxyter::NxI2c::settingToDelay(fRegMain[i]));
    }
    os << endl;
  }

  if ((domask & kDoTrim)==0) return;

  for (int i=0; i<129; i++) {
    if (i%8 == 0) {
      static format fmt("Reg(42) TrimDAQ-Power : ch %|3|:");
      os << fmt % i;
    }
    static format fmt("%|2|");
    os << "  " << (getPowerOffMaskBit(i) ? "d" : "u");
    os << " " << fmt % uint16_t(getThresholdTrim(i));
    if (i%8 == 7) os << endl;
  }
  os << endl; 
}

//----------------------------------------------------------------------------
//! Print context as rocutil command file.
/*!
 * Produces set of commands to set all regeisters of nxyter.
 */
 
bool nxyter::NxContext::fillCmdFile(FILE* f, int nx)
{
   fprintf(f,"//Actual nXYTER registers\n");

   // bias registers
   for (int reg=16;reg<30;reg++)
      fprintf(f,"setnx %d,%d,%d \n", nx, reg, getRegister(reg));

   for (int reg=32;reg<34;reg++)
      fprintf(f,"setnx %d,%d,%d \n", nx, reg, getRegister(reg));

   for (int reg=38;reg<40;reg++)
      fprintf(f,"setnx %d,%d,%d \n", nx, reg, getRegister(reg));

   for (int reg=43;reg<46;reg++)
      fprintf(f,"setnx %d,%d,%d \n", nx, reg, getRegister(reg));


   // mask values
   int nbeg = 0, nch = 1;
   bool begoff = getChannelMaskBit(0);

   while (nch<=128) {
      bool isoff = nch < 128 ? getChannelMaskBit(nch) : begoff;

      if ((isoff!=begoff) || (nch == 128)) {
         fprintf(f,"setnxmask %d,%d,%d,%d,1 \n", nx, begoff ? 1 : 0, nbeg, nch-1);
         nbeg = nch;
         begoff = isoff;
      }
      nch++;
   }

   // trimmer values
   nbeg = 0; nch = 1;
   int begtrim = getThresholdTrim(0);

   while (nch<=129) {
      int trim = nch < 129 ? getThresholdTrim(nch) : begtrim;

      if ((trim!=begtrim) || (nch == 129)) {
         fprintf(f,"setnxtrim %d,%d,%d,%d,1 \n", nx, begtrim, nbeg, nch-1);
         nbeg = nch;
         begtrim = trim;
      }
      nch++;
   }

   // power down bits
   nbeg = 0; nch = 1;
   begoff = getPowerOffMaskBit(0);

   while (nch<=129) {
      bool isoff = nch < 129 ? getPowerOffMaskBit(nch) : begoff;

      if ((isoff!=begoff) || (nch == 129)) {
         fprintf(f,"setnxpower %d,%d,%d,%d,1 \n", nx, begoff ? 1 : 0, nbeg, nch-1);
         nbeg = nch;
         begoff = isoff;
      }
      nch++;
   }


   return true;
}


//----------------------------------------------------------------------------
// allocate storage for 'static const', rootcint needs it
const int nxyter::NxContext::dimRegMain;
const int nxyter::NxContext::dimRegTrim;
