/*
  File: CbmMvdMimoStateMatch.cxx

*/

#include "CbmMvdMimoStateMatch.h"
#include <iostream>

using std::cout;
using std::endl;

CbmMvdMimoStateMatch::CbmMvdMimoStateMatch()
{
  fStateInformation.clear();
  fROCFrame = -1;
  fTimeStamp = -1;
  fSensorOffset = -1;
  fTemp = -1;
  fROCAddress = -1; 
  fStateNum = -1;
}


CbmMvdMimoStateMatch::CbmMvdMimoStateMatch(Int_t ROCFrame,Long64_t TimeStamp,Long64_t SensorOffset,Short_t Temp,Short_t ROCAddress,vector< vector< Int_t > >& stateInformation)
{
  fROCFrame = ROCFrame;
  fTimeStamp = TimeStamp;
  fSensorOffset = SensorOffset;
  fTemp = Temp;
  fROCAddress = ROCAddress; 
  fStateNum = stateInformation.size();
  fStateInformation = stateInformation;

}


void CbmMvdMimoStateMatch::GetStateDigis(Int_t indexOfState,vector< Int_t >& stateDigis)
{

  if(indexOfState > fStateNum)
    cout<<"the state num is out of the range in this StateMatch!!!"<<endl;
  else
  {
    stateDigis.clear();
    for(Int_t i=0;i<fStateInformation[indexOfState].size();i++)
      stateDigis.push_back(fStateInformation[indexOfState][i]);

  }

}




CbmMvdMimoStateMatch::~CbmMvdMimoStateMatch()
{
}

ClassImp(CbmMvdMimoStateMatch)


