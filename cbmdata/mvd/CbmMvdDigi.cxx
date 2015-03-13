// -----------------------------------------------------------------------
// -----               CbmMvdDigi source file                        -----
// -----              Created 17/04/08  by C. Dritsa                 -----
// -----------------------------------------------------------------------




#include <iostream>
#include "CbmMvdDigi.h"
#include "TMath.h"

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdDigi::CbmMvdDigi()
  : CbmDigi(),
    CbmMvdDetectorId(), 	
    fCharge(0.),
    fChannelNrX(0),
    fChannelNrY(0),
    fTrackID(-1),
    fPointID(0),
    fPixelSizeX(0.),
    fPixelSizeY(0.),
    fDetectorId(0),
    fChannelNr(0),
    fDigiTime(0.),
    fFrameNumber(0),
    fDigiFlag(-1),
    fRefId(-1)
{  
 
}
// -------------------------------------------------------------------------



/*// -----   Constructor with parameters   -----------------------------------
CbmMvdDigi::CbmMvdDigi(Int_t iStation, Int_t iChannelNrX, Int_t iChannelNrY, Float_t charge,
                       Float_t pixelSizeX, Float_t pixelSizeY)
:CbmDigi(kMVD, 0)
{
    // Check range for station
    if ( ! ( iStation >= 0 && iStation <= 255 ) ) {
	cout << "-E- CbmMvdDigi: Illegal station number " << iStation << endl;
	Fatal("", "Illegal station number");
    }

    fDetectorId = DetectorId(iStation);
    fChannelNrY = iChannelNrY;
    fCharge  = charge;
    fChannelNrX=iChannelNrX;
    fChannelNrY=iChannelNrY;
    fPixelSizeX=pixelSizeX;
    fPixelSizeY=pixelSizeY;
    
    fDigiFlag=-1;
    
}
// -------------------------------------------------------------------------*/

// -----   Constructor with parameters  --> used only due to error TODO include correct version -----------------------------------
CbmMvdDigi::CbmMvdDigi(Int_t iStation, Int_t iChannelNrX, Int_t iChannelNrY, Float_t charge,
                       Float_t pixelSizeX, Float_t pixelSizeY, Float_t time, Int_t frame)
 : CbmDigi(),
   CbmMvdDetectorId(), 	
   fCharge(charge),
   fChannelNrX(iChannelNrX),
   fChannelNrY(iChannelNrY),
   fTrackID(0),
   fPointID(0),
   fPixelSizeX(pixelSizeX),
   fPixelSizeY(pixelSizeY),
   fDetectorId(DetectorId(iStation)),
   fChannelNr(0),
   fDigiTime(time),
   fFrameNumber(frame),
   fDigiFlag(-1),
   fRefId(-1)
{
    // Check range for station
    if ( ! ( iStation >= 0 && iStation <= 600 ) ) {
	cout << "-E- CbmMvdDigi: Illegal station number " << iStation << endl;
	Fatal("", "Illegal station number");
    }
    
}
// -------------------------------------------------------------------------

Int_t CbmMvdDigi::GetAdcCharge(Int_t adcDynamic, Int_t adcOffset, Int_t adcBits)
{
    /**
     adcOffset  is the minimum value of the analogue signal
     adcDynamic is the difference between the max and min values of the full scale measurement range
     adcBits    is the number of bits used to encode the analogue signal

     Exemple:
     * If full scale measurement range is from 15 to 20 Volts
     * adcDynamic is the difference 20 - 15 = 5
     * adcOffset  = 15

     */

     Int_t adcCharge;

    if(fCharge<adcOffset){return 0;};

   
    Double_t stepSize;
    Int_t adcMax = adcOffset + adcDynamic;

    stepSize  = adcDynamic/TMath::Power(2,adcBits);
    adcCharge = int( (fCharge-adcOffset)/stepSize );


    if ( adcCharge>int( TMath::Power(2,adcBits)-1 ) )
    {
	adcCharge = (int)TMath::Power(2,adcBits)-1;

    }

    if(gDebug>0){
	cout << "CbmMvdDigi::GetAdcCharge() "<< adcCharge<< endl;
    }

    return adcCharge;

}


// -------------------------------------------------------------------------
Int_t CbmMvdDigi::GetPixelX(){
 return  fChannelNrX;

}
// -------------------------------------------------------------------------
Int_t CbmMvdDigi::GetPixelY(){
  return  fChannelNrY;

}
// -------------------------------------------------------------------------

     /** Unique channel address  **/
  Int_t    CbmMvdDigi::GetAddress() const{
 return  0;

}
// -------------------------------------------------------------------------
  /** System (enum DetectorId) **/
  Int_t   CbmMvdDigi:: GetSystemId() const{
 return  0;

}
// -------------------------------------------------------------------------

  /** Absolute time [ns]  **/
  Double_t CbmMvdDigi::GetTime() const{
 return  fDigiTime;

}
 
  
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdDigi::~CbmMvdDigi(){}
// -------------------------------------------------------------------------

ClassImp(CbmMvdDigi)
