//*-- Author : A.P.Jerusalimov ---------------
//*-- Modified: 08.02.2005 by A.P.Jerusalimov
//*-------------------------------------------
/*************************************************************************
 *  CbmStsPolyFitter --- class for polynomial fitting
 *
 * options:
 *         " found" --- fitting for found tracks
 *           fit --- makes and fill fitting information
 *           eval --- makes output for evaluaton
 *           default --- Geant tracks
 *
 *
 *************************************************************************/

#include "CbmStsPolyFitter.h"
#include "CbmStsTrack.h"
#include "CbmStsJointTrack.h"
#include "FairField.h"
#include "FairMCApplication.h"
#include "CbmStsMassHypothesis.h"
#include "FairRunAna.h"

#include <iostream>
#include <cmath>
using std::cout;
using std::endl;

Double_t M[4] = {0.511, 139.57, 493.68, 938.27};

//___________________________________________________________
CbmStsPolyFitter::~CbmStsPolyFitter() {
  FairRootManager *fManger =FairRootManager::Instance();
  fManger->Write();
}

//___________________________________________________________
CbmStsPolyFitter::CbmStsPolyFitter() {
  //
  fOutputTracks = new TClonesArray("CbmStsTrack");
}

//___________________________________________________________
CbmStsPolyFitter::CbmStsPolyFitter(const char *name, const char *title)
	:FairTask(name) {

  TrHits = new TRefArray();  
  fOutputTracks = new TClonesArray("CbmStsTrack");

}

//_________________________________________________________________
void CbmStsPolyFitter::Register() {
//
  FairRootManager::Instance()->Register("CbmStsTrack","Sts", fOutputTracks, kTRUE);

}

//___________________________________________________________
InitStatus CbmStsPolyFitter::Init() {

  Register();

  FairRootManager *fManger =FairRootManager::Instance();	
  fHits = (TClonesArray *)fManger->GetObject("StsHit");

 if ( !fHits ) { cout << "-E no Hit found" << endl; exit(1);}
  iRegFit = 1;  

  if(fOption.Contains("found")) {
//    fTracks = (TClonesArray *)fManger->CheckActivatedBranch("StsFoundTrack");
    fTracks = (TClonesArray *)fManger->GetObject("StsFoundTrack");
    iRegFit = 1;
  }
  else {
//    fTracks = (TClonesArray *)fManger->CheckActivatedBranch("StsGeantTrack");
    fTracks = (TClonesArray *)fManger->GetObject("StsGeantTrack");
  }

  if ( !fTracks ) { cout << "-E no Track found" << endl; exit(1);}
  
  FairRunAna *fRun=FairRunAna::Instance();
  fField = 0x0;
      //(FairField*) fRun->GetField();


  //  Now reconstruct the field in Memory:
  //  FairField *fMap = dynamic_cast< FairField *> (fField);
//  fMap->Init();

  zPlane[0] = 5.0;  
  zPlane[1] = 10.0;
  zPlane[2] = 20.0;
  zPlane[3] = 40.0;
  zPlane[4] = 60.0;
  zPlane[5] = 80.0;  
  zPlane[6] = 100.0;

  sigX = 20.0; // res. in X (mkm)
  sigY = 20.0; // res. in Y (mkm)

  numOfTrackPro = 1000;  // Number of tracks to be processed for each event

  return kSUCCESS;
}

