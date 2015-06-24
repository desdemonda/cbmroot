///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  PairAnalysis Pair Interface class.                                     //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


#include <TDatabasePDG.h>

#include "CbmVertex.h"

#include "PairAnalysisTrack.h"
#include "PairAnalysisPair.h"

ClassImp(PairAnalysisPair)

Double_t PairAnalysisPair::fBeamEnergy=-1.;

PairAnalysisPair::PairAnalysisPair() :
  fType(-1),
  fLabel(-1),
  fWeight(1.),
  fPdgCode(0),
  fRefD1(),
  fRefD2(),
  fPid1(0),
  fPid2(0),
  fKFUsage(kTRUE)
{
  //
  // Default Constructor
  //

}

//______________________________________________
PairAnalysisPair::PairAnalysisPair(const PairAnalysisPair& pair) :
  fType(pair.GetType()),
  fLabel(pair.GetLabel()),
  fWeight(pair.GetWeight()),
  fPdgCode(pair.PdgCode()),
  fRefD1(pair.GetFirstDaughter()),
  fRefD2(pair.GetSecondDaughter()),
  fPid1(pair.GetFirstDaughterPid()),
  fPid2(pair.GetSecondDaughterPid()),
  fKFUsage(pair.GetKFUsage())
{
  //
  // Copy Constructor
  //

}

//______________________________________________
PairAnalysisPair::PairAnalysisPair(Char_t type) :
  fType(type),
  fLabel(-1),
  fWeight(1.),
  fPdgCode(0),
  fRefD1(),
  fRefD2(),
  fPid1(0),
  fPid2(0),
  fKFUsage(kTRUE)
{
  //
  // Constructor with type
  //

}

//______________________________________________
PairAnalysisPair::~PairAnalysisPair()
{
  //
  // Default Destructor
  //

}


//______________________________________________
Double_t PairAnalysisPair::ThetaPhiCM(const PairAnalysisTrack* d1, const PairAnalysisTrack* d2, 
                                       Bool_t isHE, Bool_t isTheta)
{
  // The function calculates theta and phi in the mother rest frame with
  // respect to the helicity coordinate system and Collins-Soper coordinate system
  // TO DO: generalize for different decays (only J/Psi->e+e- now)

  // Laboratory frame 4-vectors:
  // projectile beam & target beam 4-mom
  Double_t px1=d1->Px();
  Double_t py1=d1->Py();
  Double_t pz1=d1->Pz();
  Double_t px2=d2->Px();
  Double_t py2=d2->Py();
  Double_t pz2=d2->Pz();
  const Double_t eleMass = TDatabasePDG::Instance()->GetParticle(11)->Mass();
  const Double_t proMass = TDatabasePDG::Instance()->GetParticle(2212)->Mass();
//   printf(" beam energy %f \n ", fBeamEnergy);
  TLorentzVector projMom(0.,0.,-fBeamEnergy,TMath::Sqrt(fBeamEnergy*fBeamEnergy+proMass*proMass));
  TLorentzVector targMom(0.,0., fBeamEnergy,TMath::Sqrt(fBeamEnergy*fBeamEnergy+proMass*proMass));
  
  // first & second daughter 4-mom
  TLorentzVector p1Mom(px1,py1,pz1,TMath::Sqrt(px1*px1+py1*py1+pz1*pz1+eleMass*eleMass));
  TLorentzVector p2Mom(px2,py2,pz2,TMath::Sqrt(px2*px2+py2*py2+pz2*pz2+eleMass*eleMass));
  // J/Psi 4-momentum vector
  TLorentzVector motherMom=p1Mom+p2Mom;
  
  // boost all the 4-mom vectors to the mother rest frame
  TVector3 beta = (-1.0/motherMom.E())*motherMom.Vect();
  p1Mom.Boost(beta);
  p2Mom.Boost(beta);
  projMom.Boost(beta);
  targMom.Boost(beta);
  
  // x,y,z axes 
  TVector3 zAxis;
  if(isHE) zAxis = (motherMom.Vect()).Unit();
  else zAxis = ((projMom.Vect()).Unit()-(targMom.Vect()).Unit()).Unit();
  TVector3 yAxis = ((projMom.Vect()).Cross(targMom.Vect())).Unit();
  TVector3 xAxis = (yAxis.Cross(zAxis)).Unit();
  
  // return either theta or phi
  if(isTheta) {
    if(d1->Charge()>0)
      return zAxis.Dot((p1Mom.Vect()).Unit());
    else 
      return zAxis.Dot((p2Mom.Vect()).Unit());

  }
  else {
    if(d1->Charge()>0)
      return TMath::ATan2((p1Mom.Vect()).Dot(yAxis), (p1Mom.Vect()).Dot(xAxis));
    else
      return TMath::ATan2((p2Mom.Vect()).Dot(yAxis), (p2Mom.Vect()).Dot(xAxis));
  }
}

