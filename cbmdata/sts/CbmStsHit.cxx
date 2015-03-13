/**
 * \file CbmStsHit.cxx
 **/
#include "CbmStsHit.h"

CbmStsHit::CbmStsHit()
    : CbmPixelHit()
    , fFrontDigiId(-1)
    , fBackDigiId(-1)
    , fSectorNr(-1)
    , fFrontClusterId(-1)
    , fBackClusterId(-1)
    , fTime(0.)
    , fTimeError(0.)
{
}


CbmStsHit::CbmStsHit(Int_t address, const TVector3& pos, const TVector3& dpos,
		                 Double_t dxy, Int_t frontClusterId, Int_t backClusterId,
		                 Int_t frontDigiId, Int_t backDigiId, Int_t sectorNr,
		                 Double_t time, Double_t timeError)
    : CbmPixelHit(address, pos, dpos, dxy, -1)
    , fFrontDigiId(frontDigiId)
    , fBackDigiId(backDigiId)
    , fSectorNr(sectorNr)
    , fFrontClusterId(frontClusterId)
    , fBackClusterId(backClusterId)
    , fTime(time)
    , fTimeError(timeError)
{
}
CbmStsHit::~CbmStsHit()
{
}

ClassImp(CbmStsHit)
