///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// CbmStsCMTrackFinder class - class                                             //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////

#include "CbmStsCMTrackFinder.h"
#include "CbmStsHit.h"
#include "CbmStsCMTrack.h"
#include "CbmStsCMPoint.h"
#include "FairMCApplication.h"
#include "FairTask.h"

#include "TRefArray.h"
#include "TClonesArray.h"

ClassImp(CbmStsCMTrackFinder)

//________________________________________________________________
CbmStsCMTrackFinder::CbmStsCMTrackFinder() {
// Default constructor.

  fFoundTracks = new TClonesArray("CbmStsJointTrack");

  fVertex = NULL;
  fCMTracks  = NULL;

}

//_________________________________________________________________
CbmStsCMTrackFinder::~CbmStsCMTrackFinder() {

  if (fCMTracks) {
    fCMTracks->Delete();
    delete fCMTracks;
  }

  if (fTrackletCuts) {
    delete fTrackletCuts;
  }

  if (fTrackCuts) {
    delete fTrackCuts;
  }

  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();

}

//_________________________________________________________________
CbmStsCMTrackFinder::CbmStsCMTrackFinder(const char *name, const char *title)
	:FairTask(name) {

  fTrackletCuts = new CbmStsTrackletCuts();
  fTrackCuts = new CbmStsTrackCuts();

  //  fHits = new TClonesArray("CbmStsHit");
  fFoundTracks = new TClonesArray("CbmStsJointTrack");

  fVertex = NULL;
  fCMTracks  = NULL;

}

//_________________________________________________________________
void CbmStsCMTrackFinder::Register() {
    //

    FairRootManager::Instance()->Register("StsFoundTrack","StsFoundTrack",fFoundTracks, kFALSE);

}

//________________________________________________________________
InitStatus CbmStsCMTrackFinder::Init() {

  FairRootManager *fManager =FairRootManager::Instance();
  //fHits = (TClonesArray *)fManager->CheckActivatedBranch("StsHit");
  fHits  = (TClonesArray *)fManager->GetObject("StsHit");
  if (!fHits ) exit(1);
  Register();

  fCMTracks = new TObjArray(2000);

  fNumOfStation = 8;                   // The number of stations
  fMaxSTStation = fNumOfStation;
  fMergedSplits = 0;
  fExtendedTracks = 0;

  fSegment = new CbmStsSegments();

  CbmStsVertex *vt = new CbmStsVertex(0.0, 0.0, 0.0);
  fVertex = vt;

  fSegment->CalcAlphaBetaLimits(vt);

  return kSUCCESS;

}

//_________________________________________________________________
void CbmStsCMTrackFinder::Exec(Option_t * option) {

  Reset();
  Int_t n_hits = fHits->GetEntries();    // number of hits
  fHitsUnused = n_hits;



  fCMHits = new TObjArray(n_hits);       // create TObjArray
  CbmStsHit *ghit = NULL;

  Int_t ch = 0;

  for (Int_t ih = 0; ih < n_hits; ih++) {
    ghit = (CbmStsHit *) fHits->UncheckedAt(ih);
    if(!ghit->GetUsage()) {
      fCMHits->AddAt(new CbmStsCMPoint(ghit), ch);
      ((CbmStsCMPoint *)fCMHits->At(ch))->SetHitNumber(ghit->GetHitNumber());
      ch++;
    }
  }

//  cout << " hits in " << n_hits << " after cleaning " << fCMHits->GetEntriesFast();

  fSegment->FillSegments(fCMHits);

  MainVertexTracking();

  OutputTracks();

}

//________________________________________________________________
void CbmStsCMTrackFinder::AddTrackForFit(CbmStsJointTrack *track_in) {
// Add a new track to the list of tracks for this event.

  TRefArray *rhits = (TRefArray *)track_in->GetRHits();

  Int_t NHits = rhits->GetEntries();

  Int_t tracknumber = track_in->GetTrackNumber();

  TClonesArray &tracks = *fFoundTracks;
  Int_t size = tracks.GetEntriesFast();
  CbmStsJointTrack *track = new(tracks[size]) CbmStsJointTrack(tracknumber);
  track->ComesFromMainVertex(track_in->ComesFromMainVertex());
  
  CbmStsHit *hit = NULL;

  for (Int_t lh = NHits-1; lh >= 0; lh--) {
    hit = (CbmStsHit *) rhits->At(lh);
    track->AddHit(hit);
  }
  
}

