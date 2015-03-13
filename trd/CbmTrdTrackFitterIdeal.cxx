// -------------------------------------------------------------------------
// -----                CbmTrdTrackFitterIdeal source file             -----
// -----                  Created 12/05/06  by D. Kresan               -----
// -------------------------------------------------------------------------
#include "CbmTrdTrackFitterIdeal.h"

#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;


//__________________________________________________________________________
//
// CbmTrdTrackFitterIdeal
//
// Concrete implementation of TRD track fitting algorithm, based on MC
//


// -------------------------------------------------------------------------
CbmTrdTrackFitterIdeal::CbmTrdTrackFitterIdeal()
  : CbmTrdTrackFitter(),
    fArrayTrdPoint(NULL),
    fArrayTrdHit(NULL)
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
CbmTrdTrackFitterIdeal::~CbmTrdTrackFitterIdeal()
{
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmTrdTrackFitterIdeal::Init()
{
    // Task initialisation
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cerr << "-E- CbmTrdTrackFitterIdeal::Init : "
	    << "ROOT manager is not instantiated!" << endl;
        return;
    }

    fArrayTrdPoint = (TClonesArray*) rootMgr->GetObject("TrdPoint");
    if(NULL == fArrayTrdPoint) {
	cout << "-W- CbmTrdTrackFitterIdeal::Init : "
            << "no TRD point array!" << endl;
    }

    fArrayTrdHit = (TClonesArray*) rootMgr->GetObject("TrdHit");
    if(NULL == fArrayTrdHit) {
	cout << "-W- CbmTrdTrackFitterIdeal::Init : "
            << "no TRD hit array!" << endl;
    }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Int_t CbmTrdTrackFitterIdeal::DoFit(CbmTrdTrack* pTrack)
{
    // Implementation of the fitting algorithm
    if(NULL == fArrayTrdPoint ||
       NULL == fArrayTrdHit) return 0;

    // Parameters at the first plane
    Int_t hitIndex = pTrack->GetHitIndex(0);
    if(hitIndex < 0) return 0;
    CbmTrdHit* hit = (CbmTrdHit*) fArrayTrdHit->At(hitIndex);
    if(NULL == hit) return 0;
    Int_t pointIndex = hit->GetRefId();
    if(pointIndex < 0) return 0;
    CbmTrdPoint* point = (CbmTrdPoint*) fArrayTrdPoint->At(pointIndex);
    if(NULL == point) return 0;
    FairTrackParam par1;
    SetTrackParam(point, &par1);
    pTrack->SetParamFirst(&par1);

    // Parameters at the last plane
    hitIndex = pTrack->GetHitIndex( pTrack->GetNofHits()-1 );
    if(hitIndex < 0) return 0;
    hit = (CbmTrdHit*) fArrayTrdHit->At(hitIndex);
    if(NULL == hit) return 0;
    pointIndex = hit->GetRefId();
    if(pointIndex < 0) return 0;
    point = (CbmTrdPoint*) fArrayTrdPoint->At(pointIndex);
    if(NULL == point) return 0;
    FairTrackParam par2;
    SetTrackParam(point, &par2);
    pTrack->SetParamLast(&par2);

    return 1;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmTrdTrackFitterIdeal::SetTrackParam(CbmTrdPoint* point, FairTrackParam* trackParam)
{
    // Set track parameters from the MC point
    TVector3 pos;
    TVector3 mom;
    point->Position(pos);
    point->Momentum(mom);
    trackParam->SetX(pos.X());
    trackParam->SetY(pos.Y());
    trackParam->SetZ(pos.Z());
    trackParam->SetTx(mom.X()/mom.Z());
    trackParam->SetTy(mom.Y()/mom.Z());
}
// -------------------------------------------------------------------------


ClassImp(CbmTrdTrackFitterIdeal)


