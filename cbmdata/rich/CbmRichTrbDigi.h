#ifndef CBMRICHTRBDIGI_H
#define CBMRICHTRBDIGI_H

#include "TObject.h"

class CbmRichTrbDigi : public TObject
{
public:

   CbmRichTrbDigi();
   CbmRichTrbDigi(UInt_t TDCid,
                  Bool_t hasLedge,
                  Bool_t hasTedge,
                  UInt_t Lch,
                  UInt_t Tch,
                  Double_t Ltimestamp,
                  Double_t Ttimestamp);
   virtual ~CbmRichTrbDigi();

   UInt_t GetTDCid() { return fTDCid; }

   Bool_t GetHasLeadingEdge() { return fHasLeadingEdge; }
   Bool_t GetHasTrailingEdge() { return fHasTrailingEdge; }

   UInt_t GetLeadingEdgeChannel() { return fLeadingEdgeChannel; }
   UInt_t GetTrailingEdgeChannel() { return fTrailingEdgeChannel; }

   Double_t GetLeadingEdgeTimeStamp() { return fLeadingEdgeTimestamp; }
   Double_t GetTrailingEdgeTimeStamp() { return fTrailingEdgeTimestamp; }

protected:

   UInt_t fTDCid;

   Bool_t fHasLeadingEdge;
   Bool_t fHasTrailingEdge;

   UInt_t fLeadingEdgeChannel;
   UInt_t fTrailingEdgeChannel;

   Double_t fLeadingEdgeTimestamp;
   Double_t fTrailingEdgeTimestamp;

   ClassDef(CbmRichTrbDigi,1)
};

#endif // CBMRICHTRBDIGI_H
