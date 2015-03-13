/**
 * \file CbmBaseHit.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "CbmBaseHit.h"

CbmBaseHit::CbmBaseHit():
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
CbmBaseHit::CbmBaseHit(const CbmBaseHit& rhs)
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
CbmBaseHit& CbmBaseHit::operator=(const CbmBaseHit& rhs)
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

CbmBaseHit::~CbmBaseHit()
{
}

ClassImp(CbmBaseHit);
