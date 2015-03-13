#include "roc/Sorter.h"

#include "roc/Iterator.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

roc::Sorter::Sorter(unsigned max_inp_buf,
                    unsigned intern_out_buf,
                    MessageFormat intern_out_fmt,
                    unsigned intbuf)
{
   fRocId = 0;

   fIntBufMaxSize = intbuf;
   fExtBufMaxSize = max_inp_buf;

   fIntBuf = (roc::Message*) malloc(fIntBufMaxSize*sizeof(roc::Message));
   fIntBufTimes = new uint32_t[fIntBufMaxSize + fExtBufMaxSize];

   fIntBufSize = 0;
   fIntBufFrontEpoch = 0;
   fIsIntBufFrontEpoch = false;
   fIntBufCurrEpoch = 0;
   fIntBufCurrRealEpoch = 0;
   fIntBuffCurrMaxTm = 0;

   fAccumMissed = 0;
   fLastOutEpoch = 0;

   fInternOutBufSize = intern_out_buf;
   fInternOutFormat = intern_out_fmt;
   fInternOutBuf = fInternOutBufSize==0 ? 0 : malloc(fInternOutBufSize * roc::Message::RawSize(fInternOutFormat));

   fFillBuffer = fInternOutBuf;
   fFillTotalSize = fInternOutBufSize;
   fFillSize = 0;
   fFillFormat = fInternOutFormat;
}

roc::Sorter::~Sorter()
{
   free(fIntBuf); fIntBuf = 0;
   free(fInternOutBuf); fInternOutBuf = 0;
   delete [] fIntBufTimes;
}

bool roc::Sorter::startFill(void* buf, unsigned totalsize, MessageFormat fmt)
{
   if (fFillSize!=0) {
      printf("There is %u messages in output, cannot start !!!\n", fFillSize);
      return false;
   }

   unsigned msg_size = roc::Message::RawSize(fmt);

   if (totalsize < 243*msg_size) {
      printf("roc::Sorter::startFill provided buffer size %u too small\n", totalsize);
      return false;
   }

   fFillBuffer = buf;
   fFillTotalSize = totalsize / msg_size;
   fFillSize = 0;
   fFillFormat = fmt;

   return true;
}

void roc::Sorter::cleanBuffers()
{
   // anything which was in internal buffer, is lost
   fIntBufSize = 0;

   // all messages in output are gone
   fFillSize = 0;

   // reset all internal variables
   fIntBufFrontEpoch = 0;
   fIsIntBufFrontEpoch = false;
   fIntBufCurrEpoch = 0;
   fIntBufCurrRealEpoch = 0;
   fIntBuffCurrMaxTm = 0;

   fAccumMissed = 0;
   fLastOutEpoch = 0;
}


void roc::Sorter::stopFill()
{
   fFillBuffer = fInternOutBuf;
   fFillTotalSize = fInternOutBufSize;
   fFillSize = 0;
   fFillFormat = fInternOutFormat;
}

bool roc::Sorter::checkLastEpoch(roc::Message* data, unsigned indx, const roc::Iterator& iter)
{
   if (indx==0) return false;

   uint8_t lastid = data->getNxChNum();
   unsigned revindx = indx;

   unsigned loopcnt = 0;
   unsigned fifocnt = 1; // we suppose that fifo had only item

   uint32_t hittime = (fIntBufCurrEpoch - 0x4000) | data->getNxTs();
   // 16 ns?  - time needed to get hit ready in pipe
   uint32_t candetecttime = hittime + 48;
   uint32_t maxhittime = 0;

   roc::Message* rev = 0;
   roc::Iterator reviter(iter);

   while (revindx-->0) {
      if (revindx<fIntBufSize)
         rev = fIntBuf + revindx;
      else {
         if (!reviter.prev()) {
            fprintf(stderr, "Cannot get previous message\n");
            return false;
         }
         rev = &reviter.msg();
      }

      // in any case hit will be detected in 1 epoch
      if (hittime > fIntBufTimes[revindx] + 0x4400) break;

      // take only hits which were
      if (!rev->isHitMsg() || (rev->getNxNumber() != data->getNxNumber())) continue;

      // define time of any message, that was where
      if (fIntBufTimes[revindx] > maxhittime) maxhittime = fIntBufTimes[revindx];

      candetecttime += 32; // we need 32 ns to read out hit

      if (rev->getNxChNum() >= lastid) loopcnt++;

      if (rev->getNxChNum() == data->getNxChNum()) fifocnt++;

      lastid = rev->getNxChNum();

      // our fifo cannot be more that 4 items longer
      // or if we made more loops than piped items in channel
      if ((fifocnt==4) || (loopcnt > fifocnt)) break;
   }

   // if last hit in the nXYTER comes later than time when with guarantee we can detect our hit,
   // it is not a normal and cannot be
   // set security factor
   if (maxhittime > candetecttime) return false;

   // why to have last epoch if hit can be detected in same epoch??
   if (candetecttime >> 14 == hittime >> 14) return false;

   return true;
}


