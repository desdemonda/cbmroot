#ifndef CBMTSUNPACKSTSXYTER_H
#define CBMTSUNPACKSTSXYTER_H

#ifndef __CINT__
#include "Timeslice.hpp"
#endif

#include "CbmTSUnpack.h"
#include "TClonesArray.h"
#ifdef __APPLE__
#include <_types/_uint8_t.h> 
#include <_types/_uint16_t.h>
#include <_types/_uint32_t.h>
#include <_types/_uint64_t.h>
#else
#include <stdint.h>
#endif

class CbmTSUnpackStsxyter : public CbmTSUnpack
{
public:

    CbmTSUnpackStsxyter();
    virtual ~CbmTSUnpackStsxyter();

    virtual Bool_t Init();
#ifndef __CINT__
    virtual Bool_t DoUnpack(const fles::Timeslice& ts, size_t component);
#endif
    virtual void Reset();

    virtual void Finish() {;}

private:
    static const size_t fkgDtmSizeOffset = 0;
    static const size_t fkgDtmNumOffset = 1;
    static const size_t fkgDtmDataOffset = 2;
    static const size_t fkgNumStsxytersDefault = 8;
    static const size_t fkgNumStsxyterLinks = 4;

    Int_t    fDtmNum;
    Bool_t   fDtmNumInitialized;
    std::vector< std::vector< Int_t > > fCurrEpoch;

    Bool_t           CheckDtmNumSequence( const uint8_t * const & dtm,  const Bool_t & newTimeslice );
    const uint8_t *  GetFirstDtm( const uint8_t * const & microsliceBegin ) const;
    const uint8_t *  GetDtmContent( const uint8_t * const & dtm ) const;
    size_t           GetDtmContentSize( const uint8_t * const & dtm ) const;
    size_t           GetDtmFullSize( const uint8_t * const & dtm ) const;
    Bool_t           IsStsxyterDtm( const uint8_t * const & dtmContent, const size_t & size ) const;
    Bool_t           IsStsxyterDtmUnstrict( const uint8_t * const & dtmContent, const size_t & size ) const;
    void             ByteDump( const uint8_t * const & data, size_t sizeBytes, size_t lineLenBytes = 32  ) const;
    void             ByteDumpSwap( const uint8_t * const & data, size_t sizeBytes, size_t lineLenBytes = 32  ) const;
    void             UnpackDtmContent( const uint8_t * const & dtmContent, const size_t & size, const uint16_t & equipmentId );
    void             UnpackStsxyterDtmContent( const uint8_t * const & dtmContent, const size_t & size, const uint16_t & equipmentId );
    void             UnpackStsxyterMessage( const uint32_t & msg, const Int_t & sourceAddress, const uint16_t & equipmentId );
    void             UnpackEpochMessage( const uint32_t & msg, const Int_t & sourceAddress, const uint16_t & equipmentId );
    void             UnpackHitMessage( const uint32_t & msg, const Int_t & sourceAddress, const uint16_t & equipmentId );
    static Bool_t    IsEpochMessage( const uint32_t & msg );
    static Bool_t    IsHitMessage( const uint32_t & msg );

    static uint32_t  GetUint32( const uint8_t * const & val ) { return * ( reinterpret_cast< const uint32_t * const >( val ) ); }
    static uint32_t  GetField(const uint32_t dword, const uint8_t shift, const uint8_t len) { return (dword >> shift) & ((((uint32_t)1) << len) - 1); }
    static uint32_t  GetBit(const uint32_t dword, const uint8_t shift) { return ( dword >> shift ) & 1; }

    static void      InitializeParityTable();
    static std::vector< Bool_t > fgParityTable;
    static Bool_t    fgParityTableinitialized;
    static Bool_t    CalculateEvenParity( size_t number );
    static Bool_t    GetEvenParity( uint32_t number ) { return fgParityTable.at( number ); }

    TClonesArray* fStsxyterRawHitMessage; // Output array of raw hit messages

#ifdef __SWAPWORDS
#error "__SWAPWORDS" identifier is defined
#endif

#define __SWAPWORDS( firstBit ) ( (firstBit) >= 16 ? (firstBit) - 16 : (firstBit) + 16 )

    static const uint8_t kHitSignatureFirstBit = __SWAPWORDS( 30 );
    static const uint8_t kHitSignatureNBits = 31 - 30 + 1 ;
    static const uint8_t kHitSignatureValue = 0b10 ;

    static const uint8_t kHitLinkFirstBit = __SWAPWORDS( 28 );
    static const uint8_t kHitLinkNBits = 29 - 28 + 1 ;

    static const uint8_t kHitChannelFirstBit = __SWAPWORDS( 23 );
    static const uint8_t kHitChannelNBits = 27 - 23 + 1;

    static const uint8_t kHitUnusedBit0 = __SWAPWORDS( 22 );
    static const uint32_t kHitUnusedBit0Value = 0;

    static const uint8_t kHitAmpFirstBit = __SWAPWORDS( 17 );
    static const uint8_t kHitAmpNBits = 21 - 17 + 1;

    static const uint8_t kHitUnusedBit1 = __SWAPWORDS( 16 );
    static const uint32_t kHitUnusedBit1Value = 0;

    static const uint8_t kHitParityBit = __SWAPWORDS( 15 );

    static const uint8_t kHitEvMissedBit = __SWAPWORDS( 14 );

    static const uint8_t kHitUnusedBits23FirstBit = __SWAPWORDS( 12 );
    static const uint8_t kHitUnusedBits23NBits = 13 - 12 + 1;
    static const uint32_t kHitUnusedBits23Value = 0b00;

    static const uint8_t kHitTimestampFirstBit = __SWAPWORDS( 0 );
    static const uint8_t kHitTimestampNBits = 11 - 0 + 1;
    // static const uint8_t kHitTimestampNBits = 10 - 0 + 1;

    static const uint8_t kEpochSignatureFirstBit = __SWAPWORDS( 29 );
    static const uint8_t kEpochSignatureNBits = 3;
    static const uint8_t kEpochSignatureValue = 0b001;

    static const uint8_t kEpochLinkFirstBit = __SWAPWORDS( 27 );
    static const uint8_t kEpochLinkNBits = 28 - 27 + 1;

    static const uint8_t kEpochNChMarkersFirstBit = __SWAPWORDS( 22 );
    static const uint8_t kEpochNChMarkersNBits = 26 - 22 + 1 ;

    static const uint8_t kEpochFifoOvfBit = __SWAPWORDS( 22 );

    static const uint8_t kEpochABits0To1FirstBit = __SWAPWORDS( 14 );
    static const uint8_t kEpochABits0To1NBits = 15 - 14 + 1 ;

    static const uint8_t kEpochABits2To6FirstBit = __SWAPWORDS( 16 );
    static const uint8_t kEpochABits2To6NBits = 20 - 16 + 1 ;

    static const uint8_t kEpochBFirstBit = __SWAPWORDS( 7 );
    static const uint8_t kEpochBNBits = 13 - 7 + 1 ;

    static const uint8_t kEpochCFirstBit = __SWAPWORDS( 0 );
    static const uint8_t kEpochCNBits =  6 - 0 + 1 ;

#undef __SWAPWORDS


    ClassDef(CbmTSUnpackStsxyter, 1);
};

#endif
