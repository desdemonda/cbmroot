#pragma once
#include <string>

class TrbBridgeStatistics {
public:
   TrbBridgeStatistics();
   
   std::string dumpHeader() const;
   std::string dump(const TrbBridgeStatistics& ref,unsigned int includeHeaderInterval=0);
   std::string dump(unsigned int includeHeaderInterval=0);
   
   void reset();
   
   void incrementTimeslicesProcessed(unsigned int long delta=1);
   
   void incrementFramesProcessed(unsigned int long delta=1);
   void incrementFramesSkipped(unsigned int long delta=1);
   void incrementFramesDiscarded(unsigned int long delta=1);
   void incrementFramesAssigned(unsigned int long delta=1);
   
   void incrementTransactionsStarted(unsigned int long delta=1);   
   void incrementTransactionsCompleted(unsigned int long delta=1);
   void incrementTransactionsMissing(unsigned int long delta=1);
   
   void incrementSSEStructureError(unsigned int long delta=1);
   
   void incrementMissedTriggerNumbers(unsigned int long delta=1);
   void incrementTriggerNumberSequenceErrors(unsigned int long delta=1);
   
   void addEventSize(unsigned int long size);
   
private:
   unsigned long _timeslicesProcessed;
   
   unsigned long _framesProcessed;
   unsigned long _framesAssigned;
   unsigned long _framesSkipped;
   unsigned long _framesDiscarded;
   
   unsigned long _transactionsStarted;
   unsigned long _transactionsCompleted;
   unsigned long _transactionsMissing;
   
   unsigned long _SSEStructureError;
   unsigned long _missedTriggerNumbers;
   unsigned long _triggerNumberSequenceErrors;
   
   unsigned int _dumpsSinceLastHeader=0;
   
   unsigned long _eventSizeTotal;
   unsigned long _eventSizeNumber;
   unsigned long _eventSizeMin;
   unsigned long _eventSizeMax;
};
