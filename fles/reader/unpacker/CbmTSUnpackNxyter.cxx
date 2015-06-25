// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                      CbmTSUnpackNxyter                            -----
// -----                    Created 07.11.2014 by                          -----
// -----                   E. Ovcharenko, F. Uhlig                         -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "CbmTSUnpackNxyter.h"

#include "CbmNxyterRawMessage.h"
#include "CbmNxyterRawSyncMessage.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <stdint.h>

#define VERBOSE

struct DTM_header
{
   uint8_t packet_length;
   uint8_t packet_counter;
   uint16_t ROC_ID;

   void Dump() {
      #ifdef VERBOSE
      printf ("HEADER ======================= pack_len=0x%02X, pack_cnt=0x%02X, ROC=0x%04X\n",
               packet_length, packet_counter, ROC_ID);
      #endif
   }
};

CbmTSUnpackNxyter::CbmTSUnpackNxyter()
  : CbmTSUnpack(),
    fNxyterRaw(new TClonesArray("CbmNxyterRawMessage", 10)),
    fNxyterRawSync(new TClonesArray("CbmNxyterRawSyncMessage", 10))
{
    for( size_t iRoc = 0; iRoc < fgkNRocsMax; ++iRoc ) {
        fCurrEpoch[ iRoc ] = 0;
    }
}

CbmTSUnpackNxyter::~CbmTSUnpackNxyter()
{
}

Bool_t CbmTSUnpackNxyter::Init()
{
  LOG(INFO) << "Initializing nxyter unpacker" << FairLogger::endl;

  FairRootManager* ioman = FairRootManager::Instance();
  if (ioman == NULL) {
    LOG(FATAL) << "No FairRootManager instance" << FairLogger::endl;
  }
  ioman->Register("NxyterRawMessage", "nXYTER raw data", fNxyterRaw, kTRUE);
  ioman->Register("NxyterRawSyncMessage", "nXYTER raw sync data", fNxyterRawSync, kTRUE);

  return kTRUE;
}

Bool_t CbmTSUnpackNxyter::DoUnpack(const fles::Timeslice& ts, size_t component)
{
   // Loop over microslices
   for (size_t m = 0; m < ts.num_microslices(component); ++m)
   {
      auto msDescriptor = ts.descriptor(component, m);
      const uint8_t* msContent = reinterpret_cast<const uint8_t*>(ts.content(component, m));

      uint32_t offset = 16;
      uint32_t local_offset = 0;
      const uint8_t* msContent_shifted;
      int padding;

      // Loop over the data of one microslice
      while (offset < msDescriptor.size)
      {
         msContent_shifted = &msContent[offset];

         // Extract DTM header info
         DTM_header cur_DTM_header;

         cur_DTM_header.packet_length = msContent_shifted[0];
         cur_DTM_header.packet_counter = msContent_shifted[1];
         const uint16_t* ROC_ID_pointer = reinterpret_cast<const uint16_t*>(&msContent_shifted[2]);
         cur_DTM_header.ROC_ID = *ROC_ID_pointer;

         // Debug printout
         cur_DTM_header.Dump();

         uint32_t packageSize = static_cast<uint32_t>(cur_DTM_header.packet_length*2+4);

         // Loop over messages
         local_offset = 4;
         while (local_offset < packageSize)
         {
            // Extract the message type to define which procedure to apply to the 6-bytes message
            uint8_t messageType = (msContent_shifted[local_offset+5] >> 0) & 0x07;   // 3 bits

            this->Print6bytesMessage(&msContent_shifted[local_offset]);

            switch(messageType) {
            case MSG_HIT:
               this->ProcessMessage_hit(&msContent_shifted[local_offset], msDescriptor.eq_id, cur_DTM_header.ROC_ID);
               break;
            case MSG_EPOCH:
               this->ProcessMessage_epoch(&msContent_shifted[local_offset], cur_DTM_header.ROC_ID);
               break;
            case MSG_SYNC:
               this->ProcessMessage_sync(&msContent_shifted[local_offset], msDescriptor.eq_id, cur_DTM_header.ROC_ID);
               break;
            case MSG_AUX:
               this->ProcessMessage_aux(&msContent_shifted[local_offset], msDescriptor.eq_id, cur_DTM_header.ROC_ID);
               break;
            case MSG_SYS:
               this->ProcessMessage_sys(&msContent_shifted[local_offset], msDescriptor.eq_id, cur_DTM_header.ROC_ID);
               break;
            }

            local_offset += 6; // next message
         }

         offset += packageSize;

         // shift some more bytes to fit the CbmNet package size
         if ((padding = offset % 8) > 0) {
            offset += (8-padding);
         }

      }

   }

  return kTRUE;
}

