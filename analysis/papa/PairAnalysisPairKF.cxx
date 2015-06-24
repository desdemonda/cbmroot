/*************************************************************************
* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  PairAnalysis Pair class. Internally it makes use of AliKFParticle.     //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


#include <TDatabasePDG.h>
#include <AliVTrack.h>
#include <AliVVertex.h>
#include <AliExternalTrackParam.h>

#include "PairAnalysisPairKF.h"

ClassImp(PairAnalysisPairKF)

PairAnalysisPairKF::PairAnalysisPairKF() :
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
PairAnalysisPairKF::PairAnalysisPairKF(const PairAnalysisPair& pair) :
  PairAnalysisPair(pair),
  fPair(),
  fD1(),
  fD2()
{
  //
  // Copy Constructor, TOCHECK
  //
  SetTracks(pair.GetFirstDaughter(), pair.GetFirstDaughterPid(), pair.GetSecondDaughter(), pair.GetSecondDaughterPid());
}

//______________________________________________
PairAnalysisPairKF::PairAnalysisPairKF(AliVTrack * const particle1, Int_t pid1,
					 AliVTrack * const particle2, Int_t pid2, Char_t type) :
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
PairAnalysisPairKF::PairAnalysisPairKF(const AliKFParticle * const particle1,
                                     const AliKFParticle * const particle2,
                                     AliVTrack * const refParticle1,
                                     AliVTrack * const refParticle2, Char_t type) :
  PairAnalysisPair(type),
  fPair(),
  fD1(),
  fD2()
{
  //
  // Constructor with tracks
  //
  SetTracks(particle1, particle2,refParticle1,refParticle2);
}

//______________________________________________
PairAnalysisPairKF::~PairAnalysisPairKF()
{
  //
  // Default Destructor
  //

}

//______________________________________________
void PairAnalysisPairKF::SetTracks(AliVTrack * const particle1, Int_t pid1,
				    AliVTrack * const particle2, Int_t pid2)
{
  //
  // Sort particles by pt, first particle larget Pt
  // set AliKF daughters and pair
  // refParticle1 and 2 are the original tracks. In the case of track rotation
  // they are needed in the framework
  //
  fPair.Initialize();
  fD1.Initialize();
  fD2.Initialize();

  AliKFParticle kf1(*particle1,pid1);
  AliKFParticle kf2(*particle2,pid2);

  fPair.AddDaughter(kf1);
  fPair.AddDaughter(kf2);

  if (particle1->Pt()>particle2->Pt()){
    fRefD1 = particle1;
    fRefD2 = particle2;
    fD1+=kf1;
    fD2+=kf2;
  } else {
    fRefD1 = particle2;
    fRefD2 = particle1;
    fD1+=kf2;
    fD2+=kf1;
  }
}

//______________________________________________
void PairAnalysisPairKF::SetGammaTracks(AliVTrack * const particle1, Int_t pid1,
					 AliVTrack * const particle2, Int_t pid2)
{
  //
  // Sort particles by pt, first particle larget Pt
  // set AliKF daughters and a GAMMA pair
  // refParticle1 and 2 are the original tracks. In the case of track rotation
  // they are needed in the framework
  //
  fD1.Initialize();
  fD2.Initialize();

  AliKFParticle kf1(*particle1,pid1);
  AliKFParticle kf2(*particle2,pid2);
  fPair.ConstructGamma(kf1,kf2);

  if (particle1->Pt()>particle2->Pt()){
    fRefD1 = particle1;
    fRefD2 = particle2;
    fD1+=kf1;
    fD2+=kf2;
  } else {
    fRefD1 = particle2;
    fRefD2 = particle1;
    fD1+=kf2;
    fD2+=kf1;
  }
}

//______________________________________________
void PairAnalysisPairKF::SetTracks(const AliKFParticle * const particle1,
				    const AliKFParticle * const particle2,
				    AliVTrack * const refParticle1,
				    AliVTrack * const refParticle2)
{
  //
  // Sort particles by pt, first particle larget Pt
  // set AliKF daughters and pair
  // refParticle1 and 2 are the original tracks. In the case of track rotation
  // they are needed in the framework
  //
  fPair.Initialize();
  fD1.Initialize();
  fD2.Initialize();
  
  AliKFParticle kf1(*particle1);
  AliKFParticle kf2(*particle2);
  
  fPair.AddDaughter(kf1);
  fPair.AddDaughter(kf2);
  
  if (kf1.GetPt()>kf2.GetPt()){
    fRefD1 = refParticle1;
    fRefD2 = refParticle2;
    fD1+=kf1;
    fD2+=kf2;
  } else {
    fRefD1 = refParticle2;
    fRefD2 = refParticle1;
    fD1+=kf2;
    fD2+=kf1;
  }
}

//______________________________________________
void PairAnalysisPairKF::GetThetaPhiCM(Double_t &thetaHE, Double_t &phiHE, Double_t &thetaCS, Double_t &phiCS) const
{
  //
  // Calculate theta and phi in helicity and Collins-Soper coordinate frame
  //
  Double_t pxyz1[3]={fD1.GetPx(),fD1.GetPy(),fD1.GetPz()};
  Double_t pxyz2[3]={fD2.GetPx(),fD2.GetPy(),fD2.GetPz()};
  const Double_t eleMass = TDatabasePDG::Instance()->GetParticle(11)->Mass();
  const Double_t proMass = TDatabasePDG::Instance()->GetParticle(2212)->Mass();
  
//   AliVParticle *d1 = static_cast<AliVParticle*>(fRefD1.GetObject());
//   AliVParticle *d2 = static_cast<AliVParticle*>(fRefD2.GetObject());

//   d1->PxPyPz(pxyz1);
//   d2->PxPyPz(pxyz2);
  
  TLorentzVector projMom(0.,0.,-fBeamEnergy,TMath::Sqrt(fBeamEnergy*fBeamEnergy+proMass*proMass));
  TLorentzVector targMom(0.,0., fBeamEnergy,TMath::Sqrt(fBeamEnergy*fBeamEnergy+proMass*proMass));
  
  // first & second daughter 4-mom
  TLorentzVector p1Mom(pxyz1[0],pxyz1[1],pxyz1[2],
                       TMath::Sqrt(pxyz1[0]*pxyz1[0]+pxyz1[1]*pxyz1[1]+pxyz1[2]*pxyz1[2]+eleMass*eleMass));
  TLorentzVector p2Mom(pxyz2[0],pxyz2[1],pxyz2[2],
                       TMath::Sqrt(pxyz2[0]*pxyz2[0]+pxyz2[1]*pxyz2[1]+pxyz2[2]*pxyz2[2]+eleMass*eleMass));
  // J/Psi 4-momentum vector
  TLorentzVector motherMom=p1Mom+p2Mom;
  
  // boost all the 4-mom vectors to the mother rest frame
  TVector3 beta = (-1.0/motherMom.E())*motherMom.Vect();
  p1Mom.Boost(beta);
  p2Mom.Boost(beta);
  projMom.Boost(beta);
  targMom.Boost(beta);

    // x,y,z axes
  TVector3 zAxisHE = (motherMom.Vect()).Unit();
  TVector3 zAxisCS = ((projMom.Vect()).Unit()-(targMom.Vect()).Unit()).Unit();
  TVector3 yAxis = ((projMom.Vect()).Cross(targMom.Vect())).Unit();
  TVector3 xAxisHE = (yAxis.Cross(zAxisHE)).Unit();
  TVector3 xAxisCS = (yAxis.Cross(zAxisCS)).Unit();
  
  // fill theta and phi
  if(fD1.GetQ()>0){
    thetaHE = zAxisHE.Dot((p1Mom.Vect()).Unit());
    thetaCS = zAxisCS.Dot((p1Mom.Vect()).Unit());
    phiHE   = TMath::ATan2((p1Mom.Vect()).Dot(yAxis), (p1Mom.Vect()).Dot(xAxisHE));
    phiCS   = TMath::ATan2((p1Mom.Vect()).Dot(yAxis), (p1Mom.Vect()).Dot(xAxisCS));
  } else {
    thetaHE = zAxisHE.Dot((p2Mom.Vect()).Unit());
    thetaCS = zAxisCS.Dot((p2Mom.Vect()).Unit());
    phiHE   = TMath::ATan2((p2Mom.Vect()).Dot(yAxis), (p2Mom.Vect()).Dot(xAxisHE));
    phiCS   = TMath::ATan2((p2Mom.Vect()).Dot(yAxis), (p2Mom.Vect()).Dot(xAxisCS));
  }
}


//______________________________________________
Double_t PairAnalysisPairKF::PsiPair(Double_t MagField) const
{
  //Following idea to use opening of colinear pairs in magnetic field from e.g. PHENIX
  //to ID conversions. Adapted from AliTRDv0Info class
  Double_t x, y;//, z;
  x = fPair.GetX();
  y = fPair.GetY();
  //  z = fPair.GetZ();

  Double_t m1[3] = {0,0,0};
  Double_t m2[3] = {0,0,0};

  m1[0] = fD1.GetPx();
  m1[1] = fD1.GetPy();
  m1[2] = fD1.GetPz();  

  m2[0] = fD2.GetPx();
  m2[1] = fD2.GetPy();
  m2[2] = fD2.GetPz();

  Double_t deltat = 1.;
  deltat = TMath::ATan(m2[2]/(TMath::Sqrt(m2[0]*m2[0] + m2[1]*m2[1])+1.e-13))-
	TMath::ATan(m1[2]/(TMath::Sqrt(m1[0]*m1[0] + m1[1]*m1[1])+1.e-13));//difference of angles of the two daughter tracks with z-axis

  Double_t radiussum = TMath::Sqrt(x*x + y*y) + 50;//radius to which tracks shall be propagated

  Double_t mom1Prop[3];
  Double_t mom2Prop[3];

  AliExternalTrackParam *d1 = static_cast<AliExternalTrackParam*>(fRefD1.GetObject());
  AliExternalTrackParam *d2 = static_cast<AliExternalTrackParam*>(fRefD2.GetObject());

  AliExternalTrackParam nt(*d1), pt(*d2);

  Double_t fPsiPair = 4.;
  if(nt.PropagateTo(radiussum,MagField) == 0)//propagate tracks to the outside
	fPsiPair =  -5.;
  if(pt.PropagateTo(radiussum,MagField) == 0)
	fPsiPair = -5.;
  pt.GetPxPyPz(mom1Prop);//Get momentum vectors of tracks after propagation
  nt.GetPxPyPz(mom2Prop);



  Double_t pEle =
	TMath::Sqrt(mom2Prop[0]*mom2Prop[0]+mom2Prop[1]*mom2Prop[1]+mom2Prop[2]*mom2Prop[2]);//absolute momentum val
  Double_t pPos =
	TMath::Sqrt(mom1Prop[0]*mom1Prop[0]+mom1Prop[1]*mom1Prop[1]+mom1Prop[2]*mom1Prop[2]);//absolute momentum val

  Double_t scalarproduct =
	mom1Prop[0]*mom2Prop[0]+mom1Prop[1]*mom2Prop[1]+mom1Prop[2]*mom2Prop[2];//scalar product of propagated posit

  Double_t chipair = TMath::ACos(scalarproduct/(pEle*pPos));//Angle between propagated daughter tracks

  fPsiPair =  TMath::Abs(TMath::ASin(deltat/chipair));

  return fPsiPair;

}

//______________________________________________
Double_t PairAnalysisPairKF::GetCosPointingAngle(const AliVVertex *primVtx) const
{
  //
  // Calculate the poiting angle of the pair to the primary vertex and take the cosine
  //
  if(!primVtx) return -1.;

  Double_t deltaPos[3]; //vector between the reference point and the V0 vertex
  deltaPos[0] = fPair.GetX() - primVtx->GetX();
  deltaPos[1] = fPair.GetY() - primVtx->GetY();
  deltaPos[2] = fPair.GetZ() - primVtx->GetZ();

  Double_t momV02    = Px()*Px() + Py()*Py() + Pz()*Pz();
  Double_t deltaPos2 = deltaPos[0]*deltaPos[0] + deltaPos[1]*deltaPos[1] + deltaPos[2]*deltaPos[2];

  Double_t cosinePointingAngle = (deltaPos[0]*Px() + deltaPos[1]*Py() + deltaPos[2]*Pz()) / TMath::Sqrt(momV02 * deltaPos2);
  
  return TMath::Abs(cosinePointingAngle);

}

//______________________________________________
Double_t PairAnalysisPairKF::GetArmAlpha() const
{
  //
  // Calculate the Armenteros-Podolanski Alpha
  //
  Int_t qD1 = fD1.GetQ();

  TVector3 momNeg( (qD1<0?fD1.GetPx():fD2.GetPx()),
		   (qD1<0?fD1.GetPy():fD2.GetPy()),
		   (qD1<0?fD1.GetPz():fD2.GetPz()) );
  TVector3 momPos( (qD1<0?fD2.GetPx():fD1.GetPx()),
		   (qD1<0?fD2.GetPy():fD1.GetPy()),
		   (qD1<0?fD2.GetPz():fD1.GetPz()) );
  TVector3 momTot(Px(),Py(),Pz());

  Double_t lQlNeg = momNeg.Dot(momTot)/momTot.Mag();
  Double_t lQlPos = momPos.Dot(momTot)/momTot.Mag();

  return ((lQlPos - lQlNeg)/(lQlPos + lQlNeg));
}

//______________________________________________
Double_t PairAnalysisPairKF::GetArmPt() const
{
  //
  // Calculate the Armenteros-Podolanski Pt
  //
  Int_t qD1 = fD1.GetQ();

  TVector3 momNeg( (qD1<0?fD1.GetPx():fD2.GetPx()),
		   (qD1<0?fD1.GetPy():fD2.GetPy()),
		   (qD1<0?fD1.GetPz():fD2.GetPz()) );
  TVector3 momTot(Px(),Py(),Pz());

  return (momNeg.Perp(momTot));
}

//______________________________________________
Double_t PairAnalysisPairKF::PhivPair(Double_t MagField) const
{
  //Following idea to use opening of colinear pairs in magnetic field from e.g. PHENIX
  //to ID conversions. Angle between ee plane and magnetic field is calculated.

  //Define local buffer variables for leg properties                                                                                                               
  Double_t px1=-9999.,py1=-9999.,pz1=-9999.;
  Double_t px2=-9999.,py2=-9999.,pz2=-9999.;

  if(MagField>0){
    if(fD1.GetQ()>0){
      px1 = fD1.GetPx();
      py1 = fD1.GetPy();
      pz1 = fD1.GetPz();

      px2 = fD2.GetPx();
      py2 = fD2.GetPy();
      pz2 = fD2.GetPz();
    }else{
      px1 = fD2.GetPx();
      py1 = fD2.GetPy();
      pz1 = fD2.GetPz();

      px2 = fD1.GetPx();
      py2 = fD1.GetPy();
      pz2 = fD1.GetPz();
    }
  }else{
    if(fD1.GetQ()>0){
      px1 = fD2.GetPx();
      py1 = fD2.GetPy();
      pz1 = fD2.GetPz();

      px2 = fD1.GetPx();
      py2 = fD1.GetPy();
      pz2 = fD1.GetPz();
    }else{
      px1 = fD1.GetPx();
      py1 = fD1.GetPy();
      pz1 = fD1.GetPz();

      px2 = fD2.GetPx();
      py2 = fD2.GetPy();
      pz2 = fD2.GetPz();
    }
  }    

  Double_t px = px1+px2;
  Double_t py = py1+py2;
  Double_t pz = pz1+pz2;
  Double_t dppair = TMath::Sqrt(px*px+py*py+pz*pz);

  //unit vector of (pep+pem) 
  Double_t pl = dppair;
  Double_t ux = px/pl;
  Double_t uy = py/pl;
  Double_t uz = pz/pl;
  Double_t ax = uy/TMath::Sqrt(ux*ux+uy*uy);
  Double_t ay = -ux/TMath::Sqrt(ux*ux+uy*uy); 
  
  //momentum of e+ and e- in (ax,ay,az) axis. Note that az=0 by 
  //definition. 
  //Double_t ptep = iep->Px()*ax + iep->Py()*ay; 
  //Double_t ptem = iem->Px()*ax + iem->Py()*ay; 
  
  Double_t pxep = px1;
  Double_t pyep = py1;
  Double_t pzep = pz1;
  Double_t pxem = px2;
  Double_t pyem = py2;
  Double_t pzem = pz2;
  
  //vector product of pep X pem 
  Double_t vpx = pyep*pzem - pzep*pyem; 
  Double_t vpy = pzep*pxem - pxep*pzem; 
  Double_t vpz = pxep*pyem - pyep*pxem; 
  Double_t vp = sqrt(vpx*vpx+vpy*vpy+vpz*vpz); 
  //Double_t thev = acos(vpz/vp); 
  
  //unit vector of pep X pem 
  Double_t vx = vpx/vp; 
  Double_t vy = vpy/vp; 
  Double_t vz = vpz/vp; 

  //The third axis defined by vector product (ux,uy,uz)X(vx,vy,vz) 
  Double_t wx = uy*vz - uz*vy; 
  Double_t wy = uz*vx - ux*vz; 
  //Double_t wz = ux*vy - uy*vx; 
  //Double_t wl = sqrt(wx*wx+wy*wy+wz*wz); 
  // by construction, (wx,wy,wz) must be a unit vector. 
  // measure angle between (wx,wy,wz) and (ax,ay,0). The angle between them 
  // should be small if the pair is conversion 
  //
  Double_t cosPhiV = wx*ax + wy*ay; 
  Double_t phiv = TMath::ACos(cosPhiV); 

  return phiv;

}

//______________________________________________
Double_t PairAnalysisPairKF::GetPairPlaneAngle(Double_t v0rpH2, Int_t VariNum)const
{

  // Calculate the angle between electron pair plane and variables
  // kv0rpH2 is reaction plane angle using V0-A,C,AC,Random

  Double_t px1=-9999.,py1=-9999.,pz1=-9999.;
  Double_t px2=-9999.,py2=-9999.,pz2=-9999.;

  px1 = fD1.GetPx();
  py1 = fD1.GetPy();
  pz1 = fD1.GetPz();

  px2 = fD2.GetPx();
  py2 = fD2.GetPy();
  pz2 = fD2.GetPz();

  //p1+p2
  Double_t px = px1+px2;
  Double_t py = py1+py2;
  Double_t pz = pz1+pz2;

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
Double_t PairAnalysisPairKF::PairPlaneMagInnerProduct(Double_t ZDCrpH1) const
{

  // Calculate inner product of the strong magnetic field and electron pair plane

  if(ZDCrpH1 == 0.) return -9999.;

  Double_t px1=-9999.,py1=-9999.,pz1=-9999.;
  Double_t px2=-9999.,py2=-9999.,pz2=-9999.;

  px1 = fD1.GetPx();
  py1 = fD1.GetPy();
  pz1 = fD1.GetPz();


  px2 = fD2.GetPx();
  py2 = fD2.GetPy();
  pz2 = fD2.GetPz();

  // normal vector of ee plane
  Double_t pnorx = py2*pz1 - pz2*py1;
  Double_t pnory = pz2*px1 - px2*pz1;
  Double_t pnorz = px2*py1 - py2*px1;
  Double_t pnor  = TMath::Sqrt( pnorx*pnorx + pnory*pnory + pnorz*pnorz );

  //unit vector
  Double_t upnx = -9999.;
  Double_t upny = -9999.;
  //Double_t upnz = -9999.;

  if (pnor == 0) return -9999.;
  upnx= pnorx/pnor;
  upny= pnory/pnor;
  //upnz= pnorz/pnor;

  //direction of strong magnetic field
  Double_t magx = TMath::Cos(ZDCrpH1+(TMath::Pi()/2));
  Double_t magy = TMath::Sin(ZDCrpH1+(TMath::Pi()/2));

  //inner product of strong magnetic field and  ee plane
  Double_t upnmag = upnx*magx + upny*magy;

  return upnmag;
}