//______________________________________________
Double_t PairAnalysisPair::ThetaPhiCM(Bool_t isHE, Bool_t isTheta) const {
  // The function calculates theta and phi in the mother rest frame with 
  // respect to the helicity coordinate system and Collins-Soper coordinate system
  // TO DO: generalize for different decays (only J/Psi->e+e- now)

  // Laboratory frame 4-vectors:
  // projectile beam & target beam 4-mom
  PairAnalysisTrack *d1 = static_cast<PairAnalysisTrack*>(fRefD1.GetObject());
  PairAnalysisTrack *d2 = static_cast<PairAnalysisTrack*>(fRefD2.GetObject());
  
  Double_t px1=d1->Px();
  Double_t py1=d1->Py();
  Double_t pz1=d1->Pz();
  Double_t px2=d2->Px();
  Double_t py2=d2->Py();
  Double_t pz2=d2->Pz();
  const Double_t eleMass = TDatabasePDG::Instance()->GetParticle(11)->Mass();
  const Double_t proMass = TDatabasePDG::Instance()->GetParticle(2212)->Mass();
  
  TLorentzVector projMom(0.,0.,-fBeamEnergy,TMath::Sqrt(fBeamEnergy*fBeamEnergy+proMass*proMass));
  TLorentzVector targMom(0.,0., fBeamEnergy,TMath::Sqrt(fBeamEnergy*fBeamEnergy+proMass*proMass));
  
  // first & second daughter 4-mom
  TLorentzVector p1Mom(px1,py1,pz1,TMath::Sqrt(px1*px1+py1*py1+pz1*pz1+eleMass*eleMass));
  TLorentzVector p2Mom(px2,py2,pz2,TMath::Sqrt(px2*px2+py2*py2+pz2*pz2+eleMass*eleMass));
  // J/Psi 4-momentum vector
  TLorentzVector motherMom=p1Mom+p2Mom;

  // boost all the 4-mom vectors to the mother rest frame
  TVector3 beta = (-1.0/motherMom.E())*motherMom.Vect();
  p1Mom.Boost(beta);
  p2Mom.Boost(beta);
  projMom.Boost(beta);
  targMom.Boost(beta);

  // x,y,z axes 
  TVector3 zAxis;
  if(isHE) zAxis = (motherMom.Vect()).Unit();
  else zAxis = ((projMom.Vect()).Unit()-(targMom.Vect()).Unit()).Unit();
  TVector3 yAxis = ((projMom.Vect()).Cross(targMom.Vect())).Unit();
  TVector3 xAxis = (yAxis.Cross(zAxis)).Unit();

  // return either theta or phi
  if(isTheta) {
    if(d1->Charge()>0) 
      return zAxis.Dot((p1Mom.Vect()).Unit());
    else
      return zAxis.Dot((p2Mom.Vect()).Unit());
  }
  else {
    if(d1->Charge()>0)
      return TMath::ATan2((p1Mom.Vect()).Dot(yAxis), (p1Mom.Vect()).Dot(xAxis));
    else
      return TMath::ATan2((p2Mom.Vect()).Dot(yAxis), (p2Mom.Vect()).Dot(xAxis));
  }
}

//______________________________________________
void PairAnalysisPair::GetDCA(const CbmVertex *primVtx, Double_t d0z0[2]) const
{
  //
  // Calculate the dca of the mother with respect to the primary vertex
  //
  if(!primVtx) return;

  d0z0[0] = TMath::Sqrt(TMath::Power(Xv()-primVtx->GetX(),2) +
			TMath::Power(Yv()-primVtx->GetY(),2) );

  d0z0[1] = Zv() - primVtx->GetZ();
  return;

}

//______________________________________________
void PairAnalysisPair::SetBeamEnergy(Double_t beamEbyHand)
{
  //
  // set the beam energy by hand
  //
  // if(ev->IsA()==AliESDEvent::Class())
  //   fBeamEnergy = ((AliESDEvent*)ev)->GetBeamEnergy();
  // else
  fBeamEnergy = beamEbyHand;
}


