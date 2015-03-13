#include "TrbBridgeStatistics.hpp"
#include <sstream>
#include <boost/format.hpp>

std::string fmtPercent(double v) {
   std::string val("n/a");
   if (v == v) {
      // not nan
      val = (boost::format("%.2f") % v).str();
   }
   return (boost::format("% 7s") % val).str();
}

std::string fmtSize(double v) {
   std::string val;
   if (v > 900) {
      return (boost::format("%.1fk") % (v / 1e3)).str();
   } else {
      return (boost::format("%.1f") %  (v / 1e0)).str();
   }
}
    
   
   
// Init
TrbBridgeStatistics::TrbBridgeStatistics() 
  : _timeslicesProcessed(0)
  , _framesProcessed(0)
  , _framesAssigned(0)
  , _framesSkipped(0)
  , _framesDiscarded(0)
  , _transactionsStarted(0)
  , _transactionsCompleted(0)
  , _transactionsMissing(0)
  , _SSEStructureError(0)
  , _missedTriggerNumbers(0)
  , _triggerNumberSequenceErrors(0)
  , _dumpsSinceLastHeader(0)
  , _eventSizeTotal(0)
  , _eventSizeNumber(0)
  , _eventSizeMin(0xffffff)
  , _eventSizeMax(0)     
{
      reset();
}

   void TrbBridgeStatistics::reset() {
      _framesProcessed = 0;
      _framesAssigned = 0;
      _framesSkipped = 0;
      _framesDiscarded = 0;
      _transactionsStarted = 0;
      _transactionsCompleted = 0;
      _transactionsMissing = 0;
      _SSEStructureError = 0;
      _timeslicesProcessed = 0;
      _triggerNumberSequenceErrors = 0;
      _missedTriggerNumbers = 0;
      
      _dumpsSinceLastHeader=0;
      
      _eventSizeTotal = 0;
      _eventSizeNumber = 0;
      _eventSizeMin = 0xffffff;
      _eventSizeMax = 0;
   }

