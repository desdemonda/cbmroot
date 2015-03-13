// -------------------------------------------------------------------------
// -----         CbmLitPolarizedGenerator source file                     -----
// -----          Created 11/09/09  by E. Kryshen                      -----
// -------------------------------------------------------------------------

#include "TRandom.h"
#include "CbmLitPolarizedGenerator.h"
#include "FairPrimaryGenerator.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TF1.h"
#include "TMath.h"
#include "TLorentzVector.h"

// ------------------------------------------------------------------------
CbmLitPolarizedGenerator::CbmLitPolarizedGenerator():FairGenerator(){
  // Default constructor
  fPDGType =  -1;
  fMult    =   0;
  fAlpha   =   0;
  fBeamMomentum = 25.;
  fFrame = kHelicity;
  fDecayMode = kDiMuon;
  fPol = NULL;
  fBox = 0;
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
CbmLitPolarizedGenerator::CbmLitPolarizedGenerator(Int_t pdgid, Int_t mult)
  :FairGenerator(){
  // Constructor. Set default distributions
  fPDGType = pdgid;
  fMult    = mult;
  fAlpha   = 0;
  fBeamMomentum = 25.;
  fFrame = kHelicity;
  fDecayMode = kDiMuon;
  fPol = NULL;
  fBox = 0;
  SetDistributionPt();
  SetDistributionY();
  SetRangePt();
  SetRangeY();
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
Bool_t CbmLitPolarizedGenerator::Init(){
  // Initialize generator
  // Check for particle type
  TDatabasePDG* pdgBase = TDatabasePDG::Instance();
  TParticlePDG *particle = pdgBase->GetParticle(fPDGType);
  if (! particle) Fatal("CbmLitPolarizedGenerator","PDG code %d not defined.",fPDGType);
  fPDGMass = particle->Mass();
  if (fPtDistMass<0) fPtDistMass=fPDGMass;
  //gRandom->SetSeed(0);
  fDistPt = new TF1("distPt","x*exp(-sqrt(x*x+[1]*[1])/[0])",fPtMin,fPtMax);
  fDistPt->SetParameters(fT,fPtDistMass);
  fPol = new TF1("dsigdcostheta","1.+[0]*x*x",-1.,1.);
  fPol->SetParameter(0,fAlpha);
  Info("Init","pdg=%i y0=%4.2f sigma_y=%4.2f T_pt=%6.4f",fPDGType,fY0,fSigma,fT);
  return 0;
}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
Bool_t CbmLitPolarizedGenerator::ReadEvent(FairPrimaryGenerator* primGen){
  Double_t phi, pt, y, mt, px, py, pz;

  // Generate particles
  for (Int_t k = 0; k < fMult; k++) {

    phi = gRandom->Uniform(0,TMath::TwoPi());
    if (fBox) pt = gRandom->Uniform(fPtMin,fPtMax);
    else pt  = fDistPt->GetRandom(fPtMin,fPtMax);
    px  = pt*TMath::Cos(phi);
    py  = pt*TMath::Sin(phi);
    if (fBox) y =  gRandom->Uniform(fYMin,fYMax);
    else y   = gRandom->Gaus(fY0,fSigma);
    mt  = TMath::Sqrt(fPDGMass*fPDGMass + pt*pt);
    pz  = mt * TMath::SinH(y);
//    Info("ReadEvent","Particle generated: pdg=%i pt=%f y=%f",fPDGType,pt,y);
//    primGen->AddTrack(fPDGType, px, py, pz, 0, 0, 0);
    GenerateDaughters(TVector3(px,py,pz),primGen);
  }
  return kTRUE;

}
// ------------------------------------------------------------------------


// ------------------------------------------------------------------------
Bool_t CbmLitPolarizedGenerator::GenerateDaughters(
    TVector3 pMother, FairPrimaryGenerator* primGen)
{
  
  TParticlePDG* part; 
  if     (fDecayMode==kDiMuon)     part = TDatabasePDG::Instance()->GetParticle("mu+"); 
  else if(fDecayMode==kDiElectron) part = TDatabasePDG::Instance()->GetParticle("e+");
  else Fatal("GenerateDaughters","Polarized dilepton decay only implemented\n");
  
  // energies and momenta in dilepton rest frame
  Double_t m = part->Mass();
  Double_t e = fPDGMass/2.;
  Double_t p = TMath::Sqrt(e*e-m*m);

  Double_t cost = fPol->GetRandom();
  Double_t sint = TMath::Sqrt(1.-cost*cost);
  Double_t phi = TMath::TwoPi()*gRandom->Rndm();
  Double_t px = p*sint*TMath::Cos(phi);
  Double_t py = p*sint*TMath::Sin(phi);
  Double_t pz = p*cost;

  TLorentzVector v1,v2, boosted1, boosted2;
  v1.SetPxPyPzE(-px,-py,-pz,e);
  v2.SetPxPyPzE(+px,+py,+pz,e);

  // Define boost-vector
  TLorentzVector v;
  v.SetVectM(pMother,fPDGMass);
  TVector3 boost = v.BoostVector();
  
  // Define z-axis
  TVector3 zaxis; 
  if(fFrame == kHelicity) {
    //  polarization axis: direction of meson in the delipton rest frame
    zaxis = pMother.Unit();
  } else if (fFrame == kColSop){
    //  polarization axis: bisector of proj and target in the dilepton rest frame
    Double_t mp = 0.938;
    Double_t ep = TMath::Sqrt(fBeamMomentum*fBeamMomentum+mp*mp);
    TLorentzVector proj = TLorentzVector(0.,0.,fBeamMomentum,ep); // projectile
    TLorentzVector targ = TLorentzVector(0.,0.,0.           ,mp); // target
    proj.Boost(-boost);   //boost proj and targ from lab to dilepton rest frame
    targ.Boost(-boost);
    zaxis=(proj.Vect().Unit()-targ.Vect().Unit()).Unit();
  } else {
    zaxis = TVector3(0.,0.,1.);
  }
  v1.RotateUz(zaxis); // rotate lepton vectors with respect to polarization axis
  v2.RotateUz(zaxis);

  v1.Boost(boost);  //boost leptons from dilepton rest frame to lab frame
  v2.Boost(boost);

  Int_t pdg = part->PdgCode();
  Info("ReadEvent","Particle generated: pdg=%3i pt=%7.4f y=%7.4f", pdg,v1.Pt(),v1.Rapidity());
  Info("ReadEvent","Particle generated: pdg=%3i pt=%7.4f y=%7.4f",-pdg,v2.Pt(),v2.Rapidity());
  primGen->AddTrack( pdg,v1[0],v1[1],v1[2], 0, 0, 0);
  primGen->AddTrack(-pdg,v2[0],v2[1],v2[2], 0, 0, 0);

  return kTRUE;
}
// ------------------------------------------------------------------------


ClassImp(CbmLitPolarizedGenerator)

