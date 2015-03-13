#ifndef CBMNXYTERRAWMESSAGE_H
#define CBMNXYTERRAWMESSAGE_H 1

#include "CbmRawMessage.h"

class CbmNxyterRawMessage : public CbmRawMessage
{

public:

   /** Default constructor  **/
   CbmNxyterRawMessage();

   /** Constructor  **/
   CbmNxyterRawMessage(Int_t FlibLink, Int_t FebId, Int_t ChannelId, 
                        Int_t EpochMarker, Int_t Time,
                        Int_t ADCval, Bool_t lastEpoch, Bool_t pileUp, Bool_t overflow);

   /** Destructor  **/
   virtual ~CbmNxyterRawMessage() { };

   Int_t GetADCvalue() { return fADCvalue;}

   ULong_t GetFullTime();

private:

   Int_t fADCvalue;
   Bool_t fLastEpoch;
   Bool_t fPileUp;
   Bool_t fOverflow;

   ClassDef(CbmNxyterRawMessage,2);

};

#endif // CBMNXYTERRAWMESSAGE_H
