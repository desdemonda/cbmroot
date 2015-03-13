//============================================================================
/*! \file QuickDaqStats.h
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#ifndef NXYTER_QUICKDAQSTATS_H
#define NXYTER_QUICKDAQSTATS_H

#include <iostream>
#include <vector>

//#include "nxyter/Data.h"
#include "roc/Message.h"

namespace nxyter {     

   class QuickDaqStats {
      protected:
         std::vector<double> fStats;        //!< statistic counters

      public:

         //! Names for statistic counter indices
         enum daqStat {
            kNMessage = 0,                  //!< messages received
            kNNop,                          //!< nop messages
            kNHit,                          //!< nXYTER hits received
            kNHitPileup,                    //!< hits with pileup
            kNHitOverflow,                  //!< hits with overflow
            kNHitNx0,                       //!< hits on nx:0
            kNHitNx1,                       //!< hits on nx:1
            kNHitNx2,                       //!< hits on nx:2
            kNHitNx3,                       //!< hits on nx:3
            kNEpoch,                        //!< epoch markers
            kNEpochWithMiss,                //!< epochs with missed hits
            kNMissedHit,                    //!< missed hits (low bound)
            kNSync0,                        //!< sync0 messages
            kNSync1,                        //!< sync1 messages
            kNAux0,                         //!< aux0 messages
            kNaux1,                         //!< aux1 messages
            kNAux2,                         //!< aux2 messages
            kNAux3,                         //!< aux3 messages
            kNAuxOverflow,                  //!< aux with pileup
            kNSysMesNxParErr0,              //!< nx:0 parity errors
            kNSysMesNxParErr1,              //!< nx:1 parity errors
            kNSysMesNxParErr2,              //!< nx:2 parity errors
            kNSysMesNxParErr3,              //!< nx:3 parity errors
            kNSysMesSyncParErr0,            //!< sync0 parity errors
            kNSysMesSyncParErr1,            //!< sync1 parity errors
            kNSysMesDaqRes,                 //!< daq resume messages
            kNSysMesFifoReset,              //!< fifo reset messages
            kNSysMesOther,                  //!< other system messages
            kNEpoch2,                       //!< Get4 epoch markers
            kNGet4Hit,                      //!< Get4 hit message
            kNGet4Event,                    //!< Get4 event sys message
            kNOther,                        //!< other message types
            kDimDaqStat                     //!< dimension = highest index + 1 
         };

         //! Names for print levels
         enum plevel {
           kPrintAll = 0,                   //!< print all stats counters
           kPrintNonZero,                   //!< print only non-zero counters
           kPrintError                      //!< print only error counters
         };
         
         QuickDaqStats();
         QuickDaqStats(const QuickDaqStats& obj);
         virtual ~QuickDaqStats();

         void clear();
         void increment(roc::Message& msg);

         double errorCount() const;

         double operator[](int ind) const;
         QuickDaqStats& operator=(const QuickDaqStats& rhs);
         QuickDaqStats& operator-(const QuickDaqStats& rhs);
         QuickDaqStats& operator*(double rhs);

         void print(std::ostream& os, double dtime=0., 
                    int lvl=kPrintNonZero) const;
         static const char* counterName(int ind);

      private:
         //! Increment counter with index \a ind.
         void inc(int ind)
           { fStats[ind] += 1.; }

   };


}

#endif
