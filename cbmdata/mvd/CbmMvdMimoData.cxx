/*
  File: CbmMvdMimoData.cxx

*/

#include "CbmMvdMimoData.h"
#include <iostream>

using std::cout;
using std::endl;

CbmMvdMimoData::CbmMvdMimoData()
{
  fROCFrame = -1;
  fTimeStamp = -1;
  fSensorOffset = -1;
  fTemp = -1;
  fROCAddress = -1; 
  fStateNum = -1;
  
  for(Int_t i=0;i<fByteNumberOfData;i++)
    fMimoData[i] = 0;
  
 
}


CbmMvdMimoData::CbmMvdMimoData( Int_t ROCFrame,Long64_t TimeStamp,Long64_t SensorOffset,Short_t Temp,Short_t ROCAddress,unsigned char* mimodata,Int_t stateNum)
{
  fROCFrame = ROCFrame;
  fTimeStamp = TimeStamp;
  fSensorOffset = SensorOffset;
  fTemp = Temp;
  fROCAddress = ROCAddress; 
  fStateNum = stateNum;
  
  for(Int_t i=0;i<fByteNumberOfData;i++)
    fMimoData[i] = mimodata[i];
}

/** get the usfuldata from the fMimoData[] based on the NumUsefulData**/
void CbmMvdMimoData::GetState( vector< unsigned char >& state)
{
  
  Int_t NumUsefulData = GetDatalength()*2;
  state.clear();
  for(Int_t i=0;i<NumUsefulData;i++){
    unsigned char dat = fMimoData[i+12];
    state.push_back(dat);
  }

}

CbmMvdMimoData::~CbmMvdMimoData()
{
}

ClassImp(CbmMvdMimoData)


