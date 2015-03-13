// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                      CbmTSUnpackGet4v1x                           -----
// -----                    Created 15.12.2014 by                          -----
// -----                        P.-A. Loizeau                              -----
// -----                  Based on CbmTSUnpackGet4v1x                       -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "CbmTSUnpackGet4v1x.h"

// Specific headers
//#include "CbmNxyterRawMessage.h"
//#include "CbmNxyterRawSyncMessage.h"
#include "CbmGet4v1xDef.h"

// FAIRROOT headers
#include "FairLogger.h"
#include "FairRootManager.h"

// ROOT headers
#include "TF1.h"
#include "TH2.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TMath.h"

// C++ std headers
#include <iostream>
#include <stdint.h>

/*********
 * TODO
 * 1) Use the time sorted data => ~OK
 * 2) Counts as function of time (s bin, per chip/ROC)
 * 3) Split class
 * 4) Time sorted data output
 * 5) Monitor size of close epochs
 *********/

struct DTM_header
{
   uint8_t packet_length;
   uint8_t packet_counter;
   uint16_t ROC_ID;

   void Dump() {
      printf ("HEADER ======================= pack_len=0x%02X, pack_cnt=0x%02X, ROC=0x%04X\n",
               packet_length, packet_counter, ROC_ID);
   }
};

CbmTSUnpackGet4v1x::CbmTSUnpackGet4v1x()
: CbmTSUnpack(),
  fbVerbose(kFALSE),
  fiMode(0),
  fuNbRocs(0),
  fuNbGet4(0),
  fuMsOverlapTs(0),
  fvbActiveChips(),
  fulTsNb(0),
  fulMsNb(0),
  fsMaxMsNb(0),
  fuOffset(0),
  fuMaxOffset(0),
  fuLocalOffset(0),
  fuMaxLocalOffset(0),
  fvbRocFeetSyncStart(kFALSE),
  fvbGet4WaitFirstSync(kTRUE),
  fvuCurrEpoch(),
  fvuCurrEpochCycle(),
  fvuCurrEpoch2(),
  fvuCurrEpoch2Cycle(),
  fhMessageTypePerRoc(NULL),
  fhRocSyncTypePerRoc(NULL),
  fhRocAuxTypePerRoc(NULL),
  fhSysMessTypePerRoc(NULL),
  fhGet4EpochFlags(NULL),
  fhGet4EpochSyncDist(NULL),
  fhGet4ChanDataCount(NULL),
  fhGet4ChanDllStatus(NULL),
  fhGet4ChanTotMap(NULL),
  fhGet4ChanErrors(NULL),
  fhGet4ChanSlowContM(NULL),
  fbPulserMode(kFALSE),
  fuPulserFee(0),
  fuPulserChan(),
  fvuLastHitEp(),
  fvmLastHit(),
  fhTimeResFee(),
  fhTimeResAllFee(NULL),
  fhTimeResPairs(),
  fhTimeResCombi(),
  fhPulserHitDistNs(NULL),
  fhPulserHitDistUs(NULL),
  fhPulserHitDistMs(NULL),
  fhPulserFeeDnl(NULL),
  fhPulserFeeInl(NULL),
  fbOldReadoutOk(kFALSE),
  fhGet4ChanTotCount(NULL),
  fvuLastOldTotEp(),
  fvmLastOldTot(),
  fhPulserFeeTotDnl(NULL),
  fhPulserFeeTotInl(NULL),
  fmsOrderedEpochsData(),
  fmsOrderedEpochsBuffStat(),
  fvuCurrEpochBuffer(),
  fvuCurrEpochBufferIt(),
  fuLastEpBufferReady(0),
  fuCurrEpReadBuffer(0)
{
}

CbmTSUnpackGet4v1x::~CbmTSUnpackGet4v1x()
{
}

Bool_t CbmTSUnpackGet4v1x::Init()
{
  LOG(INFO) << "Initializing" << FairLogger::endl;

  FairRootManager* ioman = FairRootManager::Instance();
  if (ioman == NULL) {
    LOG(FATAL) << "No FairRootManager instance" << FairLogger::endl;
  }
//  ioman->Register("Get4RawMessage",       "GET4 raw data",      fGet4Raw, kTRUE);
//  ioman->Register("NxyterRawSyncMessage", "GET4 raw sync data", fGet4RawSync, kTRUE);

  if( 0 >= fuNbRocs || 0 >= fuNbGet4 )
  {
     LOG(ERROR) << "CbmTSUnpackGet4v1x::Init => Nb of ROCs or GET4s not defined!!! " << FairLogger::endl;
     LOG(ERROR) << "Nb of ROCs:"<< fuNbRocs << " Nb of GET4s : " << fuNbGet4 << FairLogger::endl;
     LOG(FATAL) << "Please use the functions SetRocNb and/or SetGet4Nb before running!!" << FairLogger::endl;
  }

  // Initialize TS counter
  fulTsNb = 0;

  // At start all ROCs and GET4 are considered "unsynchronized"
  // Stay so until the DLM 10 is issued: ~/flesnet/build/sync_frontend
  // The GET4 are synchronized after reception of 1st epoch2 with SYNC
  fvbRocFeetSyncStart.resize( fuNbRocs);
  for( UInt_t uRoc = 0; uRoc < fuNbRocs; uRoc++)
     fvbRocFeetSyncStart[uRoc] = kFALSE;
  fvbGet4WaitFirstSync.resize(fuNbGet4);
  for( UInt_t uChip = 0; uChip < fuNbGet4; uChip++)
     fvbGet4WaitFirstSync[uChip] = kTRUE;

  // Prepare the active chips flags
  if( 0 == fvbActiveChips.size() && 0 < fuNbGet4  )
  {
     fvbActiveChips.resize(fuNbGet4);
     for( UInt_t uChip = 0; uChip < fuNbGet4; uChip++)
        fvbActiveChips[uChip] = kTRUE;
  } // if( 0 == fvbActiveChips.size()
  // Prepare the epoch storing vectors
  fvuCurrEpoch.resize( fuNbRocs);
  fvuCurrEpochCycle.resize( fuNbRocs);
  for( UInt_t uRoc = 0; uRoc < fuNbRocs; uRoc++)
  {
     fvuCurrEpoch[uRoc] = 0;
     fvuCurrEpochCycle[uRoc] = 0;
  } // for( UInt_t uRoc = 0; uRoc < fuNbRocs; uRoc++)
  fvuCurrEpoch2.resize(fuNbGet4);
  fvuCurrEpoch2Cycle.resize(fuNbGet4);
  fvuCurrEpochBuffer.resize(fuNbGet4);
  fvuCurrEpochBufferIt.resize(fuNbGet4);
  for( UInt_t uChip = 0; uChip < fuNbGet4; uChip++)
  {
     fvuCurrEpoch2[uChip] = 0;
     fvuCurrEpoch2Cycle[uChip] = 0;
     fvuCurrEpochBuffer[uChip] = 0;
     fvuCurrEpochBufferIt[uChip] = fmsOrderedEpochsData.end();
  } // for( UInt_t uChip = 0; uChip < fuNbGet4; uChip++)

  InitMonitorHistograms();

  return kTRUE;
}

void CbmTSUnpackGet4v1x::SetActiveGet4( UInt_t uChipsIndex, Bool_t bActiveFlag)
{
   if( 0 == fvbActiveChips.size() && 0 < fuNbGet4 )
   {
      fvbActiveChips.resize(fuNbGet4);
      for( UInt_t uChip = 0; uChip < fuNbGet4; uChip++)
         fvbActiveChips[uChip] = kTRUE;
   } // if( 0 == fvbActiveChips.size()

   if( uChipsIndex < fuNbGet4 )
      fvbActiveChips[uChipsIndex] = bActiveFlag;
      else LOG(ERROR)<<" CbmTSUnpackGet4v1x::SetActiveGet4 => Invalid chip index "
                     << uChipsIndex <<FairLogger::endl;
}

void CbmTSUnpackGet4v1x::SetPulserChans(
      UInt_t inPulserChanA, UInt_t inPulserChanB, UInt_t inPulserChanC, UInt_t inPulserChanD,
      UInt_t inPulserChanE, UInt_t inPulserChanF, UInt_t inPulserChanG, UInt_t inPulserChanH,
      UInt_t inPulserChanI, UInt_t inPulserChanJ, UInt_t inPulserChanK, UInt_t inPulserChanL,
      UInt_t inPulserChanM, UInt_t inPulserChanN, UInt_t inPulserChanO, UInt_t inPulserChanP )
{
   fuPulserChan[ 0] = inPulserChanA;
   fuPulserChan[ 1] = inPulserChanB;
   fuPulserChan[ 2] = inPulserChanC;
   fuPulserChan[ 3] = inPulserChanD;
   fuPulserChan[ 4] = inPulserChanE;
   fuPulserChan[ 5] = inPulserChanF;
   fuPulserChan[ 6] = inPulserChanG;
   fuPulserChan[ 7] = inPulserChanH;
   fuPulserChan[ 8] = inPulserChanI;
   fuPulserChan[ 9] = inPulserChanJ;
   fuPulserChan[10] = inPulserChanK;
   fuPulserChan[11] = inPulserChanL;
   fuPulserChan[12] = inPulserChanM;
   fuPulserChan[13] = inPulserChanN;
   fuPulserChan[14] = inPulserChanO;
   fuPulserChan[15] = inPulserChanP;
}

Bool_t CbmTSUnpackGet4v1x::DoUnpack(const fles::Timeslice& ts, size_t component)
{
   fulTsNb++;
   if( 0 == fiMode || kTRUE == fbVerbose )
      LOG(INFO)<<" ++++++++++++ Ts # "<<fulTsNb<<FairLogger::endl;

   if( 2 == fiMode )
      LOG(INFO)<<" ++++++++++++ Ts # "<<fulTsNb<<" Start!"<<FairLogger::endl;

   // Loop over microslices
   fsMaxMsNb = ts.num_microslices(component) - fuMsOverlapTs;
   for (size_t m = 0; m < fsMaxMsNb; ++m)
   {
      fulMsNb = m;
      auto msDescriptor = ts.descriptor(component, m);
      const uint8_t* msContent = reinterpret_cast<const uint8_t*>(ts.content(component, m));

      uint32_t offset = 16;
      uint32_t local_offset = 0;
      const uint8_t* msContent_shifted;
      int padding;

      if( 0 == fiMode || kTRUE == fbVerbose )
         LOG(INFO)<<" ************ Ms # "<<m<<FairLogger::endl;

      fuMaxOffset = msDescriptor.size;

      // Loop over the data of one microslice
      while (offset < msDescriptor.size)
      {
         msContent_shifted = &msContent[offset];
         fuOffset = offset;

         // Extract DTM header info
         DTM_header cur_DTM_header;

         cur_DTM_header.packet_length = msContent_shifted[0];
         cur_DTM_header.packet_counter = msContent_shifted[1];
         const uint16_t* ROC_ID_pointer = reinterpret_cast<const uint16_t*>(&msContent_shifted[2]);
//         cur_DTM_header.ROC_ID = *ROC_ID_pointer;
         // TODO: Check if planned behavior
         //       -> ROC ID increased by 256 per link, 16 bits => only links 1 and 2 easy to use
         cur_DTM_header.ROC_ID = (*ROC_ID_pointer) / 256; // ROC ID increased by 256 per link

         uint32_t packageSize = static_cast<uint32_t>(cur_DTM_header.packet_length*2+4);
         fuMaxLocalOffset = packageSize;

         // Loop over messages
         local_offset = 4;
         if( 0 == fiMode || kTRUE == fbVerbose )
         {
            // Debug printout
            cur_DTM_header.Dump();
            LOG(INFO)<<" Package Size: "<<packageSize<<" ROC ID: "<<cur_DTM_header.ROC_ID<<FairLogger::endl;
            while (local_offset < packageSize)
            {
               fuLocalOffset = local_offset;
               get4v1x::Message mess;
               uint64_t dataContent =
                       ( static_cast<uint64_t>( cur_DTM_header.ROC_ID                & 0xFFFF) << 48)
                     + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 0] ) & 0xFF) << 40)
                     + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 1] ) & 0xFF) << 32)
                     + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 2] ) & 0xFF) << 24)
                     + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 3] ) & 0xFF) << 16)
                     + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 4] ) & 0xFF) <<  8)
                     + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 5] ) & 0xFF));
               mess.setData( dataContent );
               mess.printDataLog();

               local_offset += 6; // next message
            } // while (local_offset < packageSize)
         } // if( 0 == fiMode || kTRUE == fbVerbose )
         switch(fiMode)
         {
            case 0:
            {
               // debug mode, nothing to do here (printout already done)
               break;
            } // case 0
            case 1:
            {
               // Monitor mode, fill histograms
               while (local_offset < packageSize)
               {
                  fuLocalOffset = local_offset;
                  get4v1x::Message mess;
                  uint64_t dataContent =
                          ( static_cast<uint64_t>( cur_DTM_header.ROC_ID                & 0xFFFF) << 48)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 0] ) & 0xFF) << 40)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 1] ) & 0xFF) << 32)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 2] ) & 0xFF) << 24)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 3] ) & 0xFF) << 16)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 4] ) & 0xFF) <<  8)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 5] ) & 0xFF));
                  mess.setData( dataContent );

                  fhMessageTypePerRoc->Fill( cur_DTM_header.ROC_ID, mess.getMessageType() );

                  switch( mess.getMessageType() )
                  {
                     case get4v1x::MSG_HIT:
                     {
                        // This is NXYTER in a GET4 unpacker => ignore
                        break;
                     } // case get4v1x::MSG_HIT:
                     case get4v1x::MSG_EPOCH:
                     {
                        // Ignore all messages before RocFeet system SYNC
                        if (kFALSE == fvbRocFeetSyncStart[cur_DTM_header.ROC_ID] )
                           break;
                        this->MonitorMessage_epoch(mess, msDescriptor.eq_id);
                        break;
                     } // case get4v1x::MSG_EPOCH:
                     case get4v1x::MSG_SYNC:
                     {
                        // Ignore all messages before RocFeet system SYNC
                        if (kFALSE == fvbRocFeetSyncStart[cur_DTM_header.ROC_ID] )
                           break;
                        this->MonitorMessage_sync(mess, msDescriptor.eq_id);
                        break;
                     } // case get4v1x::MSG_SYNC:
                     case get4v1x::MSG_AUX:
                     {
                        // Ignore all messages before RocFeet system SYNC
                        if (kFALSE == fvbRocFeetSyncStart[cur_DTM_header.ROC_ID] )
                           break;
                        this->MonitorMessage_aux(mess, msDescriptor.eq_id);
                        break;
                     } // case get4v1x::MSG_AUX:
                     case get4v1x::MSG_EPOCH2:
                     {
                        // Ignore all messages before RocFeet system SYNC
                        if (kFALSE == fvbRocFeetSyncStart[cur_DTM_header.ROC_ID] )
                           break;
                        this->MonitorMessage_epoch2(mess, msDescriptor.eq_id);
                        break;
                     } // case get4v1x::MSG_EPOCH2:
                     case get4v1x::MSG_GET4:
                     {
                        // Ignore all messages before RocFeet system SYNC
                        if (kFALSE == fvbRocFeetSyncStart[cur_DTM_header.ROC_ID] )
                           break;
                        this->MonitorMessage_get4(mess, msDescriptor.eq_id);
                        break;
                     } // case get4v1x::MSG_GET4:
                     case get4v1x::MSG_SYS:
                        this->MonitorMessage_sys(mess, msDescriptor.eq_id);
                        break;
                     default:
                        break;
                  } // switch( mess.getMessageType() )
                  local_offset += 6; // next message
               } // while (local_offset < packageSize)
               break;
            } // case 1
            case 2:
            {
               // Normal mode, unpack data and fill TClonesArray of CbmRawMessage
               while (local_offset < packageSize)
               {
                  fuLocalOffset = local_offset;
                  get4v1x::Message mess;
                  uint64_t dataContent =
                          ( static_cast<uint64_t>( cur_DTM_header.ROC_ID                & 0xFFFF) << 48)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 0] ) & 0xFF) << 40)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 1] ) & 0xFF) << 32)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 2] ) & 0xFF) << 24)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 3] ) & 0xFF) << 16)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 4] ) & 0xFF) <<  8)
                        + ( static_cast<uint64_t>( (msContent_shifted[local_offset + 5] ) & 0xFF));
                  mess.setData( dataContent );

                  fhMessageTypePerRoc->Fill( cur_DTM_header.ROC_ID, mess.getMessageType() );
                  switch( mess.getMessageType() )
                  {
                     case get4v1x::MSG_HIT:
                     {
                        // This is NXYTER in a GET4 unpacker => ignore
                        break;
                     } // case get4v1x::MSG_HIT:
                     case get4v1x::MSG_EPOCH:
                     {
                        // Ignore all messages before RocFeet system SYNC
                        if (kFALSE == fvbRocFeetSyncStart[cur_DTM_header.ROC_ID] )
                           break;

                        this->ProcessMessage_epoch(mess, msDescriptor.eq_id);
                        break;
                     } // case get4v1x::MSG_EPOCH:
                     case get4v1x::MSG_SYNC:
                     {
                        // Ignore all messages before RocFeet system SYNC
                        if (kFALSE == fvbRocFeetSyncStart[cur_DTM_header.ROC_ID] )
                           break;

                        this->ProcessMessage_sync(mess, msDescriptor.eq_id);
                        break;
                     } // case get4v1x::MSG_SYNC:
                     case get4v1x::MSG_AUX:
                     {
                        // Ignore all messages before RocFeet system SYNC
                        if (kFALSE == fvbRocFeetSyncStart[cur_DTM_header.ROC_ID] )
                           break;

                        this->ProcessMessage_aux(mess, msDescriptor.eq_id);
                        break;
                     } // case get4v1x::MSG_AUX:
                     case get4v1x::MSG_EPOCH2:
                     {
                        // Ignore all messages before RocFeet system SYNC
                        if (kFALSE == fvbRocFeetSyncStart[cur_DTM_header.ROC_ID] )
                           break;

                        this->ProcessMessage_epoch2(mess, msDescriptor.eq_id);
                        break;
                     } // case get4v1x::MSG_EPOCH2:
                     case get4v1x::MSG_GET4:
                     {
                        // Ignore all messages before RocFeet system SYNC
                        if (kFALSE == fvbRocFeetSyncStart[cur_DTM_header.ROC_ID] )
                           break;

                        this->ProcessMessage_get4(mess, msDescriptor.eq_id);
                        break;
                     } // case get4v1x::MSG_GET4:
                     case get4v1x::MSG_SYS:
                        this->ProcessMessage_sys(mess, msDescriptor.eq_id);
                        break;
                     default:
                        break;
                  } // switch( mess.getMessageType() )

                  local_offset += 6; // next message
               } // while (local_offset < packageSize)
               break;
            } // case 2
            default:
               break;
         } // switch(fiMode)

         offset += packageSize;

         // shift some more bytes to fit the CbmNet package size
         if ((padding = offset % 8) > 0)
         {
            offset += (8-padding);
         } // if ((padding = offset % 8) > 0)

      } // while (offset < msDescriptor.size)

   } // for (size_t m = 0; m < ts.num_microslices(component); ++m)

  FinishUnpack();

  return kTRUE;
}

