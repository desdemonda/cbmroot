/** CbmStsDigi.h
 **@author V.Friese <v.friese@gsi.de>
 **@since 28.08.2006
 **@version 2.0
 **
 ** Data class for digital STS information
 ** Data level: RAW
 **
 ** The information is encoded into 8 bytes (1 Long64_t).
 ** Address:                       25 bits
 **   - Station Nr.        4 bits
 **   - Sector Nr.        10 bits
 **   - Side (front/back)  1 bit
 **   - Channel Nr.       10 bits
 ** Charge (ADC):                  12 bits
 ** Time (ns):                     14 bits
 **
 ** The time is relative to the event time. 
 **/


#ifndef CBMSTSDIGI_H
#define CBMSTSDIGI_H 1

#include "CbmDigi.h"

#include "CbmDetectorList.h"

class CbmStsDigi : public CbmDigi
{

 public:

  /**   Default constructor   **/
  CbmStsDigi();


  /**   Standard constructor
   ** @param  address  Unique element address
   ** @param  time     Measurement time [ns]
   ** @param  charge   Charge [ADC units]
   **/
  CbmStsDigi(UInt_t address, ULong64_t time, UShort_t charge);


  /**@brief Constructor with additional sectorNr
   **
   ** @param  address  Unique element address
   ** @param  time     Measurement time [ns]
   ** @param  charge   Charge [ADC units]
   ** @param  sectorNr Number of sector within station
   **
   ** TODO: Temporarily kept for backward compatibility with classes using
   ** CbmStsDigiScheme. SectorNr will be stored in a separate variable,
   ** since it does not fit into the CbmAddress scheme. In the address
   ** variable, it is set to zero.
   **
   **/
  CbmStsDigi(UInt_t address, ULong64_t time, UShort_t charge,
  		       Int_t sectorNr);


  /**   Destructor   **/
  virtual ~CbmStsDigi();


  /**  Unique detector element address  (see CbmStsAddress) **/
  virtual Int_t GetAddress() const { return fAddress; }


  /**  Charge [ADC units]  **/
  virtual Double_t GetCharge() const { return Double_t(fCharge); }


  /**   Sector number
   **    Kept for bacjwrad compatibility
   **/
  Int_t GetSectorNr() const { return fSectorNr; }


  /**  System ID (type DetectorId)  **/
  virtual Int_t GetSystemId() const { return kSTS; }


  /**  Time of measurement [ns]  **/
  virtual Double_t GetTime() const { return Double_t(fTime); }






  /**   Add charge   **/
//  void AddAdc(Int_t adc);
//   void AddIndex(int index, int adc){
// 			AddLink(FairLink(kStsPoint, index, adc));
// 		}
//   void AddIndex(std::vector<Int_t> index){
// 			SetLinks(FairMultiLinkedData(kStsPoint, index));
// 		}


  /**   Maximal number of ADC bits   **/
 // static Int_t GetNofAdcBits() { return fgkCharBits; }


  /**   Station number
   **   Kept for backward compatibility   **/
  //Int_t GetStationNr() const { return CbmStsAddress:;GetElementId(fAddress, kStsStation); }



  /**   Side ( 0 = front, 1 = back )   **/
  //Int_t GetSide() const { return Side(GetDetectorId()); }


  /**   Channel number   **/
  //Int_t GetChannelNr() const { return ChannelNr(GetDetectorId()); }


  /**   Charge   **/
//  Int_t GetAdc() const { return ( (fData >> fgkCharShift) & fgkCharMask ); }


  /**   Maximum ADC   **/
 // static Int_t GetMaxAdc() { return ( (1 << fgkCharBits) - 1 ); }



  /**   Unique detector ID   **/
//  Int_t GetDetectorId() const {
//    return ( kSTS | ( (fData & fgkAddrMask) << 4) );
//  }



 private:


  UInt_t fAddress;     ///< Unique detector address
  ULong64_t fTime;     ///< Time [ns]
  UShort_t fCharge;    ///< Charge [ADC units]


  // TODO: fSectorNr is here for backward compatibility with classes
  // using CbmStsDigiScheme. Should be eventually removed.
  Int_t fSectorNr;      ///< Number of sector within station


  /**   Modifiers   **/
  void SetAdc(Int_t charge);


  /**   Static members for bit operations
  static const Int_t fgkAddrBits;
  static const Int_t fgkCharBits;
  static const Int_t fgkTimeBits;
  static const Int_t fgkCharShift;
  static const Int_t fgkTimeShift;
  static const Long64_t fgkAddrMask;
  static const Long64_t fgkCharMask;
  static const Long64_t fgkTimeMask;

 **/


  ClassDef(CbmStsDigi,3);

};


#endif
