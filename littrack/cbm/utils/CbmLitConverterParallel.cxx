/**
 * \file CbmLitConverterParallel.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#include "CbmLitConverterParallel.h"
#include "parallel/LitTrackParam.h"
#include "parallel/LitScalTrack.h"
#include "parallel/LitScalPixelHit.h"
#include "CbmTrack.h"
#include "CbmTrdTrack.h"
#include "CbmMuchTrack.h"
#include "CbmPixelHit.h"
#include "FairTrackParam.h"
#include "TClonesArray.h"

#include "TClonesArray.h"

#include <cassert>


CbmLitConverterParallel::CbmLitConverterParallel()
{

}

CbmLitConverterParallel::~CbmLitConverterParallel()
{

}

void CbmLitConverterParallel::FairTrackParamToLitTrackParamScal(
   const FairTrackParam* par,
   lit::parallel::LitTrackParamScal* lpar)
{
   lpar->X = par->GetX();
   lpar->Y = par->GetY();
   lpar->Tx = par->GetTx();
   lpar->Ty = par->GetTy();
   lpar->Qp = par->GetQp();
   lpar->Z = par->GetZ();
   Double_t cov[15];
   par->CovMatrix(cov);
   lpar->C0 = cov[0];
   lpar->C1 = cov[1];
   lpar->C2 = cov[2];
   lpar->C3 = cov[3];
   lpar->C4 = cov[4];
   lpar->C5 = cov[5];
   lpar->C6 = cov[6];
   lpar->C7 = cov[7];
   lpar->C8 = cov[8];
   lpar->C9 = cov[9];
   lpar->C10 = cov[10];
   lpar->C11 = cov[11];
   lpar->C12 = cov[12];
   lpar->C13 = cov[13];
   lpar->C14 = cov[14];
}

void CbmLitConverterParallel::LitTrackParamScalToFairTrackParam(
   const lit::parallel::LitTrackParamScal* lpar,
   FairTrackParam* par)
{
   par->SetX(lpar->X);
   par->SetY(lpar->Y);
   par->SetTx(lpar->Tx);
   par->SetTy(lpar->Ty);
   par->SetQp(lpar->Qp);
   par->SetZ(lpar->Z);
   Double_t cov[15];
   cov[0] = lpar->C0;
   cov[1] = lpar->C1;
   cov[2] = lpar->C2;
   cov[3] = lpar->C3;
   cov[4] = lpar->C4;
   cov[5] = lpar->C5;
   cov[6] = lpar->C6;
   cov[7] = lpar->C7;
   cov[8] = lpar->C8;
   cov[9] = lpar->C9;
   cov[10] = lpar->C10;
   cov[11] = lpar->C11;
   cov[12] = lpar->C12;
   cov[13] = lpar->C13;
   cov[14] = lpar->C14;
   par->SetCovMatrix(cov);
}

void CbmLitConverterParallel::CbmPixelHitToLitScalPixelHit(
      const CbmPixelHit* hit,
      lit::parallel::LitScalPixelHit* lhit)
{
   assert(hit->GetType() == kTRDHIT || hit->GetType() == kMUCHPIXELHIT || hit->GetType() == kTOFHIT);

   lhit->X = hit->GetX();
   lhit->Y = hit->GetY();
   lhit->Z = hit->GetZ();
   lhit->Dx = hit->GetDx();
   lhit->Dy = hit->GetDy();
   lhit->Dxy = hit->GetDxy();
   lhit->refId = hit->GetRefId();

   if (hit->GetType() == kTRDHIT) {
      lhit->stationId = hit->GetPlaneId();
   } else if (hit->GetType() == kMUCHPIXELHIT) {
      lhit->stationId = (hit->GetPlaneId() - 1) / 2;
   } else if (hit->GetType() == kTOFHIT) {
      lhit->stationId = 0;
   }
}

void CbmLitConverterParallel::CbmPixelHitArrayToLitScalPixelHitArray(
      const TClonesArray* hits,
      vector<lit::parallel::LitScalPixelHit*>& lhits)
{
   Int_t nofHits = hits->GetEntriesFast();
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      const CbmPixelHit* hit = static_cast<const CbmPixelHit*>(hits->At(iHit));
      lit::parallel::LitScalPixelHit* lhit = new lit::parallel::LitScalPixelHit();
      CbmPixelHitToLitScalPixelHit(hit, lhit);
      lhits.push_back(lhit);
   }
}

void CbmLitConverterParallel::CbmTrackToLitScalTrack(
      const CbmTrack* track,
      const vector<lit::parallel::LitScalPixelHit*>& lhits,
      lit::parallel::LitScalTrack* ltrack)
{
   // Convert hits
   Int_t nofHits = track->GetNofHits();
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      // Now we convert only pixel hits
      if (track->GetHitType(iHit) != kPIXELHIT && track->GetHitType(iHit) != kTRDHIT && track->GetHitType(iHit) != kMUCHPIXELHIT) continue;
      Int_t hitId = track->GetHitIndex(iHit);
      ltrack->AddHit(lhits[hitId]);
   }
   // Convert first and last track parameter
   lit::parallel::LitTrackParamScal lpar;
   FairTrackParamToLitTrackParamScal(track->GetParamFirst(), &lpar);
   ltrack->SetParamFirst(lpar);
   FairTrackParamToLitTrackParamScal(track->GetParamLast(), &lpar);
   ltrack->SetParamFirst(lpar);
   // Convert other parameters
   ltrack->SetChiSq(track->GetChiSq());
   ltrack->SetNDF(track->GetNDF());
   ltrack->SetPreviousTrackId(track->GetPreviousTrackId());
   ltrack->SetNofMissingHits(0);
   ltrack->IsGood(true);
}

void CbmLitConverterParallel::CbmTrackArrayToLitScalTrackArray(
      const TClonesArray* tracks,
      const vector<lit::parallel::LitScalPixelHit*>& lhits,
      vector<lit::parallel::LitScalTrack*>& ltracks)
{
   Int_t nofTracks = tracks->GetEntriesFast();
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      const CbmTrack* track = static_cast<const CbmTrack*>(tracks->At(iTrack));
      lit::parallel::LitScalTrack* ltrack = new lit::parallel::LitScalTrack();
      CbmTrackToLitScalTrack(track, lhits, ltrack);
      ltracks.push_back(ltrack);
   }
}

void CbmLitConverterParallel::LitScalTrackToCbmTrack(
      const lit::parallel::LitScalTrack* ltrack,
      CbmTrack* track)
{
   // Convert hits
   Int_t nofHits = ltrack->GetNofHits();
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      Int_t hitId = ltrack->GetHit(iHit)->refId;
      track->AddHit(hitId, kTRDHIT);
   }
   // Convert first and last track parameter
   FairTrackParam par;
   LitTrackParamScalToFairTrackParam(&ltrack->GetParamFirst(), &par);
   track->SetParamFirst(&par);
   LitTrackParamScalToFairTrackParam(&ltrack->GetParamLast(), &par);
   track->SetParamLast(&par);
   // Convert other parameters
   track->SetChiSq(ltrack->GetChiSq());
   track->SetNDF(ltrack->GetNDF());
   track->SetPreviousTrackId(ltrack->GetPreviousTrackId());
}

void CbmLitConverterParallel::LitScalTrackArrayToCbmTrdTrackArray(
      const vector<lit::parallel::LitScalTrack*>& ltracks,
      TClonesArray* tracks)
{
   Int_t nofTracks = ltracks.size();
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      lit::parallel::LitScalTrack* ltrack = ltracks[iTrack];
      CbmTrdTrack* track = new ((*tracks)[iTrack]) CbmTrdTrack();
      LitScalTrackToCbmTrack(ltrack, track);
   }
}

void CbmLitConverterParallel::LitScalTrackArrayToCbmMuchTrackArray(
      const vector<lit::parallel::LitScalTrack*>& ltracks,
      TClonesArray* tracks)
{
   Int_t nofTracks = ltracks.size();
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      lit::parallel::LitScalTrack* ltrack = ltracks[iTrack];
      CbmMuchTrack* track = new ((*tracks)[iTrack]) CbmMuchTrack();
      LitScalTrackToCbmTrack(ltrack, track);
   }
}
