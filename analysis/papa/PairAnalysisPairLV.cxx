///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  PairAnalysis Pair class. Internally it makes use of TLorentzVector.    //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


#include <TDatabasePDG.h>

#include "CbmVertex.h"
#include "CbmMCTrack.h"
#include "PairAnalysisTrack.h"

#include "PairAnalysisPairLV.h"

ClassImp(PairAnalysisPairLV)

PairAnalysisPairLV::PairAnalysisPairLV() :
  PairAnalysisPair(),
  fPair(),
  fD1(),
  fD2()
{
  //
  // Default Constructor
  //

}

//______________________________________________
PairAnalysisPairLV::PairAnalysisPairLV(const PairAnalysisPair& pair) :
  PairAnalysisPair(pair),
  fPair(),
  fD1(),
  fD2()
{
  //
  // Copy Constructor
  //
  SetTracks(pair.GetFirstDaughter(), pair.GetFirstDaughterPid(), pair.GetSecondDaughter(), pair.GetSecondDaughterPid());
}

//______________________________________________
PairAnalysisPairLV::PairAnalysisPairLV(PairAnalysisTrack * const particle1, Int_t pid1,
			       PairAnalysisTrack * const particle2, Int_t pid2, Char_t type) :
  PairAnalysisPair(type),
  fPair(),
  fD1(),
  fD2()
{
  //
  // Constructor with tracks
  //
  SetTracks(particle1, pid1, particle2, pid2);
}

//______________________________________________
PairAnalysisPairLV::~PairAnalysisPairLV()
{
  //
  // Default Destructor
  //

}

//______________________________________________
void PairAnalysisPairLV::SetTracks(PairAnalysisTrack * const particle1, Int_t pid1,
			       PairAnalysisTrack * const particle2, Int_t pid2)
{
  //
  // Sort particles by pt, first particle larget Pt
  // set TLorentzVector daughters and pair
  // refParticle1 and 2 are the original tracks. In the case of track rotation
  // they are needed in the framework
  //
  // TODO: think about moving the pid assignement to PairAnalysisTrack and use it here
  // BUT think about mixed events or LS-pairs
  const Double_t mpid1 = TDatabasePDG::Instance()->GetParticle(pid1)->Mass();
  const Double_t mpid2 = TDatabasePDG::Instance()->GetParticle(pid2)->Mass();
  const Double_t cpid1 = TDatabasePDG::Instance()->GetParticle(pid1)->Charge()*3;
  const Double_t cpid2 = TDatabasePDG::Instance()->GetParticle(pid2)->Charge()*3;

  // match charge of track to pid and set mass accordingly
  fPid1  = pid1;
  fPid2  = pid2;
  Double_t m1 = mpid1;
  Double_t m2 = mpid2;
  if(particle1->Charge() == cpid2) { m1=mpid2; fPid1=pid2; } //TODO: what about 2e-charges
  if(particle2->Charge() == cpid1) { m2=mpid1; fPid2=pid1; }

  // Calculate Energy per particle by hand
  Double_t e1 = TMath::Sqrt(m1*m1                           +
			    particle1->Px()*particle1->Px() +
			    particle1->Py()*particle1->Py() +
			    particle1->Pz()*particle1->Pz() );

  Double_t e2 = TMath::Sqrt(m2*m2                           +
			    particle2->Px()*particle2->Px() +
			    particle2->Py()*particle2->Py() +
			    particle2->Pz()*particle2->Pz() );

  fRefD1 = particle1;
  fRefD2 = particle2;
  fD1.SetPxPyPzE(particle1->Px(),particle1->Py(),particle1->Pz(),e1);
  fD2.SetPxPyPzE(particle2->Px(),particle2->Py(),particle2->Pz(),e2);
  //^^^ this should become obsolete

  // build pair
  fPair=(fD1+fD2);
  fWeight=TMath::Sqrt(particle1->GetWeight() * particle2->GetWeight() );
  //  printf("fill pair weight: %.1f * %.1f = %.1f \n",particle1->GetWeight(),particle2->GetWeight(),fWeight);
}

//______________________________________________
void PairAnalysisPairLV::SetGammaTracks(PairAnalysisTrack * const particle1, Int_t pid1,
				      PairAnalysisTrack * const particle2, Int_t pid2)
{
  //
  // special gamma function not used in LV class using std constructor
  //
  SetTracks(particle1, pid1, particle2, pid2);
}

