#include "CbmTSUnpackStsxyter.h"
#include "CbmStsxyterRawHitMessage.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include <iostream>
#include "MicrosliceContents.hpp"


#define VERBOSE



CbmTSUnpackStsxyter::CbmTSUnpackStsxyter()
: CbmTSUnpack(),
  fDtmNum( 0 ),
  fDtmNumInitialized( kFALSE ),
  fCurrEpoch(),
  fStsxyterRawHitMessage(new TClonesArray("CbmStsxyterRawHitMessage", 10))
{
    fCurrEpoch.resize( fkgNumStsxytersDefault );
    for( size_t iChip = 0; iChip < fCurrEpoch.size(); ++iChip ) {
        fCurrEpoch.at( iChip).resize( fkgNumStsxyterLinks );
    }
}

Bool_t CbmTSUnpackStsxyter::fgParityTableinitialized = kFALSE;
std::vector< Bool_t >  CbmTSUnpackStsxyter::fgParityTable;

CbmTSUnpackStsxyter::~CbmTSUnpackStsxyter()
{
}



Bool_t CbmTSUnpackStsxyter::Init()
{
    LOG(INFO) << "Initializing" << FairLogger::endl;

    if( ! fgParityTableinitialized ) {
        InitializeParityTable();
    }

    FairRootManager* ioman = FairRootManager::Instance();
    if (ioman == NULL) {
        LOG(FATAL) << "No FairRootManager instance" << FairLogger::endl;
    }
    ioman->Register("StsxyterRawHit", "STS-XYTER raw hit data", fStsxyterRawHitMessage, kTRUE);
    fDtmNum = 0;
    fDtmNumInitialized = kFALSE;

    return kTRUE;
}



Bool_t CbmTSUnpackStsxyter::DoUnpack(const fles::Timeslice& timeslice, size_t component)
{
    for( size_t iMicroslice = 0; iMicroslice < timeslice.num_microslices(component); ++iMicroslice )
    {
        // LOG(INFO) << "New microslice" << FairLogger::endl;

        // size_t microsliceSize = timeslice.descriptor( component, iMicroslice ).size;
        // const uint16_t * microsliceContentPtr = ( const uint16_t * ) timeslice.content( component, iMicroslice );
        // flib_dpb::MicrosliceContents microslice( microsliceContentPtr, microsliceSize );
        // auto dtms = microslice.dtms();
        // for( size_t iDtm = 0; iDtm < dtms.size(); ++iDtm ) {
        //     flib_dpb::DTM currDtm = dtms.at( iDtm );
        //     UnpackDtm( (const uint8_t * ) currDtm.data, currDtm.size );
        // }

        auto microsliceDescriptor = timeslice.descriptor( component, iMicroslice );
        // const uint8_t* microsliceContent = timeslice.content( component, iMicroslice );
        const uint8_t * const microsliceBegin = timeslice.content( component, iMicroslice );
        const uint8_t * const microsliceEnd = microsliceBegin + microsliceDescriptor.size;

        // printf("Microslice byte dump: \n");
        // ByteDumpSwap( microsliceBegin, microsliceDescriptor.size );

        const uint8_t * currDtm = GetFirstDtm( microsliceBegin );
        while( currDtm < microsliceEnd ) {
            Bool_t dtmSeqNumError = CheckDtmNumSequence( currDtm, (iMicroslice == 0) );
            if( dtmSeqNumError ) {
                printf("Microslice number %lu byte dump:\n", iMicroslice );
                ByteDumpSwap( microsliceBegin, microsliceDescriptor.size );
                printf("Error at dtm at 0x%04x\n", currDtm - microsliceBegin );
            }
            UnpackDtmContent( GetDtmContent( currDtm ), GetDtmContentSize( currDtm ), microsliceDescriptor.eq_id );
            currDtm += GetDtmFullSize( currDtm );
        }
    }
    return kTRUE;
}