bool roc::Sorter::addData(void* new_data, unsigned datalen, MessageFormat fmt, bool flush_data)
{
   if (fFillBuffer==0) return false;
   if (datalen == 0) return true;

   unsigned msg_size = roc::Message::RawSize(fmt);
   unsigned num_msg = datalen / msg_size;

   if (fIntBufSize + num_msg > fIntBufMaxSize) {
      printf("One always need enough internal buffer space to be able store complete data buffer now:%u add:%u total:%u\n",
               fIntBufSize, num_msg, fIntBufMaxSize);
      return false;
   }

   roc::Iterator iter(fmt);
   iter.assign(new_data, datalen);

   uint32_t diff = 0;

   unsigned total_data_size = fIntBufSize + num_msg;

   // here we filling time stamp value for all hits, found in data
   for (unsigned indx = fIntBufSize; indx < total_data_size; indx++) {

      // if (indx<10) { printf ("indx%u", indx); data->printData(7); }

      if (!iter.next()) {
         printf("Cannot iterate over input buffer - FATAL ERROR!!!");
         exit(1);
      }

      roc::Message* data = &iter.msg();
//      data->printData(std::cerr, 7);

      switch (data->getMessageType()) {
         case roc::MSG_HIT:
            if (data->getNxLastEpoch()==0) {
               fIntBufTimes[indx] = fIntBufCurrEpoch | data->getNxTs();
            } else
            if (fIntBufCurrEpoch > 0) {
               fIntBufTimes[indx] = fIntBufCurrEpoch | data->getNxTs();

               if (checkLastEpoch(data, indx, iter)) fIntBufTimes[indx] -= 0x4000;
//                                               else fprintf(stderr, "Last epoch bit cannot be!!!\n");
            } else {
               fIntBufTimes[indx] = tmFailure;
               fprintf(stderr, "HIT Last epoch err - should not happen !!!\n");
            }
            break;
         case roc::MSG_EPOCH:
            fRocId = data->getRocNumber();

            fIntBufCurrRealEpoch = data->getEpochNumber();
            // when we start filling from indx==0, there is no data in buffer
            // and one can reset front epoch, take it from past to avoid any problem with "jumping" messages
            if ((indx==0) || !fIsIntBufFrontEpoch) {
               fIntBufFrontEpoch = fIntBufCurrRealEpoch - 0x10;
               fIsIntBufFrontEpoch = true;
               fIntBuffCurrMaxTm = 0;
            }

            diff = fIntBufCurrRealEpoch - fIntBufFrontEpoch; // no problem with overflow, epoch has 32 bit

            // epoch difference from current epoch too much - try flush and starts from beginning
            if (diff >= 0x20000) {
               printf("Epoch diff too much %x - %x = %x - try to repair\n", fIntBufCurrRealEpoch, fIntBufFrontEpoch, diff);

               unsigned hitsprocessed = indx - fIntBufSize;

               if (!flushBuffer(new_data, hitsprocessed, fmt, true)) {
                   printf ("Cannot flush buffer - error!!!\n");
                   return false;
               }

               return addData((uint8_t*)new_data + hitsprocessed*msg_size, (num_msg - hitsprocessed)*msg_size , fmt);
            }

            fIntBufCurrEpoch = diff << 14; // store current epoch in shifted manner
            fIntBufTimes[indx] = fIntBufCurrEpoch;

            break;
         case roc::MSG_SYNC:
            if (data->getSyncEpochLSB() == (fIntBufCurrRealEpoch & 0x1))
               fIntBufTimes[indx] = fIntBufCurrEpoch | data->getSyncTs();
            else
            if (fIntBufCurrEpoch > 0) {
               fIntBufTimes[indx] = (fIntBufCurrEpoch - 0x4000) | data->getSyncTs();
//               printf("SYNC marker in previous epoch cur:%x \n", fIntBufCurrRealEpoch);
            } else {
               printf("SYNC Last epoch err - should not happen !!!\n");
               fIntBufTimes[indx] = tmFailure;
            }
            break;
         case roc::MSG_AUX:
            if (data->getAuxEpochLSB() == (fIntBufCurrRealEpoch & 0x1))
               fIntBufTimes[indx] = fIntBufCurrEpoch | data->getAuxTs();
            else
            if (fIntBufCurrEpoch > 0)
               fIntBufTimes[indx] = (fIntBufCurrEpoch - 0x4000) | data->getAuxTs();
            else {
               printf("AUX Last epoch err - should not happen !!!\n");
               fIntBufTimes[indx] = tmFailure;
            }
            break;
         case roc::MSG_SYS:
            fIntBufTimes[indx] = fIntBuffCurrMaxTm;
            if (data->isStopDaqMsg()) flush_data = true;
            break;
         default:
            fIntBufTimes[indx] = tmEmpty;
      }

      if ((fIntBufTimes[indx] <= tmLastValid) && (fIntBufTimes[indx]>fIntBuffCurrMaxTm))
         fIntBuffCurrMaxTm = fIntBufTimes[indx];
   }

   if (!flushBuffer(new_data, num_msg, fmt, flush_data)) {
      // copy everything into internal buffer
      fprintf(stderr, "flushBuffer error, just copy it in the internal buffer\n");

      roc::Iterator iter(fmt);
      iter.assign(new_data, num_msg*msg_size);
      roc::Message* tgt = fIntBuf + fIntBufSize;
      while (iter.next()) {
         tgt->assign(iter.msg());
         tgt++;
      }
      fIntBufSize += num_msg;
   }

   if ((fIntBufSize > 0) && (fIntBufTimes[0] > tmLastValid)) {
      printf("Hard internal error !!!!\n");
      exit(1);
   }

//   printf("At the end we have %u msg intern\n", fIntBufSize);

   if (fIntBufCurrEpoch >= tmFrontShift + tmBoundary * 16)
      // be sure that no any other hits from past can come
      if ((fIntBufSize==0) || (fIntBufTimes[0] > tmFrontShift + tmBoundary * 2)) {
//         printf("shift front epoch on %x  size = %u\n", tmFrontShift, fIntBufSize);

         fIntBufFrontEpoch += tmFrontShift >> 14;
         fIntBuffCurrMaxTm -= tmFrontShift;
         fIntBufCurrEpoch -= tmFrontShift;

         for (unsigned n=0; n < fIntBufSize; n++)
            if (fIntBufTimes[n] <= tmLastValid)
               fIntBufTimes[n] -= tmFrontShift;
      }

   return true;
}