void CbmTSUnpackGet4v1x::FinishUnpack()
{
   // Something to do only in Processing mode
   // TODO: Use the stored data for something usefull !!!
   if( 2 == fiMode )
   {
      if( kFALSE == fvbGet4WaitFirstSync[0] )
      {
      LOG(DEBUG)<<" CbmTSUnpackGet4v1x::FinishUnpack => Stored epochs: "
               <<fmsOrderedEpochsData.size()<<" "
               <<fmsOrderedEpochsBuffStat.size()<<FairLogger::endl;
      UInt_t uNbClosedEpochs = 0;
      Bool_t bOneEpochPrinted = kFALSE;
      auto itBuffer = fmsOrderedEpochsData.begin();
      for( auto itFlag = fmsOrderedEpochsBuffStat.begin();  itFlag!=fmsOrderedEpochsBuffStat.end(); ++itFlag )
      {
         if( kTRUE == itFlag->second )
         {
            uNbClosedEpochs++;
            LOG(DEBUG)<<" CbmTSUnpackGet4v1x::FinishUnpack => Epochs: "<<uNbClosedEpochs
                     <<" Size: "<< (itBuffer->second).size() << FairLogger::endl;


               // Use data for monitoring
            if( kTRUE == fbPulserMode )
            {
               // Loop on data
               for( auto itData = (itBuffer->second).begin(); itData != (itBuffer->second).end(); ++itData)
               {
                  get4v1x::Message mess = *itData;
                  uint8_t  cRocId    = mess.getRocNumber();
                  uint8_t  cChipId   = mess.getGet4V10R32ChipId();
                  uint32_t uChipFullId = cChipId + get4v1x::kuMaxGet4PerRoc*cRocId;
//                  uint8_t  cMessType = mess.getGet4V10R32MessageType();
                  UInt_t uFullChId =
                     get4v1x::kuChanPerGet4*( uChipFullId )
                     + mess.getGet4V10R32HitChan();

                  // As long as the 1st epoch with sync flag after SYNC DLM is found,
                  // consider data as not good
                  if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
                     return;

                  fhGet4ChanDataCount->Fill( uFullChId );
                  fhGet4ChanDllStatus->Fill( uFullChId, mess.getGet4V10R32HitDllFlag() );
                  fhGet4ChanTotMap->Fill(    uFullChId, mess.getGet4V10R32HitTot()
                                                        *get4v1x::kdTotBinSize/1000.0 );

                  // Save the hit info in order to fill later the pulser histos

                  // First fill time interval histos
                  if( fuPulserFee == (uFullChId/kuNbChanFee) )
                  {
                     Double_t dHitsDt = mess.CalcGet4V10R32HitTimeDiff(
                                 fvuCurrEpoch2[uChipFullId],
                                 fvuLastHitEp[ uFullChId ],
                                 fvmLastHit[   uFullChId ] );
                     if( 0 == fvuLastHitEp[uFullChId])
                        {}
                     else if( dHitsDt < 1e6 )
                        fhPulserHitDistNs->Fill( uFullChId%kuNbChanFee, dHitsDt / 1e3 );
                     else if( dHitsDt < 1e9)
                        fhPulserHitDistUs->Fill( uFullChId%kuNbChanFee, dHitsDt / 1e6 );
                     else
                        fhPulserHitDistMs->Fill( uFullChId%kuNbChanFee, dHitsDt / 1e9 );

                     if( 0 < fvuLastHitEp[uFullChId] && dHitsDt < 5e3 )
                        LOG(DEBUG) <<uFullChId<<" "<<dHitsDt<<" "
                            <<fvuLastHitEp[ uFullChId ]<<" "<<fvuCurrEpoch2[uChipFullId]<<" "
                            <<fvmLastHit[   uFullChId ].getGet4V10R32HitTot()*get4v1x::kdTotBinSize/1000.0<<" "
                            <<mess.getGet4V10R32HitTot()*get4v1x::kdTotBinSize/1000.0<<FairLogger::endl;;


                     // Fill the DNL histos
                     fhPulserFeeDnl->Fill( uFullChId%kuNbChanFee, mess.getGet4V10R32HitFt() );
                  } // if( fuPulserFee == (uFullChId/kuNbChanFee) )

                  // Epoch of Last hit message (one per GET4 chip & channel)
                  fvuLastHitEp[ uFullChId ] = itData->getExtendedEpoch();
                  // Last hit message (one per GET4 chip & channel)
                  fvmLastHit[ uFullChId ] = mess;
               } // for( auto itData = (itBuffer->second).begin(); itData != (itBuffer->second).begin(); ++itData)

               // Fill the time difference for all channels pairs in
               // the chosen Fee
               UInt_t uHistoFeeIdx = 0;
               for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
                  for( UInt_t uChanFeeB = uChanFeeA + 1; uChanFeeB < kuNbChanFee; uChanFeeB++)
                  {
                     if( ( 0xF0 <= fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeA].getSysMesType() ) &&
                         ( 0xF0 <= fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeB].getSysMesType() ) &&
                         (   fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeA ]
                           < fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeB ] + 2 ) &&
                         (   fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeA ] + 2
                           > fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeB ] ) )
                     {
                        Double_t dTimeDiff = 0.0;
                        dTimeDiff = fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeA].CalcGet4V10R32HitTimeDiff(
                              fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeA],
                              fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeB],
                              fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeB] );

                        fhTimeResFee[uHistoFeeIdx]->Fill( dTimeDiff );
                     } // if both channels have matching data
                     uHistoFeeIdx++;
                  } // for any unique pair of channel in chosen Fee

               // Fill the time difference for the chosen channel pairs
               UInt_t uHistoCombiIdx = 0;
               for( UInt_t uChanA = 0; uChanA < kuNbChanTest-1; uChanA++)
               {
                  if( ( 0xF0 <= fvmLastHit[ fuPulserChan[uChanA]   ].getSysMesType() ) &&
                      ( 0xF0 <= fvmLastHit[ fuPulserChan[uChanA+1] ].getSysMesType() ) &&
                      ( fvuLastHitEp[ fuPulserChan[uChanA]   ]
                            < fvuLastHitEp[ fuPulserChan[uChanA+1] ] + 2 ) &&
                      ( fvuLastHitEp[ fuPulserChan[uChanA]   ] + 2
                            > fvuLastHitEp[ fuPulserChan[uChanA+1] ]     ) )
                  {
                     Double_t dTimeDiff =
                           fvmLastHit[   fuPulserChan[uChanA]   ].CalcGet4V10R32HitTimeDiff(
                           fvuLastHitEp[ fuPulserChan[uChanA]   ],
                           fvuLastHitEp[ fuPulserChan[uChanA+1] ],
                           fvmLastHit[   fuPulserChan[uChanA+1] ] );
                     fhTimeResPairs[uChanA]->Fill( dTimeDiff );
                  } // // if both channels have data

                  for( UInt_t uChanB = uChanA+1; uChanB < kuNbChanComb; uChanB++)
                  {
                     if( ( 0xF0 <= fvmLastHit[ fuPulserChan[uChanA] ].getSysMesType() ) &&
                         ( 0xF0 <= fvmLastHit[ fuPulserChan[uChanB] ].getSysMesType() ) &&
                         ( fvuLastHitEp[ fuPulserChan[uChanA] ]
                               < fvuLastHitEp[ fuPulserChan[uChanB] ] + 2 ) &&
                         ( fvuLastHitEp[ fuPulserChan[uChanA] ] + 2
                               > fvuLastHitEp[ fuPulserChan[uChanB] ]     ) )
                     {
                        Double_t dTimeDiff =
                              fvmLastHit[   fuPulserChan[uChanA] ].CalcGet4V10R32HitTimeDiff(
                              fvuLastHitEp[ fuPulserChan[uChanA] ],
                              fvuLastHitEp[ fuPulserChan[uChanB] ],
                              fvmLastHit[   fuPulserChan[uChanB] ] );
                        fhTimeResCombi[uHistoCombiIdx]->Fill( dTimeDiff );
                     } // if both channels have data
                     uHistoCombiIdx++;
                  } // for( UInt_t uChanB = uChanA+1; uChanB < kuNbChanComb; uChanB++)
               } // for( UInt_t uChanA = 0; uChanA < kuNbChanTest; uChanA++)
            } // if( kTRUE == fbPulserMode )

         }
         if( kFALSE == bOneEpochPrinted && 0 < (itBuffer->second).size() )
         {
            for( auto itData = (itBuffer->second).begin(); itData != (itBuffer->second).end(); ++itData)
            {
               LOG(DEBUG)<<" Epoch: "<< itData->getExtendedEpoch() <<" ";
               itData->printDataLog();
            } // for( auto itData = (itBuffer->second).begin(); itData != (itBuffer->second).begin(); ++itData)
            bOneEpochPrinted = kTRUE;
         } // if( kFALSE == bOneEpochPrinted && 0 < (itBuffer->second).size() )
         ++itBuffer;
      } // for( auto itFlag = fmsOrderedEpochsBuffStat.begin();  itFlag!=fmsOrderedEpochsBuffStat.end(); ++itFlag )
      LOG(DEBUG)<<" CbmTSUnpackGet4v1x::FinishUnpack => Closed epochs: "<<uNbClosedEpochs<<FairLogger::endl;
      } // if( kFALSE == fvbGet4WaitFirstSync[0] )


      auto itBuffer = fmsOrderedEpochsData.begin();
      for( auto itFlag = fmsOrderedEpochsBuffStat.begin();  itFlag!=fmsOrderedEpochsBuffStat.end(); ++itFlag )
      {
         if( kTRUE == itFlag->second )
         {
            (itBuffer->second).clear();
            fmsOrderedEpochsData.erase( itBuffer );
            fmsOrderedEpochsBuffStat.erase( itFlag );
         } // if( kTRUE == itFlag->second )
         ++itBuffer;
      } // for( auto itFlag = fmsOrderedEpochsBuffStat.begin();  itFlag!=fmsOrderedEpochsBuffStat.end(); ++itFlag )
      if( 0 < fmsOrderedEpochsBuffStat.size() )
         fuLastEpBufferReady = (fmsOrderedEpochsBuffStat.begin())->first ;
         else fuLastEpBufferReady = 0;


      LOG(DEBUG)<<" ++++++++++++ Ts # "<<fulTsNb<<" Done!"<<FairLogger::endl;
   } // if( 2 == fiMode )
}

