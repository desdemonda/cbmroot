//============================================================================
/*! \file QuickDaq.cxx
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#include <sys/time.h>
#include <time.h>

#include <iostream>
#include <stdexcept>

#include "roc/UdpBoard.h"

#include "nxyter/QuickDaq.h"

using std::cout;
using std::endl;

using roc::Board;


/*!
 * \class nxyter::QuickDaq
 * \brief Simple DAQ environent for setups with a single ROC and 0-2 FEBs.
 *
 */

//----------------------------------------------------------------------------
//! Constructor with Board and no FEB
/*!
 * Sets up \c QuickDaq object with no FEB.
 * \param board ROC board pointer
 */

nxyter::QuickDaq::QuickDaq(roc::Board* board) :
  fBoard(board),
  fRocNx(board),
  fState(kDaqStopped),
  fStopCode(kDaqStopPending),
  fDaqLowWater(250),                        // 250 kB low water
  fDaqHighWater(500),                       // 500 kB high water
  fDaqFlushTimer(100),                      // 100 msec flush timer
  fDaqStaStoTimeout(1.),                    // 1 sec start/stop timeout
  fNextNxMaskActive(false), fNextNxMask(0),
  fStartTime(0.), fStopTime(0.),
  fStartMsgTime(0.), fStopMsgTime(0.),
  fMaxMsg(0.), fMaxTime(0.),
  fIter(board),
  fMsgTime(0.), fNowTime(0.)
{
  fFebs[0] = 0;
  fFebs[1] = 0;
}

//----------------------------------------------------------------------------
//! Normal constructor with one or two FEB's
/*!
 * Sets up \c QuickDaq object with full board and FEB context.
 * \param feb0  1st connected FEB (maybe 0)
 * \param feb1  2nd connected FEB (maybe 0)
 * \throws runtime_error when both \a feb0 and \a feb1 or null pointers
 *     or if two febs given both not hosted by the same board.
 *
 */


nxyter::QuickDaq::QuickDaq(nxyter::FebBase* feb0, nxyter::FebBase* feb1) :
  fBoard(feb0 ? feb0->getBoard() : (feb1 ? feb1->getBoard() : 0)),
  fRocNx(fBoard),
  fState(kDaqStopped),
  fStopCode(kDaqStopPending),
  fDaqLowWater(250),                        // 250 kB low water
  fDaqHighWater(500),                       // 500 kB high water
  fDaqFlushTimer(100),                      // 100 msec flush timer
  fDaqStaStoTimeout(1.),                    // 1 sec start/stop timeout
  fNextNxMaskActive(false), fNextNxMask(0),
  fStartTime(0.), fStopTime(0.),
  fStartMsgTime(0.), fStopMsgTime(0.),
  fMaxMsg(0.), fMaxTime(0.),
  fIter(fBoard),
  fMsgTime(0.),
  fNowTime(0.)
{
  if (!fBoard)
    throw std::runtime_error("not any FEB specified");

  if (feb0 && feb1 && (feb0->getBoard() != feb1->getBoard()))
    throw std::runtime_error("FEB not hosted by same board");

   fFebs[0] = feb0;
   fFebs[1] = feb1;
}

//----------------------------------------------------------------------------
/*!
 * \todo recheck dtor logic, is buffer drain needed with DABC (it is with KNUT)
 */

nxyter::QuickDaq::~QuickDaq()
{
  // if still running, stop it
  if (fState == kDaqRunning) {
    stopRun(true);
  }
}

//----------------------------------------------------------------------------
//! Setup <em>test pulser</em> and <em>test trigger</em> mode in all nXYTERs.
/*!
 * Calls for all nXYTERs in the setup the NxI2c::setTestModes() method, check
 * the documentation of this method for details.
 *
 * \param testtrig determines whether <em>test trigger</em> mode is enabled
 * \param testpuls determines whether <em>test pulser</em> mode is enabled
 * \param calselect determines which channel group receives test pulse
 * \returns see \ref roc_board_opererr
 */