//______________________________________________
void PairAnalysisPairLV::SetMCTracks(const CbmMCTrack * const particle1, const CbmMCTrack * const particle2)
{
  //
  // build MC pair from TLorentzVector daughters
  // no references are set
  //
  particle1->Get4Momentum(fD1);
  particle2->Get4Momentum(fD2);
  fPair=(fD1+fD2);
}

//______________________________________________
Short_t PairAnalysisPairLV::Charge() const
{
  return (dynamic_cast<PairAnalysisTrack*>(fRefD1.GetObject())->Charge() +
	  dynamic_cast<PairAnalysisTrack*>(fRefD2.GetObject())->Charge());
}

//______________________________________________
void PairAnalysisPairLV::GetThetaPhiCM(Double_t &thetaHE, Double_t &phiHE, Double_t &thetaCS, Double_t &phiCS) const
{
  //
  // Calculate theta and phi in helicity and Collins-Soper coordinate frame
  //
  const Double_t proMass = TDatabasePDG::Instance()->GetParticle(2212)->Mass();
  TLorentzVector projMom(0.,0.,-fBeamEnergy,TMath::Sqrt(fBeamEnergy*fBeamEnergy+proMass*proMass));
  TLorentzVector targMom(0.,0., fBeamEnergy,TMath::Sqrt(fBeamEnergy*fBeamEnergy+proMass*proMass));

  // boost all the 4-mom vectors to the mother rest frame
  TVector3 beta = (-1.0/fPair.E())*fPair.Vect();
  TLorentzVector p1(fD1);
  TLorentzVector p2(fD2);
  p1.Boost(beta);
  p2.Boost(beta);
  projMom.Boost(beta);
  targMom.Boost(beta);

  // x,y,z axes
  TVector3 zAxisHE = (fPair.Vect()).Unit();
  TVector3 zAxisCS = ((projMom.Vect()).Unit()-(targMom.Vect()).Unit()).Unit();
  TVector3 yAxis   = ((projMom.Vect()).Cross(targMom.Vect())).Unit();
  TVector3 xAxisHE = (yAxis.Cross(zAxisHE)).Unit();
  TVector3 xAxisCS = (yAxis.Cross(zAxisCS)).Unit();

  // fill theta and phi
  if(dynamic_cast<PairAnalysisTrack*>(fRefD1.GetObject())->Charge()>0){
    thetaHE = zAxisHE.Dot((p1.Vect()).Unit());
    thetaCS = zAxisCS.Dot((p1.Vect()).Unit());
    phiHE   = TMath::ATan2((p1.Vect()).Dot(yAxis), (p1.Vect()).Dot(xAxisHE));
    phiCS   = TMath::ATan2((p1.Vect()).Dot(yAxis), (p1.Vect()).Dot(xAxisCS));
  } else {
    thetaHE = zAxisHE.Dot((p2.Vect()).Unit());
    thetaCS = zAxisCS.Dot((p2.Vect()).Unit());
    phiHE   = TMath::ATan2((p2.Vect()).Dot(yAxis), (p2.Vect()).Dot(xAxisHE));
    phiCS   = TMath::ATan2((p2.Vect()).Dot(yAxis), (p2.Vect()).Dot(xAxisCS));
  }
}


