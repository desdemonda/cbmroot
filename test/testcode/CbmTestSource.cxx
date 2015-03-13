#include "CbmTestSource.h"


CbmTestSource::CbmTestSource()
  : FairSource()
{
}


CbmTestSource::CbmTestSource(const CbmTestSource& source)
  : FairSource(source)
{
}


CbmTestSource::~CbmTestSource()
{
}

Bool_t CbmTestSource::Init()
{
  return kTRUE;
}

Int_t CbmTestSource::ReadEvent(UInt_t)
{
  return FairSource::ReadEvent(2);
}

void CbmTestSource::Close()
{
  return;
}

void CbmTestSource::Reset()
{
  return;
}

ClassImp(CbmTestSource)
