///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// CbmStsTrackCuts class - representation for track tools and cuts               //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////


#include "CbmStsTrackCuts.h"
#include "CbmStsTrackletCuts.h"

#include "iomanip.h"

ClassImp(CbmStsTrackCuts)


//________________________________________________________________
CbmStsTrackCuts::CbmStsTrackCuts() {
}


//________________________________________________________________
CbmStsTrackCuts::~CbmStsTrackCuts() {
}

//________________________________________________________________
void CbmStsTrackCuts::Reset() {

  fClosestCircleDist = 1.e7;
  fClosestCircleHit = NULL;

  fClosestDeepDist = 1.e7;
  fClosestDeepHit = NULL;

}

//________________________________________________________________
void CbmStsTrackCuts::VerifyTrack(CbmStsCMTrack *track,
				  CbmStsCMPoint *hit) {

  // track search - look for nearest neighbor to  extrapolated track
		
  Double_t *coeff = new Double_t[6];

  TrackFit(track, coeff);
  Double_t circle_dist = CalcDistance(hit->GetXprime(), hit->GetZprime(), coeff+0);
  Double_t deep_dist = CalcDistance(hit->GetZv(), hit->GetYv(), coeff+2);

  if (coeff) {
    delete[] coeff; 
    coeff = NULL;
  }


  if (deep_dist < fCutDeepDist && circle_dist < fCutCircleDist ) {

    if ( circle_dist < fClosestCircleDist ) { 
      fClosestCircleDist = circle_dist;
      fClosestCircleHit = hit;

    }		


    if ( deep_dist < fClosestDeepDist ) { 
      fClosestDeepDist = deep_dist;
      fClosestDeepHit = hit;

    }
  }		


}

//________________________________________________________________
  CbmStsCMPoint* CbmStsTrackCuts::GetClosestHit(CbmStsCMTrack *track) {

    if (GetCircleHit() && GetDeepHit()) {

      if (GetCircleHit() == GetDeepHit()) {
	fEqualHits++;

	if (TrackHitDist(track, GetCircleHit()) < fCutDist)
	  return fClosestCircleHit;
      }
      else {

      }
    }

    return NULL;

}

//________________________________________________________________
Double_t const CbmStsTrackCuts::CalcDistance(Double_t Xh,
					     Double_t Yh,
					     Double_t *coeff) {
// Returns the distance of a point to a straight line.
// The point is given by the to conformal coordinates of a cluster and the
// straight line is given by its to coefficients: y = coeff[0]*x + coeff[1].

  Double_t x = (coeff[0] / (1 + coeff[0]*coeff[0])) *
    (1/coeff[0] * Xh + Yh - coeff[1]);

  return TMath::Sqrt(TMath::Power(x - Xh, 2) +
    TMath::Power(coeff[0]*x + coeff[1] - Yh, 2));
}

//________________________________________________________________
void CbmStsTrackCuts::SetPrimaryHighMomTrackCuts() {
// Sets cuts of tracks for the given vertex constraint.

  fEqualHits = 0;
  fDiffHitsStill = 0;

  fTrackMinPoints = 3;
  fStationScopeTrack = 1;

  fBetaScope = 1;    // beta
  fAlphaScope = 0;   // alpha

  fCutDist = .001; 
  fCutCircleDist = 1.e-4;
  fCutDeepDist = .1;

  fMergedTracksDist = .0006;
  fMaxMergedCircleDist = .034;
  fMaxMergedDeepDist = .004;

}

//________________________________________________________________
void CbmStsTrackCuts::SetPrimaryExtTrackCuts() {
// Sets cuts of tracks for the given vertex constraint.

  fCutDist = .0015; 
  fCutCircleDist = 2.e-4;
  fCutDeepDist = .15;

}

//________________________________________________________________
void CbmStsTrackCuts::SetPrimaryIntermedMomTrackCuts() {

  fBetaScope = 2;
  fAlphaScope = 1;

  fCutDist = .006;
  fCutCircleDist = 1.05e-4;
  fCutDeepDist = .6; 

  fMergedTracksDist = .0012;
  fMaxMergedCircleDist = .034;
  fMaxMergedDeepDist = .015;

}