const uint8_t * CbmTSUnpackStsxyter::GetFirstDtm( const uint8_t * const & microsliceBegin ) const
{
    const size_t kMicrosliceCopyOfHalfOfDescriptorSize = 16;
    const uint8_t * dtmBegin = microsliceBegin + kMicrosliceCopyOfHalfOfDescriptorSize;
    return dtmBegin;
}



Bool_t CbmTSUnpackStsxyter::CheckDtmNumSequence( const uint8_t * const & dtm, const Bool_t & newTimeslice )
{
    if( newTimeslice ) {
        fDtmNumInitialized = kFALSE;
    }

    uint8_t dtmNumCurr = dtm[ fkgDtmNumOffset ];
    uint8_t dtmNumExpected = fDtmNum < 255 ? fDtmNum + 1 : 0;
    if( fDtmNumInitialized ) {
        if( dtmNumCurr - dtmNumExpected != 0 ) {
            LOG(ERROR) << "Wrong DTM number! Previous DTM had number " << static_cast< size_t >( fDtmNum ) << ". Current DTM had number " << static_cast< size_t >( dtmNumCurr ) << " expected " << static_cast< int >( dtmNumExpected ) << FairLogger::endl;
            fDtmNum = dtmNumCurr;
            return kTRUE;
        }
    }
    fDtmNum = dtmNumCurr;
    fDtmNumInitialized = kTRUE;
    return kFALSE;
}



const uint8_t * CbmTSUnpackStsxyter::GetDtmContent( const uint8_t * const & dtm ) const
{
    return dtm + fkgDtmDataOffset;
}



size_t CbmTSUnpackStsxyter::GetDtmContentSize( const uint8_t * const & dtm ) const
{
    const size_t dtmSizeWordsMinusOne = dtm[ fkgDtmSizeOffset ];
    const size_t kDtmWordSizeBytes = 2;
    const size_t dtmSizeBytes = ( dtmSizeWordsMinusOne + 1 ) * kDtmWordSizeBytes;
    return dtmSizeBytes;
}



size_t CbmTSUnpackStsxyter::GetDtmFullSize( const uint8_t * const & dtm ) const
{
    const size_t dtmSizeFieldSize = 1;
    const size_t dtmNumFieldSize = 1;
    const size_t dtmSizeWithoutPadding = dtmSizeFieldSize + dtmNumFieldSize + GetDtmContentSize( dtm );
    const size_t kDtmSizeMultipleOfBytes = 8;
    const size_t unpaddedRemainderSize = dtmSizeWithoutPadding % kDtmSizeMultipleOfBytes;
    const size_t paddingSize = unpaddedRemainderSize > 0 ? kDtmSizeMultipleOfBytes - unpaddedRemainderSize : 0;
    const size_t dtmSizeWithPadding = dtmSizeWithoutPadding + paddingSize;
    return dtmSizeWithPadding;
}



Bool_t CbmTSUnpackStsxyter::IsStsxyterDtm( const uint8_t * const & dtmContent, const size_t & size ) const
{
    // printf("Dtm contents:\n");
    // ByteDump( dtmContent, size );

    if( size % 2 ) {
        LOG(ERROR) << "Size of the DTM payload is not even number of bytes" << FairLogger::endl;
        return kFALSE;
    }

    const uint16_t * const wordData = reinterpret_cast< const uint16_t * const >( dtmContent );
    const size_t wordSize = size / 2;

    const size_t kActualStsyxterSourceAddressOffsetWords = 1;
    const size_t kStsxyterSourceAddress = 0x0000;
    const size_t kTrailingJunkWordReverseOffsetWords = 1;
    const size_t kTrailingJunkWordMastercopyReverseOffsetWords = 2;

    // printf("SrcAddr: %04x\n", wordData[ kActualStsyxterSourceAddressOffsetWords ] );
    // printf("LastWrd: %04x\n", wordData[ wordSize - kTrailingJunkWordReverseOffsetWords ] );
    // printf("BLstWrd: %04x\n", wordData[ wordSize - kTrailingJunkWordMastercopyReverseOffsetWords ]);

    if( wordData[ kActualStsyxterSourceAddressOffsetWords ] == kStsxyterSourceAddress
            && wordData[ wordSize - kTrailingJunkWordReverseOffsetWords ] == wordData[ wordSize - kTrailingJunkWordMastercopyReverseOffsetWords ] ) {
        return kTRUE;
    }
    return kFALSE;
}



