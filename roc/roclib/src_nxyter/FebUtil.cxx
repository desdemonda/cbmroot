//============================================================================
/*! \file FebUtil.cxx
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include <stdexcept>
#include <memory>

#include "boost/format.hpp"

#include "roc/Gpio.h"
#include "nxyter/RocNx.h"
#include "nxyter/FebUtil.h"
#include "nxyter/NxContext.h"
#include "nxyter/DistFuncArray.h"

using std::cout;
using std::endl;
using boost::format;

using roc::Board;
using nxyter::NxContext;

/*!
 * \class nxyter::FebUtil
 * \brief All kinds methods to setup and test FEBs.
 *
 */

//----------------------------------------------------------------------------
//! Constructor with full addressing path information.

nxyter::FebUtil::FebUtil(FebBase* feb, QuickDaq* qdaq) :
  fFeb(feb),
  fQdaq(qdaq),
  fLoopState(kLoopStopped),
  fStartLoopTime(0.),
  fTestLoopTime(0.),
  fTestTime(0.),
  fLoopCount(0)
{
}

//----------------------------------------------------------------------------

nxyter::FebUtil::~FebUtil()
{
}

//----------------------------------------------------------------------------
//! Initializes test loop context.
/*!
 * The test loop logic is documented in detail under testLoop(). This method
 * initializes the context, resets the counter and time keeping logic.
 * \param time time in seconds the next test loop should be active. A \a time
 *          0. results in indefinite execution, in this case the loop is
 *          usually terminated via ^C and an interruptLoop() call or via a
 *          stopLoop() call.
 * \sa testLoop(), stopLoop(), interruptLoop(), getLoopCounter()
 */

void nxyter::FebUtil::startLoop(double time)
{
  fLoopState = kLoopRunning;
  fStartLoopTime = now();
  fTestTime = time;
  fLoopCount = 0;
}

//----------------------------------------------------------------------------
//! Terminate loop by interrupt.
/*!
 * When running the loop state is kLoopRunning. A call to this method will
 * change the loop state to kLoopInterrupted.  The testLoop() method will 
 * return \c false when the loop state is no longer kLoopRunning, thus 
 * causing the test loop to be terminated.
 *
 * Main application for this method is to be called from a signal handler,
 * often a SIGINT handler to catch a ^C. The code example below shows how
 * such a SIGINT handler can be implemented.
\code

void hdl_int(int sig, siginfo_t *info, void *cnxt)
{
  p_to_febutil->interruptLoop();
}


int main(int argc, char* argv[])
{
  struct sigaction act;
  memset (&act, '\0', sizeof(act));
  act.sa_sigaction = &hdl_int;
  act.sa_flags = SA_SIGINFO;
  if (sigaction(SIGINT, &act, 0) < 0) {
    perror ("sigaction");
  }
...
}
\endcode
 *
 * With such a handler test loops will be gracefully terminated upon ^C.
 *
 * \sa startLoop(), stopLoop(), testLoop()
 */

void nxyter::FebUtil::interruptLoop() 
{ 
  if (fLoopState == kLoopRunning) fLoopState = kLoopInterrupted;
}

//----------------------------------------------------------------------------
//! Terminate loop
/*!
 * When running the loop state is kLoopRunning. A call to this method will
 * change the loop state to kLoopStopped.  The testLoop() method will 
 * return \c false when the loop state is no longer kLoopRunning, thus 
 * causing the test loop to be terminated.
 *
 * By inspecting the loop state with getLoopState() it is possible to
 * determine whether the loop ended due to an interrupt or because
 * stopLoop() was called.
 *
 * \sa startLoop(), interruptLoop(), testLoop()
 */

void nxyter::FebUtil::stopLoop() 
{ 
  if (fLoopState == kLoopRunning) fLoopState = kLoopStopped;
}

//----------------------------------------------------------------------------
//! Iteration test in test loop.
/*!
 * This is the central method of the test loop environment. It allows to
 * implement test loops with run for a given amount of time. A call of
 * startLoop() initializes the loop context, the testLoop() method is
 * used as test condition of a \c while() loop. Simple example
\code
  gUtil->startLoop(10.);
  while(gUtil->testLoop()) {
    int rc = i2c.setRegister(reg, val, veri);
    if (rc) cout << Board::operErrToString(rc) << endl;
  }
\endcode
 * See stopLoop() of how to implement a loop abort via ^C.
 * \sa startLoop(), stopLoop(), getLoopCount()
 */

