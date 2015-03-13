//============================================================================
/*! \file FebBase.cxx
 *  \author W.F.J.Mueller/GSI, significant adaptions
 *  \author Norbert Abel/KIP, initial version
 */
//============================================================================

#include <string.h>

#include <stdexcept>

#include "roc/defines_i2c.h"
#include "roc/I2cDevice.h"

#include "nxyter/defines_nxyter.h"
#include "nxyter/FebBase.h"

#include "nxyter/Feb1nxGenB.h"
#include "nxyter/Feb1nxGenC.h"
#include "nxyter/Feb1nxGenD.h"
#include "nxyter/Feb2nxGas.h"
#include "nxyter/Feb4nxBT.h"

using roc::Board;

/*!
 * \class nxyter::FebBase
 * \brief Represents a generic FEB.
 *
 */

//----------------------------------------------------------------------------
//! Constructor with full addressing path information.
/*!
 * Sets up \c FebBase object with full addressing information
 * \param board ROC board pointer
 * \param port the \ref glo_roc_port
 * \param type of FEB as encoded in FebBase::FebType
 */

nxyter::FebBase::FebBase() :
  roc::Peripheral(),
  fPort(0),
  fType(0),
  fMainAdc(),
  fMonAdcDevice(),
  fEepromDevice()
{
}


nxyter::FebBase::FebBase(roc::Board* board, int port, int type) :
  roc::Peripheral(board),
  fPort(port),
  fType(type),
  fMainAdc(board, port, (type/100==4)),
  fMonAdcDevice(board, port, 0),
  fEepromDevice(board, port, 0)
{
  int numnx = type/100;                     // this holds by convention !!
  fNxChip.reserve(numnx);
  fNxOffline.reserve(numnx);
  fMode4nx = (numnx == 4);
  fAdcChannel.reserve(numnx);
}

//----------------------------------------------------------------------------

nxyter::FebBase::~FebBase()
{
  for (int i=0; i<fNxChip.size(); i++) delete fNxChip[i];
}

//----------------------------------------------------------------------------
//! Gives access to the NxChip object of the nXYTER with index \a ind.
/*!
 * Provides to access the object representing one of the nXYTERs on the
 * FEB. Acceptable values of \a ind are 0 to \a numNx() -1, otherwise
 * an exception is thrown.
 * \param nxind see \ref glo_nxyter_nxind
 * \throws std::out_of_range if \a nxind out of range
 * \note the nXYTER index needed here can be different from the nXYTER
 * number (usually named \a nxnum). The index \a nxind identifies the nXYTER
 * on a given FEB. The nXYTER number provides a unique identification of
 * all nXYTERs on a ROC.
 */

nxyter::NxChip& nxyter::FebBase::nx(int nxind)
{
  return *(fNxChip.at(nxind));
}

//----------------------------------------------------------------------------
//! Initialize ROC to support \c this FEB.
/*!
 * This method will setup
 * \li  set nXYTER active/inactive
 * \li  setup Feb4nx mode flag
 * \li  setup adc channel mux
 *
 * \returns see \ref roc_board_opererr
 */

int nxyter::FebBase::initRoc()
{
  int rc = board().put(ROC_NX_FEB4NX, (fMode4nx ? 1 : 0));
  if (rc) return rc;
  for (int i=0; i<numNx(); i++) {
    if (!getNxOffline(i))
       rc = adc().setChannelMux(nx(i).getNxNumber(), fAdcChannel[i]);
    if (rc) return Board::operErrBuildInc(rc, 2*(i+1));

    if (getNxOffline(i))
       rc = nx(i).deactivate();
    else
       rc = nx(i).activate();
    if (rc) return Board::operErrBuildInc(rc, 2*(i+1)+1);
  }
  return 0;
}


//----------------------------------------------------------------------------
//! Get current FEB state - active flags of nXYTERs
/*!
 * \returns see \ref roc_board_opererr
 */

int nxyter::FebBase::getFebState()
{
  for (int i=0; i<numNx(); i++) {
     bool active = nx(i).isactive();
     setNxOffline(i, !active);
  }
  return 0;
}


