///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// CbmStsTrackletCuts class - class                                             //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////

#include "CbmStsTrackletCuts.h"
#include "iostream.h"

ClassImp(CbmStsTrackletCuts)

//________________________________________________________________
  CbmStsTrackletCuts::CbmStsTrackletCuts() {
}


//________________________________________________________________
CbmStsTrackletCuts::~CbmStsTrackletCuts() {
}


//________________________________________________________________
void CbmStsTrackletCuts::SetPrimaryTrackletCuts() {

  fTrackletMinPoints = 2;
  fStationScopeTracklet = 1;

  fMaxAngleTracklet = 0.08;
  fMaxAlphaTracklet = 0.001;
  fMaxPhiTracklet   = 0.05;

}

//________________________________________________________________
void CbmStsTrackletCuts::SetPrimaryIntermedMomTrackletCuts() {

  fTrackletMinPoints = 2;
  fStationScopeTracklet = 1;

  fMaxAngleTracklet = 0.1;
  fMaxAlphaTracklet = 0.001;
  fMaxPhiTracklet   = 0.05;

}

//________________________________________________________________
void CbmStsTrackletCuts::SetPrimaryLowTrackletCuts() {

  fTrackletMinPoints = 2;
  fStationScopeTracklet = 1;

  fMaxAngleTracklet = 0.11;
  fMaxAlphaTracklet = 0.001;
  fMaxPhiTracklet   = 0.05;

}

//________________________________________________________________
void CbmStsTrackletCuts::SecondaryHighTrackletCuts() {

  fMaxAngleTracklet = 0.15;
  fMaxAlphaTracklet = 0.01;
  fMaxPhiTracklet   = 0.2;

}

//________________________________________________________________
void CbmStsTrackletCuts::SecondaryLowTrackletCuts() {

  fMaxAngleTracklet = 0.2;
  fMaxAlphaTracklet = 0.012;
  fMaxPhiTracklet   = 0.2;

}

//________________________________________________________________
Bool_t CbmStsTrackletCuts::CheckTracklet(Double_t ang) {

  if (ang < fMaxAngleTracklet) {
    return kTRUE;
  }
  else {
    return kFALSE;
  }
}

//________________________________________________________________
void CbmStsTrackletCuts::Reset() {

  fClosestDist = 1.e7;
  fClosestHit = NULL;
}

//________________________________________________________________
void CbmStsTrackletCuts::VerifyTracklet(const CbmStsCMPoint *start_hit,
						CbmStsCMPoint *hit,
						Bool_t backward) {

  Double_t dist = CalcDistance(start_hit, hit);

  if (TrackletAngle(start_hit, hit) < fMaxAngleTracklet) {

    if ( dist < fClosestDist ) {
      // hit found that is closer than the hits before
      fClosestDist = dist;
      fClosestHit = hit;

    }     // if (dist < closest_dist)
  }

}

//________________________________________________________________
Double_t const CbmStsTrackletCuts::CalcDistance(const CbmStsCMPoint *hit1,
					     const CbmStsCMPoint *hit2) {

// Returns the distance of two given clusters.

  Double_t diff;
  Double_t alpha_diff = TMath::Abs( hit1->GetAlpha() - hit2->GetAlpha() );

  diff = TMath::Abs(hit1->GetStation() - hit2->GetStation()) * alpha_diff;
  return diff;

}

//________________________________________________________________
Double_t const CbmStsTrackletCuts::TrackletAngle(const CbmStsCMPoint *hit1,
						 const CbmStsCMPoint *hit2) {

// Returns the angle between vectors from tracklet hits

  TVector3 v1( hit1->GetX(), hit1->GetY(), hit1->GetZ());
  TVector3 v2( hit2->GetX(), hit2->GetY(), hit2->GetZ());

  return v1.Angle(v2);
}
