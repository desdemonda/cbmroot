/**
 * \file CbmTrdHit.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "CbmTrdHit.h"
#include "TVector3.h"
#include <sstream>
using std::stringstream;
using std::endl;

CbmTrdHit::CbmTrdHit()
  : CbmPixelHit(),
    fELossTR(-1.),
    fELossdEdx(-1.),
    fELoss(-1.)
{
  SetType(kTRDHIT);
}

CbmTrdHit::CbmTrdHit(
		Int_t address,
		const TVector3& pos,
		const TVector3& dpos,
		Double_t dxy,
		Int_t refId,
		Double_t eLossTR,
		Double_t eLossdEdx,
		Double_t eLoss)
  : CbmPixelHit(address, pos, dpos, dxy, refId),
    fELossTR(eLossTR),
    fELossdEdx(eLossdEdx),
    fELoss(eLoss)
{
  SetType(kTRDHIT);
}

CbmTrdHit::~CbmTrdHit()
{
}

string CbmTrdHit::ToString() const
{
   stringstream ss;
   ss << "CbmTrdHit: address=" << GetAddress()
       << " pos=(" << GetX() << "," << GetY() << "," << GetZ()
       << ") err=(" << GetDx() << "," << GetDy() << "," << GetDz()
       << ") dxy=" << GetDxy() << " refId=" << GetRefId()
       << " eloss=" << GetELoss() << " elossdEdX=" << GetELossdEdX()
       << " elossdTR=" << GetELossTR() << endl;
   return ss.str();
}

ClassImp(CbmTrdHit);
