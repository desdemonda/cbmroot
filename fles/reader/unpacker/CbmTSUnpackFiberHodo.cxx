// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                      CbmTSUnpackFiberHodo                            -----
// -----                    Created 07.11.2014 by                          -----
// -----                   E. Ovcharenko, F. Uhlig                         -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
#include "CbmTSUnpackFiberHodo.h"

#include "CbmNxyterRawMessage.h"
#include "CbmFiberHodoDigi.h"
#include "CbmFiberHodoAddress.h"
#include "CbmNxyterRawMessage.h"
#include "CbmFiberHodoMapping.h"
#include "CbmDaqBuffer.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <stdint.h>

//#define VERBOSE 

struct DTM_header
{
  uint8_t packet_length;
  uint8_t packet_counter;
  uint16_t ROC_ID;
};

struct hit_data
{
  uint8_t MessageType;       // message type: typ. hit (01) or epoch (02)
  uint8_t NxNumber;          // nxyter number
  uint8_t NxLtxMsb;          // ROC TS MSBs
  uint16_t NxTs;             // timestamp value from nxyter
  uint8_t NxChNum;           // channel number
  uint16_t NxAdcValue;       // ADC value
  bool NxPileUp;             // pileup flag
  bool NxOverflow;           // overflow flag
  bool NxLastEpoch;          // last epoch flag
  
  void Dump() {
    /*
      printf("MessageType=%02X, NxNumber=%02X, NxLtxMsb=%02X, NxTs=%04X, NxChNum=%02X, NxAdcValue=%04X, NxPileUp=%d, NxOverflow=%d, NxLastEpoch=%d\n",
      MessageType, NxNumber, NxLtxMsb, NxTs, NxChNum,
      NxAdcValue, NxPileUp, NxOverflow, NxLastEpoch);
     */
  }
};



CbmTSUnpackFiberHodo::CbmTSUnpackFiberHodo()
  : CbmTSUnpack(),
    fCurrEpoch(0),
    fFiberHodoRaw(new TClonesArray("CbmNxyterRawMessage", 10)),
    fFiberHodoDigi(new TClonesArray("CbmFiberHodoDigi", 10)),
    fRawMessage(NULL),
    fDigi(NULL),
    fHodoMap(new CbmFiberHodoMapping()),
    fBuffer(CbmDaqBuffer::Instance())
{
}

CbmTSUnpackFiberHodo::~CbmTSUnpackFiberHodo()
{
}

Bool_t CbmTSUnpackFiberHodo::Init()
{
  LOG(INFO) << "Initializing" << FairLogger::endl; 
  
  FairRootManager* ioman = FairRootManager::Instance();
  if (ioman == NULL) {
    LOG(FATAL) << "No FairRootManager instance" << FairLogger::endl;
  }
  ioman->Register("FiberHodoRawMessage", "fiberhodo raw data", fFiberHodoRaw, kTRUE);
  ioman->Register("FiberHodoDigi", "fiber hodo digi", fFiberHodoDigi, kTRUE);
  
  return kTRUE;
}

