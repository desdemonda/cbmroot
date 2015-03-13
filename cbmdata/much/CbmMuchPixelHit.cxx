/**
 * \file CbmMuchPixelHit.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "CbmMuchPixelHit.h"

CbmMuchPixelHit::CbmMuchPixelHit()
  : CbmPixelHit(),
    fPlaneId(-1),
    fTime(-1.),
    fDTime(-1.),
    fFlag(0)
{
  SetType(kMUCHPIXELHIT);
}

CbmMuchPixelHit::~CbmMuchPixelHit()
{

}

CbmMuchPixelHit::CbmMuchPixelHit(
		Int_t address,
		Double_t x,
		Double_t y,
		Double_t z,
		Double_t dx,
		Double_t dy,
		Double_t dz,
		Double_t dxy,
		Int_t refId,
		Int_t planeId,
		Double_t t,
		Double_t dt)
  : CbmPixelHit(address, x, y, z, dx, dy, dz, dxy, refId),
    fPlaneId(planeId),
    fTime(t),
    fDTime(dt),
    fFlag(0)
{
	SetType(kMUCHPIXELHIT);
}

CbmMuchPixelHit::CbmMuchPixelHit(
		Int_t address,
		const TVector3& pos,
		const TVector3& err,
		Double_t dxy,
		Int_t refId,
		Int_t planeId)
  : CbmPixelHit(address, pos, err, dxy, refId),
    fPlaneId(planeId),
    fTime(-1.),
    fDTime(-1.),
    fFlag(0)
{
  SetType(kMUCHPIXELHIT);
}

CbmMuchPixelHit::CbmMuchPixelHit(
		Int_t address,
		const TVector3& pos,
		const TVector3& err,
		Double_t dxy,
		Int_t refId,
		Int_t planeId,
		Double_t time,
		Double_t dtime)
  : CbmPixelHit(address, pos, err, dxy, refId),
    fPlaneId(planeId),
    fTime(time),
    fDTime(dtime),
    fFlag(0)
{
  SetType(kMUCHPIXELHIT);
}

ClassImp(CbmMuchPixelHit);
