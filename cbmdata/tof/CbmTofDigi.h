/** CbmTofDigi.h
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @date 07.06.2013
 **/
 
/** @class CbmTofDigi
 ** @brief Data class for expanded digital TOF information
 ** @brief Data level: TDC CALIB
 ** @version 1.0
 **
 ** The information is encoded into 1*8 bytes (1 Long64_t).
 ** Unique Address:                 32 bits following CbmTofAddress
 ** Calibrated Time [T Digig Bin]:  22 bits with Bin Size defined as constant in this file
 ** Calibrated Tot  [Tot Digi Bin]: 10 bits with Bin Size defined as constant in this file 
 **
 ** In triggered setup, the time is relative to the trigger time, which 
 ** is measured with a resolution of a few ns corresponding to the TDC 
 ** system clock cycle. 
 ** In free-streaming setups, the time is relative to the last epoch.
 **/ 
 
#ifndef CBMTOFDIGI_H
#define CBMTOFDIGI_H 1

#include "CbmDigi.h"
#include "CbmTofAddress.h"

class TString;

class CbmTofDigi : public CbmDigi
{
   public:
      /**
       ** @brief Default constructor.
       **/
      CbmTofDigi();

      /**
       ** @brief Constructor with assignment.
       ** @param[in] address Unique channel address. (cf CbmTofAddress)
       ** @param[in] time    Absolute time [ps].
       ** @param[in] tot     Time Over Threshold [ps].
       **/
      CbmTofDigi( UInt_t address, Double_t time, Double_t tot );

      /**
       ** @brief Constructor with detailled assignment.
       ** @param[in] Sm      Super Module Id. (cf CbmTofAddress)
       ** @param[in] Rpc     Rpc Id. (cf CbmTofAddress)
       ** @param[in] Channel Channel Id. (cf CbmTofAddress)
       ** @param[in] time    Absolute time [ps].
       ** @param[in] tot     Time Over Threshold [ps].
       ** @param[in] Side    Channel Side (optional, used for strips). (cf CbmTofAddress)
       ** @param[in] Sm Type Super Module Type (optional). (cf CbmTofAddress)
       **/
      CbmTofDigi( UInt_t Sm, UInt_t Rpc, UInt_t Channel, 
                     Double_t time, Double_t tot, 
                     UInt_t Side = 0, UInt_t SmType = 0 );

      /**
       ** @brief Copy constructor.
       **/
      CbmTofDigi(const CbmTofDigi&);

      /**
       ** @brief Destructor.
       **/
      ~CbmTofDigi();

      /** Accessors **/
         /**
          ** @brief Inherited from CbmDigi.
          **/
      Int_t GetAddress() const { return   ( (flData >> fgkiAddressOffs) & fgklAddrMask ); };

         /**
          ** @brief Inherited from CbmDigi.
          **/
      Int_t GetSystemId() const { return CbmTofAddress::GetSystemId( GetAddress() ); };

         /**
          ** @brief Inherited from CbmDigi.
          **/
      Double_t GetTime() const { return fgkdTimeBinSize * ( (flData >> fgkiTimeOffs)    & fgklTimeMask ); };

         /**
          ** @brief Inherited from CbmDigi.
          **/
      Double_t GetCharge() const { return fgkdTotBinSize * ( (flData >> fgkiTotOffs)     & fgkiTotOffs ); };
         /**
          ** @brief Alias for GetCharge.
          **/
      Double_t GetTot()    const { return GetCharge(); };
         /**
          ** @brief Sm.
          **/
      Double_t GetSm()    const { return CbmTofAddress::GetSmId( GetAddress() ); };
         /**
          ** @brief Sm Type .
          **/
      Double_t GetType()    const { return CbmTofAddress::GetSmType( GetAddress() ); };
         /**
          ** @brief Detector aka Module aka RPC .
          **/
      Double_t GetRpc()    const { return CbmTofAddress::GetRpcId( GetAddress() ); };
         /**
          ** @brief Channel .
          **/
      Double_t GetChannel()    const { return CbmTofAddress::GetChannelId( GetAddress() ); };
         /**
          ** @brief Channel Side.
          **/
      Double_t GetSide()    const { return CbmTofAddress::GetChannelSide( GetAddress() ); };
         /**
          ** @brief Full data acess.
          **/
      Long64_t GetData()    const { return flData; };

         /**
          ** @brief Sorting using the time, assumes Digis are in same reference frame (e.g. same epoch).
          **/
      virtual Bool_t operator <( const CbmTofDigi& rhs) const; 
      virtual Int_t	 Compare(      const TObject* obj) const;
      virtual Int_t	 Compare(      const CbmTofDigi*  obj) const;
      virtual Bool_t IsSortable() const { return kTRUE; };

      /** Modifiers **/
      void SetAddress( UInt_t address);
      void SetAddress( UInt_t Sm, UInt_t Rpc, UInt_t Channel,
                        UInt_t Side = 0, UInt_t SmType = 0 );
      void SetTime(    Double_t time);
      void SetTot(     Double_t tot);

      TString ToString() const;
   
   private:
      Long64_t flData; ///< Fully compressed data
      
// -----   Bit fields sizes   --------------------------------------------------
      static const Int_t    fgkiAddressSize =  32;
      static const Int_t    fgkiTimeSize    =  22;
      static const Int_t    fgkiTotSize     =  10;
// -----------------------------------------------------------------------------
      
// -----   Bit masks -----------------------------------------------------------
      static const Long64_t fgklAddrMask;
      static const Long64_t fgklTimeMask;
      static const Long64_t fgklTotMask;
// -----------------------------------------------------------------------------

// -----   Bit shifts   --------------------------------------------------------
      static const Int_t    fgkiAddressOffs;
      static const Int_t    fgkiTimeOffs;
      static const Int_t    fgkiTotOffs;
// -----------------------------------------------------------------------------
      
      static const Double_t fgkdTimeBinSize; // [ps] => 41 943.40 ns ( > GET4 epoch, > trigger window)
      static const Double_t fgkdTotBinSize; // [ps] => 0-51.200 range
      
   ClassDef(CbmTofDigi, 1);
};
#endif // CBMTOFDIGI_H 
