#include "TrbBridgeStatistics.hpp"
#include <sstream>
#include <boost/format.hpp>

// Init
   TrbBridgeStatistics::TrbBridgeStatistics() {
      reset();
   }

   void TrbBridgeStatistics::reset() {
      _framesProcessed = 0;
      _framesAssigned = 0;
      _framesSkipped = 0;
      _framesDiscarded = 0;
      _transactionsStarted = 0;
      _transactionsCompleted = 0;
      _SSEStructureError = 0;
      _timeslicesProcessed = 0;
   }

// Output statistics   
   std::string TrbBridgeStatistics::dump(const TrbBridgeStatistics& ref) const {
      std::stringstream ss;
      ss << (
         boost::format("TS processed: % 6d, Frames process: % 6d, skipped: % 6d, assigned: % 6d, discarded: % 6d  Transactions started: % 4d, completed % 4d, SSE-Errors: % 4d")
            % (_timeslicesProcessed - ref._timeslicesProcessed) 
            % (_framesProcessed - ref._framesProcessed) 
            % (_framesSkipped - ref._framesSkipped) 
            % (_framesAssigned - ref._framesAssigned) 
            % (_framesDiscarded - ref._framesDiscarded) 
            % (_transactionsStarted - ref._transactionsStarted)
            % (_transactionsCompleted - ref._transactionsCompleted)
            % (_SSEStructureError - ref._SSEStructureError)
      ) << std::endl;
      return ss.str();
   }

   std::string TrbBridgeStatistics::dump() const {
      TrbBridgeStatistics nil;
      return dump(nil);
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