//_________________________________________________________________
void CbmStsCMTrackFinder::Finish()
{

  if (fCMHits) {
    fCMHits->Clear();
  }

  if (fCMTracks) {
    fCMTracks->Clear();
  }

  cout << " # of found tracks " << fFoundTracks->GetEntries() << endl;
  FairRootManager *fManager =FairRootManager::Instance();	
  fManager->Fill();

}

//_________________________________________________________________
void CbmStsCMTrackFinder::OutputTracks() {

  CbmStsJointTrack *track = NULL;

  for (Int_t t = 0; t < fCMTracks->GetEntriesFast(); t++) {
    track = (CbmStsJointTrack *) fCMTracks->At(t);
    AddTrackForFit(track);
  }
}

//________________________________________________________________
void CbmStsCMTrackFinder::Reset() {
//

 fMergedSplits = 0;
 fMainVertexTracks = 0;

 if(fFoundTracks) {
   fFoundTracks->Clear();
 }

//  if (fCMTracks) {
//    fCMTracks->Clear();
//  }

}

//________________________________________________________________
void CbmStsCMTrackFinder::MainVertexTracking() {

  // Tracking with vertex constraint.

  SetVertexConstraint(kTRUE);

  PrimaryHighMomTracking();

  PrimaryIntermedMomTracking();

  PrimaryLowMomTracking();
}

//________________________________________________________________
void CbmStsCMTrackFinder::PrimaryHighMomTracking() {

  // settings for PrimaryHighMom

  fTrackletCuts->SetPrimaryTrackletCuts();
  fTrackCuts->SetPrimaryHighMomTrackCuts();
  
  LoopOverHits();

  fTrackCuts->SetPrimaryExtTrackCuts();
  ExtendTracks();


  CheckClonedTracks();
  
}

//________________________________________________________________
void CbmStsCMTrackFinder::PrimaryIntermedMomTracking() {

  // settings for PrimaryIntermedMom

  fTrackletCuts->SetPrimaryIntermedMomTrackletCuts();
  fTrackCuts->SetPrimaryIntermedMomTrackCuts();

  LoopOverHits();

  ExtendTracks();
  
  CheckClonedTracks();

}

//________________________________________________________________
void CbmStsCMTrackFinder::PrimaryLowMomTracking() {

  // Tracking low momenta 

  fTrackletCuts->SetPrimaryLowTrackletCuts();
  fTrackCuts->SetPrimaryLowMomTrackCuts();
  
  LoopOverHits();

  ExtendTracks();
  
  CheckClonedTracks();

}


//________________________________________________________________
void CbmStsCMTrackFinder::ExtendTracks() {
  // Loops over all found tracks and passes them to the part of the
  // program where each track is tried to be extended.
  
  for (Int_t t = 0; t < fCMTracks->GetEntriesFast(); t++) {
    
    CbmStsCMTrack *track = (CbmStsCMTrack *)fCMTracks->At(t);
    if(track->GetNumberOfHits() < fMaxSTStation) {
  
      if (TrackExtension(track)) {
	fExtendedTracks++;

      }
    }
  }
}

