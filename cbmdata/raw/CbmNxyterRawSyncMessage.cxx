#include "CbmNxyterRawSyncMessage.h"

// -----   Default constructor   -------------------------------------------
CbmNxyterRawSyncMessage::CbmNxyterRawSyncMessage()
  : CbmRawMessage(),
    fStFlag(0),
    fData(0)
{
}

// ------ Constructor -----------------------------------------------------
CbmNxyterRawSyncMessage::CbmNxyterRawSyncMessage(Int_t FlibLink, Int_t FebId, Int_t ChannelId, 
         Int_t EpochMarker, Int_t Time,
         Int_t StFlag, Int_t Data)
  : CbmRawMessage(FlibLink, FebId, ChannelId, EpochMarker, Time),
    fStFlag(StFlag),
    fData(Data)
{
}

// -------------------------------------------------------------------------

ClassImp(CbmNxyterRawSyncMessage)
