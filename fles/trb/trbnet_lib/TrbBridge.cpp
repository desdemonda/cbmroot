#include "TrbBridge.hpp"
#include "MicrosliceDescriptor.hpp"
#include <iostream>
#include <sstream>
#include <boost/format.hpp>
#include <chrono>
#include <ctime>
#include <stdlib.h>
#include "TrbBridgeTrbNetHeaders.hpp"

/* Construction & Destruction */
   TrbBridge::TrbBridge() 
     : _eventBuffer(TrbBridgeTransaction::MAX_SIZE / sizeof(_eventBuffer[0]) + 8)
     , _transactionBuffer(reinterpret_cast<uint16_t*>(_eventBuffer.data()) + 16)
     , _filePtr()
     , _transaction(_transactionBuffer)
     ,  _eventNum(0)
     ,  _runNum(0)
     , _time(0)
     , _date(0)
     , _flags(0)
     , _currentStats()
     , _lastStats()
     , _offlineProcessing(false)
     , _MCSize(0)
     , _creationTimeMCCount(0)
     , _archiveCreationTime()
     , _lastStatDump(0)

   {
     //      _eventNum = 0;
     //      _flags = 0;

      srand(std::time(NULL));
      _runNum = rand();
      
      //     _creationTimeMCCount = 0;
      //      _offlineProcessing = false;
   }

   TrbBridge::~TrbBridge() {
      closeFile();
   }

/* File handling */
   void TrbBridge::openFile(const std::string& filename) {
      if (_filePtr) closeFile();
      _filePtr.reset( new std::ofstream(filename, std::ios::out | std::ios::trunc | std::ios::binary) );
      if (_filePtr) {
         TrbBridgeTrbNetEventHeader evtHdr;
         evtHdr.swappedEndianess = false;
         evtHdr.timestamp = _getEventTime();
         evtHdr.runNumber = _runNum;
         evtHdr.eventNumberPerFile = 0;
         evtHdr.setSizeWithPadding(32);
         
         _eventBuffer.data() << evtHdr;
         _eventBuffer[2] = 0x00010002; // first event in hld file has special id
         
         _filePtr->write(reinterpret_cast<char*> (_eventBuffer.data()),  size_t(evtHdr.size) );
      }
   }

   void TrbBridge::closeFile() {
      if (!_filePtr) return;
      
      _filePtr->close();
      _filePtr.release();
   }

/* Wrapper */
   void TrbBridge::setFlags(uint32_t f) {
      _flags = f;
   }
   
   uint32_t TrbBridge::getFlags() const {
      return _flags;
   }