void CbmTSUnpackNxyter::Reset()
{
  fNxyterRaw->Clear();
  fNxyterRawSync->Clear();
}

/*
void CbmTSUnpackNxyter::Register()
{
}
*/

void CbmTSUnpackNxyter::Print6bytesMessage(const uint8_t* msContent_shifted)
{
   #ifdef VERBOSE
   printf("0x%02X%02X%02X%02X%02X%02X :\t",
            msContent_shifted[0], msContent_shifted[1],
            msContent_shifted[2], msContent_shifted[3],
            msContent_shifted[4], msContent_shifted[5]);
   #endif
}


// Last epoch flag, overflow flag, pileup flag, ADC value, channel, timestamp, LTS, NX id, ROC id, messageType=1
// [LOPAAAAA][AAAAAAA-][CCCCCCCT][TTTTTTTT][TTTTTLLL][NNRRR001]
void CbmTSUnpackNxyter::ProcessMessage_hit(const uint8_t* msContent_shifted, uint16_t EqID, uint16_t RocID)
{

   Int_t messageType, rocID, nxID, lts, timestamp, channel, ADCvalue;
   Bool_t pileUp, overflow, lastEpoch;

   messageType = (msContent_shifted[5] >> 0) & 0x07;          // 3 bits
   rocID       = (msContent_shifted[5] >> 3) & 0x07;          // 3 bits
   nxID        = (msContent_shifted[5] >> 6) & 0x03;          // 2 bits
   lts         = (msContent_shifted[4] >> 0) & 0x07;          // 3 bits
   timestamp  = ((msContent_shifted[4] >> 3) & 0x1F)          // 5 bits
             + (((msContent_shifted[3] >> 0) & 0xFF) << 5)    // 8 bits
             + (((msContent_shifted[2] >> 0) & 0x01) << 13);  // 1 bit
   channel     = (msContent_shifted[2] >> 1) & 0x7F;          // 7 bits
   ADCvalue   = ((msContent_shifted[1] >> 1) & 0x7F)          // 7 bits
             + (((msContent_shifted[0] >> 0) & 0x1F) << 7);   // 5 bits
   pileUp      = (msContent_shifted[0] >> 5) & 0x01;          // 1 bit
   overflow    = (msContent_shifted[0] >> 6) & 0x01;          // 1 bit
   lastEpoch   = (msContent_shifted[0] >> 7) & 0x01;          // 1 bit

   #ifdef VERBOSE
      printf("messType=%d\tROC=0x%01X\tNX=0x%01X\tLTS=0x%01X\tTs=0x%04X\tCh=0x%02X\tADC=0x%04X\tpileUp=%d\toverfl=%d\tlastE=%d\n",
         messageType, rocID, nxID, lts, timestamp, channel, ADCvalue, pileUp, overflow, lastEpoch);
   #endif

   new( (*fNxyterRaw)[fNxyterRaw->GetEntriesFast()] )
   CbmNxyterRawMessage(
         EqID,
         RocID*4 + nxID,
         channel,
         fCurrEpoch[RocID], //              - cur_hit_data.NxLastEpoch,  //TODO subtract here or in GetFullTime() method ?!
         timestamp,
         ADCvalue,
         lastEpoch,
         pileUp,
         overflow);
}

// Missed event (currently skipped), epoch counter value, ROC id, messageType=2
// [MMMMMMMM][EEEEEEEE][EEEEEEEE][EEEEEEEE][EEEEEEEE][--RRR010]
void CbmTSUnpackNxyter::ProcessMessage_epoch(const uint8_t* msContent_shifted, uint16_t RocID)
{
   Int_t messageType = (msContent_shifted[5] >> 0) & 0x07;          // 3 bits

   fCurrEpoch[ RocID ] = ((msContent_shifted[4] >> 0) & 0xFF)               // 8 bits
                     + (((msContent_shifted[3] >> 0) & 0xFF) << 8)          // 8 bits
                     + (((msContent_shifted[2] >> 0) & 0xFF) << 16)         // 8 bits
                     + (((msContent_shifted[1] >> 0) & 0xFF) << 24);        // 8 bits

   // Debug printout
   #ifdef VERBOSE
   printf("messType=%d\tepoch=0x%08X\troc=%04d\n", messageType, fCurrEpoch[RocID], RocID);
   #endif
}


