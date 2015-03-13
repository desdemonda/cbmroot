//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// CbmStsTrackletCuts class - representation of tracklets                       //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef CBM_STS_TRACKLET_CUTS_H
#define CBM_STS_TRACKLET_CUTS_H

#include "CbmStsCMPoint.h"

class CbmStsTrackletCuts {

protected:

  // Cuts
  Int_t    fTrackletMinPoints;       // minimal number of stations for tracklets
  Int_t    fStationScopeTracklet;       // minimal number of stations for tracklets

  Double_t fMaxAngleTracklet;    // limit of space angle between tracklet hits
  Double_t fMaxAlphaTracklet;    // limit of deep angle between tracklet hits
  Double_t fMaxPhiTracklet;      // limit of phi angle between tracklet hits

  Double_t fClosestDist;
  CbmStsCMPoint *fClosestHit;

public:

  CbmStsTrackletCuts();                   //
  virtual ~CbmStsTrackletCuts();          //
  
  // Setters
  void SetTrackletMinPoints(Int_t f)     { fTrackletMinPoints = f; }

  void SetMaxAngleTracklet(Double_t f)   { fMaxAngleTracklet = f; }  // sets angle cuts

  void SetPrimaryTrackletCuts();
  void SetPrimaryIntermedMomTrackletCuts();
  void SetPrimaryLowTrackletCuts();
  void SecondaryHighTrackletCuts();
  void SecondaryLowTrackletCuts();
  void Reset();


  // Getters
  Double_t GetMaxAngleTracklet()  const { return fMaxAngleTracklet; }      // gets angle cuts
  Int_t GetTrackletMinPoints()    const { return fTrackletMinPoints; }     //
  Int_t GetStationScopeTracklet() const { return fStationScopeTracklet; }  //
  CbmStsCMPoint *GetClosestHit()  const { return fClosestHit; }
  //
  Double_t const CalcDistance(const CbmStsCMPoint *hit1, const CbmStsCMPoint *hit2);
  Bool_t CheckTracklet(Double_t ang);   //
  void VerifyTracklet(const CbmStsCMPoint *start_hit,
			      CbmStsCMPoint *hit,
			      Bool_t backward);
  Double_t const TrackletAngle(const CbmStsCMPoint *hit1,
				const CbmStsCMPoint *hit2);

  ClassDef(CbmStsTrackletCuts, 1)  //
    
    };

#endif