//________________________________________________________________
Bool_t CbmStsCMTrackFinder::TrackExtension(CbmStsCMTrack *track) {
  // Try to extend a given track.

  Int_t point;
  Int_t number_of_points = track->GetNumberOfPoints();
  Double_t *coeff = NULL;

  CbmStsCMPoint *hit;
  CbmStsCMPoint *closest_hit;
  TObjArray *trackpoint = track->GetCMHits();
  
  
  for (Int_t direction = 0; direction < 2; direction ++) {
    
    for (point = number_of_points; point < fMaxSTStation; point++) {
   
      if (direction == 1) {  // backward
	hit = (CbmStsCMPoint *)trackpoint->Last();
      }
      else {                 // forward
	hit = (CbmStsCMPoint *)trackpoint->First();
      }
      
      Int_t cur_station = hit->GetStation();

      if ((cur_station <= 1 && direction == 1) ||
	  (cur_station >= 7 && direction == 0)) break;

      if ((cur_station > 1 && direction == 1) ||
	  (cur_station < 7 && direction == 0)) {

	if (!coeff) coeff = new Double_t[6];

	fTrackCuts->TrackFit(track, coeff);
	GetNextNeighbor(hit, track, (Bool_t)direction);

	if ((closest_hit = fTrackCuts->GetClosestHit(track))) {

	  // add closest hit to track
	  if(direction == 1) {                        // backward
	    track->AddPoint(closest_hit);
	  }
	  else {                                      // forward
	    track->AddForwardPoint(closest_hit);
	  }
	}

	else { 
	  break;
	}
      }
    }
  }    

  // cleanup

  if (coeff) {
    delete[] coeff; 
    coeff = NULL;
  }

  if (track->GetNumberOfPoints() - number_of_points) {
    
    track->SetTrackMask();
    track->ComesFromMainVertex(fVertexConstraint);
    //    track->CalculateNMax();


    fHitsUnused -= (track->GetNumberOfPoints() - number_of_points);

    return kTRUE;
  }
  else {
    return kFALSE;
  } 

}


//________________________________________________________________
void CbmStsCMTrackFinder::TrackingInfo(char *info) {
  // Information about the tracking process.
  
 cout << "\n\n --- TrackFinder information ---\n";
 cout << info << endl;
  
 cout.width(5);  cout << GetNumberOfTracks() << " (";
 cout.width(5);  cout << GetNumPrimaryTracks() << "/";
 cout.width(5);  cout << GetNumberOfTracks() - GetNumPrimaryTracks() <<
		   ") tracks (primary/secondary) are found." << endl;
 cout.width(5);
 cout << GetNumberOfHits() << " (";
 cout << GetNumberOfHits() - GetNumHitsUnused() << "/";
 cout << GetNumHitsUnused() << ") hits (used/unused)." << endl;
 cout.width(5);
 cout << GetNumExtendedTracks() << " tracks are extended." << endl;
 cout.width(5);
 cout << GetNumMergedSplits() << " splited tracks are merged.";
 cout << "\n --- --- --- --- --- --- --- ---\n " << endl;

}

//________________________________________________________________
void CbmStsCMTrackFinder::LoopOverHits() {

// This function loops over all hits to do the tracking.

  TObjArray *segment;
  TObjArray *cell = fSegment->GetSegments();

  CbmStsCMPoint *hit;

  // loop over the  stations

  for (Int_t st_num = fNumOfStation-1; st_num > 1; st_num--) {

    // loop over beta segments
    for (Int_t beta_segm_counter = 0;
	 beta_segm_counter < fSegment->GetNumBetaSegments();
	 beta_segm_counter++) {

      // go over beta in two directions, one segment in each direction alternately
      Int_t beta_segm;

        beta_segm = beta_segm_counter;

      // loop over alpha segments
      for(Int_t alpha_segm = 0; alpha_segm < fSegment->GetNumAlphaSegments();
	  alpha_segm++) {
	
	// loop over entries in one segment
        segment = (TObjArray *)cell->At(fSegment->GetSegm(st_num, beta_segm, alpha_segm));
        Int_t entries = segment->GetEntriesFast();
        if (entries) {

          for (Int_t hit_num = 0; hit_num < entries; hit_num++) {
            hit = (CbmStsCMPoint *)segment->At(hit_num);

	    // start hit was used before
            if (!hit->GetUsage()) {
              CreateTrack(hit);
            }
          }
        }    // if (entries)

      }
    }
  }

}