//______________________________________________
Double_t PairAnalysisPairLV::PsiPair(Double_t MagField) const
{
  //Following idea to use opening of colinear pairs in magnetic field from e.g. PHENIX
  //to ID conversions. Adapted from AliTRDv0Info class
  //TODO: adapt and get magnetic field
  Double_t x, y;//, z;
  x = fPair.X();
  y = fPair.Y();
  //  z = fPair.Z();

  Double_t m1[3] = {0,0,0};
  Double_t m2[3] = {0,0,0};

  m1[0] = fD1.Px();
  m1[1] = fD1.Py();
  m1[2] = fD1.Pz();

  m2[0] = fD2.Px();
  m2[1] = fD2.Py();
  m2[2] = fD2.Pz();

  Double_t deltat = 1.;
  //difference of angles of the two daughter tracks with z-axis
  deltat = TMath::ATan(m2[2]/(TMath::Sqrt(m2[0]*m2[0] + m2[1]*m2[1])+1.e-13))-
    TMath::ATan(m1[2]/(TMath::Sqrt(m1[0]*m1[0] + m1[1]*m1[1])+1.e-13));

  Double_t radiussum = TMath::Sqrt(x*x + y*y) + 50;//radius to which tracks shall be propagated

  Double_t mom1Prop[3]={0.,0.,0.};
  Double_t mom2Prop[3]={0.,0.,0.};

  // TODO: adapt code
  Double_t fPsiPair = 4.;
  /*
  AliExternalTrackParam *d1 = static_cast<AliExternalTrackParam*>(fRefD1.GetObject());
  AliExternalTrackParam *d2 = static_cast<AliExternalTrackParam*>(fRefD2.GetObject());
  AliExternalTrackParam nt(*d1), pt(*d2);

  if(nt.PropagateTo(radiussum,MagField) == 0)//propagate tracks to the outside
	fPsiPair =  -5.;
  if(pt.PropagateTo(radiussum,MagField) == 0)
	fPsiPair = -5.;
  pt.GetPxPyPz(mom1Prop);//Get momentum vectors of tracks after propagation
  nt.GetPxPyPz(mom2Prop);
  */

  // absolute momentum values
  Double_t pEle =
	TMath::Sqrt(mom2Prop[0]*mom2Prop[0]+mom2Prop[1]*mom2Prop[1]+mom2Prop[2]*mom2Prop[2]);
  Double_t pPos =
	TMath::Sqrt(mom1Prop[0]*mom1Prop[0]+mom1Prop[1]*mom1Prop[1]+mom1Prop[2]*mom1Prop[2]);
  //scalar product of propagated posit
  Double_t scalarproduct =
	mom1Prop[0]*mom2Prop[0]+mom1Prop[1]*mom2Prop[1]+mom1Prop[2]*mom2Prop[2];
  //Angle between propagated daughter tracks
  Double_t chipair = TMath::ACos(scalarproduct/(pEle*pPos));

  fPsiPair =  TMath::Abs(TMath::ASin(deltat/chipair));

  return fPsiPair;

}

//______________________________________________
Double_t PairAnalysisPairLV::ThetaPhiCM(Bool_t isHE, Bool_t isTheta) const {
  // The function calculates theta and phi in the mother rest frame with 
  // respect to the helicity coordinate system and Collins-Soper coordinate system
  // TODO: generalize for different decays (only J/Psi->e+e- now) still true??

  // Laboratory frame 4-vectors:
  const Double_t proMass = TDatabasePDG::Instance()->GetParticle(2212)->Mass();
  TLorentzVector projMom(0.,0.,-fBeamEnergy,TMath::Sqrt(fBeamEnergy*fBeamEnergy+proMass*proMass));
  TLorentzVector targMom(0.,0., fBeamEnergy,TMath::Sqrt(fBeamEnergy*fBeamEnergy+proMass*proMass));

  // boost all the 4-mom vectors to the mother rest frame
  TVector3 beta = (-1.0/fPair.E())*fPair.Vect();
  TLorentzVector p1(fD1);
  TLorentzVector p2(fD2);
  p1.Boost(beta);
  p2.Boost(beta);
  projMom.Boost(beta);
  targMom.Boost(beta);

  // x,y,z axes
  TVector3 zAxis;
  if(isHE) zAxis = (fPair.Vect()).Unit();
  else     zAxis = ((projMom.Vect()).Unit()-(targMom.Vect()).Unit()).Unit();
  TVector3 yAxis = ((projMom.Vect()).Cross(targMom.Vect())).Unit();
  TVector3 xAxis = (yAxis.Cross(zAxis)).Unit();

  // return either theta or phi
  if(isTheta) {
    if(dynamic_cast<PairAnalysisTrack*>(fRefD1.GetObject())->Charge()>0)
      return zAxis.Dot((p1.Vect()).Unit());
    else
      return zAxis.Dot((p2.Vect()).Unit());
  }
  else {
    if(dynamic_cast<PairAnalysisTrack*>(fRefD1.GetObject())->Charge()>0)
      return TMath::ATan2((p1.Vect()).Dot(yAxis), (p1.Vect()).Dot(xAxis));
    else
      return TMath::ATan2((p2.Vect()).Dot(yAxis), (p2.Vect()).Dot(xAxis));
  }
}
//______________________________________________
Double_t PairAnalysisPairLV::GetCosPointingAngle(const CbmVertex *primVtx) const
{
  //
  // Calculate the poiting angle of the pair to the primary vertex and take the cosine
  //
  if(!primVtx) return -1.;

  Double_t deltaPos[3]; //vector between the reference point and the V0 vertex
  deltaPos[0] = fPair.X() - primVtx->GetX();
  deltaPos[1] = fPair.Y() - primVtx->GetY();
  deltaPos[2] = fPair.Z() - primVtx->GetZ();

  Double_t momV02    = Px()*Px() + Py()*Py() + Pz()*Pz();
  Double_t deltaPos2 = deltaPos[0]*deltaPos[0] + deltaPos[1]*deltaPos[1] + deltaPos[2]*deltaPos[2];

  Double_t cosinePointingAngle = (deltaPos[0]*Px() + deltaPos[1]*Py() + deltaPos[2]*Pz()) / TMath::Sqrt(momV02 * deltaPos2);

  return TMath::Abs(cosinePointingAngle);

}

