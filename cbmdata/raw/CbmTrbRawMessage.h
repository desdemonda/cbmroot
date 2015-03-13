#ifndef CBMTRBRAWMESSAGE_H
#define CBMTRBRAWMESSAGE_H 1


#include "CbmRawMessage.h"

class CbmTrbRawMessage : public CbmRawMessage
{
  
 public:
  
  /** Default constructor  **/
  CbmTrbRawMessage();
  
  /** Constructor  **/
  CbmTrbRawMessage(Int_t FlibLink, Int_t FebId, Int_t ChannelId, 
		      Int_t EpochMarker, Int_t CoarseTime,		      
		      Int_t FineTime, Bool_t Edge);
  
  
  /** Destructor  **/
  virtual ~CbmTrbRawMessage() { };

 Int_t GetTDCfine() { return fTDCfine;}
 Int_t GetTDCcoarse() { return fTime;}
 Int_t GetEdge() { return fEdge;}
  
 ULong_t GetFullTime();
  
 private:
  
  Int_t fTDCfine;
  Bool_t fEdge;

  ClassDef(CbmTrbRawMessage,1);
  
};

#endif // CBMTRBRAWMESSAGE_H