//________________________________________________________________
void CbmStsCMTrackFinder::CreateTrack(CbmStsCMPoint *hit) {
  // This function takes as input a point in the STS which acts as
  // starting point for a new track.  It forms tracklets then extends
  // them to tracks.

  CbmStsCMPoint *closest_hit = NULL;
  CbmStsCMTrack *track = NULL;

  Int_t tracks = GetNumberOfTracks();
  if (tracks >= fCMTracks->GetEntriesFast())
    fCMTracks->Expand(fCMTracks->GetEntriesFast()+1000);

  fCMTracks->AddAt(new CbmStsCMTrack(tracks), tracks);
  track = (CbmStsCMTrack *)fCMTracks->At(tracks);
  TObjArray *trackpoint = track->GetCMHits();

  track->AddPoint(hit);

  // set conformal mapping coordinates if looking for non vertex tracks
  if (fVertexConstraint) {
    track->ComesFromMainVertex(kTRUE);
  }
  else {
    hit->SetAllCoord(hit);
  }	

  // create tracklets

  Int_t npoints = fTrackletCuts->GetTrackletMinPoints();

  for (Int_t point = 0; point < npoints-1; point++) {

    // going to the vertex --- only backward

    GetNextNeighbor(hit, track, kTRUE);
    if ((closest_hit = fTrackletCuts->GetClosestHit())) {

      track->AddPoint(closest_hit);     // adding second hit to track
      hit = closest_hit;

    }

    else {
      RemoveTrack(track);

      // continue with next hit in segment
      point = fTrackletCuts->GetTrackletMinPoints();
    }
  }

  if (track && (trackpoint->GetEntriesFast()) >= 2) {

    // continue create track

    tracks++;

    for (Int_t point = fSegment->GetStationSegm(hit); point > 0; point--) {
      GetNextNeighbor((CbmStsCMPoint *)trackpoint->Last(),
		      track, kTRUE);
      closest_hit = fTrackCuts->GetClosestHit(track);

      if ( closest_hit) {
	track->AddPoint(closest_hit);   // add closest hit to track

      }
      else {                       // closest hit does not exist
	  
	break;
      }
    }

    // remove tracks with not enough points already now
    if (track->GetNumberOfPoints() < fTrackCuts->GetMinPoints()) {
      RemoveTrack(track);
      tracks--;
    }
    else {
      CompleteTrack(track);

      // print information of track

    }
  }

}

//________________________________________________________________
void CbmStsCMTrackFinder::GetNextNeighbor(CbmStsCMPoint *start_hit,
					  CbmStsCMTrack *track,
					  Bool_t backward) {

  // Returns the nearest hit to a given start_hit.

  CbmStsCMPoint *hit = NULL;
  TObjArray *cell = fSegment->GetSegments();

  fTrackletCuts->Reset();
  fTrackCuts->Reset();

  TObjArray *sub_segment;
  Int_t sub_entries;

  Int_t sub_row_segm;
  Int_t sub_beta_segm;
  Int_t sub_alpha_segm;
  Int_t sub_hit_num;

  Int_t start_row;
  Int_t end_row;
  Bool_t track_flag = (track->GetNumberOfPoints() < fTrackletCuts->GetTrackletMinPoints())?
    track_flag = kFALSE : track_flag = kTRUE;

  if (backward) {
    start_row = fSegment->GetStationSegm(start_hit) - 1;

    //    if (coeff) {
    if(track_flag) {
      
      end_row = fSegment->GetStationSegm(start_hit) -
	fTrackCuts->GetStationScopeTrack();
    }
    else {
      end_row = fSegment->GetStationSegm(start_hit) -
	fTrackletCuts->GetStationScopeTracklet();
    }

    if (end_row < 0) end_row = 0;

    //    if (start_row < end_row) return 0;
    if (start_row < end_row) return;

  }

  else {                                // forward
    start_row = fSegment->GetStationSegm(start_hit) + 1;

    //    if (coeff) {
    if (track_flag) {
      end_row = fSegment->GetStationSegm(start_hit) +
	fTrackCuts->GetStationScopeTrack();
    }

    else {
      end_row = fSegment->GetStationSegm(start_hit) +
	fTrackletCuts->GetStationScopeTracklet();
    }

    while (end_row > fMaxSTStation - 1) {
      end_row--;
    }

    //    if (start_row > end_row) return 0;
    if (start_row > end_row) return;
  }

// loop over sub rows

  for (sub_row_segm = start_row;
       TestExpression(sub_row_segm, end_row, backward);
       LoopUpdate(&sub_row_segm, backward)) {

    //  loop over sub beta segments
    for (Int_t i = -(fTrackCuts->GetBetaScope());
	 i <= fTrackCuts->GetBetaScope(); i++) {
      // neighboring beta segment
      sub_beta_segm = fSegment->GetBetaSegm(start_hit) + i;
      
      if (sub_beta_segm < 0 || sub_beta_segm >= fSegment->GetNumBetaSegments()) {
          continue;                     // #segment exceeds bounds -> skip
      }

      // loop over sub alpha segments
      for (Int_t j = -(fTrackCuts->GetAlphaScope());
	   j <= fTrackCuts->GetAlphaScope(); j++) {
	// neighboring eta segment
        sub_alpha_segm = fSegment->GetAlphaSegm(start_hit) + j;
	
        if (sub_alpha_segm < 0 || sub_alpha_segm >= fSegment->GetNumAlphaSegments()) {
          continue;                     // #segment exceeds bounds -> skip
        }

	// loop over entries in one sub segment
        sub_segment = (TObjArray *)cell->At(
		     fSegment->GetSegm(sub_row_segm,
				       sub_beta_segm,
				       sub_alpha_segm));

        sub_entries = sub_segment->GetEntriesFast();
        if (sub_entries) {
	  
          for (sub_hit_num = 0; sub_hit_num < sub_entries; sub_hit_num++) {
            hit = (CbmStsCMPoint *)sub_segment->At(sub_hit_num);
	    
            if (!hit->GetUsage()) {     // hit was not used before

	      // set conformal mapping coordinates if looking for non vertex tracks
//               if (!fVertexConstraint) {
//                 hit->SetAllCoord(start_hit);
//               }
	      
              if (track_flag) {
		// track search - look for nearest neighbor to  extrapolated track
		
		fTrackCuts->VerifyTrack(track, hit);
              }
              else {
		// first tracklet search - just look for the nearest neighbor

		fTrackletCuts->VerifyTracklet(start_hit, hit, backward);
	      }    //  tracklet or track search
	    }      //  if (!hit->GetUsage())
	  }        //  for (sub_hit_num = 0; sub_hit_num < sub_entries; sub_hit_num++)
        }          //  if (sub_entries)
      }            //  for (alpha)
    }              //  for (beta)
  }                //  for (stations)

}


