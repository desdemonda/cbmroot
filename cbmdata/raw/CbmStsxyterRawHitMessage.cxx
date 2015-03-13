#include "CbmStsxyterRawHitMessage.h"

CbmStsxyterRawHitMessage::CbmStsxyterRawHitMessage()
: CbmRawMessage(),
  fAdcValue(0),
  fParityError( kFALSE )
{
}

CbmStsxyterRawHitMessage::CbmStsxyterRawHitMessage(Int_t EquipmentID, Int_t SourceAdress, Int_t ChannelId,
        Int_t EpochMarker, Int_t Time, Int_t AdcValue, Bool_t ParityError)
: CbmRawMessage(EquipmentID, SourceAdress, ChannelId, EpochMarker, Time),
  fAdcValue( AdcValue ),
  fParityError( ParityError )
{
}

ClassImp(CbmStsxyterRawHitMessage)
