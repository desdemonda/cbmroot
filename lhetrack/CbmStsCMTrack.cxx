#include "TObjArray.h"
#include "TClonesArray.h"
#include "CbmStsCMTrack.h"
#include "CbmStsVertex.h"
#include "CbmStsCMPoint.h"

ClassImp(CbmStsCMTrack)

//___________________________________________________________
  CbmStsCMTrack::CbmStsCMTrack() {
  // Creates a ObjArray of the hits belonging to the track.
  SetCMDefaults();
} ///:~

//___________________________________________________________
CbmStsCMTrack::CbmStsCMTrack(Int_t tracknumber) {
  // Same as default constructor except that the track number is set.
  
  SetCMDefaults();
  SetTrackID(tracknumber);
}

//___________________________________________________________
CbmStsCMTrack::~CbmStsCMTrack() {

  delete fMappingHits;

}

//___________________________________________________________
void CbmStsCMTrack::SetCMDefaults() {
  // Executes the default setup for the track.

  fMappingHits = new TObjArray(0);

  SetRadius(0.);

}

//___________________________________________________________
void CbmStsCMTrack::SetTrackID(Int_t number) {

  // Sets the tracknumber.  If the track has already some hits
  // assigned the track number of the hits is also set.

  SetTrackNumber(number); // fTrackNumber = number;

  for (Int_t i = 0; i < fMappingHits->GetEntriesFast(); i++) {
    ((CbmStsHit*)fMappingHits->At(i))->SetTrackID(number);    
  }

}

//___________________________________________________________
void CbmStsCMTrack::AddPoint(CbmStsCMPoint* point) {
  // Adds a given point to the track.

  fRealHits->AddLast(point);
  fMappingHits->AddLast(point);
  point->SetUsage(kTRUE);

}

//___________________________________________________________
void CbmStsCMTrack::AddForwardPoint(CbmStsCMPoint* point) {
  // Shifts all found points by one. Adds a given point in the (now)
  // empty first slot.

  Int_t num = fMappingHits->GetEntriesFast();
  fMappingHits->Expand(num+1);
  fRealHits->Expand(num+1);

  for (Int_t i = num-1; i >= 0; i--) {
    fMappingHits->AddAt(fMappingHits->At(i), i+1);
    fRealHits->AddAt(fRealHits->At(i), i+1);
  }
  
  fMappingHits->AddFirst(point);
  fRealHits->AddFirst(point);
  point->SetUsage(kTRUE);

}

//___________________________________________________________
void CbmStsCMTrack::ClearPoints() {

  //  Release track points

  for (Int_t i = 0; i < fMappingHits->GetEntriesFast(); i++) {    
    CbmStsCMPoint *p = (CbmStsCMPoint *)fMappingHits->At(i);
    p->SetUsage(kFALSE);
  }
}

//______________________________________________________________
void CbmStsCMTrack::SetTrackMask() {

  fMask = 0;
  
  for (Int_t i = 0; i < fMappingHits->GetEntriesFast(); i++) {    
    CbmStsCMPoint *p = (CbmStsCMPoint *)fMappingHits->At(i);

    fMask += (Int_t)TMath::Power(2, ((p->GetStation()-1)%10)+1);
    
  }    
}

