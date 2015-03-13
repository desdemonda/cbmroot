#ifndef CBMSTTHIT_H
#define CBMSTTHIT_H

#include "CbmHit.h"

class CbmSttPoint;
class TVector3;

class CbmSttHit : public CbmHit {

private:
    Int_t           fPlaneID; // unique ID of STT plane in the setup (1 - 18)
    Int_t           fChamber; // chamber No.
    Int_t           fTube;    // tube No. in the plane
    Int_t           fSegment; // segment No. along the wire
    Double32_t      fAlong;   // hit position along the wire
    Double32_t      fU;       // hit coordinate across the wire in local coordinate system (rotated) 

public:
    CbmSttHit();
    CbmSttHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index,
	      CbmSttPoint *point);
    virtual ~CbmSttHit();
    void Print(const Option_t *option) const {}
    inline Int_t    GetPlaneID() const        { return fPlaneID; }
    inline Int_t    GetChamber() const        { return fChamber; }
    inline Int_t    GetTube() const           { return fTube; }
    inline Int_t    GetSegment() const        { return fSegment; }
    inline Double_t GetAlong() const          { return fAlong; }
    inline Int_t GetStationNr() const         { return fPlaneID; }
    inline Double_t GetU() const              { return fU; } 

    inline void SetPlaneID(Int_t plane)       { fPlaneID = plane; }
    inline void SetChamber(Int_t chNo)        { fChamber = chNo; }
    inline void SetTube(Int_t tubeNo)         { fTube = tubeNo; }
    inline void SetSegment(Int_t segNo)       { fSegment = segNo; }
    inline void SetAlong(Double_t along)      { fAlong = along; }
    inline void SetU(Double_t u)              { fU = u; }

    ClassDef(CbmSttHit, 1)
};

#endif