bool nxyter::FebUtil::testLoop()
{
  if (fLoopState != kLoopRunning) return false;  
  fTestLoopTime = now();
  if (fLoopCount>0 &&                       // if not first iteration and
      fTestTime>0. &&                       // finite time chosen check time
      fTestLoopTime > fStartLoopTime + fTestTime) return false;
  fLoopCount++;
  return true;
}

//----------------------------------------------------------------------------
//! Save context of all nXYTER.
/*!
 * The context of nXYTER of the current FEB which are not set 'offline'
 * is saved in the context array \a cntx. The \a domask determines which
 * parts are saved.
 *
 * \param cntx array of NxContext objects. cntx[i] will hold the saved
 *    context of the nXYTER with \ref glo_nxyter_nxind i.
 * \param domask one or several of the mask bits nxyter::kDoMask, 
 *     nxyter::kDoCore, or nxyter::kDoTrim
 * \returns see \ref roc_board_opererr.
 *
 * \sa restoreFeb()
 */

int nxyter::FebUtil::saveFeb(nxyter::NxContext cntx[4], int domask)
{
  if (fFeb==0) return -1;

  int rc;
  for (int i=0; i<fFeb->numNx(); i++) {
    if (fFeb->getNxOffline(i)) continue;    // skip if offline
    rc = fFeb->nx(i).i2c().getContext(cntx[i], domask);
    if (rc) return rc;
  }
  return 0;
}

//----------------------------------------------------------------------------
//! Restore context of all nXYTER.
/*!
 * The context of nXYTER of the current FEB which are not set 'offline'
 * is restored from the context array \a cntx. The \a domask determines which
 * parts are saved.
 *
 * \param cntx array of NxContext objects. cntx[i] is used to restore the
 *    context of the nXYTER with \ref glo_nxyter_nxind i.
 * \param domask one or several of the mask bits nxyter::kDoMask, 
 *     nxyter::kDoCore, or nxyter::kDoTrim
 * \returns see \ref roc_board_opererr.
 *
 * \sa saveFeb()
 */

int nxyter::FebUtil::restoreFeb(nxyter::NxContext cntx[4], int domask)
{
  if (fFeb==0) return -1;

  int rc;
  for (int i=0; i<fFeb->numNx(); i++) {
    if (fFeb->getNxOffline(i)) continue;    // skip if offline
    rc = fFeb->nx(i).i2c().setContext(cntx[i], domask, true);
    if (rc) return rc;
  }
  return 0;
}

//----------------------------------------------------------------------------
//! Acquire data for test trigger runs for a single nXYTER.
/*!
 * This method will
 * \li start a DAQ run with only the nXYTER given by \a nxind enabled
 * \li activate pulse generator with 2.5 kHz and \a npuls pulses
 * \li aim for collecting data for about \a npuls test pulses
 *
 * Only data received from the selected nXYTER will be accumulated in the
 * distribution function array \a dfa.
 *
 * \param nxind \ref glo_nxyter_nxind of the nXYTER to be inspected
 * \param npuls number of test pulses to be accumulated
 * \param dfa DistFuncArray object to return the data
 * \param cleardist if \c true \a dfa is cleared before filling 
 * \returns 0 on success, -1 if FEB or DAQ unavailable
 * \throws std::out_of_range if \a nxind out of range
 */

