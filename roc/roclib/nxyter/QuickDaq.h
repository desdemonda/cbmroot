//============================================================================
/*! \file QuickDaq.h
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#ifndef NXYTER_QUICKDAQ_H
#define NXYTER_QUICKDAQ_H

#include "roc/Board.h"
#include "roc/Message.h"
#include "roc/Iterator.h"
#include "nxyter/FebBase.h"
//#include "nxyter/Data.h"
#include "nxyter/QuickDaqStats.h"
#include "nxyter/RocNx.h"

namespace nxyter {     

   class QuickDaq {
      protected:
         roc::Board* fBoard;                //!< board pointer
         nxyter::FebBase* fFebs[2];         //!< available FEBs
         nxyter::RocNx fRocNx;              //!< rocnx pointer
         int fState;                        //!< DAQ state
         int fStopCode;                     //!< DAQ stop code
         int fDaqLowWater;                  //!< for setRocLowHighWater
         int fDaqHighWater;                 //!< for setRocLowHighWater
         uint32_t fDaqFlushTimer;           //!< for setRocBufferFlushTimer
         double fDaqStaStoTimeout;          //!< timeout for start/stop
         QuickDaqStats fStats;              //!< statistic counters
         bool   fNextNxMaskActive;          //!< nextRunNxMask() called
         int    fNextNxMask;                //!< nXYTER mask for next run
         double fStartTime;                 //!< time start call
         double fStopTime;                  //!< time stop call
         double fStartMsgTime;              //!< time start message
         double fStopMsgTime;               //!< time stop message
         double fMaxMsg;                    //!< maximal number of msg in run
         double fMaxTime;                   //!< maximal time of run
         roc::Iterator fIter;               //!< current iterator
         double fMsgTime;                   //!< current message time stamp
         double fNowTime;                   //!< wall time of message

      public:

         //! Names for DAQ states
         enum daqState {
            kDaqStopped  = 0,               //!< stopped, not active
            kDaqStarting,                   //!< started, before start sysmsg
            kDaqRunning,                    //!< started, start sysmsg received
            kDaqStopping                    //!< rundown, before stopped sysmsg
         };
         //! Names for DAQ stop codes
         enum daqStopCode {
            kDaqStopPending  = 0,           //!< DAQ active or never started
            kDaqStopByCount,                //!< stop due to message count
            kDaqStopByTime,                 //!< stop due to time expired
            kDaqStopByInterrupt,            //!< stop due to interrupt()
            kDaqStopByStop,                 //!< stop due to stop()
            kDaqStopByTimeout               //!< stop due to timeout
         };

         explicit QuickDaq(roc::Board* board);
         explicit QuickDaq(nxyter::FebBase* feb0=0,
                           nxyter::FebBase* feb1=0);
         virtual ~QuickDaq();

         int setNxTestModes(bool testpuls, bool testtrig, int calselect); 
         int setNxRegisterAll(uint8_t reg, uint8_t val, bool veri=true); 

         void nextRunNxMask(int nxmask);

         int startRun(int maxmsg=0, double maxtime=0., bool noinit=false);
         int stopRun(bool hardstop=false);
         void interruptRun();

         bool testRun(double timeout=0.);

         //! Access current message data object
         roc::Message& msg()
           { return fIter.msg(); }

         //! Returns current epoch
         uint32_t getMsgEpoch() const
           { return fIter.getMsgEpoch(); }

         //! Returns full message time as \c double
         double getMsgTime()
           { return fMsgTime; }

         //! Returns wall clock time of message (measured since start run)
         double getRunTime()
           { return fNowTime - fStartTime; }

         //! Returns start time
         double getStartTime()
           { return fStartTime; }

         //! Returns stop time
         double getStopTime()
           { return fStopTime; }

         //! Access statistics object
         const QuickDaqStats& stats()
           { return fStats; }

         //! Returns DAQ state
         int getState()
           { return fState; }

         //! Returns DAQ stop code
         int getStopCode()
           { return fStopCode; }

         static double now();

   };

}

#endif
