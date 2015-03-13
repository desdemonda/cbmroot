// -------------------------------------------------------------------------
// -----                     CbmMvdMimosa26AHR source file             -----
// -----                  Created 31/01/11  by M. Deveaux              -----
// -------------------------------------------------------------------------

#include "CbmMvdMimosa26AHR.h"
#include "TMath.h"

#include <iostream>


using std::cout;
using std::endl;



// -----   Default constructor   -------------------------------------------
CbmMvdMimosa26AHR::CbmMvdMimosa26AHR()  {
  
  fMimosaName="Mimosa-26AHR";       // Clear name of the simulated sensor
  fPixelPitchX=0.0022;	  	    // Pixel pitch in x of this sensor in cm
  fPixelPitchY=0.0033;              // Pixel pitch in y of this sensor in cm
  fNPixelsX=1362;    		    // Number of pixels in row
  fNPixelsY=300;  		    // Number of pixels in col
  fNPixels=fNPixelsX*fNPixelsY;	    // Number of pixels in sensor
  fPixelSignX=1;       		    // Direction of the pixel count X, if true, Pixel x=0 is at
				    // left corner, else at right corner
  fPixelSignY=1;       		    // Direction of the pixel count Y, if true, Pixel x=0 is at
				    // the lower corner, else at upper corner
  fShutterSign=1;				    
  fIntegrationTime=30.0e3;  	    // Integration time of the pixels in ns
  fEpiTh=18e-4;		   	    // Thickness of the epitaxial layer
  
  fChargeThreshold = 1.;

  /** Description of the sensor for the digitizer //mesaured by melisa for Mimosa26AHR**/
       fNoise	     = 13;
       fLorentzPar0 = 520.;
       fLorentzPar1 = 0.34;
       fLorentzPar2 = -1.2;
       fLandauMPV   = 877.4;
       fLandauSigma = 204.93;
       fLandauGain= 3.58;
  
  /** ADC description **/
  
  fAdcDynamic = 150;
  fAdcOffset  = 0;
  fAdcBits    = 1;
  fAdcSteps= TMath::Power(2,fAdcBits);
  fAdcStepSize=fAdcDynamic/fAdcSteps;
  fStatesPerBank=6;
  fStatesPerLine=9;
  fStatesPerFrame=570;
  fPixelsPerBank=64;
  
  
  /** Self-organizsation **/
   
  fValidData=kTRUE;
  
  
}

// -----   Destructor   ----------------------------------------------------
CbmMvdMimosa26AHR::~CbmMvdMimosa26AHR() {
}
// -------------------------------------------------------------------------

ClassImp(CbmMvdMimosa26AHR)