//----------------------------------------------------------------------------
//! Probe whether nx and adc on the FEB are available. set 'offline' flags.
/*!
 * The probe method uses the NxI2c::probe() and MainAdc::probe() methods to
 * determine whether control access to the nXYTER and ADC can be established.
 * If an nXYTER probe fails, the 'offline' flag will be set \c true, see
 * setNxOffline().
 * \returns 0 in case ADC and all nXYTYERs are successfully probed. Otherwise
 *            a bit pattern of the missing components is returned, where 
 *            bit 0 to 3 set indicate that nXYTER 0 to 3 is missing, and 
 *            bit 4 indicates that the ADC is missing.
 */

int nxyter::FebBase::probe() 
{
  int rc = 0;
  
  for (int i=0; i<fNxChip.size(); i++) {
    if (nx(i).i2c().probe()) {              // !=0 return means probe failed !
      setNxOffline(i, true);
      rc |= 1<<i;
    } else {
      setNxOffline(i, false);
    }
  }
  if (adc().probe()) rc |= 1<<5;
  return rc;
}

//----------------------------------------------------------------------------
//! Write default values into ADC and nXYTER registers
/*!
 * This method uses the NxI2c::setToDefault() and MainAdc::setToDefault()
 * methods to setup the registers with default values, consult the respective
 * documentation for details. The nXYTER polarity is determined by \a ispos0
 * and \a ispos1, where ispos0 determines the polarity for an FEB1nx, the 
 * first (index 0) chip if an FEB2nx and the first two (index 0 and 1) of
 * an FEB4nx. \a ispos1 is used for the remaining chips.
 * \returns see \ref roc_board_opererr
 */

int nxyter::FebBase::setToDefault(bool ispos0, bool ispos1)
{
  bool pos[4];
  for (int i=0; i<4; i++) pos[i] = ispos0;
  if (fNxChip.size()==2) pos[1] = ispos1;
  if (fNxChip.size()==4) pos[2] = pos[3] = ispos1;

  int rc;
  
  for (int i=0; i<fNxChip.size(); i++) {
    if (getNxOffline(i)) continue;          // skip if offline
    rc = nx(i).i2c().setToDefault(pos[i]);
    if (rc) return Board::operErrBuild(rc, i);
  }
  rc = adc().setToDefault();
  if (rc) return Board::operErrBuild(rc, 5);
  return 0;
}

//----------------------------------------------------------------------------
//! Print standart headline for a nXYTER \a nxind.

void nxyter::FebBase::printNxHeadLine(std::ostream& os, int nxind)
{
  os << std::dec;
  os << "----- nXYTER at port=" << fPort
     << " index=" << nxind
     << " number=" << nx(nxind).getNxNumber()
     << " addr=" << uint32_t(nx(nxind).i2c().getSlaveAddr());
  if (getNxOffline(nxind)) {
    os << " ----- is set OFFLINE -----";
  } else {
    os << " --------------------------";
  }
  os << std::endl;
}

//----------------------------------------------------------------------------
//! Read nXYTER and ADC registers and print to stream \a os.
/*!
 * This method uses the NxI2c::printRegisters() and MainAdc::printRegisters()
 * methods to print the current status of the FEB board, consult the respective
 * documentation for details.
 */

void nxyter::FebBase::printRegisters(std::ostream& os, int domask)
{
  for (int i=0; i<fNxChip.size(); i++) {
    printNxHeadLine(os, i);
    if (!getNxOffline(i)) nx(i).i2c().printRegisters(os, domask);
  }
  os << "----- ADC at port=" << fPort << " --------------------" << std::endl;
  adc().printRegisters(os);
}

//----------------------------------------------------------------------------
//! Reset the I2C Interface of all nXYTERs of the FEB.
/*! 
 * This methods resets the I2C bus state machine in all nXYYERs on the FEB.
 * It might be useful in case this state machine is stuck. It should not
 * affect the status of any register in the nXYTER.
 * \sa resetNxI2cRegister()
 */

void nxyter::FebBase::resetNxI2cBus()
{
  board().operPPP(ROC_NX_I2C_SWITCH, fPort,
                  ROC_NX_I2C_RESET,  0,
                  ROC_NX_I2C_RESET,  1);
}

