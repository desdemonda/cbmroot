#ifndef CBMNXYTERRAWSYNCMESSAGE_H
#define CBMNXYTERRAWSYNCMESSAGE_H 1

#include "CbmRawMessage.h"

class CbmNxyterRawSyncMessage : public CbmRawMessage
{
public: // methods

   /** Default constructor  **/
   CbmNxyterRawSyncMessage();
   
   
   /** Constructor  **/
   CbmNxyterRawSyncMessage(Int_t FlibLink, Int_t FebId, Int_t ChannelId, 
	         Int_t EpochMarker, Int_t Time,
	         Int_t StFlag, Int_t Data);
   
   /** Destructor  **/
   virtual ~CbmNxyterRawSyncMessage() { };
   
public: // data members

private: // methods

private: // data members

   Int_t fStFlag;
   Int_t fData;

   ClassDef(CbmNxyterRawSyncMessage, 1);
};

#endif //CBMNXYTERRAWSYNCMESSAGE_H
