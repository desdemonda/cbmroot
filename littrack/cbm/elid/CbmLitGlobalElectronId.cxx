/**
 * \file CbmLitReconstructionQa.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/

#include "elid/CbmLitGlobalElectronId.h"
#include "FairRootManager.h"
#include "CbmRichRing.h"
#include "CbmTrdTrack.h"
#include "CbmTofHit.h"
#include "CbmRichElectronIdAnn.h"
#include "CbmGlobalTrack.h"

#include "TMath.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TSystem.h"

#include <cmath>

CbmLitGlobalElectronId::CbmLitGlobalElectronId():
   fTrdAnnCut(0.85),
   fRichAnnCut(0.0),
   fRichUseAnn(true),
   fRichMeanA(-1.),
   fRichMeanB(-1.),
   fRichRmsA(-1.),
   fRichRmsB(-1.),
   fRichRmsCoeff(-1.),
   fRichDistCut(-1.),
   fRichElIdAnn(NULL),
   fGlobalTracks(NULL),
   fRichRings(NULL),
   fTrdTracks(NULL),
   fTofHits(NULL)
{

}

CbmLitGlobalElectronId::~CbmLitGlobalElectronId()
{

}

void CbmLitGlobalElectronId::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
	fTofHits = (TClonesArray*) ioman->GetObject("TofHit");

   if (fRichUseAnn) {
      fRichElIdAnn = new CbmRichElectronIdAnn();
      fRichElIdAnn->Init();
   }
}

Bool_t CbmLitGlobalElectronId::IsRichElectron(
		Int_t globalTrackIndex,
		Double_t momentum)
{
   if (NULL == fGlobalTracks || NULL == fRichRings) return false;
   const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(globalTrackIndex));
   Int_t richId = globalTrack->GetRichRingIndex();
   if (richId < 0) return false;
   CbmRichRing* ring = static_cast<CbmRichRing*> (fRichRings->At(richId));
   if (NULL == ring) return false;

   if (fRichUseAnn == false) {
      Double_t axisA = ring->GetAaxis();
      Double_t axisB = ring->GetBaxis();
      Double_t dist = ring->GetDistance();
      if (fabs(axisA - fRichMeanA) < fRichRmsCoeff * fRichRmsA && fabs(axisB
            - fRichMeanB) < fRichRmsCoeff * fRichRmsB && dist < fRichDistCut) {
         return true;
      } else {
         return false;
      }
   } else {
      Double_t ann = fRichElIdAnn->DoSelect(ring, momentum);
      if (ann > fRichAnnCut) return true;
      else return false;
   }
}

Bool_t CbmLitGlobalElectronId::IsTrdElectron(
		Int_t globalTrackIndex,
		Double_t momentum)
{
   if (NULL == fGlobalTracks || NULL == fTrdTracks) return false;
   const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(globalTrackIndex));
   Int_t trdId = globalTrack->GetTrdTrackIndex();
   if (trdId < 0) return false;
   CbmTrdTrack* trdTrack = static_cast<CbmTrdTrack*>(fTrdTracks->At(trdId));
   if (NULL == trdTrack) return false;

   Double_t ann = trdTrack->GetPidANN();
   if (ann > fTrdAnnCut) return true;
   else return false;
}

Bool_t CbmLitGlobalElectronId::IsTofElectron(
		Int_t globalTrackIndex,
		Double_t momentum)
{
   if (NULL == fGlobalTracks || NULL == fTofHits) return false;
   const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTracks->At(globalTrackIndex));
   Double_t trackLength = globalTrack->GetLength() / 100.;
   Int_t tofId = globalTrack->GetTofHitIndex();
   if (tofId < 0) return false;
   CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofId);
   if (NULL == tofHit)return false;

   Double_t time = 0.2998 * tofHit->GetTime(); // time in ns -> transfrom to ct in m
   Double_t mass2 = TMath::Power(momentum, 2.) * (TMath::Power(time/ trackLength, 2) - 1);

   if (momentum >= 1.) {
       if (mass2 < (0.013*momentum - 0.003)){
           return true;
       }
   } else {
       if (mass2 < 0.01){
           return true;//fTofM2
       }
   }
   return false;
}

ClassImp(CbmLitGlobalElectronId);
