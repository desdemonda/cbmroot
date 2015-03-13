/**@file CbmDaqMapCosy2014.cxx
 **@date 11.12.2013
 **@author V. Friese <v.friese@gsi.de>
 **/

#include "CbmDaqMapCosy2014.h"

#include "FairLogger.h"
#include "CbmDetectorList.h"

#include <iostream>


using std::cout;
using std::endl;



// -----   Default Constructor  ---------------------------------------------- 
CbmDaqMapCosy2014::CbmDaqMapCosy2014() 
  : CbmDaqMap(),
    fFiberHodoFiber(),
    fFiberHodoPlane(),
    fFiberHodoPixel(),
    fRocToSystem(),
    fRocToStsStation(),
    fRocToHodoStation(),
    fStsStrip()
{
  InitializeFiberHodoMapping();
  InitializeRocToSystemArray();
  InitializeRocToStsStation();
  InitializeRocToHodoStation();
  InitializeStsMapping(); 
}
// ---------------------------------------------------------------------------



// -----   Standard Constructor  --------------------------------------------- 
CbmDaqMapCosy2014::CbmDaqMapCosy2014(Int_t iRun) 
  : CbmDaqMap(iRun),
    fFiberHodoFiber(),
    fFiberHodoPlane(),
    fFiberHodoPixel(),
    fRocToSystem(),
    fRocToStsStation(),
    fRocToHodoStation(),
    fStsStrip()
{
  InitializeFiberHodoMapping();
  InitializeRocToSystemArray();
  InitializeRocToStsStation();
  InitializeRocToHodoStation();
  InitializeStsMapping(); 
}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmDaqMapCosy2014::~CbmDaqMapCosy2014() { }
// ---------------------------------------------------------------------------


// -----   Get System   ------------------------------------------------------
Int_t CbmDaqMapCosy2014::GetSystemId(Int_t rocId) 
{

  if ( rocId < 0  || rocId >= fMaxNumRoc ) {
    LOG(WARNING) << GetName() << ": Unknown ROC id " << rocId
		 << FairLogger::endl;
    return -1;
  } else {
    return fRocToSystem[rocId];
  }
}
// ---------------------------------------------------------------------------


// -----   Get STS station number   ------------------------------------------
Int_t CbmDaqMapCosy2014::GetStsStation(Int_t rocId) 
{
  Int_t station =  fRocToStsStation[rocId];

  if ( -1 == station) {
    LOG(ERROR) << GetName() << ": Illegal STS ROC Id " << rocId
	       << FairLogger::endl;
  } 

  return station;
}
// ---------------------------------------------------------------------------


// -----   Get MUCH station number  ------------------------------------------
Int_t CbmDaqMapCosy2014::GetMuchStation(Int_t rocId) 
{
  // No MUCH in the STS setup 
  Int_t station = -1;
  return station;
}
// ---------------------------------------------------------------------------

// -----   Get Fiber Hodoscope station number   ------------------------------
Int_t CbmDaqMapCosy2014::GetFiberHodoStation(Int_t rocId) 
{ 
  Int_t station =  fRocToHodoStation[rocId];

  if ( -1 == station) {
    LOG(ERROR) << GetName() << ": Illegal Hodo ROC Id " << rocId
	       << FairLogger::endl;
  } 

  return station;
}
// ---------------------------------------------------------------------------



// ----  Get STS sensor side   -----------------------------------------------
Int_t CbmDaqMapCosy2014::GetStsSensorSide(Int_t rocId) {
	return ( TMath::Even(rocId) ? 1 : 0 );
}
// ---------------------------------------------------------------------------

