#include "CbmRichTrbDigi.h"

CbmRichTrbDigi::CbmRichTrbDigi() 
  : TObject(),
    fTDCid(0),
    fHasLeadingEdge(kFALSE),
    fHasTrailingEdge(kFALSE),
    fLeadingEdgeChannel(0),
    fTrailingEdgeChannel(0),
    fLeadingEdgeTimestamp(0.),
    fTrailingEdgeTimestamp(0.)
{
}

CbmRichTrbDigi::CbmRichTrbDigi(
      UInt_t TDCid,
      Bool_t hasLedge,
      Bool_t hasTedge,
      UInt_t Lch,
      UInt_t Tch,
      Double_t Ltimestamp,
      Double_t Ttimestamp) :
   TObject(),
   fTDCid(TDCid),
   fHasLeadingEdge(hasLedge),
   fHasTrailingEdge(hasTedge),
   fLeadingEdgeChannel(Lch),
   fTrailingEdgeChannel(Tch),
   fLeadingEdgeTimestamp(Ltimestamp),
   fTrailingEdgeTimestamp(Ttimestamp)
{
}

CbmRichTrbDigi::~CbmRichTrbDigi()
{
}

ClassImp(CbmRichTrbDigi)