//______________________________________________________
void CbmStsPolyFitter::Exec(Option_t * option) {

  CbmStsJointTrack *track=NULL;
  CbmStsHit *hit=NULL;

  fOutputTracks->Clear();

  for (Int_t jtr=0; jtr <  fTracks->GetEntries(); jtr++ ) {  // Loop for tracks

    qTrack = -1;
    if(jtr >= numOfTrackPro) continue;
    track = (CbmStsJointTrack*) fTracks->At(jtr);

    if(!fOption.Contains("fit")) {
      Output(track);
      continue;
    }

    TrHits = track->GetRHits();
    fNHit = TrHits->GetEntries();

    if(fNHit < 3) continue;  // !!!

    numOfTrack = track->GetTrackNumber();

    if(iRegFit == 0) {
      pID = track->GetPid();
      Pin = track->GetP();
      BETAin = track->GetBeta();
      TGAin = track->GetTanAlpha();
    }
    else {
      pID = 0;
      Pin = 0.0;
    }

//--- Prefilter ---

    if(iRegFit == 0 && (Pin < 0.025 || Pin > 50.0)) continue;

    if(pID != -11 && pID != 11 &&
       pID != 211 && pID != -211 && 
       pID != 321 && pID != -321 &&
       pID != 2212 && pID != 0) continue;

//--- Reconstruct of Geant data

    Int_t jhit;
    for(jhit=0; jhit < fNHit; jhit++) {
      hit = (CbmStsHit *)TrHits->At(jhit);
      Xhit[jhit] = hit->GetX();
      Yhit[jhit] = hit->GetY();
      Zhit[jhit] = hit->GetZ();
      for(Int_t iPlane = 0; iPlane < 7; iPlane++) {
        iHit[jhit] = -1;
        if(fabs(Zhit[jhit]-zPlane[iPlane]) < 0.2) {
          iHit[jhit] = iPlane+1;
          break;
        } 
      }
    }

//---    

    fNFitPoint = fNHit;

    if(iRegFit == 0) {
      fNFitPoint = fNHit +1;
      for(jhit=fNHit; jhit >0 ; jhit--) {
        Xhit[jhit] = Xhit[jhit-1];
        Yhit[jhit] = Yhit[jhit-1];
        Zhit[jhit] = Zhit[jhit-1];
        iHit[jhit] = iHit[jhit-1];
      }
      Xhit[0] = track->GetVertex().GetX();
      Yhit[0] = track->GetVertex().GetY();
      Zhit[0] = track->GetVertex().GetZ();
      iHit[0] = 0;
    }

    Double_t point[3], Hhi[3];
    for(jhit=0; jhit < fNFitPoint; jhit++) {
      point[0] = Xhit[jhit];
      point[1] = Yhit[jhit];
      point[2] = Zhit[jhit];
      fField->GetFieldValue( point, Hhi);
      Hx[jhit] = Hhi[0]/10.0;
      Hy[jhit] = Hhi[1]/10.0;
      Hz[jhit] = Hhi[2]/10.0;
    }

//---------  Processing of tracks  ----------

  if(Parabolica()) {

    qTrack = iRegFit;

    nonuniHandEloss();

    parEndPoint();
    
    if(fOption.Contains("eval")) {
      OutputForEval(track);
    }
    else {
      Output(track);
    }

  }
  else {
    track->SetPid(-9999);
  }

 }  // End of loop for tracks

}

//_____________________________________________________
void CbmStsPolyFitter::Finish() {
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Fill();

  if(!fOption.Contains("eval")) {
    fTracks->Clear();
  }
  cout << " # of fitted tracks " << fOutputTracks->GetEntries() << endl;
 
}