// -----   Get STS channel number   ------------------------------------------
Int_t CbmDaqMapCosy2014::GetStsChannel(Int_t rocId, Int_t nxId, Int_t nxChannel) {

  Int_t channel = -1;
  
  if (rocId == 2){ //sts1 p-side
    if (nxId == 0) channel = 256 - ((nxChannel < 64) ? (2 * nxChannel - 4 * (nxChannel % 2) + 3) : (2 * nxChannel + 1)); // even
    if (nxId == 2) channel = 256 - ((nxChannel < 64) ? (252 - 2 * nxChannel + 4 * (nxChannel % 2)) : (254 -2 * nxChannel)); // odd
  }
  
  if (rocId == 3){ //sts1 n-side
    if (nxId == 0) channel = 256 - ((nxChannel < 64) ? (253 - 2 * nxChannel + 4 * (nxChannel % 2)) : (255 - 2 * nxChannel)); // even
    if (nxId == 2) channel = 256 - ((nxChannel < 64) ? (2 * nxChannel + 2 - 4 * (nxChannel % 2)) : (2 * nxChannel)); // odd
  }
  
  if (rocId == 4 ){ //sts0  p-side
    if (nxId == 0) channel = ((nxChannel < 64) ? (2 * nxChannel - 4 * (nxChannel % 2) + 3) : (2 * nxChannel + 1)); // even
    if (nxId == 2) channel = ((nxChannel < 64) ? (252 - 2 * nxChannel + 4 * (nxChannel % 2)) : (254 -2 * nxChannel)); // odd
  }
  
  if (rocId == 5 ){ //sts0 n-side
    if (nxId == 0) channel = ((nxChannel < 64) ? (253 - 2 * nxChannel + 4 * (nxChannel % 2)) : (255 - 2 * nxChannel)); // even
    if (nxId == 2) channel = ((nxChannel < 64) ? (2 * nxChannel + 2 - 4 * (nxChannel % 2)) : (2 * nxChannel)); // odd
  }
  

  // station which was in until run53 

  if (rocId == 6) { //sts2 p-side
    if (nxId == 0) channel = 128 - ((nxChannel < 64) ? (nxChannel + 2 * ((nxChannel + 1) % 2) - 1) : nxChannel);
  }
  
  if (rocId == 7) { //sts2 n-side
    if (nxId == 0) channel = 128 - ((nxChannel < 64) ? (127 - nxChannel) : (- nxChannel + 126 + 2 * (nxChannel % 2)));
  }

  //station which is in since run54
  
  if (rocId == 6) { //sts2 p-side
    if (nxId == 0) channel = nxChannel;
  }
  
  if (rocId == 7) { //sts2 n-side
    if (nxId == 0) channel = nxChannel;
  }
  
  return channel;
}
// ---------------------------------------------------------------------------


// -----   Get STS channel number   ------------------------------------------
Int_t CbmDaqMapCosy2014::GetFiberHodoChannel(Int_t rocId, Int_t nxId, Int_t nxChannel) {
	Int_t channel = -1;
	return channel;
}
// ---------------------------------------------------------------------------



// -----   Mapping   ---------------------------------------------------------
Bool_t CbmDaqMapCosy2014::Map(Int_t iRoc, Int_t iNx, Int_t iId,
			Int_t& iStation, Int_t& iSector, 
			Int_t& iSide, Int_t& iChannel) 
{
  // --- Valid ROC Id
  if ( iRoc < 0  || iRoc >= fMaxNumRoc ) {
    LOG(FATAL) << GetName() << ": Invalid ROC id " << iRoc
	       << FairLogger::endl;
  }
  
  // --- ROC 0 to 1: Hodoscope
  if ( 0 == iRoc  ) {
    if ( iNx == 0) {
      iStation = iRoc;
      iSector = 0;
      iSide = fFiberHodoPlane[iId];
      iChannel = fFiberHodoFiber[iId];
      return kTRUE;
    } else {
      LOG(FATAL) << GetName() << ": Unknown Nxyter Id " << iNx
		 << " for Roc Id " << iRoc << FairLogger::endl;
    }
  } else if ( 1 == iRoc ) {
    if ( iNx-2 == 0) {
      iStation = iRoc;
      iSector = 0;
      iSide = fFiberHodoPlane[iId];
      iChannel = fFiberHodoFiber[iId];
      return kTRUE;
    } else {
      LOG(FATAL) << GetName() << ": Unknown Nxyter Id " << iNx
		 << " for Roc Id " << iRoc << FairLogger::endl;

    }
    // ROC 2 to 7: Sts
  } else {
    if ( iNx == 2 )  iNx = 1; // there is no nxyter 1, but there is a array index 1
    iStation = fRocToStsStation[iRoc];
    iSector = 0;
    iSide = GetStsSensorSide(iRoc);
    Int_t arrayIndex= iStation*2 + iRoc%2;
    iChannel = fStsStrip[arrayIndex][iNx][iId];
    return kTRUE;
  }
 
  return kTRUE;
}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
void CbmDaqMapCosy2014::InitializeFiberHodoMapping()
{
  // This code was copied from the Go4 analysis used for previous beamtimes
  for (Int_t i=0; i<128; i++) {
    fFiberHodoFiber[i] = -1;
    fFiberHodoPlane[i] = -1;
    fFiberHodoPixel[i] = -1;
  }
  
  for (Int_t ifiber=1; ifiber<=64; ifiber++) {
    // Calculate fiber number [1..64] from feb channel
    // lcn: linearconnectornumber, is the wire number on one of the
    // flat cables. [1..16]
    // each 16 fibers go to one connector.
    // fibersubnr[0..15] linear fiber counter in groups of 16

    Int_t fibersubnr=(ifiber-1)%16;

    Int_t lcn=15-fibersubnr*2;
    if (fibersubnr>=8) lcn=(fibersubnr-7)*2;
    
    Int_t channel=-1;
    Int_t cable=(ifiber-1)/16+1;
    Int_t pixel= ((lcn-1)/2)*8 +((lcn-1)%2);
    if (cable==1) {
      channel=(lcn-1)*4+0;
      pixel=pixel+1;
    }
    if (cable==2) {
      channel=(lcn-1)*4+2;
      pixel=pixel+3;
    }
    if (cable==3) {
      channel=(lcn-1)*4+1;
      pixel=pixel+5;
    }
    if (cable==4) {
      channel=(lcn-1)*4+3;
      pixel=pixel+7;
    }
    
    // new code to resolve cabling problem during cern-oct12
    int ifiber_bis = ifiber;
    if (ifiber <= 8 )  ifiber_bis = ifiber + 56; else
      if (ifiber <= 16 ) ifiber_bis = ifiber + 40; else
	if (ifiber <= 24 ) ifiber_bis = ifiber + 24; else
	  if (ifiber <= 32 ) ifiber_bis = ifiber + 8; else 
	    if (ifiber <= 40 ) ifiber_bis = ifiber - 8; else 
	      if (ifiber <= 48 ) ifiber_bis = ifiber - 24; else
		if (ifiber <= 56 ) ifiber_bis = ifiber - 40; else
		  if (ifiber <= 64 ) ifiber_bis = ifiber - 56;
    
    // and swap at the end
    ifiber_bis = 65 - ifiber_bis;

    fFiberHodoFiber[channel] = ifiber_bis - 1;
    fFiberHodoPlane[channel] = 0;
    fFiberHodoPixel[channel] = pixel;

    fFiberHodoFiber[channel+64] = ifiber_bis - 1;
    fFiberHodoPlane[channel+64] = 1;
    fFiberHodoPixel[channel+64] = pixel;

  }

  for (Int_t i=0; i<128; i++) {
    LOG(DEBUG) << "Channel[" << i << "]: " << fFiberHodoFiber[i] << ", " 
              << fFiberHodoPlane[i] << ", " << fFiberHodoPixel[i] 
              << FairLogger::endl;
  }

}