bool roc::Sorter::flushBuffer(void* new_data, unsigned num_msg, MessageFormat fmt, bool force_flush)
{
   if (fFillSize == fFillTotalSize) return false;

   unsigned tail_indx(0), head_indx(fIntBufSize);

   unsigned msg_size = roc::Message::RawSize(fmt);
   unsigned out_msg_size = roc::Message::RawSize(fFillFormat);

   uint32_t boundary = 0;
   unsigned nless = 0;
   unsigned pmin = 0;
   unsigned indx = 0;
   uint32_t item_real_epoch = 0;

   roc::Message inp_buf, out_msg;
   roc::Message* src_data = 0;

   uint8_t* out_ptr = (uint8_t*) fFillBuffer + fFillSize*out_msg_size;

   unsigned total_data_size = fIntBufSize + num_msg;
   if (total_data_size==0) return true;

   unsigned loop_limit = total_data_size;
   // if one force flushing of the buffer, make sorting loop for all items
   if (force_flush) {
     // add fake maximum time that all other hits will be flushed from buffer
      if (total_data_size >= fIntBufMaxSize + fExtBufMaxSize) {
         printf("Something completely wrong !!!!\n");
         exit(1);
      }
      fIntBufTimes[total_data_size] = tmLastValid;
      loop_limit++;
   }

   bool output_full = false;

   // from here one start to fill output buffers, doing sorting
   for (;head_indx < loop_limit; head_indx++) {

      // in case of flush last index correspond to non-existing index
      boundary = fIntBufTimes[head_indx];

      // not valid times take off from consideration
      // too small time - not enougth to start sorting
      if ((boundary > tmLastValid) || (boundary < tmBoundary)) continue;

      boundary -= tmBoundary;

      do {

         nless = 0;
         pmin = 0;

         for(indx = tail_indx; indx < head_indx; indx++)
            if (fIntBufTimes[indx] < boundary) {
               if ((nless==0) || (fIntBufTimes[indx] < fIntBufTimes[pmin])) pmin = indx;
               nless++;
            }

         if (nless==0) break;

         if (pmin < fIntBufSize)
            src_data = fIntBuf + pmin;
         else {
            inp_buf.assign(((uint8_t*) new_data) + (pmin-fIntBufSize)*msg_size, fmt);
            src_data = &inp_buf;
         }

         item_real_epoch = fIntBufFrontEpoch + (fIntBufTimes[pmin] >> 14);

         // add extra epoch to output if some timed values should be added
         if ((item_real_epoch != fLastOutEpoch) && (src_data->isHitMsg() || src_data->isSyncMsg() ||  src_data->isAuxMsg())) {

            out_msg.setMessageType(roc::MSG_EPOCH);
            out_msg.setRocNumber(fRocId);
            out_msg.setEpochNumber(item_real_epoch);
            out_msg.setEpochMissed(fAccumMissed > 255 ? 255 : fAccumMissed);

            fLastOutEpoch = item_real_epoch;
            fAccumMissed = 0;

            out_msg.copyto(out_ptr, fFillFormat);
            out_ptr += out_msg_size;
            fFillSize++;

            // cannot write item iteslf, it still marked as used
            if (fFillSize == fFillTotalSize) {
               output_full = true;
               break;
            }
         }

         bool accept_msg = false;

         switch (src_data->getMessageType()) {
            case roc::MSG_NOP:
               break;
            case roc::MSG_HIT:
               out_msg.assign(*src_data);
               out_msg.setNxLastEpoch(0);
               accept_msg = true;
               break;
            case roc::MSG_EPOCH:
               // we do not copy epoch directly, but remember value of missed hits
//               printf("get out epoch %x\n", src_data->getEpoch());

               fAccumMissed += src_data->getEpochMissed();
               break;
            case roc::MSG_SYNC:
               out_msg.assign(*src_data);
               out_msg.setSyncEpochLSB(fLastOutEpoch & 1);
               accept_msg = true;
               break;
            case roc::MSG_AUX:
               out_msg.assign(*src_data);
               out_msg.setAuxEpochLSB(fLastOutEpoch & 1);
               accept_msg = true;
               break;
            case roc::MSG_SYS:
               out_msg.assign(*src_data);
               accept_msg = true;
               break;
            default:
               printf("Absolutely strange message type %u\n", src_data->getMessageType());
               break;
         }

         if (accept_msg) {
            out_msg.copyto(out_ptr, fFillFormat);
            out_ptr += out_msg_size;
            fFillSize++;
         }

         // mark message as done
         fIntBufTimes[pmin] = tmEmpty;

         // cannot necessary to continue - output buffer is full
         if (fFillSize == fFillTotalSize) output_full = true;

      } while ((nless>1) && !output_full);

      // "eat" all items which no longer in use
      while ((tail_indx < head_indx) && (fIntBufTimes[tail_indx] > tmLastValid)) tail_indx++;

      if (output_full) break;
   }

   // now store rest of the data in internal buffers

   if (tail_indx == total_data_size) {

      fIntBufSize = 0; // everything empty !!!

   } else {

      roc::Message* tgt = fIntBuf;

      void* src = 0;

      unsigned copy_size = total_data_size - tail_indx;

      // copy data inside internal buffer first
      if (tail_indx < fIntBufSize) {
         memmove(tgt, fIntBuf + tail_indx, (fIntBufSize - tail_indx) * sizeof(roc::Message));
         tgt += (fIntBufSize - tail_indx);

         src = new_data;
         copy_size = num_msg;
      } else {
         src = (uint8_t*) new_data + (tail_indx - fIntBufSize) * msg_size;
      }

      // copy data from external buffer with conversion
      roc::Iterator iter(fmt);
      iter.assign(src, copy_size * msg_size);
      while (iter.next()) {
         tgt->assign(iter.msg());
         tgt++;
      }

      // copy calculated timing information

      fIntBufSize = total_data_size - tail_indx;

      memmove(fIntBufTimes, fIntBufTimes + tail_indx, fIntBufSize * sizeof(uint32_t));
   }

   return true;
}

bool roc::Sorter::shiftFilledData(unsigned num)
{
   if (num>=fFillSize) {
      fFillSize = 0;
      return true;
   } else
   if (num==0) return true;

   fFillSize -= num;

   unsigned msg_size = roc::Message::RawSize(fFillFormat);

   memmove(fFillBuffer, (uint8_t*) fFillBuffer + num *msg_size, fFillSize*msg_size);

   return true;
}