//___________________________________________________________________________
void CbmStsPolyFitter::Output(CbmStsJointTrack *track) {

  // fill the track information for external uses

  TClonesArray& trackRef = *fOutputTracks;
  Int_t size = trackRef.GetEntriesFast();
  CbmStsTrack *out =  new(trackRef[size]) CbmStsTrack();

  TrHits = track->GetRHits();     
  CbmStsHit *hit = NULL;

  for (Int_t k = TrHits->GetEntries(); --k >= 0.; ) {
    hit = (CbmStsHit *)TrHits->At(k);      
//    if(hit->GetStation() < 4){
//      out->AddMapsHit(hit->GetHitNumber(),(CbmStsMapsHit *) hit);
      out->AddStsHit(hit->GetHitNumber(),(FairHit *) hit);
//    }
//    else {
//      out->AddStripHit(hit->GetHitNumber(), (CbmStsStripHit *) hit);
//      out->AddStsHit(hit->GetHitNumber(), (CbmStsStripHit *) hit);
//    }
  }

  if(fOption.Contains("fit")) {

    // fill track parameters only for pi(+-) hypothesis

    out->SetPidHypo(211);
    out->SetChi2(chi2);

    FairTrackParam *parfst =  out->GetParamFirst();

    Double_t tgbet = TMath::Tan(fBETA1);
    parfst->SetTx(tgbet);  //  beta
    parfst->SetTy(fTGA1*TMath::Sqrt(1. + tgbet*tgbet));  //  tg(alpha)*sqrt((1+beta)

    if(Rparab > 0.) {
      parfst->SetQp(1.e+03/fP1f[1]);  //
    }
    else {
      parfst->SetQp(-1.e+03/fP1f[1]);  //
    }

    hit = (CbmStsHit *)TrHits->First();

    parfst->SetX(hit->GetX());  /** Position of track at given z [cm]   ??? **/
    parfst->SetY(hit->GetY());  //
    parfst->SetZ(hit->GetZ());  //

    FairTrackParam *parlast =  out->GetParamLast();

    tgbet = TMath::ATan(fBETAN);
    parlast->SetTx(tgbet);  //  beta
    parlast->SetTy(fTGAN*TMath::Sqrt(1. + tgbet*tgbet));  //  tg(alpha)*sqrt((1+beta)

    if(Rparab > 0.) {
      parlast->SetQp(1.e+03/P1fN[1]);  //
    }
    else {
      parlast->SetQp(-1.e+03/P1fN[1]);  //
    }

    hit = (CbmStsHit *)TrHits->Last();

    parlast->SetX(hit->GetX());  /** Position of track at given z [cm]   ??? **/
    parlast->SetY(hit->GetY());  //
    parlast->SetZ(hit->GetZ());  //

  }

  //  out->Print(); cout << endl;

}

//___________________________________________________________________________
void CbmStsPolyFitter::OutputForEval(CbmStsJointTrack *track) {

  // fill the momentum information for evaluation

  track->SetInAngle(fTGA1, fBETA1);
  for (Int_t k =0; k < 4; k++) fP1f[k] /= 1000;
 
  track->SetInMomenta(fP1f[3], fP1f[2], fP1f[1], fP1f[0]);
  //  track->SetVertex( fP1f[3], fP1f[2], fP1f[1]);

  track->SetOutAngle(fTGAN, fBETAN);
  track->SetOutMomenta(P1fN[3], P1fN[2], P1fN[1], P1fN[0]);

  CbmStsMassHypothesis mh = track->GetInMomenta();

}

