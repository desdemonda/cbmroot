//============================================================================
/*! \file FebUtil.h
 *  \author W.F.J.Mueller/GSI, merged in functionality from other places
 *  \author N.Abel/KIP, calibration methods
 */
//============================================================================

#ifndef NXYTER_FEBUTIL_H
#define NXYTER_FEBUTIL_H

#include <iostream>

#include "roc/Board.h"
#include "nxyter/FebBase.h"
#include "nxyter/NxContext.h"
#include "nxyter/QuickDaq.h"
#include "nxyter/DistFuncArray.h"
#include "nxyter/NxDataSummary.h"

namespace nxyter {

   class FebUtil {
      protected:
         FebBase*   fFeb;                   //!< FEB under test
         QuickDaq*  fQdaq;                  //!< QuickDaq for all DAQ
         int        fLoopState;             //!< loop state
         double     fStartLoopTime;         //!< time of last startLoop()
         double     fTestLoopTime;          //!< time of last testLoop()
         double     fTestTime;              //!< maximal duration of test
         uint64_t   fLoopCount;             //!< test loop counter

      public:
         enum LoopState { kLoopStopped = 0,
                          kLoopInterrupted,
                          kLoopRunning
         };

         //! Container for run conditions for acquireData()
         struct RunCond {
            uint32_t npuls;                 //!< number of test pulses
            uint32_t pulsperiod;            //!< pulse period (in 4 ns units)
            int32_t pulswidth;              //!< pulse width (in 4 ns units)
            RunCond() : npuls(0), pulsperiod(0), pulswidth(0)
            {}
         };


         explicit FebUtil(FebBase* feb, QuickDaq* qdaq=0);
         virtual ~FebUtil();

         void startLoop(double time);
         bool testLoop();
         void interruptLoop();
         void stopLoop();

         //! Returns the number of test loop iterations.
         /*!
          * \returns number of \c true responses of testLoop() after the
          * last startLoop().
          * \sa testLoop(), startLoop()
          */
         uint64_t getLoopCount() const
           { return fLoopCount; }

         //! Returns the time of last startLoop()
         double getStartLoopTime() const
           { return fStartLoopTime; }

         //! Returns the time of last testLoop()
         double getTestLoopTime() const
           { return fStartLoopTime; }

         //! Returns loop state (kLoopStopped, kLoopInterrupted or kLoopRunning)
         int getLoopState() const
           { return fLoopState; }

         int saveFeb(NxContext cntx[4], int domask=nxyter::kDoAll);
         int restoreFeb(NxContext cntx[4], int domask=nxyter::kDoAll);

         int acquireTestTriggerData(int nxind, int npuls, DistFuncArray& dfa,
                                    bool cleardist=true);
         int acquireTestTriggerData(int nxind, int npuls, NxDataSummary& nds);

         int acquireTestPulserData(int nxind, const NxContext& cntx,
                                   int npuls, DistFuncArray& dfa);
         int acquireTestPulserData(int nxind, const NxContext& cntx,
                                   int npuls, NxDataSummary& nds);
         
         int testNxControlPath(int nxind, double time);
         int testNxRegisters(int nxind);
         int testMainAdcControlPath(double time);
         int testMainAdcDataPath(double time);
         int testFebControlPathCombo(double time);

         static void probe(roc::Board* board, bool verbose=true,
                           std::ostream& os=std::cout);
         static double now();
   };

}

#endif
