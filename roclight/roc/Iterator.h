#ifndef ROC_ITERATOR_H
#define ROC_ITERATOR_H

#include "roc/Message.h"

#include "roc/Board.h"

namespace roc {

   class Iterator {
      protected:
         enum { MaxGet4=16 };
      
         MessageFormat fFormat;
         void*         fBuffer;     // assigned buffer
         uint32_t      fBufferLen;  // length of assigned buffer
         uint32_t      fBufferPos;  // current position
         uint32_t      fMsgSize;    // size of single message
         Message       fMsg;        // current read message
         uint32_t      fEpoch;      // current epoch
         uint32_t      fEpoch2[MaxGet4];  // current epoch2 for each Get4
         Board*        fBoard;      // board instance, which delivers buffers
         bool          fOwner;      // owner of board instance

      public:
         Iterator(MessageFormat fmt = formatNormal);

         Iterator(Board* brd, bool owner = false);

         Iterator(const Iterator& src);

         Iterator(const char* filename);

         virtual ~Iterator();

         void setFormat(MessageFormat fmt);
         MessageFormat getFormat() const { return fFormat; }

         bool isFile() const;

         void setRocNumber(uint16_t rocnum = 0);

         uint32_t getMsgSize() const { return fMsgSize; }

         bool assign(void* buf, uint32_t len);

         bool nextBuffer(Board* brd, double tmout = 1.);

         void resetEpochs();

         inline bool next(double tmout = 1.)
         {
            if ((fBuffer==0) || (fBufferPos>=fBufferLen))
               if (!nextBuffer(fBoard, tmout)) return false;
            if (fMsg.assign((uint8_t*) fBuffer + fBufferPos, fFormat)) {
               fBufferPos += fMsgSize;
               switch (fMsg.getMessageType()) {
                  case MSG_EPOCH:  fEpoch = fMsg.getEpochNumber(); break;
                  case MSG_EPOCH2:
                     if (fMsg.getEpoch2ChipNumber() < MaxGet4) fEpoch2[fMsg.getEpoch2ChipNumber()] = fMsg.getEpoch2Number();
                     break;
               }
               return true;
            }
            fBufferPos = fBufferLen;
            return false;
         }

         // can be used only inside buffer, not with board source
         inline bool last()
         {
            if ((fBuffer==0) || (fBufferLen<fMsgSize)) return false;

            fBufferPos = (fBufferLen - fMsgSize) / fMsgSize * fMsgSize;

            return next(0.);
         }

         // can be used only inside buffer, not with board source
         inline bool prev()
         {
            if ((fBuffer==0) || (fBufferPos<fMsgSize*2)) return false;

            fBufferPos -= fMsgSize*2;

            return next(0.);
         }

         uint32_t getLastEpoch() const { return fEpoch; }

         uint32_t getLastEpoch2(unsigned n=0) const { return n<MaxGet4 ? fEpoch2[n] : 0; }

         inline Message& msg() { return fMsg; }

         inline uint32_t getMsgEpoch() const 
         {
            switch(fMsg.getMessageType()) {
               case MSG_EPOCH2:
//		 return (fMsg.getEpoch2ChipNumber() < MaxGet4) ? fEpoch2[fMsg.getEpoch2ChipNumber()] : 0;
                  return fMsg.getEpoch2Number();
               case MSG_GET4:
                  return (fMsg.getGet4Number() < MaxGet4) ? fEpoch2[fMsg.getGet4Number()] : 0;
            }
            return fEpoch;
         }

         inline uint64_t getMsgFullTime() const
         {
            return fMsg.getMsgFullTime(getMsgEpoch());
         }

         inline double getMsgFullTimeD() const
         {
            return fMsg.getMsgFullTimeD(getMsgEpoch());
         }

         void printMessage(unsigned kind = msg_print_Prefix | msg_print_Data);

         void printMessages(unsigned cnt = 100, unsigned kind = msg_print_Prefix | msg_print_Data);
   };
}


#endif