//_______________________________________________________
Bool_t CbmStsPolyFitter::Parabolica() {

  Double_t x0[10], y0[10], z0[10];

  Double_t limMeanDelx1 = 500.0;  // for FIELD.v03b.map
  Double_t limMeanDely1 = 150.0;  // for FIELD.v03b.map
  Double_t sigX0Z;
  Double_t corrX = 8.0;   

  Double_t dSi = 0.010;

  Int_t j;

  for(j=0; j < fNFitPoint; j++) {
    x0[j] = Xhit[j];
    y0[j] = Yhit[j];
    z0[j] = Zhit[j];
  }

//---  Determination of R_parabola and Beta  

  Double_t x00 = x0[0];
  Double_t z00 = z0[0];
  Int_t iEndHit = fNFitPoint-1;  
  Double_t lTR = TMath::Sqrt((x0[iEndHit]-x00)*(x0[iEndHit]-x00) +
		 (z0[iEndHit]-z00)*(z0[iEndHit]-z00));
  cosPhi1 =  (z0[iEndHit]-z00)/lTR;
  sinPhi1 =  (x0[iEndHit]-x00)/lTR;
  FI0 = TMath::ATan2(sinPhi1,cosPhi1);

  Double_t x1[10], y1[10], z1[10];

  for(j=0; j < fNFitPoint; j++) {
    x1[j] = (x0[j]-x00)*cosPhi1 - (z0[j]-z00)*sinPhi1;
    y1[j] = y0[j];
    z1[j] = (x0[j]-x00)*sinPhi1 + (z0[j]-z00)*cosPhi1;
  }

  Double_t B = (z1[iEndHit]-z1[0])/2.0;
  Double_t a11 = 0.0, a12 = 0.0, a22 = 0.0, b1 = 0.0, b2 = 0.0;
  Double_t delx, det, A, C;

  for(j=0; j < fNFitPoint; j++) {
    delx= (z1[j]-B)*(z1[j]-B);
    a11 += delx*delx;
    a12 += delx;
    a22 += 1.0;
    b1 += x1[j]*delx;
    b2 += x1[j];
  }

  det = a11*a22 - a12*a12;
  A = (b1*a22-b2*a12)/det;
  C = (b2*a11-b1*a12)/det;

  Rparab = 1.0/(2.0*A);
  fBETA0 = -2.0*A*B;
  tgBeta1 = -(Rparab+C)/B;

//--- Beta from 3 hits ---
  Double_t B3h = (x1[1]*z1[2]*z1[2]-x1[2]*z1[1]*z1[1])/(z1[1]*z1[2]*(z1[2]-z1[1]));
  fBETA1 = TMath::ATan(B3h) + FI0;
  if(Rparab > 0.0) fBETA1 += 0.0005;
  if(Rparab < 0.0) fBETA1 -= 0.0005;     
//---

  Double_t phi[10], ARGphi[10], delx1[10];
  Double_t meanDelx1 = 0.0;

  for(j=0; j < fNFitPoint; j++) {
    delx1[j] = (x1[j]-A*(z1[j]-B)*(z1[j]-B)-C)*10000.0;  // in mkm
    delx1[j] = delx1[j]/fabs(C);  // Corr. of delx1[j]
    meanDelx1 += delx1[j]*delx1[j];
    ARGphi[j] = (z1[j]-B)/Rparab;
    if(TMath::Abs(ARGphi[j]) >0.99999) {
      qTrack = -1;
      return kFALSE;  // Too small curvature
    } 
    phi[j] = TMath::ASin(ARGphi[j]);
  }

  sigX0Z = sigX*corrX;
  chi2 = meanDelx1/(sigX0Z*sigX0Z)/(0.000001+fNFitPoint-3);

  meanDelx1 = TMath::Sqrt(meanDelx1/(fNFitPoint));
  if(meanDelx1 > limMeanDelx1) {
    qTrack = -2;
    return kFALSE;  // Too large dev in XZ plane
  }

//---
  Sp[0] = 0.0;
  StSi = 0.0;
  for(j=1; j < fNFitPoint; j++) {
     Sp[j] = (phi[j] - phi[j-1])*Rparab + Sp[j-1];
     StSi += dSi*TMath::Sqrt(1.0 + 4.0*A*A*(z1[j]-B)*(z1[j]-B))/cosPhi1;
  }

//---  Determination of tgA

  Int_t numOfHitsTgA = fNFitPoint;
  while(numOfHitsTgA > 3 && iHit[numOfHitsTgA-1] > 3) numOfHitsTgA-- ;

  a11 = 0.0, a12 = 0.0, a22 = 0.0, b1 = 0.0, b2 = 0.0;

  for(j=0; j < numOfHitsTgA; j++) {
    a11 += phi[j]*phi[j];
    a12 += phi[j];
    a22 += 1.0;
    b1 += y1[j]*phi[j];
    b2 += y1[j];
  }

  det = a11*a22 - a12*a12;
  fTGA0 = (b1*a22-b2*a12)/det/Rparab;
  Double_t Y10 = (b2*a11-b1*a12)/det;
  Double_t dely1[10];
  Double_t meanDely1 = 0.0;

  for(j=0; j < fNFitPoint; j++) {
    dely1[j] = (y1[j]-Y10-fTGA0*Rparab*phi[j])*10000.0;  // in mkm
    if(j < numOfHitsTgA)
      meanDely1 += dely1[j]*dely1[j];
  }

  w2 = meanDely1/(sigY*sigY)/(0.000001+fNFitPoint-2);  
  meanDely1 = TMath::Sqrt(meanDely1/(numOfHitsTgA+1));
  if(meanDely1 > limMeanDely1) {
    qTrack = -3;
    return kFALSE;  // Too large dev in YZ plane
  }
  fTGA1 = fTGA0;

 return kTRUE;

 }
