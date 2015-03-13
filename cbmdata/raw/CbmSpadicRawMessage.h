/** @file CbmSpadicRawMessage.h
 ** @author Florian Uhlig <f.uhlig@gsi.de>
 ** @date 15.08.2014
 **/


#ifndef CBMSPADICRAWMESSAGE_H
#define CBMSPADICRAWMESSAGE_H 1


#include "CbmRawMessage.h"


/** @class CbmSpadicRawMessage
 ** @brief Class for representation of raw spadic information, derrived from CbmRawMessage.
 ** @author Florian Uhlig <f.uhlig@gsi.de>
 ** @version 1.0
 **
 ** CbmSpadicRawMessage is a base class for the ROOT representation of
 ** the smallest information unit delivered by the spadic front-ends.
 ** The CbmSpadicRawMessage extends the CbmRawMessage by the spadic sample data and the number of samples. 
 ** a single electronics channel and should be context free. an
 **
 **/
class CbmSpadicRawMessage : public CbmRawMessage
{

 public:

  /** Default constructor  **/
  CbmSpadicRawMessage();

  /** Constructor  **/
  CbmSpadicRawMessage(Int_t EquipmentID, Int_t SourceAddress, Int_t ChannelId,
		      Int_t EpochMarker, Int_t Time, Int_t SuperEpoch, Int_t TriggerType,
		      Int_t InfoType, Int_t StopType, Int_t GroupId, Int_t BufferOverflow, 
		      Int_t NrSamples, Int_t* Samples);

	
  /** Destructor  **/
  virtual ~CbmSpadicRawMessage() { };

  Int_t GetNrSamples() { return fNrSamples; }
  Int_t* GetSamples() { return fSamples; }
  Int_t GetTriggerType() { return fTriggerType; }
  Int_t GetStopType() { return fStopType; }
  Int_t GetInfoType() { return fInfoType; }
  Int_t GetGroupId() { return fGroupId; }

  ULong_t GetFullTime();

	
 private:

  Int_t fSuperEpoch;
  Int_t fTriggerType;
  Int_t fInfoType;
  Int_t fStopType;
  Int_t fGroupId;
  Int_t fBufferOverflowCount; 
  Int_t fNrSamples;
  Int_t fSamples[32];

  ClassDef(CbmSpadicRawMessage,2);

};

#endif
