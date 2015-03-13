//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// CbmStsHit class - representation of STS hit for the STS trackers.            //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#ifndef CBM_STS_HIT_H
#define CBM_STS_HIT_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"

#include "CbmStsPoint.h"
//#include "CbmStsTrack.h"

class CbmStsJointTrack;

class CbmStsHit : public TObject {
  
private:
  
  TVector3  fCoord;         // vector of hit coordinates
  TVector3  fError;         // vector of errors on hit coordinates
  
  
  Bool_t    fUsed;          // flag that indicates if the hit is assigned to a track 
  Int_t     fHitNumber;     // number of this hit in this event
  Int_t     fTrackID;       // track number to which this hit belongs to
  Int_t     fStation;       // STS station number

public:
  
  CbmStsHit();                          // default constructor
  CbmStsHit(CbmStsPoint *point);        // constructor for data after hit finding
  CbmStsHit(Double_t *x, Int_t stn);    // constructor which take an arbitrary point
  virtual ~CbmStsHit();                 // destructor

  
  // getters
  TVector3 GetCoord()  { return fCoord; }
  TVector3 GetError()  { return fError; }
  
  Double_t GetX()          const { return fCoord.X(); }
  Double_t GetY()          const { return fCoord.Y(); }
  Double_t GetZ()          const { return fCoord.Z(); }
  Double_t GetXerr()       const { return fError.X(); }
  Double_t GetYerr()       const { return fError.Y(); }
  Double_t GetZerr()       const { return fError.Z(); }
  
  CbmStsJointTrack *GetTrack(TClonesArray *tracks) const;

  Bool_t   GetUsage()         const { return fUsed; }
  Int_t    GetHitNumber()     const { return fHitNumber; }
  Int_t    GetTrackID()       const { return fTrackID; }
  Int_t    GetStation()       const { return fStation; }

  
  // setters  
  void  SetX(Double_t f)        { fCoord.SetX(f); }
  void  SetY(Double_t f)        { fCoord.SetY(f); } 
  void  SetZ(Double_t f)        { fCoord.SetZ(f); }
  void  SetXerr(Double_t f)     { fError.SetX(f); }
  void  SetYerr(Double_t f)     { fError.SetY(f); }
  void  SetZerr(Double_t f)     { fError.SetZ(f); }
  
  void  SetUsage(Bool_t f)        { fUsed =  f; }
  void  SetHitNumber(Int_t f)     { fHitNumber =  f; }
  void  SetTrackID(Int_t f)       { fTrackID =  f; }
  void  SetStation(Long_t f)      { fStation =  f; }

  void Print();
  
  
  ClassDef(CbmStsHit, 1)   //STS point class
};

#endif
