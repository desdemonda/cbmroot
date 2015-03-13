#include "CbmEcalGenerator.h"

#include "FairPrimaryGenerator.h"

#include "TRandom.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TMath.h"

//#include <iostream>

//using namespace std;

// ------------------------------------------------------------------------
CbmEcalGenerator::CbmEcalGenerator() :
  fPDGType(0),fMult(0),fPDGMass(0),
  fPhiMin(0),fPhiMax(0),
  fPMin(0),fPMax(0),fThetaMin(0),fThetaMax(0),
  fEcalZ(1190),fDistanceMin(-1111), fDistanceMax(-1111),
  fPMin2(-1111), fPMax2(-1111), fPDGType2(0), fDebug(0)
{
  // Default constructor
}

// ------------------------------------------------------------------------
CbmEcalGenerator::CbmEcalGenerator(Int_t pdgid, Int_t mult) :
  fPDGType(pdgid),fMult(mult),fPDGMass(0),
  fPhiMin(0),fPhiMax(0),
  fPMin(0),fPMax(0),fThetaMin(0),fThetaMax(0),
  fEcalZ(1190),fDistanceMin(-1111), fDistanceMax(-1111),
  fPMin2(-1111), fPMax2(-1111), fPDGType2(0), fDebug(0)
{
  // Constructor. Set default kinematics limits
  SetPhiRange  ();
}

// ------------------------------------------------------------------------
Bool_t CbmEcalGenerator::Init()
{
  // Initialize generator

  if (fPhiMax-fPhiMin>360)
    Fatal("Init","CbmEcalGenerator: phi range is too wide: %f<phi<%f",
	  fPhiMin,fPhiMax);

  // Check for particle type
  TDatabasePDG* pdgBase = TDatabasePDG::Instance();
  TParticlePDG *particle = pdgBase->GetParticle(fPDGType);
  if (! particle) Fatal("Init","PDG code %d not defined.",fPDGType);
  fPDGMass = particle->Mass();

  return kTRUE;
}

// ------------------------------------------------------------------------
Bool_t CbmEcalGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
  // Generate one event: produce primary particles emitted from one vertex.
  // Primary particles are distributed uniformly along
  // those kinematics variables which were limitted by setters.
  // if SetCosTheta() function is used, the distribution will be uniform in 
  // cos(theta)

  Double32_t pabs=0, phi, theta=0, pt, px, py, pz=0, x, y, z, tr, r;

  // Generate particles
  for(Int_t k=0;k<fMult;k++)
  {
    phi=gRandom->Uniform(fPhiMin,fPhiMax)*TMath::DegToRad();

    pabs=gRandom->Uniform(fPMin,fPMax);
    theta=gRandom->Uniform(fThetaMin,fThetaMax)*TMath::DegToRad();
    pz=pabs*TMath::Cos(theta);
    pt=pabs*TMath::Sin(theta);
    px=pt*TMath::Cos(phi);
    py=pt*TMath::Sin(phi);
    z=fEcalZ;
    tr=z*TMath::Tan(theta);
    x=tr*TMath::Cos(phi);
    y=tr*TMath::Sin(phi);

    primGen->AddTrack(fPDGType, px, py, pz, x, y, z);
    if (fDistanceMax<0) continue;
    phi=gRandom->Uniform(0.0, 360.0)*TMath::DegToRad();
    r=gRandom->Uniform(fDistanceMin, fDistanceMax);
    x+=r*TMath::Cos(phi);
    y+=r*TMath::Sin(phi);
    pabs=gRandom->Uniform(fPMin2, fPMax2);
    r=TMath::Sqrt(x*x+y*y+z*z);
    px=pabs/r*x;
    py=pabs/r*y;
    pz=pabs/r*z;
    if (fPDGType2==0) fPDGType2=fPDGType;
    primGen->AddTrack(fPDGType2, px, py, pz, x, y, z);
  }
  return kTRUE;

}
// ------------------------------------------------------------------------


ClassImp(CbmEcalGenerator)
