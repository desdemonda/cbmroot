/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
// CbmStsCMTrack class - representation of one STS track with CM points        //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#ifndef CBM_STS_CM_Track_H
#define CBM_STS_CM_Track_H

#include "TObject.h"
#include "TObjArray.h"
#include "TVector3.h"

#include "CbmStsJointTrack.h"
#include "CbmStsCMPoint.h"
#include "CbmStsHit.h"

#include "Riostream.h"

typedef unsigned char Mask8_t;

class CbmStsCMTrack : public CbmStsJointTrack {
  
private:
  
  TObjArray *fMappingHits;         // Array of pointers to hits of track
  Mask8_t    fMask;           // Binary pattern to know in which station  track has points
  
  Double_t   fChi2Circle;     // Chi squared of circle fit
  Double_t   fChi2Deep;       // Chi squared of length fit
  Double_t   fRadius;         // Radius of the helix (projected to a circle)

public:
  
  CbmStsCMTrack();                    // constructor
  CbmStsCMTrack(Int_t tracknumber);   // constructor with tracknumber

  virtual  ~CbmStsCMTrack();          // destructor

  void   SetCMDefaults();               // performs the default setup for the track
  void   AddPoint(CbmStsCMPoint *point);  // adds a point to the track
  void   AddForwardPoint(CbmStsCMPoint* point);   // shifts and adds first point

  // getters

  TObjArray  *GetCMHits()          const { return fMappingHits; }
  Int_t      GetMask()             const { return fMask; }
  Double_t   GetChi2Circle()       const { return fChi2Circle; }
  Double_t   GetChi2Deep()         const { return fChi2Deep; }
  Double_t   GetRadius()           const { return fRadius; }
  Int_t      GetNumberOfPoints()   const { return fMappingHits->GetEntriesFast(); }

  // setters   
  void   SetTrackID(Int_t number);
  void   SetChi2Circle(Double_t f)     { fChi2Circle = f; }
  void   SetChi2Deep(Double_t f)       { fChi2Deep = f; }
  void   SetRadius(Double_t f)         { fRadius = f; }
  void   ClearPoints();  
  void   SetTrackMask();

//________________________________________________________________
inline void printMask() {

  unsigned char val = fMask;
  cout.setf(ios::oct, ios::basefield);

  for(int i = 7; i >= 0; i--)
    if(val & (1 << i))
      std::cout << "1";
    else
      std::cout << "0";

  std::cout << endl;
  cout.setf(ios::dec, ios::basefield);
} ///:~

  ClassDef(CbmStsCMTrack, 1)    // 

};

#endif

