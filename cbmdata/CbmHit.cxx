/**
 * \file CbmHit.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Former name CbmBaseHit. Renamed 11 May 2015.
 **/
#include "CbmHit.h"

CbmHit::CbmHit():
    TObject(),
	fType(kHIT),
	fZ(0.),
	fDz(0.),
	fRefId(-1),
	fAddress(-1),
	fMatch(NULL)
{
}

// Only shallow copy needed
CbmHit::CbmHit(const CbmHit& rhs)
 : TObject(rhs),
   fType(rhs.fType),
   fZ(rhs.fZ),
   fDz(rhs.fDz),
   fRefId(rhs.fRefId),
   fAddress(rhs.fAddress),
   fMatch(NULL)
{
}

// Only shallow copy needed
CbmHit& CbmHit::operator=(const CbmHit& rhs)
{

  if (this != &rhs) {
    TObject::operator=(rhs);
    fType = rhs.fType;
    fZ = rhs.fZ;
    fDz = rhs.fDz;
    fRefId = rhs.fRefId;
    fAddress = rhs.fAddress;
    fMatch = rhs.fMatch;
  }
  return *this;
}

CbmHit::~CbmHit()
{
}

ClassImp(CbmHit);
