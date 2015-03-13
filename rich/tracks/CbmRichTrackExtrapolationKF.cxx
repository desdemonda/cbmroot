/**
* \file CbmRichTrackExtrapolationKF.cxx
*
* \author Claudia Hoehne
* \date 206
**/
#include "CbmRichTrackExtrapolationKF.h"

#include "CbmStsKFTrackFitter.h"

#include "FairTrackParam.h"
#include "CbmStsTrack.h"
#include "CbmGlobalTrack.h"
#include "FairTrackParam.h"
#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TMatrixFSym.h"

#include <iostream>

using std::cout;
using std::endl;

CbmRichTrackExtrapolationKF::CbmRichTrackExtrapolationKF():
   fStsTracks(0)
{
}

CbmRichTrackExtrapolationKF::~CbmRichTrackExtrapolationKF()
{
}

void CbmRichTrackExtrapolationKF::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if ( NULL == ioman) { Fatal("CbmRichTrackExtrapolationKF::Init", "RootManager not instantised!");}

   fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
   if ( NULL == fStsTracks) {Fatal("CbmRichTrackExtrapolationKF::Init", "No StsTrack array!");}
}

void CbmRichTrackExtrapolationKF::DoExtrapolation(
      TClonesArray* globalTracks,
      TClonesArray* extrapolatedTrackParams,
      double z,
      int minNofStsHits)
{
   if ( NULL == extrapolatedTrackParams ) {
      cout << "-E- CbmRichTrackExtrapolationKF::DoExtrapolate: TrackParamArray missing!" << endl;
      return;
   }

   if ( NULL == globalTracks ) {
      cout << "-E- CbmRichTrackExtrapolationKF::DoExtrapolate: Track Array missing!" << endl;
      return;
   }

   TMatrixFSym covMat(5);
   for(Int_t i=0;i<5;i++) for(Int_t j=0; j<=i; j++) covMat(i,j) = 0;
   covMat(0,0) = covMat(1,1) = covMat(2,2) = covMat(3,3) = covMat(4,4) = 1.e-4; 

   TVector3 pos, mom;

   Int_t nTracks = globalTracks->GetEntriesFast();
   for (Int_t iTrack=0; iTrack < nTracks; iTrack++){
      CbmGlobalTrack* gTrack = (CbmGlobalTrack*)globalTracks->At(iTrack);
      new((*extrapolatedTrackParams)[iTrack]) FairTrackParam(0.,0.,0.,0.,0.,0.,covMat);
      Int_t idSTS = gTrack->GetStsTrackIndex();
      if (idSTS < 0 ) continue;
      CbmStsTrack* pSTStr = (CbmStsTrack*) fStsTracks->At(idSTS);
      if ( NULL == pSTStr ) continue;
      CbmStsKFTrackFitter KF;
      FairTrackParam ExTrack;

      KF.Extrapolate(pSTStr, z, &ExTrack);

      Int_t Nsts = pSTStr->GetNofHits();

      if ( Nsts >= minNofStsHits) {
         *(FairTrackParam*)(extrapolatedTrackParams->At(iTrack)) = ExTrack;
      }
   }
}
