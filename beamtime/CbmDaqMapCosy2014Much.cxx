/**@file CbmDaqMapCosy2014Much.cxx
 **@date 16.12.2014
 **@author V. Friese <v.friese@gsi.de>
 **/

#include "CbmDaqMapCosy2014Much.h"

#include "FairLogger.h"
#include "CbmDetectorList.h"

#include "MuchRealSize.h"

#include <iostream>


using std::cout;
using std::endl;



// -----   Default Constructor  ---------------------------------------------- 
CbmDaqMapCosy2014Much::CbmDaqMapCosy2014Much()
  : CbmDaqMap(),
    fFiberHodoFiber(),
    fFiberHodoPlane(),
    fFiberHodoPixel()
{
  InitializeFiberHodoMapping();
}
// ---------------------------------------------------------------------------



// -----   Standard Constructor  --------------------------------------------- 
CbmDaqMapCosy2014Much::CbmDaqMapCosy2014Much(Int_t iRun)
  : CbmDaqMap(iRun),
    fFiberHodoFiber(),
    fFiberHodoPlane(),
    fFiberHodoPixel()
{
  InitializeFiberHodoMapping();
}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmDaqMapCosy2014Much::~CbmDaqMapCosy2014Much() { }
// ---------------------------------------------------------------------------


// -----   Get System   ------------------------------------------------------
Int_t CbmDaqMapCosy2014Much::GetSystemId(Int_t rocId)
{
  Int_t systemId = -1;
  
  // --- ROC 0 and 1 are MUCH
  if ( rocId >= 0  && rocId <= 2 ) systemId = kMUCH;
  // --- ROC 3 is HODO
  else if ( rocId == 3 ) systemId = kFHODO;
  // --- other ROC ids will be ignored
  else {
    //LOG(WARNING) << GetName() << ": Unknown ROC id " << rocId
		// << FairLogger::endl;
  }
  
  return systemId;
}
// ---------------------------------------------------------------------------


// -----   Get STS station number   ------------------------------------------
Int_t CbmDaqMapCosy2014Much::GetStsStation(Int_t rocId)
{
  if ( rocId == 2 || rocId == 3 ) {
    return 1;
  } else 	if ( rocId == 4 || rocId == 5 ) {
    return 0;
  } else 	if ( rocId == 6 || rocId == 7 ) {
    return 2;
  } else {
    LOG(ERROR) << GetName() << ": Illegal STS ROC Id " << rocId
	       << FairLogger::endl;
    return -1;
  }
}
// ---------------------------------------------------------------------------


// -----   Get MUCH station number  ------------------------------------------
Int_t CbmDaqMapCosy2014Much::GetMuchStation(Int_t rocId) {
	Int_t station = -1;
	if ( rocId == 0 || rocId == 1 ) station = 0;
	else if ( rocId == 2 ) station = 1;
	else if ( rocId == 3 || rocId == 4 ) station = 2;
	else LOG(ERROR) << GetName() << ": Illegal MUCH ROC Id " << rocId
			            << FairLogger::endl;
	return station;
}
// ---------------------------------------------------------------------------

// -----   Get Fiber Hodoscope station number   ------------------------------
Int_t CbmDaqMapCosy2014Much::GetFiberHodoStation(Int_t rocId) {
	if ( rocId < 0 || rocId > 1 ) {
		LOG(ERROR) << GetName() << ": Illegal Fiber Hodoscope ROC Id " << rocId
				       << FairLogger::endl;
		return -1;
	}
	return ( rocId );
}
// ---------------------------------------------------------------------------



// ----  Get STS sensor side   -----------------------------------------------
Int_t CbmDaqMapCosy2014Much::GetStsSensorSide(Int_t rocId) {
	return ( TMath::Even(rocId) ? 1 : 0 );
}
// ---------------------------------------------------------------------------

// -----   Get STS channel number   ------------------------------------------
Int_t CbmDaqMapCosy2014Much::GetStsChannel(Int_t rocId, Int_t nxId, Int_t nxChannel) {

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
  
  if (rocId == 6) { //sts2 p-side
    if (nxId == 0) channel = 128 - ((nxChannel < 64) ? (nxChannel + 2 * ((nxChannel + 1) % 2) - 1) : nxChannel);
  }
  
  if (rocId == 7) { //sts2 n-side
    if (nxId == 0) channel = 128 - ((nxChannel < 64) ? (127 - nxChannel) : (- nxChannel + 126 + 2 * (nxChannel % 2)));
  }
  
  return channel;
}
// ---------------------------------------------------------------------------


// -----   Get STS channel number   ------------------------------------------
Int_t CbmDaqMapCosy2014Much::GetFiberHodoChannel(Int_t rocId, Int_t nxId, Int_t nxChannel) {
	Int_t channel = -1;
	return channel;
}
// ---------------------------------------------------------------------------



// -----   Mapping   ---------------------------------------------------------
Bool_t CbmDaqMapCosy2014Much::Map(Int_t iRoc, Int_t iNx, Int_t iId,
			Int_t& iStation, Int_t& iSector, 
			Int_t& iSide, Int_t& iChannel) {

  // --- ROC 3: Hodoscope
  if ( 3 == iRoc  ) {
    if ( iNx-2 == 0) {
      iStation = 1;
      iSector = 0;
      iSide = fFiberHodoPlane[iId];
      iChannel = fFiberHodoFiber[iId];
      return kTRUE;
    } else {
      LOG(FATAL) << GetName() << ": Unknown Nxyter Id " << iNx
		 << " for Roc Id " << iRoc << FairLogger::endl;
    }
  	LOG(INFO) << "Map: HODO station " << iStation << " sector " << iSector
  			      << " side " << iSide << " channel " << iChannel << FairLogger::endl;
  }

    // -- ROC 0 to 1: MUCH Station
  else if ( iRoc == 0 || iRoc == 1 ) {

  	// Get FEB number
  	Int_t febId = GetFebId(iRoc, iNx);
  	if ( febId < 0 || febId > 14 ) {
  		LOG(FATAL) << "Illegal FEB ID " << febId << "( ROC " << iRoc
  				<< ", NXYTER " << iNx << ")" << FairLogger::endl;
  		return kFALSE;
  	}

  	// Get pad x coordinate
  	Int_t xPad = xl[febId][iId];

  	// Get pad y coordinate
  	Int_t yPad = yl[febId][iId];

  	// Return values
  	iStation = 0;
  	iSector  = 0;
  	iSide    = xPad;
  	iChannel = yPad;


  	return kTRUE;

  }

  return kTRUE;
}
// ---------------------------------------------------------------------------


// --- Get FEB ID  -----------------------------------------------------------
Int_t CbmDaqMapCosy2014Much::GetFebId(Int_t rocId, Int_t nxyterId) {

	if ( rocId == 0 && nxyterId == 0) return 0;
	if ( rocId == 0 && nxyterId == 2) return 1;
	if ( rocId == 1 && nxyterId == 0) return 4;
	if ( rocId == 1 && nxyterId == 2) return 2;

	return -1;

}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
void CbmDaqMapCosy2014Much::InitializeFiberHodoMapping()
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

ClassImp(CbmDaqMapCosy2014Much)