Bool_t CbmTSUnpackStsxyter::IsStsxyterDtmUnstrict( const uint8_t * const & dtmContent, const size_t & size ) const
{

    if( size % 2 ) {
        LOG(ERROR) << "Size of the DTM payload is not even number of bytes" << FairLogger::endl;
        return kFALSE;
    }

    const uint16_t * const wordData = reinterpret_cast< const uint16_t * const >( dtmContent );

    const size_t kActualStsyxterSourceAddressOffsetWords = 1;
    const size_t kStsxyterSourceAddress = 0x0000;

    if( wordData[ kActualStsyxterSourceAddressOffsetWords ] == kStsxyterSourceAddress ) {
        return kTRUE;
    }
    return kFALSE;
}



void CbmTSUnpackStsxyter::ByteDump( const uint8_t * const & data, size_t sizeBytes, size_t lineLenBytes ) const
{
    for( size_t iByte = 0; iByte < sizeBytes; iByte += 1 ) {
        printf( "%02x ", data[ iByte ] );
        if( lineLenBytes > 0 ) {
            if( iByte % lineLenBytes == lineLenBytes - 1 ) printf("\n");
        }
    }
    if( lineLenBytes > 0 ) {
        if( sizeBytes % lineLenBytes != 0 ) printf("\n");
    }
}



void CbmTSUnpackStsxyter::ByteDumpSwap( const uint8_t * const & data, size_t sizeBytes, size_t lineLenBytes ) const
{
    for( size_t iByte = 0; iByte < sizeBytes; iByte += 1 ) {
        size_t printByte = iByte % 2 ? iByte - 1 : iByte + 1;
        printf( "%02x ", data[ printByte ] );
        if( lineLenBytes > 0 ) {
            if( iByte % lineLenBytes == lineLenBytes - 1 ) printf("\n");
        }
    }
    if( lineLenBytes > 0 ) {
        if( sizeBytes % lineLenBytes != 0 ) printf("\n");
    }
}



void CbmTSUnpackStsxyter::UnpackDtmContent( const uint8_t * const & dtmContent, const size_t & size, const uint16_t & equipmentId )
{
    // printf("Processing DTM:\n");
    // ByteDumpSwap( dtmContent, size );
    // if( IsStsxyterDtm( dtmContent, size ) ) {
    if( IsStsxyterDtmUnstrict( dtmContent, size ) ) {
        UnpackStsxyterDtmContent( dtmContent, size, equipmentId );
        return;
    }
    LOG(ERROR) << "In CbmTSUnpackStsxyter::UnpackDtmContent: Unidentified Dtm\n";
}