int nxyter::FebUtil::acquireTestTriggerData(int nxind, int npuls, 
                                            DistFuncArray& dfa, bool cleardist)
{  
  if (fFeb==0 || fQdaq==0) return -1;

  roc::Gpio     gpio(fFeb->getBoard());
  nxyter::RocNx rocnx(fFeb->getBoard());

  int nxnum   = fFeb->nx(nxind).getNxNumber();
  double time = double(npuls) / 2500.;      // FIXME: is that safe ??

  if (cleardist) dfa.clear();

  rocnx.fireTestPulse(0,0,0);               // FIXME: rc; ensure that pulser off

  uint32_t save_gpiomask;
  gpio.getConfig(save_gpiomask);            // FIXME: retcode
  uint32_t new_gpiomask = save_gpiomask;
  gpio.packConfig(new_gpiomask, 6, false, false, false, true, true);
  gpio.setConfig(new_gpiomask);             // FIXME: retcode

  fQdaq->nextRunNxMask(1<<nxnum);
  fQdaq->startRun(0, time);

  rocnx.fireTestPulse(100000,50000,npuls);   // FIXME: rc; fire pulser 2.5 KHz

  while(fQdaq->testRun()) {
    roc::Message& msg = fQdaq->msg();
    
    if (msg.isHitMsg()) {
      int num = msg.getNxNumber();
      if (num == nxnum) {
        int cha = msg.getNxChNum();
        int adc = msg.getNxAdcValue();
        dfa.addEntry(cha, float(adc));
      }
    }
  }

  if (fQdaq->stats().errorCount()) {
    fQdaq->stats().print(cout, 0., nxyter::QuickDaqStats::kPrintError);
  }

  gpio.setConfig(save_gpiomask);            // FIXME: retcode

  return 0;
}

//----------------------------------------------------------------------------
//! Aquire data for test trigger runs for a single nXYTER.
/*!
 * Like acquireTestTriggerData(int nxind, int npuls, DistFuncArray& dfa) but
 * returns already reduced data in a NxDataSummary object \a nds.
 */

int nxyter::FebUtil::acquireTestTriggerData(int nxind, int npuls, 
                                            NxDataSummary& nds)
{
  DistFuncArray dfa(128, npuls);
  dfa.setMaxEntries(2*npuls);
  int rc = acquireTestTriggerData(nxind, npuls, dfa);
  nds.analyse(dfa);
  return rc;
}

//----------------------------------------------------------------------------
//! Acquire data for test pulser runs for a single nXYTER.
/*!
 * This method will
 * \li start a DAQ run with only the nXYTER given by \a nxind enabled
 * \li activate pulse generator with 2.5 kHz and \a npuls pulses
 * \li aim for collecting data for about \a npuls test pulses
 *
 * Only data received from the selected nXYTER will be accumulated in the
 * distribution function array \a dfa.
 *
 * \param nxind \ref glo_nxyter_nxind of the nXYTER to be inspected
 * \param cntx context of nXYTER under study with Mask part filled
 * \param npuls number of test pulses to be accumulated
 * \param dfa DistFuncArray object to return the data
 * \returns see \ref roc_board_opererr, -1 if FEB or DAQ unavailable
 * \throws std::out_of_range if \a nxind out of range
 */

int nxyter::FebUtil::acquireTestPulserData(int nxind, const NxContext& cntx,
                                           int npuls, DistFuncArray& dfa)
{
  if (fFeb==0 || fQdaq==0) return -1;

  int rc;
  uint8_t conf1;
  rc = fFeb->nx(nxind).i2c().getRegister(kNxRegConfig1, conf1);
  if (rc) return rc;

  for (int csel=0; csel<=3; csel++) {
    NxContext cntx_run(cntx);

    // mask off all channels not covered by current csel
    for (int ch=0; ch<128; ch++) {
      if ((ch & 0x3) != csel) cntx_run.setChannelMaskBit(ch, true);
    }
    // no readback verification here to save the test trigger mode jojo
    // each time here. If there a comm problems, they also show elsewhere...
    rc = fFeb->nx(nxind).i2c().setContext(cntx_run, nxyter::kDoMask, false);
    if (rc) return rc;

    // now set csel in conf1
    conf1 &= ~kNxC1CalibSelectMask;
    conf1 |= uint8_t(csel);
    rc = fFeb->nx(nxind).i2c().setRegister(kNxRegConfig1, conf1, true);
    if (rc) return rc;

    rc = acquireTestTriggerData(nxind, npuls, dfa, csel==0);
    if (rc) return rc;
  }

  return 0;
}

//----------------------------------------------------------------------------
//! Aquire data for test pulser runs for a single nXYTER.
/*!
 * Like acquireTestPulserData(int nxind, NxContext cntx, int npuls, 
 * DistFuncArray& dfa) but returns already reduced data in a 
 * NxDataSummary object \a nds.
 */

