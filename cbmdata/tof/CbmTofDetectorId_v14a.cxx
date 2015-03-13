// -------------------------------------------------------------------------
// -----                   CbmTofDetectorId source file                -----
// -----                  Created 20/11/12  by F. Uhlig                -----
// -------------------------------------------------------------------------


#include "CbmTofDetectorId_v14a.h"
#include "CbmDetectorList.h"
#include "FairLogger.h"

const Int_t CbmTofDetectorId_v14a::shiftarray[] = {0,4,12,16,23,24};
const Int_t CbmTofDetectorId_v14a::bitarray[]   = {4,8,4,7,1,8};


CbmTofDetectorId_v14a::CbmTofDetectorId_v14a()
  : CbmTofDetectorId(),
    result_array(),
    maskarray(),
    modulemask(0)
{
  for (Int_t i=0; i < array_length; i++) {
	maskarray[i]=(1 << bitarray[i]) - 1;
  }

  modulemask =(   (maskarray[0] << shiftarray[0]) |
		  (maskarray[1] << shiftarray[1]) |
		  (maskarray[2] << shiftarray[2]) |
		  (maskarray[3] << shiftarray[3])  |
		  (0 << shiftarray[4]) |
		  (maskarray[5] << shiftarray[5])
  );
}

CbmTofDetectorInfo CbmTofDetectorId_v14a::GetDetectorInfo(const Int_t detectorId)
{
  for (Int_t i=0; i < array_length; i++) {
   result_array[i] = (( detectorId >> shiftarray[i] ) & maskarray[i] );
  }

  return CbmTofDetectorInfo(result_array[0], result_array[2], result_array[1], 
			    result_array[3], result_array[4], result_array[5]);

}

Int_t  CbmTofDetectorId_v14a::GetSystemId(Int_t detectorId) 
{
  return (detectorId & maskarray[0]);
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v14a::GetSMType(const Int_t detectorId)
{
  /*
  Int_t iRet=( detectorId >> shiftarray[2] ) & maskarray[2];
  LOG(DEBUG)<< " V14a::GetSMType "
	    <<detectorId<<" "<<shiftarray[2]<<" "<<maskarray[2]
	    <<" -> " << iRet 
	    <<FairLogger::endl;
  */
  return (( detectorId >> shiftarray[2] ) & maskarray[2] );
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v14a::GetSModule(const Int_t detectorId)
{
  return (( detectorId >> shiftarray[1] ) & maskarray[1] );
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v14a::GetCounter(const Int_t detectorId)
{
   return (( detectorId >> shiftarray[3] ) & maskarray[3] );
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v14a::GetSide(const Int_t detectorId)
{
   return (( detectorId >> shiftarray[4] ) & maskarray[4] );
}

Int_t CbmTofDetectorId_v14a::GetGap(const Int_t detectorId)
{
  return -1; 
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v14a::GetCell(const Int_t detectorId)
{
   return (( detectorId >> shiftarray[5] ) & maskarray[5] );
}

Int_t CbmTofDetectorId_v14a::GetStrip(const Int_t detectorId)
{
   return (( detectorId >> shiftarray[5] ) & maskarray[5] );
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v14a::GetRegion(const Int_t detectorId)
{
  return -1;
}

Int_t CbmTofDetectorId_v14a::GetCellId(const Int_t detectorId)
{
  return (detectorId & modulemask);
}

//-----------------------------------------------------------

Int_t CbmTofDetectorId_v14a ::SetDetectorInfo(const CbmTofDetectorInfo detInfo)
{
  return ( (detInfo.fDetectorSystem << shiftarray[0]) | 
           (detInfo.fSMtype << shiftarray[2]) | 
           (detInfo.fSModule << shiftarray[1]) | 
           (detInfo.fCounter << shiftarray[3])  | 
           (detInfo.fGap << shiftarray[4])  |
           (detInfo.fCell << shiftarray[5])  
         ); 
}

