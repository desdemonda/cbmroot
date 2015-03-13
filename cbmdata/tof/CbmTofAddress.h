/** @file CbmTofAddress.h
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @date 07.06.2013
 **/
  
/** @class CbmTofAddress
 ** @brief CBM ToF interface class to the unique address
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @version 1.0
 **
 ** CbmTofAddress is the class for the concrete interfaces to the
 ** unique address, which is encoded in a 32-bit field (Int_t), for the 
 ** ToF detector elements.
 ** Difference to CbmTofDetectorId is that this class is adapted to
 ** real data instead of simulated data => no Gap info but instead info
 ** on strip side.
 ** Conversion functions are provided for now, but version dependent!
 **                                   3         2         1         0 Shift Bits Values
 ** Current definition:              10987654321098765432109876543210
 ** System ID (kTof=6) on bits  0- 3 00000000000000000000000000001111  << 0    4     15
 ** Super Module (SM)  on bits  4-11 00000000000000000000111111110000  << 4    8    256
 ** SM Type            on bits 12-14 00000000000000001111000000000000  <<12    4     15
 ** RPC ID             on bits 16-22 00000000011111110000000000000000  <<16    7    128
 ** Channel Side       on bits 23-23 00000000100000000000000000000000  <<23    1      2
 ** Channel ID         on bits 24-31 11111111000000000000000000000000  <<24    8    256
 **
 ** Changing the number of bits of a fields automatically shift all others
 ** => to adapt field length, just change its size and the size of one of the other fields
 **/

#ifndef CBMTOFADDRESS_H
#define CBMTOFADDRESS_H 1

#include "CbmAddress.h"

#include "CbmTofDetectorId_v12b.h"

class CbmTofAddress : public CbmAddress
{
   public:
 	
      /** Constructor  **/
      CbmTofAddress() { };

      /** Destructor  **/
      virtual ~CbmTofAddress() { };

      /** Field size accessors **/
         /** Number of bits for Super Module Id in the address field
          ** @return Number of bits
          **/
      static Int_t GetNofSmIdBits() { return fgkSmIdBits; };
         /** Number of bits for Super Module Type in the address field
          ** @return Number of bits
          **/
      static Int_t GetNofSmTypeBits() { return fgkSmTypeBits; };
         /** Number of bits for Rpc Id in the address field
          ** @return Number of bits
          **/
      static Int_t GetNofRpcIdBits() { return fgkRpcIdBits; };
         /** Number of bits for Channel Id in the address field
          ** @return Number of bits
          **/
      static Int_t GetNofChannelIdBits() { return fgkChannelIdBits; };
         /** Number of bits for Channel Side in the address field
          ** @return Number of bits
          **/
      static Int_t GetNofChSideBits() { return fgkChannelSideBits; };

      /** Maskers **/
         /** Get the Super Module Id from the address
          ** @param address  Unique address
          ** @return  systemId
          **/
      static Int_t GetSmId(UInt_t address) {
         return ( (address >> fgkSmIdOffset)        & ( ( 1 << fgkSmIdBits ) - 1 ) );
      };
         /** Get the Super Module Type from the address
          ** @param address  Unique address
          ** @return  systemId
          **/
      static Int_t GetSmType(UInt_t address) {
         return ( (address >> fgkSmTypeOffset)      & ( ( 1 << fgkSmTypeBits ) - 1 ) );
      };
         /** Get the Rpc Id from the address
          ** @param address  Unique address
          ** @return  systemId
          **/
      static Int_t GetRpcId(UInt_t address) {
         return ( (address >> fgkRpcIdOffset)       & ( ( 1 << fgkRpcIdBits ) - 1 ) );
      };
         /** Get the Channel Id from the address
          ** @param address  Unique address
          ** @return  systemId
          **/
      static Int_t GetChannelId(UInt_t address) {
         return ( (address >> fgkChannelIdOffset)   & ( ( 1 << fgkChannelIdBits ) - 1 ) );
      };
         /** Get the Channel Side from the address
          ** @param address  Unique address
          ** @return  systemId
          **/
      static Int_t GetChannelSide(UInt_t address) {
         return ( (address >> fgkChannelSideOffset) & ( ( 1 << fgkChannelSideBits ) - 1 ) );
      };
         /** Get the module Full Id from the address
          ** @param address  Unique address
          ** @return  systemId
          **/
      static Int_t GetModFullId(UInt_t address) {
         return ( address & fgkiModFullIdMask );
      };
      
