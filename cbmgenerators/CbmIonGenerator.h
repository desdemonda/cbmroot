// -------------------------------------------------------------------------
// -----                   CbmIonGenerator source file                 -----
// -----       Created  04 Jun 11 by A.Chernogorov / S.Belogurov       -----
// -----       Modified 09 Apr 12 by A.Chernogorov / S.Belogurov       -----
// -------------------------------------------------------------------------

/**
* based on FairIonGenerator
*/

#ifndef CBMIONGERATOR_H
#define CBMIONGERATOR_H

#include "FairGenerator.h"

class FairPrimaryGenerator;
class FairIon;
class TString;

class CbmIonGenerator : public FairGenerator
{
 public:
  /** Default constructor
   ** For the generation of ions with atomic number z and mass number a.
   ** By default, the mass equals a times the proton mass and the
   ** excitation energy is zero. This can be changed with the
   ** respective modifiers.
   **
   **@param z         Atomic number
   **@param a         Atomic mass
   **@param q         Electric charge [e]
   **@param mult      Number of ions per event
   **@param p         Momentum [GeV/c] per nucleon!
   **@param vz        Vertex z-coordinate [cm]
   **
   ** Uncollimated beam is described by four Gaussian distributions (angular and spatial).
   ** Collimated beam is described by four functions each is a product of the Gaussian
   ** distribution and a trapezoid.
   ** Note that the created description has following free parameters: 4 standard deviations
   ** for angular and spatial distributions of x and y, respectively, and 16 parameters of
   ** trapezoids - 4 (start and end coordinates of the lateral sides) for each of four previously
   ** mentioned distributions.
   ** Parameters of the trapezoid are set with respect to mean of Gaussian, NOT necesseraly in absolute
   ** coordinates (x1 < x2 < 0 < x3 < x4).
   **
   **@param meanX, sigmaX              Gaus(mean,sigma) for beam spatial distribution before target [cm]
   **@param meanY, sigmaY              Gaus(mean,sigma) for beam spatial distribution before target [cm]
   **@param sigmaPxdivP, sigmaPydivP   Gaus(0,sigma) for beam angular distribution before target [rad]
   **@param x1 to x4,y1 to y4          Parameters of trapezoid (collimation case) [cm]
   **@param PxdivP1 to PxdivP4         Parameters of trapezoid (collimation case) [rad]
   **@param PydivP1 to PydivP4         Parameters of trapezoid (collimation case) [rad]
   **/
  CbmIonGenerator();
  
  CbmIonGenerator(Int_t z,
		  Int_t a,
		  Int_t q,
		  Int_t mult,
		  Double_t p,
		  Double_t sigmaX,
		  Double_t sigmaY,
		  Double_t sigmaPxdivP,
		  Double_t sigmaPydivP,
		  Double_t meanX=0., Double_t meanY=0., Double_t vz=-0.0125);
  
  CbmIonGenerator(Int_t z,
		  Int_t a,
		  Int_t q,
		  Int_t mult,
		  Double_t p,
		  Double_t sigmaX,
		  Double_t sigmaY,
		  Double_t sigmaPxdivP,
		  Double_t sigmaPydivP,
		  Double_t meanX, Double_t meanY, Double_t vz,
		  Double_t x1, Double_t x2, Double_t x3, Double_t x4,
		  Double_t y1, Double_t y2, Double_t y3, Double_t y4,
		  Double_t PxdivP1, Double_t PxdivP2, Double_t PxdivP3, Double_t PxdivP4,
		  Double_t PydivP1, Double_t PydivP2, Double_t PydivP3, Double_t PydivP4);
  
  /** Destructor **/
  virtual ~CbmIonGenerator();
  
  /** Modifiers **/
  //void SetCharge(Int_t charge){fQ=charge;}
  //void SetExcitationEnergy(Double_t eExc);
  //void SetMass(Double_t mass);
  
  /** Method ReadEvent
   ** Generates <mult> of the specified ions and hands them to the FairPrimaryGenerator.
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);
  
 private:
  static Int_t fgNIon;			//! Number of the instance of this class
  Int_t fMult;				// Multiplicity per event
  Double_t fP;				// Momentum [GeV/c] per nucleon
  Double_t fVz;				// Vertex z-coordinate [cm]
  FairIon *fIon;				// Pointer to the FairIon to be generated
  //Int_t fQ;				// Electric charge [e]
  
  /**
   ** fcollimationFlag == 0 - off
   ** fcollimationFlag == 1 - on
   **/
  Int_t fcollimationFlag;
  // spatial distr.
  Double_t fmeanX, fsigmaX, fx[4];    // Gaus(mean,sigma), trapezoid(x[4])
  Double_t fmeanY, fsigmaY, fy[4];    // Gaus(mean,sigma), trapezoid(y[4])
  // angular distr.
  Double_t fsigmaPxdivP, fPxdivPa[4]; // Gaus(0.,sigma), trapezoid(PxdivPa[4])
  Double_t fsigmaPydivP, fPydivPa[4]; // Gaus(0.,sigma), trapezoid(PydivPa[4])
  
  Double_t CalculateCollDistrValue(Double_t fsigma, Double_t* fa);
  
  CbmIonGenerator(const CbmIonGenerator&);
  CbmIonGenerator& operator=(const CbmIonGenerator&);
  
  ClassDef(CbmIonGenerator, 1)
};

#endif // CBMIONGERATOR_H