void CbmTSUnpackStsxyter::UnpackStsxyterDtmContent( const uint8_t * const & dtmContent, const size_t & size, const uint16_t & equipmentId )
{
    const size_t kFirstMsgOffset = 4;
    const size_t kLeadingJunkWordSize = 2;
    const size_t kAddressSize = 2;
    const size_t kTrailingJunkWordSize = 2;
    const size_t kOverheadSize = kLeadingJunkWordSize + kAddressSize + kTrailingJunkWordSize;
    const size_t payloadSize = size - kOverheadSize;
    const size_t kMsgSize = 4;
    if( payloadSize % kMsgSize > 0 ) {
        LOG(ERROR) << "Looks like the payload size in STS-XYTER DTM is not a multiple of STS-XYTER messages (" << kMsgSize << " bytes). Either the data is corrupted, or DTM source is not properly identified." << FairLogger::endl;
    }
    const size_t nMsgs = payloadSize / kMsgSize;

    const uint8_t * currMsg = dtmContent + kFirstMsgOffset;
    for( size_t iMsg = 0; iMsg < nMsgs; iMsg += 1 ) {
        const size_t kSourceAddressInStsxyterDtmContentsAcutalOffsetWords = 1;
        UnpackStsxyterMessage( GetUint32( currMsg ), reinterpret_cast< const uint16_t * >( dtmContent)[kSourceAddressInStsxyterDtmContentsAcutalOffsetWords], equipmentId );
        currMsg += kMsgSize;
    }


/*
    if( GetField( msg, kEpochSignatureFirstBit, kEpochSignatureNBits ) == kEpochSignatureValue ) {
        fMsgType = MsgEpo;
        UnpackXEpo();
        return;
    }
    if( GetField( msg, kHitSignatureFirstBit, kHitSignatureNBits ) == kHitSignatureValue ) {
        fMsgType = MsgHit;
        UnpackXHit();
        return;
    }
    printf("Error: neither hit, nor epoch message from stsxyter!\n");

    // printf("getField( Epoch ) = %d\n", getField( msg, kEpochSignatureFirstBit, kEpochSignatureNBits ) );
    // printf("getField( Hit   ) = %d\n", getField( msg, kHitSignatureFirstBit, kHitSignatureNBits ) );
    // printf("kEpochSignatureFirstBit = %d\n", kEpochSignatureFirstBit );
    // printf("kHitSignatureFirstBit = %d\n",   kHitSignatureFirstBit );
    // printf("fStsxMsgRaw = %p\n", fMsgRaw );
    // printf("fPktRaw     = %p\n", fPktRaw );
    // for( uint8_t iByte = 0; iByte < 4; ++iByte ) {
    //     for( int iBit = 7; iBit >=0 ; --iBit ) {
    //         printf("%d", getField( msg, iBit + 8 * iByte, 1 ) );
    //     }
    //     printf(" ");
    // }

    printf("Nearest fragment of current usb packet:\n");
    PrintUsbpktRaw( 50 );
    PrintUsbpktPos();
    printf("Current cbmnet packet in hex:\n");
    PrintCbmnetPktRawHex();
    printf("Current cbmnet packet in bin:\n");
    PrintCbmnetPktRawBin();
*/


}



void CbmTSUnpackStsxyter::UnpackStsxyterMessage( const uint32_t & msg, const Int_t & sourceAddress, const uint16_t & equipmentId  )
{
    // printf("Stsxyter message:\n");
    // printf("%04x\n", msg);
    // ByteDump( (const uint8_t * const) & msg, 4 );
    if( IsEpochMessage( msg ) ) {
        UnpackEpochMessage( msg, sourceAddress, equipmentId );
        return;
    }
    if( IsHitMessage( msg ) ) {
        UnpackHitMessage( msg, sourceAddress, equipmentId );
        return;
    }
    LOG(ERROR) << "STS-XYTER message is neither hit nor epoch!" << FairLogger::endl;
}



