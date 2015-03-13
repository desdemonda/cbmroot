#ifndef CBMECALGENERATOR_H
#define CBMECALGENERATOR_H

#include "FairGenerator.h"

//#include <iostream>


class FairPrimaryGenerator;

class CbmEcalGenerator : public FairGenerator
{
public:

  /** Default constructor. **/
  CbmEcalGenerator();

  /** Constructor with PDG-ID, multiplicity
   **@param pdgid Particle type (PDG encoding)
   **@param mult  Multiplicity (default is 1)
   **/
  CbmEcalGenerator(Int_t pdgid, Int_t mult=1);

  /** Destructor **/
  virtual ~CbmEcalGenerator() {};

  /** Modifiers **/
  void SetPDGType      (Int_t pdg)  {fPDGType = pdg;  }
  void SetPDGType2     (Int_t pdg)  {fPDGType2= pdg;  }

  void SetMultiplicity (Int_t mult) {fMult    = mult; }
  void SetDistance(Double_t min, Double_t max) {fDistanceMin=min; fDistanceMax=max;}

  void SetPRange(Double32_t pmin=0   , Double32_t pmax=10)
    {fPMin=pmin; fPMax=pmax;}

  void SetPRange2(Double32_t pmin=0   , Double32_t pmax=10)
    {fPMin2=pmin; fPMax2=pmax;}

  void SetEcalZ(Double32_t ecalz=1190)
    {fEcalZ=ecalz;}

  void SetPhiRange     (Double32_t phimin=0  , Double32_t phimax=360)
    {fPhiMin=phimin; fPhiMax=phimax;};

  void SetThetaRange   (Double32_t thetamin=0, Double32_t thetamax=90)
    {fThetaMin=thetamin; fThetaMax=thetamax;};

  void SetDebug(Bool_t debug=0) {fDebug = debug;}

  /** Initializer **/
  Bool_t Init();

  /** Creates an event with given type and multiplicity.
   **@param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

private:
  Int_t      fPDGType;             // Particle type (PDG encoding)
  Int_t      fMult;                // Multiplicity

  Double32_t fPDGMass;             // Particle mass [GeV]
  Double32_t fPhiMin, fPhiMax;     // Azimuth angle range [degree]
  Double32_t fPMin,   fPMax;       // Momentum range in lab system
  Double32_t fThetaMin, fThetaMax; // Polar angle range in lab system [degree]

  Double32_t fEcalZ;
  Double32_t fDistanceMin;
  Double32_t fDistanceMax;
  Double32_t fPMin2,   fPMax2;       // Momentum range in lab system
  Int_t fPDGType2;

  Bool_t     fDebug;               // Debug switch

  ClassDef(CbmEcalGenerator,3);

};

#endif
