/** @file CbmSpadicRawMessage.cxx
 ** @author Florian Uhlig <f.uhlig@gsi.de>
 ** @date 15.08.2014
 **/

#include "CbmSpadicRawMessage.h"

#include "FairLogger.h"

// -----   Default constructor   -------------------------------------------
CbmSpadicRawMessage::CbmSpadicRawMessage() 
 : CbmRawMessage(), 
   fSuperEpoch(-1),  
   fTriggerType(-1),
   fInfoType(-1),
   fStopType(-1),
   fGroupId(-1),
   fBufferOverflowCount(-1),
   fNrSamples(-1),
   fSamples()
{
}

CbmSpadicRawMessage::CbmSpadicRawMessage(Int_t EquipmentID, Int_t SourceAddress, Int_t ChannelId,
					 Int_t EpochMarker, Int_t Time, 
					 Int_t SuperEpoch, Int_t TriggerType,
					 Int_t InfoType, Int_t StopType, 
					 Int_t GroupId, Int_t BufferOverflow, 
					 Int_t NrSamples, Int_t* Samples)
 : CbmRawMessage(EquipmentID, SourceAddress, ChannelId, EpochMarker, Time),
   fSuperEpoch(SuperEpoch),
   fTriggerType(TriggerType),
   fInfoType(InfoType),
   fStopType(StopType),
   fGroupId(GroupId),
   fBufferOverflowCount(BufferOverflow),
   fNrSamples(NrSamples),
   fSamples()
{
  for (Int_t i = 0; i < NrSamples; ++i) {
    fSamples[i] = Samples[i];
  }

}

// -------------------------------------------------------------------------

ULong_t CbmSpadicRawMessage::GetFullTime() 
{
  // Spadic time counter runs with 17.5 MHz => time between conters is
  // 1/17.5 MHz = 57,14 ns !!!
  // Spadic counter has 2^12 bits
  // Spadic epoch counter has also 2^12 bits 
  //         => Overflow after 2^24 * 57,14 ns = 0,95s
  // So we need to take care of this overflow. In the Spadic unpacker we
  // have introduced a "SuperEpoch" counter which is increased by one for
  // each overflow of the epoch counter. 
  // This works only reliable if we get the complete stream and are able 
  // to measure each of this overflows.
  // So the absolute time in ns is calculated by the folowing formular
  // time (ns) = (SuperEpoch * 2^24 + Epoch *2^12 + timecounter) * 1000/17.5 MHz
  // To speed up the calculation we use here some bit operators described 
  // below. The result should be the same for both calculations
  // 
  //  (number << 1) will shift all bits to the left by one bit. 
  //  This is a multiplication of  the number by the factor 2^1.  
  //  (number << 12) will shift all bits to the left by one bit. 
  //  This is a multiplication of  the number by the factor 2^12. 
  //  The lower 12 bits are after the shift set to zero.
  //  (number & 0xFFF) will set all bit above bit 12 to zero  
  //  (number1 | number2) will add both numbers bitwise 

  ULong_t result1 = ( ( static_cast<ULong_t>(fSuperEpoch) << 24) | 
		      ( static_cast<ULong_t>(fEpochMarker) << 12) | 
		      ( fTime & 0xfff )
		      );
  /*
  ULong_t result2 = fSuperEpoch * 4096 * 4096 + fEpochMarker *4096 + fTime;

  LOG(INFO) << "Time: " << result1 << " : " << result2 << FairLogger::endl;  
  */

  return result1;
}

ClassImp(CbmSpadicRawMessage)