//----------------------------------------------------------------------------
//! Reset the I2C Registers of all nXYTERs of the FEB.
/*!
 * This method resets the I2C registers of all nXYTERs on the FEB to the
 * <em>chip default</em> values, which are 0 for masks and 128 (mid range)
 * for all bias DAC's. In general this will out the nXYTER in a state where
 * it will not operate properly.
 *
 * This method might be useful in some cases of despair and to test this
 * reset mechanism of last resort. To put the FEB into a working state
 * use setToDefault().
 *
 * \sa resetI2cBus(), setToDefault().
 */

void nxyter::FebBase::resetNxI2cRegister()
{
  board().operPPP(ROC_NX_I2C_SWITCH,    fPort,
                  ROC_NX_I2C_REGRESET,  0,
                  ROC_NX_I2C_REGRESET,  1);
}

//----------------------------------------------------------------------------
//! Setup <em>test pulser</em> and <em>test trigger</em> mode in all nXYTERs.
/*!
 * Calls for all nXYTERs on of the FEB the NxI2c::setNxTestModes() method, check
 * the documentation of this method for details.
 *
 * \param testtrig determines whether <em>test trigger</em> mode is enabled
 * \param testpuls determines whether <em>test pulser</em> mode is enabled
 * \param calselect determines which channel group receives test pulse
 * \returns see \ref roc_board_opererr
 */

int nxyter::FebBase::setNxTestModes(bool testpuls, bool testtrig, 
                                    int calselect)
{
  for (int i=0; i<fNxChip.size(); i++) {
    int rc = nx(i).i2c().setTestModes(testpuls, testtrig, calselect);
    if (rc) return Board::operErrBuild(rc, i);
  }
  return 0;
}

//----------------------------------------------------------------------------
//! Write a value into a register of all nXYTERs.
/*!
 * Writes \a val into register \a reg of all nXYTERs on the FEB.
 * \param reg register number
 * \param val value to be written
 * \param veri if \c true readback verification done (default is \c true)
 * \returns see \ref roc_board_opererr
 */

int nxyter::FebBase::setNxRegisterAll(uint8_t reg, uint8_t val, bool veri)
{
  for (int i=0; i<fNxChip.size(); i++) {
    if (getNxOffline(i)) continue;          // skip if offline
    int rc = nx(i).i2c().setRegister(reg, val, veri);
    if (rc) return Board::operErrBuild(rc, i);
  }
  return 0;
}

//----------------------------------------------------------------------------
// ! return number of monitor ADC channels, 0 if not supported

int nxyter::FebBase::getNumMonAdc()
{
   if (!monAdcSupport()) return 0;

   return 4;
}

//----------------------------------------------------------------------------
//! Read value from monitoring ADC.
/*!
 * Returns reading of channel \c ch from the monitoring ADC.
 * \param ch channel number (0 to 3)
 * \param val 16 bit value to be written
 * \returns see \ref roc_board_opererr, or -1 if no monitoring ADC 
 *    support available, or -2 if \c ch out of range, or -3 if ADC read
 *    error
 */

int nxyter::FebBase::getMonAdc(int ch, uint16_t &val)
{
  val = 0;
  if (!monAdcSupport()) return -1;
  if (ch < 0 || ch > 3) return -2;

  uint8_t mask = 1 << (ch+4);
  uint16_t adcval;
  int rc = fMonAdcDevice.getRegister16(mask, adcval);
  val = adcval & 0xfff;                     // strip 4 MSB (hold channel num)
  if (((adcval>>12) & 0x3) != ch) rc = -3;  // check for valid channel id
  return rc;
}

//----------------------------------------------------------------------------
/*!
 * Adds a new nXYTER with I2C slave address \a addr and nXYYER number \a nxnum
 * to the configuration. The FebBase class supports FEB's with up to 4
 * nXYTERs, so this method can be called at most 4 times.
 * \param addr I2C slave address of the nXYTER
 * \param nxnum see \ref glo_nxyter_nxnum
 * \param adcmux setting for the ADC channel Mux
 * \throws std::out_of_range if called more often than the FebType requires.
 */

void nxyter::FebBase::addNxChip(int addr, int nxnum, int adcmux)
{
  if (fNxChip.size() >= fNxChip.capacity()) 
    throw std::out_of_range("FATAL ERROR: addNxChip(), extraneous call");

  fNxChip.push_back(new nxyter::NxChip(getBoard(), fPort, addr, nxnum));
  fNxOffline.push_back(false);
  fAdcChannel.push_back(adcmux);
}
  
