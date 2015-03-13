/**
* \file CbmRichRecGeoPar.h
*
* \brief RICH geometry parameters for the reconstruction. This class is used for convinient storing
* of the basic RICH geometry parameters like: PMT and mirrors position, dimensions and tilting.
*
* \author Lebedev Semen
* \date 2013
**/

#ifndef CBM_RICH_REC_GEO_PAR
#define CBM_RICH_REC_GEO_PAR

#include <iostream>
#include "TMath.h"

using namespace std;

/**
* \class CbmRichMatchRings
*
* \brief RICH geometry parameters for the reconstruction. This class is used for convinient storing
* of the basic RICH geometry parameters like: PMT and mirrors position, dimensions and tilting.
*
* \author Lebedev Semen
* \date 2013
**/
class CbmRichRecGeoPar
{

public:

   /**
    * \brief Default constructor.
    */
   CbmRichRecGeoPar()
     : fPmtXOrig(0.),
    fPmtYOrig(0.),
    fPmtZOrig(0.),
    fPmtTheta(0.),
    fPmtPhi(0.),
    fPmtX(0.),
    fPmtY(0.),
    fPmtZ(0.),
    fPmtWidthX(0.),
    fPmtWidthY(0.),
    fNRefrac(0.),
    fMirrorX(0.),
    fMirrorY(0.),
    fMirrorZ(0.),
    fMirrorR(0.),
    fMirrorTheta(0.)
    {;}

   /**
    * \brief Destructor.
    */
   ~CbmRichRecGeoPar(){;}

   /**
    * \brief Print geometry parameters
    */
   void Print() {
      cout << endl << "-I- RICH geometry parameters" << endl;
      cout << "PMT position in (x,y,z) [cm]: " << fPmtXOrig << "  " << fPmtYOrig << "  " << fPmtZOrig << endl;
      cout << "Tilted PMT position in (x,y,z) [cm]: " << fPmtX << "  " << fPmtY << "  " << fPmtZ << endl;
      cout << "PMT size in x and y [cm]: " << fPmtWidthX << "  " << fPmtWidthY << endl;
      cout << "PMT was tilted around x by " << fPmtTheta*180./TMath::Pi() << " degrees" << endl;
      cout << "PMT was tilted around y by " << fPmtPhi*180./TMath::Pi() << " degrees" << endl;
      cout << "Refractive index for lowest photon energies: "<< fNRefrac << ", (n-1)*10000: " << (fNRefrac-1.0)*10000.0 << endl;

      cout << "Mirror center (x,y,z): " << fMirrorX << " " << fMirrorY << " " << fMirrorZ << endl;
      cout << "Mirror radius: " << fMirrorR << endl;
      cout << "Mirror tilting angle: " << fMirrorTheta*180./TMath::Pi() << " degrees" << endl << endl;
   }


public:
   Double_t fPmtXOrig; // X-coordinate of photodetector (original, before tilting)
   Double_t fPmtYOrig; // Y-coordinate of photodetector (original, before tilting)
   Double_t fPmtZOrig; // Z-coordinate of photodetector (original, before tilting)

   Double_t fPmtTheta; // angle by which photodetector was tilted around X-axis
   Double_t fPmtPhi; // angle by which photodetector was tilted around Y-axis

   Double_t fPmtX; // X-coordinate of photodetector after tilting
   Double_t fPmtY; // Y-coordinate of photodetector after tilting
   Double_t fPmtZ; // Z-coordinate of photodetector after tilting

   Double_t fPmtWidthX; // width of photodetector in X
   Double_t fPmtWidthY; // width of photodetector in Y

   Double_t fNRefrac; // refraction index

   Double_t fMirrorX; // X-coordinate of mirror center
   Double_t fMirrorY; // Y-coordinate of mirror center
   Double_t fMirrorZ; // Z-coordinate of mirror center
   Double_t fMirrorR; // mirror radius

   Double_t fMirrorTheta; // mirror rotation angle around X-axis
};

#endif
