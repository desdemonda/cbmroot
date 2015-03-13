#pragma once

#include <fstream>
#include <memory>

#include "Timeslice.hpp"
#include "TrbBridgeTransaction.hpp"
#include "TrbBridgeStatistics.hpp"

#include <chrono>

/**
 * Functions as "glue logic" between the flesnet-ipc, the user and the TrbBridge code
 */
class TrbBridge {
public:
    TrbBridge();
   ~TrbBridge();
   
   void openFile(const std::string& filename);
   void closeFile();
   
   void setFlags(uint32_t flags);
   uint32_t getFlags() const;
   
   void importTimeslice(const fles::Timeslice& ts);

   unsigned int runNumber() const;
   void setRunNumber(unsigned int num);
   
   // Creation Time
   void setArchiveCreationTime(std::time_t &);
   std::time_t archiveCreationTime() const;
   
   void setOfflineProcessing(bool v);
   bool offlineProcessing() const;
   
   void setMCSize(unsigned int mcsize);
   unsigned int MCSize() const;
   
   constexpr static int flesnetSysId = 0x90;
   constexpr static uint32_t SSEStructCheck = 0x1;
   constexpr static double flibMCSizeGranularity = 8e-9; // seconds per MCSize count
   
   const TrbBridgeStatistics & getCurrentStatistics() const;
   
private:
   void _importFrame(const TrbBridgeFrame & frame, unsigned int retry = 1);
   
   std::vector<uint32_t> _eventBuffer;
   uint16_t* _transactionBuffer;
   
   std::unique_ptr<std::ofstream> _filePtr;
   TrbBridgeTransaction _transaction;
   
   unsigned int _eventNum;
   unsigned int _runNum;
   unsigned int _time;
   unsigned int _date;
   
   unsigned int _flags;
   
   TrbBridgeStatistics _currentStats;
   TrbBridgeStatistics _lastStats;
   
   // creation time required only for offline processing
   bool _offlineProcessing;
   unsigned int _MCSize;
   
   unsigned int _creationTimeMCCount;
   std::time_t _archiveCreationTime;
   std::time_t _getEventTime(unsigned int delta = 0);
   
   std::time_t _lastStatDump;
};