//______________________________________________
Double_t PairAnalysisPairLV::GetArmAlpha() const
{
  //
  // Calculate the Armenteros-Podolanski Alpha
  //
  Int_t qD1 = dynamic_cast<PairAnalysisTrack*>(fRefD1.GetObject())->Charge()>0;
  TVector3 momNeg = (qD1<0? fD1.Vect() : fD2.Vect());
  TVector3 momPos = (qD1<0? fD2.Vect() : fD1.Vect());
  TVector3 momTot(Px(),Py(),Pz());

  Double_t lQlNeg = momNeg.Dot(momTot)/momTot.Mag();
  Double_t lQlPos = momPos.Dot(momTot)/momTot.Mag();

  return ((lQlPos - lQlNeg)/(lQlPos + lQlNeg));
}

//______________________________________________
Double_t PairAnalysisPairLV::GetArmPt() const
{
  //
  // Calculate the Armenteros-Podolanski Pt
  //
  Int_t qD1 = dynamic_cast<PairAnalysisTrack*>(fRefD1.GetObject())->Charge()>0;
  TVector3 momNeg = (qD1<0? fD1.Vect() : fD2.Vect());
  TVector3 momTot(Px(),Py(),Pz());
  return (momNeg.Perp(momTot));
}

//______________________________________________
Double_t PairAnalysisPairLV::PhivPair(Double_t MagField) const
{
  //Following idea to use opening of colinear pairs in magnetic field from e.g. PHENIX
  //to ID conversions. Angle between ee plane and magnetic field is calculated.

  Int_t qD1 = dynamic_cast<PairAnalysisTrack*>(fRefD1.GetObject())->Charge()>0;
  TVector3 p1;
  TVector3 p2;
  if(MagField>0){
    p1 = (qD1>0? fD1.Vect() : fD2.Vect());
    p2 = (qD1>0? fD2.Vect() : fD1.Vect());
  }  else {
    p2 = (qD1>0? fD1.Vect() : fD2.Vect());
    p1 = (qD1>0? fD2.Vect() : fD1.Vect());
  }

  //unit vector of (pep+pem)
  TVector3 u = fPair.Vect();
  u.SetMag(1.); // normalize
  Double_t ax =  u.Py()/u.Perp();
  Double_t ay = -u.Px()/u.Perp();

  //vector product of pep X pem
  TVector3 vpm  = p1.Cross(p2);
  vpm.SetMag(1.); // normalize

  //The third axis defined by vector product (ux,uy,uz)X(vx,vy,vz)
  TVector3 w  = u.Cross(vpm);
  // by construction, (wx,wy,wz) must be a unit vector. 
  // measure angle between (wx,wy,wz) and (ax,ay,0). The angle between them 
  // should be small if the pair is conversion 
  //
  Double_t cosPhiV = w.Px()*ax + w.Py()*ay;
  Double_t phiv = TMath::ACos(cosPhiV);

  return phiv;

}