//----------------------------------------------------------------------------
//! Converts a FebClass or FebType value into a string.
/*!
 * \param type \ref glo_feb_class or \ref glo_feb_type value
 * \returns pointer to a static string with text representation, or a pointer
 *   to an empty string ("") if \a type isn't a member of FebBase::FebType
 */

const char* nxyter::FebBase::typeToString(int type)
{
  switch (type) {
    case kFeb1nx     : return "Feb1nx";
    case kFeb1nxGenB : return "Feb1nxGenB";
    case kFeb1nxGenC : return "Feb1nxGenC";
    case kFeb1nxGenD : return "Feb1nxGenD";
    case kFeb2nx     : return "Feb2nx";
    case kFeb2nxGas  : return "Feb2nxGas";
    case kFeb4nx     : return "Feb4nx";
    case kFeb4nxBT   : return "Feb4nxBT";
  }  
  return "";
}

//----------------------------------------------------------------------------
//! Converts a string into a FebClass or FebType value.
/*!
 * \param str string with the text representation of a \ref glo_feb_class
 *    or \ref glo_feb_type.
 * \returns \ref glo_feb_class or \ref glo_feb_type value, or 0 if 
 *    \a str doesn't match any of the valid names.
 */

int nxyter::FebBase::stringToType(const char* str)
{
  if (strcmp(str, "Feb1nx")==0)     return kFeb1nx;
  if (strcmp(str, "Feb1nxGenB")==0) return kFeb1nxGenB;
  if (strcmp(str, "Feb1nxGenC")==0) return kFeb1nxGenC;
  if (strcmp(str, "Feb1nxGenD")==0) return kFeb1nxGenD;
  if (strcmp(str, "Feb2nx")==0)     return kFeb2nx;
  if (strcmp(str, "Feb2nxGas")==0)  return kFeb2nxGas;
  if (strcmp(str, "Feb4nx")==0)     return kFeb4nx;
  if (strcmp(str, "Feb4nxBT")==0)   return kFeb4nxBT;

  return 0;
}

//----------------------------------------------------------------------------
//! Method to discover FEB's connected to the ROC
/*!
 * This static method can be used to determine what type of FEBs are connected
 * to the two ports of the ROC represented by \a board.
 * When \a liberal is \c false (the default), only fully
 * fully functional FEBs with all nXYTERs operational will be recognized.
 * If \a liberal is \c true, also FEBs with non-operational nXYTERs. The ADC 
 * must always be available.
 *
 * The discover logic is based on the I2C slave address convention
 * \li 1nx boards have the nXYTER in the range 2,...,30
 * \li 2nx boards have the nXYTER in the range 32,...,62
 * \li 4nx boards have the nXYTER in the range 64,...,126
 *
 * The currently available FEB's use:
 * \li FEB1nxGenC: [0] on 8
 * \li FEB1nxGenD: [0] on 8
 * \li FEB2nxGas:  [0] on 40; [1] on 48
 * \li FEB4nxBT:   [0] on 72; [1] on 82; [2] on 84; [3] on 88
 *
 * For strict mode (\a liberal = \c false) all nXYTERs must be seen,
 * for liberal mode (\a liberal = \c true) one is enough.
 *
 * \param board ROC board pointer
 * \param typeport0 the type of FEB as defined in FebType for the FEB found
 *          on port 0, or 0 if none found.
 * \param typeport1 the type of FEB as defined in FebType for the FEB found
 *          on port 1, or 0 if none found.
 * \param liberal if \a true, also FEBs with disfunctional nXYTERs are accepted
 */