int nxyter::FebUtil::acquireTestPulserData(int nxind, const NxContext& cntx, 
                                           int npuls, NxDataSummary& nds)
{
  DistFuncArray dfa(128, npuls);
  dfa.setMaxEntries(20*npuls);
  int rc = acquireTestPulserData(nxind, cntx, npuls, dfa);
  nds.analyse(dfa);
  return rc;
}

//----------------------------------------------------------------------------
//! Test nXYTER control path.
/*!
 * \param nxind \ref glo_nxyter_nxind
 * \param time test time in sec
 * \returns number of errors, or -1 if no FEB attached
 */

int nxyter::FebUtil::testNxControlPath(int nxind, double time)
{
  if (fFeb == 0) return -1;
  int errcnt = 0;
  startLoop(time);
  while(testLoop()) {
    int rc = fFeb->nx(nxind).i2c().probe();
    if (rc) {
      errcnt++;
      cout << errcnt << ":" << Board::operErrToString(rc) << endl;
    }
  }
  return errcnt;
}

//----------------------------------------------------------------------------
//! Test nXYTER registers.
/*!
 * \param nxind \ref glo_nxyter_nxind
 * \returns number of errors, or -1 if no FEB attached
 */

int nxyter::FebUtil::testNxRegisters(int nxind)
{
  if (fFeb == 0) return -1;
  int errcnt = 0;

  int rc;
  nxyter::NxContext cntx_save;

  rc = fFeb->nx(nxind).i2c().getContext(cntx_save, kDoAll);
  if (rc) {
    errcnt++;
    cout << errcnt << ": getContext({save}): "
         << Board::operErrToString(rc) << endl;
  }

  startLoop(0.);

  // part 1: mask and core registers, except configuration regs
  static const uint8_t reglist[] = 
    {  0,  1,  2,  3,  4,  5,  6,  7,       // masks
       8,  9, 10, 11, 12, 13, 14, 15,       // "
      16, 17, 18, 19, 20, 21, 22, 23,       // daq regs
      24, 25, 26, 27, 28, 29,               // "
      38, 39, 43, 44, 45                    // delay regs
    };
  static const int dim_reglist = sizeof(reglist)/sizeof(reglist[0]);
  
  static const uint8_t vallist[] =
    {0x00, 0xff,                                     // all 0, all 1
     0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, // 1 bit up
     0xaa, 0x55,                                     // 1010..., 0101...
     0xcc, 0x33, 0x66,                               // 1100..., 0011..., 0110..
     0x0f, 0xf0                                      // 00001111, 11110000
    };
  static const int dim_vallist = sizeof(vallist)/sizeof(vallist[0]);

  // loop over all test patterns
  for (int itest=0; itest<dim_vallist; itest++) {
    if (!testLoop()) break;

    // write all regs with test patters cyclic
    // this way all regs see all patterns, but regs have different values
    for (int ireg=0; ireg<dim_reglist; ireg++) {
      uint8_t reg = reglist[ireg];
      uint8_t val = vallist[(ireg+itest)%dim_vallist];
      rc = fFeb->nx(nxind).i2c().setRegister(reg, val, true);
      if (rc) {
        errcnt++;
        cout << errcnt << ": setRegister(" << uint32_t(reg)
             << "," << std::hex << uint32_t(val) << std::dec << ") : "
             << Board::operErrToString(rc) << endl;
      }
    }

    // read all regs check for proper values
    for (int ireg=0; ireg<dim_reglist; ireg++) {
      uint8_t reg = reglist[ireg];
      uint8_t valexp = vallist[(ireg+itest)%dim_vallist];
      uint8_t val;
      rc = fFeb->nx(nxind).i2c().getRegister(reg, val);
      if (rc) {
        errcnt++;
        cout << errcnt << ": getRegister(" << uint32_t(reg)
             << "," << std::hex << uint32_t(val) << std::dec << ") : "
             << Board::operErrToString(rc) << endl;
      }
      if (val != valexp) {
        errcnt++;
        cout << errcnt << ": getRegister(" << uint32_t(reg) << ") : "
             << " found: " << std::hex << uint32_t(val)
             << " expect: " << uint32_t(valexp)
             << std::dec << endl;
      }
    }
  }
       
  // part 2: configuration regs
  for (int reg=kNxRegConfig0; reg<=kNxRegConfig1; reg++) {
    if (!testLoop()) break;

    uint8_t valold;
    fFeb->nx(nxind).i2c().getRegister(reg, valold);

    for (int ival=0; ival<dim_vallist; ival++) {
      uint8_t val = vallist[(reg+ival)%dim_vallist];

      rc = fFeb->nx(nxind).i2c().setRegister(reg, val, true);
      if (rc) {
        errcnt++;
        cout << errcnt << ": setRegister(" << uint32_t(reg)
             << "," << std::hex << uint32_t(val) << std::dec << ") : "
             << Board::operErrToString(rc) << endl;
      }

      uint8_t valget;
      rc = fFeb->nx(nxind).i2c().getRegister(reg, valget);
      if (rc) {
        errcnt++;
        cout << errcnt << ": getRegister(" << uint32_t(reg)
             << "," << std::hex << uint32_t(val) << std::dec << ") : "
             << Board::operErrToString(rc) << endl;
      }

      if (valget != val) {
        errcnt++;
        cout << errcnt << ": getRegister(" << uint32_t(reg) << ") : "
             << " found: " << std::hex << uint32_t(valget)
             << " expect: " << uint32_t(val)
             << std::dec << endl;
      }

    }

    fFeb->nx(nxind).i2c().setRegister(reg, valold);
  }

  // part 3: trim registers
  nxyter::NxContext cntx_set;
  nxyter::NxContext cntx_get;

  static const uint8_t val5list[] =
    {0x00, 0x1f,                                     // all 0, all 1
     0x01, 0x02, 0x04, 0x08, 0x10,                   // 1 bit up
     0x15, 0x0a,                                     // 10101, 01010
     0x19, 0x0d, 0x06                                // 11001, 01101, 00110
    };
  static const int dim_val5list = sizeof(val5list)/sizeof(val5list[0]);
 
  for (int itest=0; itest<dim_val5list; itest++) {
    if (!testLoop()) break;

    for (int cha=0; cha<129; cha++) {
      uint8_t val = val5list[(cha+itest)%dim_val5list];
      cntx_set.setPowerOffMaskBit(cha, true);
      cntx_set.setThresholdTrim(cha, val);
    }

    rc = fFeb->nx(nxind).i2c().setContext(cntx_set, kDoTrim, false);
    if (rc) {
      errcnt++;
      cout << errcnt << ": setContext(,kDoTrim {trim check}): "
           << Board::operErrToString(rc) << endl;
    }
    rc = fFeb->nx(nxind).i2c().getContext(cntx_get, kDoTrim);
    if (rc) {
      errcnt++;
      cout << errcnt << ": getContext(,kDoTrim {trim check}): "
           << Board::operErrToString(rc) << endl;
    }

    int errtest = 0;
    for (int cha=0; cha<129; cha++) {
      uint8_t valexp = val5list[(cha+itest)%dim_val5list];
      uint8_t val = cntx_get.getThresholdTrim(cha);
      if (val != valexp) {
        errcnt++;
        errtest++;
        cout << errcnt << ": trim(" << cha << ") : "
             << " found: " << std::hex << uint32_t(val)
             << " expect: " << uint32_t(valexp)
             << std::dec << endl;
      }
    }

    if (errtest++) {
      cout << "--- set values for trim-daq test " << itest  << endl;
      cntx_set.print(cout, kDoTrim);
      cout << "--- get values for trim-daq test" << endl;
      cntx_get.print(cout, kDoTrim);
    }
  }
  
  // part 4: power-down registers
  // uses (cha/n)%2 ==0 or ==1 patterns for n=1,2,4,8,16,32,64,128,256

  for (int itest=0; itest<18; itest++) {
    if (!testLoop()) break;

    int ibit = itest%2;
    int idiv = 1<<(itest/2);
    for (int cha=0; cha<129; cha++) {
      bool val = (cha/idiv)%2 == ibit;
      cntx_set.setPowerOffMaskBit(cha, val);
      cntx_set.setThresholdTrim(cha, 16);
    }
    rc = fFeb->nx(nxind).i2c().setContext(cntx_set, kDoTrim, false);
    if (rc) {
      errcnt++;
      cout << errcnt << ": setContext(,kDoTrim {poff check}): "
           << Board::operErrToString(rc) << endl;
    }
    rc = fFeb->nx(nxind).i2c().getContext(cntx_get, kDoTrim);
    if (rc) {
      errcnt++;
      cout << errcnt << ": getContext(,kDoTrim {poff check}): "
           << Board::operErrToString(rc) << endl;
    }
  
    int errtest = 0;
    for (int cha=0; cha<129; cha++) {
      bool valexp = (cha/idiv)%2 == ibit;
      bool val = cntx_get.getPowerOffMaskBit(cha);
      if (val != valexp) {
        errcnt++;
        errtest++;
        cout << errcnt << ": power(" << cha << ") : "
             << " found: "  << val
             << " expect: " << valexp
             << std::dec << endl;
      }
    }
    if (errtest++) {
      cout << "--- set values for power-down test with ibit=" << ibit
           << " idiv=" << idiv << endl;
      cntx_set.print(cout, kDoTrim);
      cout << "--- get values for power-down test" << endl;
      cntx_get.print(cout, kDoTrim);
    }
  }

  rc = fFeb->nx(nxind).i2c().setContext(cntx_save, kDoAll);
  if (rc) {
    errcnt++;
    cout << errcnt << ": setContext({restore}): "
         << Board::operErrToString(rc) << endl;
  }

  return errcnt;
}

