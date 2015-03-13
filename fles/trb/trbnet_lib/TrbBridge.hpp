#pragma once

#include <fstream>
#include <memory>

#include "Timeslice.hpp"
#include "TrbBridgeTransaction.hpp"
#include "TrbBridgeStatistics.hpp"

#include <chrono>
#include <list>

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
   
   /** Import Timeslice.
    * If avail. write reconstructed events to HLD-File and may perform analysis, if configured by flags
    * DO NOT CALL THIS FUNCTION  (or extractEventsFromTimeslice) MULTIPLE TIMES FOR THE SAME TIMESLICE
    * 
    * @param ts Timeslice to be imported
    * @param skipMS Number of microslices to be skipped at the beginning (overlap ...)
    */
   void importTimeslice(const fles::Timeslice& ts, unsigned int skipMS);
   
   /** Same as importTimeslice but returns list of reconstructed events.
    * If an event is not finished within the timeslice it will be returned as the first event of the 
    * next timeslice. 
    * DO NOT CALL THIS FUNCTION (or importTimeslice) MULTIPLE TIMES FOR THE SAME TIMESLICE
    * 
    * @param ts Timeslice to be imported
    * @param skipMS Number of microslices to be skipped at the beginning (overlap ...)
    */
   std::list<std::vector<uint32_t>> extractEventsFromTimeslice(const fles::Timeslice &, unsigned int skipMS);
   
   unsigned int runNumber() const;
   void setRunNumber(unsigned int num);
   
   // Creation Time
   void setArchiveCreationTime(std::time_t &);
   std::time_t archiveCreationTime() const;
   
   void setOfflineProcessing(bool v);
   bool offlineProcessing() const;
   
   void setMCSize(unsigned int mcsize);
   unsigned int MCSize() const;
   
   constexpr static int flesnetSysId = 0xE0; // Number has to correspond to system ID set in flib.cfg
   // take this number from a header files which uniquely defines all system relevant variables 
//   constexpr static int flesnetSysId = 0x90; // Number was valid for some older test files 
   constexpr static double flibMCSizeGranularity = 8e-9; // seconds per MCSize count

   constexpr static uint32_t SSEStructCheck = 0x1;
   constexpr static uint32_t ShowExceptions = 0x2;

   
   const TrbBridgeStatistics & getCurrentStatistics() const;
   
private:
   void _importFrame(const TrbBridgeFrame & frame, std::list<std::vector<uint32_t>>* resultList, unsigned int retry = 1);
   
   std::vector<uint32_t> _eventBuffer;
   uint16_t* _transactionBuffer;
   
   std::unique_ptr<std::ofstream> _filePtr;
   TrbBridgeTransaction _transaction;
   int _lastTransactionNumber = -1;
   
   
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
   
   int _lastEvtNumber = -1;

  TrbBridge(const TrbBridge&);
  TrbBridge operator=(const TrbBridge&);
};