void CbmDaqMapCosy2014::InitializeRocToSystemArray() 
{
  fRocToSystem[0] = kFHODO;
  fRocToSystem[1] = kFHODO;
  fRocToSystem[2] = kSTS;
  fRocToSystem[3] = kSTS;
  fRocToSystem[4] = kSTS;
  fRocToSystem[5] = kSTS;
  fRocToSystem[6] = kSTS;
  fRocToSystem[7] = kSTS;
}

void CbmDaqMapCosy2014::InitializeRocToStsStation() 
{
  fRocToStsStation[0] = -1;
  fRocToStsStation[1] = -1;
  fRocToStsStation[2] = 1;
  fRocToStsStation[3] = 1;
  fRocToStsStation[4] = 0;
  fRocToStsStation[5] = 0;
  fRocToStsStation[6] = 2;
  fRocToStsStation[7] = 2;
}

void CbmDaqMapCosy2014::InitializeRocToHodoStation() 
{
  fRocToHodoStation[0] = 0;
  fRocToHodoStation[1] = 1;
  fRocToHodoStation[2] = -1;
  fRocToHodoStation[3] = -1;
  fRocToHodoStation[4] = -1;
  fRocToHodoStation[5] = -1;
  fRocToHodoStation[6] = -1;
  fRocToHodoStation[7] = -1;
}

