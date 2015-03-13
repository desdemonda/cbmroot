// ------------------------------------------------------------------
// -----                  CbmGlobalTrackFitterIdeal             -----
// -----              Created 07/03/2006 by D.Kresan            -----
// ------------------------------------------------------------------
#include "CbmGlobalTrackFitterIdeal.h"

#include "CbmGlobalTrack.h"

#include "FairRootManager.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

//___________________________________________________________________
//
// CbmGlobalTrackFitterIdeal
//
// Concrete implementation of the global track fitting algorithm, ba-
// sed on the MC information
//


// ------------------------------------------------------------------
CbmGlobalTrackFitterIdeal::CbmGlobalTrackFitterIdeal()
  : CbmGlobalTrackFitter(),
    fArrayStsTrack(NULL),
    fArrayTrdTrack(NULL)
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmGlobalTrackFitterIdeal::~CbmGlobalTrackFitterIdeal()
{
    // Destructor
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmGlobalTrackFitterIdeal::Init()
{
    // Initialisation

    // Get pointer to the ROOT I/O manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmGlobalTrackFitterIdeal::Init :"
	    << " ROOT manager is not instantiated" << endl;
        return;
    }
    // Get track arrays
    fArrayStsTrack = (TClonesArray*) rootMgr->GetObject("StsTrack");
    if(NULL == fArrayStsTrack) {
	cout << "-W- CbmGlobalTrackFitterIdeal::Init : "
            << "no STS track array!" << endl;
    }
    fArrayTrdTrack = (TClonesArray*) rootMgr->GetObject("TrdTrack");
    if(NULL == fArrayTrdTrack) {
	cout << "-W- CbmGlobalTrackFitterIdeal::Init : "
            << "no TRD track array!" << endl;
    }
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmGlobalTrackFitterIdeal::DoFit(CbmGlobalTrack* glbTrack)
{
    // Implementation of the algorithm
    if(NULL == glbTrack) return;
    if(NULL == fArrayStsTrack || NULL == fArrayTrdTrack) return;

    if(glbTrack->GetStsTrackIndex() < 0 &&
       glbTrack->GetTrdTrackIndex() < 0) {
        return;
    } else if(glbTrack->GetStsTrackIndex() >= 0 &&
	      glbTrack->GetTrdTrackIndex() < 0) {
	CbmStsTrack* stsTrack = (CbmStsTrack*) fArrayStsTrack->At(glbTrack->GetStsTrackIndex());
	if(NULL == stsTrack) {
            return;
	}
        glbTrack->SetParamFirst(stsTrack->GetParamFirst());
        glbTrack->SetParamLast(stsTrack->GetParamLast());
    } else if(glbTrack->GetStsTrackIndex() < 0 &&
	      glbTrack->GetTrdTrackIndex() >= 0) {
	CbmTrdTrack* trdTrack = (CbmTrdTrack*) fArrayTrdTrack->At(glbTrack->GetTrdTrackIndex());
	if(NULL == trdTrack) {
            return;
	}
        glbTrack->SetParamFirst(trdTrack->GetParamFirst());
        glbTrack->SetParamLast(trdTrack->GetParamLast());
    } else {
	CbmStsTrack* stsTrack = (CbmStsTrack*) fArrayStsTrack->At(glbTrack->GetStsTrackIndex());
	if(NULL == stsTrack) {
            return;
	}
	CbmTrdTrack* trdTrack = (CbmTrdTrack*) fArrayTrdTrack->At(glbTrack->GetTrdTrackIndex());
	if(NULL == trdTrack) {
            return;
	}
        glbTrack->SetParamFirst(stsTrack->GetParamFirst());
        glbTrack->SetParamLast(trdTrack->GetParamLast());
    }
}
// ------------------------------------------------------------------


ClassImp(CbmGlobalTrackFitterIdeal);

