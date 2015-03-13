#include "CbmTrbRawMessage.h"

// -----   Default constructor   -------------------------------------------
CbmTrbRawMessage::CbmTrbRawMessage() 
  : CbmRawMessage(),
    fTDCfine(0),
    fEdge(0)
{
}

// ------ Constructor -----------------------------------------------------
CbmTrbRawMessage::CbmTrbRawMessage(Int_t FlibLink, Int_t FebId, 
				   Int_t ChannelId, 
				   Int_t EpochMarker, Int_t CoarseTime,
				   Int_t FineTime, Bool_t Edge)
  : CbmRawMessage(FlibLink, FebId, ChannelId, EpochMarker, CoarseTime),
    fTDCfine(FineTime),
    fEdge(Edge)
{
}

// -------------------------------------------------------------------------

ULong_t CbmTrbRawMessage::GetFullTime() 
{
  return 0L;
}  
/*
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
*/

ClassImp(CbmTrbRawMessage)
