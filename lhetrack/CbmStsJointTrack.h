/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//       CbmSTSTrack class - implementation of STS track                       //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#ifndef CBM_STS_JOINT_TRACK_H
#define CBM_STS_JOINT_TRACK_H

#include "TObject.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include "TRefArray.h"
#include "CbmStsHit.h"
#include "CbmStsVertex.h"
#include "CbmStsMassHypothesis.h"

class CbmStsJointTrack: public TObject {
  
protected:
  
  TRefArray  *fRealHits;           // Array of indices of hits

  Int_t   fQ;                   // track charge
  Int_t   fTrackNumber;         // number of track
  Bool_t  fFromMainVertex;      // true for primary tracks
  
  CbmStsVertex   fVertex;              // track vertex
  CbmStsVertex   fLastHit;             // last track hit in STS

  CbmStsMassHypothesis fIn;   // momenta & angles for initial track 
  CbmStsMassHypothesis fOut;  // momenta & angles for output track 

  //  ---  the code below is needed only for testing with Geant

  Int_t   fPid;               // GEANT particle id
  TVector3   fP;              // track momentum

  Double_t   fMoment;         // momentum estimation
  Double_t   fBeta;           // 
  Double_t   fTanAlpha;       //

  // -----------

public:
  
  CbmStsJointTrack ();                              // constructor
  CbmStsJointTrack (Int_t tracknumber);             // constructor with tracknumber

  virtual  ~CbmStsJointTrack ();                    // destructor

  void SetDefaults();                // performs the default setup for the track
  void AddHit(CbmStsHit *point);     // adds a hit to the track

  // getters

  TRefArray  *GetRHits()           const { return fRealHits; }

  Int_t   GetTrackNumber()         const { return fTrackNumber; }
  Int_t   GetPid()                 const { return fPid; }
  Int_t   GetNumberOfHits()        const { return fRealHits->GetEntries(); }
  Bool_t  ComesFromMainVertex()    const { return fFromMainVertex; }
  TVector3   GetMomentum()         const { return fP; }

  CbmStsMassHypothesis  GetInMomenta()        const { return fIn; }
  CbmStsMassHypothesis  GetOutMomenta()       const { return fOut; }

  Double_t  GetMoment ()           { return fMoment; }
  Double_t  GetBeta ();
  Double_t  GetTanAlpha ();

  Double_t   GetPx()                const { return fP.X(); }
  Double_t   GetPy()                const { return fP.Y(); }
  Double_t   GetPz()                const { return fP.Z(); }
  Double_t   GetPt()                const;
  Double_t   GetP()                 const;
  Double_t   GetPseudoRapidity()    const;

  CbmStsVertex   GetVertex()        const { return fVertex; }
  Int_t   GetCharge()               const { return fQ; }

  // setters   

  void   SetTrackNumber(Int_t num);
	    
  void   SetPx(Double_t f) {fP.SetX(f); }
  void   SetPy(Double_t f) {fP.SetY(f); }
  void   SetPz(Double_t f) {fP.SetZ(f); }

  void   SetInAngle(Double_t alpha, Double_t beta);
  void   SetInMomenta(Double_t prot, Double_t kaon,
		      Double_t pion, Double_t elec);

  void   SetOutAngle(Double_t alpha, Double_t beta);
  void   SetOutMomenta(Double_t prot, Double_t kaon,
		       Double_t pion, Double_t elec);

  void   SetMoment(Double_t f)      { fMoment = f;   }
  void   SetBeta(Double_t f)  { fBeta = f;  }
  void   SetTanAlpha(Double_t f) { fTanAlpha = f; }

  void   SetVertex(Double_t vx, Double_t vy, Double_t vz);

  void   SetCharge(Int_t f) {fQ = f; }
  void   SetPid(Int_t f) {fPid = f; }

  void   ComesFromMainVertex(Bool_t f) { fFromMainVertex = f; }

  void   Print(TClonesArray * fHit);

  void   PrintLV(TLorentzVector * fLv);

  ClassDef(CbmStsJointTrack , 1)    // STS track class  

    };

//________________________________________________________________
inline  Double_t  CbmStsJointTrack::GetBeta () {
 return TMath::ATan2 (fP.X(), fP.Z());
}

//________________________________________________________________
inline Double_t  CbmStsJointTrack::GetTanAlpha () {
  return   fP.Y()/TMath::Sqrt(fP.X()*fP.X() + fP.Z()*fP.Z());
 }

//________________________________________________________________
inline Double_t CbmStsJointTrack::GetP() const
{
  // Returns total momentum.
  return fP.Mag();
}

//________________________________________________________________
inline Double_t CbmStsJointTrack::GetPt() const
{
  // Returns transverse momentum.
  return fP.Pt();
}

//________________________________________________________________
inline Double_t CbmStsJointTrack::GetPseudoRapidity() const
{
  // Returns the pseudorapidity of the particle.

  return 0.5 * TMath::Log((fP.Mag() + fP.Z()) / (fP.Mag() - fP.Z()));  
}

//________________________________________________________________
inline void CbmStsJointTrack::SetInAngle(Double_t alpha, Double_t beta) {
  fIn.SetAngle(alpha, beta);
}

//________________________________________________________________
inline void CbmStsJointTrack::SetOutAngle(Double_t alpha, Double_t beta) {
  fOut.SetAngle(alpha, beta);
}

//________________________________________________________________
inline void CbmStsJointTrack::SetInMomenta(Double_t pr, Double_t kn,
				       Double_t pi, Double_t el) {
  fIn.SetMomenta(pr, kn, pi, el);
}

//________________________________________________________________
inline void CbmStsJointTrack::SetOutMomenta(Double_t pr, Double_t kn,
				       Double_t pi, Double_t el) {
  fOut.SetMomenta(pr, kn, pi, el);
}

#endif
