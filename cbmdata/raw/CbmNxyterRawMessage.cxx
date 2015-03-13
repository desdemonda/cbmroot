#include "CbmNxyterRawMessage.h"

// -----   Default constructor   -------------------------------------------
CbmNxyterRawMessage::CbmNxyterRawMessage() 
  : CbmRawMessage(),
    fADCvalue(0),
    fLastEpoch(kFALSE),
    fPileUp(kFALSE),
    fOverflow(kFALSE)
{
}

// ------ Constructor -----------------------------------------------------
CbmNxyterRawMessage::CbmNxyterRawMessage(Int_t FlibLink, Int_t FebId, Int_t ChannelId,
                                         Int_t EpochMarker, Int_t Time,
                                         Int_t ADCval, Bool_t lastEpoch, Bool_t pileUp, Bool_t overflow)
  
  : CbmRawMessage(FlibLink, FebId, ChannelId, EpochMarker, Time),
    fADCvalue(ADCval),
    fLastEpoch(lastEpoch),
    fPileUp(pileUp),
    fOverflow(overflow)
{
}

// -------------------------------------------------------------------------

ULong_t CbmNxyterRawMessage::GetFullTime() {
  
  Int_t epoch;
  if (fLastEpoch) {
    epoch = fEpochMarker - 1; 
  } else {
    epoch = fEpochMarker;
  }
  
  ULong_t result = ( ( static_cast<ULong_t>(epoch) << 14 ) | 
		     ( fTime & 0x3fff )
		     );

  return result;

}

ClassImp(CbmNxyterRawMessage)
