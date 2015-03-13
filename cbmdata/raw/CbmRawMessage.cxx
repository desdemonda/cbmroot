/** @file CbmRawMessage.cxx
 ** @author Florian Uhlig <f.uhlig@gsi.de>
 ** @date 15.08.2014
 **/

#include "CbmRawMessage.h"

// -----   Default constructor   -------------------------------------------
CbmRawMessage::CbmRawMessage() 
 : TObject(), 
   fEquipmentID(-1),
   fSourceAddress(-1),
   fChannelID(-1),
   fEpochMarker(-1),
   fTime(-1)
   
{
}

CbmRawMessage::CbmRawMessage(Int_t EquipmentID, Int_t SourceAddress, 
			     Int_t ChannelId,
			     Int_t EpochMarker, Int_t Time)
 : TObject(), 
   fEquipmentID(EquipmentID),
   fSourceAddress(SourceAddress),
   fChannelID(ChannelId),
   fEpochMarker(EpochMarker),
   fTime(Time)   
{
}


ClassImp(CbmRawMessage)
