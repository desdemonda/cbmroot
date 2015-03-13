// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           CbmROCUnpackDummy                       -----
// -----                    Created 14.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
#include "CbmROCUnpackDummy.h"

CbmROCUnpackDummy::CbmROCUnpackDummy()
  : CbmROCUnpack()
{
}
  
CbmROCUnpackDummy::~CbmROCUnpackDummy()
{
}


Bool_t CbmROCUnpackDummy::Init()
{
  return kTRUE;
}

Bool_t CbmROCUnpackDummy::DoUnpack(roc::Message* Message, ULong_t hitTime)
{
  return kTRUE;
}

void CbmROCUnpackDummy::Reset()
{
}

ClassImp(CbmROCUnpackDummy)