//----------------------------------------------------------------------------
//! Test MainAdc control path.
/*!
 * \param time test time in sec
 * \returns number of errors, or -1 if no FEB attached
 */

int nxyter::FebUtil::testMainAdcControlPath(double time)
{
  if (fFeb == 0) return -1;
  int errcnt = 0;
  startLoop(time);
  while(testLoop()) {
    int rc = fFeb->adc().probe();
    if (rc) {
      errcnt++;
      cout << errcnt << ":" << Board::operErrToString(rc) << endl;
    }
  }
  return errcnt;
}

//----------------------------------------------------------------------------
//! Test MainAdc data path.
/*!
 * \param time test time in sec
 * \returns number of errors, or -1 if no FEB attached
 */

int nxyter::FebUtil::testMainAdcDataPath(double time)
{
  if (fFeb == 0) return -1;
  int errcnt = 0;
  startLoop(time);
  int rc;
  // set all adc channels to test pattern mode
             rc = fFeb->adc().setTestMode(0, 8);
  if (rc==0) rc = fFeb->adc().setTestMode(1, 8);
  if (rc==0) rc = fFeb->adc().setTestMode(2, 8);
  if (rc==0) rc = fFeb->adc().setTestMode(3, 8);
  if (rc) {
    cout << "failed to set test pattern mode for ADC: "
         << Board::operErrToString(rc) << endl;
    cout << "Abort test because startup failed..." << endl;
    return 1;
  }

  uint16_t adcpatt = 0;
  int chmax = 2;
  
  while(testLoop()) {
    fFeb->adc().setUserPattern(adcpatt);
    for (int ch=0; ch<chmax; ch++) {
      uint32_t val;
      rc = fFeb->adc().getAdcDirect(ch, val);
      if (rc) {
        cout << errcnt << ": adc().getAdcDirect(): "
             << Board::operErrToString(rc) << endl;
        errcnt++;;
      }
      if ((uint16_t) val != adcpatt) {
        static format fmt("mismatch for adc chan=%|1|:"
                          " found=0x%|03x|, expected 0x%|03x|");
        cout << errcnt << ":"
             << fmt % ch % val % uint32_t(adcpatt)
             << endl;
        errcnt++;
      }
    }
    adcpatt = (adcpatt+1) & 0x0fff;
  }

  // reset all adc channels to normal mode
             rc = fFeb->adc().setTestMode(0, 0);
  if (rc==0) rc = fFeb->adc().setTestMode(1, 0);
  if (rc==0) rc = fFeb->adc().setTestMode(2, 0);
  if (rc==0) rc = fFeb->adc().setTestMode(3, 0);
  if (rc) {
    cout << "failed to restore ADC default settings: "
         << Board::operErrToString(rc) << endl;
    errcnt++;
  }

  return errcnt;
}

