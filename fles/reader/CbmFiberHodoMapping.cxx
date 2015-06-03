#include "CbmFiberHodoMapping.h"

#include "CbmBeamDefaults.h"

#include "FairLogger.h"

CbmFiberHodoMapping::CbmFiberHodoMapping()
  :TObject(),
   fFiberHodoFiber(),
   fFiberHodoPlane(),
   fFiberHodoPixel()
{
  InitializeFiberHodoMapping();
}

CbmFiberHodoMapping::~CbmFiberHodoMapping()
{
}

// -----   Get Fiber Hodoscope station number   ------------------------------
Int_t CbmFiberHodoMapping::GetFiberHodoStation(Int_t rocId) {

  return rocId;

  if ( rocId < 5 || rocId > 6 ) {
    LOG(ERROR) << GetName() << ": Illegal Fiber Hodoscope ROC Id " << rocId
	       << FairLogger::endl;
    return -1;
  }
  return ( rocId - 5 );
}
// ---------------------------------------------------------------------------

Int_t CbmFiberHodoMapping::GetFiberHodoChannel(Int_t /*rocId*/, Int_t /*nxId*/, Int_t /*nxChannel*/) {
	Int_t channel = -1;
	return channel;
}


// -----   Mapping   ---------------------------------------------------------
Bool_t CbmFiberHodoMapping::Map(Int_t eqId, Int_t /*febId*/, Int_t channel,
				Int_t& iStation, Int_t& iSector, 
				Int_t& iSide, Int_t& iChannel) {

  iStation = eqId - kHodo1;
  iSector = 0;
  iSide = fFiberHodoPlane[channel];
  iChannel = fFiberHodoFiber[channel];
  return kTRUE;

}
// ---------------------------------------------------------------------------

void CbmFiberHodoMapping::InitializeFiberHodoMapping()
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

ClassImp(CbmFiberHodoMapping)
