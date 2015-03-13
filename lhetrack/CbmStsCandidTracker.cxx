#include "CbmStsCandidTracker.h"
#include "FairMCApplication.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "FairVolume.h"
#include "FairTask.h"
#include "CbmStsPoint.h"
#include "CbmMCTrack.h"
#include "CbmStsJointTrack.h"
#include "CbmStsHit.h"
#include "TParticle.h"
#include "TRandom.h"
#include "CbmArrayI.h"
#include "CbmMCTrack.h"
#include "TRef.h"

#include "iomanip.h"
#include "Riostream.h"
#include "TH1F.h"

Int_t StsUid [7];

//_________________________________________________________________
CbmStsCandidTracker::CbmStsCandidTracker() {
  fHits = new TClonesArray("CbmStsHit");
  fGeantTracks = new TClonesArray("CbmStsJointTrack");

}

//_________________________________________________________________
CbmStsCandidTracker::CbmStsCandidTracker(const char *name,
					 const char *title) : FairTask(name) {

  fHits = new TClonesArray("CbmStsHit");
  fGeantTracks = new TClonesArray("CbmStsJointTrack");

}

//_________________________________________________________________
CbmStsCandidTracker::~CbmStsCandidTracker() {
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//_________________________________________________________________
InitStatus CbmStsCandidTracker::Init()
{

  FairRootManager *fManager =FairRootManager::Instance();	

  fListSTSpts = (TClonesArray *)fManager->GetObject("StsPoint");
  fListMCtracks = (TClonesArray *)fManager->GetObject("MCTrack");

  fDx = 0.002; //!  20 mkm
  fDy = 0.002; //!  20 mkm

  if (fOption.Contains("smear"))   fSmearingXY = kTRUE;
  else  fSmearingXY = kFALSE;

  Char_t stsname[10];

  for (Int_t i = 0; i < 7; i++) {
    sprintf(stsname,"stsstation%i",i+1);
    if (gGeoManager ) {
      StsUid[i] = gGeoManager->GetUID(stsname);
    }
    else cout << " Error no Manager Built !!! " << endl;

  }

  Register();

  return kSUCCESS;

}

//_________________________________________________________________
void CbmStsCandidTracker::Exec(Option_t * option)
{
    
  Reset();

  CbmStsPoint *point=NULL;
  CbmStsHit *hit=NULL;

  Double_t dx=0.0;
  Double_t dy=0.0;

  for (int j=0; j <  fListSTSpts->GetEntries(); j++ ) {
    point = (CbmStsPoint*) fListSTSpts->At(j);

    hit = AddHit();
    hit->SetHitNumber(fNHit++);

    hit->SetZ(point->GetZOut());

    if (fSmearingXY) {

      dx = gRandom->Gaus(point->GetXOut(), fDx);
      dy = gRandom->Gaus(point->GetYOut(), fDy);

      hit->SetX(dx);
      hit->SetY(dy);
      
      hit->SetXerr(point->GetXOut() - dx);
      hit->SetYerr(point->GetYOut() - dy);
    }
    else {

      hit->SetX(point->GetXOut());
      hit->SetY(point->GetYOut());
      hit->SetXerr(.0);
      hit->SetYerr(.0);

    }

    Int_t DetId = point->GetDetectorID();

    for (Int_t i = 0; i < 7; i++) {
      if(DetId == StsUid[i]) {
	hit->SetStation(i+1); //
	break;
      }
    }

    hit->SetTrackID(point->GetTrackID());
    SetTrack(hit);

  }

  CheckTracks();
  
}

//________________________________________________________________
void CbmStsCandidTracker::SetTrack(CbmStsHit *hit) {

  Int_t mTrackNumber = hit->GetTrackID();
  Int_t mTN;
  Bool_t newtrack;

  CbmStsJointTrack *track = NULL;

  newtrack = kTRUE;

  
  if (fNTrack > 0) {
    for (Int_t i = 0; i < fGeantTracks->GetEntriesFast(); i++) {
      CbmStsJointTrack *currtrk = ((CbmStsJointTrack*) fGeantTracks->At(i));
      mTN = currtrk->GetTrackNumber();
      if (mTN == mTrackNumber) {
	track = currtrk;
	newtrack = kFALSE;
	break;
      }
    }
  }
  if (newtrack)  track = AddTrack(mTrackNumber);

  track->AddHit(hit);
  
}

//________________________________________________________________
CbmStsJointTrack *CbmStsCandidTracker::AddTrack(Int_t mTrackNumber) {
// Add a new track to the list of tracks for this event.

  TClonesArray &tracks = *fGeantTracks;
  CbmStsJointTrack *track = new(tracks[fNTrack++]) CbmStsJointTrack(mTrackNumber);
  return track;
}

//_________________________________________________________________
CbmStsHit * CbmStsCandidTracker::AddHit()
{
  // Creates a new hit in the TClonesArray.
  TClonesArray& hitRef = *fHits;
  Int_t size = hitRef.GetEntriesFast();
  return new(hitRef[size]) CbmStsHit();
}

//_________________________________________________________________
void CbmStsCandidTracker::Register() {
//
  FairRootManager::Instance()->Register("StsGeantTrack","StsGeantTrack",fGeantTracks,kFALSE);
  FairRootManager::Instance()->Register("StsHit","StsHit", fHits, kFALSE);

}

//_______________________________________________________________
void CbmStsCandidTracker::CheckTracks() {

  Int_t  parent, tr_num, tr_pdg, station;
  Int_t  nhits_old, ntracks_old;
  Int_t nhit = 0;

  TObjArray dtracks;
  CbmArrayI badtracks;
  CbmStsHit *ghit = 0;
  CbmMCTrack *gtrack = 0;
  TRefArray  *chits;


  if      (fOption.Contains("prim")) {
    cout << " Selected primary tracks" << endl;
  }
  else if (fOption.Contains("scd")) {
    cout << " Selected  secondary tracks" << endl;
  }
  else {
    cout << " Selected  all "  << fGeantTracks->GetEntries() << " geant tracks" << endl;
  }

  for (Int_t itrack = 0; itrack < fNTrack; itrack++) {

    CbmStsJointTrack *track = (CbmStsJointTrack*)fGeantTracks->UncheckedAt(itrack);
    tr_num = track->GetTrackNumber();
    tr_pdg = 0;
    parent = 99999;

    gtrack = (CbmMCTrack *) fListMCtracks->At(tr_num);

    tr_pdg = gtrack->GetPdgCode();
    track->SetPid(tr_pdg);

    parent = gtrack->GetMotherId();
    if(parent == -1) track->ComesFromMainVertex(kTRUE);

    TVector3 vertex;
    gtrack->GetStartVertex(vertex);
    track->SetVertex(vertex.X(), vertex.Y(), vertex.Z());

    TVector3 moment;
    gtrack->GetMomentum(moment);
    track->SetPx(moment.X());
    track->SetPy(moment.Y());
    track->SetPz(moment.Z());

    nhit = track->GetNumberOfHits();
    chits = track->GetRHits();

    Bool_t SameStation = kFALSE;
    Int_t station1 = 99;

    for (Int_t j=0; j<nhit; j++) {
	  
      ghit = (CbmStsHit *) chits->At(j);
      station = ghit->GetStation();
      if(station == station1) {
	SameStation = kTRUE;
	break;
      }
      station1 = station;

    }

    Bool_t badtrack = kFALSE;

    if(nhit > 7 || SameStation ) {
      badtrack = kTRUE;
    }
    else {
      if      (fOption.Contains("prim")) {
	if(!track->ComesFromMainVertex()) {
	  badtrack = kTRUE;
	}
      }
      else if (fOption.Contains("scd")) {
	if(track->ComesFromMainVertex()) {
	  badtrack = kTRUE;
	}
      }
    }

    if (badtrack) {
      dtracks.AddLast(track);
      badtracks.AddLast(tr_num);
	    
    }

    if (parent == 99999 ) cout << "  track parent unknown";

  }

    nhits_old = fNHit;
    ntracks_old = fNTrack;

    for (Int_t btrack = 0; btrack < badtracks.GetSize(); btrack++) {
      fGeantTracks->Remove(dtracks.At(btrack));
    }

     fGeantTracks->Compress();
     Int_t nRealHits = fHits->GetEntries();

     for (Int_t j=0; j< nRealHits; j++) {
       ghit = (CbmStsHit *) fHits->UncheckedAt(j);
       ghit->SetUsage(kFALSE);
       Int_t fid = ghit->GetTrackID();
       for (Int_t btrack = 0; btrack < badtracks.GetSize(); btrack++) {
	 if(fid == badtracks[btrack]) {
	   ghit->SetUsage(kTRUE);

	 }
       }
     }

     fNTrack = fGeantTracks->GetEntries();
     fNHit = fHits->GetEntries();

}

//_________________________________________________________________
void CbmStsCandidTracker::Finish() {

  fGeantTracks->Clear();
  FairRootManager *fManager =FairRootManager::Instance();	
  fManager->Fill();

}

//_________________________________________________________________
void CbmStsCandidTracker::Reset() {

  fNHit = 0;
  fNTrack = 0;	

  fHits->Clear();
  fGeantTracks->Clear();

}

//_________________________________________________________________
void CbmStsCandidTracker::PrintTracks(Int_t ntr) {

  Int_t  ptracks;

  if(ntr != 0 && ntr < fNTrack) 
    ptracks = ntr;
  else
    ptracks = fNTrack;

  for (Int_t itrack = 0; itrack < ptracks; itrack++) {
	
    CbmStsJointTrack *track = (CbmStsJointTrack *)fGeantTracks->UncheckedAt(itrack);
    track->Print(fHits);
  }

}

ClassImp(CbmStsCandidTracker)
