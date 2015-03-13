// -----------------------------------------------------------------------
// -----                  CbmTrdTrackFitterKF_CA                        -----
// -----             Created 29/11/05  by D. Kresan                  -----
// -----------------------------------------------------------------------
#include "CbmTrdTrackFitterKF_CA.h"

#include "CbmKFTrack.h"
#include "CbmKFTrdHit.h"
#include "CbmKFVertex.h"

#include "FairRootManager.h"
#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"

#include "TClonesArray.h"

#include <iostream>
#include <vector>
#include "math.h"

using std::cout;
using std::endl;
using std::vector;
//using std::sqrt;
//using std::finite;
//________________________________________________________________________
//
// CbmTrdTrackFitterKF_CA
//
// Concrete implementation of the TRD track fitter based on Kalman filter
//


// -----------------------------------------------------------------------
CbmTrdTrackFitterKF_CA::CbmTrdTrackFitterKF_CA()
  : CbmTrdTrackFitter(),
    fArrayTrdHit(NULL),
    fVerbose(1),
    fPid(211),
    fKfTrack(new CbmKFTrack())
{
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
CbmTrdTrackFitterKF_CA::CbmTrdTrackFitterKF_CA(Int_t verbose, Int_t pid)
  : CbmTrdTrackFitter(),
    fArrayTrdHit(NULL),
    fVerbose(verbose),
    fPid(pid),
    fKfTrack(new CbmKFTrack())
{
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
CbmTrdTrackFitterKF_CA::~CbmTrdTrackFitterKF_CA()
{
    // Destructor
    delete fKfTrack;
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
void CbmTrdTrackFitterKF_CA::Init()
{
    // Initialisation

    // Get the pointer to FairRootManager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
        cout << "-E- CbmTrdTrackFitterKF_CA::Init : "
            << " ROOT manager is not instantiated!" << endl;
        return;
    }

    // Activate data branches
    fArrayTrdHit = (TClonesArray*) rootMgr->GetObject("TrdHit");
    if(NULL == fArrayTrdHit) {
        cout << "-E- CbmTrdTrackFitterKF_CA::Init : "
            << " no TRD hit array!" << endl;
        return;
    }
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
Int_t CbmTrdTrackFitterKF_CA::DoFit(CbmTrdTrack* pTrack)
{
    // Implementation of the fitting algorithm
    if(NULL==fArrayTrdHit) return 1;

    // Declare variables outside the loop
    CbmTrdHit* pHit = NULL;
    CbmKFTrdHit* pKFHit = NULL;
    Int_t hitIndex = 0;
    Int_t materialIndex = 0;
    Double_t eLoss = 0.;

    // Loop over TRD hits of the track
    for(Int_t iHit = 0; iHit < pTrack->GetNofHits(); iHit++) {
        // Get current hit index
        hitIndex = pTrack->GetHitIndex(iHit);
        //Get the pointer to the CbmTrdHit
        pHit = (CbmTrdHit*) fArrayTrdHit->At(hitIndex);
        if(NULL == pHit) {
            cout << "-E- CbmTrdTrackFitterKF_CA::DoFit : "
                << " empty TRD hit : " << hitIndex << ", "
                << pHit << endl;
            return 1;
        }
        // Accumulate TR energy loss
        eLoss += pHit->GetELoss();
        // Create CbmKFTrdHit
        pKFHit = new CbmKFTrdHit();
        pKFHit->Create(pHit);
        materialIndex = pKFHit->MaterialIndex;
        // Add to the KFTrack
        fKfTrack->fHits.push_back(pKFHit);
        if(fVerbose > 2) {
            cout << "   TRD hit : ("
                << pHit->GetX() << ", " << pHit->GetY()
                << ", " << pHit->GetZ() << ") "
                << " is added to track. matidx="
                << materialIndex << endl;
        }
    } // Loop over TRD hits

    fKfTrack->GetRefChi2() = 0.;
    fKfTrack->GetRefNDF() = 0;
    fKfTrack->SetTrackParam( *(const_cast<FairTrackParam*>(pTrack->GetParamLast())) );
    fKfTrack->SetPID(fPid);

    fKfTrack->Fit(0);
    fKfTrack->Fit(1);
    fKfTrack->Fit(0);
    // Store parameters at first layer
    fKfTrack->GetTrackParam(*(const_cast<FairTrackParam*>(pTrack->GetParamFirst())));
    if(fVerbose > 2) {
//        pTrack->GetParamFirst()->Print();
    }
    fKfTrack->Fit(1);
    // Store parameters at last layer
    fKfTrack->GetTrackParam(*(const_cast<FairTrackParam*>(pTrack->GetParamLast())));
    if(fVerbose > 2) {
//        pTrack->GetParamLast()->Print();
    }

    // Store chi2 of fit
    pTrack->SetChiSq(fKfTrack->GetRefChi2());
    pTrack->SetNDF(fKfTrack->GetRefNDF());

    // Store accumulated TR energy loss
    pTrack->SetELoss(eLoss);

    // Delete CbmKFTrdHit objects
    vector<CbmKFHit*>::iterator it;
    for(it = fKfTrack->fHits.begin(); it != fKfTrack->fHits.end(); it++) {
        pKFHit = (CbmKFTrdHit*) (*it);
        delete pKFHit;
    }
    fKfTrack->fHits.clear();

    if(fVerbose > 1) {
        cout << "TRD track fitted. chi2/ndf = " << pTrack->GetChiSq()/pTrack->GetNDF()
            << endl;
        if(fVerbose > 2) {
            cout << endl << endl;
        }
    }

    return 0;
}
// -----------------------------------------------------------------------

//section added 28.04.2008 by Maciej Krauze

Double_t CbmTrdTrackFitterKF_CA::FitToVertex( CbmTrdTrack* track, CbmVertex *vtx, FairTrackParam *v_track )
{


      Double_t ret = 100.;
      if( !track || !vtx || !v_track ) return ret;
      FairTrackParam param = *(track->GetParamFirst());
      //CbmKFTrack T( &(track->GetParamFirst()) );
      CbmKFTrack T( param );

      T.SetPID( track->GetPidHypo() );
      T.GetRefChi2() = track->GetChiSq();
      T.GetRefNDF() = track->GetNDF();

      CbmKFVertex V( *vtx );
      T.Fit2Vertex( V );
      T.GetTrackParam( *v_track );
      if( T.GetRefNDF()>0 && T.GetRefChi2()>=0 )
     {

	      ret = T.GetRefChi2()/T.GetRefNDF();
	      if( finite(ret) ) ret =  sqrt(ret);
     }

      return ret;
}


void CbmTrdTrackFitterKF_CA::Extrapolate( CbmTrdTrack* track, Double_t z, FairTrackParam* e_track )
{


        if( !track ) return;
        CbmKFTrack T;
        T.SetPID( track->GetPidHypo() );
        const FairTrackParam *fpar = track->GetParamFirst(), *lpar = track->GetParamLast();




        if( z<=fpar->GetZ() )
     {

	        T.SetTrackParam( *(const_cast<FairTrackParam*>(fpar)) );
	        T.Extrapolate( z );
     }


      else if (z >= lpar->GetZ() )
     {


	        T.SetTrackParam( *(const_cast<FairTrackParam*>(lpar)) );
	        T.Extrapolate( z );
     }




           if( e_track ) T.GetTrackParam( *e_track );

}






//end of added section



ClassImp(CbmTrdTrackFitterKF_CA);