void CbmTSUnpackGet4v1x::Reset()
{
//  fGet4Raw->Clear();
//  fGet4RawSync->Clear();
}

void CbmTSUnpackGet4v1x::Finish()
{
   WriteMonitorHistograms();
   DeleteMonitorHistograms();
}

/*
void CbmTSUnpackGet4v1x::Register()
{
}
*/
/*
void CbmTSUnpackGet4v1x::Print6bytesMessage(const uint8_t* msContent_shifted)
{
   if( kTRUE == fbVerbose )
	   printf("0x%02X%02X%02X%02X%02X%02X :\t",
				msContent_shifted[0], msContent_shifted[1],
				msContent_shifted[2], msContent_shifted[3],
				msContent_shifted[4], msContent_shifted[5]);
}
*/

//************** Monitor functions *************/


void CbmTSUnpackGet4v1x::InitMonitorHistograms()
{
   fhMessageTypePerRoc = new TH2I("hMessageTypePerRoc",
         "Nb of message for each type per ROC; ROC #; Type",
         fuNbRocs, -0.5, fuNbRocs -0.5,
         16, -0.5, 15.5);
   fhMessageTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::MSG_NOP,      "NOP");
   fhMessageTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::MSG_HIT,      "HIT");
   fhMessageTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::MSG_EPOCH,    "EPOCH");
   fhMessageTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::MSG_SYNC,     "SYNC");
   fhMessageTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::MSG_AUX,      "AUX");
   fhMessageTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::MSG_EPOCH2,   "EPOCH2");
   fhMessageTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::MSG_GET4,     "GET4");
   fhMessageTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::MSG_SYS,      "SYS");
   fhMessageTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::MSG_GET4_SLC, "MSG_GET4_SLC");
   fhMessageTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::MSG_GET4_32B, "MSG_GET4_32B");
   fhMessageTypePerRoc->GetYaxis()->SetBinLabel(1 + 15, "GET4 Hack 32B");

   fhRocSyncTypePerRoc = new TH2I("hRocSyncTypePerRoc",
         "Nb of message for each SYNC type per ROC; ROC # ; SYNC Type",
         fuNbRocs, -0.5, fuNbRocs -0.5,
         get4v1x::kuMaxSync, -0.5, get4v1x::kuMaxSync - 0.5);

   fhRocAuxTypePerRoc = new TH2I("hRocAuxTypePerRoc",
         "Nb of message for each AUX type per ROC; ROC # ; AUX Type",
         fuNbRocs, -0.5, fuNbRocs -0.5,
         get4v1x::kuMaxAux, -0.5, get4v1x::kuMaxAux - 0.5);

   fhSysMessTypePerRoc = new TH2I("hSysMessTypePerRoc",
         "Nb of system message for each type per ROC; ROC # ; System Type",
         fuNbRocs, -0.5, fuNbRocs -0.5,
         16, -0.5, 15.5);
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_DAQ_START,       "DAQ START");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_DAQ_FINISH,      "DAQ FINISH");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_NX_PARITY,       "NX PARITY");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_SYNC_PARITY,     "SYNC PARITY");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_DAQ_RESUME,      "DAQ RESUME");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_FIFO_RESET,      "FIFO RESET");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_USER,            "USER");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_PCTIME,          "PCTIME");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_ADC,             "ADC");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_PACKETLOST,      "PACKET LOST");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_GET4_EVENT,      "GET4 ERROR");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_CLOSYSYNC_ERROR, "CLOSYSYNC ERROR");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + get4v1x::SYSMSG_TS156_SYNC,      "TS156 SYNC");
   fhSysMessTypePerRoc->GetYaxis()->SetBinLabel(1 + 15, "GET4 Hack 32B");

   fhGet4EpochFlags    = new TH2I("hGet4EpochFlags",
         "Number of epochs with corresponding flag set per GET4; GET4 # ;",
         fuNbGet4, -0.5, fuNbGet4 -0.5,
         4, -0.5, 3.5);
   fhGet4EpochFlags->GetYaxis()->SetBinLabel(1, "SYNC");
   fhGet4EpochFlags->GetYaxis()->SetBinLabel(2, "Ep. Missmatch");
   fhGet4EpochFlags->GetYaxis()->SetBinLabel(3, "Ep. Loss");
   fhGet4EpochFlags->GetYaxis()->SetBinLabel(4, "Data Loss");

   fhGet4EpochSyncDist = new TH2I("hGet4EpochSyncDist",
         "Distance between epochs with SYNC flag for each GET4; SYNC distance [epochs]; Epochs",
         fuNbGet4, -0.5, fuNbGet4 -0.5,
         2*get4v1x::kuSyncCycleSzGet4, -0.5, 2*get4v1x::kuSyncCycleSzGet4 -0.5);

   fhGet4EpochJumps    = new TH2I("hGet4EpochJumps",
         "Distance between epochs when jump happens for each GET4; Epoch distance [epochs]; Epochs",
         fuNbGet4, -0.5, fuNbGet4 -0.5,
         401, -200.5, 200.5);

   fhGet4ChanDataCount = new TH1I("hGet4ChanDataCount",
         "Data Messages per GET4 channel; GET4 channel # ; Data Count",
         fuNbGet4*get4v1x::kuChanPerGet4, -0.5, fuNbGet4*get4v1x::kuChanPerGet4 -0.5);

   fhGet4ChanDllStatus = new TH2I("hGet4ChanDllStatus",
         "DLL flag status per GET4 channel; GET4 channel # ; DLL Flag value Count",
         fuNbGet4*get4v1x::kuChanPerGet4 , -0.5, fuNbGet4*get4v1x::kuChanPerGet4 -0.5,
         2, -0.5, 1.5);

   fhGet4ChanTotMap    = new TH2I("hGet4ChanTotMap",
         "TOT values per GET4 channel; GET4 channel # ; TOT value [ns]",
         fuNbGet4*get4v1x::kuChanPerGet4, -0.5, fuNbGet4*get4v1x::kuChanPerGet4 -0.5,
         50, 0.0, 50.0);

   fhGet4ChanErrors    = new TH2I("hGet4ChanErrors",
         "Error messages per GET4 channel; GET4 channel # ; Error",
         fuNbGet4*get4v1x::kuChanPerGet4*2, -0.5, fuNbGet4*get4v1x::kuChanPerGet4 -0.5,
         32, -0.5, 31.5);
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(1,  "0x00: Readout Init    ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(2,  "0x01: Sync            ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(3,  "0x02: Epoch count sync");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(4,  "0x03: Epoch           ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(5,  "0x04: FIFO Write      ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(6,  "0x05: Lost event      ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(7,  "0x06: Channel state   ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(8,  "0x07: Token Ring state");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(9,  "0x08: Token           ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(10, "0x09: Error Readout   ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(11, "0x0a: SPI             ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(12, "0x0b: DLL Lock error  "); // <- From GET4 v1.2
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(13, "0x0c: DLL Reset invoc."); // <- From GET4 v1.2
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(14, "0x11: Overwrite       ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(15, "0x12: ToT out of range");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(16, "0x13: Event Discarded ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(17, "0x7f: Unknown         ");
   fhGet4ChanErrors->GetYaxis()->SetBinLabel(18, "Corrupt error or unsupported yet");

   fhGet4ChanSlowContM = new TH2I("hGet4ChanSlowContM",
         "Slow control messages per GET4 channel; GET4 channel # ; Type",
         fuNbGet4*get4v1x::kuChanPerGet4*2, -0.5, fuNbGet4*get4v1x::kuChanPerGet4 -0.5,
         4, -0.5, 3.5);
   fhGet4ChanSlowContM->GetYaxis()->SetBinLabel(1,  "0: Scaler event            ");
   fhGet4ChanSlowContM->GetYaxis()->SetBinLabel(2,  "1: Dead time counter event ");
   fhGet4ChanSlowContM->GetYaxis()->SetBinLabel(3,  "2: SPI receiver data        ");
   fhGet4ChanSlowContM->GetYaxis()->SetBinLabel(4,  "3: Start message/Hamming Er.");


   if( kTRUE == fbPulserMode )
   {
      // Full Fee test
      UInt_t uHistoFeeIdx =   0;
      UInt_t uNbBinsDt    = 500;
      Double_t dMinDt     = -1.*(uNbBinsDt*get4v1x::kdBinSize/2.) -get4v1x::kdBinSize/2.;
      Double_t dMaxDt     =  1.*(uNbBinsDt*get4v1x::kdBinSize/2.) +get4v1x::kdBinSize/2.;
      uNbBinsDt ++; // To account for extra bin due to shift by 1/2 bin of both ranges
      for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
         for( UInt_t uChanFeeB = uChanFeeA + 1; uChanFeeB < kuNbChanFee; uChanFeeB++)
         {
            fhTimeResFee[uHistoFeeIdx] = new TH1I(
                  Form("hTimeResFee_%03u_%03u", uChanFeeA, uChanFeeB),
                  Form("Time difference for channels %03u an %03u in chosen Fee; DeltaT [ps]; Counts",
                        uChanFeeA, uChanFeeB),
                  uNbBinsDt, dMinDt, dMaxDt);
            uHistoFeeIdx++;
         } // for any unique pair of channel in chosen Fee
      fhTimeResAllFee = new TH2D( "hTimeResAllFee",
            "Time resolution for any channels pair in chosen Fee; Ch A; Ch B",
            kuNbChanFee - 1, -0.5, kuNbChanFee - 1.5,
            kuNbChanFee - 1,  0.5, kuNbChanFee - 0.5);

      // Chosen channels test
      UInt_t uHistoCombiIdx = 0;
      for( UInt_t uChanA = 0; uChanA < kuNbChanTest-1; uChanA++)
      {
         fhTimeResPairs[uChanA]  = new TH1I(
               Form("hTimeResPairs_%03u_%03u", fuPulserChan[uChanA], fuPulserChan[uChanA+1]),
               Form("Time difference for selected channels %03u an %03u; DeltaT [ps]; Counts",
                     fuPulserChan[uChanA], fuPulserChan[uChanA+1]),
               uNbBinsDt, dMinDt, dMaxDt);
         for( UInt_t uChanB = uChanA+1; uChanB < kuNbChanComb; uChanB++)
         {
            fhTimeResCombi[uHistoCombiIdx]  = new TH1I(
               Form("hTimeResCombi_%03u_%03u", fuPulserChan[uChanA], fuPulserChan[uChanB]),
               Form("Time difference for selected channels %03u an %03u; DeltaT [ps]; Counts",
                     fuPulserChan[uChanA], fuPulserChan[uChanB]),
               uNbBinsDt, dMinDt, dMaxDt);
            uHistoCombiIdx++;
         } // for( UInt_t uChanB = uChanA+1; uChanB < kuNbChanComb; uChanB++)
      } // for( UInt_t uChanA = 0; uChanA < kuNbChanTest; uChanA++)

      // Prepare the vector storing the hit data for time diff calculation
      fvuLastHitEp.resize( fuNbGet4 * get4v1x::kuChanPerGet4);
      fvmLastHit.resize(   fuNbGet4 * get4v1x::kuChanPerGet4);
      if( kTRUE == fbOldReadoutOk )
      {
         fvuLastOldTotEp.resize(   fuNbGet4 * get4v1x::kuChanPerGet4);
         fvmLastOldTot.resize(   fuNbGet4 * get4v1x::kuChanPerGet4);
      } // if( kTRUE == fbOldReadoutOk )

      // Now clear the hits
      for( UInt_t uChan = 0; uChan < fuNbGet4 * get4v1x::kuChanPerGet4; uChan++)
      {
         fvuLastHitEp[uChan] = 0;
         fvmLastHit[uChan].reset();
         if( kTRUE == fbOldReadoutOk )
         {
            fvuLastOldTotEp[uChan] = 0;
            fvmLastOldTot[uChan].reset();
         } // if( kTRUE == fbOldReadoutOk )
      } // for( UInt_t uChan = 0; uChan < fuNbGet4 * get4v1x::kuChanPerGet4; uChan++)

      fhPulserHitDistNs = new TH2D( "hPulserHitDistNs",
            "Time Interval between hits for all channels in chosen Fee; Chan # ; Hits interval [ns]",
            kuNbChanFee, -0.5, kuNbChanFee - 0.5,
            1000 ,  0., 1000.0);
      fhPulserHitDistUs = new TH2D( "hPulserHitDistUs",
            "Time Interval between hits for all channels in chosen Fee; Chan # ; Hits interval [us]",
            kuNbChanFee, -0.5, kuNbChanFee - 0.5,
            1000 ,  0., 1000.0);
      fhPulserHitDistMs = new TH2D( "hPulserHitDistMs",
            "Time Interval between hits for all channels in chosen Fee; Chan # ; Hits interval [ms]",
            kuNbChanFee, -0.5, kuNbChanFee - 0.5,
            1000 ,  0., 1000.0);

      fhPulserFeeDnl = new TH2D( "hPulserFeeDnl",
            "DNL for all channels in chosen FEE board; Chan # ; FT Bin; DNL [bin]",
            kuNbChanFee, -0.5, kuNbChanFee - 0.5,
            get4v1x::kuFineTime+1 ,  -0.5, get4v1x::kuFineTime + 0.5);

      fhPulserFeeInl = new TH2D( "hPulserFeeInl",
            "INL for all channels in chosen FEE board; Chan # ; FT Bin; INL [bin]",
            kuNbChanFee, -0.5, kuNbChanFee - 0.5,
            get4v1x::kuFineTime+1 ,  -0.5, get4v1x::kuFineTime + 0.5);

      if( kTRUE == fbOldReadoutOk )
      {
         fhGet4ChanTotCount = new TH1I("hGet4ChanTotCount",
               "Data Messages per GET4 TOT channel; GET4 channel # ; Data Count",
               fuNbGet4*get4v1x::kuChanPerGet4, -0.5, fuNbGet4*get4v1x::kuChanPerGet4 -0.5);

         fhPulserFeeTotDnl = new TH2D( "fhPulserFeeTotDnl",
               "DNL for all TOT channels in chosen FEE board; Chan # ; FT Bin; DNL [bin]",
               kuNbChanFee, -0.5, kuNbChanFee - 0.5,
               get4v1x::kuFineTime+1 ,  -0.5, get4v1x::kuFineTime + 0.5);

         fhPulserFeeTotInl = new TH2D( "hPulserFeeTotInl",
               "INL for all TOT channels in chosen FEE board; Chan # ; FT Bin; INL [bin]",
               kuNbChanFee, -0.5, kuNbChanFee - 0.5,
               get4v1x::kuFineTime+1 ,  -0.5, get4v1x::kuFineTime + 0.5);
      } // if( kTRUE == fbOldReadoutOk )
   } // if( kTRUE == fbPulserMode )
}
void CbmTSUnpackGet4v1x::FillMonitorHistograms()
{
}
void CbmTSUnpackGet4v1x::WriteMonitorHistograms()
{
   TDirectory * oldir = gDirectory;
   TFile *fHist;
   fHist = new TFile("./get4Monitor.hst.root","RECREATE");

   fHist->cd();

   fhMessageTypePerRoc->Write();
   fhRocSyncTypePerRoc->Write();
   fhRocAuxTypePerRoc ->Write();
   fhSysMessTypePerRoc->Write();
   fhGet4EpochFlags   ->Write();
   fhGet4EpochSyncDist->Write();
   fhGet4EpochJumps   ->Write();
   fhGet4ChanDataCount->Write();
   fhGet4ChanDllStatus->Write();
   fhGet4ChanTotMap   ->Write();
   fhGet4ChanErrors   ->Write();
   fhGet4ChanSlowContM->Write();

   if( kTRUE == fbPulserMode )
   {
      // First make a gauss fit to obtain the time resolution data
      // for all Fee channels pairs
      UInt_t uHistoFeeIdx = 0;
      TF1 *fitFunc[kuNbChanFee*(kuNbChanFee-1)/2];
      for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
         for( UInt_t uChanFeeB = uChanFeeA + 1; uChanFeeB < kuNbChanFee; uChanFeeB++)
         {
            Double_t dRes = 0.0;

            // No need to fit if not data in histo
            if( 0 == fhTimeResFee[uHistoFeeIdx]->Integral() )
            {

               LOG(INFO)<<" FMS histo empty: "<<uHistoFeeIdx<<" "
                     <<uChanFeeA<<" "<<uChanFeeB<<FairLogger::endl;
               uHistoFeeIdx++;
               continue;
            } //  if( 0 == fhTimeResFee[uHistoFeeIdx]->Integral() )

            fitFunc[uHistoFeeIdx] = new TF1( Form("f_%03d_%03d",uChanFeeA,uChanFeeB), "gaus",
                  fhTimeResFee[uHistoFeeIdx]->GetMean() - 5*fhTimeResFee[uHistoFeeIdx]->GetRMS() ,
                  fhTimeResFee[uHistoFeeIdx]->GetMean() + 5*fhTimeResFee[uHistoFeeIdx]->GetRMS());

            fhTimeResFee[uHistoFeeIdx]->Fit( Form("f_%03d_%03d",uChanFeeA,uChanFeeB), "IQRM0");

            dRes = fitFunc[uHistoFeeIdx]->GetParameter(2);

            // If needed uncomment for debugging
            //(WARNING: this adds 1024 histos to the file!)
            fhTimeResFee[uHistoFeeIdx]->Write();

            delete fitFunc[uHistoFeeIdx];

            fhTimeResAllFee->Fill(uChanFeeA, uChanFeeB, dRes/TMath::Sqrt2() );

            uHistoFeeIdx++;
         } // for any unique pair of channel in chosen Fee

      // Then write the Fee summary histo
      fhTimeResAllFee->Write();

      // Write the histos for the test on chosen channel pairs
      UInt_t uHistoCombiIdx = 0;
      for( UInt_t uChanA = 0; uChanA < kuNbChanTest-1; uChanA++)
      {
         fhTimeResPairs[uChanA]->Write();
         for( UInt_t uChanB = uChanA+1; uChanB < kuNbChanComb; uChanB++)
         {
            fhTimeResCombi[uHistoCombiIdx]->Write();
            uHistoCombiIdx++;
         } // for( UInt_t uChanB = uChanA+1; uChanB < kuNbChanComb; uChanB++)
      } // for( UInt_t uChanA = 0; uChanA < kuNbChanTest; uChanA++)

      // Write the hists interval histos
      fhPulserHitDistNs->Write();
      fhPulserHitDistUs->Write();
      fhPulserHitDistMs->Write();

      // Compute the DNL from the bins occupancy
      for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
      {
         Double_t dNbCountsTotal = fhPulserFeeDnl->Integral( uChanFeeA+1, uChanFeeA+1, 1, get4v1x::kuFineTime+1);
         Double_t dNbCountsBinMean = dNbCountsTotal/(get4v1x::kuFineTime+1);
         for( UInt_t uBin = 1; uBin <= get4v1x::kuFineTime+1; uBin ++)
            fhPulserFeeDnl->SetBinContent( uChanFeeA+1, uBin,
                            fhPulserFeeDnl->GetBinContent(uChanFeeA+1, uBin)/dNbCountsBinMean );
      } // for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
      fhPulserFeeDnl->Write();
      // COmpute the INL from the DNL
      for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
      {
         Double_t dInl = 0.0;
         for( UInt_t uBin = 1; uBin <= get4v1x::kuFineTime+1; uBin ++)
         {
            dInl += 0.5 * fhPulserFeeDnl->GetBinContent(uChanFeeA+1, uBin);
            fhPulserFeeInl->SetBinContent( uChanFeeA+1, uBin, dInl - (uBin-0.5) );
            dInl += 0.5 * fhPulserFeeDnl->GetBinContent(uChanFeeA+1, uBin);
         } // for( UInt_t uBin = 1; uBin <= get4v1x::kuFineTime+1; uBin ++)
      } // for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
      fhPulserFeeInl->Write();


      if( kTRUE == fbOldReadoutOk )
      {
         fhGet4ChanTotCount->Write();

         // Compute the DNL from the bins occupancy
         for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
         {
            Double_t dNbCountsTotal = fhPulserFeeTotDnl->Integral( uChanFeeA+1, uChanFeeA+1, 1, get4v1x::kuFineTime+1);
            Double_t dNbCountsBinMean = dNbCountsTotal/(get4v1x::kuFineTime+1);
            for( UInt_t uBin = 1; uBin <= get4v1x::kuFineTime+1; uBin ++)
               fhPulserFeeTotDnl->SetBinContent( uChanFeeA+1, uBin,
                               fhPulserFeeTotDnl->GetBinContent(uChanFeeA+1, uBin)/dNbCountsBinMean );
         } // for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
         fhPulserFeeTotDnl->Write();
         // COmpute the INL from the DNL
         for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
         {
            Double_t dInl = 0.0;
            for( UInt_t uBin = 1; uBin <= get4v1x::kuFineTime+1; uBin ++)
            {
               dInl += 0.5 * fhPulserFeeTotDnl->GetBinContent(uChanFeeA+1, uBin);
               fhPulserFeeTotInl->SetBinContent( uChanFeeA+1, uBin, dInl - (uBin-0.5) );
               dInl += 0.5 * fhPulserFeeTotDnl->GetBinContent(uChanFeeA+1, uBin);
            } // for( UInt_t uBin = 1; uBin <= get4v1x::kuFineTime+1; uBin ++)
         } // for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
         fhPulserFeeTotInl->Write();
      } // if( kTRUE == fbOldReadoutOk )
   } // if( kTRUE == fbPulserMode )

   gDirectory->cd( oldir->GetPath() );
   fHist->Close();
}
void CbmTSUnpackGet4v1x::DeleteMonitorHistograms()
{

}
void CbmTSUnpackGet4v1x::MonitorMessage_epoch(  get4v1x::Message mess, uint16_t EqID)
{
   // ROC epoch message: 250 MHz clock for now
   // TODO: check compatibility when ROC clock changed to 200 MHz
   uint8_t  cRocId    = mess.getRocNumber();
   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_epoch => ROC Epoch: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;

   // Epoch counter overflow book keeping
   if( (get4v1x::kulMainEpochCycleSz - 2) < fvuCurrEpoch[cRocId] &&
       mess.getEpochNumber() < 2  )
      fvuCurrEpochCycle[cRocId]++;

   fvuCurrEpoch[cRocId] = mess.getEpochNumber();
}
void CbmTSUnpackGet4v1x::MonitorMessage_sync(   get4v1x::Message mess, uint16_t EqID)
{
   // ROC sync message: TS in 250 MHz clock for now
   // TODO: check compatibility when ROC clock changed to 200 MHz
   uint8_t  cRocId    = mess.getRocNumber();
   fhRocSyncTypePerRoc->Fill( cRocId, mess.getSyncChNum() );
   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_sync => ROC Sync: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;
}
void CbmTSUnpackGet4v1x::MonitorMessage_aux(    get4v1x::Message mess, uint16_t EqID)
{
   // ROC aux message: TS in 250 MHz clock for now
   // TODO: check compatibility when ROC clock changed to 200 MHz
   uint8_t  cRocId    = mess.getRocNumber();
   fhRocAuxTypePerRoc->Fill( cRocId, mess.getAuxChNum() );
   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_aux => ROC Aux: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;
}
void CbmTSUnpackGet4v1x::MonitorMessage_epoch2( get4v1x::Message mess, uint16_t EqID)
{
   // GET4 v1.x epoch message (24b only for now)
   // TODO: check compatibility when 32b format without hack ready
   uint8_t  cRocId    = mess.getRocNumber();
   uint8_t  cChipId   = mess.getEpoch2ChipNumber();
   uint32_t uChipFullId = cChipId + get4v1x::kuMaxGet4PerRoc*cRocId;

   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_epoch2 => GET4 Epoch2: EqId "
               <<EqID<<" roc "
               <<cRocId<<" chip "
               <<cChipId<<FairLogger::endl;

   // Reject data of chips above defined limit to avoid overflow/seg fault
   if( fuNbGet4 <= uChipFullId )
   {
      if( kTRUE == fbVerbose )
         LOG(INFO)<<"CbmTSUnpackGet4v1x::MonitorMessage_epoch2 => Ignored message with ChipId above limit!"
                  <<" ChipId: "<<cChipId
                  <<" RocId: " <<cRocId
                  <<" Limit: " <<fuNbGet4<<FairLogger::endl;
      return;
   } // if( fuNbGet4 <= cChipId + kuMaxGet4PerRoc*mess.getRocNumber() )

   // Reject data of Inactive chips
   if( kFALSE == fvbActiveChips[uChipFullId])
   {
      if( kTRUE == fbVerbose )
         LOG(INFO)<<"CbmTSUnpackGet4v1x::MonitorMessage_epoch2 => Ignored message with Inactive Chip!"
                  <<" ChipId: "<<cChipId
                  <<" RocId: " <<cRocId <<FairLogger::endl;
      return;
   } // if( kFALSE == fvbActiveChips[uChipFullId])

   // As long as the 1st epoch with sync flag after SYNC DLM is found,
   // consider data as not good
   if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
   {
      if( 1 == mess.getEpoch2Sync() )
      {
         fvbGet4WaitFirstSync[uChipFullId] = kFALSE;

         LOG(INFO) << "First SYNC Epoch after DLM in chip "
                   <<Form("%3u: ", uChipFullId)
                   <<Form(" EP #%6u", mess.getEpoch2Number() )
                   <<Form(" TS #%12llu", fulTsNb)
                   <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                   <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                   <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                   << FairLogger::endl;
      } // if( 1 == mess.getEpoch2Sync() )
         else return;
   } // if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )

   if( fvuCurrEpoch2[uChipFullId] +1 != mess.getEpoch2Number() &&
         0 != fvuCurrEpoch2[uChipFullId] )
   {
      Int_t iEpJump = mess.getEpoch2Number();
      iEpJump      -= fvuCurrEpoch2[uChipFullId];
      LOG(INFO) << "Epoch nb jump in chip "
                 <<Form("%3u: ", uChipFullId)
                 << Form(" %3d (%6u -> %6u)", iEpJump, fvuCurrEpoch2[uChipFullId],
                                         mess.getEpoch2Number() )
                 <<Form(" TS #%12llu", fulTsNb)
                 <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                 <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                 <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                 << FairLogger::endl;

      fhGet4EpochJumps->Fill(uChipFullId, iEpJump);
   } // if( fvuCurrEpoch2[uChipFullId] +1 != mess.getGet4V10R32EpochNumber())

   // Epoch counter overflow book keeping
   if( (get4v1x::kul24bGet4EpochCycleSz - 2) < fvuCurrEpoch2[uChipFullId] &&
       mess.getEpoch2Number() < 2  )
      fvuCurrEpoch2Cycle[uChipFullId]++;

   fvuCurrEpoch2[uChipFullId] = mess.getEpoch2Number();

   if( 1 == mess.getEpoch2Sync() )
   {
      fhGet4EpochFlags->Fill(uChipFullId, 0);
      fhGet4EpochSyncDist->Fill( uChipFullId, fvuCurrEpoch2[uChipFullId] % get4v1x::kuSyncCycleSzGet4 );
   } // if( 1 == mess.getEpoch2Sync() )

   if( 1 == mess.getEpoch2EpochMissmatch() )
   {
      fhGet4EpochFlags->Fill(uChipFullId, 1);
      LOG(INFO) << "Epoch missmatch in chip "
                <<Form("%3u: ", uChipFullId)
                <<Form(" EP #%6u", mess.getEpoch2Number() )
                <<Form(" TS #%12llu", fulTsNb)
                <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                << FairLogger::endl;
   } // if( 1 == mess.getEpoch2EpochMissmatch() )
   if( 1 == mess.getEpoch2EpochLost() )
   {
      fhGet4EpochFlags->Fill(uChipFullId, 2);
      LOG(INFO) << "Epoch loss in chip      "
                <<Form("%3u: ", uChipFullId)
                <<Form(" EP #%6u", mess.getEpoch2Number() )
                <<Form(" TS #%12llu", fulTsNb)
                <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                << FairLogger::endl;
   } // if( 1 == mess.getEpoch2EpochLost() )
   if( 1 == mess.getEpoch2DataLost() )
   {
      fhGet4EpochFlags->Fill(uChipFullId, 3);
      LOG(INFO) << "Data Loss in chip       "
                <<Form("%3u: ", uChipFullId)
                <<Form(" EP #%6u", mess.getEpoch2Number() )
                <<Form(" TS #%12llu", fulTsNb)
                <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                << FairLogger::endl;
   } // if( 1 == mess.getEpoch2DataLost() )

   // Fill Pulser test histos if needed
   // Accepted pairs are when both messages are defined and they are at most
   // 1 epoch apart => possibility of double use is the pulse happens on top of
   // an epoch and more than once every 3 epochs. For example:
   // HHHHEHHHH.......E......HHHHEHHHH leads to
   // (HHHHHHHH)             (HHHHHHHH) and
   //     (HHHH              HHHH)
   if( kTRUE == fbPulserMode && 0 == uChipFullId && kTRUE == fbOldReadoutOk )
   {
      // Fill the time difference for all channels pairs in
      // the chosen Fee
      UInt_t uHistoFeeIdx = 0;
      for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
         for( UInt_t uChanFeeB = uChanFeeA + 1; uChanFeeB < kuNbChanFee; uChanFeeB++)
         {
            if(
                ( get4v1x::MSG_GET4 <= fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeA].getMessageType() ) &&
                ( get4v1x::MSG_GET4 <= fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeB].getMessageType() ) &&
                (   fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeA ]
                  < fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeB ] + 2 ) &&
                (   fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeA ] + 2
                  > fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeB ] ) )
            {
               Double_t dTimeDiff = 0.0;
               dTimeDiff = fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeA].CalcGet4V10R24HitTimeDiff(
                     fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeA],
                     fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeB],
                     fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeB] );

               fhTimeResFee[uHistoFeeIdx]->Fill( dTimeDiff );
            } // if both channels have matching data
            uHistoFeeIdx++;
         } // for any unique pair of channel in chosen Fee

      // Fill the time difference for the chosen channel pairs
      UInt_t uHistoCombiIdx = 0;
      for( UInt_t uChanA = 0; uChanA < kuNbChanTest-1; uChanA++)
      {
         if( ( get4v1x::MSG_GET4 <= fvmLastHit[ fuPulserChan[uChanA]   ].getMessageType() ) &&
             ( get4v1x::MSG_GET4 <= fvmLastHit[ fuPulserChan[uChanA+1] ].getMessageType() ) &&
             ( fvuLastHitEp[ fuPulserChan[uChanA]   ]
                   < fvuLastHitEp[ fuPulserChan[uChanA+1] ] + 2 ) &&
             ( fvuLastHitEp[ fuPulserChan[uChanA]   ] + 2
                   > fvuLastHitEp[ fuPulserChan[uChanA+1] ]     ) )
         {
            Double_t dTimeDiff =
                  fvmLastHit[   fuPulserChan[uChanA]   ].CalcGet4V10R24HitTimeDiff(
                  fvuLastHitEp[ fuPulserChan[uChanA]   ],
                  fvuLastHitEp[ fuPulserChan[uChanA+1] ],
                  fvmLastHit[   fuPulserChan[uChanA+1] ] );
            fhTimeResPairs[uChanA]->Fill( dTimeDiff );
         } // // if both channels have data

         for( UInt_t uChanB = uChanA+1; uChanB < kuNbChanComb; uChanB++)
         {
            if( ( get4v1x::MSG_GET4 <= fvmLastHit[ fuPulserChan[uChanA] ].getMessageType() ) &&
                ( get4v1x::MSG_GET4 <= fvmLastHit[ fuPulserChan[uChanB] ].getMessageType() ) &&
                ( fvuLastHitEp[ fuPulserChan[uChanA] ]
                      < fvuLastHitEp[ fuPulserChan[uChanB] ] + 2 ) &&
                ( fvuLastHitEp[ fuPulserChan[uChanA] ] + 2
                      > fvuLastHitEp[ fuPulserChan[uChanB] ]     ) )
            {
               Double_t dTimeDiff =
                     fvmLastHit[   fuPulserChan[uChanA] ].CalcGet4V10R24HitTimeDiff(
                     fvuLastHitEp[ fuPulserChan[uChanA] ],
                     fvuLastHitEp[ fuPulserChan[uChanB] ],
                     fvmLastHit[   fuPulserChan[uChanB] ] );
               fhTimeResCombi[uHistoCombiIdx]->Fill( dTimeDiff );
            } // if both channels have data
            uHistoCombiIdx++;
         } // for( UInt_t uChanB = uChanA+1; uChanB < kuNbChanComb; uChanB++)
      } // for( UInt_t uChanA = 0; uChanA < kuNbChanTest; uChanA++)

   } // if( kTRUE == fbPulserMode && 0 == uChipFullId && kTRUE == fbOldReadoutOk )

}
void CbmTSUnpackGet4v1x::MonitorMessage_get4(   get4v1x::Message mess, uint16_t EqID)
{
   // GET4 v1.x data message (24b only for now)
   // TODO: check compatibility when 32b format without hack ready
   uint8_t  cRocId    = mess.getRocNumber();
   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_get4 => GET4 Data: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;

   if( kTRUE == fbPulserMode && kTRUE == fbOldReadoutOk )
   {
      uint8_t  cChipId   = mess.getGet4Number();
      UInt_t uChipFullId = cChipId + get4v1x::kuMaxGet4PerRoc*cRocId;
      UInt_t uFullChId =
            get4v1x::kuChanPerGet4*( uChipFullId )
            + mess.getGet4ChNum();
      UInt_t uTimeStamp = mess.getGet4Ts();

      // Reject data of chips above defined limit to avoid overflow/seg fault
      if( fuNbGet4 <= uChipFullId )
      {
         if( kTRUE == fbVerbose )
            LOG(INFO)<<"CbmTSUnpackGet4v1x::MonitorMessage_get4 => Ignored message with ChipId above limit!"
                     <<" ChipId: "<<cChipId
                     <<" RocId: " <<cRocId
                     <<" Limit: " <<fuNbGet4<<FairLogger::endl;
         return;
      } // if( fuNbGet4 <= cChipId + kuMaxGet4PerRoc*mess.getRocNumber() )

      // Reject data of Inactive chips
      if( kFALSE == fvbActiveChips[uChipFullId])
      {
         if( kTRUE == fbVerbose )
            LOG(INFO)<<"CbmTSUnpackGet4v1x::MonitorMessage_get4 => Ignored message with Inactive Chip!"
                     <<" ChipId: "<<cChipId
                     <<" RocId: " <<cRocId <<FairLogger::endl;
         return;
      } // if( kFALSE == fvbActiveChips[uChipFullId])

      // As long as the 1st epoch with sync flag after SYNC DLM is found,
      // consider data as not good
      if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
         return;

      if( 1 == mess.getGet4Edge() )
      {
         fhGet4ChanTotCount->Fill( uFullChId );
         Double_t dTot = mess.CalcGet4V10R24HitTimeDiff(
                           fvuCurrEpoch2[uChipFullId],
                           fvuLastHitEp[    uFullChId],
                           fvmLastHit[   uFullChId ] )/1000.0;
         if( 0.0 < dTot && dTot < 100.0 )
            fhGet4ChanTotMap->Fill(   uFullChId, dTot);

         // Save the hit info in order to fill later the pulser histos
         // First fill time interval histos
         if( fuPulserFee == (uFullChId/kuNbChanFee) )
            // Fill the DNL histos
            fhPulserFeeTotDnl->Fill( uFullChId%kuNbChanFee, uTimeStamp & get4v1x::kuFineTime );

         // Epoch of Last hit message (one per GET4 chip & channel)
         fvuLastOldTotEp[ uFullChId ] = fvuCurrEpoch2[uChipFullId];
         // Last hit message (one per GET4 chip & channel)
         fvmLastOldTot[ uFullChId ] = mess;
      } // if( 1 == mess.getGet4Edge() )
      else
         {
            fhGet4ChanDataCount->Fill( uFullChId );
            Double_t dTot = fvmLastOldTot[   uFullChId ].CalcGet4V10R24HitTimeDiff(
                              fvuLastOldTotEp[ uFullChId ],
                              fvuCurrEpoch2[uChipFullId],
                              mess )/1000.0;
            if( 0.0 < dTot && dTot < 100.0 )
               fhGet4ChanTotMap->Fill(   uFullChId, dTot);

            // Save the hit info in order to fill later the pulser histos
            // First fill time interval histos
            if( fuPulserFee == (uFullChId/kuNbChanFee) )
            {
               Double_t dHitsDt = mess.CalcGet4V10R24HitTimeDiff(
                           fvuCurrEpoch2[uChipFullId],
                           fvuLastHitEp[ uFullChId ],
                           fvmLastHit[   uFullChId ] );
               if( 0 == fvuLastHitEp[uFullChId])
                  {}
               else if( dHitsDt < 1e6 )
                  fhPulserHitDistNs->Fill( uFullChId%kuNbChanFee, dHitsDt / 1e3 );
               else if( dHitsDt < 1e9)
                  fhPulserHitDistUs->Fill( uFullChId%kuNbChanFee, dHitsDt / 1e6 );
               else
                  fhPulserHitDistMs->Fill( uFullChId%kuNbChanFee, dHitsDt / 1e9 );

               if( 0 < fvuLastHitEp[uFullChId] && dHitsDt < 5e3 )
                  LOG(DEBUG) <<uFullChId<<" "<<dHitsDt<<" "
                      <<fvuLastHitEp[ uFullChId ]<<" "<<fvuCurrEpoch2[uChipFullId]<<" "
                      <<fvmLastOldTot[   uFullChId ].CalcGet4V10R24HitTimeDiff(
                            fvuLastOldTotEp[ uFullChId ],
                            fvuLastHitEp[    uFullChId],
                            fvmLastHit[   uFullChId ] )/1000.0<<" "
                      <<fvmLastOldTot[   uFullChId ].CalcGet4V10R24HitTimeDiff(
                            fvuLastOldTotEp[ uFullChId ],
                            fvuCurrEpoch2[uChipFullId],
                            mess )/1000.0<<FairLogger::endl;;

               // Fill the DNL histos
               fhPulserFeeDnl->Fill( uFullChId%kuNbChanFee, uTimeStamp & get4v1x::kuFineTime );
            } // if( fuPulserFee == (uFullChId/kuNbChanFee) )

            // Epoch of Last hit message (one per GET4 chip & channel)
            fvuLastHitEp[ uFullChId ] = fvuCurrEpoch2[uChipFullId];
            // Last hit message (one per GET4 chip & channel)
            fvmLastHit[ uFullChId ] = mess;
         } // else of if( 1 == mess.getGet4Edge() )

   } // if( kTRUE == fbPulserMode && kTRUE == fbOldReadoutOk )
}
void CbmTSUnpackGet4v1x::MonitorMessage_sys(    get4v1x::Message mess, uint16_t EqID)
{
   uint8_t  cRocId    = mess.getRocNumber();
   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_sys => System message: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;

   if( mess.getSysMesType() < get4v1x::SYSMSG_GET4V1_32BIT_0 )
      fhSysMessTypePerRoc->Fill( cRocId, mess.getSysMesType() );

   switch( mess.getSysMesType() )
   {
      case get4v1x::SYSMSG_DAQ_START:
      {
         break;
      } // case get4v1x::SYSMSG_DAQ_START
      case get4v1x::SYSMSG_DAQ_FINISH:
      {
         break;
      } // case get4v1x::SYSMSG_DAQ_FINISH
      case get4v1x::SYSMSG_NX_PARITY:
      case get4v1x::SYSMSG_ADC:
      case get4v1x::SYSMSG_PCTIME:
      {
         // NXYTER related => don't care in this unpacker
         break;
      } // case get4v1x::SYSMSG_NX_PARITY
      case get4v1x::SYSMSG_SYNC_PARITY:
      {
         break;
      } // case get4v1x::SYSMSG_SYNC_PARITY
      case get4v1x::SYSMSG_DAQ_RESUME:
      {
         break;
      } // case get4v1x::SYSMSG_DAQ_RESUME
      case get4v1x::SYSMSG_FIFO_RESET:
      {
         break;
      } // case get4v1x::SYSMSG_FIFO_RESET
      case get4v1x::SYSMSG_USER:
      {
         if( get4v1x::SYSMSG_USER_ROCFEET_SYNC == mess.getSysMesData() )
         {
            LOG(INFO)<<"CbmTSUnpackGet4v1x::MonitorMessage_sys => SYNC DLM!"
                     <<" RocId: " << Form("%02u", cRocId)
                     <<Form(" TS  #%12llu", fulTsNb)
                     <<Form(" MS  #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                     <<Form(" OF  #%5u/%5u", fuOffset, fuMaxOffset)
                     <<Form(" LO  #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                     <<FairLogger::endl;
            fvbRocFeetSyncStart[cRocId] = kTRUE;
         } // if( get4v1x::SYSMSG_USER_ROCFEET_SYNC == mess.getSysMesData() )
         break;
      } // case get4v1x::SYSMSG_USER
      case get4v1x::SYSMSG_PACKETLOST:
      {
         break;
      } // case get4v1x::SYSMSG_PACKETLOST
      case get4v1x::SYSMSG_GET4_EVENT:
      {
         // GET4 v1.x error message (24b only for now)
         // TODO: check compatibility when 32b format without hack ready
         break;
      } // case get4v1x::SYSMSG_GET4_EVENT
      case get4v1x::SYSMSG_CLOSYSYNC_ERROR:
      {
         break;
      } // case get4v1x::SYSMSG_CLOSYSYNC_ERROR
      case get4v1x::SYSMSG_TS156_SYNC:
      {
         break;
      } // case get4v1x::SYSMSG_TS156_SYNC
      case get4v1x::SYSMSG_GET4V1_32BIT_0:
      case get4v1x::SYSMSG_GET4V1_32BIT_1:
      case get4v1x::SYSMSG_GET4V1_32BIT_2:
      case get4v1x::SYSMSG_GET4V1_32BIT_3:
      case get4v1x::SYSMSG_GET4V1_32BIT_4:
      case get4v1x::SYSMSG_GET4V1_32BIT_5:
      case get4v1x::SYSMSG_GET4V1_32BIT_6:
      case get4v1x::SYSMSG_GET4V1_32BIT_7:
      case get4v1x::SYSMSG_GET4V1_32BIT_8:
      case get4v1x::SYSMSG_GET4V1_32BIT_9:
      case get4v1x::SYSMSG_GET4V1_32BIT_10:
      case get4v1x::SYSMSG_GET4V1_32BIT_11:
      case get4v1x::SYSMSG_GET4V1_32BIT_12:
      case get4v1x::SYSMSG_GET4V1_32BIT_13:
      case get4v1x::SYSMSG_GET4V1_32BIT_14:
      case get4v1x::SYSMSG_GET4V1_32BIT_15:
      {
         // Ignore all messages before RocFeet system SYNC
         if (kFALSE == fvbRocFeetSyncStart[cRocId] )
            break;

         // GET4 v1.x 32b raw message using hack
         fhMessageTypePerRoc->Fill( cRocId, 15 );
         fhSysMessTypePerRoc->Fill( cRocId, 15 );

         MonitorMessage_Get4v1( mess, EqID);
         break;
      } // case get4v1x::SYSMSG_TS156_SYNC
      default:
      {
         if( kTRUE == fbVerbose )
            LOG(WARNING)<<"CbmTSUnpackGet4v1x::MonitorMessage_sys => Unknown sysMess type ignored: "
                        <<mess.getSysMesType()<<FairLogger::endl;
         break;
      } // default:
   } // switch( mess.getSysMesType() )
}
void CbmTSUnpackGet4v1x::MonitorMessage_Get4v1( get4v1x::Message mess, uint16_t EqID)
{
   // GET4 v1.x 32b raw message using hack
   uint8_t  cRocId    = mess.getRocNumber();
   uint8_t  cChipId   = mess.getGet4V10R32ChipId();
   uint32_t uChipFullId = cChipId + get4v1x::kuMaxGet4PerRoc*cRocId;
   uint8_t  cMessType = mess.getGet4V10R32MessageType();

   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_Get4v1 => 32b GET4 message: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;

   // Reject data of chips above defined limit to avoid overflow/seg fault
   if( fuNbGet4 <= uChipFullId )
   {
      if( kTRUE == fbVerbose )
         LOG(INFO)<<"CbmTSUnpackGet4v1x::MonitorMessage_Get4v1 => Ignored message with ChipId above limit!"
                  <<" ChipId: "<<cChipId
                  <<" RocId: " <<cRocId
                  <<" Limit: " <<fuNbGet4<<FairLogger::endl;
      return;
   } // if( fuNbGet4 <= cChipId + kuMaxGet4PerRoc*mess.getRocNumber() )

   // Reject data of Inactive chips
   if( kFALSE == fvbActiveChips[uChipFullId])
   {
      if( kTRUE == fbVerbose )
         LOG(INFO)<<"CbmTSUnpackGet4v1x::MonitorMessage_Get4v1 => Ignored message with Inactive Chip!"
                  <<" ChipId: "<<cChipId
                  <<" RocId: " <<cRocId <<FairLogger::endl;
      return;
   } // if( kFALSE == fvbActiveChips[uChipFullId])

   switch( cMessType )
   {
      case get4v1x::GET4_32B_EPOCH: // => Epoch message
      {
         // As long as the 1st epoch with sync flag after SYNC DLM is found,
         // consider data as not good
         if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
         {
            if( 1 == mess.getGet4V10R32SyncFlag() )
            {
               fvbGet4WaitFirstSync[uChipFullId] = kFALSE;

               LOG(INFO) << "First SYNC Epoch after DLM in chip "
                         <<Form("%3u: ", uChipFullId)
                         <<Form(" EP #%6u", mess.getGet4V10R32EpochNumber() )
                         <<Form(" TS #%12llu", fulTsNb)
                         <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                         <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                         <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                         << FairLogger::endl;
            } // if( 1 == mess.getGet4V10R32SyncFlag() )
               else return;
         } // if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )

         if( fvuCurrEpoch2[uChipFullId] +1 != mess.getGet4V10R32EpochNumber() &&
               0 != fvuCurrEpoch2[uChipFullId] )
         {
            Int_t iEpJump = mess.getGet4V10R32EpochNumber();
            iEpJump      -= fvuCurrEpoch2[uChipFullId];
            LOG(INFO) << "Epoch nb jump in chip "
                       <<Form("%3u: ", uChipFullId)
                       << Form(" %3d (%6u -> %6u)", iEpJump, fvuCurrEpoch2[uChipFullId],
                                                    mess.getGet4V10R32EpochNumber() )
                       <<Form(" TS  #%12llu", fulTsNb)
                       <<Form(" MS  #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                       <<Form(" OF  #%5u/%5u", fuOffset, fuMaxOffset)
                       <<Form(" LO  #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                       << FairLogger::endl;

            fhGet4EpochJumps->Fill(uChipFullId, iEpJump);
         } // if( fvuCurrEpoch2[uChipFullId] +1 != mess.getGet4V10R32EpochNumber())

         // Epoch counter overflow book keeping
         if( (get4v1x::kulGet4EpochCycleSz - 2) < fvuCurrEpoch2[uChipFullId] &&
             mess.getEpoch2Number() < 2  )
            fvuCurrEpoch2Cycle[uChipFullId]++;

         fvuCurrEpoch2[uChipFullId] = mess.getGet4V10R32EpochNumber();

         if( 1 == mess.getGet4V10R32SyncFlag() )
         {
            fhGet4EpochFlags->Fill(uChipFullId , 0);
            fhGet4EpochSyncDist->Fill( uChipFullId, fvuCurrEpoch2[uChipFullId] % get4v1x::kuSyncCycleSzGet4 );
         } // if( 1 == mess.getGet4V10R32SyncFlag() )

         // Fill Pulser test histos if needed
         // Accepted pairs are when both messages are defined and they are at most
         // 1 epoch apart => possibility of double use is the pulse happens on top of
         // an epoch and more than once every 3 epochs. For example:
         // HHHHEHHHH.......E......HHHHEHHHH leads to
         // (HHHHHHHH)             (HHHHHHHH) and
         //     (HHHH              HHHH)
         if( kTRUE == fbPulserMode && 0 == uChipFullId )
         {
            // Fill the time difference for all channels pairs in
            // the chosen Fee
            UInt_t uHistoFeeIdx = 0;
            for( UInt_t uChanFeeA = 0; uChanFeeA < kuNbChanFee; uChanFeeA++)
               for( UInt_t uChanFeeB = uChanFeeA + 1; uChanFeeB < kuNbChanFee; uChanFeeB++)
               {
                  if( ( 0xF0 <= fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeA].getSysMesType() ) &&
                      ( 0xF0 <= fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeB].getSysMesType() ) &&
                      (   fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeA ]
                        < fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeB ] + 2 ) &&
                      (   fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeA ] + 2
                        > fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeB ] ) )
                  {
                     Double_t dTimeDiff = 0.0;
                     dTimeDiff = fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeA].CalcGet4V10R32HitTimeDiff(
                           fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeA],
                           fvuLastHitEp[ fuPulserFee * kuNbChanFee+ uChanFeeB],
                           fvmLastHit[ fuPulserFee * kuNbChanFee+ uChanFeeB] );

                     fhTimeResFee[uHistoFeeIdx]->Fill( dTimeDiff );
                  } // if both channels have matching data
                  uHistoFeeIdx++;
               } // for any unique pair of channel in chosen Fee

            // Fill the time difference for the chosen channel pairs
            UInt_t uHistoCombiIdx = 0;
            for( UInt_t uChanA = 0; uChanA < kuNbChanTest-1; uChanA++)
            {
               if( ( 0xF0 <= fvmLastHit[ fuPulserChan[uChanA]   ].getSysMesType() ) &&
                   ( 0xF0 <= fvmLastHit[ fuPulserChan[uChanA+1] ].getSysMesType() ) &&
                   ( fvuLastHitEp[ fuPulserChan[uChanA]   ]
                         < fvuLastHitEp[ fuPulserChan[uChanA+1] ] + 2 ) &&
                   ( fvuLastHitEp[ fuPulserChan[uChanA]   ] + 2
                         > fvuLastHitEp[ fuPulserChan[uChanA+1] ]     ) )
               {
                  Double_t dTimeDiff =
                        fvmLastHit[   fuPulserChan[uChanA]   ].CalcGet4V10R32HitTimeDiff(
                        fvuLastHitEp[ fuPulserChan[uChanA]   ],
                        fvuLastHitEp[ fuPulserChan[uChanA+1] ],
                        fvmLastHit[   fuPulserChan[uChanA+1] ] );
                  fhTimeResPairs[uChanA]->Fill( dTimeDiff );
               } // // if both channels have data

               for( UInt_t uChanB = uChanA+1; uChanB < kuNbChanComb; uChanB++)
               {
                  if( ( 0xF0 <= fvmLastHit[ fuPulserChan[uChanA] ].getSysMesType() ) &&
                      ( 0xF0 <= fvmLastHit[ fuPulserChan[uChanB] ].getSysMesType() ) &&
                      ( fvuLastHitEp[ fuPulserChan[uChanA] ]
                            < fvuLastHitEp[ fuPulserChan[uChanB] ] + 2 ) &&
                      ( fvuLastHitEp[ fuPulserChan[uChanA] ] + 2
                            > fvuLastHitEp[ fuPulserChan[uChanB] ]     ) )
                  {
                     Double_t dTimeDiff =
                           fvmLastHit[   fuPulserChan[uChanA] ].CalcGet4V10R32HitTimeDiff(
                           fvuLastHitEp[ fuPulserChan[uChanA] ],
                           fvuLastHitEp[ fuPulserChan[uChanB] ],
                           fvmLastHit[   fuPulserChan[uChanB] ] );
                     fhTimeResCombi[uHistoCombiIdx]->Fill( dTimeDiff );
                  } // if both channels have data
                  uHistoCombiIdx++;
               } // for( UInt_t uChanB = uChanA+1; uChanB < kuNbChanComb; uChanB++)
            } // for( UInt_t uChanA = 0; uChanA < kuNbChanTest; uChanA++)

         } // if( kTRUE == fbPulserMode )
         break;
      } // case get4v1x::GET4_32B_EPOCH
      case get4v1x::GET4_32B_SLCM:  // => Slow control
      {
         Double_t dFullChId =
               get4v1x::kuChanPerGet4*( uChipFullId )
               + mess.getGet4V10R32SlChan()
               + 0.5*mess.getGet4V10R32SlEdge();

         // As long as the 1st epoch with sync flag after SYNC DLM is found,
         // consider data as not good
         if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
            return;

         fhGet4ChanSlowContM->Fill( dFullChId, mess.getGet4V10R32SlType() );
         break;
      } // case get4v1x::GET4_32B_SLCM
      case get4v1x::GET4_32B_ERROR: // => Error message
      {
         Double_t dFullChId =
               get4v1x::kuChanPerGet4*( uChipFullId )
               + mess.getGet4V10R32ErrorChan()
               + 0.5*mess.getGet4V10R32ErrorEdge();

         // As long as the 1st epoch with sync flag after SYNC DLM is found,
         // consider data as not good
         if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
            return;

         switch( mess.getGet4V10R32ErrorData() )
         {
            case get4v1x::GET4_V1X_ERR_READ_INIT:
               fhGet4ChanErrors->Fill( dFullChId, 0);
               LOG(INFO)<<"CbmTSUnpackGet4v1x::MonitorMessage_Get4v1 => Readout Init!"
                        <<" ChipId: "<<Form("%02u", cChipId)
                        <<" RocId: " <<Form("%02u", cRocId)
                        <<" TS: " <<fulTsNb<<FairLogger::endl;
               break;
            case get4v1x::GET4_V1X_ERR_SYNC:
               fhGet4ChanErrors->Fill( dFullChId, 1);
               break;
            case get4v1x::GET4_V1X_ERR_EP_CNT_SYNC:
               fhGet4ChanErrors->Fill( dFullChId, 2);
               break;
            case get4v1x::GET4_V1X_ERR_EP:
               fhGet4ChanErrors->Fill( dFullChId, 3);
               break;
            case get4v1x::GET4_V1X_ERR_FIFO_WRITE:
               fhGet4ChanErrors->Fill( dFullChId, 4);
               break;
            case get4v1x::GET4_V1X_ERR_LOST_EVT:
               fhGet4ChanErrors->Fill( dFullChId, 5);
               break;
            case get4v1x::GET4_V1X_ERR_CHAN_STATE:
               fhGet4ChanErrors->Fill( dFullChId, 6);
               break;
            case get4v1x::GET4_V1X_ERR_TOK_RING_ST:
               fhGet4ChanErrors->Fill( dFullChId, 7);
               break;
            case get4v1x::GET4_V1X_ERR_TOKEN:
               fhGet4ChanErrors->Fill( dFullChId, 8);
               break;
            case get4v1x::GET4_V1X_ERR_READOUT_ERR:
               fhGet4ChanErrors->Fill( dFullChId, 9);
               break;
            case get4v1x::GET4_V1X_ERR_SPI:
               fhGet4ChanErrors->Fill( dFullChId, 10);
               break;
            case get4v1x::GET4_V1X_ERR_DLL_LOCK:
               fhGet4ChanErrors->Fill( dFullChId, 11);
               break;
            case get4v1x::GET4_V1X_ERR_DLL_RESET:
               fhGet4ChanErrors->Fill( dFullChId, 12);
               break;
            case get4v1x::GET4_V1X_ERR_TOT_OVERWRT:
               fhGet4ChanErrors->Fill( dFullChId, 13);
               break;
            case get4v1x::GET4_V1X_ERR_TOT_RANGE:
               fhGet4ChanErrors->Fill( dFullChId, 14);
               break;
            case get4v1x::GET4_V1X_ERR_EVT_DISCARD:
               fhGet4ChanErrors->Fill( dFullChId, 15);
               break;
            case get4v1x::GET4_V1X_ERR_UNKNOWN:
               fhGet4ChanErrors->Fill( dFullChId, 16);
               break;
            default: // Corrupt error or not yet supported error
               fhGet4ChanErrors->Fill( dFullChId, 17);
               break;
         } // switch( mess.getGet4V10R32ErrorData() )
         break;
      } // case get4v1x::GET4_32B_ERROR
      case get4v1x::GET4_32B_DATA:  // => Hit Data
      {
         UInt_t uFullChId =
               get4v1x::kuChanPerGet4*( uChipFullId )
               + mess.getGet4V10R32HitChan();

         // As long as the 1st epoch with sync flag after SYNC DLM is found,
         // consider data as not good
         if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
            return;

         fhGet4ChanDataCount->Fill( uFullChId );
         fhGet4ChanDllStatus->Fill( uFullChId, mess.getGet4V10R32HitDllFlag() );
         fhGet4ChanTotMap->Fill(    uFullChId, mess.getGet4V10R32HitTot()
                                               *get4v1x::kdTotBinSize/1000.0 );

         // Save the hit info in order to fill later the pulser histos
         if( kTRUE == fbPulserMode )
         {
            // First fill time interval histos
            if( fuPulserFee == (uFullChId/kuNbChanFee) )
            {
               Double_t dHitsDt = mess.CalcGet4V10R32HitTimeDiff(
                           fvuCurrEpoch2[uChipFullId],
                           fvuLastHitEp[ uFullChId ],
                           fvmLastHit[   uFullChId ] );
               if( 0 == fvuLastHitEp[uFullChId])
                  {}
               else if( dHitsDt < 1e6 )
                  fhPulserHitDistNs->Fill( uFullChId%kuNbChanFee, dHitsDt / 1e3 );
               else if( dHitsDt < 1e9)
                  fhPulserHitDistUs->Fill( uFullChId%kuNbChanFee, dHitsDt / 1e6 );
               else
                  fhPulserHitDistMs->Fill( uFullChId%kuNbChanFee, dHitsDt / 1e9 );

               if( 0 < fvuLastHitEp[uFullChId] && dHitsDt < 5e3 )
                  LOG(DEBUG) <<uFullChId<<" "<<dHitsDt<<" "
                      <<fvuLastHitEp[ uFullChId ]<<" "<<fvuCurrEpoch2[uChipFullId]<<" "
                      <<fvmLastHit[   uFullChId ].getGet4V10R32HitTot()*get4v1x::kdTotBinSize/1000.0<<" "
                      <<mess.getGet4V10R32HitTot()*get4v1x::kdTotBinSize/1000.0<<FairLogger::endl;;


               // Fill the DNL histos
               fhPulserFeeDnl->Fill( uFullChId%kuNbChanFee, mess.getGet4V10R32HitFt() );
            } // if( fuPulserFee == (uFullChId/kuNbChanFee) )

            // Epoch of Last hit message (one per GET4 chip & channel)
            fvuLastHitEp[ uFullChId ] = fvuCurrEpoch2[uChipFullId];
            // Last hit message (one per GET4 chip & channel)
            fvmLastHit[ uFullChId ] = mess;
         } // if( kTRUE == fbPulserMode )

         break;
      } // case get4v1x::GET4_32B_DATA
   } // switch( cMessType )

}
//**********************************************/