//________________________________________________________________
void CbmStsCMTrackFinder::LoopUpdate(Int_t *sub_row_segm, Bool_t backward) {
// Increments or decrements *sub_row_segm, depending on backward.

  if (backward) {
    (*sub_row_segm)--;
  }

  else {                                // forward
    (*sub_row_segm)++;
  }
}

//________________________________________________________________
Bool_t const CbmStsCMTrackFinder::TestExpression(Int_t sub_row_segm,
						 Int_t end_row,
						 Bool_t backward) {
// Tests if loop should be continued or not.

  if (backward) {

    if (sub_row_segm >= end_row) {
      return kTRUE;
    }

    else {
      return kFALSE;
    }
  }

  else {                                // forward

    if (sub_row_segm <= end_row) {
      return kTRUE;
    }

    else {
      return kFALSE;
    }
  }

  return kFALSE;
}

//________________________________________________________________
void CbmStsCMTrackFinder::CompleteTrack(CbmStsCMTrack *track) {

// Do everything which is still left to be done for the given track.
// Updates the statistics.

  track->SetTrackMask();
  track->ComesFromMainVertex(fVertexConstraint);

  fHitsUnused -= track->GetNumberOfPoints();
  if (fVertexConstraint) fMainVertexTracks++;

}

//______________________________________________________________________
void CbmStsCMTrackFinder::CheckClonedTracks() {

  // Looks for split tracks and passes them to the track merger.

  Int_t mhits;

  if (fVertexConstraint)
    mhits = 5;
  else 
    mhits = 6;

  CbmStsCMTrack *t1;
  CbmStsCMTrack *t2;

  Int_t entries = fCMTracks->GetEntriesFast();
      
  for (Int_t i = 0; i < entries; i++) {
    t1 = (CbmStsCMTrack *)fCMTracks->At(i);

    if (t1->GetNumberOfPoints() < mhits && t1->GetTrackNumber() != -1 ) {
      // track was not merged already
      for (Int_t j = i+1; j < entries; j++) {
	t2 = (CbmStsCMTrack *)fCMTracks->At(j);
     
	if (t2->GetNumberOfPoints() < mhits && t2->GetTrackNumber() != -1 ) {
	  // track was removed before (already merged)

	  if ( TestTrackMasks(t1->GetMask(), t2->GetMask())) {

	    // align tracks
	    if (!fVertexConstraint) AlignTracks( t1, t2);
	    
	    if(fTrackCuts->DistBetweenTracks(t1, t2)) {
	      MergeSplitTracks(t1, t2);
	      break;
	    }   // if(DistBetweenTracks
	  }           // if (!(t1->GetMask() & t2->GetMask()))
	}
      }
    }
  }
  
  for (Int_t btrack = 0; btrack < entries; btrack++) {
    t1 = (CbmStsCMTrack *)fCMTracks->At(btrack);
    if (t1->GetTrackNumber() == -1 ) fCMTracks->Remove(t1);
    
  }
  
  fCMTracks->Compress();

}