// Output statistics   
   std::string TrbBridgeStatistics::dumpHeader() const {
      std::stringstream ss;
      ss <<
         (boost::format("% 8s |% 78s| % 40s |% 26s |% 23s") 
            % "Timeslc" 
            % "Frames                               " 
            % "Transactions           " 
            % "TrbNet-Trigger-Nr.   " 
            % "Event Size      "
         )
         << std::endl;
      
      ss <<
         (boost::format("% 8s |% 12s % 20s % 20s % 22s |% 10s % 19s % 10s |% 6s % 18s |% 7s % 7s % 7s")
            % "Count"
            
            % "Processed"
            % "Skipped"
            % "Discarded (%)"
            % "Matched (%)"
            
            % "Started"
            % "Aborted"
            % "Missing"
            
            % "Errors"
            % "Missing"
            
            % "Min"
            % "Avg"
            % "Max"
         ) << std::endl;
      return ss.str();
   }

   std::string TrbBridgeStatistics::dump(const TrbBridgeStatistics& ref, unsigned int includeHeaderInterval) {
      long transStarted= (_transactionsStarted - ref._transactionsStarted);
      long transAborted = transStarted - (_transactionsCompleted - ref._transactionsCompleted);
      
      std::stringstream ss;
      return ss.str();
      if (includeHeaderInterval) {
         if (!_dumpsSinceLastHeader) 
            ss << dumpHeader();

         _dumpsSinceLastHeader++;
         if (includeHeaderInterval <= _dumpsSinceLastHeader) {
            _dumpsSinceLastHeader = 0;
         }
      }
      
      std::string evtSzeMin("n/a"), evtSzeMax("n/a"), evtSzeAvg("n/a");
      
      if (!ref._eventSizeNumber) {
         evtSzeMin = fmtSize( _eventSizeMin );
         evtSzeMax = fmtSize( _eventSizeMax );
      }
      
      if (_eventSizeNumber - ref._eventSizeNumber) {
         evtSzeAvg = fmtSize( double(_eventSizeTotal - ref._eventSizeTotal) / 
                              double(_eventSizeNumber - ref._eventSizeNumber) );
      }
      
      ss << (
         boost::format("% 8d |% 12d % 10d (%s) % 10d (%s) % 12d (%s) |% 10d % 9d (%s) % 10d |% 7d % 9d (%s)|% 7s % 7s % 7s")
         // Timeslice   
            % (_timeslicesProcessed - ref._timeslicesProcessed)
            
         // Frames
            % (_framesProcessed - ref._framesProcessed) 
            % (_framesSkipped - ref._framesSkipped) 
            % fmtPercent(100.0*double(_framesSkipped - ref._framesSkipped) / (_framesProcessed - ref._framesProcessed))
            
            % (_framesDiscarded - ref._framesDiscarded) 
            % fmtPercent(100.0*double(_framesDiscarded - ref._framesDiscarded) / (_framesProcessed - ref._framesProcessed))
            
            % (_framesAssigned - ref._framesAssigned) 
            % fmtPercent(100.0*double(_framesAssigned - ref._framesAssigned) / (_framesProcessed - ref._framesProcessed))
            
         // Transactions
            % transStarted
            % transAborted
            % fmtPercent(100.*transAborted / transStarted)
            % (_transactionsMissing - ref._transactionsMissing)
               
         // Trg-Seq
            % (_triggerNumberSequenceErrors - ref._triggerNumberSequenceErrors) 
            % (_missedTriggerNumbers - ref._missedTriggerNumbers) 
            % fmtPercent(1.0e6 * (_missedTriggerNumbers - ref._missedTriggerNumbers) / transStarted)
            
         // Size
            % evtSzeMin
            % evtSzeAvg
            % evtSzeMax
      ) << std::endl;
      return ss.str();
   }

   std::string TrbBridgeStatistics::dump(unsigned int includeHeaderInterval) {
      TrbBridgeStatistics nil;
      return dump(nil, includeHeaderInterval);
   }

// Boring Wrappers
   void TrbBridgeStatistics::incrementTimeslicesProcessed(unsigned int long delta) {
      _timeslicesProcessed += delta;
   }
   
   void TrbBridgeStatistics::incrementTransactionsStarted(unsigned int long delta) {
      _transactionsStarted += delta;
   }   

   void TrbBridgeStatistics::incrementTransactionsCompleted(unsigned int long delta) {
      _transactionsCompleted += delta;
   }
   
   void TrbBridgeStatistics::incrementTransactionsMissing(unsigned int long delta) {
      _transactionsMissing += delta;
   }

   void TrbBridgeStatistics::incrementFramesProcessed(unsigned int long delta) {
      _framesProcessed += delta;
   }

   void TrbBridgeStatistics::incrementFramesSkipped(unsigned int long delta) {
      _framesSkipped += delta;
   }

   void TrbBridgeStatistics::incrementFramesDiscarded(unsigned int long delta) {
      _framesDiscarded += delta;
   }

   void TrbBridgeStatistics::incrementFramesAssigned(unsigned int long delta) {
      _framesAssigned += delta;
   }

   void TrbBridgeStatistics::incrementSSEStructureError(unsigned int long delta) {
      _SSEStructureError += delta;
   }
   
   void TrbBridgeStatistics::incrementMissedTriggerNumbers(unsigned int long delta) {
      _missedTriggerNumbers += delta;
   }

   void TrbBridgeStatistics::incrementTriggerNumberSequenceErrors(unsigned int long delta) {
      _triggerNumberSequenceErrors += delta;
   }

   void TrbBridgeStatistics::addEventSize(unsigned int long size) {
      _eventSizeTotal += size;
      _eventSizeNumber++;
      if (_eventSizeMin > size) _eventSizeMin = size;
      if (_eventSizeMax < size) _eventSizeMax = size;
   }
