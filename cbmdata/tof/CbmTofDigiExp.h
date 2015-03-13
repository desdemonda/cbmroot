/** CbmTofDigiExp.h
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @date 07.06.2013
 **/
 
/** @class CbmTofDigiExp
 ** @brief Data class for expanded digital TOF information
 ** @brief Data level: TDC CALIB
 ** @version 1.0
 **
 ** The information is encoded into 3*4 bytes (2 Double_t + 1 UInt_t).
 ** Unique Address:                32 bits following CbmTofAddress
 ** Calibrated Time [ps]:          32 bits double
 ** Calibrated Tot  [ps]:          32 bits double
 **
 ** In triggered setup, the time is relative to the trigger time, which 
 ** is measured with a resolution of a few ns corresponding to the TDC 
 ** system clock cycle. 
 ** In free-streaming setups, the time is relative to the last epoch.
 **/ 
 
#ifndef CBMTOFDIGIEXP_H
#define CBMTOFDIGIEXP_H 1

#include "CbmDigi.h"
#include "CbmTofAddress.h"

class TString;

class CbmTofDigiExp : public CbmDigi
{
   public:
      /**
       ** @brief Default constructor.
       **/
      CbmTofDigiExp();

      /**
       ** @brief Constructor with assignment.
       ** @param[in] address Unique channel address. (cf CbmTofAddress)
       ** @param[in] time    Absolute time [ps].
       ** @param[in] tot     Time Over Threshold [ps].
       **/
      CbmTofDigiExp( UInt_t address, Double_t time, Double_t tot );

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
      CbmTofDigiExp( UInt_t Sm, UInt_t Rpc, UInt_t Channel, 
                     Double_t time, Double_t tot, 
                     UInt_t Side = 0, UInt_t SmType = 0 );

      /**
       ** @brief Copy constructor.
       **/
      CbmTofDigiExp(const CbmTofDigiExp&);

      /**
       ** @brief Destructor.
       **/
      ~CbmTofDigiExp();

      /** Accessors **/
         /**
          ** @brief Inherited from CbmDigi.
          **/
      Int_t GetAddress() const { return fuAddress; };

         /**
          ** @brief Inherited from CbmDigi.
          **/
      Int_t GetSystemId() const { return CbmTofAddress::GetSystemId(fuAddress); };

         /**
          ** @brief Inherited from CbmDigi.
          **/
      Double_t GetTime() const { return fdTime; };

         /**
          ** @brief Inherited from CbmDigi.
          **/
      Double_t GetCharge() const { return fdTot; };
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
          ** @brief Sorting using the time, assumes Digis are in same reference frame (e.g. same epoch).
          **/
      virtual Bool_t operator <( const CbmTofDigiExp& rhs) const; 
      virtual Int_t	 Compare(      const TObject* obj) const;
      virtual Int_t	 Compare(      const CbmTofDigiExp*  obj) const;
      virtual Bool_t IsSortable() const { return kTRUE; };
      
      /** Modifiers **/
      void SetAddress(Int_t address) { fuAddress = address; };
//      void SetAddress( UInt_t Sm, UInt_t Rpc, UInt_t Channel,
//                        UInt_t Side = 0, UInt_t SmType = 0 );
      void SetTime(Double_t time) { fdTime = time; };
      void SetTot(Double_t tot) { fdTot = tot; };

      TString ToString() const;
   
   private:
      UInt_t   fuAddress; ///< Unique channel address
      Double_t fdTime;    ///< Absolute time [ps]
      Double_t fdTot;     ///< Tot [ps]
      
   ClassDef(CbmTofDigiExp, 1);
};
#endif // CBMTOFDIGIEXP_H
