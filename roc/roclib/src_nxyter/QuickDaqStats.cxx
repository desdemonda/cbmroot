//============================================================================
/*! \file QuickDaqStats.cxx
 *  \author W.F.J.Mueller/GSI
 */
//============================================================================

#include "boost/format.hpp"

#include "nxyter/QuickDaqStats.h"

using boost::format;

/*!
 * \class nxyter::QuickDaqStats
 * \brief Set of statistic counters for QuickDaq environment
 *
 */

//----------------------------------------------------------------------------
//! Constructor, clears all counters.

nxyter::QuickDaqStats::QuickDaqStats() :
  fStats(kDimDaqStat)                       // allocate vector
{
}

//----------------------------------------------------------------------------
//! Copy constructor.

nxyter::QuickDaqStats::QuickDaqStats(const QuickDaqStats& obj) :
  fStats(obj.fStats)                        // allocate and copy vector
{
}

//----------------------------------------------------------------------------

nxyter::QuickDaqStats::~QuickDaqStats()
{
}

//----------------------------------------------------------------------------
//! Clear all counters.

void nxyter::QuickDaqStats::clear()
{
  for (int i=0; i<kDimDaqStat; i++) fStats[i] = 0.;
}

//----------------------------------------------------------------------------
//! Increment counters according to the contents of \a data.

void nxyter::QuickDaqStats::increment(roc::Message& msg)
{
  int synnum;
  int auxnum;
  int nxnum;

  inc(kNMessage);

  switch(msg.getMessageType()) {

    case roc::MSG_NOP:
      inc(kNNop);
      break;

    case roc::MSG_HIT:
      inc(kNHit);
      if (msg.getNxPileup())   inc(kNHitPileup);
      if (msg.getNxOverflow()) inc(kNHitOverflow);
      nxnum = msg.getNxNumber() & 0x3;
      inc(kNHitNx0 + nxnum);
      break;

    case roc::MSG_EPOCH:
      inc(kNEpoch);
      if (msg.getEpochMissed()) {
        inc(kNEpochWithMiss);
        fStats[kNMissedHit] += double(msg.getEpochMissed());
      }
      break;

    case roc::MSG_SYNC:
      synnum = msg.getSyncChNum() & 0x1; // FIXME: hack to map to 0:1
      inc(kNSync0 + synnum);
      break;

    case roc::MSG_AUX:
      auxnum = msg.getAuxChNum() & 0x3;  // FIXME: hack to map to 0:3
      inc(kNAux0 + auxnum);
      if (msg.getAuxOverflow()) inc(kNAuxOverflow);
      break;
      
    case roc::MSG_EPOCH2:   
      inc(kNEpoch2);
      break;
     
    case roc::MSG_GET4:
      inc(kNGet4Hit);
      break;

    case roc::MSG_SYS:
      switch(msg.getSysMesType()) {
        case roc::SYSMSG_NX_PARITY:
          nxnum = msg.getNxNumber() & 0x3;
          inc(kNSysMesNxParErr0 + nxnum);
          break;
        case roc::SYSMSG_SYNC_PARITY:
          nxnum = msg.getNxNumber() & 0x1;
          inc(kNSysMesSyncParErr0 + nxnum);
          break;
        case roc::SYSMSG_DAQ_RESUME:
          inc(kNSysMesDaqRes);
          break;
        case roc::SYSMSG_FIFO_RESET:
          inc(kNSysMesFifoReset);
          break;
        case roc::SYSMSG_GET4_EVENT:
          inc(kNGet4Event);
          break;
        default:
          inc(kNSysMesOther);
          break;
      }
      break;

    default:
      inc(kNOther);
      break;
  }
  
}

//----------------------------------------------------------------------------
//! Returns number of nXYTER and sync parity errors.

double nxyter::QuickDaqStats::errorCount() const
{
  double ecnt = 0.;
  for (int i=kNSysMesNxParErr0; i<=kNSysMesSyncParErr1; i++) {
    ecnt += fStats[i];
  }
  return ecnt;
}

//----------------------------------------------------------------------------
//! Returns counter with index \a ind.
/*!
 * \param ind index of counter to be returned
 * \returns counter with index \a ind, or 0. if \a ind is out of range
 */

double nxyter::QuickDaqStats::operator[](int ind) const
{
  return (ind>=0 && ind<kDimDaqStat) ? fStats[ind] : 0.;
}

//----------------------------------------------------------------------------
//! Assignment operator.

nxyter::QuickDaqStats& nxyter::QuickDaqStats::operator=(const QuickDaqStats& rhs)
{
  if (&rhs != this) fStats = rhs.fStats;
  return *this;
}
//----------------------------------------------------------------------------
//! Subtraction operator.

nxyter::QuickDaqStats& nxyter::QuickDaqStats::operator-(const QuickDaqStats& rhs)
{
  for (int i=0; i<kDimDaqStat; i++) fStats[i] -= rhs.fStats[i];
  return *this;
}

//----------------------------------------------------------------------------
//! Multiplication operator.

nxyter::QuickDaqStats& nxyter::QuickDaqStats::operator*(double factor)
{
  for (int i=0; i<kDimDaqStat; i++) fStats[i] *= factor;
  return *this;
}

