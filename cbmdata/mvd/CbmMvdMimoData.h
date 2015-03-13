/*
  File: CbmMvdMimoData.h

*/

#ifndef CbmMvdMimoData_H
#define CbmMvdMimoData_H 1

#include "TObject.h"
#include <vector>
#include <iostream>
//#include "FairTimeStamp.h"

using std::cout;
using std::endl;
using namespace std;

class CbmMvdMimoData : public TObject
{
  public:
    
    /** Default constructor**/
    CbmMvdMimoData();

    /** Constructor with all variables **/
    CbmMvdMimoData(Int_t ROCFrame,Long64_t TimeStamp,Long64_t SensorOffset,Short_t Temp,Short_t ROCAddress,unsigned char* mimodata,Int_t stateNum);
  
  // destructor
   ~CbmMvdMimoData();
 // vector< Int_t > vectorPoint;

  /** accesors**/
  //get the board parameters
  Int_t    GetROCFrame()           {return fROCFrame;};
  Long64_t    GetTimeStamp()          {return fTimeStamp;};
  Double_t GetTimeStampSI()        {return fTimeStamp;};
  Long64_t    GetSensorOffset()       {return fSensorOffset;};
  Double_t GetSensorOffsetSI()     {return fSensorOffset;};
  Short_t    GetTemp()               {return fTemp;};
  Short_t    GetTempSI()             {return fTemp;};
  Short_t    GetROCAddress()         {return fROCAddress;};
  Short_t    GetStateNum()           {return fStateNum;};
  
  //get the mimosaData parameters
  Int_t GetHeader()		{return (Int_t)( fMimoData[fWidthOfData-1]*16777216 + fMimoData[fWidthOfData-2]*65536 + fMimoData[fWidthOfData-3]*256 + fMimoData[fWidthOfData-4]);};
  Int_t GetFrameID()	        {return (Int_t)( fMimoData[2*fWidthOfData-1]*16777216 + fMimoData[2*fWidthOfData-2]*65536 + fMimoData[2*fWidthOfData-3]*256 + fMimoData[2*fWidthOfData-4]);};
  Int_t GetDatalength()		{return (Int_t)( fMimoData[3*fWidthOfData-1]*16777216 + fMimoData[3*fWidthOfData-2]*65536 + fMimoData[3*fWidthOfData-3]*256 + fMimoData[3*fWidthOfData-4]);};
  Int_t GetTrailer()		{return (Int_t)( fMimoData[fByteNumberOfData-1]*16777216 + fMimoData[fByteNumberOfData-2]*65536 + fMimoData[fByteNumberOfData-3]*256 + fMimoData[fByteNumberOfData-4]);};
 
  void GetState( vector< unsigned char >& state);//only read the useful data from fMimoData into the vector state[]
 
   /** modifier **/
  //header and framecounter are 32-bits value
  void SetHeader(Int_t header)			{fMimoData[fWidthOfData-4]=header%255;fMimoData[fWidthOfData-3]=(header>>8)%255;fMimoData[fWidthOfData-2]=(header>>16)%255;fMimoData[fWidthOfData-1]=(header>>24);};
  void SetFrameCounter(Int_t framecounter)	{fMimoData[2*fWidthOfData-4]=framecounter%255;fMimoData[2*fWidthOfData-3]=(framecounter>>8)%255;fMimoData[2*fWidthOfData-2]=(framecounter>>16)%255;fMimoData[2*fWidthOfData-1]=(framecounter>>24);};
  void SetDatalength(Int_t datalength)		{fMimoData[3*fWidthOfData-4]=datalength%255;fMimoData[3*fWidthOfData-3]=(datalength>>8)%255;fMimoData[3*fWidthOfData-2]=(datalength>>16)%255;fMimoData[3*fWidthOfData-1]=(datalength>>24);};
  void SetTrailer(Int_t trailer)		{fMimoData[fByteNumberOfData-4]=trailer%255;fMimoData[fByteNumberOfData-3]=(trailer>>8)%255;fMimoData[fByteNumberOfData-2]=(trailer>>16)%255;fMimoData[fByteNumberOfData-1]=(trailer>>24);}
  
  
  private:
    
    static const Int_t fByteNumberOfData = 570*2;//570*16bit
    static const Int_t fWidthOfData = 4;
    
   Int_t fROCFrame;//32bits
   Long64_t fTimeStamp;
   Long64_t fSensorOffset;
   Short_t fTemp;//16bit
   Short_t fROCAddress;
   Int_t fStateNum;
 
    unsigned char fMimoData[fByteNumberOfData];
  
  
      
ClassDef(CbmMvdMimoData,1);
};
#endif