#include "TrbBridgeFrame.hpp"
#include <boost/format.hpp>
#include <sstream> 

TrbBridgeFrame::TrbBridgeFrame(const uint16_t* msContent, size_t frameLength) 
  : _content(msContent)
  , _length(frameLength)
{
   if (frameLength < 4 || frameLength > 64 || frameLength & 1) throw TrbBrideFrameException("Invalid frameLength");
}

   
const uint16_t* TrbBridgeFrame::payload() const {
   return _content + 1;
}

size_t TrbBridgeFrame::payloadSize() const {
   return _length - 2; // subtract header length
}
   
uint16_t TrbBridgeFrame::frameNumber() const {
   if (isStartFrame() || isStopFrame())
      return 0;
   
   return *_content & 0x7f;
}
   
uint16_t TrbBridgeFrame::transactionNumber() const {
   if (isStartFrame() || isStopFrame())
      return *_content & 0x0fff;
   
   // inner frame: bits 11 .. 7
   return (_content[0] >> 7) & 0x1f;
}
   
   
bool TrbBridgeFrame::isStartFrame() const {
   return (_content[0] >> 14) & 1;
}
   
bool TrbBridgeFrame::isStopFrame() const {
   return (_content[0] >> 15) & 1;
}

bool TrbBridgeFrame::matchesFrameNumber(uint16_t frameNo) const {
   if (isStartFrame())
      return frameNo == 0;
   
   if (isStopFrame())
      return true;
   
   return (frameNo & 0x7f) == frameNumber();
}
   
   
bool TrbBridgeFrame::matchesTransactionNumber(uint16_t transactionNo) const {
   if (isStartFrame() || isStopFrame())
      return (transactionNo & 0x0fff) == transactionNumber();
   
   return (transactionNo & 0x1f) == transactionNumber();
}

   
std::string TrbBridgeFrame::dump() const {
   std::stringstream ss;
   
   ss << boost::format("Frame (Start: %s, Stop: %s, TransNo: % 4d, FrameNo: ") % isStartFrame() % isStopFrame() % transactionNumber();
   if (isStopFrame())
      ss << "???";
   else
      ss << boost::format("% 3d") % frameNumber();
   
   ss << boost::format(", size: % 2d bytes): ") % payloadSize();
   
   for(unsigned int i=0; i < payloadSize()/2; i++) {
      if (i % 16 == 0)
         ss << std::endl;
      ss << boost::format(" [% 2d] %04x") % (i+1) % payload()[i];
   }
   
   ss << std::endl;
   
   return ss.str();
}

TrbBrideFrameException::TrbBrideFrameException(const std::string& what_arg, const TrbBridgeFrame* frameInvolved) 
  : _what("")
{
   if (frameInvolved) {
      std::stringstream ss;
      ss << what_arg << std::endl << "Related to " << frameInvolved->dump();
      _what = ss.str();
   } else {
      _what = what_arg;
   }
}




