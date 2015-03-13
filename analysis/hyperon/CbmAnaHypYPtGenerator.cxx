// -------------------------------------------------------------------------
// -----         CbmAnaHypYPtGenerator source file                     -----
// -----          Created 03/10/04  by E. Kryshen                      -----
// -------------------------------------------------------------------------

#include "TRandom.h"
#include "CbmAnaHypYPtGenerator.h"
#include "FairPrimaryGenerator.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TF1.h"
#include "TMath.h"

// ------------------------------------------------------------------------
CbmAnaHypYPtGenerator::CbmAnaHypYPtGenerator()
  : FairGenerator(),
    fPDGType(-1),
    fMult(0),
    fT(0.),
    fY0(0.),
    fSigma(0.),
    fPtMin(0.),
    fPtMax(0.),
    fPDGMass(0.),
    fDistPt(NULL)
{
}

// ------------------------------------------------------------------------
CbmAnaHypYPtGenerator::CbmAnaHypYPtGenerator(Int_t pdgid, Int_t mult)
  : FairGenerator(),
    fPDGType(pdgid),
    fMult(mult),
    fT(0.),
    fY0(0.),
    fSigma(0.),
    fPtMin(0.),
    fPtMax(0.),
    fPDGMass(0.),
    fDistPt(NULL)
{
  SetDistributionPt();
  SetDistributionY();
  SetRangePt();
}
// ------------------------------------------------------------------------
Bool_t CbmAnaHypYPtGenerator::Init(){
  // Initialize generator
  // Check for particle type
  TDatabasePDG* pdgBase = TDatabasePDG::Instance();
  TParticlePDG *particle = pdgBase->GetParticle(fPDGType);
  if (! particle) Fatal("CbmAnaHypYPtGenerator","PDG code %d not defined.",fPDGType);
  fPDGMass = particle->Mass();
  //gRandom->SetSeed(0);
  fDistPt = new TF1("distPt","x*exp(-sqrt(x*x+[1]*[1])/[0])",fPtMin,fPtMax);
  fDistPt->SetParameters(fT,fPDGMass,fY0,fSigma);
  Info("Init","pdg=%i y0=%4.2f sigma_y=%4.2f T_pt=%6.4f",fPDGType,fY0,fSigma,fT);
  return 0; 
}

// ------------------------------------------------------------------------
Bool_t CbmAnaHypYPtGenerator::ReadEvent(FairPrimaryGenerator* primGen){
  Double_t phi, pt, y, mt, px, py, pz;

  // Generate particles
  for (Int_t k = 0; k < fMult; k++) {

    phi = gRandom->Uniform(0,TMath::TwoPi());
    pt  = fDistPt->GetRandom(fPtMin,fPtMax);
    px  = pt*TMath::Cos(phi);
    py  = pt*TMath::Sin(phi);
    y   = gRandom->Gaus(fY0,fSigma);
    mt  = TMath::Sqrt(fPDGMass*fPDGMass + pt*pt);
    pz  = mt * TMath::SinH(y);
    Info("ReadEvent","Particle generated: pdg=%i pt=%f y=%f\n",fPDGType,pt,y);
    primGen->AddTrack(fPDGType, px, py, pz, 0, 0, 0);
  }
  return kTRUE;

}
// ------------------------------------------------------------------------

ClassImp(CbmAnaHypYPtGenerator)