//______________________________________________
Double_t PairAnalysisPairLV::GetPairPlaneAngle(Double_t v0rpH2, Int_t VariNum)const
{

  // Calculate the angle between electron pair plane and variables
  // kv0rpH2 is reaction plane angle using V0-A,C,AC,Random

  Double_t px1=-9999.,py1=-9999.,pz1=-9999.;
  Double_t px2=-9999.,py2=-9999.,pz2=-9999.;

  px1 = fD1.Px();
  py1 = fD1.Py();
  pz1 = fD1.Pz();

  px2 = fD2.Px();
  py2 = fD2.Py();
  pz2 = fD2.Pz();

  //p1+p2
  Double_t px = Px();
  Double_t py = Py();
  Double_t pz = Pz();

  // normal vector of ee plane
  Double_t pnorx = py1*pz2 - pz1*py2;
  Double_t pnory = pz1*px2 - px1*pz2;
  Double_t pnorz = px1*py2 - py1*px2;
  Double_t pnor  = TMath::Sqrt( pnorx*pnorx + pnory*pnory + pnorz*pnorz );

  //unit vector
  Double_t upnx = -9999.;
  Double_t upny = -9999.;
  Double_t upnz = -9999.;

  if (pnor !=0)
    {
      upnx= pnorx/pnor;
      upny= pnory/pnor;
      upnz= pnorz/pnor;
    }


  Double_t ax = -9999.;
  Double_t ay = -9999.;
  Double_t az = -9999.;

  //variable 1
  //seeing the angle between ee decay plane and reaction plane by using V0-A,C,AC,Random
  if(VariNum == 1){
    ax = TMath::Sin(v0rpH2);
    ay = -TMath::Cos(v0rpH2);
    az = 0.0;
  }


  //variable 2
  //seeing the angle between ee decay plane and (p1+p2) rot ez
  else if (VariNum == 2 ){
    ax = py;
    ay = -px;
    az = 0.0;
  }

  //variable 3
  //seeing the angle between ee decay plane and (p1+p2) rot (p1+p2)x'z
  else if (VariNum == 3 ){
    Double_t rotpx = px*TMath::Cos(v0rpH2)+py*TMath::Sin(v0rpH2);
    //Double_t rotpy = 0.0;
    // Double_t rotpz = pz;

    ax = py*pz;
    ay = pz*rotpx-pz*px;
    az = -rotpx*py;
  }

  //variable 4
  //seeing the angle between ee decay plane and (p1+p2) rot ey'
  else if (VariNum == 4){
    ax = 0.0;
    ay = 0.0;
    az = pz;
  }

  Double_t denomHelper = ax*ax + ay*ay +az*az;
  Double_t uax = -9999.;
  Double_t uay = -9999.;
  Double_t uaz = -9999.;

  if (denomHelper !=0) {
    uax = ax/TMath::Sqrt(denomHelper);
    uay = ay/TMath::Sqrt(denomHelper);
    uaz = az/TMath::Sqrt(denomHelper);
  }

  //PM is the angle between Pair plane and a plane decided by using variable 1-4

  Double_t cosPM = upnx*uax + upny*uay + upnz*uaz;
  Double_t PM = TMath::ACos(cosPM);

  //keep interval [0,pi/2]
  if(PM > TMath::Pi()/2){
    PM -= TMath::Pi();
    PM *= -1.0;
  }
  return PM;
}


//_______________________________________________
Double_t PairAnalysisPairLV::PairPlaneMagInnerProduct(Double_t ZDCrpH1) const
{
  //
  // Calculate inner product of the strong magnetic field and electron pair plane
  //
  if(ZDCrpH1 == 0.) return -9999.;
  //daughter vectors
  TVector3 p1 = fD1.Vect();
  TVector3 p2 = fD2.Vect();
  //vector product of pep X pem
  TVector3 vpm  = p2.Cross(p1);
  vpm.SetMag(1.); // normalize
  //direction of strong magnetic field
  Double_t magx = TMath::Cos(ZDCrpH1+(TMath::Pi()/2));
  Double_t magy = TMath::Sin(ZDCrpH1+(TMath::Pi()/2));
  //inner product of strong magnetic field and  ee plane
  Double_t upnmag = vpm.Px()*magx + vpm.Py()*magy;
  return upnmag;
}

//_______________________________________________
void PairAnalysisPairLV::RotateTrack(PairAnalysisTrackRotator *rot)
{
  //
  // Rotate one of the legs according to the track rotator settings
  //

  Double_t rotAngle  = rot->GetAngle();
  Double_t rotCharge = rot->GetCharge();

  PairAnalysisTrack* first  =  GetFirstDaughter();
  if(!first) return;

  //Printf("\n before rotation:");
  //fD1.Print("");
  //fD2.Print("");

  if( rot->GetRotationType()==PairAnalysisTrackRotator::kRotatePositive ||
      (rot->GetRotationType()==PairAnalysisTrackRotator::kRotateBothRandom&&rotCharge==0) ) {
    if  (first->Charge()>0) fD1.RotateZ(rotAngle);
    else                    fD2.RotateZ(rotAngle);
  }

  if ( rot->GetRotationType()==PairAnalysisTrackRotator::kRotateNegative ||
       (rot->GetRotationType()==PairAnalysisTrackRotator::kRotateBothRandom&&rotCharge==1) ) {
    if  (first->Charge()>0) fD1.RotateZ(rotAngle);
    else                    fD2.RotateZ(rotAngle);
  }
  //  Printf("after rotation:");
  //fD1.Print("");
  //fD2.Print("");

  // rebuild pair
  fPair=(fD1+fD2);

}