// Status, Data, Timestamp, syncChannel, ROC id, messageType=3
// [SSDDDDDD][DDDDDDDD][DDDDDDDD][DDTTTTTT][TTTTTTTT][CCRRR011]
void CbmTSUnpackNxyter::ProcessMessage_sync(const uint8_t* msContent_shifted, uint16_t EqID, uint16_t RocID)
{
   Int_t messageType, rocID, syncCh, timestamp, data, status;

   messageType = (msContent_shifted[5] >> 0) & 0x07;          // 3 bits
   rocID       = (msContent_shifted[5] >> 3) & 0x07;          // 3 bits
   syncCh      = (msContent_shifted[5] >> 6) & 0x03;          // 2 bits
   timestamp  = ((msContent_shifted[4] >> 0) & 0xFF)          // 8 bits
             + (((msContent_shifted[3] >> 0) & 0x3F) << 8);   // 6 bits
   data       = ((msContent_shifted[3] >> 6) & 0x03)          // 2 bits
             + (((msContent_shifted[2] >> 0) & 0xFF) << 2)    // 8 bits
             + (((msContent_shifted[1] >> 0) & 0xFF) << 10)   // 8 bits
             + (((msContent_shifted[0] >> 0) & 0x3F) << 18);  // 6 bits
   status      = (msContent_shifted[0] >> 6) & 0x03;          // 2 bits

   new( (*fNxyterRawSync)[fNxyterRawSync->GetEntriesFast()] )
   CbmNxyterRawSyncMessage(
         EqID,
         RocID*4 + syncCh,  //TODO check
         syncCh,
         fCurrEpoch[RocID], // - cur_hit_data.NxLastEpoch,  //TODO subtract here or in GetFullTime() method ?!
         timestamp,
         status,
         data);

   #ifdef VERBOSE
      printf("messType=%d\tROC=0x%01X\tsyncCh=0x%01X\tTs=0x%04X\tdata=0x%08X\tstat=0x%01X\n",
         messageType, rocID, syncCh, timestamp, data, status);
   #endif
}

// Pileup flag, edge type, timestamp, channel, ROC id, messageType=4
// [--------][--------][---PETTT][TTTTTTTT][TTTCCCCC][CCRRR100]
void CbmTSUnpackNxyter::ProcessMessage_aux(const uint8_t* msContent_shifted, uint16_t /*EqID*/, uint16_t /*RocID*/)
{
   Int_t messageType, rocID, channel, timestamp;
   Bool_t edgeType, pileUp;

   messageType = (msContent_shifted[5] >> 0) & 0x07;          // 3 bits
   rocID       = (msContent_shifted[5] >> 3) & 0x07;          // 3 bits
   channel    = ((msContent_shifted[5] >> 6) & 0x03)          // 2 bits
             + (((msContent_shifted[4] >> 0) & 0x1F) << 2);   // 5 bits

   timestamp  = ((msContent_shifted[4] >> 5) & 0x07)          // 3 bits
             + (((msContent_shifted[3] >> 0) & 0xFF) << 3)    // 8 bits
             + (((msContent_shifted[2] >> 0) & 0x07) << 11);  // 3 bit

   edgeType    = (msContent_shifted[2] >> 3) & 0x01;          // 1 bit
   pileUp      = (msContent_shifted[2] >> 4) & 0x01;          // 1 bit

   //TODO put the unpacked data into some output array

   #ifdef VERBOSE
      printf("messType=%d\tROC=0x%01X\tCh=0x%02X\tTs=0x%04X\tedgeType=%d\tpileUp=%d\n",
         messageType, rocID, channel, timestamp, edgeType, pileUp);
   #endif
}

// First four bytes are yet not unpacked, system message type, NX id, ROC id, messageType=7
// [--------][--------][--------][--------][TTTTTTTT][NNRRR111]
void CbmTSUnpackNxyter::ProcessMessage_sys(const uint8_t* msContent_shifted, uint16_t /*EqID*/, uint16_t /*RocID*/)
{
   Int_t messageType, rocID, nxID, sysMessType;

   messageType = (msContent_shifted[5] >> 0) & 0x07;          // 3 bits
   rocID       = (msContent_shifted[5] >> 3) & 0x07;          // 3 bits
   nxID        = (msContent_shifted[5] >> 6) & 0x03;          // 2 bits
   sysMessType = (msContent_shifted[4] >> 0) & 0xFF;          // 8 bits

   //TODO put the unpacked data into some output array

   #ifdef VERBOSE
      printf("messType=%d\tROC=0x%01X\tNX=0x%02X\tsysMessType=%02X\n",
         messageType, rocID, nxID, sysMessType);
   #endif
}

ClassImp(CbmTSUnpackNxyter)
