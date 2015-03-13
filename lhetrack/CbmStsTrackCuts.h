//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// CbmStsTrackCuts class -                                                      //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef CBM_STS_TRACK_CUTS_H
#define CBM_STS_TRACK_CUTS_H

#include "TArrayD.h"
#include "CbmStsCMTrack.h"
#include "CbmStsCMPoint.h"

class CbmStsTrackCuts {

protected:

  // Cuts

  Int_t fTrackMinPoints;       // minimum number of points on one track

  Double_t fMaxMergedCircleDist;     // limit of circle distance 
  Double_t fMaxMergedDeepDist;       // limit of circle distance 
  Double_t fMergedTracksDist;        // limit of distanse between merged tracks

  Double_t fCutCircleDist;     // limit of circle distance 
  Double_t fCutDeepDist;       // limit of
  Double_t fCutDist;           // limit of max distanse between hits in track

//   Double_t fChi2Circle;        // limit of circle distance 
//   Double_t fChi2Deep;          // limit of pseudorapidity between to subsequent hits

  Int_t fStationScopeTrack;    // number of stations to look for the next point of a track
  Int_t fBetaScope;            // number of phi segments to look for the next point
  Int_t fAlphaScope;           // number of eta segments to look for the next point

  Int_t fEqualHits;            // circle fit and length fit returned differnet hits
  Int_t fDiffHitsStill;        // different hits from fits could not be resolved

  Double_t fClosestCircleDist;
  CbmStsCMPoint *fClosestCircleHit;

  Double_t fClosestDeepDist;
  CbmStsCMPoint *fClosestDeepHit;

public:

  CbmStsTrackCuts();               // default constructor
  virtual ~CbmStsTrackCuts();      // destructor
  
  // Setters
  void Reset();

  void SetPrimaryHighMomTrackCuts();
  void SetPrimaryIntermedMomTrackCuts();
  void SetPrimaryLowMomTrackCuts();
  void SetPrimaryExtTrackCuts();
  void SetPrimaryWideExtTrackCuts();

  void SetSecondaryHighMomCuts();
  void SetSecondaryHighMomExtCuts();
  void SetSecondaryLowMomCuts();
  void SetSecondaryLowMomExtCuts();

  // Getters

  Int_t GetMinPoints()    { return fTrackMinPoints; }             //
  Int_t GetStationScopeTrack()   { return fStationScopeTrack; }   //
  Int_t GetBetaScope()     { return fBetaScope; }                 //
  Int_t GetAlphaScope()     { return fAlphaScope; }               //
  Int_t GetNumEqualHits()  { return fEqualHits;}                  // 
  CbmStsCMPoint *GetClosestHit(CbmStsCMTrack *track);
  CbmStsCMPoint *GetCircleHit()    { return fClosestCircleHit; }  //
  CbmStsCMPoint *GetDeepHit()      { return fClosestDeepHit; }    //

  // Others

  Double_t const CalcDistance(Double_t Xh,
			      Double_t Yh,
			      Double_t *coeff);
  void VerifyTrack(CbmStsCMTrack *track, CbmStsCMPoint *hit);
  Bool_t DistBetweenTracks(CbmStsCMTrack *track1, CbmStsCMTrack *track2);
  Double_t TrackHitDist(CbmStsCMTrack *t1, CbmStsCMPoint *hit);

  void TrackFit(CbmStsCMTrack *track, Double_t *a);
  void LineFit(TArrayD *x, TArrayD *delx,
	       TArrayD *y, TArrayD *dely,
	       Double_t *a, Int_t np);

  ClassDef(CbmStsTrackCuts, 1)  //
    
    };

#endif
