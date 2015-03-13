#ifndef CBMMUCHADDRESS_H
#define CBMMUCHADDRESS_H 1

#include <iomanip>
#include "Rtypes.h"
#include "FairLogger.h"
#include "CbmAddress.h"


/** Enumerator for MUCH element levels
 ** If this is changed, the initialisation of the static members
 ** of CbmMuchAddress must be changed accordingly!
 **/
enum MuchElementLevel {
  kMuchSystem,     //!< System = MUCH
  kMuchStation,    //!< Station
  kMuchLayer,      //!< Layer
  kMuchLayerSide,  //!< LayerSide
  kMuchModule,     //!< Module
  kMuchSector,     //!< Sector
  kMuchChannel,    //!< Channel
  kMuchNofLevels   //!< Number of MUCH levels
};



/** @class CbmMuchAddress
 ** @brief Interface class to unique address for the MUCH
 ** @author E.Kryshen <e.kryshen@gsi.de>
 ** @version 1.0
 **
 **Created as modification of CbmStsAddress class
 **
 ** The CbmMuchAddress interprets and modifies the unique address
 ** for the MUCH by the proper bit operation on the address bit field.
 **
 ** The current definition of the bit field for the MUCH is:
 **
 **                                               3         2         1
 ** Level           Bits  Max. Elements  Shift   10987654321098765432109876543210 \n
 ** System (kMUCH)    4         16          0    0000000000000000000000000000xxxx \n
 ** Station           3          8          4    0000000000000000000000000xxx0000 \n
 ** Layer             2          4          7    00000000000000000000000xx0000000 \n
 ** LayerSide         1          2          9    0000000000000000000000x000000000 \n
 ** Module            5         32         10    00000000000000000xxxxx0000000000 \n
 ** Sector            8        256         15    000000000xxxxxxxx000000000000000 \n
 ** Channel           9        512         23    xxxxxxxxx00000000000000000000000 \n
 **/
class CbmMuchAddress : public CbmAddress
{

  public:

    /** Construct address
     ** @param station      Station index
     ** @param layer        Layer index in station
     ** @param side         Side index in layer
     ** @param module       Module index within layerside
     ** @param sector       Sector index within module
     ** @param channel      Channel number
     ** @return Unique element address
     **/
    static UInt_t GetAddress(Int_t station = 0,
                             Int_t layer = 0,
                             Int_t side = 0,
                             Int_t module = 0,
                             Int_t sector = 0,
                             Int_t channel = 0);


    static UInt_t GetAddress(Int_t* elementIds);


    /** Get the number of hierarchy levels
     ** For use in macros which do not include this header file.
     ** @return       Number of hierarchy levels
     **/
    static Int_t GetNofLevels() { return kMuchNofLevels; }


    /** Get the number of bits for a given hierarchy level
     ** @param level  Requested element level
     ** @return       Number of bits in address field
     **/
    static Int_t GetNofBits(Int_t level) {
      if ( level < 0 || level >= kMuchNofLevels ) return 0;
      return fgkBits[level];
    }


    /** Get the index of an element
     ** @param address Unique element address
     ** @param level Hierarchy level
     ** @return Element index
     **/
    static Int_t GetElementId(UInt_t address, Int_t level) {
      if ( level < 0 || level >= kMuchNofLevels ) return -1;
      return ( address & ( fgkMask[level] << fgkShift[level] ) )
               >> fgkShift[level];
    }
    
    /** Derivatives */
    static Int_t GetSystemIndex(Int_t address)    { return GetElementId(address,kMuchSystem);    }
    static Int_t GetStationIndex(Int_t address)   { return GetElementId(address,kMuchStation);   }
    static Int_t GetLayerIndex(Int_t address)     { return GetElementId(address,kMuchLayer);     }
    static Int_t GetLayerSideIndex(Int_t address) { return GetElementId(address,kMuchLayerSide); }
    static Int_t GetModuleIndex(Int_t address)    { return GetElementId(address,kMuchModule);    }
    static Int_t GetSectorIndex(Int_t address)    { return GetElementId(address,kMuchSector);    }
    static Int_t GetChannelIndex(Int_t address)   { return GetElementId(address,kMuchChannel);   }

    static Int_t GetElementAddress(Int_t address, Int_t level) {
      Int_t mask = (1 << (CbmMuchAddress::fgkShift[level] + CbmMuchAddress::fgkBits[level])) -1;
      return address & mask;
    }
    
    /** Print information on the bit field **/
    static void Print();


    /** Set the index of an element
     ** leaving the other element levels untouched
     ** @param address Unique element address
     ** @param level   Hierarchy level
     ** @param newId   New element index
     ** @return New address
     **/
     static UInt_t SetElementId(UInt_t address, Int_t level, Int_t newId) {
       if ( level < 0 || level >= kMuchNofLevels ) return address;
       if ( newId >= ( 1 << fgkBits[level]) ) {
         LOG(ERROR) << "Id " << newId << " for MUCH level " << level
                    << " exceeds maximum (" << (1 << fgkBits[level]) - 1
                    << ")" << FairLogger::endl;
         return 0;
       }
       return ( address & (~ (fgkMask[level] << fgkShift[level]) ) )
              | ( newId << fgkShift[level]);
     }

  private:

    /** Number of bits for the different levels **/
    static const Int_t fgkBits[kMuchNofLevels];

    /** Bit shifts for the different levels **/
    static const Int_t fgkShift[kMuchNofLevels];

    /** Bit masks for the different levels **/
    static const Int_t fgkMask[kMuchNofLevels];


    ClassDef(CbmMuchAddress,1);
};


#endif /* CBMMUCHADDRESS_H  */