//----------------------------------------------------------------------------
//! Test control path of all nXYTER of a FEB with random patterns.
/*!
 * \param time test time in sec
 * \returns number of errors, or -1 if no FEB attached
 * \todo add ADC test too.
 */

int nxyter::FebUtil::testFebControlPathCombo(double time)
{
  if (fFeb == 0) return -1;

  nxyter::NxContext cntx_save[4];
  nxyter::NxContext cntx_test[4];
  int errcnt = 0;
  int numnx = fFeb->numNx();

  int nxreg [] = {0,1, 2, 3, 4, 5, 6, 7,    // all mask regs
                  8,9,10,11,12,13,14,15,
                  kNxRegcal,                // 2 daq regs
                  kNxRegiTWC,
                  kNxRegdelayTestPuls,      // 2 delay regs
                  kNxRegdelayTestTrig
  };      

  const static int dim_nxreg = sizeof(nxreg)/sizeof(int);

  uint8_t testpatt[4][dim_nxreg];

  // save nx mask+core register set
  for (int i=0; i<numnx; i++) {
    int rc = fFeb->nx(i).i2c().getContext(cntx_save[i], 
                                          nxyter::kDoMask|nxyter::kDoCore);
    if (rc) {
      cout << "failed to save context for nxind=" << i << " :"
           << Board::operErrToString(rc) << endl;
      errcnt++;
    }
  }

  if (errcnt) {
    cout << "Abort test because startup failed..." << endl;
    return errcnt;
  }

  startLoop(time);
  while(testLoop()) {
    int rc;
    
    for (int i=0; i<numnx; i++) {
      cntx_test[i] = cntx_save[i];
      for (int j=0; j<dim_nxreg; j++) {
        testpatt[i][j] = (random()>>22) & 0xff; // take bits 22 to 29
        cntx_test[i].setRegister(nxreg[j], testpatt[i][j]);
      }
      rc = fFeb->nx(i).i2c().setContext(cntx_test[i], 
                                        nxyter::kDoMask|nxyter::kDoCore);
      if (rc) {
        cout << errcnt << ":setContext() on " << i << " :"
             << Board::operErrToString(rc) << endl;
        errcnt++;
      }
    }

    for (int i=0; i<numnx; i++) {
      rc = fFeb->nx(i).i2c().getContext(cntx_test[i],
                                        nxyter::kDoMask|nxyter::kDoCore);
      if (rc) {
        cout << errcnt << ":getContext() on " << i << " :"
             << Board::operErrToString(rc) << endl;
        errcnt++;
      }
      for (int j=0; j<dim_nxreg; j++) {
        uint8_t val = cntx_test[i].getRegister(nxreg[j]);
        if (val != testpatt[i][j]) {
          static format fmt("mismatch for nxind=%|1|,reg=%|2|:"
                            " found=0x%|02x|, expected 0x%|02x|");
          cout << errcnt << ":"
               << fmt % i % nxreg[j] % uint32_t(val) % uint32_t(testpatt[i][j])
               << endl;
          errcnt++;
        }
      }
    }    
  }

  // restore nx mask+core register set
  for (int i=0; i<numnx; i++) {
    int rc = fFeb->nx(i).i2c().setContext(cntx_save[i],
                                          nxyter::kDoMask|nxyter::kDoCore);
    if (rc) {
      cout << "failed to restore context for nxind=" << i << " :"
           << Board::operErrToString(rc) << endl;
      errcnt++;
    }
  }

  return errcnt;
}