      /** Builder **/
         /** Get the unique address from all parameters
          ** @param[in] Sm      Super Module Id.
          ** @param[in] Rpc     Rpc Id.
          ** @param[in] Channel Channel Id.
          ** @param[in] Side    Channel Side (optional, used for strips).
          ** @param[in] Sm Type Super Module Type (optional).
          ** @return  address
          **/
      static UInt_t GetUniqueAddress( UInt_t Sm, UInt_t Rpc, UInt_t Channel, UInt_t Side = 0, UInt_t SmType = 0) {
         return (UInt_t)( ( ( kTof    & ( ( 1 << fgkSystemBits      ) - 1 ) )                         ) +
                           ( ( Sm      & ( ( 1 << fgkSmIdBits        ) - 1 ) ) << fgkSmIdOffset        ) +
                           ( ( SmType  & ( ( 1 << fgkSmTypeBits      ) - 1 ) ) << fgkSmTypeOffset      ) +
                           ( ( Side    & ( ( 1 << fgkChannelSideBits ) - 1 ) ) << fgkChannelSideOffset ) +
                           ( ( Rpc     & ( ( 1 << fgkRpcIdBits       ) - 1 ) ) << fgkRpcIdOffset       ) +
                           ( ( Channel & ( ( 1 << fgkChannelIdBits   ) - 1 ) ) << fgkChannelIdOffset   ) );
      };
      
      static Bool_t SameModule( UInt_t addressA, UInt_t addressB )
      {
         return ( GetModFullId( addressA ) == GetModFullId( addressB ) ) ? kTRUE : kFALSE;
      };
      
      static UInt_t ConvertCbmTofDetectorInfo( CbmTofDetectorInfo infoInput ) {
         // For now assume that the system ID will always be correct
         // Otherwise would need including CbmDetectorList.h
      //   if( kTof != infoInput.fDetectorSystem)
      //      return 0;

         return GetUniqueAddress( infoInput.fSModule, infoInput.fCounter, infoInput.fCell, 0, infoInput.fSMtype );
      };
      static UInt_t ConvertCbmTofDetectorId( Int_t detIdInput ) {
         // For now assume that the system ID will always be correct
         // Otherwise would need including CbmDetectorList.h
//         if( kTof != CbmTofDetectorId::GetSystemId( detIdInput ) )
//            return 0;
         CbmTofDetectorId_v12b detId;
         return GetUniqueAddress( detId.GetSModule( detIdInput ),
                                   detId.GetCounter( detIdInput ),
                                   detId.GetCell( detIdInput ),
                                   0,
                                   detId.GetSMType( detIdInput ) );
      };

   private:
      /**
       ** To adapt the address sub-fields repartition in size, 
       ** you just need to change number of bits of the two sub-fields changing length.
       **/
       
      /** Sub-fields sizes in bits   **/
         /** Number of bits for Super Module Id in the address field  **/
      static const Int_t fgkSmIdBits        = 8;
         /** Number of bits for Super Module Type in the address field  **/
      static const Int_t fgkSmTypeBits      = 4;
         /** Number of bits for Rpc Id in the address field  **/
      static const Int_t fgkRpcIdBits       = 7;
         /** Number of bits for Channel Side in the address field  **/
      static const Int_t fgkChannelSideBits = 1;
         /** Number of bits for Channel Id in the address field  **/
      static const Int_t fgkChannelIdBits   = 8;
      
      /**
       ** To adapt the address sub-fields repartition in order,
       ** you just need to change the way the offset are calculated.
       **/
       
      /** Sub-fields offsets in bits **/   
         /** Offset in bits for Super Module Id in the address field  **/
      static const Int_t fgkSmIdOffset;
         /** Offset in bits for Super Module Type in the address field  **/
      static const Int_t fgkSmTypeOffset;
         /** Offset in bits for Channel Side Id in the address field  **/
      static const Int_t fgkChannelSideOffset; 
         /** Offset in bits for Rpc Id in the address field  **/
      static const Int_t fgkRpcIdOffset;
         /** Offset in bits for Channel Id in the address field  **/
      static const Int_t fgkChannelIdOffset;
      
      /**
       ** For the module Full Id determination
       **/
      static const Int_t fgkiModFullIdMask;
};

#endif // CBMTOFADDRESS_H
 
