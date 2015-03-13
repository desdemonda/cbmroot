#pragma once
#include <string>

class TrbBridgeStatistics {
public:
   TrbBridgeStatistics();
   
   std::string dump(const TrbBridgeStatistics& ref) const;
   std::string dump() const;
   
   void reset();
   
   void incrementTimeslicesProcessed(unsigned int long delta=1);
   
   void incrementFramesProcessed(unsigned int long delta=1);
   void incrementFramesSkipped(unsigned int long delta=1);
   void incrementFramesDiscarded(unsigned int long delta=1);
   void incrementFramesAssigned(unsigned int long delta=1);
   
   void incrementTransactionsStarted(unsigned int long delta=1);   
   void incrementTransactionsCompleted(unsigned int long delta=1);
   void incrementTransactionsInvalid(unsigned int long delta=1);
   
   void incrementSSEStructureError(unsigned int long delta=1);
   
   
private:
   unsigned long _timeslicesProcessed;
   
   unsigned long _framesProcessed;
   unsigned long _framesAssigned;
   unsigned long _framesSkipped;
   unsigned long _framesDiscarded;
   
   unsigned long _transactionsStarted;
   unsigned long _transactionsCompleted;
   
   unsigned long _SSEStructureError;
};
