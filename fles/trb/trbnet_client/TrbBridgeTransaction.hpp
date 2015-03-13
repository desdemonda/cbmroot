#pragma once

#include <exception>
#include <string>
#include "TrbBridgeFrame.hpp"

/**
 * The class implements the merging of several frames into a complete transaction.
 */
class TrbBridgeTransaction {
public:
   /**
    * Generates an empty transaction w/o buffer.
    * You cannot use this transaction !
    */
   TrbBridgeTransaction() {
      TrbBridgeTransaction(0);
   }

   /**
    * Generates an empty transaction
    * 
    * @param buffer Pointer to a sufficiently large buffer area (>= MAX_SIZE)
    */
   TrbBridgeTransaction(uint16_t* buffer);

   /**
    * Generate human-readable representation of the transaction
    */
   std::string dump() const;
   
   /**
    * Empties a transaction to start over (same state as new creation)
    */
   void reset();
   
   /**
    * Append a frame to the transaction. Function automatically handles start/stop frames.
    * Once a transaction has started, it only accepts frames with the same transaction no.
    *
    * @param frame Frame to append
    */
   TrbBridgeTransaction operator<< (const TrbBridgeFrame & frame);
   
   /**
    * Indicates whether a start frame was found yet
    * 
    * @return true, if start frame was detected
    */
   bool isStarted() const;

   /**
    * Indicates whether a valid stop frame was found yet.
    * This implies, that the transaction seemes complete and no errors have been found.
    * 
    * @return true, if transaction seems complete
    */
   bool isCompleted() const;
   
   /**
    * @return Pointer provided in constructor
    */
   uint16_t* buffer();

   /**
    * @return Pointer provided in constructor
    */
   const uint16_t* buffer() const;
   
   /**
    * Size of transaction (incl. sub-event header) in bytes
    * 
    * @return if isStarted() length found in sub-event header, 0 otherwise
    */
   uint32_t size() const;
   
   
   /**
    * Transaction Number
    * 
    * @return if isStarted(): transaction number, 0 otherwise
    */
   uint16_t transactionNumber() const;
   
   /**
    * Frame Number
    * 
    * @return if isStarted(): transaction number, 0 otherwise
    */
   uint16_t nextFrameNumberExpected() const;
   
   /**
    * Maximal transaction size in bytes. The class wont accept frame that exceed this boundary,
    * nor will it process a start frame indicating a larger transaction.
    * Take care, that the buffer provided to the constructor can fit this size.
    */
   constexpr static size_t MAX_SIZE = 4096;
   
private:
   uint16_t* _buffer;  /**< Write buffer, as provided in constructor */
   size_t _writeIdx;   /**< Index to first empty word in buffer */
   size_t _size;       /**< Size gathered upon importing the start frame */
   
   bool _completed;  /**< Indicates that all frame belonging to this transaction have been received w/o framing errors */

   uint16_t _transactionNumber;   /**< Transaction Number contained in start frame. Only valid if isStarted() */
   uint16_t _frameNumberExpected; /**< Frame Number of next frame expected */
};

class TrbBridgeTransactionException : public std::exception {
public:
   explicit TrbBridgeTransactionException(const std::string& what_arg = "", const TrbBridgeTransaction* involvedTransaction = NULL, const TrbBridgeFrame* involvedFrame = NULL);
   virtual const char* what() const throw() {
      return _what.c_str();
   }
    
private:
   std::string _what;
};