int nxyter::QuickDaq::setNxTestModes(bool testpuls, bool testtrig, 
                                     int calselect)
{
  for (int i=0; i<2; i++) {
    if (fFebs[i]) {
      int rc = fFebs[i]->setNxTestModes(testpuls, testtrig, calselect);
      if (rc) return Board::operErrBuildInc(rc, 10*i);
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
//! Write a value into a register of all nXYTERs.
/*!
 * Writes \a val into register \a reg of all nXYTERs in the setup.
 * \param reg register number
 * \param val value to be written
 * \param veri if \c true readback verification done (default is \c true)
 * \returns see \ref roc_board_opererr
 */

int nxyter::QuickDaq::setNxRegisterAll(uint8_t reg, uint8_t val, bool veri)
{
  for (int i=0; i<2; i++) {
    if (fFebs[i]) {
      int rc = fFebs[i]->setNxRegisterAll(reg, val, veri);
      if (rc) return Board::operErrBuildInc(rc, 10*i);
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
//! Set active nXYTER mask for next run.

void nxyter::QuickDaq::nextRunNxMask(int nxmask)
{
  fNextNxMaskActive = true;
  fNextNxMask = nxmask;
}

//----------------------------------------------------------------------------
//! Prepare for data taking, start a run.
/*!
 *
 */

int nxyter::QuickDaq::startRun(int maxmsg, double maxtime, bool noinit)
{
  fMaxMsg  = double(maxmsg);
  fMaxTime = maxtime;
  fStats.clear();

  stopRun(true);

  if (!fBoard->stopDaq()) {
     fBoard->Debug(-1, "Cannot StopDaq - break QuickDaq");
     return 1;
  }

  if (noinit == false) {
    fRocNx.setNxActive(0);
    for (int i=0; i<2; i++) {
      if (fFebs[i]) fFebs[i]->initRoc();
    }
    if (fNextNxMaskActive) {
      uint32_t rocnxmask;
      fRocNx.getNxActive(rocnxmask);
      fRocNx.setNxActive(rocnxmask & fNextNxMask);
    }
  }


  // set timeout to flush data from internal buffer of driver
  // should reduce time for transfer of last data before stop daq
  roc::UdpBoard* udp = dynamic_cast<roc::UdpBoard*> (fBoard);
  if (udp) {
     udp->setRocLowHighWater(fDaqLowWater, fDaqHighWater);
     udp->setRocBufferFlushTimer(fDaqFlushTimer);
     udp->setFlushTimeout(fDaqFlushTimer>10 ? fDaqFlushTimer/1000. : 0.01);
  } else
     fBoard->setFlushTimeout(0.01);

  if (fBoard->uploadStartDaqCmdList(!noinit)) {
     fBoard->Debug(-1, "Cannot init startdaq sequence - break QuickDaq");
     return 1;
  }

  if (!fBoard->startDaq()) {
     fBoard->Debug(-1, "Cannot StartDaq - break QuickDaq");
     return 1;
  }

  fState = kDaqStarting;
  fStopCode = kDaqStopPending;
  fStartTime = now();

  if (fBoard->isFile()) {
     fState = kDaqRunning;
     fStartMsgTime = now();
  }

  // now reset all 'next run' conditions
  fNextNxMaskActive = false;
  fNextNxMask = 0;

  return 0;
}

//----------------------------------------------------------------------------
//! End data taking.
/*!
 *
 */

int nxyter::QuickDaq::stopRun(bool hardstop)
{
  if (fState == kDaqStarting || fState == kDaqRunning) {
    fBoard->suspendDaq();                     // FIXME ret codes
    fState = kDaqStopping;
    if (fStopCode == kDaqStopPending) fStopCode = kDaqStopByStop;
    fStopTime = now();
  }
  
  if (hardstop) {
    // for a hard stop, make an effort to drain the buffers
    // this is needed when used with KNUT
    // FIXME: do we need this with DABC transport ?
    while ((fState == kDaqStopping) && (now() < fStopTime+0.1)) {
      bool valid = fIter.next(0.05);
      if (!valid) break;
    }
  }
  
  return 0;
}

//----------------------------------------------------------------------------
//! Interrupt data taking.
/*!
 *
 * Main application for this method is to be called from a signal handler,
 * often a SIGINT handler to catch a ^C. The code example below shows how
 * such a SIGINT handler can be implemented.
\code

void hdl_int(int sig, siginfo_t *info, void *cnxt)
{
  p_to_quickdaq->interruptRun();
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
 * With such a handler DAQ runs will be gracefully terminated upon ^C.
 *
 */

void nxyter::QuickDaq::interruptRun()
{
  // Note: this will typically called from a SIGINT signal handler
  //       thus simply set a flag, but no other calls...
  if (fState != kDaqStopped) fStopCode = kDaqStopByInterrupt;
}

//----------------------------------------------------------------------------
//! Check run status

bool nxyter::QuickDaq::testRun(double timeout)
{
  double timein = 0.;

  while(true) {

    fNowTime = now();
    if (timein == 0.) timein = fNowTime;
    
    // if daq stopped, quit
    if (fState == kDaqStopped) return false;

    // if interrupted, cleanup and quit
    if (fStopCode == kDaqStopByInterrupt) {
      stopRun(true);
      return false;
    }

    // daq not starting, bail out
    if (fState==kDaqStarting && fNowTime>=fStartTime+fDaqStaStoTimeout) {
      cout << "+++ daq not starting, bail out" << endl;
      fStopCode == kDaqStopByTimeout;
      stopRun(true);
      return false;
    }
    
    // daq not stopping, bail out
    if (fState==kDaqStopping && fNowTime>=fStopTime+fDaqStaStoTimeout) {
      cout << "+++ daq not stopping, bail out" << endl;
      fStopCode == kDaqStopByTimeout;
      stopRun(true);
      return false;
    }
    
    // check for message limit
    if (fMaxMsg > 0. &&
        fStats[nxyter::QuickDaqStats::kNMessage] >= fMaxMsg) {
      if (fStopCode == kDaqStopPending) fStopCode == kDaqStopByCount;
      stopRun(true);
      return false;
    }

    // check for time limit
    if (fState==kDaqRunning) {
      if ((fMaxTime > 0.) && (fNowTime >= fStartMsgTime+fMaxTime)) {
        stopRun();
        if (fStopCode == kDaqStopPending) fStopCode == kDaqStopByTime;
      }
    }
    
    bool valid = fIter.next(0.02);

    if (valid) {

      roc::Message& msg = fIter.msg();
      fStats.increment(msg);
      
      if (msg.isStartDaqMsg()) {
        fState = kDaqRunning;
        fStartMsgTime = now();
      } else if (msg.isStopDaqMsg()) {
        fState = kDaqStopped;
        fStopMsgTime = now();
      } 
      
      fMsgTime = fIter.getMsgFullTimeD() * 1.e-9;

    } else {

      if (fBoard->isFile()) {
         fState = kDaqStopped;
         fStopMsgTime = now();
         return true;
      }
    }

    double time = now();
    bool tmo = (timeout > 0. && time-timein > timeout);
    fNowTime = time;

    if (valid || tmo) return true;
  }
}

//----------------------------------------------------------------------------
//! Returns current time as a \c double.

double nxyter::QuickDaq::now()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return double(tv.tv_sec) + 1.e-6*double(tv.tv_usec);
}

//----------------------------------------------------------------------------