//________________________________________________________________
void CbmStsTrackCuts::SetPrimaryLowMomTrackCuts() {
// Sets cuts of tracks for the given vertex constraint.

  fTrackMinPoints = 3;

  fBetaScope = 10;
  fAlphaScope = 2;

  fCutDist = .012;

  fCutCircleDist = 1.7e-4;
  fCutDeepDist = .7;

  fMergedTracksDist = .001;
  fMaxMergedCircleDist = .1;
  fMaxMergedDeepDist = .04;

}

//________________________________________________________________
void CbmStsTrackCuts::SetPrimaryWideExtTrackCuts() {
// Sets cuts of tracks for the given vertex constraint.

  fMergedTracksDist = .0014;

}

//________________________________________________________________
void CbmStsTrackCuts::SetSecondaryHighMomCuts() {

  fTrackMinPoints = 2;

  fBetaScope = 1;
  fAlphaScope = 1;
  fStationScopeTrack = 1;

  fMaxMergedCircleDist = .003;
  fMergedTracksDist = .015;

  fCutCircleDist = .003;
  fCutDeepDist = .005;

}

//________________________________________________________________
void CbmStsTrackCuts::SetSecondaryLowMomCuts() {

  fTrackMinPoints = 2;
  fBetaScope = 2;
  fAlphaScope = 1;

  fMaxMergedCircleDist = .003;
  fMergedTracksDist = .015;

  fCutCircleDist = .003;
  fCutDeepDist = .005;

}

//________________________________________________________________
void CbmStsTrackCuts::SetSecondaryLowMomExtCuts() {

  fTrackMinPoints = 2;
  fBetaScope = 2;
  fAlphaScope = 1;

  fCutCircleDist = .003;
  fCutDeepDist = .005;

  fMaxMergedCircleDist = .034;
  fMaxMergedDeepDist = .004;
  fMergedTracksDist = .015;

}

//________________________________________________________________
void CbmStsTrackCuts::SetSecondaryHighMomExtCuts() {

  fMaxMergedCircleDist = .003;
  fMergedTracksDist   = .015;

  fCutCircleDist = .003;
  fCutDeepDist   = .005;

}

//________________________________________________________________
Bool_t CbmStsTrackCuts::DistBetweenTracks(CbmStsCMTrack *t1,
					   CbmStsCMTrack *t2) {

  Double_t *coeff1 = new Double_t[6];
  Double_t *coeff2 = new Double_t[6];

  TrackFit(t1, coeff1);
  TrackFit(t2, coeff2);
  
  Double_t TracksCircleAngle = TMath::Abs((coeff1[0] - coeff2[0]) /
				 (1. + coeff1[0] *coeff2[0]) );

  Double_t TracksDeepAngle = TMath::Abs((coeff1[2] - coeff2[2]) /
			       (1. + coeff1[2] *coeff2[2]) );

  if (coeff1) {
    delete[] coeff1; 
    coeff1 = NULL;
  }
  if (coeff2) {
    delete[] coeff2; 
    coeff2 = NULL;
  }

  if (TracksDeepAngle > fMaxMergedDeepDist) return kFALSE;
  if (TracksCircleAngle > fMaxMergedCircleDist) return kFALSE;

  CbmStsCMTrack *fst = NULL;
  CbmStsCMTrack *scd = NULL;

  if( t1->GetMask() < t2->GetMask()) {
    fst = t2;
    scd = t1; 
  }
  else {
    fst = t1;
    scd = t2; 
  }

  CbmStsCMPoint *hit1 =  (CbmStsCMPoint*)(fst->GetCMHits())->Last();
  CbmStsCMPoint *hit2 =  (CbmStsCMPoint*)(scd->GetCMHits())->First();

  Double_t alpha_diff = TMath::Abs( hit1->GetAlpha() - hit2->GetAlpha() );

  Double_t dist = TMath::Abs(hit1->GetStation() - hit2->GetStation()) * alpha_diff;

  if (dist < fMergedTracksDist)
    return kTRUE;
  else
    return kFALSE;

}

