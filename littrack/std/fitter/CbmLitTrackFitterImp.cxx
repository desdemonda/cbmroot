/**
 * \file CbmLitTrackFitterImp.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 **/
#include "fitter/CbmLitTrackFitterImp.h"

#include "data/CbmLitTrackParam.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitHit.h"
#include "data/CbmLitStripHit.h"
#include "data/CbmLitFitNode.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackUpdate.h"
#include "utils/CbmLitMath.h"

#include <iostream>

CbmLitTrackFitterImp::CbmLitTrackFitterImp(
   TrackPropagatorPtr propagator,
   TrackUpdatePtr update)
{
   fPropagator = propagator;
   fUpdate = update;
}

CbmLitTrackFitterImp::~CbmLitTrackFitterImp()
{
}

LitStatus CbmLitTrackFitterImp::Fit(
   CbmLitTrack* track,
   Bool_t downstream)
{
   track->SortHits(downstream);
   track->SetChi2(0.0);
   Int_t nofHits = track->GetNofHits();
   FitNodeVector nodes(nofHits);
   CbmLitTrackParam par;
   std::vector<litfloat> F(25);

   if (downstream) {
      track->SetParamLast(track->GetParamFirst());
      par = *track->GetParamLast();
   } else {
      track->SetParamFirst(track->GetParamLast());
      par = *track->GetParamFirst();
   }

   litfloat totalLength = 0.;

   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      const CbmLitHit* hit = track->GetHit(iHit);
      litfloat Ze = hit->GetZ();
      litfloat length = 0;
      if (fPropagator->Propagate(&par, Ze, track->GetPDG(), &F, &length) == kLITERROR) {
         track->SetQuality(kLITBAD);
         //std::cout << "PROP ERROR: Ze=" << Ze << " length=" << length << " par=" << par.ToString();
         return kLITERROR;
      }
      totalLength += length;
      nodes[iHit].SetPredictedParam(&par);
      nodes[iHit].SetF(F);
      litfloat chi2Hit = 0.;
      if (fUpdate->Update(&par, hit, chi2Hit) == kLITERROR) {
         track->SetQuality(kLITBAD);
         //std::cout << "UPD ERROR: Ze=" << Ze << " length=" << length << " par=" << par.ToString();
         return kLITERROR;
      }
      nodes[iHit].SetUpdatedParam(&par);
      nodes[iHit].SetChiSqFiltered(chi2Hit);
      track->SetChi2(track->GetChi2() + chi2Hit);
      if (iHit == 0) {
         if (downstream) { track->SetParamFirst(&par); }
         else { track->SetParamLast(&par); }
      }
   }
   if (downstream) { track->SetParamLast(&par); }
   else { track->SetParamFirst(&par); }

   track->SetFitNodes(nodes);
   track->SetNDF(lit::NDF(track));
   track->SetLength(totalLength);

   return kLITSUCCESS;
}