//----------------------------------------------------------------------------
//! Probes for ADC and nXYTERs and prints a report to \a os.
/*!
 * Uses the nxyter::MainAdc::probe() and nxyter::NxI2c::probe() and
 * FebBase::discoverFebs() methods to scan both ports of the ROC. 
 * Currently the even I2C addresses between 2 and 126 are probed,
 * this is the range conceptually accessible in the current FEBs.
 *
 * When \a verbose is \c true, a detailed list of all probe return codes
 * is given, it can be helpfull to determine why a device isn't probed
 * successfully. A typical list looks like
\verbatim
  adc  0    : rc =    8,20 -> readback verify error
  nx   0,  8: rc =    0,21 -> secondary bus error
...
  adc  1    : rc =    0, 0 -> success
  nx   1,  8: rc =    0, 0 -> success
  nx   1, 10: rc =    6,20 -> readback verify error
...
\endverbatim
 *
 * The report ends always with a summary of the form like
\verbatim
Full MainAdc.probe() and NxI2c::probe() scan found:
  port adc ----- nx i2c slave address in range 2,..,126, only even -------
                                                            11111111111111
           000011111222223333344444555556666677777888889999900000111112222
           246802468024680246802468024680246802468024680246802468024680246
    0   y  ---y-----------------------------------------------------------
    1   -  ---------------------------------------------------------------
Autoconfigure with FebBase::discoverFebs() found:
  port 0 : Feb1nxGenC
  port 1 : nothing
\endverbatim
 *
 * \param os output stream
 * \param board ROC board pointer
 * \param verbose if \c true, the return codes of all probes are listed
 */