//__________________________________________________________________
void CbmStsPolyFitter::nonuniHandEloss() {

  Double_t H[3], dHdS[3], d2HdS2[3];
  Double_t coeH = 1.0242;  // for GSI H

  Double_t beta1 = TMath::ATan(tgBeta1);
  if(beta1 < 0.0) beta1 = beta1 + TMath::Pi();
  Double_t BETA = beta1 + FI0;
  Double_t cosBETA = TMath::Cos(BETA);
  Double_t sinBETA = TMath::Sin(BETA);
  Double_t cosA = 1.0/TMath::Sqrt(1.0+fTGA0*fTGA0);
  Double_t sinA = fTGA0*cosA;
  Double_t Rexp = TMath::Abs(Rparab);

  getHtrack(H, dHdS, d2HdS2);

//------  Take into account non-uniformity of H field 

  Double_t H0 = 10.0*(-H[1] + fTGA0*(H[0]*cosBETA + H[2]*sinBETA));
  Double_t fP0 = TMath::Abs(0.3*Rexp*H0/cosA);
  Double_t H1 = 10.0*(-dHdS[1] + fTGA0*(dHdS[0]*cosBETA + dHdS[2]*sinBETA));
  Double_t H2 = 10.0*coeH*(-d2HdS2[1] + fTGA0*(d2HdS2[0]*cosBETA + d2HdS2[2]*sinBETA));
  Double_t BH0 = 10.0*(H[2]*cosBETA - H[0]*sinBETA);
  Double_t BH1 = 10.0*(dHdS[2]*cosBETA - dHdS[0]*sinBETA);
  Double_t DH0 = 10.0*(H[0]*cosBETA + H[2]*sinBETA);
  Double_t DH1 = 10.0*(dHdS[0]*cosBETA + dHdS[2]*sinBETA);

  Double_t TERM1 = 0.15/(cosA*cosA)*(H1*Rexp + 1.0/cosA*BH0*DH0/H0 + 2.0*BH0*sinA);
  Double_t TERM2=0.3/(7.0*cosA*cosA)*
    (Rexp/cosA*H2 + 3.0*fTGA0*BH1 - 2.0*sinA/Rexp*DH0 +
     1.0/2.0*fTGA0*BH0*H1/H0 + 1.0/(cosA*cosA)*BH1*DH0/H0 +
     2.0/(cosA*cosA)*BH0*DH1/H0+3.0/8.0*cosA/Rexp*H0);
  Strack = Sp[fNFitPoint-1];
  Double_t PaddHV = (TERM1 + TERM2*Strack*cosA)*Strack*cosA;
  fP1 = fP0 + PaddHV;

//------  Take into account ionization loss
  Double_t dPiDiM[4][2], d2PiDiM[4][2], dPiD[4], effS[2];
  Int_t iD;
  effS[0] = Strack/cosA;
  effS[1] = StSi/cosA;

  for(iD = 0; iD < 4; iD++) {
    dPiD[iD] = 0.0;
    for(int iM = 0; iM < 2; iM++) {
      dPiDiM[iD][iM] = dPdS(fP0, iD, iM)*effS[iM];
      d2PiDiM[iD][iM] = d2PdS2(fP0, iD, iM)*effS[iM]*effS[iM];
      dPiD[iD] += 0.5*dPiDiM[iD][iM] + d2PiDiM[iD][iM]/7.0; 
    }
    fP1f[iD] = fP1 - dPiD[iD];
  }

}

//__________________________________________________________________
void CbmStsPolyFitter::getHtrack(Double_t *H0, Double_t *dHdS, Double_t *d2HdS2) {

 Double_t Hp[3][10];
 Int_t i;

 for(Int_t jhit=0; jhit < fNFitPoint; jhit++) {
   Hp[0][jhit] = Hx[jhit];
   Hp[1][jhit] = Hy[jhit];
   Hp[2][jhit] = Hz[jhit];
 }

  Double_t a11=0.0;
  Double_t a12=0.0;
  Double_t a22=0.0;

  for( i = 1; i < fNFitPoint; i++) {
    Double_t Spi2 = Sp[i]*Sp[i];
    a11 += Spi2*Spi2;
    a12 += Spi2*Sp[i];
    a22 += Spi2;
  }

  Double_t det = a11*a22 -a12*a12;

  for(int iH = 0; iH < 3; iH++) {
    Double_t b1=0.0;
    Double_t b2=0.0;
    H0[iH] = Hp[iH][0];
    for(i = 1; i < fNFitPoint; i++) {
      b1 += (Hp[iH][i] - H0[iH])*Sp[i]*Sp[i];
      b2 += (Hp[iH][i] - H0[iH])*Sp[i];
    }
    d2HdS2[iH] = 2.0*(b1*a22 - b2*a12)/det;
    dHdS[iH] = (b2*a11 - b1*a12)/det;
  }
}

