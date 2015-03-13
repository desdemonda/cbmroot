#include "CbmStsHit.h"
#include "CbmStsJointTrack.h"
#include "CbmStsVertex.h"
#include "TClonesArray.h"

#include "Riostream.h"
#include "iomanip.h"
////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
// CbmStsJointTrack  class - track representation for the STS                     //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////

ClassImp(CbmStsJointTrack)

//______________________________________________________________
CbmStsJointTrack::CbmStsJointTrack () {
  // Default constructor.

  SetDefaults();
}

//______________________________________________________________
CbmStsJointTrack::CbmStsJointTrack (Int_t tracknumber) {
  // Same as default constructor except that the track number is set.
  
  SetDefaults();
  SetTrackNumber(tracknumber);
}

//______________________________________________________________
CbmStsJointTrack ::~CbmStsJointTrack () {
  // Destructor.

  //  fPoints->Delete();
  delete fRealHits;
  fRealHits = 0;

}

//___________________________________________________________
void CbmStsJointTrack ::SetDefaults() {
  // Default setup for the track.

  fRealHits = new TRefArray();

  fTrackNumber = -1;

  SetPid(0);

  ComesFromMainVertex(kFALSE);

  fP.SetX(0.);
  fP.SetY(0.);
  fP.SetZ(0.);

  fVertex.SetX(0.);
  fVertex.SetY(0.);
  fVertex.SetZ(0.);

  fQ = 0;

}

//______________________________________________________________
void CbmStsJointTrack ::SetTrackNumber(Int_t number) {

  // Sets the tracknumber for track.
  fTrackNumber = number;
}

//______________________________________________________________
void CbmStsJointTrack::SetVertex(Double_t vx, Double_t vy, Double_t vz) {

   fVertex.SetX(vx);
   fVertex.SetY(vy);
   fVertex.SetZ(vz);
}

//______________________________________________________________
 void CbmStsJointTrack::AddHit(CbmStsHit* point) {
  // Adds a hit to the track.

  fRealHits->AddLast(point);
  point->SetUsage(kTRUE);
}

//______________________________________________________________
void CbmStsJointTrack::Print(TClonesArray * fHits) {

    Int_t nhit = fRealHits->GetEntries();

    cout << "\n Track " << fTrackNumber << "  nhits " << nhit;
    cout << " Primary "  << fFromMainVertex << endl;

    cout << " vertex XYZ: " << setw(5) << fVertex.GetX();
    cout << " " << setw(5) << fVertex.GetY();
    cout << " " << setw(5) << fVertex.GetZ();

    cout << " Pxyz: ";
    cout.precision(3);
    cout << " " << setw(7) <<  fP.X();
    cout << " " << setw(7) <<  fP.Y();
    cout << " " << setw(7) <<  fP.Z();
    cout << endl;

    CbmStsHit * ghit = NULL;

    for (Int_t j=0; j<nhit; j++) {
      ghit = (CbmStsHit *) fRealHits->At(j);
      ghit->Print();
    }

}

//______________________________________________________________
void CbmStsJointTrack::PrintLV (TLorentzVector* lvect) {

  float p,mass,e;
  e=lvect->E();
  p=lvect->Vect().Mag();
  mass =TMath::Sqrt(e*e-p*p);
  cout << " p="<<p<<" ("<<lvect->Px()<<","<<lvect->Py()<<","<<lvect->Pz()<<"), E="
       <<e<<", m="<<mass<<endl;
}