void nxyter::FebUtil::probe(roc::Board* board, bool verbose, std::ostream& os)
{
  bool nxok[2][128];
  bool adcok[2];
  
  if (verbose) os << "Detailed probe() return codes:" << endl;

  for (int port=0; port<=1; port++) {
    nxyter::MainAdc adc(board, port);
    int rc = adc.probe();
    adcok[port] = (rc==0);
    if (verbose) {
      os << format("  adc %|2|    : rc = %|4|,%|2| -> %s") % 
        port % Board::operErrIndex(rc) % Board::operErrCode(rc) %
        Board::operErrCodeName(rc) << endl;
    }

    for (int addr=2; addr<=126; addr+=2) {
      nxyter::NxI2c nx(board, port, addr);
      rc = nx.probe();
      nxok[port][addr] = (rc==0);
      if (verbose) {
        os << format("  nx  %2d,%3d: rc = %4d,%2d -> %s") % 
          port % int(addr) % Board::operErrIndex(rc) % 
          Board::operErrCode(rc) %
          Board::operErrCodeName(rc) << endl;
      }
    }

  }

  os << "Full MainAdc.probe() and NxI2c::probe() scan found:" << endl;
  os << "  port adc ----- nx i2c slave address in range 2,..,126, only even -------" << endl;
  os << "           ";
  for (int addr=2; addr<=126; addr+=2) os << ( (addr>=100)?"1":" " );
  os << endl;
  os << "           ";
  for (int addr=2; addr<=126; addr+=2) os << ((addr/10)%10);
  os << endl;
  os << "           ";
  for (int addr=2; addr<=126; addr+=2) os << (addr%10);
  os << endl;

  for (int port=0; port<=1; port++) {
    os << "    " << port << "   " << (adcok[port]?"y":"-") << "  ";
    for (int addr=2; addr<=126; addr+=2) os << (nxok[port][addr] ? "y" : "-");
    os << endl;
  }

  int ftyps[2], ftypl[2];
  FebBase::discoverFebs(board, ftyps[0], ftyps[1]);
  FebBase::discoverFebs(board, ftypl[0], ftypl[1], true);

  os << "Autoconfigure with FebBase::discoverFebs() found:" << endl;
  for (int port=0; port<=1; port++) {
    os << "  port " << port << " : ";
    if (ftyps[port]==0 && ftypl[port]==0) {
      os << "nothing";
    } else if (ftyps[port] == ftypl[port]) {
      os << FebBase::typeToString(ftyps[port]);
    } else {
      if (ftyps[port])
        os << FebBase::typeToString(ftyps[port]) 
           << " (strict mode)";
      if (ftypl[port])
        os << FebBase::typeToString(ftypl[port]) 
           << " (liberal mode, some nx missing)";
    }
    os << endl;
  }

}

//----------------------------------------------------------------------------
//! Returns current time as a \c double.

double nxyter::FebUtil::now()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return double(tv.tv_sec) + 1.e-6*double(tv.tv_usec);
}

//----------------------------------------------------------------------------
