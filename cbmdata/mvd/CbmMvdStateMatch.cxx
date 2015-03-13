/*
  File: CbmMvdStateMatch.cxx

*/

#include "CbmMvdStateMatch.h"
#include <iostream>

using std::cout;
using std::endl;

CbmMvdStateMatch::CbmMvdStateMatch()
{
  fStationID = -1;
  fSensorID = -1;
  for(Int_t i=0;i<fByteNumberOfData;i++)
    fMimoData[i] = 0;
  
 
}


CbmMvdStateMatch::CbmMvdStateMatch(unsigned char* mimodata)

{
  fStationID = stationId;
  fSensorID = sensorId;
  for(Int_t i=0;i<fByteNumberOfData;i++)
    fMimoData[i] = mimodata[i];
}

/** get the usfuldata from the fMimoData[] based on the NumUsefulData**/
void CbmMvdStateMatch::GetState( vector< unsigned char >& state)
{
  
  Int_t NumUsefulData = GetDatalength()*2;
  state.clear();
  for(Int_t i=0;i<NumUsefulData;i++){
    unsigned char dat = fMimoData[i+12];
    state.push_back(dat);
  }

}

CbmMvdStateMatch::~CbmMvdStateMatch()
{
}

ClassImp(CbmMvdStateMatch)


