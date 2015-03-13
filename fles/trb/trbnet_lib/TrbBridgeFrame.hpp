#pragma once

#include <exception>
#include <string>

/**
 * Read-only wrapper for a CBMNet frame extracted from a microslice interpreting the TrbNet bridge's header
 */
class TrbBridgeFrame {
public:
   TrbBridgeFrame(const uint16_t* msContent, size_t frameLength);
   
   /**
    * Generate human-readable representation of frame
    */
   std::string dump() const;
   
   /**
    * Pointer to payload area of the frame (i.e. excl. header)
    */
   const uint16_t* payload() const;
   
   /**
    * Payload size in bytes
    */
   size_t payloadSize() const;
   
   /**
    * Frame number extracted from header. 
    * @return 0 if start frame or undefined
    */
   uint16_t frameNumber() const;
   
   /**
    * Transaction number extracted from header.
    * Number is truncated for inner frames
    */
   uint16_t transactionNumber() const;
   
   /**
    * @return true, if frame header indicates a start frame
    */
   bool isStartFrame() const;

   /**
    * @return true, if frame header indicates a stop frame
    */
   bool isStopFrame() const;
   
   /**
    * Matches the frame number incl. in header against a frame number provided
    * modulo the number of bits in the header's field. Hence, in case of stop
    * frame, the function always returns true.
    * @return true if frameNo & header mask matches frame 
    */
   bool matchesFrameNumber(uint16_t frameNo) const;
   
   /**
    * Matches the transaction number incl. in header against a number provided
    * modulo the number of bits in the header's field. Thus, the significance of
    * a start and stop frame is higher compared to inner frames.
    * @return true if transactionNo & mask matches frames
    */
   bool matchesTransactionNumber(uint16_t transactionNo) const;
   
private:
   const uint16_t* _content; /**< Pointer first data word of frame (i.e. the header) */
   size_t _length;     /**< Length of frame as provided in constructor. IN BYTES */
};

class TrbBrideFrameException : public std::exception {
public:
   explicit TrbBrideFrameException(const std::string& what_arg = "", const TrbBridgeFrame* frameInvolved = NULL);
   virtual const char* what() const throw() {
      return _what.c_str();
   }
    
private:
   std::string _what;
};
  