void CbmDaqMapCosy2014::InitializeStsMapping()
{ 
  
  Int_t arrayIndex;
  Int_t evenChannel;
  Int_t oddChannel;
  
  // Fill Array for RocId 2
  // RocId 2 belongs to the first Roc of station 1
  // so it must be the third in the array  
  // sts1 p-side
  arrayIndex = 2;
  for(Int_t nxChannel = 0; nxChannel < 128; ++nxChannel) {
    evenChannel = 256 - ((nxChannel < 64) ? (2 * nxChannel - 4 * (nxChannel % 2) + 3) : (2 * nxChannel + 1)); // even
    oddChannel =   256 - ((nxChannel < 64) ? (252 - 2 * nxChannel + 4 * (nxChannel % 2)) : (254 -2 * nxChannel)); // odd
  
    fStsStrip[arrayIndex][0][nxChannel] = evenChannel;
    fStsStrip[arrayIndex][1][nxChannel] = oddChannel;
  }

  // Fill Array for RocId 3
  // RocId 3 belongs to the second Roc of station 1
  // so it must be the third in the array  
  // sts1 n-side
  arrayIndex = 3;
  for(Int_t nxChannel = 0; nxChannel < 128; ++nxChannel) {
     evenChannel = 256 - ((nxChannel < 64) ? (253 - 2 * nxChannel + 4 * (nxChannel % 2)) : (255 - 2 * nxChannel)); // even
     oddChannel = 256 - ((nxChannel < 64) ? (2 * nxChannel + 2 - 4 * (nxChannel % 2)) : (2 * nxChannel)); // odd
    fStsStrip[arrayIndex][0][nxChannel] = evenChannel;
    fStsStrip[arrayIndex][1][nxChannel] = oddChannel;
  }

  // Fill Array for RocId 4
  // RocId 4 belongs to the first Roc of station 0
  // so it must be the first in the array  
  // sts0 p-side
  arrayIndex = 0;
  for(Int_t nxChannel = 0; nxChannel < 128; ++nxChannel) {
    evenChannel = ((nxChannel < 64) ? (2 * nxChannel - 4 * (nxChannel % 2) + 3) : (2 * nxChannel + 1)); // even
    oddChannel = ((nxChannel < 64) ? (252 - 2 * nxChannel + 4 * (nxChannel % 2)) : (254 -2 * nxChannel)); // odd
    fStsStrip[arrayIndex][0][nxChannel] = evenChannel;
    fStsStrip[arrayIndex][1][nxChannel] = oddChannel;
  }

  // Fill Array for RocId 5
  // RocId 5 belongs to the second Roc of station 0
  // so it must be the second in the array  
  // sts0 n-side
  arrayIndex = 1;
  for(Int_t nxChannel = 0; nxChannel < 128; ++nxChannel) {
    evenChannel = ((nxChannel < 64) ? (253 - 2 * nxChannel + 4 * (nxChannel % 2)) : (255 - 2 * nxChannel)); // even
    oddChannel = ((nxChannel < 64) ? (2 * nxChannel + 2 - 4 * (nxChannel % 2)) : (2 * nxChannel)); // odd
    fStsStrip[arrayIndex][0][nxChannel] = evenChannel;
    fStsStrip[arrayIndex][1][nxChannel] = oddChannel;
  }

  if ( fRun < 54 ) {
    // Fill Array for RocId 6
    // RocId 4 belongs to the first Roc of station 2
    // so it must be the forth in the array  
    // sts0 p-side
    arrayIndex = 4;
    for(Int_t nxChannel = 0; nxChannel < 128; ++nxChannel) {
      evenChannel = 128 - ((nxChannel < 64) ? (nxChannel + 2 * ((nxChannel + 1) % 2) - 1) : nxChannel);
      fStsStrip[arrayIndex][0][nxChannel] = evenChannel;
      fStsStrip[arrayIndex][1][nxChannel] = -1;
    }

    // Fill Array for RocId 7
    // RocId 5 belongs to the second Roc of station 2
    // so it must be the fifth in the array  
    // sts0 n-side
    arrayIndex = 5;
    for(Int_t nxChannel = 0; nxChannel < 128; ++nxChannel) {
      evenChannel = 128 - ((nxChannel < 64) ? (127 - nxChannel) : (- nxChannel + 126 + 2 * (nxChannel % 2)));
      fStsStrip[arrayIndex][0][nxChannel] = evenChannel;
      fStsStrip[arrayIndex][1][nxChannel] = -1;
    }
    
  } else { 
    // Fill Array for RocId 6
    // RocId 4 belongs to the first Roc of station 2
    // so it must be the forth in the array  
    // sts0 p-side
    arrayIndex = 4;
    for(Int_t nxChannel = 0; nxChannel < 128; ++nxChannel) {
      fStsStrip[arrayIndex][0][nxChannel] = nxChannel;
      fStsStrip[arrayIndex][1][nxChannel] = -1;
    }

    // Fill Array for RocId 7
    // RocId 5 belongs to the second Roc of station 2
    // so it must be the fifth in the array  
    // sts0 n-side
    arrayIndex = 5;
    for(Int_t nxChannel = 0; nxChannel < 128; ++nxChannel) {
      fStsStrip[arrayIndex][0][nxChannel] = nxChannel;
      fStsStrip[arrayIndex][1][nxChannel] = -1;
    }

  }
  

}

ClassImp(CbmDaqMapCosy2014)