//________________________________________________________________
Double_t CbmStsTrackCuts::TrackHitDist(CbmStsCMTrack *t1,
				       CbmStsCMPoint *hit) {

  CbmStsCMPoint *hit1 =  (CbmStsCMPoint*)(t1->GetCMHits())->Last();

  Double_t alpha_diff = TMath::Abs( hit1->GetAlpha() - hit->GetAlpha() );
  Double_t dist = TMath::Abs(hit1->GetStation() - hit->GetStation()) * alpha_diff;

  return dist;

}

//________________________________________________________________
void CbmStsTrackCuts::TrackFit(CbmStsCMTrack *track,
			       Double_t *a) {

  TObjArray *trackpoints = track->GetCMHits();
  Int_t n = trackpoints->GetEntriesFast();

  CbmStsCMPoint *trackpoint = NULL;

  TArrayD *x = new TArrayD();       x->Set(n);
  TArrayD *y = new TArrayD();       y->Set(n);
  TArrayD *delx = new TArrayD();    delx->Set(n);
  TArrayD *dely = new TArrayD();    dely->Set(n);

  for (Int_t i = 0; i < n; i++) {
    trackpoint = (CbmStsCMPoint *)trackpoints->At(i);
    x->AddAt(trackpoint->GetXprime(), i);  
    y->AddAt(trackpoint->GetZprime(), i);  
    
    delx->AddAt(1., i);  
    dely->AddAt(1., i);  
  }

  Double_t coeff[3];

  LineFit(x, delx, y, dely, coeff, n);

  a[0] = coeff[0];
  a[1] = coeff[1];
  
  track->SetChi2Circle(coeff[2]);

  // estimate track radius
  Double_t D = TMath::Sqrt((coeff[0]*coeff[0] + 1.) / (4.* coeff[1]*coeff[1]));
  track->SetRadius(D);

  for (Int_t j = 0; j < n; j++) {
    trackpoint = (CbmStsCMPoint *)trackpoints->At(j);
    x->AddAt(trackpoint->GetZv(), j);  
    y->AddAt(trackpoint->GetYv(), j);  
    
    delx->AddAt(1., j);  
    dely->AddAt(1., j);  
  }

  LineFit(x, delx, y, dely, coeff, n);

  a[2]= coeff[0];
  a[3]= coeff[1];
  
  track->SetTanAlpha(coeff[0]);
  track->SetChi2Deep(coeff[2]);

    delete x;
    delete y;
    delete delx;
    delete dely;

}

//________________________________________________________________
void CbmStsTrackCuts::LineFit(TArrayD *x, TArrayD *delx, 
			      TArrayD *y, TArrayD *dely,
			      Double_t *coeff,
			      Int_t np) {

  Double_t L11 = 0.;
  Double_t L12 = 0.;
  Double_t L22 = 0.;
  Double_t  g1 = 0.;
  Double_t  g2 = 0.;

  for (Int_t i = 0; i < np; i++) {

    L11 += 1.;
    L12 +=  x->At(i);
    L22 += x->At(i) * x->At(i);
    g1  += y->At(i);
    g2  += x->At(i) * y->At(i);
  }

  Double_t D = L11*L22 - L12*L12;

  coeff[0] = (g2*L11 - g1*L12)/D;
  coeff[1] = (g1*L22 - g2*L12)/D;

// Calculate chi squared

  Double_t chi2 = 0.;

  if (np > 2) {

    for ( Int_t i = 0; i < np; i++) {

      chi2 += TMath::Power(y->At(i) - coeff[0] * x->At(i) - coeff[1], 2.) /
	TMath::Abs(coeff[0] * x->At(i) + coeff[1]);
				 
    }
    chi2 = chi2/float(np - 2);

  }
  coeff[2] = chi2;

}
