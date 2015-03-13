#ifndef ROC_SORTER_H
#define ROC_SORTER_H

#include "roc/Message.h"

#include "roc/Iterator.h"

namespace roc {

   class Sorter {
      protected:
         // only because of losses of epoch markers tmBoundary is expanded, normally should be about 0x4100

         enum TimesConstants { tmLastValid = 0x3fffffff, tmEmpty = tmLastValid + 100, tmFailure = tmLastValid + 200, tmBoundary = 0x8100, tmFrontShift = 0x10000000 };

         unsigned     fRocId;

         unsigned     fIntBufMaxSize;    // maximum number of hits in intern buffer
         unsigned     fExtBufMaxSize;    // maximum size of extern buffer by addData

         Message     *fIntBuf;
         uint32_t    *fIntBufTimes;
         unsigned     fIntBufSize;   // number of hits in internal buffer
         uint32_t     fIntBufFrontEpoch;  // basis epoch number of intern buffers
         bool         fIsIntBufFrontEpoch; // true if we have value for fIntBufFrontEpoch
         uint32_t     fIntBufCurrEpoch; // current epoch of intern buffer in special form
         uint32_t     fIntBufCurrRealEpoch; // current epoch of intern buffer in real form
         uint32_t     fIntBuffCurrMaxTm;  // current maxtm of intern buffer

         unsigned     fAccumMissed;    // accumulated missed hits from epochs to write at generated epoch
         uint32_t     fLastOutEpoch;   // value of last written epoch to output

         void*        fFillBuffer;     // buffer to fill
         unsigned     fFillTotalSize;  // total size of external buffer in bytes
         unsigned     fFillSize;       // already filled size in messages
         MessageFormat fFillFormat;    // data format of fill buffer

         void*         fInternOutBuf;     // internal output buffer, which can be used for output
         unsigned      fInternOutBufSize; // size of internal output buffer in messages
         MessageFormat fInternOutFormat;  // data format of fill buffer

         bool flushBuffer(void* data, unsigned num_msg, MessageFormat fmt, bool force = false);

         bool checkLastEpoch(roc::Message* data, unsigned indx, const roc::Iterator& iter);

      public:
         explicit Sorter(unsigned maxinpbuf = 1024,
                         unsigned intern_out_buf = 1024,
                         MessageFormat intern_out_fmt = roc::formatNormal,
                         unsigned intbuf = 128);
         virtual ~Sorter();

         // is data in internal buffer
         bool isInternData() const { return fIntBufSize > 0; }

         // Add messages as they come from ROC
         // Normally no more than 243 messages should be added - size of UDP packet
         // Return true if buffer is already filled
         bool addData(void* data, unsigned datalen, MessageFormat fmt, bool flush_data = false);

         bool flush() { return flushBuffer(0, 0, formatNormal, true); }

         // clean all messages from buffers, can continue filling
         void cleanBuffers();

         // number of messages, filled in output buffer
         unsigned sizeFilled() const { return fFillSize; }

         void* filledBuf() const { return fFillBuffer; }

         // skip num messages in output buffer
         bool shiftFilledData(unsigned num);

         // initialize variables to be able fill external buffer
         // return false, if buffer size is too small
         // if buf==0, internal buffer of specified size will be allocated
         bool startFill(void* buf, unsigned totalsize, MessageFormat fmt = formatNormal);

         // Stop filling of external buffer, next messages goes into internal (if exists)
         void stopFill();
   };

}

#endif
