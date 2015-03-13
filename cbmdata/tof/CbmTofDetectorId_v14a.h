// -------------------------------------------------------------------------
// -----               CbmTofDetectorId_v12b header file               -----
// -----                 Created 20/11/12  by F. Uhlig                 -----
// -------------------------------------------------------------------------


/** CbmTofDetectorId.h
 ** Defines unique detector identifier for all TOF modules.
 ** This class is the implementation for tof geometry version v14a 
 ** nh, 11.03.2014
 ** All classes which uses this scheme should have a data member
 ** of this class 
 ** @author F. Uhlig <f.uhlig@gsi.de>
 **/
 
//                                  3         2         1          shift length
/** Current definition:            10987654321098765432109876543210
 ** System ID (kTOF=6) on bits 0-3 00000000000000000000000000001111       15
 ** SuperModule ID on bits 4-11    00000000000000000000111111110000 <<4   255
 ** SuperModule Type on bits 12-15 00000000000000001111000000000000 <<12  15
 ** Module NR on bits 16-23        00000000011111110000000000000000 <<16  127
 ** Side NR on bits 20-23          00000000100000000000000000000000 <<23  1
 ** Cell NR on bits 24-31          11111111000000000000000000000000 <<24  255
 **/


#ifndef CBMTOFDETECTORID_V14A_H
#define CBMTOFDETECTORID_V14A_H 1

#include"CbmTofDetectorId.h"

class CbmTofDetectorId_v14a : public CbmTofDetectorId
{

  public:
  
  /** Constructor **/
  CbmTofDetectorId_v14a();

  /** Destructor **/
  ~CbmTofDetectorId_v14a() {};
  
  /** Get complete system info from detector ID
   ** This will return a pointer to an integer
   ** array of length array_length
   **/
  CbmTofDetectorInfo GetDetectorInfo(const Int_t detectorId);

  /** Get the global sytem ID **/
  Int_t GetSystemId(Int_t detectorId);

  /** Get SMType number from detector ID **/
  Int_t GetSMType(const Int_t detectorId);
  
   /** Get smodule number from detector ID **/
  Int_t GetSModule(const Int_t detectorId);
  
  /** Get module number from detector ID **/
  Int_t GetCounter(const Int_t detectorId);
  
  /** Get sector number from detector ID **/
  Int_t GetGap(const Int_t detectorId);

  /** Get sector number from detector ID **/
  Int_t GetSide(const Int_t detectorId);
  
  /** Get cell number from detector ID **/
  Int_t GetCell(const Int_t detectorId);

  /** Get cell number from detector ID **/
  Int_t GetStrip(const Int_t detectorId);
  
  /** Get region number from detector ID **/
  Int_t GetRegion(const Int_t detectorId);

  /** Get cell number from detector ID.
   ** This is masking the the gap number
   ** if this is set.
   **/
  Int_t GetCellId(const Int_t detectorId);

  /** Calculate the unique detector ID
   ** This will return a pointer to an integer
   ** array of length array_length
   **/
  Int_t SetDetectorInfo(const CbmTofDetectorInfo detectorInfo);
  
 private:
 
  static const Int_t array_length = 6;
  static const Int_t shiftarray[];
  static const Int_t bitarray[];
  Int_t result_array[array_length];
  Int_t maskarray[array_length];
  Int_t modulemask;
};

#endif
