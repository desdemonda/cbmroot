#include "TrbBridgeTransaction.hpp"
#include <algorithm>
#include <iostream>
#include <assert.h>

#include <sstream> 
#include <boost/format.hpp>

#include "TrbBridgeTrbNetHeaders.hpp"

TrbBridgeTransaction::TrbBridgeTransaction(uint16_t* tmpbuffer) 
  : _buffer(tmpbuffer)
  , _writeIdx(0)
  , _size(0)
  , _completed(false)
  , _transactionNumber(0)
  , _frameNumberExpected(0)    
{
  //   _buffer = buffer;
   reset();
}

void TrbBridgeTransaction::reset() {
   _size = 0;
   _writeIdx = 0;
   _frameNumberExpected = 0;
   _completed = false;
}

TrbBridgeTransaction TrbBridgeTransaction::operator<< (const TrbBridgeFrame & frame) {
   if (!_buffer)
      throw TrbBridgeTransactionException("Cannot append frame to buffer-less transaction", this, &frame);
   
// deal with start frames
   if (isCompleted())
      throw TrbBridgeTransactionException("Cannot append frame to completed transaction", this, &frame);
   
   if (!isStarted() && !frame.isStartFrame()) {
      // we're waiting for a start frame, so let's skip this frame
      // because this can happend when synchronising to a stream, this is not considered an error
      
      return *this;
   }

   if (isStarted() && frame.isStartFrame())
      throw TrbBridgeTransactionException("Cannot process second start frame", this, &frame);
   
   
// check transaction & frame number (if necessary)
   size_t wordsFromFrame = frame.payloadSize() / 2;
   if (!isStarted()) {
      _transactionNumber = frame.transactionNumber();
      _size=(frame.payload()[0] << 16) | frame.payload()[1];   // little quirk to convert the median endian input into little endian
      
   } else {
      if (!frame.matchesTransactionNumber(_transactionNumber))
         throw TrbBridgeTransactionException("Tried to append frame with wrong transaction number", this, &frame);
      
      if (!frame.matchesFrameNumber(_frameNumberExpected))
         throw TrbBridgeTransactionException("Tried to append frame with wrong frame number", this, &frame);
      
   
   // check memory boundaries
      const size_t wordsLeftsInTransaction = size() / 2 - _writeIdx;
      
      if (wordsFromFrame > wordsLeftsInTransaction) {
         if (frame.isStopFrame() && wordsFromFrame - wordsLeftsInTransaction <= 2 && wordsFromFrame == 3) {
            // the excess seems to be padding .. let's have a closer look
            
            for (unsigned int i=wordsLeftsInTransaction; i < wordsFromFrame; i++)
               if (frame.payload()[i] != 0xaaaa)
                  throw TrbBridgeTransactionException("Unexpected padding", this, &frame);
            
            wordsFromFrame = wordsLeftsInTransaction;
         
         } else {
            throw TrbBridgeTransactionException("Frame too long for transaction", this, &frame);
         
         }
      }
   }   
// copy data
   for(unsigned int i=0; i < wordsFromFrame; i++)
      // THIS STEP IS CRUCIAL: TrbNet sends MSB big-endian words, while FLIB serves
      // little-endian half-words; so have to swap the half-words in each word and
      // obtain a little-endian word
      _buffer[ _writeIdx++ ^ 1 ] = frame.payload()[i];   
      
   
   _frameNumberExpected++;
      
// check for valid header length
   if (frame.isStartFrame()) {
      if (size() > MAX_SIZE || size() & 0x3) {
         //std::cout << size() << std::endl;
         throw TrbBridgeTransactionException("Start frame indicates an unsupported transaction length", 0, &frame);
      }
   }
   
// close transaction ?
   if (_writeIdx == size() / 2) {
      if (!frame.isStopFrame()) 
         throw TrbBridgeTransactionException("Transaction completed without stop frame", this, &frame);
      
      _completed = true;
      
   } else if (frame.isStopFrame()) {
      throw TrbBridgeTransactionException("Premature stop frame", this, &frame);
      
   }
   
   return *this;
}

bool TrbBridgeTransaction::isStarted() const {
   return _writeIdx != 0;
}

bool TrbBridgeTransaction::isCompleted() const {
   return _completed;
}

uint16_t* TrbBridgeTransaction::buffer() {
   return _buffer;
}

const uint16_t* TrbBridgeTransaction::buffer() const {
   return const_cast<const uint16_t*> (_buffer);
}

uint32_t TrbBridgeTransaction::size() const {
   if (!_buffer)
      return 0;
   
   if (!isStarted())
      throw TrbBridgeTransactionException("TrbBridgeTransaction::size() is available only, once a start frame was processed");
   
   return _size;
}

uint16_t TrbBridgeTransaction::transactionNumber() const {
   if (!isStarted())
      throw TrbBridgeTransactionException("TrbBridgeTransaction::transactionNumber() is available only, once a start frame was processed");
   
   return _transactionNumber;
}

uint16_t TrbBridgeTransaction::nextFrameNumberExpected() const {
   return _frameNumberExpected;
}
   
std::string TrbBridgeTransaction::dump() const {
   if (!isStarted())
      return "Transaction: Not started\n";
   
   std::stringstream ss;
   
   ss << boost::format("Transaction: %s (TransNo %d,  size: % 4d)") 
            % (isCompleted() ? "completed" : "pending") 
            % transactionNumber()
            % size()
       << std::endl;
       
   for(unsigned int i=0; i< size()/2; i++) {
      if (i % 8 == 0 && i)
         ss << std::endl;
      
      if (i < _writeIdx)
         ss << boost::format(" [% 4d] 0x%04x") % i % _buffer[i];
      else
         ss << boost::format(" [% 4d] .recv.") % i;
   }
      
   return ss.str();
}

TrbBridgeTransactionException::TrbBridgeTransactionException(const std::string& what_arg, const TrbBridgeTransaction* involvedTransaction, const TrbBridgeFrame* involvedFrame) 
  : _what("")
{
   if (involvedTransaction || involvedFrame) {
      std::stringstream ss;
      ss << what_arg << std::endl;
      if (involvedTransaction)
         ss << "Involves " << involvedTransaction->dump() << std::endl;
      
      if (involvedFrame)
         ss << "Involves " << involvedFrame->dump();
      
      _what = ss.str();
   } else {
      _what = what_arg;
   }
}