void CbmTSUnpackStsxyter::UnpackEpochMessage( const uint32_t & msg, const Int_t & sourceAddress, const uint16_t & /*equipmentId*/ )
{
    uint32_t link   = GetField( msg, kEpochLinkFirstBit, kEpochLinkNBits );
    uint32_t nchem  = GetField( msg, kEpochNChMarkersFirstBit, kEpochNChMarkersNBits );
    uint32_t ovf    = GetBit( msg, kEpochFifoOvfBit );
    uint32_t epochA = GetField( msg, kEpochABits0To1FirstBit, kEpochABits0To1NBits )
                                | GetField( msg, kEpochABits2To6FirstBit, kEpochABits2To6NBits ) << kEpochABits0To1NBits;
    uint32_t epochB = GetField( msg, kEpochBFirstBit, kEpochBNBits );
    uint32_t epochC = GetField( msg, kEpochCFirstBit, kEpochCNBits );

    if( nchem != 0 ) {
        LOG(ERROR) << "CbmTSUnpackStsxyter::UnpackEpochMessage: Number of channel epoch markers is %d\n" << nchem << FairLogger::endl;
        // PrintCbmnetPktRawHex();
    }

    if( epochA != epochB || epochB != epochC ) {
        LOG(ERROR) << "CbmTSUnpackStsxyter::UnpackEpochMessage: Epoch numbers don't match: " << "XEpo: link=" << link << " epochA=" << epochA << " epochB=" << epochB << " epochC=" << epochC << " epochOvf=" << ovf << FairLogger::endl;
    }

    if( link >= fkgNumStsxyterLinks ) {
        LOG(ERROR) << "CbmTSUnpackStsxyter::UnpackEpochMessage: Invalid link number " << link << FairLogger::endl;
    }

    if( sourceAddress >= fCurrEpoch.size() ) {
        fCurrEpoch.resize( sourceAddress + 1, std::vector< Int_t>( fkgNumStsxyterLinks, 0 ) );
    }

    fCurrEpoch.at( sourceAddress ).at(link) = epochA;

    // LOG(INFO) << "XEpo: link=" << link << " epochA=" << epochA << " epochB=" << epochB << " epochC=" << epochC << " epochOvf=" << ovf << FairLogger::endl;
}



void CbmTSUnpackStsxyter::UnpackHitMessage( const uint32_t & msg, const Int_t & sourceAddress, const uint16_t & equipmentId )
{
    uint32_t link = GetField( msg, kHitLinkFirstBit, kHitLinkNBits );

    uint32_t kNLinks = 4;
    if( link >= kNLinks ) {
        LOG(ERROR) << "In UnpackHitMessage: Invalid link number " << link << FairLogger::endl;
    }
    uint32_t channelWithinLink = GetField( msg, kHitChannelFirstBit, kHitChannelNBits );
    const uint32_t kNChannelsPerLink = 32;
    uint32_t channel = channelWithinLink + link * kNChannelsPerLink;
    uint32_t zeroBit0   =   GetBit( msg, kHitUnusedBit0 );
    uint32_t amplitude  = GetField( msg, kHitAmpFirstBit, kHitAmpNBits );
    uint32_t zeroBit1   =   GetBit( msg, kHitUnusedBit1 );
    uint32_t parity     =   GetBit( msg, kHitParityBit );
    uint32_t hitMissed  =   GetBit( msg, kHitEvMissedBit );
    uint32_t zeroBits23 = GetField( msg, kHitUnusedBits23FirstBit, kHitUnusedBits23NBits );
    uint32_t time       = GetField( msg, kHitTimestampFirstBit, kHitTimestampNBits );

    if( zeroBit0 || zeroBit1 || zeroBits23 ) {
        LOG(ERROR) << "In UnpackHitMessage: In a HIT message one of the bits, that must be zero, is not" << FairLogger::endl;
    }

    uint32_t epoch = fCurrEpoch.at(sourceAddress).at( link );
    if( ( time & ( 1 << ( kHitTimestampNBits - 1 ) ) ) == ( epoch & 1 ) ) {
        --epoch;
    }

    // fXFullTsNs = ( static_cast<uint64_t>(epoch) * 2048 + fXTs ) * 2;

    Bool_t parityError = GetEvenParity( time ) ^ GetEvenParity( amplitude ) ^ GetEvenParity( hitMissed) ^ 1 == parity ;

    // LOG(INFO) << "XHit: ch=" << channel << " amp=" << amplitude << " time=" << time << " missed=" << hitMissed << " par_err=" << parityError << FairLogger::endl;

    Int_t hitNumber=fStsxyterRawHitMessage->GetEntriesFast();
    new( (*fStsxyterRawHitMessage)[hitNumber] )
    CbmStsxyterRawHitMessage(
          equipmentId,
          sourceAddress,
          channel,
          fCurrEpoch.at(sourceAddress).at(link),
          time,
          amplitude,
          parityError);
}



Bool_t CbmTSUnpackStsxyter::IsEpochMessage( const uint32_t & msg )
{
    if( GetField( msg, kEpochSignatureFirstBit, kEpochSignatureNBits ) == kEpochSignatureValue ) {
        return kTRUE;
    }
    return kFALSE;
}



