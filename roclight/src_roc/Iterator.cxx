#include "roc/Iterator.h"

roc::Iterator::Iterator(MessageFormat fmt) :
   fFormat(fmt),
   fBuffer(0),
   fBufferLen(0),
   fBufferPos(0),
   fMsgSize(0),
   fMsg(),
   fEpoch(0),
   fBoard(0),
   fOwner(false)
{
   fMsgSize = roc::Message::RawSize(fFormat);
   for (unsigned n=0;n<MaxGet4;n++) fEpoch2[n] = 0;
}

roc::Iterator::Iterator(const Iterator& src) :
   fFormat(src.fFormat),
   fBuffer(src.fBuffer),
   fBufferLen(src.fBufferLen),
   fBufferPos(src.fBufferPos),
   fMsgSize(src.fMsgSize),
   fMsg(src.fMsg),
   fEpoch(src.fEpoch),
   fBoard(0),
   fOwner(false)
{
   for (unsigned n=0;n<MaxGet4;n++) fEpoch2[n] = src.fEpoch2[n];
}


roc::Iterator::Iterator(roc::Board* brd, bool owner) :
   fFormat(brd->getMsgFormat()),
   fBuffer(0),
   fBufferLen(0),
   fBufferPos(0),
   fMsgSize(0),
   fMsg(),
   fEpoch(0),
   fBoard(brd),
   fOwner(owner)
{
   fMsgSize = roc::Message::RawSize(fFormat);
   setRocNumber(brd->rocNumber());
   for (unsigned n=0;n<MaxGet4;n++) fEpoch2[n] = 0;
}

roc::Iterator::Iterator(const char* filename) :
   fFormat(roc::formatEth1),
   fBuffer(0),
   fBufferLen(0),
   fBufferPos(0),
   fMsgSize(0),
   fMsg(),
   fEpoch(0),
   fBoard(0),
   fOwner(false)
{
   for (unsigned n=0;n<MaxGet4;n++) fEpoch2[n] = 0;
   roc::Board* brd = roc::Board::Connect(filename);
   if (brd!=0) {
      fBoard = brd;
      fOwner = true;
      brd->startDaq();

      fFormat = brd->getMsgFormat();
      setRocNumber(brd->rocNumber());
      fMsgSize = roc::Message::RawSize(fFormat);
   }
}

roc::Iterator::~Iterator()
{
   if (fOwner) {
      roc::Board::Close(fBoard);
      fBoard = 0;
      fOwner = false;
   }
}

bool roc::Iterator::isFile() const
{
   if (fBoard==0) return false;

   return fBoard->isFile();
}


void roc::Iterator::setFormat(MessageFormat fmt)
{
   fFormat = fmt;
   fMsgSize = roc::Message::RawSize(fFormat);
}


void roc::Iterator::setRocNumber(uint16_t rocnum)
{
   if (fFormat == formatEth2)
      fMsg.setRocNumber(rocnum);
}

void roc::Iterator::resetEpochs()
{
   fEpoch = 0;
   for (unsigned n=0;n<MaxGet4;n++) fEpoch2[n] = 0;
}

bool roc::Iterator::assign(void* buf, uint32_t len)
{
   fBuffer = buf;
   fBufferLen = len;
   fBufferPos = 0;

   return len >= fMsgSize;
}

bool roc::Iterator::nextBuffer(roc::Board* brd, double tmout)
{
   if (brd==0) return false;

   void *buf = 0;
   unsigned len = 0;

   if (!brd->getNextBuffer(buf, len, tmout)) return false;

   fFormat = brd->getMsgFormat();
   setRocNumber(brd->rocNumber());
   fMsgSize = roc::Message::RawSize(fFormat);

   return assign(buf, len);
}

void roc::Iterator::printMessage(unsigned kind)
{
   msg().printData(kind, getMsgEpoch());
}


void roc::Iterator::printMessages(unsigned cnt, unsigned kind)
{
   while (cnt-- > 0) {
      if (!next()) return;
      msg().printData(kind, getMsgEpoch());
   }
}