//************ Processing functions ************/
void CbmTSUnpackGet4v1x::ProcessMessage_epoch(  get4v1x::Message mess, uint16_t EqID)
{
   // ROC epoch message: 250 MHz clock for now
   // TODO: check compatibility when ROC clock changed to 200 MHz
   uint8_t  cRocId    = mess.getRocNumber();
   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_epoch => ROC Epoch: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;

   // Epoch counter overflow book keeping
   if( (get4v1x::kulMainEpochCycleSz - 2) < fvuCurrEpoch[cRocId] &&
       mess.getEpochNumber() < 2  )
      fvuCurrEpochCycle[cRocId]++;

   fvuCurrEpoch[cRocId] = mess.getEpochNumber();
}
void CbmTSUnpackGet4v1x::ProcessMessage_sync(   get4v1x::Message mess, uint16_t EqID)
{
   // ROC sync message: TS in 250 MHz clock for now
   // TODO: check compatibility when ROC clock changed to 200 MHz
   uint8_t  cRocId    = mess.getRocNumber();
   fhRocSyncTypePerRoc->Fill( cRocId, mess.getSyncChNum() );

   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_sync => ROC Sync: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;
}
void CbmTSUnpackGet4v1x::ProcessMessage_aux(    get4v1x::Message mess, uint16_t EqID)
{
   // ROC aux message: TS in 250 MHz clock for now
   // TODO: check compatibility when ROC clock changed to 200 MHz
   uint8_t  cRocId    = mess.getRocNumber();
   fhRocSyncTypePerRoc->Fill( cRocId, mess.getSyncChNum() );

   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_aux => ROC Aux: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;
}
void CbmTSUnpackGet4v1x::ProcessMessage_epoch2( get4v1x::Message mess, uint16_t EqID)
{
   // GET4 v1.x epoch message (24b only for now)
   // TODO: check compatibility when 32b format without hack ready
   uint8_t  cRocId    = mess.getRocNumber();
   if( kTRUE == fbVerbose )
      LOG(INFO)<<"CbmTSUnpackGet4v1x::ProcessMessage_epoch2 => Found a GET4 24b message: ignore it"
               <<FairLogger::endl;

   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_epoch2 => GET4 Epoch2: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;
   /*
   // Reject data of chips above defined limit to avoid overflow/seg fault
   if( fuNbGet4 <= uChipFullId )
   {
      if( kTRUE == fbVerbose )
         LOG(DEBUG)<<"CbmTSUnpackGet4v1x::MonitorMessage_epoch2 => Ignored message with ChipId above limit!"
                  <<" ChipId: "<<cChipId
                  <<" RocId: " <<cRocId
                  <<" Limit: " <<fuNbGet4<<FairLogger::endl;
      return;
   } // if( fuNbGet4 <= cChipId + kuMaxGet4PerRoc*mess.getRocNumber() )

   // Reject data of Inactive chips
   if( kFALSE == fvbActiveChips[uChipFullId])
   {
      if( kTRUE == fbVerbose )
         LOG(DEBUG)<<"CbmTSUnpackGet4v1x::MonitorMessage_epoch2 => Ignored message with Inactive Chip!"
                  <<" ChipId: "<<cChipId
                  <<" RocId: " <<cRocId <<FairLogger::endl;
      return;
   } // if( kFALSE == fvbActiveChips[uChipFullId])

   // As long as the 1st epoch with sync flag after SYNC DLM is found,
   // consider data as not good
   if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
   {
      if( 1 == mess.getEpoch2Sync() )
      {
         fvbGet4WaitFirstSync[uChipFullId] = kFALSE;

         LOG(INFO) << "First SYNC Epoch after DLM in chip "
                   <<Form("%3u: ", uChipFullId)
                   <<Form(" EP #%6u", mess.getEpoch2Number() )
                   <<Form(" TS #%12llu", fulTsNb)
                   <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                   <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                   <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                   << FairLogger::endl;
      } // if( 1 == mess.getEpoch2Sync() )
         else return;
   } // if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )

   if( fvuCurrEpoch2[uChipFullId] +1 != mess.getEpoch2Number() &&
         0 != fvuCurrEpoch2[uChipFullId] )
   {
      Int_t iEpJump = mess.getEpoch2Number();
      iEpJump      -= fvuCurrEpoch2[uChipFullId];
      LOG(DEBUG) << "Epoch nb jump in chip "
                 <<Form("%3u: ", uChipFullId)
                 << Form(" %3d (%6u -> %6u)", iEpJump, fvuCurrEpoch2[uChipFullId],
                                         mess.getEpoch2Number() )
                 <<Form(" TS #%12llu", fulTsNb)
                 <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                 <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                 <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                 << FairLogger::endl;

      fhGet4EpochJumps->Fill(uChipFullId, iEpJump);
   } // if( fvuCurrEpoch2[uChipFullId] +1 != mess.getGet4V10R32EpochNumber())

   // Epoch counter overflow book keeping
   if( (get4v1x::kul24bGet4EpochCycleSz - 2) < fvuCurrEpoch2[uChipFullId] &&
       mess.getEpoch2Number() < 2  )
      fvuCurrEpoch2Cycle[uChipFullId]++;

   fvuCurrEpoch2[uChipFullId] = mess.getEpoch2Number();

   if( 1 == mess.getEpoch2Sync() )
   {
      fhGet4EpochFlags->Fill(uChipFullId, 0);
      fhGet4EpochSyncDist->Fill( uChipFullId, fvuCurrEpoch2[uChipFullId] % get4v1x::kuSyncCycleSzGet4 );
   } // if( 1 == mess.getEpoch2Sync() )

   if( 1 == mess.getEpoch2EpochMissmatch() )
   {
      fhGet4EpochFlags->Fill(uChipFullId, 1);
      LOG(DEBUG) << "Epoch missmatch in chip "
                <<Form("%3u: ", uChipFullId)
                <<Form(" EP #%6u", mess.getEpoch2Number() )
                <<Form(" TS #%12llu", fulTsNb)
                <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                << FairLogger::endl;
   } // if( 1 == mess.getEpoch2EpochMissmatch() )
   if( 1 == mess.getEpoch2EpochLost() )
   {
      fhGet4EpochFlags->Fill(uChipFullId, 2);
      LOG(DEBUG) << "Epoch loss in chip      "
                <<Form("%3u: ", uChipFullId)
                <<Form(" EP #%6u", mess.getEpoch2Number() )
                <<Form(" TS #%12llu", fulTsNb)
                <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                << FairLogger::endl;
   } // if( 1 == mess.getEpoch2EpochLost() )
   if( 1 == mess.getEpoch2DataLost() )
   {
      fhGet4EpochFlags->Fill(uChipFullId, 3);
      LOG(DEBUG) << "Data Loss in chip       "
                <<Form("%3u: ", uChipFullId)
                <<Form(" EP #%6u", mess.getEpoch2Number() )
                <<Form(" TS #%12llu", fulTsNb)
                <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                << FairLogger::endl;
   } // if( 1 == mess.getEpoch2DataLost() )
   */
}
void CbmTSUnpackGet4v1x::ProcessMessage_get4(   get4v1x::Message mess, uint16_t EqID)
{
   // GET4 v1.x data message (24b only for now)
   // TODO: check compatibility when 32b format without hack ready
   uint8_t  cRocId    = mess.getRocNumber();
   if( kTRUE == fbVerbose )
      LOG(DEBUG)<<"CbmTSUnpackGet4v1x::ProcessMessage_get4   => Found a GET4 24b message: ignore it"
               <<FairLogger::endl;

   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_get4 => GET4 Data: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;
}
void CbmTSUnpackGet4v1x::ProcessMessage_sys(    get4v1x::Message mess, uint16_t EqID)
{
   uint8_t  cRocId    = mess.getRocNumber();
   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::MonitorMessage_sys => System message: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;

   if( mess.getSysMesType() < get4v1x::SYSMSG_GET4V1_32BIT_0 )
      fhSysMessTypePerRoc->Fill( cRocId, mess.getSysMesType() );

   switch( mess.getSysMesType() )
   {
      case get4v1x::SYSMSG_DAQ_START:
      {
         break;
      } // case get4v1x::SYSMSG_DAQ_START
      case get4v1x::SYSMSG_DAQ_FINISH:
      {
         break;
      } // case get4v1x::SYSMSG_DAQ_FINISH
      case get4v1x::SYSMSG_NX_PARITY:
      case get4v1x::SYSMSG_ADC:
      case get4v1x::SYSMSG_PCTIME:
      {
         // NXYTER related => don't care in this unpacker
         break;
      } // case get4v1x::SYSMSG_NX_PARITY
      case get4v1x::SYSMSG_SYNC_PARITY:
      {
         break;
      } // case get4v1x::SYSMSG_SYNC_PARITY
      case get4v1x::SYSMSG_DAQ_RESUME:
      {
         break;
      } // case get4v1x::SYSMSG_DAQ_RESUME
      case get4v1x::SYSMSG_FIFO_RESET:
      {
         break;
      } // case get4v1x::SYSMSG_FIFO_RESET
      case get4v1x::SYSMSG_USER:
      {
         if( get4v1x::SYSMSG_USER_ROCFEET_SYNC == mess.getSysMesData() )
         {
            LOG(INFO)<<"CbmTSUnpackGet4v1x::MonitorMessage_sys => SYNC DLM!"
                     <<" RocId: " << Form("%02u", cRocId)
                     <<Form(" TS  #%12llu", fulTsNb)
                     <<Form(" MS  #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                     <<Form(" OF  #%5u/%5u", fuOffset, fuMaxOffset)
                     <<Form(" LO  #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                     <<FairLogger::endl;
            fvbRocFeetSyncStart[cRocId] = kTRUE;
         } // if( get4v1x::SYSMSG_USER_ROCFEET_SYNC == mess.getSysMesData() )
         break;
      } // case get4v1x::SYSMSG_USER
      case get4v1x::SYSMSG_PACKETLOST:
      {
         break;
      } // case get4v1x::SYSMSG_PACKETLOST
      case get4v1x::SYSMSG_GET4_EVENT:
      {
         // GET4 v1.x error message (24b only for now)
         // TODO: check compatibility when 32b format without hack ready
         break;
      } // case get4v1x::SYSMSG_GET4_EVENT
      case get4v1x::SYSMSG_CLOSYSYNC_ERROR:
      {
         // GET4 v1.x error message (24b only for now)
         // TODO: check compatibility when 32b format without hack ready
         break;
      } // case get4v1x::SYSMSG_CLOSYSYNC_ERROR
      case get4v1x::SYSMSG_TS156_SYNC:
      {
         break;
      } // case get4v1x::SYSMSG_TS156_SYNC
      case get4v1x::SYSMSG_GET4V1_32BIT_0:
      case get4v1x::SYSMSG_GET4V1_32BIT_1:
      case get4v1x::SYSMSG_GET4V1_32BIT_2:
      case get4v1x::SYSMSG_GET4V1_32BIT_3:
      case get4v1x::SYSMSG_GET4V1_32BIT_4:
      case get4v1x::SYSMSG_GET4V1_32BIT_5:
      case get4v1x::SYSMSG_GET4V1_32BIT_6:
      case get4v1x::SYSMSG_GET4V1_32BIT_7:
      case get4v1x::SYSMSG_GET4V1_32BIT_8:
      case get4v1x::SYSMSG_GET4V1_32BIT_9:
      case get4v1x::SYSMSG_GET4V1_32BIT_10:
      case get4v1x::SYSMSG_GET4V1_32BIT_11:
      case get4v1x::SYSMSG_GET4V1_32BIT_12:
      case get4v1x::SYSMSG_GET4V1_32BIT_13:
      case get4v1x::SYSMSG_GET4V1_32BIT_14:
      case get4v1x::SYSMSG_GET4V1_32BIT_15:
      {
         // Ignore all messages before RocFeet system SYNC
         if (kFALSE == fvbRocFeetSyncStart[cRocId] )
            break;

         // GET4 v1.x 32b raw message using hack
         fhMessageTypePerRoc->Fill( cRocId, 15 );
         fhSysMessTypePerRoc->Fill( cRocId, 15 );

         ProcessMessage_Get4v1( mess, EqID);
         break;
      } // case get4v1x::SYSMSG_TS156_SYNC
      default:
      {
         if( kTRUE == fbVerbose )
            LOG(WARNING)<<"CbmTSUnpackGet4v1x::ProcessMessage_sys => Unknown sysMess type ignored: "
                        <<mess.getSysMesType()<<FairLogger::endl;
         break;
      } // default:
   } // switch( mess.getSysMesType() )
}
void CbmTSUnpackGet4v1x::ProcessMessage_Get4v1( get4v1x::Message mess, uint16_t EqID)
{
   // GET4 v1.x 32b raw message using hack
   uint8_t  cRocId    = mess.getRocNumber();
   uint8_t  cChipId   = mess.getGet4V10R32ChipId();
   uint32_t uChipFullId = cChipId + get4v1x::kuMaxGet4PerRoc*cRocId;
   uint8_t  cMessType = mess.getGet4V10R32MessageType();

   LOG(DEBUG3)<<"CbmTSUnpackGet4v1x::ProcessMessage_Get4v1 => 32b GET4 message: EqId "
               <<EqID<<" roc "
               <<cRocId<<FairLogger::endl;

   // Reject data of chips above defined limit to avoid overflow/seg fault
   if( fuNbGet4 <= uChipFullId )
   {
      if( kTRUE == fbVerbose )
         LOG(INFO)<<"CbmTSUnpackGet4v1x::ProcessMessage_Get4v1 => Ignored message with ChipId above limit!"
                  <<" ChipId: "<<cChipId
                  <<" RocId: " <<cRocId
                  <<" Limit: " <<fuNbGet4<<FairLogger::endl;
      return;
   } // if( fuNbGet4 <= cChipId + kuMaxGet4PerRoc*mess.getRocNumber() )

   // Reject data of Inactive chips
   if( kFALSE == fvbActiveChips[uChipFullId])
   {
      if( kTRUE == fbVerbose )
         LOG(INFO)<<"CbmTSUnpackGet4v1x::ProcessMessage_Get4v1 => Ignored message with Inactive Chip!"
                  <<" ChipId: "<<cChipId
                  <<" RocId: " <<cRocId <<FairLogger::endl;
      return;
   } // if( kFALSE == fvbActiveChips[uChipFullId])

   switch( cMessType )
   {
      case get4v1x::GET4_32B_EPOCH: // => Epoch message
      {

         // As long as the 1st epoch with sync flag after SYNC DLM is found,
         // consider data as not good
         if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
         {
            if( 1 == mess.getGet4V10R32SyncFlag() )
            {
               fvbGet4WaitFirstSync[uChipFullId] = kFALSE;

               LOG(INFO) << "First SYNC Epoch after DLM in chip "
                         <<Form("%3u: ", uChipFullId)
                         <<Form(" EP #%6u", mess.getGet4V10R32EpochNumber() )
                         <<Form(" TS #%12llu", fulTsNb)
                         <<Form(" MS #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                         <<Form(" OF #%5u/%5u", fuOffset, fuMaxOffset)
                         <<Form(" LO #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                         << FairLogger::endl;
            } // if( 1 == mess.getGet4V10R32SyncFlag() )
               else return;
         } // if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )

         if( fvuCurrEpoch2[uChipFullId] +1 != mess.getGet4V10R32EpochNumber() &&
               0 != fvuCurrEpoch2[uChipFullId] )
         {
            Int_t iEpJump = mess.getGet4V10R32EpochNumber();
            iEpJump      -= fvuCurrEpoch2[uChipFullId];
            LOG(DEBUG) << "Epoch nb jump in chip "
                       <<Form("%3u: ", uChipFullId)
                       << Form(" %3d (%6u -> %6u)", iEpJump, fvuCurrEpoch2[uChipFullId],
                                                    mess.getGet4V10R32EpochNumber() )
                       <<Form(" TS  #%12llu", fulTsNb)
                       <<Form(" MS  #%5llu/%5lu", fulMsNb, fsMaxMsNb)
                       <<Form(" OF  #%5u/%5u", fuOffset, fuMaxOffset)
                       <<Form(" LO  #%5u/%5u", fuLocalOffset, fuMaxLocalOffset)
                       << FairLogger::endl;

            fhGet4EpochJumps->Fill(uChipFullId, iEpJump);
         } // if( fvuCurrEpoch2[uChipFullId] +1 != mess.getGet4V10R32EpochNumber())

         // Epoch counter overflow book keeping
         if( (get4v1x::kulGet4EpochCycleSz - 2) < fvuCurrEpoch2[uChipFullId] &&
             mess.getEpoch2Number() < 2  )
            fvuCurrEpoch2Cycle[uChipFullId]++;

         fvuCurrEpoch2[uChipFullId] = mess.getGet4V10R32EpochNumber();

         if( 1 == mess.getGet4V10R32SyncFlag() )
         {
            fhGet4EpochFlags->Fill(uChipFullId , 0);
            fhGet4EpochSyncDist->Fill( uChipFullId, fvuCurrEpoch2[uChipFullId] % get4v1x::kuSyncCycleSzGet4 );
         } // if( 1 == mess.getGet4V10R32SyncFlag() )

         // Check other active chips current epoch and close all epochs which all chips passed
         fvuCurrEpochBuffer[uChipFullId] = static_cast<ULong64_t>(fvuCurrEpoch2[uChipFullId]) +
               ( static_cast<ULong64_t>(fvuCurrEpoch2Cycle[uChipFullId]) << 24);
         ULong64_t uLowerCurrentReadyEp = fvuCurrEpochBuffer[uChipFullId];
//         LOG(DEBUG2)<<"CbmTSUnpackGet4v1x::MonitorMessage_Get4v1 => Check close "
//               << uLowerCurrentReadyEp << " => ";
         for( UInt_t uChipScan = 0; uChipScan < fuNbGet4; uChipScan++)
         {
//            LOG(DEBUG2)<<"( "<<fvbActiveChips[uChipScan]<<", "<<fvuCurrEpochBuffer[uChipScan]<<") ";
            if( kTRUE == fvbActiveChips[uChipScan] &&
                  fvuCurrEpochBuffer[uChipScan] < uLowerCurrentReadyEp )
               uLowerCurrentReadyEp = fvuCurrEpochBuffer[uChipScan];
         } // for( UInt_t uChipScan = 0; uChipScan < fuNbGet4; uChipScan++)

         //

//         LOG(INFO)<<" => ( "<<fuLastEpBufferReady<<", "<<uLowerCurrentReadyEp<<") "<<FairLogger::endl;

         // The map is ordered so we can just loop over
         if( fuLastEpBufferReady < uLowerCurrentReadyEp )
         {
//            for( std::map<ULong64_t, Bool_t>::iterator it= fmsOrderedEpochsBuffStat.find(fuLastEpBufferReady);
            // Check to avoid starting at an entry already removed in previous loops
            auto it_Start = fmsOrderedEpochsBuffStat.find(fuLastEpBufferReady);
            if( fmsOrderedEpochsBuffStat.end() == it_Start )
               it_Start = fmsOrderedEpochsBuffStat.begin();

            for( auto it= it_Start;
                 it!=fmsOrderedEpochsBuffStat.find(uLowerCurrentReadyEp);
                 ++it)
               it->second = kTRUE;
            auto it = fmsOrderedEpochsBuffStat.find(uLowerCurrentReadyEp);
            --it; // Go back to previous element
            fuLastEpBufferReady = it->first;
         }

         // Start new buffer for this epoch if it does not already exist
         if( fmsOrderedEpochsData.end() == fmsOrderedEpochsData.find(fvuCurrEpochBuffer[uChipFullId]) )
         {
            // new entry in map with empty multiset
            fmsOrderedEpochsData.insert( fmsOrderedEpochsData.end(),
                  std::pair< ULong64_t, std::multiset< get4v1x::FullMessage > >(
                        fvuCurrEpochBuffer[uChipFullId],
                        std::multiset< get4v1x::FullMessage >() ) );
            // new entry in map with kFALSE flag
            fmsOrderedEpochsBuffStat.insert( fmsOrderedEpochsBuffStat.end(),
                  std::pair<ULong64_t, Bool_t>(fvuCurrEpochBuffer[uChipFullId], kFALSE) );
         } // if( fmsOrderedEpochsData.end() == fmsOrderedEpochsData.find(fvuCurrEpochBuffer[uChipFullId]) )

         fvuCurrEpochBufferIt[uChipFullId] = fmsOrderedEpochsData.find(fvuCurrEpochBuffer[uChipFullId]);

         break;
      } // case get4v1x::GET4_32B_EPOCH
      case get4v1x::GET4_32B_SLCM:  // => Slow control
      {
         Double_t dFullChId =
               get4v1x::kuChanPerGet4*2*( uChipFullId )
               + mess.getGet4V10R32SlChan()
               + 0.5*mess.getGet4V10R32SlEdge();

         // As long as the 1st epoch with sync flag after SYNC DLM is found,
         // consider data as not good
         if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
            return;

         fhGet4ChanSlowContM->Fill( dFullChId, mess.getGet4V10R32SlType() );
         break;
      } // case get4v1x::GET4_32B_SLCM
      case get4v1x::GET4_32B_ERROR: // => Error message
      {
         Double_t dFullChId =
               get4v1x::kuChanPerGet4*2*( uChipFullId )
               + mess.getGet4V10R32ErrorChan()
               + 0.5*mess.getGet4V10R32ErrorEdge();

         // As long as the 1st epoch with sync flag after SYNC DLM is found,
         // consider data as not good
         if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
            return;

         switch( mess.getGet4V10R32ErrorData() )
         {
            case get4v1x::GET4_V1X_ERR_READ_INIT:
               fhGet4ChanErrors->Fill( dFullChId, 0);
               LOG(INFO)<<"CbmTSUnpackGet4v1x::MonitorMessage_Get4v1 => Readout Init!"
                        <<" ChipId: "<<Form("%02u", cChipId)
                        <<" RocId: " <<Form("%02u", cRocId)
                        <<" TS: " <<fulTsNb<<FairLogger::endl;
               break;
            case get4v1x::GET4_V1X_ERR_SYNC:
               fhGet4ChanErrors->Fill( dFullChId, 1);
               break;
            case get4v1x::GET4_V1X_ERR_EP_CNT_SYNC:
               fhGet4ChanErrors->Fill( dFullChId, 2);
               break;
            case get4v1x::GET4_V1X_ERR_EP:
               fhGet4ChanErrors->Fill( dFullChId, 3);
               break;
            case get4v1x::GET4_V1X_ERR_FIFO_WRITE:
               fhGet4ChanErrors->Fill( dFullChId, 4);
               break;
            case get4v1x::GET4_V1X_ERR_LOST_EVT:
               fhGet4ChanErrors->Fill( dFullChId, 5);
               break;
            case get4v1x::GET4_V1X_ERR_CHAN_STATE:
               fhGet4ChanErrors->Fill( dFullChId, 6);
               break;
            case get4v1x::GET4_V1X_ERR_TOK_RING_ST:
               fhGet4ChanErrors->Fill( dFullChId, 7);
               break;
            case get4v1x::GET4_V1X_ERR_TOKEN:
               fhGet4ChanErrors->Fill( dFullChId, 8);
               break;
            case get4v1x::GET4_V1X_ERR_READOUT_ERR:
               fhGet4ChanErrors->Fill( dFullChId, 9);
               break;
            case get4v1x::GET4_V1X_ERR_SPI:
               fhGet4ChanErrors->Fill( dFullChId, 10);
               break;
            case get4v1x::GET4_V1X_ERR_DLL_LOCK:
               fhGet4ChanErrors->Fill( dFullChId, 11);
               break;
            case get4v1x::GET4_V1X_ERR_DLL_RESET:
               fhGet4ChanErrors->Fill( dFullChId, 12);
               break;
            case get4v1x::GET4_V1X_ERR_TOT_OVERWRT:
               fhGet4ChanErrors->Fill( dFullChId, 13);
               break;
            case get4v1x::GET4_V1X_ERR_TOT_RANGE:
               fhGet4ChanErrors->Fill( dFullChId, 14);
               break;
            case get4v1x::GET4_V1X_ERR_EVT_DISCARD:
               fhGet4ChanErrors->Fill( dFullChId, 15);
               break;
            case get4v1x::GET4_V1X_ERR_UNKNOWN:
               fhGet4ChanErrors->Fill( dFullChId, 16);
               break;
            default: // Corrupt error or not yet supported error
               fhGet4ChanErrors->Fill( dFullChId, 17);
               break;
         } // switch( mess.getGet4V10R32ErrorData() )
         break;
      } // case get4v1x::GET4_32B_ERROR
      case get4v1x::GET4_32B_DATA:  // => Hit Data
      {
         UInt_t uFullChId =
               get4v1x::kuChanPerGet4*( uChipFullId )
               + mess.getGet4V10R32HitChan();

         // As long as the 1st epoch with sync flag after SYNC DLM is found,
         // consider data as not good
         if( kTRUE == fvbGet4WaitFirstSync[uChipFullId] )
            return;

         fhGet4ChanDataCount->Fill( uFullChId );
         fhGet4ChanDllStatus->Fill( uFullChId, mess.getGet4V10R32HitDllFlag() );
         fhGet4ChanTotMap->Fill(    uFullChId, mess.getGet4V10R32HitTot()
                                               *get4v1x::kdTotBinSize/1000.0 );

         // Save the message in the corresponding epoch vector
         get4v1x::FullMessage fullMess( mess, fvuCurrEpochBuffer[uChipFullId] );
//         std::map< ULong64_t, std::multiset< get4v1x::FullMessage > >::iterator it;
//         auto it = fmsOrderedEpochsData.find( fvuCurrEpochBuffer[uChipFullId] );
         auto it = fvuCurrEpochBufferIt[uChipFullId];
         (it->second).insert( (it->second).end(), fullMess );

         break;
      } // case get4v1x::GET4_32B_DATA
   } // switch( cMessType )
}
//**********************************************/

ClassImp(CbmTSUnpackGet4v1x)