Bool_t CbmTSUnpackStsxyter::IsHitMessage( const uint32_t & msg )
{
    if( GetField( msg, kHitSignatureFirstBit, kHitSignatureNBits ) == kHitSignatureValue ) {
        return kTRUE;
    }
    return kFALSE;
}


void  CbmTSUnpackStsxyter::InitializeParityTable()
{
    size_t parityTableSize = 2 << kHitAmpNBits;
    parityTableSize = TMath::Max( parityTableSize, static_cast< size_t >(2 << kHitTimestampNBits) );
    const uint8_t evMissedBitNBits = 1;
    parityTableSize = TMath::Max( parityTableSize, static_cast< size_t >(2 << evMissedBitNBits ) );
    fgParityTable.resize( parityTableSize );
    for( size_t number = 0; number < parityTableSize; ++number ) {
        fgParityTable.at( number ) = CalculateEvenParity( number );
        // printf("Parity of %08lu is %d\n", number, CalculateEvenParity( number ) );
    }
    fgParityTableinitialized = kTRUE;
}



Bool_t CbmTSUnpackStsxyter::CalculateEvenParity( size_t number )
{
    bool parity = 0;
    while( number ) {
        parity = !parity;
        number = number & (number - 1);
    }
    return parity;
}



void CbmTSUnpackStsxyter::Reset()
{
    fStsxyterRawHitMessage->Clear();
}


/*
void CbmTSUnpackStsxyter::Register()
{
}
 */


/*

void CbmTSUnpackStsxyter::Print6bytesMessage(const uint8_t* msContent_shifted)
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
void CbmTSUnpackStsxyter::ProcessMessage_hit(const uint8_t* msContent_shifted, uint16_t EqID, uint16_t RocID)
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
         fCurrEpoch, //              - cur_hit_data.NxLastEpoch,  //TODO subtract here or in GetFullTime() method ?!
         timestamp,
         ADCvalue,
         lastEpoch,
         pileUp,
         overflow);
}

// Missed event (currently skipped), epoch counter value, ROC id, messageType=2
// [MMMMMMMM][EEEEEEEE][EEEEEEEE][EEEEEEEE][EEEEEEEE][--RRR010]
void CbmTSUnpackStsxyter::ProcessMessage_epoch(const uint8_t* msContent_shifted)
{
   Int_t messageType = (msContent_shifted[5] >> 0) & 0x07;          // 3 bits

   fCurrEpoch = ((msContent_shifted[4] >> 0) & 0xFF)                // 8 bits
             + (((msContent_shifted[3] >> 0) & 0xFF) << 8)          // 8 bits
             + (((msContent_shifted[2] >> 0) & 0xFF) << 16)         // 8 bits
             + (((msContent_shifted[1] >> 0) & 0xFF) << 24);        // 8 bits

   // Debug printout
   #ifdef VERBOSE
   printf("messType=%d\tepoch=0x%08X\n", messageType, fCurrEpoch);
   #endif
}


// Status, Data, Timestamp, syncChannel, ROC id, messageType=3
// [SSDDDDDD][DDDDDDDD][DDDDDDDD][DDTTTTTT][TTTTTTTT][CCRRR011]
void CbmTSUnpackStsxyter::ProcessMessage_sync(const uint8_t* msContent_shifted, uint16_t EqID, uint16_t RocID)
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
         fCurrEpoch, //              - cur_hit_data.NxLastEpoch,  //TODO subtract here or in GetFullTime() method ?!
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
void CbmTSUnpackStsxyter::ProcessMessage_aux(const uint8_t* msContent_shifted, uint16_t EqID, uint16_t RocID)
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
void CbmTSUnpackStsxyter::ProcessMessage_sys(const uint8_t* msContent_shifted, uint16_t EqID, uint16_t RocID)
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

 */

ClassImp(CbmTSUnpackStsxyter)
