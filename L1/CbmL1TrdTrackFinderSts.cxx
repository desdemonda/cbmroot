// -----------------------------------------------------------------------
// -----                    CbmL1TrdTrackFinderSts                   -----
// -----                 Created 6/12/05  by D. Kresan               -----
// -----------------------------------------------------------------------

#include "CbmL1TrdTrackFinderSts.h"

#include "CbmL1Def.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairBaseParSet.h"
#include "FairRuntimeDb.h"
#include "FairDetector.h"
#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmTrdTrack.h"
#include "CbmKFTrack.h"
#include "CbmKFTrdHit.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TArc.h"
#include "TLine.h"
#include "TMath.h"

#include <iostream>
#include <vector>
#include <map>

using std::cout;
using std::endl;
using std::vector;
using std::map;


//________________________________________________________________________
//
// CbmL1TrdTrackFinderSts
//
// Engine for track finding in the TRD. Based on the track extrapolation
// from the STS and further track following.
//



// -----------------------------------------------------------------------
CbmL1TrdTrackFinderSts::CbmL1TrdTrackFinderSts():
    fEvents(0),                   // Number of events processed
fVerbose(1),                  // Verbosity level
fArrayTrdPoint(NULL),            // Array of TRD points
fArrayTrdHit(0),              // Array of TRD hits
fArrayStsTrack(NULL),            // Array of STS tracks
fArrayStsTrackM(NULL),           // Array of STS tracks
fArrayKFTrdHit(new TClonesArray("CbmKFTrdHit")),            // Array of KF TRD hits
fvTrdTrack(),
fArrayTrdTrack(NULL),            // Output Array of TRD tracks
fPid(211),                      // PID assumption
fNoTrdStations(0),            // Number of TRD stations
fNoTrdPerStation(0),          // Number of TRD layers per station
fmapHitUsed(),
fh_chi2hit(0),                // Control histogramm
fh_chi2hit_plane(0),          // Control histogramm
fh_resx_plane_true(0),        // Control histogramm
fh_resy_plane_true(0),        // Control histogramm
fh_resx_plane_fake(0),        // Control histogramm
fh_resy_plane_fake(0),        // Control histogramm
fh_resx_mom_true(0),          // Control histogramm
fh_resy_mom_true(0),          // Control histogramm
fh_pullx_plane_true(0),        // Control histogramm
fh_pully_plane_true(0),        // Control histogramm
fh_pullx_plane_fake(0),        // Control histogramm
fh_pully_plane_fake(0),        // Control histogramm
fLostTracks()
{
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
CbmL1TrdTrackFinderSts::CbmL1TrdTrackFinderSts(Int_t verbose):
    fEvents(0),                   // Number of events processed
fVerbose(verbose),                  // Verbosity level
fArrayTrdPoint(NULL),            // Array of TRD points
fArrayTrdHit(0),              // Array of TRD hits
fArrayStsTrack(NULL),            // Array of STS tracks
fArrayStsTrackM(NULL),           // Array of STS tracks
fArrayKFTrdHit(new TClonesArray("CbmKFTrdHit")),            // Array of KF TRD hits
fvTrdTrack(),
fArrayTrdTrack(NULL),            // Output Array of TRD tracks
fPid(211),                      // PID assumption
fNoTrdStations(0),            // Number of TRD stations
fNoTrdPerStation(0),          // Number of TRD layers per station
fmapHitUsed(),
fh_chi2hit(0),                // Control histogramm
fh_chi2hit_plane(0),          // Control histogramm
fh_resx_plane_true(0),        // Control histogramm
fh_resy_plane_true(0),        // Control histogramm
fh_resx_plane_fake(0),        // Control histogramm
fh_resy_plane_fake(0),        // Control histogramm
fh_resx_mom_true(0),          // Control histogramm
fh_resy_mom_true(0),          // Control histogramm
fh_pullx_plane_true(0),        // Control histogramm
fh_pully_plane_true(0),        // Control histogramm
fh_pullx_plane_fake(0),        // Control histogramm
fh_pully_plane_fake(0),        // Control histogramm
fLostTracks()
{
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
CbmL1TrdTrackFinderSts::~CbmL1TrdTrackFinderSts()
{
    // Destructor
  if(fArrayKFTrdHit)fArrayKFTrdHit->Delete();
  delete fArrayKFTrdHit;
  // why delete the trdtrack array? This pointer
  // is passed from CbmTrdFindTracks and is registered with the iomanager...
  // should be not our business to delete the TClonesArray pointer
  if(fArrayTrdTrack)fArrayTrdTrack->Delete();
  //  delete fArrayTrdTrack;
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::Init()
{
    // Initialisation of the algorithm

    // Create histogramms
    CreateHistogramms();
    // Activate data branches
    DataBranches();
    // Determine the TRD layout
    TrdLayout();
//    fNoTrdStations = 3;
//    fNoTrdPerStation = 4;
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
Int_t CbmL1TrdTrackFinderSts::DoFind(TClonesArray* hitArray,
				     TClonesArray* trackArray)
{
  // Implementation of the track finding algorithm
  if(NULL == hitArray) {
    return 0;
  }
  fArrayTrdHit = hitArray;
  fArrayTrdTrack = trackArray;

  fmapHitUsed.clear();
  fLostTracks.clear();

  // Sort the TRD hits by plane number
  SortTrdHits();

  // Main part of algorithm
  // Follow the track in TRD

  Process();

  Int_t nTrdTracks = fArrayTrdTrack->GetEntriesFast();

  // Event output
  if(fVerbose > 0) {
    cout << "---------------------------------------" << endl
	 << "-I-     CbmL1TrdTrackFinderSts      -I-" << endl
	 << "-I-         Event summary           -I-" << endl
	 << "-I-        STS tracks : "
	 << fArrayStsTrack->GetEntriesFast() << endl
	 << "-I-  TRD tracks found : " << nTrdTracks << endl
	 << "-I-  TRD tracks lost  : " << fLostTracks.size() << endl
	 << "---------------------------------------" << endl
	 << endl;
  } else {
    cout << "-I- CbmL1TrdTrackFinderSts::DoFind : "
	 << nTrdTracks << " TRD tracks found." << endl;
  }

  // Clear array of KF trd hits
  fArrayKFTrdHit->Clear();

  // Increment number of events
  fEvents += 1;

  // control output
  cout << "-I- CbmL1TrdTrackFinder : "
       << fEvents << " events processed" << endl;

  // Return number of found TRD tracks
  return nTrdTracks;
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::Process()
{
    // Create TRD tracks from STS tracks, process them throug all stations
    // and move to the output array

/*    new TCanvas("c1", "", 10, 10, 800, 800);
    Int_t n;
    Double_t x[10000], y[10000];
    Int_t index;
    CbmTrdPoint *pt;
    TVector3 pos;
    for(Int_t i = 0; i < fNoTrdHits[0]; i++) {
	index = fTrdHitIndex[0][i];
	pt = (CbmTrdPoint*) fArrayTrdPoint->At(index);
	if(NULL == pt) continue;
	pt->Position(pos);
	x[n] = pos.X();
	y[n] = pos.Y();
        n += 1;
    }
    TGraph *gr = new TGraph(n, x, y);
    gr->SetMarkerStyle(24);
    gr->SetMarkerColor(4);
    gr->SetMarkerSize(0.5);
    gr->SetTitle("first layer");
    gr->Draw("AP");
    gr->GetHistogram()->GetXaxis()->SetRangeUser(0., 273.);
    gr->GetHistogram()->GetXaxis()->SetTitle("x (cm)");
    gr->GetHistogram()->GetYaxis()->SetRangeUser(0., 273.);
    gr->GetHistogram()->GetYaxis()->SetTitle("y (cm)");*/


    Sts2Trd(0., 1e16,
	    0., 1e16);

    ProcessAllStations();
//    RemoveFakes();
    MoveOut();

/*
    Sts2Trd(0., 1.,
	    0., 1e16);
    ProcessAllStations();
//    RemoveFakes();
    MoveOut();*/
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::Sts2Trd(Double_t pmin, Double_t pmax,
				     Double_t chi2min, Double_t chi2max)
{
    // Create TRD track from each STS track, that fullfill the selection
    // criteria

    Int_t nStsTrack = fArrayStsTrack->GetEntriesFast();
    CbmStsTrack* stsTrack = NULL;
    CbmTrdTrack* trdTrack = NULL;
/*    Double_t mom;
    Double_t chi2;*/
    // Loop over STS tracks
    for(Int_t iStsTrack = 0; iStsTrack < nStsTrack; iStsTrack++) {
        // Get pointer to the STS track
        stsTrack = L1_DYNAMIC_CAST<CbmStsTrack*>( fArrayStsTrack->At(iStsTrack) );
	if(NULL == stsTrack) continue;
/*
	// Cut on momentum
        mom = 1./TMath::Abs(stsTrack->GetParamLast()->GetQp());
	if(mom < pmin || mom >= pmax) continue;
	// Cut on chi2
	chi2 = stsTrack->GetChi2() / (Double_t)stsTrack->GetNDF();
	if(chi2 < chi2min || chi2 >= chi2max) continue;
*/
        // Create TRD track
        trdTrack = new CbmTrdTrack();
        // Copy track parameters at plane of last hit
	trdTrack->SetParamLast( stsTrack->GetParamLast() );
        // Copy chi2
        trdTrack->SetChiSq(stsTrack->GetChiSq());
        trdTrack->SetNDF(stsTrack->GetNDF());
        // Set sts track index
        trdTrack->SetPreviousTrackId(iStsTrack);
        // Add it to the array
        fvTrdTrack.push_back(trdTrack);
        // Control output
        if(fVerbose > 1) {
            cout << "TRD track created from STS track " << iStsTrack
		 << endl;
        }
    }
//    sort(fvTrdTrack.begin(), fvTrdTrack.end(), CbmTrdTrack::CompareMomentum);
}
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::ProcessAllStations()
{
  // Process all tracks through all stations

  // Loop over tracks
  for(Int_t station = 0; station < fNoTrdStations; station++) {
    vector<CbmTrdTrack*>::iterator iter;
    int itr=0;
    for(iter = fvTrdTrack.begin(); iter != fvTrdTrack.end(); iter++) {
      // Attach hits to track
      ProcessStation(*iter, station);
      // Update track
      UpdateTrack(station, *iter);
      itr++;
    }
    Clear();
    if(fVerbose > 0) {
      cout << "track candidates: " << fvTrdTrack.size() << "." << endl;
    }
  }
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::MoveOut()
{
  // Move the tracks from temporary array to the output array
  vector<CbmTrdTrack*>::iterator iter;
  CbmTrdTrack* track;
  Int_t nOut = fArrayTrdTrack->GetEntries();
  for(iter = fvTrdTrack.begin(); iter != fvTrdTrack.end(); iter++) {
    track = *iter;
    if(0 == track->GetFlag()) {
      new ((*fArrayTrdTrack)[nOut]) CbmTrdTrack(*track);
      nOut += 1;
    }
    delete track;
  }
  fvTrdTrack.clear();
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::ProcessStation(CbmTrdTrack* pTrack,
                                            const Int_t& station)
{
  // Extrapolate track parameters to the layers of current station,
  // and pick up the closest hits

  // Track parameters

  CbmKFTrack kfTrack;
  kfTrack.SetTrackParam(*(const_cast<FairTrackParam*>(pTrack->GetParamLast())));
  kfTrack.SetPID(fPid);
  Double_t qp0 = pTrack->GetParamLast()->GetQp();
  Double_t *T = kfTrack.GetTrack();

  // Declare variables outside the loop
  Int_t plane = 0;
  Double_t ze = 0.;
  Int_t hitIndex = 0;
  CbmTrdHit* pHit = NULL;
  Double_t chi2hit = 0;

  Int_t indexOfClosest;
  Double_t minChi2;
//   CbmTrdHit* closestHit = 0;
  Int_t pointIndex;
  CbmTrdPoint *trdPoint;
  TVector3 pos;

  Int_t stsTrackIndex = pTrack->GetPreviousTrackId();
  if(stsTrackIndex < 0) {
    Fatal("ProcessStation", "Invalid track index");
  }
  CbmTrackMatch *stsM = L1_DYNAMIC_CAST<CbmTrackMatch*>( fArrayStsTrackM->
    At(stsTrackIndex) );
  Int_t trackID = stsM->GetMCTrackId();

  // Loop over layers in this station
  for(Int_t iLayer = 0; iLayer < fNoTrdPerStation; iLayer++) {

    if(pTrack->GetFlag()) {
      return;
    }
    // Plane number
    plane =  station*fNoTrdPerStation + iLayer;
    // Skip if no TRD hits
    if(fNoTrdHits[plane] < 1) continue;
    // Get z coordinate of plane
    ze = (L1_DYNAMIC_CAST<CbmTrdHit*>( fArrayTrdHit->At(fTrdHitIndex[plane][0]) ) )->GetZ();
    // Extrapolate to the plane
    kfTrack.Extrapolate(ze, &qp0);

    minChi2 = 1e16;
    indexOfClosest = -1;

    // Loop over TRD hits in this plane
    for(Int_t iHit = 0; iHit < fNoTrdHits[plane]; iHit++) {

      // Get hit index
      hitIndex = fTrdHitIndex[plane][iHit];
      // If the hit is used, skip
      if(fmapHitUsed[hitIndex]) continue;
      // Get pointer to the hit
      pHit = L1_DYNAMIC_CAST<CbmTrdHit*>( fArrayTrdHit->At(hitIndex) );
      // Get MC point
      pointIndex = pHit->GetRefId();
      trdPoint = L1_DYNAMIC_CAST<CbmTrdPoint*>( fArrayTrdPoint->At(pointIndex) );
      trdPoint->Position(pos);

      Double_t c1 = kfTrack.GetCovMatrix()[0];
      Double_t c2 = kfTrack.GetCovMatrix()[2];
      if( finite(c1) && c1>1.e-10 ) c1 = ( T[0]-pos.X())/TMath::Sqrt(c1);
      else c1 = 100;
      if( finite(c2) && c2>1.e-10 ) c2 = ( T[1]-pos.Y())/TMath::Sqrt(c2);
      else c2 = 0;

      if(trdPoint->GetTrackID() == trackID) {
	fh_resx_plane_true->Fill( T[0]-pos.X(), plane);
	fh_resy_plane_true->Fill( T[1]-pos.Y(), plane);
	fh_pullx_plane_true->Fill( c1, plane);
	fh_pully_plane_true->Fill( c2, plane);
	/*		if(0==plane && kfTrack.GetTrack()[0]>0 && kfTrack.GetTrack()[1]>0) {
			TArc *arc = new TArc(kfTrack.GetTrack()[0], kfTrack.GetTrack()[1], TMath::Max(TMath::Sqrt(kfTrack.GetCovMatrix()[0]),
			TMath::Sqrt(kfTrack.GetCovMatrix()[2])));
			arc->SetLineColor(2);
		    arc->Draw();
		    TLine *line  = new TLine(kfTrack.GetTrack()[0], kfTrack.GetTrack()[1], pos.X(), pos.Y());
		    line->Draw();

		    fh_resx_mom_true->Fill(kfTrack.GetTrack()[0]-pos.X(), 1./TMath::Abs(qp0));
		    fh_resy_mom_true->Fill(kfTrack.GetTrack()[1]-pos.Y(), 1./TMath::Abs(qp0));
		}*/
      } else {

	fh_resx_plane_fake->Fill( T[0]-pos.X(), plane);
	fh_resy_plane_fake->Fill( T[1]-pos.Y(), plane);
	fh_pullx_plane_fake->Fill( c1, plane);
	fh_pully_plane_fake->Fill( c2, plane);
      }
      // Check for geometrical overlap
      if(kFALSE == Overlap(kfTrack, pHit)) {
	if(trdPoint->GetTrackID()==trackID) {
	  if(fVerbose > 1) {
	    cout << "-W- Lost true hit:   plane:" << plane << "   track: ("
		 << T[0] << ", " << T[1] << ") ("
		 << TMath::Sqrt(TMath::Abs(kfTrack.GetCovMatrix()[0])) << ", " << TMath::Sqrt(TMath::Abs(kfTrack.GetCovMatrix()[2]))
		 << "),   hit: (" << pHit->GetX() << ", " << pHit->GetY() << ") ("
		 << pHit->GetDx() << ", " << pHit->GetDy() << "),   point: ("
		 << pos.X() << ", " << pos.Y() << ")"
		 << endl;
	  }
	  fLostTracks[trackID] = kTRUE;
	}

	continue;
      }
      // Calculate normalised distance to hit
      chi2hit = GetChi2Hit(kfTrack, pHit);
      // Fill histogramms
      fh_chi2hit->Fill(chi2hit);
      fh_chi2hit_plane->Fill(chi2hit, plane);
      // Take the smallest chi2
      if(chi2hit < minChi2) {
	minChi2 = chi2hit;
	indexOfClosest = hitIndex;
// 	closestHit = pHit;
      }
    }// Loop over TRD hits

    // Add hit to the track
    if(indexOfClosest != -1) {
      pTrack->AddHit(indexOfClosest, kTRDHIT);
    } else {
      pTrack->SetFlag(1);
    }
  } // Loop over layers
//  pTrack->SortHits();
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::UpdateTrack(Int_t station, CbmTrdTrack* pTrack)
{
    // Update track parameters using Kalman Filter

    // Get number of hits
    Int_t nHits = pTrack->GetNofHits();
    if( nHits < static_cast<Int_t>( (station+1)*fNoTrdPerStation ) ) {
        pTrack->SetFlag(1);
        return;
    }
    // Kalman filter track
    CbmKFTrack kfTrack;
    kfTrack.SetTrackParam(*(const_cast<FairTrackParam*>(pTrack->GetParamLast())));
    kfTrack.GetRefChi2() = pTrack->GetChiSq();
    kfTrack.GetRefNDF() = pTrack->GetNDF();
    kfTrack.SetPID(fPid);
    // Loop over hits
    Int_t hitIndex = 0;
    CbmTrdHit *pHit = NULL;
    CbmKFTrdHit *pKFHit = NULL;
    Double_t qp0 = pTrack->GetParamLast()->GetQp();
    for(Int_t iHit = static_cast<Int_t>( station*fNoTrdPerStation );
	iHit < nHits; iHit++) {
	// Get hit index
        hitIndex = pTrack->GetHitIndex(iHit);
        // Get pointer to the hit
        pHit = L1_DYNAMIC_CAST<CbmTrdHit*>( fArrayTrdHit->At(hitIndex) );
        // Extrapolate to this hit
        kfTrack.Extrapolate(pHit->GetZ());
        // Get KF TRD hit
        pKFHit = L1_DYNAMIC_CAST<CbmKFTrdHit*>( fArrayKFTrdHit->At(hitIndex) );
        // Add measurement
        pKFHit->Filter(kfTrack, kTRUE, qp0);
    } // Loop over hits
    // Set track parameters
    kfTrack.GetTrackParam( *(const_cast<FairTrackParam*>(pTrack->GetParamLast())) );
    pTrack->SetChiSq(kfTrack.GetRefChi2());
    pTrack->SetNDF(kfTrack.GetRefNDF());
    if(station == (fNoTrdStations-1)) {
	if(pTrack->GetChiSq()/static_cast<Double_t>(pTrack->GetNDF()) > 100) pTrack->SetFlag(1);
    }
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::Clear(const Option_t* a)
{
    // Delete bad tracks from track array
    CbmTrdTrack *track;
    for(vector<CbmTrdTrack*>::iterator iter = fvTrdTrack.begin();
	iter != fvTrdTrack.end(); iter++) {
	track = *iter;
	if(0 != track->GetFlag()) {
	    fvTrdTrack.erase(iter);
            iter--;
	    delete track;
	}
    }
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::RemoveFakes()
{
    // Remove ghost tracks from the track candidate array. Among two
    // tracks with common hits, priority has one with smaller chi2.

    /** Map from sts track index to boolean flag **/
    map<Int_t, Bool_t> mapStsTrackUsed;

    // Sort found tracks by chi2
    sort(fvTrdTrack.begin(), fvTrdTrack.end(), CompareChi2);

    Int_t n_false;

    // Loop over sorted tracks
    CbmTrdTrack* track;
    Int_t nHits = 0;
    Int_t hitIndex = 0;
    vector<CbmTrdTrack*>::iterator iter;
    for(iter = fvTrdTrack.begin(); iter != fvTrdTrack.end();
        iter++) {
        // Pointer to the track
	track = *iter;
	if(track->GetFlag()) {
//	    fvTrdTrack.erase(iter);
//	    delete track;
	    continue;
	}

        // Loop over hits of this track, check if they are already
        // attached
	nHits = track->GetNofHits();
        n_false = 0;
        for(Int_t iHit = 0; iHit < nHits; iHit++) {
            // Get hit index
            hitIndex = track->GetHitIndex(iHit);
            // Check flag
	    if(fmapHitUsed[hitIndex]) {
                n_false += 1;
            }
        } // Loop over hits

//	if((Double_t)n_false/(Double_t)nHits > 0.3) {
	if(n_false > 0) {
	    track->SetFlag(1);
	}

        if( mapStsTrackUsed[ track->GetPreviousTrackId() ] ) {
            track->SetFlag(1);
        }

        // Skip the fake tracks
	if(track->GetFlag()) {
//	    fvTrdTrack.erase(iter);
//	    delete track;
	    continue;
	}

        // Mark hits as attached
        for(Int_t iHit = 0; iHit < nHits; iHit++) {
            // Get hit index
            hitIndex = track->GetHitIndex(iHit);
            // Set flag
            fmapHitUsed[hitIndex] = kTRUE;
        } // Loop over hits

        mapStsTrackUsed[ track->GetPreviousTrackId() ] = kTRUE;
    } // Loop over tracks
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
Bool_t CbmL1TrdTrackFinderSts::Overlap(CbmKFTrack& track, CbmTrdHit* pHit)
{
    // Check for geometrical overlap between track extrapolation and hit
    if( track.GetCovMatrix()[0] > 100 ) {
        return kFALSE;
    }
    if( track.GetCovMatrix()[2] > 100 ) {
        return kFALSE;
    }

    Bool_t overlap;
    if(pHit->GetDx() < 1e-9 && pHit->GetDy() < 1e-9) {
/*	Double_t x1 = track.GetTrack()[0];
	Double_t dx1 = TMath::Sqrt(track.GetCovMatrix()[0]);
	Double_t x2 = pHit->GetX();
	Bool_t overlap_x = ( ((x1+10*dx1) >= x2) &&
			    ((x1-10*dx1) <= x2) );
	Double_t y1 = track.GetTrack()[1];
	Double_t dy1 = TMath::Sqrt(track.GetCovMatrix()[2]);
	Double_t y2 = pHit->GetY();
	Bool_t overlap_y = ( ((y1+10*dy1) >= y2) &&
			    ((y1-10*dy1) <= y2) );
     		    overlap = overlap_x && overlap_y;*/
	Double_t x1 = track.GetTrack()[0];
	Double_t x2 = pHit->GetX();
	Bool_t overlap_x = TMath::Abs(x1-x2) < 7;
	Double_t y1 = track.GetTrack()[1];
	Double_t y2 = pHit->GetY();
	Bool_t overlap_y = TMath::Abs(y1-y2) < 7;
	overlap = overlap_x && overlap_y;
    } else {
/*	Double_t x1 = track.GetTrack()[0];
	Double_t dx1 = TMath::Sqrt(track.GetCovMatrix()[0]);
	Double_t x2 = pHit->GetX();
	Double_t dx2 = pHit->GetDx() * 1e-4;
	Bool_t overlap_x1 = ((x1+5*dx1) <= (x2+3*dx2)) &&
	    ((x1+5*dx1) >= (x2-3*dx2));
	Bool_t overlap_x2 = ((x1-5*dx1) <= (x2+3*dx2)) &&
	    ((x1-5*dx1) >= (x2-3*dx2));
	Bool_t overlap_x3 = ((x1+5*dx1) >= (x2+3*dx2)) &&
	    ((x1-5*dx1) <= (x2-3*dx2));
        Bool_t overlap_x = overlap_x1 || overlap_x2 || overlap_x3;
	Double_t y1 = track.GetTrack()[1];
	Double_t dy1 = TMath::Sqrt(track.GetCovMatrix()[2]);
	Double_t y2 = pHit->GetY();
	Double_t dy2 = pHit->GetDy() * 1e-4;
	Bool_t overlap_y1 = ((y1+5*dy1) <= (y2+3*dy2)) &&
	    ((y1+5*dy1) >= (y2-3*dy2));
	Bool_t overlap_y2 = ((y1-5*dy1) <= (y2+3*dy2)) &&
	    ((y1-5*dy1) >= (y2-3*dy2));
	Bool_t overlap_y3 = ((y1+5*dy1) >= (y2+3*dy2)) &&
	    ((y1-5*dy1) <= (y2-3*dy2));
        Bool_t overlap_y = overlap_y1 || overlap_y2 || overlap_y3;
        overlap = overlap_x && overlap_y;*/
	Double_t x1 = track.GetTrack()[0];
	Double_t x2 = pHit->GetX();
	Double_t dx2 = pHit->GetDx();
	Bool_t overlap_x = TMath::Abs(x1-x2) <= (1.117*3 + 3*dx2);
	Double_t y1 = track.GetTrack()[1];
	Double_t y2 = pHit->GetY();
	Double_t dy2 = pHit->GetDy();
	Bool_t overlap_y = TMath::Abs(y1-y2) <= (1.314*3 + 3*dy2);
        overlap = overlap_x && overlap_y;
    }
    return overlap;
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
Double_t CbmL1TrdTrackFinderSts::GetChi2Hit(CbmKFTrack& track, CbmTrdHit* pHit)
{
    // Get chi2 from track extrapolation to hit
    Double_t chi2 = 0.;
    if(pHit->GetDx() < 1e-14 && pHit->GetDx() > pHit->GetDy() && pHit->GetDy() < 1e-14) {
	Double_t dx = track.GetTrack()[0] - pHit->GetX();
	Double_t dy = track.GetTrack()[1] - pHit->GetY();
	Double_t c0 = track.GetCovMatrix()[0];
	Double_t c1 = track.GetCovMatrix()[1];
	Double_t c2 = track.GetCovMatrix()[2];
	chi2 = 0.5*(dx*dx*c0-2*dx*dy*c1+dy*dy*c2)/(c0*c2-c1*c1);
    } else if(pHit->GetDx() < pHit->GetDy()) {
	Double_t dx = track.GetTrack()[0] - pHit->GetX();
	Double_t c0 = track.GetCovMatrix()[0] + TMath::Power(pHit->GetDx(), 2);
	chi2 = dx*dx / c0;
    } else {
        Double_t dy = track.GetTrack()[1] - pHit->GetY();
        Double_t c2 = track.GetCovMatrix()[2] + TMath::Power(pHit->GetDy(), 2);
        chi2 = dy*dy / c2;
    }
    return chi2;
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::CreateHistogramms()
{
    // Create control histogramms

    // Normalized distance to hit
    fh_chi2hit = new TH1F("h_chi2hit",
                          "Normalized distance to hit",
                          500, 0., 50.);
    fh_chi2hit_plane = new TH2F("h_chi2hit_plane",
                                "Normalized distance to hit vs. plane number",
                                500, 0., 50.,
				12, 0., 12.);

    fh_resx_plane_true = new TH2F("h_resx_plane_true",
				  "",
				  200, -10., 10.,
				  12, 0., 12.);
    fh_resy_plane_true = new TH2F("h_resy_plane_true",
				  "",
				  200, -10., 10.,
				  12, 0., 12.);
    fh_resx_plane_fake = new TH2F("h_resx_plane_fake",
				  "",
				  200, -10., 10.,
				  12, 0., 12.);
    fh_resy_plane_fake = new TH2F("h_resy_plane_fake",
				  "",
				  200, -10., 10.,
				  12, 0., 12.);
    fh_resx_mom_true = new TH2F("h_resx_mom_true",
				"",
				200, -10., 10.,
				100, 0., 10.);
    fh_resy_mom_true = new TH2F("h_resy_mom_true",
				"",
				200, -10., 10.,
				100, 0., 10.);

    fh_pullx_plane_true = new TH2F("h_pullx_plane_true",
				  "",
				  200, -10., 10.,
				  12, 0., 12.);
    fh_pully_plane_true = new TH2F("h_pully_plane_true",
				  "",
				  200, -10., 10.,
				  12, 0., 12.);
    fh_pullx_plane_fake = new TH2F("h_pullx_plane_fake",
				  "",
				  200, -10., 10.,
				  12, 0., 12.);
    fh_pully_plane_fake = new TH2F("h_pully_plane_fake",
				  "",
				  200, -10., 10.,
				  12, 0., 12.);
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::DataBranches()
{
    // Initialisation

    // Get pointer to the ROOT manager
    FairRootManager* rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
        cout << "-E- CbmL1TrdTrackFinderSts::DataBranches : "
            << "ROOT manager is not instantiated" << endl;
        return;
    }
    // Get pointer to the TRD points
    fArrayTrdPoint = L1_DYNAMIC_CAST<TClonesArray*>( rootMgr->GetObject("TrdPoint") );
    if(NULL == fArrayTrdPoint) {
        cout << "-W- CbmL1TrdTrackFinderSts::DataBranches : "
            << "no TRD point array" << endl;
    }
    // Get pointer to the STS track array
    fArrayStsTrack = L1_DYNAMIC_CAST<TClonesArray*>( rootMgr->GetObject("StsTrack") );
    if(NULL == fArrayStsTrack) {
        cout << "-W- CbmL1TrdTrackFinderSts::DataBranches : "
            << "no STS track array" << endl;
    }
    // Get pointer to the STS track match array
    fArrayStsTrackM = L1_DYNAMIC_CAST<TClonesArray*>( rootMgr->GetObject("StsTrackMatch") );
    if(NULL == fArrayStsTrackM) {
        cout << "-W- CbmL1TrdTrackFinderSts::DataBranches : "
            << "no STS track match array" << endl;
    }
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::TrdLayout()
{
    // Determine the actual TRD layout from the parameter file

    // Get the pointer to the singleton FairRunAna object
    FairRunAna* ana = FairRunAna::Instance();
    if(NULL == ana) {
        cout << "-E- CbmL1TrdTrackFinderSts::TrdLayout :"
            <<" no FairRunAna object!" << endl;
        return;
    }
    // Get the pointer to run-time data base
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    if(NULL == rtdb) {
        cout << "-E- CbmL1TrdTrackFinderSts::TrdLayout :"
            <<" no runtime database!" << endl;
        return;
    }
    // Get the pointer to container of base parameters
    FairBaseParSet* baseParSet =
        L1_DYNAMIC_CAST<FairBaseParSet*>( rtdb->getContainer("FairBaseParSet") );
    if(NULL == baseParSet) {
        cout << "-E- CbmL1TrdTrackFinderSts::TrdLayout :"
            <<" no container of base parameters!" << endl;
        return;
    }
    // Get the pointer to detector list
    TObjArray *detList = baseParSet->GetDetList();
    if(NULL == detList) {
        cout << "-E- CbmL1TrdTrackFinderSts::TrdLayout :"
            << " no detector list!" << endl;
        return;
    }
    // Find TRD detector
    FairDetector* trd = L1_DYNAMIC_CAST<FairDetector*>( detList->FindObject("TRD") );
    if(NULL == trd) {
        cout << "-E- CbmL1TrdTrackFinderSts::TrdLayout :"
            << " no TRD detector!" << endl;
        return;
    }
    // Determine the geometry version
    TString name = trd->GetGeometryFileName();
    if(name.Contains("9")) {
        cout << "-I- CbmL1TrdTrackFinderSts::TrdLayout :"
            << " TRD geometry : 3x3." << endl;
        fNoTrdStations = 3;
        fNoTrdPerStation = 3;
    } else if(name.Contains("12")) {
        cout << "-I- CbmL1TrdTrackFinderSts::TrdLayout :"
            << " TRD geometry : 3x4." << endl;
        fNoTrdStations = 3;
        fNoTrdPerStation = 4;
    } else if(name.Contains("6x2")) {
        cout << "-I- CbmL1TrdTrackFinderSts::TrdLayout :"
            << " TRD geometry : 6x2." << endl;
        fNoTrdStations = 6;
        fNoTrdPerStation = 2;
    } else if(name.Contains("standard")) {
        cout << "-I- CbmL1TrdTrackFinderSts::TrdLayout :"
            << " TRD geometry : 3x4 standard." << endl;
        fNoTrdStations = 3;
        fNoTrdPerStation = 4;
    }
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::SortTrdHits()
{
    // Sort TRD hits by plane number
    for(Int_t i = 0; i < 12; i++) {
        fNoTrdHits[i] = 0;
    }
    // Declare variables outside the loop
    Int_t nTrdHits = fArrayTrdHit->GetEntriesFast();
    CbmTrdHit* hit = NULL;
    CbmKFTrdHit* kfHit = NULL;
    Int_t planeNumber = 0;
    // Loop over TRD hits
    for(Int_t iHit = 0; iHit < nTrdHits; iHit++) {
        // Get pointer to the hit
        hit = L1_DYNAMIC_CAST<CbmTrdHit*>( fArrayTrdHit->At(iHit) );
        if(NULL == hit) continue;
        // Create KF TRD hit
        new ((*fArrayKFTrdHit)[iHit]) CbmKFTrdHit();
        kfHit = L1_DYNAMIC_CAST<CbmKFTrdHit*>( fArrayKFTrdHit->At(iHit) );
        kfHit->Create(hit);
        // Get plane number
        planeNumber = hit->GetPlaneId() - 1;
        if(planeNumber < 0 || planeNumber > 12) {
            cout << "-W- CbmL1TrdTrackFinderSts::SortTrdHits : "
                << "wrong plane number." << endl;
            continue;
        }
        // Store hit index in the array
        fTrdHitIndex[planeNumber][ fNoTrdHits[planeNumber] ] = iHit;
        // Increment number of hits in plane
        fNoTrdHits[planeNumber] += 1;
    }
    // Control output
    cout << endl << "-I- CbmL1TrdTrackFinderSts::SortTrdHits : "
        << endl;
    for(Int_t i = 0; i < (fNoTrdStations*fNoTrdPerStation); i++) {
        cout << "TRD plane no. " << i << " has " <<
            fNoTrdHits[i] << " hits" << endl;
    }
}
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
void CbmL1TrdTrackFinderSts::WriteHistogramms()
{
    // Write control histogramms to file
    fh_chi2hit->Write();
    fh_chi2hit_plane->Write();
    fh_resx_plane_true->Write();
    fh_resy_plane_true->Write();
    fh_resx_plane_fake->Write();
    fh_resy_plane_fake->Write();
    fh_resx_mom_true->Write();
    fh_resy_mom_true->Write();
    fh_pullx_plane_true->Write();
    fh_pully_plane_true->Write();
    fh_pullx_plane_fake->Write();
    fh_pully_plane_fake->Write();
}
// -----------------------------------------------------------------------


ClassImp(CbmL1TrdTrackFinderSts);