Bool_t CbmTSUnpackFiberHodo::DoUnpack(const fles::Timeslice& ts, size_t component)
{
  LOG(DEBUG) << "Unpacking FiberHodo Data" << FairLogger::endl; 
  
  Int_t counter=0;   //TODO was ist das???
  
  DTM_header cur_DTM_header;
   hit_data cur_hit_data;
   
   for (size_t m = 0; m < ts.num_microslices(component); ++m) {
     // iterate over microslice
     auto msDescriptor = ts.descriptor(component, m);
     const uint8_t* msContent = reinterpret_cast<const uint8_t*>(ts.content(component, m));
     
     uint32_t offset = 16;
     uint32_t local_offset = 0;
     const uint8_t* msContent_shifted;
     int padding;
     
     // Loop throught the data of one microslice 
     while (offset < msDescriptor.size) {
       msContent_shifted = &msContent[offset];
       
       // Extract DTM header info
       cur_DTM_header.packet_length = msContent_shifted[0];
       cur_DTM_header.packet_counter = msContent_shifted[1];
       const uint16_t* ROC_ID_pointer = reinterpret_cast<const uint16_t*>(&msContent_shifted[2]);
       cur_DTM_header.ROC_ID = *ROC_ID_pointer;
       
#ifdef VERBOSE
       printf ("pack_len=0x%02X, pack_cnt=0x%02X, ROC=0x%04X\n",
               cur_DTM_header.packet_length, cur_DTM_header.packet_counter, cur_DTM_header.ROC_ID);
#endif
       
       local_offset = 4;
       while (local_offset < static_cast<uint32_t>(cur_DTM_header.packet_length*2+4)) {                     
	 
	 cur_hit_data.MessageType = (msContent_shifted[local_offset+5] >> 0) & 0x0F;   // 1 byte
	 
	 if (cur_hit_data.MessageType == 1) // hit
	   {
	     cur_hit_data.NxNumber    = (msContent_shifted[local_offset+5] >> 6) & 0x03;   // 1 byte
	     cur_hit_data.NxLtxMsb    = (msContent_shifted[local_offset+4] >> 0) & 0x07;   // 1 byte
	     
	     uint16_t topTimestampVal =((msContent_shifted[local_offset+2] >> 0) & 0x0001) * 0x2000;   // 2 bytes
	     uint16_t midTimestampVal =((msContent_shifted[local_offset+3] >> 0)         ) * 0x0020;   // 2 bytes
	     uint16_t lowTimestampVal = (msContent_shifted[local_offset+4] >> 3) & 0x001F;             // 2 bytes
	     cur_hit_data.NxTs = topTimestampVal + midTimestampVal + lowTimestampVal;      // 2 bytes
	     
	     cur_hit_data.NxChNum     = (msContent_shifted[local_offset+2] >> 1) & 0x7F;   // 1 byte
	     
	     uint16_t highADCval      =((msContent_shifted[local_offset+0] >> 0) & 0x001F) * 0x0080;   // 2 bytes
	     uint16_t lowADCval       = (msContent_shifted[local_offset+1] >> 1) & 0x007F;             // 2 bytes
	     cur_hit_data.NxAdcValue  = highADCval + lowADCval;                            // 2 bytes

	     cur_hit_data.NxPileUp    = (msContent_shifted[local_offset+0] >> 5) & 0x01;     // 1 bit
	     cur_hit_data.NxOverflow  = (msContent_shifted[local_offset+0] >> 6) & 0x01;     // 1 bit
	     cur_hit_data.NxLastEpoch = (msContent_shifted[local_offset+0] >> 7) & 0x01;     // 1 bit
	     
#ifdef VERBOSE
	     printf("%02X%02X%02X%02X%02X%02X\t", msContent_shifted[local_offset],   msContent_shifted[local_offset+1],
		    msContent_shifted[local_offset+2], msContent_shifted[local_offset+3],
		    msContent_shifted[local_offset+4], msContent_shifted[local_offset+5]);
	     cur_hit_data.Dump();
#endif
	     
             fRawMessage = new( (*fFiberHodoRaw)[counter] )
               CbmNxyterRawMessage(msDescriptor.eq_id,
				   cur_DTM_header.ROC_ID*4 + cur_hit_data.NxNumber,   //TODO check
				   cur_hit_data.NxChNum,
				   fCurrEpoch - cur_hit_data.NxLastEpoch,             // note the trick
				   cur_hit_data.NxTs,
				   cur_hit_data.NxAdcValue,
				   cur_hit_data.NxLastEpoch,
				   cur_hit_data.NxPileUp,
				   cur_hit_data.NxOverflow);
	     ++counter;
	     
	     FillHodoDigiToBuffer();
	     
	   } else if (cur_hit_data.MessageType == 2) { // epoch counter
	   
	   uint32_t topEpochByte = msContent_shifted[local_offset+1] * 0x01000000;
	   uint32_t higEpochByte = msContent_shifted[local_offset+2] * 0x00010000;
	   uint32_t midEpochByte = msContent_shifted[local_offset+3] * 0x00000100;
	   uint32_t lowEpochByte = msContent_shifted[local_offset+4];
           
	   fCurrEpoch = topEpochByte + higEpochByte + midEpochByte + lowEpochByte;
#ifdef VERBOSE
	   printf("epoch: %02X%02X%02X%02X%02X%02X:\t\t%d\n", msContent_shifted[local_offset],   msContent_shifted[local_offset+1],
		  msContent_shifted[local_offset+2], msContent_shifted[local_offset+3],
		  msContent_shifted[local_offset+4], msContent_shifted[local_offset+5],
		  fCurrEpoch);
#endif
	   
	 }
	 
	 local_offset += 6; // next hit or epoch marker
       }
       
#ifdef VERBOSE
       printf ("\n");
#endif
       
       // add 4 - DTM header size
       offset += static_cast<uint32_t>(cur_DTM_header.packet_length * 2+4);
       // shift some more bytes to fit the CbmNet package size
       if ((padding = offset % 8) > 0) offset += (8-padding);
       
     }
     
#ifdef VERBOSE
     printf ("\n");
#endif
     
   }
   
   return kTRUE;
}

void CbmTSUnpackFiberHodo::FillHodoDigiToBuffer() {
  
  // --- Get absolute time, NXYTER and channel number
  Int_t eqId     = fRawMessage->GetEquipmentID();
  ULong_t hitTime  = fRawMessage->GetFullTime();
  Int_t febId   = fRawMessage->GetSourceAddress();
  Int_t channelId  = fRawMessage->GetChannelID();
  Int_t charge     = fRawMessage->GetADCvalue();
  
  Int_t iStation;
  Int_t iSector;
  Int_t iPlane;
  Int_t iFiber;

  fHodoMap->Map(eqId, febId, channelId, iStation, iSector, iPlane, iFiber);
  Int_t address = CbmFiberHodoAddress::GetAddress(iStation, iPlane, iFiber);


  fDigi = new( (*fFiberHodoDigi)[fFiberHodoDigi->GetEntriesFast()] )
    CbmFiberHodoDigi(address, charge, hitTime);

  // --- Create a HODO digi and send it to the buffer
    //  CbmFiberHodoDigi* digi = new CbmFiberHodoDigi(address, charge, hitTime);

  fBuffer->InsertData(fDigi);
  
}

void CbmTSUnpackFiberHodo::Reset()
{
  fFiberHodoRaw->Clear();
}

/*
void CbmTSUnpackFiberHodo::Register()
{
}
*/


ClassImp(CbmTSUnpackFiberHodo)