//__________________________________________________________________
Double_t CbmStsPolyFitter::dPdS(Double_t P, Int_t IP, Int_t IM) {
//     dP/dS(MeV/c) for air(IM=0) and Si(IM=1)
//     for e (IP=0), pi (IP=1), K (IP=2) and p (IP=3)

  Double_t a1[2] = {2.1077e-04,0.37776};
  Double_t a2[2] = {9.3355,8.686};
  Double_t X0[2] = {30420.0,9.36};
  Double_t K = 1.214;

  Double_t v2 = 1.0/(1.0+(M[IP]/P)*(M[IP]/P));
  Double_t v = TMath::Sqrt(v2);

  if(IP !=0) return (-a1[IM]/(v*v2)*(TMath::Log(v2/(1.0-v2))-v2+a2[IM]));
  else 
    return (-a1[IM]*(a2[IM]+0.4091-0.75*TMath::Log(1.0-v2)) - P/(K*X0[IM]));
}

//__________________________________________________________________
Double_t CbmStsPolyFitter::d2PdS2(Double_t P, Int_t IP, Int_t IM) {
//     d2P/dS2(MeV2/c2) for air(IM=0) and Si(IM=1)
//     for e (IP=0), pi (IP=1), K (IP=2) and p (IP=3)

//  Double_t M[4] = {0.511, 139.57, 493.68, 938.27};
  Double_t a1[2] = {2.1077e-04,0.37776};
  Double_t X0[2] = {30420.0,9.36};
  Double_t K = 1.214;

  Double_t v2 = 1.0/(1.0+(M[IP]/P)*(M[IP]/P));
  Double_t v = TMath::Sqrt(v2);

  if(IP !=0) return
    (v/(P*TMath::Sqrt(1.0+v2)*(1.0+v2))*dPdS(P,IP,IM)*
    (-3.0/v*dPdS(P,IP,IM)-2.0*a1[IM]*(1.0-v2+v2*v2)/(v2*v2*(1.0-v2))));
  else 
    return ((-1.5*a1[IM]*v/(1.0-v2)*(M[0]*M[0])/(P*P*P)-1.0/(K*X0[IM]))*dPdS(P,IP,IM));
}

