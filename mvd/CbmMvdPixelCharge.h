// -------------------------------------------------------------------------
// -----                     CbmMvdDigi header file                    -----
// -----                    Created 02/04/08  by C.Dritsa              -----
// -------------------------------------------------------------------------



#ifndef CBMMVDPIXELCHARGE_H
#define CBMMVDPIXELCHARGE_H 1

#include "TObject.h"
#include "TMath.h"



class CbmMvdPixelCharge : public TObject
{

public:

    /** Default constructor **/
    CbmMvdPixelCharge();

    /** Constructor with all variables **/

    CbmMvdPixelCharge(Float_t charge, Int_t channelNrX, Int_t channelNrY, Int_t hitId,
		      Int_t trackId,Float_t pointX, Float_t pointY, Float_t time=0.0, Int_t frame=0);
    virtual ~CbmMvdPixelCharge();

    Bool_t TestXY(Int_t channelNrX,Int_t channelNrY);


    void DigestCharge(Float_t pointX, Float_t pointY, Int_t PointId, Int_t trackId);
    void AddCharge(Float_t charge){ fTrackCharge = fTrackCharge+charge; };


    Float_t  GetCharge()   { return fCharge;           };
    Int_t    GetX()        { return fChannelNrX;       };
    Int_t    GetY()        { return fChannelNrY;       };
    Float_t  GetMaxChargeContribution()	{return fMaxChargeContribution;};
    Short_t  GetNContributors()		{return fContributors;};
    Int_t    GetDominatorIndex()	{return fDominatorIndex;}
    Int_t*   GetTrackID()    		{return fTrackId;}
    Int_t*   GetPointID()		{return fPointId;}
    Float_t* GetPointX()		{return fPointX;}
    Float_t* GetPointY()		{return fPointY;}
    Float_t* GetPointWeight()           {return fPointWeight;}
    Float_t  GetTime()			{return fPixelTime;}
    Int_t    GetFrame()			{return fFrame;}
    
    void Clear(){};

    


private:

    Int_t fFrame; 
  
    Float_t fCharge;
    Float_t fMaxChargeContribution;
    Float_t fDominatingPointX;
    Float_t fDominatingPointY;
    Short_t fContributors;
    Int_t fChannelNrX;
    Int_t fChannelNrY;
    Float_t fTrackCharge;
    Int_t fDominatorTrackId;
    Int_t fDominatorPointId;
    Int_t fTrackId[5];
    Int_t fPointId[5];
    Float_t fPointWeight[5];
    Float_t fPointX[5];
    Float_t fPointY[5];
    Short_t fDominatorIndex;
    Float_t fPixelTime;
    


    ClassDef(CbmMvdPixelCharge,1);

};


#endif

