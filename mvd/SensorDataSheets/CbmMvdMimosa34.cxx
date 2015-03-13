// -------------------------------------------------------------------------
// -----                     CbmMvdMimosa34 source file             -----
// -----                  Created 31/01/11  by M. Deveaux              -----
// -------------------------------------------------------------------------

#include "CbmMvdMimosa34.h"
#include "TMath.h"

#include <iostream>


using std::cout;
using std::endl;



// -----   Default constructor   -------------------------------------------
CbmMvdMimosa34::CbmMvdMimosa34()  {
  
  fMimosaName="Mimosa-34";       // Clear name of the simulated sensor
  fPixelPitchX=0.0033;	  	    // Pixel pitch in x of this sensor in cm
  fPixelPitchY=0.0066;              // Pixel pitch in y of this sensor in cm
  fNPixelsX=1364;    		    // Number of pixels in row
  fNPixelsY=455;  		    // Number of pixels in col
  fNPixels=fNPixelsX*fNPixelsY;	    // Number of pixels in sensor
  fPixelSignX=1;       		    // Direction of the pixel count X, if true, Pixel x=0 is at
				    // left corner, else at right corner
  fPixelSignY=1;       		    // Direction of the pixel count Y, if true, Pixel x=0 is at
				    // the lower corner, else at upper corner
  fShutterSign=1;				    
  fIntegrationTime=30.0e3;  	    // Integration time of the pixels in ns
  fEpiTh=20e-4;		   	    // Thickness of the epitaxial layer
  
  fChargeThreshold = 10.;

  /** Description of the sensor for the digitizer //mesaured by Benny for Mimosa34**/
       fNoise	     = 12; // in e
       fLorentzPar0 = 1055.914; // the resulting function is in mm and in e
       fLorentzPar1 = 0.53434;
       fLorentzPar2 = 0;
       fLandauMPV   = 588.77;  // the resulting function is in e
       fLandauSigma = 165.72;
       fLandauGain= 3.8758; // the physical gain is define as the reverse of this value.... and has unit 1/e
  
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
CbmMvdMimosa34::~CbmMvdMimosa34() {
}
// -------------------------------------------------------------------------

ClassImp(CbmMvdMimosa34)
