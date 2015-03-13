// -------------------------------------------------------------------------
// -----                     CbmMvdDigi header file                    -----
// -----                    Created 02/04/08  by C.Dritsa              -----
// -------------------------------------------------------------------------

// TODO: Include GetAddress,  GetLinks, GetSystemId, GetTime

#ifndef CBMMVDDIGI_H
#define CBMMVDDIGI_H 1

#include "CbmDetectorList.h"
#include "CbmDigi.h"
#include "CbmMvdDetectorId.h"

#include "TObject.h"
#include "TMath.h"
#include "TVector3.h"


class CbmMvdDigi : public CbmDigi, public CbmMvdDetectorId
{

public:

    /** Default constructor **/
    CbmMvdDigi();

    /** Constructor with all variables **/

    CbmMvdDigi(Int_t iStation, Int_t iChannelNrX, Int_t iChannelNrY, Float_t charge, Float_t pixelSizeX,
    Float_t pixelSizeY, Float_t time=0.0, Int_t frame = 0);
    /**
     charge     : of each fired pixel in electrons
     PixelSize  : in cm
    */

    /** Destructor **/
    ~CbmMvdDigi();

    /** Accessors **/
    Double_t  GetCharge()   { return fCharge;     };
    Int_t    GetPixelX();
    Int_t    GetPixelY();
    Double_t GetPixelSizeX() { return  fPixelSizeX; };
    Double_t GetPixelSizeY() { return  fPixelSizeY; };
    Int_t    GetAdcCharge(Int_t adcDynamic, Int_t adcOffset, Int_t adcBits);
    Int_t    GetFlag(){return fDigiFlag;};
    Int_t    GetStationNr() { return StationNr(fDetectorId); };
    Int_t    GetDetectorId() {return fDetectorId;};
    Int_t    GetAddress() const; /** Unique channel address  **/
    Int_t    GetSystemId() const; /** System (enum DetectorId) **/
    Double_t GetTime() const;/** Absolute time [ns]  **/
    Int_t    GetFrameNumber() {return fFrameNumber;};
    

    Int_t    GetRefId() const { return fRefId; };   



    //these functions are only for littrack
    Int_t GetDominatorX(){ return 0;};
    Int_t GetDominatorY(){return 0;};
    Int_t GetTrackID(){return 0;};
    Int_t GetContributors(){return 0;};
    Int_t GetPointID(){return 0;};
    //
    
    
    /** Modifiers **/
    void SetCharge(Float_t charge) { fCharge     = charge; };
    void SetPixelX(Int_t xIndex) { fChannelNrX = xIndex; };
    void SetPixelY(Int_t yIndex) { fChannelNrY = yIndex; };
    void SetPixelIndex(Int_t pixelIndex) { fChannelNr  = pixelIndex; };
    void SetDetectorId(Int_t detId)      { fDetectorId = detId;      };
    void SetFlag(Int_t flag)             { fDigiFlag   = flag;       }
    void SetFrameNr(Int_t frame) {fFrameNumber = frame;};
    void SetRefId( Int_t refId ) { fRefId = refId; }
private:


    Float_t fCharge;
    Int_t fChannelNrX;
    Int_t fChannelNrY;
    Int_t fTrackID;
    Int_t fPointID;
    Float_t fPixelSizeX;
    Float_t fPixelSizeY;
    Int_t fDetectorId;
    Int_t fChannelNr;
    Double_t fDigiTime;
    Int_t fFrameNumber;
    Int_t fRefId;


    Int_t fDigiFlag; // Debugging and analysis information

    ClassDef(CbmMvdDigi,1);

};


#endif