//________________________________________________________________
Bool_t CbmStsCMTrackFinder:: TestTrackMasks(Mask8_t mask1, Mask8_t mask2) {

  if (mask1 & mask2)
    return kFALSE;
  else {
    // find min and max
    Int_t min = 0; Int_t max = 0;
    Mask8_t both = mask1^mask2;
    //    unsigned char a, b;
    Mask8_t a;

    for (Int_t i = 0; i <  7; i++) {
      a = 1 << i;
      if (a & both) {
	min = i;
	break;
      }
    }

    for (Int_t j = 6; j >=  0; j--) {
      a = 1 << j;
      if (a & both) {
	max = j;
	break;
      }
    }

    //    cout << " maxmin " << max << " " << min << "----  ";
    a = 0;

    for (Int_t f = min; f < max; f++) a += 1 << f;

//     printBinary(a); cout << " "; printBinary(~both);
//      cout << " "; printBinary(a&~both);

    if (a & ~both) {
      //      cout << " there is a gap between tracks" << endl;
      return kFALSE;
    }
    else {
      //      cout << " tracks could be  merged " << endl;
      return kTRUE;
    }

  }
}

//________________________________________________________________
void CbmStsCMTrackFinder::AlignTracks(CbmStsCMTrack *t1, CbmStsCMTrack *t2) {

  TObjArray *trackpoints = t1->GetCMHits();

  Int_t n1 = trackpoints->GetEntriesFast();

  CbmStsCMPoint *trackpoint = NULL;

  CbmStsCMPoint * stick_hit = (CbmStsCMPoint *)trackpoints->First();
  // calculate for tracks

  for (Int_t i = 0; i < n1; i++) {
    trackpoint = (CbmStsCMPoint *)trackpoints->At(i);
    trackpoint->SetAllCoord( stick_hit);
    
  }

  trackpoints = t2->GetCMHits();
  Int_t n2 = trackpoints->GetEntriesFast();

  for (Int_t j = 0; j < n2; j++) {
    trackpoint = (CbmStsCMPoint *)trackpoints->At(j);
    trackpoint->SetAllCoord(stick_hit);
    
  }

}

//________________________________________________________________
void CbmStsCMTrackFinder::MergeSplitTracks(CbmStsCMTrack *t1,
					   CbmStsCMTrack *t2) {
  // Merges two tracks which are split

  CbmStsCMTrack *fst = NULL;
  CbmStsCMTrack *scd = NULL;

//   TObjArray *trackpoint = track->GetCMHits();
//   CbmArrayI *trackhitnumber = track->GetRHits();

  if( t1->GetMask() < t2->GetMask()) {
    fst = t2;
    scd = t1; 
  }
  else {
    fst = t1;
    scd = t2; 
  }

  for (Int_t ir = 0; ir < scd->GetNumberOfPoints(); ir++) {
    fst->AddPoint((CbmStsCMPoint*)(scd->GetCMHits())->At(ir));
  }

  fst->SetTrackMask();

   if (scd->ComesFromMainVertex()) fMainVertexTracks--;

    scd->SetTrackNumber(-1);

  fMergedSplits++;

}