//----------------------------------------------------------------------------
//! Print contents of counters to stream \a os.
/*!
 * Prints the statistic counters to \a os. If \a dtime is greater than zero,
 * also the count rates normalized to the run time \a dtime (in sec) are
 * given in Hz or kHz. The argument \a lvl determines whether all, only
 * non-zero, or only non-zero error counters are printed.
 * 
 * \param os output stream
 * \param dtime delta time (in sec), if larger zero also rates are shown
 * \param lvl print level, choices are:
 *   \li kPrintAll all counters are shown
 *   \li kPrintNonZero only non-zero counters are shown, the first, 
 *         the "messages received", is shown even when zero.
 *   \li kPrintError only non-zero error counters are shown
 * 
 * A listing with \a lvl = kPrintAll will look like
\verbatim
Stat( 0) messages received       :             4708
Stat( 1) nop messages            :                0
Stat( 2) nXYTER hits received    :             4640
Stat( 3) hits with pileup        :                0
Stat( 4) hits with overflow      :                0
Stat( 5) hits on nx:0            :             4640
Stat( 9) epoch markers           :               57
Stat(10) epochs with missed hits :                0
Stat(11) missed hits (low bound) :                0
Stat(12) sync0 messages          :                8
Stat(13) sync1 messages          :                0
Stat(14) aux0 messages           :                0
Stat(15) aux1 messages           :                0
Stat(16) aux2 messages           :                0
Stat(17) aux3 messages           :                0
Stat(18) aux with pileup         :                0
Stat(19) nx:0 parity errors      :                1 <== ERRORS!!
Stat(20) nx:1 parity errors      :                0
Stat(21) nx:2 parity errors      :                0
Stat(22) nx:3 parity errors      :                0
Stat(23) sync0 parity errors     :                2 <== ERRORS!!
Stat(24) sync1 parity errors     :                0
Stat(25) daq resume messages     :                0
Stat(26) fifo reset messages     :                1
Stat(27) other system messages   :                3
Stat(28) other message types     :                0
\endverbatim
 */

void nxyter::QuickDaqStats::print(std::ostream& os, double dtime, int lvl) const
{
  static format fmt_stat("Stat(%|2|) %|-24|: %16.0f");
  static format fmt_rate("%8.2f");
  for (int i=0; i<kDimDaqStat; i++){
    bool errorcounter = (i>=kNSysMesNxParErr0 && i<=kNSysMesSyncParErr1);
    bool show;
    switch (lvl) {
      case kPrintAll:      
        show = true; 
        break;
      case kPrintNonZero:  
        show = (i==0 || fStats[i] != 0); 
        break;
      case kPrintError:    
        show = errorcounter && fStats[i] != 0;
        break;
    }
    
    if (show) {
      os << fmt_stat % i % counterName(i) % fStats[i];
      if (dtime>0.) {
        double rate = fStats[i] / dtime;
        const char* unit = " Hz";
        if (rate >= 1000.) {
          rate = rate / 1000.;
          unit = "kHz";
        }
        os << "  " << fmt_rate % rate;
        os << " " << unit;
      }
      if (errorcounter && fStats[i] != 0) {
        os << " <== ERRORS!!";
      }
      os << std::endl;
    }
  }
}

//----------------------------------------------------------------------------
//! Returns name for counter with index \a ind (or "" if undefined).

const char* nxyter::QuickDaqStats::counterName(int ind)
{
  switch(ind) {
    case kNMessage:          return "messages received";
    case kNNop:              return "nop messages";
    case kNHit:              return "nXYTER hits received";
    case kNHitPileup:        return "hits with pileup";
    case kNHitOverflow:      return "hits with overflow";
    case kNHitNx0:           return "hits hits on nx:0";
    case kNHitNx1:           return "hits hits on nx:1";
    case kNHitNx2:           return "hits hits on nx:2";
    case kNHitNx3:           return "hits hits on nx:3";
    case kNEpoch:            return "epoch markers";
    case kNEpochWithMiss:    return "epochs with missed hits";
    case kNMissedHit:        return "missed hits (low bound)";
    case kNSync0:            return "sync0 messages";
    case kNSync1:            return "sync1 messages";
    case kNAux0:             return "aux0 messages";
    case kNaux1:             return "aux1 messages";
    case kNAux2:             return "aux2 messages";
    case kNAux3:             return "aux3 messages";
    case kNAuxOverflow:      return "aux with pileup";
    case kNSysMesNxParErr0:  return "nx:0 parity errors";
    case kNSysMesNxParErr1:  return "nx:1 parity errors";
    case kNSysMesNxParErr2:  return "nx:2 parity errors";
    case kNSysMesNxParErr3:  return "nx:3 parity errors";
    case kNSysMesSyncParErr0:return "sync0 parity errors";
    case kNSysMesSyncParErr1:return "sync1 parity errors";
    case kNSysMesDaqRes:     return "daq resume messages";
    case kNSysMesFifoReset:  return "fifo reset messages";
    case kNSysMesOther:      return "other system messages";
    case kNEpoch2:           return "epoch2 markers";
    case kNGet4Hit:          return "Get4 hits";
    case kNGet4Event:        return "Get4 events";
    case kNOther:            return "other message types";
  }
  return "";
}