//__________________________________________________________________
void CbmStsPolyFitter::parEndPoint() {

  Double_t X1[10],Y1[10],Z1[10], X12[10],Y12[10],Z12[10], X122[10],Y122[10],Z122[10];
  Double_t X2[10],Y2[10],Z2[10], tgG;
  Double_t x1[10],y1[10],z1[10], x12[10],y12[10],z12[10], x122[10],y122[10],z122[10];   

  Int_t i;

  xN = Xhit[fNFitPoint-1]; 
  yN = Yhit[fNFitPoint-1];
  zN = Zhit[fNFitPoint-1];   

  for(i = 0; i < 3; i++) {
    X1[i] = Xhit[fNFitPoint-3+i] - Xhit[fNFitPoint-2];
    Y1[i] = Yhit[fNFitPoint-3+i] - Yhit[fNFitPoint-2];
    Z1[i] = Zhit[fNFitPoint-3+i] - Zhit[fNFitPoint-2];    
  }   

   Double_t tgB2 = X1[2]/Z1[2];
   Double_t sinB2 = tgB2/sqrt(1.0+tgB2*tgB2);
   Double_t cosB2 = 1.00/sqrt(1.0+tgB2*tgB2);

    for(i = 0; i < 3; i++) {
      X12[i] = X1[i]*cosB2 - Z1[i]*sinB2;
      Y12[i] = Y1[i];
      Z12[i] = X1[i]*sinB2 + Z1[i]*cosB2;
    }           

   Double_t tgA2 = Y12[2]/Z12[2];
   Double_t sinA2 = tgA2/sqrt(1.0+tgA2*tgA2);
   Double_t cosA2 = 1.00/sqrt(1.0+tgA2*tgA2);

    for(i = 0; i < 3; i++) {
      X122[i] = X12[i];
      Y122[i] = Y12[i]*cosA2 - Z12[i]*sinA2;
      Z122[i] = Y12[i]*sinA2 + Z12[i]*cosA2;
    }

   Double_t tgA0 = Y122[0]/X122[0];
   Double_t sinA0 = tgA0/sqrt(1.0+tgA0*tgA0);
   Double_t cosA0 = 1.00/sqrt(1.0+tgA0*tgA0);

    for(i = 0; i < 3; i++) {
      X2[i] = X122[i]*cosA0 + Y122[i]*sinA0;
      Y2[i] = X122[i]*sinA0 - Y122[i]*cosA0;
      Z2[i] = Z122[i];
    }

   tgG = X2[0]/Z2[0]*Z2[2]/(Z2[0]-Z2[2]);
   X2[3] = tgG*10.0;
   Y2[3] = Y2[2];
   Z2[3] = Z2[2] + 10.0;

    for(i = 0; i < 4; i++) {
      x122[i] =  X2[i]*cosA0 - Y2[i]*sinA0;
      y122[i] = -(-X2[i]*sinA0 - Y2[i]*cosA0);      
      z122[i] =  Z2[i];
    }

    for(i = 0; i < 4; i++) {
      x12[i] =  x122[i];
      y12[i] =  y122[i]*cosA2 + z122[i]*sinA2;
      z12[i] = -y122[i]*sinA2 + z122[i]*cosA2;
    }

    for(i = 0; i < 4; i++) {
      x1[i] =  x12[i]*cosB2 + z12[i]*sinB2;
      y1[i] =  y12[i];
      z1[i] = -x12[i]*sinB2 + z12[i]*cosB2;
    }           

  Double_t BETA = atan2((x1[3]-x1[2]),(z1[3]-z1[2]));
  if(Rparab > 0.0) BETA = BETA - 0.002;
  if(Rparab < 0.0) BETA = BETA + 0.002;    
  Double_t tgA = (y1[3]-y1[2])/sqrt((x1[3]-x1[2])*(x1[3]-x1[2]) +
				    (z1[3]-z1[2])*(z1[3]-z1[2]));

  Double_t sinB = sin(BETA);
  Double_t cosB = cos(BETA);
  Double_t aL = sinB/sqrt(1.+ tgA*tgA); 
  Double_t aM = tgA/sqrt(1.+ tgA*tgA);
  Double_t aN = cosB/sqrt(1.+ tgA*tgA);

  Double_t dPiDiM[4][2], d2PiDiM[4][2], dPiD[4], effS[2]; 
  effS[0] = Strack/sqrt(1.0+fTGA1*fTGA1);
  effS[1] = StSi/sqrt(1.0+fTGA1*fTGA1);

  for(int iD = 0; iD < 4; iD++) {
    dPiD[iD] = 0.0;
    for(int iM = 0; iM < 2; iM++) {
      dPiDiM[iD][iM] = dPdS(fP1f[iD], iD, iM)*effS[iM];
      d2PiDiM[iD][iM] = d2PdS2(fP1f[iD], iD, iM)*effS[iM]*effS[iM];
      dPiD[iD] += dPiDiM[iD][iM] + 0.5*d2PiDiM[iD][iM];
    }
    P1fN[iD]  = fP1f[iD] + dPiD[iD];
    P1fNx[iD] = P1fN[iD]*aL;
    P1fNy[iD] = P1fN[iD]*aM;
    P1fNz[iD] = P1fN[iD]*aN;
  }  

  fBETAN = BETA;
  fTGAN = tgA;

}

////:~

ClassImp(CbmStsPolyFitter)