void nxyter::FebBase::discoverFebs(roc::Board* board, 
                                   int& typeport0, int& typeport1, 
                                   bool liberal)
{
  static const int dim_nxlist = 7;
  int nxaddr[dim_nxlist];
  int nxtype[dim_nxlist];
  
  nxaddr[0] =  8;  nxtype[0] = kFeb1nxGenC;
  nxaddr[1] = 40;  nxtype[1] = kFeb2nxGas;
  nxaddr[2] = 48;  nxtype[2] = kFeb2nxGas;
  nxaddr[3] = 72;  nxtype[3] = kFeb4nxBT;
  nxaddr[4] = 82;  nxtype[4] = kFeb4nxBT;
  nxaddr[5] = 84;  nxtype[5] = kFeb4nxBT;
  nxaddr[6] = 88;  nxtype[6] = kFeb4nxBT;
  
  bool ok_adc[2];
  int  num_adc = 0;
  bool ok_nx[2][dim_nxlist];
  int  numnx_1nxGen[2];
  int  numnx_2nxGas[2];
  int  numnx_4nxBT[2];
  numnx_1nxGen[0] = numnx_1nxGen[1] = 0;
  numnx_2nxGas[0] = numnx_2nxGas[1] = 0;
  numnx_4nxBT[0]  = numnx_4nxBT[1]  = 0;

  // first probe everything possible
  for (int port=0; port<=1; port++) {
    nxyter::MainAdc adc(board, port);
    if ( (ok_adc[port] = (adc.probe() == 0)) ) num_adc++;

    for (int i=0; i<dim_nxlist; i++) {
      nxyter::NxChip nx(board, port, nxaddr[i], 0);
      if ( (ok_nx[port][i] = (nx.i2c().probe() == 0)) ) {
        if (nxtype[i] == kFeb1nxGenC) numnx_1nxGen[port]++;
        if (nxtype[i] == kFeb2nxGas)  numnx_2nxGas[port]++;
        if (nxtype[i] == kFeb4nxBT)   numnx_4nxBT[port]++;
      }
    }
  }
  
  typeport0 = 0;
  typeport1 = 0;
  if (num_adc==0) return;

  int typeport[2];
  typeport[0] = 0;
  typeport[1] = 0;
  
  // in the non-liberal case ask for all
  if (not liberal) {
    if (ok_adc[0]) {
      if (numnx_1nxGen[0]==1) typeport[0] = kFeb1nxGenC;
      if (numnx_2nxGas[0]==2) typeport[0] = kFeb2nxGas;
      if (numnx_4nxBT[0]==4)  typeport[0] = kFeb4nxBT;
    }
    if (ok_adc[1] && typeport[0]!=kFeb4nx) {
      if (numnx_1nxGen[1]==1) typeport[1] = kFeb1nxGenC;
      if (numnx_2nxGas[1]==2) typeport[1] = kFeb2nxGas;
    }

    // in the liberal case one nXYTER is enough (Note that ranges are distinct)
  } else {
    if (ok_adc[0]) {
      if (numnx_1nxGen[0]>0) typeport[0] = kFeb1nxGenC;
      if (numnx_2nxGas[0]>0) typeport[0] = kFeb2nxGas;
      if (numnx_4nxBT[0]>0)  typeport[0] = kFeb4nxBT;
    }
    if (ok_adc[1] && typeport[0]!=kFeb4nx) {
      if (numnx_1nxGen[1]>0) typeport[1] = kFeb1nxGenC;
      if (numnx_2nxGas[1]>0) typeport[1] = kFeb2nxGas;
    }
  }

  // Now check whether a monitor I2C ADC is detected, if yes map
  //    Feb1nxGenC -> Feb1nxGenD

  for (int port=0; port<=1; port++) {
    if (typeport[port]) {
      uint16_t val;
      roc::I2cDevice i2c(board, port, 41);
      int rc = i2c.getRegister16(0x10, val);
      if (rc == 0) {
        if (typeport[port] == kFeb1nxGenC) typeport[port] = kFeb1nxGenD;
      }
    }
  }

  typeport0 = typeport[0];
  typeport1 = typeport[1];  
}

//----------------------------------------------------------------------------
//! Factory method to create a new Feb object
/*!
 * \param board ROC board pointer
 * \param port the \ref glo_roc_port
 * \param type of Feb object to be created as encoded in FebBase::FebType
 * \returns pointer to new Feb object, or 0 if \a type is not a valid
 *    \ref glo_feb_type.
 */

nxyter::FebBase* nxyter::FebBase::newFeb(roc::Board* board, int port, int type)
{
   if ((port<0) || (port>1)) return 0;

   switch (type) {
     case kFeb1nxGenB : return new Feb1nxGenB(board, port);
     case kFeb1nxGenC : return new Feb1nxGenC(board, port);
     case kFeb1nxGenD : return new Feb1nxGenD(board, port);
     case kFeb2nxGas  : return new Feb2nxGas(board, port);
     case kFeb4nxBT   : return new Feb4nxBT(board);
   }
   return 0;
}
