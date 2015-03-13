#include "CbmStsMapsStripTracker.h"
#include "FairMCApplication.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "FairVolume.h"
#include "FairTask.h"
#include "CbmStsMapsHit.h"
#include "CbmStsMapsHitInfo.h"
#include "CbmStsJointTrack.h"
#include "CbmStsHit.h"
#include "CbmStsStripHit.h"
#include "CbmStsHybridHit.h"
#include "TParticle.h"
#include "CbmArrayI.h"
#include "CbmMCTrack.h"
#include "TRandom.h"
#include "TRef.h"
#include "iomanip.h"
#include "Riostream.h"
#include "TH1F.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TArrayD.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"

Int_t MSStsUid [7];

//_________________________________________________________________
CbmStsMapsStripTracker::CbmStsMapsStripTracker(): ref(0) {
  fHits = new TClonesArray("CbmStsHit");
  fGeantTracks = new TClonesArray("CbmStsJointTrack");

}

//_________________________________________________________________
CbmStsMapsStripTracker::CbmStsMapsStripTracker(const char *name,
					 const char *title) : FairTask(name) {

  ref= new TRefArray();
  
  fHits = new TClonesArray("CbmStsHit");
  fGeantTracks = new TClonesArray("CbmStsJointTrack");

}

//_________________________________________________________________
CbmStsMapsStripTracker::~CbmStsMapsStripTracker() {
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

// -----   Initialization  of Parameter Containers -------------------------


void CbmStsMapsStripTracker::SetParContainers()
{
  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(CbmGeoStsPar*)(rtdb->getContainer("CbmGeoStsPar"));
}

InitStatus CbmStsMapsStripTracker::ReInit()
{
  // Update the pointers after reinitialisation
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(CbmGeoStsPar*)(rtdb->getContainer("CbmGeoStsPar"));
  return kSUCCESS;
}




//_________________________________________________________________
InitStatus CbmStsMapsStripTracker::Init() {
  FairRootManager *fManager =FairRootManager::Instance();	

  fListMCtracks = (TClonesArray *)fManager->GetObject("MCTrack");
  fListSTSpts   = (TClonesArray *)fManager->GetObject("StsPoint");

  fListStripHits  = (TClonesArray *)fManager->GetObject("StsStripHit");
  fListHybridHits  = (TClonesArray *)fManager->GetObject("StsHybridHit");
  fListMAPSpts = (TClonesArray *)fManager->GetObject("StsMapsHit");
  fListHitInfo = (TClonesArray *)fManager->GetObject("StsMapsHitInfo");



/*
  if (gGeoManager ) {
    for (Int_t i = 0; i < 7; i++) {
      sprintf(stsname,"stsstation%i",i+1);
      MSStsUid[i] = gGeoManager->GetUID(stsname);
      cout << stsname << " " << MSStsUid[i] << endl;
    }
  }
  else {
    cout << " Error no Manager Built !!! " << endl;
  }
  */

  TObjArray *aSensNodes = fGeoPar->GetGeoSensitiveNodes();
  FairGeoNode* vol=NULL;

  for (Int_t i =0; i<aSensNodes->GetEntriesFast();i++){
  vol = (FairGeoNode*) aSensNodes->At(i);
  MSStsUid[i] = vol->getMCid();
  cout << i << " " << MSStsUid[i] << endl;
  }

  Register();

  return kSUCCESS;

}

//_________________________________________________________________
void CbmStsMapsStripTracker::GetMapsHits() {
    
  CbmStsMapsHit *point=NULL;
  CbmStsMapsHitInfo *info=NULL;

  CbmStsHit *hit=NULL;

  for (int j=0; j <  fListMAPSpts->GetEntries(); j++ ) {
    point = (CbmStsMapsHit*) fListMAPSpts->At(j);
    info = (CbmStsMapsHitInfo*) fListHitInfo->At(j);

    hit = AddHit();
    hit->SetHitNumber(fNHit++);

    hit->SetX(point->GetX());
    hit->SetY(point->GetY());
    hit->SetZ(point->GetZ());

    hit->SetXerr(point->GetDx());
    hit->SetYerr(point->GetDy());
    hit->SetZerr(point->GetDz());

    Int_t DetId = point->GetDetectorID();
    hit->SetStation(GetStation(DetId)); //

      if (point->GetRefIndex() > 0) {
	hit->SetTrackID(info->GetTrackID());
      }

    if(hit->GetTrackID() > -1) SetTrack(hit);
  }
}

//_________________________________________________________________
Int_t CbmStsMapsStripTracker::GetStation(Int_t det) {
    // This is extremely dangerous !!!!!
    // FIX ME !!!
  return det - 13;

}

//_________________________________________________________________
void CbmStsMapsStripTracker::GetHybridHits() {

  CbmStsHybridHit *point=NULL;

  CbmStsHit *hit=NULL;

  for (int j=0; j <  fListHybridHits->GetEntries(); j++ ) {
    point = (CbmStsHybridHit*) fListHybridHits->At(j);

    hit = AddHit();
    hit->SetHitNumber(fNHit++);

    hit->SetX(point->GetX());
    hit->SetY(point->GetY());
    hit->SetZ(point->GetZ());

    hit->SetXerr(point->GetDx());
    hit->SetYerr(point->GetDy());
    hit->SetZerr(point->GetDz());

    Int_t DetId = point->GetDetectorID();
    hit->SetStation(GetStation(DetId)); //

    if (point->GetRefIndex() > 0) {
      FairMCPoint *mcpoint = (FairMCPoint *) fListSTSpts->At(point->GetRefIndex());
      hit->SetTrackID(mcpoint->GetTrackID());
      SetTrack(hit);
    }
    
  }  // for (int j=0; j <  fListHybridHits->GetEntries(); j++ )

}

//_________________________________________________________________
void CbmStsMapsStripTracker::GetStripHits() {
    
  CbmStsStripHit *point=NULL;

  CbmStsHit *hit=NULL;

  for (int j=0; j <  fListStripHits->GetEntries(); j++ ) {
    point = (CbmStsStripHit*) fListStripHits->At(j);

    hit = AddHit();
    hit->SetHitNumber(fNHit++);

    hit->SetX(point->GetX());
    hit->SetY(point->GetY());
    hit->SetZ(point->GetZ());

    hit->SetXerr(point->GetDx());
    hit->SetYerr(point->GetDy());
    hit->SetZerr(point->GetDz());

    Int_t DetId = point->GetDetectorID();
    hit->SetStation(GetStation(DetId)); //

    if (point->GetRefIndex() > 0) {
      FairMCPoint *mcpoint = (FairMCPoint *) fListSTSpts->At(point->GetRefIndex());
      hit->SetTrackID(mcpoint->GetTrackID());
      SetTrack(hit);
    }
    
  }  // for (int j=0; j <  fListStripHits->GetEntries(); j++ )

}

//_________________________________________________________________
void CbmStsMapsStripTracker::Exec(Option_t * option) {

  Reset();

  GetMapsHits();

  GetHybridHits();

  GetStripHits();

  CheckTracks();

}

//________________________________________________________________
void CbmStsMapsStripTracker::SetTrack(CbmStsHit *hit) {

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
CbmStsJointTrack *CbmStsMapsStripTracker::AddTrack(Int_t mTrackNumber) {
// Add a new track to the list of tracks for this event.

  TClonesArray &tracks = *fGeantTracks;
  CbmStsJointTrack *track = new(tracks[fNTrack++]) CbmStsJointTrack(mTrackNumber);
  return track;
}

//_________________________________________________________________
CbmStsHit * CbmStsMapsStripTracker::AddHit() {
  // Creates a new hit in the TClonesArray.
  TClonesArray& hitRef = *fHits;
  Int_t size = hitRef.GetEntriesFast();
  return new(hitRef[size]) CbmStsHit();
}

//_________________________________________________________________
void CbmStsMapsStripTracker::Register() {
//
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman) Fatal("Register",
		      "No FairRootManager");
  ioman->Register("StsGeantTrack", "GeantTrack", fGeantTracks,kTRUE);
  ioman->Register("StsHit", "CbmStsHit", fHits,kTRUE);

}

//_______________________________________________________________
void CbmStsMapsStripTracker::CheckTracks() {

  Int_t  parent, tr_num, tr_pdg;
  Int_t  nhits_old, ntracks_old;
  Int_t nhit = 0;


  TObjArray dtracks;
  CbmArrayI badtracks;
  CbmStsHit *ghit = 0;

  CbmMCTrack *gtrack = 0;
  TRefArray  *chits;

  Int_t nMCtracks = fListMCtracks->GetEntries();

  if      (fOption.Contains("prim")) {
    cout << " Selected primary tracks" << endl;
  }
  else if (fOption.Contains("scd")) {
    cout << " Selected  secondary tracks" << endl;
  }
  else {
    cout << " Selected  all tracks" << endl;
  }

  Int_t realTracks = 0;

  for (Int_t itrack = 0; itrack < fNTrack; itrack++) {

    CbmStsJointTrack *track = (CbmStsJointTrack*)fGeantTracks->UncheckedAt(itrack);
    tr_num = track->GetTrackNumber();

    if(tr_num > 0 && tr_num < nMCtracks) {
      tr_pdg = 0;
      parent = 99999;

      gtrack = (CbmMCTrack *) fListMCtracks->At(tr_num);
      realTracks++;

      tr_pdg = gtrack->GetPdgCode();
      track->SetPid(tr_pdg);

      parent = gtrack->GetMotherID();
      if(parent == -1) track->ComesFromMainVertex(kTRUE);

      TVector3 vertex = gtrack->GetStartVertex();
      track->SetVertex(vertex.X(), vertex.Y(), vertex.Z());

      TVector3 moment = gtrack->GetMomentum();
      track->SetPx(moment.X());
      track->SetPy(moment.Y());
      track->SetPz(moment.Z());

      nhit = track->GetNumberOfHits();
      chits = track->GetRHits();

      Bool_t badtrack = kFALSE;

      if (fOption.Contains("prim") && !track->ComesFromMainVertex()) {
	badtrack = kTRUE;
      }

      if (fOption.Contains("scd") && track->ComesFromMainVertex()) {
	badtrack = kTRUE;
      }

      if (badtrack) {
	dtracks.AddLast(track);
	badtracks.AddLast(tr_num);
	    
      }

      if (parent == 99999 ) cout << "  track parent unknown";

    }
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
	//	  fHits->Remove(ghit);
      }
    }
  }

  fNTrack = fGeantTracks->GetEntries();
  fNHit = fHits->GetEntries();

  cout << " tracks in STS " << fNTrack << endl;
  cout << " real tracks in STS " << realTracks << endl;
  cout << " Hits in STS " << fNHit << endl;

}

//_________________________________________________________________
void CbmStsMapsStripTracker::Finish()
{

}

//_________________________________________________________________
void CbmStsMapsStripTracker::FinishTasks()
{

  cout << " CbmStsMapsStripTracker::FinishTasks : Tracks " <<
        fGeantTracks->GetEntriesFast() << "nHits:" <<
        fHits->GetEntries()<< endl;

  fGeantTracks->Clear();


}

//_________________________________________________________________
void CbmStsMapsStripTracker::Reset()
{
 //   cout << " CbmStsMapsStripTracker::Reset  " << endl;

  fNHit = 0;
  fNTrack = 0;	

  fHits->Clear();
  fGeantTracks->Clear();

}

//_________________________________________________________________
void CbmStsMapsStripTracker::PrintTracks(Int_t ntr) {

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

ClassImp(CbmStsMapsStripTracker)
