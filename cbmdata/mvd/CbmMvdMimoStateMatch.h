/*
  File: CbmMvdMimoStateMatch.h

*/

#ifndef CbmMvdMimoStateMatch_H
#define CbmMvdMimoStateMatch_H 1

#include "TObject.h"
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using namespace std;

class CbmMvdMimoStateMatch : public TObject
{
  public:
    
    /** Default constructor**/
    CbmMvdMimoStateMatch();

    /** Constructor with all variables **/
    CbmMvdMimoStateMatch(Int_t ROCFrame,Long64_t TimeStamp,Long64_t SensorOffset,Short_t Temp,Short_t ROCAddress,vector< vector< Int_t > >& stateInformation);
    
  // destructor
   ~CbmMvdMimoStateMatch();

   
  /** accesors*/
  Int_t GetStateNums(){return fStateNum;};
  Int_t    GetROCFrame()           {return fROCFrame;};
  Long64_t    GetTimeStamp()          {return fTimeStamp;};
  Double_t GetTimeStampSI()        {return fTimeStamp;};
  Long64_t    GetSensorOffset()       {return fSensorOffset;};
  Double_t GetSensorOffsetSI()     {return fSensorOffset;};
  Short_t    GetTemp()               {return fTemp;};
  Short_t    GetTempSI()             {return fTemp;};
  Short_t    GetROCAddress()         {return fROCAddress;};
 
  void  GetStateDigis(Int_t indexOfState,vector< Int_t >& stateDigis);
  
  //store the data into the fStateInformation
private:
  
    vector< vector< Int_t > > fStateInformation;
    vector< Int_t > fOneState;
    Int_t fStateNum;
//     Int_t fStationID;
//     Int_t fSensorID;
//     Int_t fFrameID;
    
   Int_t fROCFrame;//32bits
   Long64_t fTimeStamp;
   Long64_t fSensorOffset;
   Short_t fTemp;//16bit
   Short_t fROCAddress;

  
  ClassDef(CbmMvdMimoStateMatch,1);
};
#endif