/* Stream frames in */
   void TrbBridge::_importFrame(const TrbBridgeFrame & frame, std::list<std::vector<uint32_t>>* resultList, unsigned int retry) {
      if (!retry) return;
      
      try {
         _transaction << frame;
         
      } catch(std::exception& e) {
         if (_flags & ShowExceptions)
            std::cerr << e.what() << std::endl;
         
         // all frames that went into this transaction are lost
         _currentStats.incrementFramesDiscarded(_transaction.nextFrameNumberExpected());
         
         _transaction.reset();
         
         if (frame.isStartFrame()) {
            _importFrame(frame, resultList, retry-1);
         }
         
         return;
      }
      
      if (_transaction.isStarted() && frame.isStartFrame()) {
         _currentStats.incrementTransactionsStarted();
         
         if (_lastTransactionNumber > 0) {
            int missingTrans = _transaction.transactionNumber() - ((_lastTransactionNumber + 1) & 0xfff);
            if (missingTrans < 0) missingTrans += 0x1000;
            _currentStats.incrementTransactionsMissing(missingTrans);
         }
         
         _lastTransactionNumber = _transaction.transactionNumber();
      }
      
      if (!_transaction.isStarted()) {
         _currentStats.incrementFramesSkipped();
         return;
      }
      
      if (_transaction.isCompleted()) {
      // add event header (very simple event building !)
         // decode sub-event header
         TrbBridgeTrbNetSubEventHeader subEvtHdr;
         subEvtHdr << reinterpret_cast<uint32_t*>(_transactionBuffer);

         // build event header based on information gathered
         TrbBridgeTrbNetEventHeader evtHdr;
         evtHdr.swappedEndianess = subEvtHdr.swappedEndianess;
         evtHdr.triggerType = 0x00002001; // TODO: trigger type missing;
         evtHdr.eventNumberPerFile = ++_eventNum;
         evtHdr.timestamp = _getEventTime();
         evtHdr.runNumber = _runNum;

         // set size and clear padding area
         evtHdr.setSizeWithPadding(subEvtHdr.size + 32);
         for(unsigned int i = (subEvtHdr.size+32) / sizeof(_eventBuffer[0]); i < evtHdr.size / sizeof(_eventBuffer[0]); i++)
            _eventBuffer[i] = 0;
         
         _eventBuffer.data() << evtHdr;
         /*         
         std::cout << boost::format("Completed transaction 0x%04x with % 4d bytes. Stored trigger number 0x%04x") 
                        % _transaction.transactionNumber() 
                        % _transaction.size()
                        % subEvtHdr.triggerNumber
                     << std::endl;
         */

      // write to hld file
         if (_filePtr)
            _filePtr->write(reinterpret_cast<char*>(_eventBuffer.data()), evtHdr.size);

         if (resultList) {
            std::vector<uint32_t> eventCopy(_eventBuffer.begin(), _eventBuffer.begin() + evtHdr.size / sizeof(uint32_t));
            resultList->push_back(eventCopy);
         }
         _currentStats.addEventSize(evtHdr.size);
         _currentStats.incrementFramesAssigned(_transaction.nextFrameNumberExpected());
         _currentStats.incrementTransactionsCompleted();
         
        // std::cout << _transaction.transactionNumber() << std::endl;
         
         if (getFlags() & SSEStructCheck) {
            try {
               std::list<TrbBridgeTrbNetSubSubEventHeader> sse = TrbBridgeTrbNetSubSubEventHeader::extractSubSubEvents(
                  subEvtHdr, reinterpret_cast<uint32_t*> (_transactionBuffer));
               
               if (sse.size() != 5) {
                  std::cout << "Found " << sse.size() << " sub-events: ";
                  for(auto it = sse.begin(); it != sse.end(); it++)
                     std::cout << boost::format("(id: 0x%04x, size: %d) ") % it->subSubEventId % it->size;
                  std::cout << std::endl;
               }
               
               if (_lastEvtNumber > 0) {
                  if (((_lastEvtNumber+1)&0xffff) != subEvtHdr.triggerNumber) {
                     //std::cout << "Event sequence error. Expected: " << ((_lastEvtNumber+1)&0xffff) << " Got: " << subEvtHdr.triggerNumber << std::endl;
                     _currentStats.incrementTriggerNumberSequenceErrors();
                     
                     int delta = subEvtHdr.triggerNumber - ((_lastEvtNumber+1)&0xffff);
                     if (delta < 0) {
                        std::cout << delta;
                        delta += 0x10000;
                        std::cout << " " << delta << "last :" << _lastEvtNumber << " now: " << subEvtHdr.triggerNumber << std::endl;
                     }
                     _currentStats.incrementMissedTriggerNumbers(delta);
                  }
               }
               
               _lastEvtNumber = subEvtHdr.triggerNumber;
               
   // do whatever you want with your data
               
            } catch(std::exception& e) {
               std::cerr << e.what() << std::endl;
               _currentStats.incrementSSEStructureError();
            }
            
         }
         
         _transaction.reset();
      }
   }

   void TrbBridge::importTimeslice(const fles::Timeslice& ts, unsigned int skipMS) {
      extractEventsFromTimeslice(ts, skipMS);
   }
      
   std::list<std::vector<uint32_t>> TrbBridge::extractEventsFromTimeslice(const fles::Timeslice & ts, unsigned int skipMS) {
      std::list<std::vector<uint32_t>> eventsReconstructed;

      for (size_t c = 0; c < ts.num_components(); ++c) {
         if (ts.num_microslices(c) == 0) {
               std::cerr << "no microslices in TS " << ts.index() << ", component "
                        << c << std::endl;
               return eventsReconstructed;
         }
         
         for (size_t m = 0; m < ts.num_microslices(c); ++m) {
            if (m < skipMS)
               continue;
            
            // iterate over microslice
            auto msDescriptor = ts.descriptor(c, m);
            auto msContent = reinterpret_cast<const uint16_t*>(ts.content(c, m));
            
            // only consider generic TrbNet subsystem
            if (msDescriptor.sys_id != flesnetSysId)
               continue;

            unsigned int msInteralPtr = 16 / sizeof(*msContent);
            while(msInteralPtr < msDescriptor.size / sizeof(*msContent)) {
               // iterate over cbmnet frames with in microslice (padding handling just copied from tsclient)
               uint8_t word_count = (msContent[msInteralPtr] & 0xff) + 1; 
               uint8_t padding_count = (4 - ((word_count + 1) & 0x3)) & 0x3;
               ++msInteralPtr;
               
               // here the magic happens
               try {
                  TrbBridgeFrame frame (&msContent[msInteralPtr], word_count*2);
                  _importFrame( frame, &eventsReconstructed );
               } catch(std::exception& e) {
                  _currentStats.incrementFramesSkipped();
               }

               _currentStats.incrementFramesProcessed();

               msInteralPtr += word_count + padding_count;
            }
         }
      }
      
      _creationTimeMCCount += ts.num_microslices(0);
      _currentStats.incrementTimeslicesProcessed();
      
      std::time_t now = std::time(NULL);
      if (_lastStatDump != now) {
         std::cout << std::endl;
         std::cout << _currentStats.dump(15);
         std::cout << _currentStats.dump(_lastStats);
         
         _lastStats = _currentStats;
         _lastStatDump = now;
      }

      return eventsReconstructed;

      //std::cout << "Pending Trans:" << _transaction.transactionNumber() << " " << _currentStats.dump();
   }

   std::time_t TrbBridge::_getEventTime(unsigned int delta) {
      if (_offlineProcessing) {
         double secondsPerMC = flibMCSizeGranularity * _MCSize;
         unsigned int offsetSeconds = int( (_creationTimeMCCount + delta) * secondsPerMC );
         return _archiveCreationTime + offsetSeconds; // TODO: This is not C++ standard conform, but should work on all POSIX-compatible systems
      } 
      
      return  std::time(NULL); 
   }
   
   
// Wrapper
   unsigned int TrbBridge::runNumber() const {
      return _runNum;
   }
   
   void TrbBridge::setRunNumber(unsigned int num) {
      _runNum = num;
   }

   void TrbBridge::setOfflineProcessing(bool v) { 
      _offlineProcessing = v;
   }
      
   bool TrbBridge::offlineProcessing() const {
      return _offlineProcessing;
   }
   
   void TrbBridge::setArchiveCreationTime(std::time_t & ts) {
      _archiveCreationTime = ts;
   }
   
   std::time_t TrbBridge::archiveCreationTime() const {
      return _archiveCreationTime;
   }
   
   void TrbBridge::setMCSize(unsigned int mcsize) {
      _MCSize = mcsize;
   }
   
   unsigned int TrbBridge::MCSize() const {
      return _MCSize;
   }

   const TrbBridgeStatistics & TrbBridge::getCurrentStatistics() const {
      return _currentStats;
   }
