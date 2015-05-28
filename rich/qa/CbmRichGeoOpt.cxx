/**
 * \file CbmRichGeoOpt.cxx
 *
 * \author Tariq Mahmoud <t.mahmoud@gsi.de>
 * \date 2014
 **/

#include "CbmRichGeoOpt.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TClonesArray.h"

#include "CbmMCTrack.h"
#include "FairTrackParam.h"
#include "CbmDrawHist.h"
#include "CbmTrackMatchNew.h"
#include "CbmRichRing.h"
#include "CbmRichHit.h"
#include "CbmRichPoint.h"

#include <iostream>
#include <string>
#include <boost/assign/list_of.hpp>
//++++++++++++++++++++++++++++++++++++++++++
#include "CbmRichHitProducer.h"
#include "CbmRichRingLight.h"

using namespace std;
using boost::assign::list_of;

CbmRichGeoOpt::CbmRichGeoOpt()
  : FairTask("CbmRichGeoOpt"),
    fRichPoints(NULL),
    fMcTracks(NULL),
    fRefPoints(NULL),
    fRichHits(NULL),
    fEventNum(0),
    fEventNum2(0),
    fGP(),
    fRichRings(NULL), 
    fRichRingMatches(NULL),
    fMinNofHits(0)

// PMTPlaneXatThird(0.),
// PMTPlaneYatThird(0.),
  H_Theta_TwoVectors(NULL),
  H_DistancePMTtoMirrCenter(NULL),
  H_DistancePMTtoMirr(NULL),
  H_MomPrim(NULL),
  H_PtPrim(NULL),
  H_MomPt(NULL),
  H_Hits_XY(NULL),
  H_PointsIn_XY(NULL),
  H_PointsOut_XY(NULL),
  H_NofPhotonsPerEv(NULL),
  H_NofPhotonsPerHit(NULL),
  H_NofPhotonsSmallerThan30(NULL),
  H_DiffXhit(NULL),
  H_DiffYhit(NULL),
  H_dFocalPoint_Delta(NULL),
  H_dFocalPoint_Rho(NULL),
  H_Alpha(NULL),
  H_Alpha_UpLeft(NULL),
  H_Alpha_UpLeft_II(NULL),
  H_Alpha_UpLeft_RegularTheta(NULL),
  H_Alpha_UpLeft_LeftHalf(NULL),
  H_Alpha_UpLeft_RightHalf(NULL),
  H_Alpha_UpLeft_UpperHalf(NULL),
  H_Alpha_UpLeft_LowerHalf(NULL),
  H_Alpha_XYposAtDet(NULL),
  H_Alpha_XYposAtDet_RegularTheta(NULL),
  H_Alpha_XYposAtDet_LeftHalf(NULL),
  H_Alpha_XYposAtDet_RightHalf(NULL),
  H_Alpha_XYposAtDet_UpperHalf(NULL),
  H_Alpha_XYposAtDet_LowerHalf(NULL),
  H_acc_mom_el(NULL),
  H_acc_pty_el(NULL),
  H_NofHitsAll(NULL),
  H_NofRings(NULL),
  H_NofRings_NofHits(NULL),
  H_RingCenterX(NULL),
  H_RingCenterY(NULL),
  H_RingCenter(NULL),
  H_Radius(NULL), 
  H_aAxis(NULL), 
  H_bAxis(NULL), 
  H_boa(NULL), 
  H_boa_RegularTheta(NULL), 
  H_boa_LeftHalf(NULL), 
  H_boa_RightHalf(NULL), 
  H_boa_UpperHalf(NULL), 
  H_boa_LowerHalf(NULL), 
  H_dR(NULL),
  H_dR_RegularTheta(NULL),
  H_dR_LeftHalf(NULL), 
  H_dR_RightHalf(NULL), 
  H_dR_UpperHalf(NULL), 
  H_dR_LowerHalf(NULL), 
  H_RingCenter_Aaxis(NULL),  
  H_RingCenter_Baxis(NULL),  
  H_RingCenter_boa(NULL),
  H_RingCenter_boa_RegularTheta(NULL),
  H_RingCenter_boa_RightHalf(NULL),
  H_RingCenter_boa_LeftHalf(NULL),
  H_RingCenter_boa_LowerHalf(NULL),
  H_RingCenter_boa_UpperHalf(NULL),
  H_RingCenter_dR(NULL),
  H_RingCenter_dR_RegularTheta(NULL),
  H_RingCenter_dR_LeftHalf(NULL),
  H_RingCenter_dR_RightHalf(NULL),
  H_RingCenter_dR_UpperHalf(NULL),
  H_RingCenter_dR_LowerHalf(NULL)

{
  fEventNum = 0;
  fEventNum2 = 0;
  fMinNofHits = 7;

}

CbmRichGeoOpt::~CbmRichGeoOpt()
{

}

InitStatus CbmRichGeoOpt::Init()
{
   cout << "CbmRichGeoOpt::Init"<<endl;
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmRichGeoOpt::Init","RootManager not instantised!"); }

   fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
   if ( NULL == fRichPoints) { Fatal("CbmRichGeoOpt::Init","No RichPoint array!"); }

   fRefPoints = (TClonesArray*) ioman->GetObject("RefPlanePoint");
   if ( NULL == fRefPoints) { Fatal("CbmRichGeoOpt::Init","No fRefPoints array!"); }

   fMcTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   if ( NULL == fMcTracks) { Fatal("CbmRichGeoOpt::Init","No MCTrack array!"); }

   fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
   if ( NULL == fRichHits) { Fatal("CbmRichGeoTest::Init","No RichHit array!"); }

   fGP = CbmRichHitProducer::InitRootGeometry();
   
   ///////////////////////////////////////////
   fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
   if ( NULL == fRichRings) { Fatal("CbmRichGeoTest::Init","No RichRing array!"); }

   fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
   if ( NULL == fRichRingMatches) { Fatal("CbmRichGeoTest::Init","No RichRingMatch array!"); }

   /////////////// need three points on the PMT plane to determine its equation
  PlanePoints.resize(3);
  for(int p=0;p<PlanePoints.size();p++){
    PlanePoints[p].SetX(-1000.);PlanePoints[p].SetY(-1000.);PlanePoints[p].SetZ(-1000.);
  }

  InitHistograms();
  
  return kSUCCESS;
}

void CbmRichGeoOpt::Exec(Option_t* option)
{
  fEventNum++;
  //Fill the coordinates of the three points on the PMT plane 
  if(fRichPoints->GetEntriesFast()>10){
    int PointsFilled=0;
    for(int p=1;p<PlanePoints.size();p++){
      if( PlanePoints[p].X() == PlanePoints[p-1].X() ){ 
	FillPointsAtPMT();PointsFilled=0;}else{ PointsFilled=1;}
    }
    PMTPlaneCenter.SetX(fGP.fPmtX); PMTPlaneCenter.SetY(fGP.fPmtY); PMTPlaneCenter.SetZ(fGP.fPmtZ);
    MirrorCenter.SetX(fGP.fMirrorX); PMTPlaneCenter.SetY(fGP.fMirrorY); PMTPlaneCenter.SetZ(fGP.fMirrorZ);

    if(PointsFilled==1){ //Run the analysis only if the points are filled and are different  
      HitsAndPoints();
      RingParameters();
      FillMcHist();
    }
  }
}



/////// Get data of hits and points
void CbmRichGeoOpt::HitsAndPoints(){
  Int_t nofRefPoints = fRefPoints->GetEntriesFast();
  Int_t nofPoints = fRichPoints->GetEntriesFast();
  if(nofPoints==0 || nofRefPoints==0){return;} if(nofPoints>1500){return;} 
  cout<<"nofPoints:  "<<  nofPoints<<endl;
  //loop over points and get momentum of photons --> calculate angle (to be done later)
  
  for (int i = 0; i < nofRefPoints; i++) {
    TVector3 PosAtRefl; TVector3 PosAtDetIn; //TVector3 PosAtDetOut;
    CbmRichPoint* RefPoint = (CbmRichPoint*)fRefPoints->At(i);
    if (RefPoint == NULL) continue;
    int trackId = RefPoint->GetTrackID(); if(trackId==-2) {continue;}
    RefPoint->Position(PosAtRefl);
    int Zpos=int(10.*PosAtRefl.Z());//2653 or 2655 -->take 2655 which is the entrance point 
                                                     //of the REFLECTED photon into the sensitive plane   
    //cout<<PosAtRefl.Z()<<"    "<<Zpos<<endl;
    if(Zpos==3037){continue;}
    CbmRichPoint* point = (CbmRichPoint*) fRichPoints->At(trackId);
    if(NULL == point) continue;
    PosAtDetIn.SetX(point->GetX()); PosAtDetIn.SetY(point->GetY()); PosAtDetIn.SetZ(point->GetZ());
    
    Int_t PointMCTrackId = point->GetTrackID();
    CbmMCTrack* PointTrack = static_cast<CbmMCTrack*>(fMcTracks->At(PointMCTrackId));
    if (NULL == PointTrack) continue;
    TVector3 PointMom; PointTrack->GetMomentum(PointMom);
    
    TVector3 LineOnPMT1=PlanePoints[1]-PlanePoints[0]; TVector3 LineOnPMT2=PlanePoints[2]-PlanePoints[0];
    TVector3 NormToPMT=LineOnPMT1.Cross(LineOnPMT2); TVector3 LineSensToPMT=PosAtDetIn-PosAtRefl;
   
      */
    TVector3 LineSensToPMT=PosAtDetIn-PosAtRefl;
    
    H_Theta_TwoVectors->Fill(LineSensToPMT.Angle(PointMom));
    /////////// calculate alpha relative to the "tilted" PMT plane !!
    double Alpha=LineSensToPMT.Angle(NormToPMT);//*TMath::RadToDeg();
    double AlphaInDeg=Alpha*TMath::RadToDeg();
    if(AlphaInDeg>90.){AlphaInDeg=180.-AlphaInDeg;}

    H_PointsIn_XY->Fill(PosAtDetIn.X(),PosAtDetIn.Y());
    H_Alpha->Fill(AlphaInDeg);
    if(PosAtDetIn.X()<0. && PosAtDetIn.Y()>0) {
      H_Alpha_UpLeft->Fill(AlphaInDeg );
      H_Alpha_XYposAtDet->Fill(PosAtDetIn.X(),PosAtDetIn.Y(),AlphaInDeg);
    }
  }
  
  //***********************************************************  
  Int_t nofHits = fRichHits->GetEntriesFast();
  //cout<<"++++++++++++++++++++++++ nofHits = "<<nofHits<<endl;
  for (Int_t iH = 0; iH < nofHits; iH++){
    CbmRichHit *hit = (CbmRichHit*) fRichHits->At(iH); if ( hit == NULL ) continue;
    Int_t pointInd =  hit->GetRefId(); if (pointInd < 0) continue;
    CbmRichPoint *point = (CbmRichPoint*) fRichPoints->At(pointInd); if ( point == NULL ) continue;
    
    H_NofPhotonsPerHit->Fill(hit->GetNPhotons());
    
    TVector3 inPos(point->GetX(), point->GetY(), point->GetZ());
    TVector3 outPos;
    CbmRichHitProducer::TiltPoint(&inPos, &outPos, fGP.fPmtPhi, fGP.fPmtTheta, fGP.fPmtZOrig);
    //cout<<hit->GetX()<<"     "<<hit->GetY()<<endl;
    H_Hits_XY->Fill(hit->GetX(), hit->GetY());
    H_DiffXhit->Fill(hit->GetX() - outPos.X());
    H_DiffYhit->Fill(hit->GetY() - outPos.Y());
    
  }
}
///////////////////////////////
void CbmRichGeoOpt::RingParameters()
{
  Int_t nofRings = fRichRings->GetEntriesFast();
  for (Int_t iR = 0; iR < nofRings; iR++){
    CbmRichRing *ring = (CbmRichRing*) fRichRings->At(iR);  if (NULL == ring) continue;
    CbmTrackMatchNew* ringMatch = (CbmTrackMatchNew*) fRichRingMatches->At(iR); if (NULL == ringMatch) continue;
    Int_t mcTrackId = ringMatch->GetMatchedLink().GetIndex();  if (mcTrackId < 0) continue;
    CbmMCTrack* mcTrack = (CbmMCTrack*)fMcTracks->At(mcTrackId);  if (!mcTrack) continue;
    
    Int_t motherId = mcTrack->GetMotherId();
    Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());
    Double_t momentum = mcTrack->GetP();
    Double_t pt = mcTrack->GetPt();
    Double_t rapidity = mcTrack->GetRapidity();
    //cout<<"pdg = "<<pdg<<", momentum = "<<momentum<<endl;
    if (pdg != 11 || motherId != -1) continue; // only primary electrons
        
    if (ring->GetNofHits() >= fMinNofHits){
      H_acc_mom_el->Fill(momentum);
      H_acc_pty_el->Fill(rapidity, pt);
    }
 
   ///////////////////////////////////
    float radius=ring->GetRadius();
    if(radius<=0.){continue;}//with ideal finder --> many rings with radius -1. 
    //Test if radius is a NAN:
    if(! (radius<=1. || radius >1.) ){continue;}
    float aA = ring->GetAaxis(); 
    float bA = ring->GetBaxis(); 

    H_Radius->Fill(radius); H_aAxis->Fill(aA); H_bAxis->Fill(bA); H_boa->Fill(bA/aA);
      
    float CentX=ring->GetCenterX();
    float CentY=ring->GetCenterY();
    //cout<<CentY<<endl;
    H_RingCenter->Fill(CentX,CentY); 
    H_RingCenter_Aaxis->Fill(CentX,CentY,aA);
    H_RingCenter_Baxis->Fill(CentX,CentY,bA); 
    H_RingCenter_boa->Fill(CentX,CentY,bA/aA);

    int nAllHitsInR=ring->GetNofHits();
    H_NofHitsAll->Fill(nAllHitsInR);
    
    for(int iH=0;iH<nAllHitsInR;iH++){
      CbmRichHit* hit = (CbmRichHit*) fRichHits->At(ring->GetHit(iH));
      double xH=hit->GetX();
      double yH=hit->GetY();
      double dR=aA-TMath::Sqrt( (CentX-xH)*(CentX-xH) + (CentY-yH)*(CentY-yH) );
      H_dR->Fill(dR);
    } 
    
   //////////////////////////////////    
    
    
  }
}

////////////////////////////////
void CbmRichGeoOpt::FillMcHist()
{
  for (Int_t i = 0; i < fMcTracks->GetEntriesFast(); i++){
    CbmMCTrack* mcTrack = (CbmMCTrack*)fMcTracks->At(i);
    if (!mcTrack) continue;
    Int_t motherId = mcTrack->GetMotherId();
    Int_t pdg = TMath::Abs(mcTrack->GetPdgCode());
    
    if (pdg == 11 && motherId == -1){
      H_MomPrim->Fill(mcTrack->GetP());
      H_PtPrim->Fill(mcTrack->GetPt());
    }
  }

}

////////////////////////////////
void CbmRichGeoOpt::InitHistograms()
{
  int nBinsX = 28; double xMin = -110.; double xMax = 110.;  
  int nBinsY = 40; double yMin = -200; double yMax = 200.;
  
  H_Theta_TwoVectors= new TH1D("H_Theta_TwoVectors", "H_Theta_TwoVectors;#theta [deg]; Yield", 180, 0., 360.);
  H_MomPrim = new TH1D("H_MomPrim", "H_MomPrim;p [GeV]; Yield", 100, 0., 10.);
  H_PtPrim = new TH1D("H_PtPrim", "H_PtPrim;p [GeV]; Yield", 80, 0., 4.);
  H_MomPt = new TH2D("H_MomPt", "H_MomPt;p [GeV];pt [GeV]; Yield", 100, 0., 10., 80, 0., 4.);

  
  H_Hits_XY = new TH2D("H_Hits_XY", "H_Hits_XY;X [cm];Y [cm];Counter", 200, -150., 50.,400, 0.,400.);
  H_PointsIn_XY = new TH2D("H_PointsIn_XY", "H_PointsIn_XY;X [cm];Y [cm];Counter", 110, -100., 10.,400, 0.,400.);
  H_PointsOut_XY = new TH2D("H_PointsOut_XY", "H_PointsOut_XY;X [cm];Y [cm];Counter", 200, -150., 50.,400, 0.,400.);
  //cout<<" init hist H_NofPhotonsPerEv"<<endl;
  H_NofPhotonsPerEv = new TH1D("H_NofPhotonsPerEv", "H_NofPhotonsPerEv;Number of photons per hit;Yield", 500, 0., 1000.);
  H_NofPhotonsPerHit = new TH1D("H_NofPhotonsPerHit", "H_NofPhotonsPerHit;Number of photons per hit;Yield", 10, -0.5, 9.5);
  H_DiffXhit = new TH1D("H_DiffXhit", "H_DiffXhit;Y_{point}-Y_{hit} [cm];Yield", 200, -1., 1.);
  H_DiffYhit = new TH1D("H_DiffYhit", "H_DiffYhit;Y_{point}-Y_{hit} [cm];Yield", 200, -1., 1.);
 
  H_Alpha= new TH1D("H_Alpha","H_Alpha;#alpha_{photon-PMT} [deg];Yield",360,0.,180.);
  H_Alpha_UpLeft= new TH1D("H_Alpha_UpLeft","H_Alpha_UpLeft;#alpha_{photon-PMT} [deg];Yield",180,0.,90.);
  H_Alpha_XYposAtDet= new TH3D("H_Alpha_XYposAtDet","H_Alpha_XYposAtDet; X [cm]; Y [cm];#alpha_{photon-PMT} [deg];Yield",270, -90, 0,  450, 50,200, 180,0.,90.);
  //////////////////////////////////////
  H_dFocalPoint_I= new TH1D("H_dFocalPoint_I","H_dFocalPoint_I;#Delta_{f} [cm];Yield",100,100.,200.);
  H_dFocalPoint_II= new TH1D("H_dFocalPoint_II","H_dFocalPoint_II;#rho_{f} [cm];Yield",100,100.,200.);


  //////////////////////////////////////
  // Detector acceptance efficiency vs. (pt,y) and p
  H_acc_mom_el = new TH1D("H_acc_mom_el", "H_acc_mom_el;p [GeV/c];Yield", 24, 0., 12.);
  H_acc_pty_el = new TH2D("H_acc_pty_el", "H_acc_pty_el;Rapidity;P_{t} [GeV/c];Yield",25, 0., 4., 20, 0., 3.);
  //////////////////////////////////

  H_NofHitsAll = new TH1D("H_NofHitsAll", "H_NofHitsAll;Nof hits in ring;Yield", 50, 0., 50.);
  
  /////////////////////////////////////////////
  H_Radius= new TH1D("H_Radius","H_Radius",401, 2.,6.);
  H_aAxis= new TH1D("H_aAxis","H_aAxis",801, 2.,10.);
  H_bAxis= new TH1D("H_bAxis","H_bAxis",801, 2.,10.);
  H_boa= new TH1D("H_boa","H_boa",500, 0.5,1.);
  H_dR= new TH1D("H_dR","H_dR",101,-5.0,5.0);  
  
  H_RingCenter= new TH2D("H_RingCenter","H_RingCenter",1001, -100., 0.,2501, 50.,300.);
  
  H_RingCenter_Aaxis= new TH3D("H_RingCenter_Aaxis","H_RingCenter_Aaxis",301, -100, 0,301, 200, 300, 401, 2.,10.);
  H_RingCenter_Baxis= new TH3D("H_RingCenter_Baxis","H_RingCenter_Baxis",301, -100, 0,301, 200, 300, 401, 2.,10.);
  H_RingCenter_boa= new TH3D("H_RingCenter_boa","H_RingCenter_boa",301, -100, 0,301, 200, 300, 251, 0.5,1.);
  
  
}

void CbmRichGeoOpt::WriteHistograms(){
 
  H_Theta_TwoVectors->Write(); 
  H_MomPrim->Write(); 
  H_PtPrim->Write(); 

  H_Hits_XY->Write(); 
  H_PointsIn_XY->Write();
  H_PointsOut_XY->Write();
  H_NofPhotonsPerHit->Write(); 
  H_DiffXhit->Write();
  H_DiffYhit->Write();
 
  H_Alpha->Write();
  H_Alpha_UpLeft->Write();
  H_Alpha_XYposAtDet->Write();
  H_acc_mom_el->Write();
  H_acc_pty_el->Write();

 H_dFocalPoint_I->Write(); 
    H_dFocalPoint_II->Write();
 H_NofHitsAll->Write();

  H_Radius->Write();
  H_aAxis->Write();
  H_bAxis->Write();
  H_boa->Write();
  H_RingCenter->Write();
  H_RingCenter_Aaxis->Write();
  H_RingCenter_Baxis->Write();
  H_RingCenter_boa->Write();
  H_dR->Write();
}

void CbmRichGeoOpt::FillPointsAtPMT()
{
  for(int p=0;p<PlanePoints.size();p++){
    if(PlanePoints[p].X() != -1000.){
      if(p==0){continue;}
      else{
	int PointFilled=1;
	for(int p2=p-1;p2>-1;p2--){
	  if(TMath::Abs( PlanePoints[p2].X() - PlanePoints[p].X() ) < 0.1){PointFilled=0;}
	}
	if(PointFilled==1){continue;}
      }
    }
    
    fEventNum2++;
    Int_t nofPoints = fRichPoints->GetEntriesFast();
    
    for(Int_t ip = 0; ip < nofPoints-10; ip+=10){
      CbmRichPoint* point = (CbmRichPoint*) fRichPoints->At(ip);
      if(NULL == point) continue;
      int trackId = point->GetTrackID(); if(trackId==-2) continue;
      
      PlanePoints[p].SetX(point->GetX());PlanePoints[p].SetY(point->GetY());PlanePoints[p].SetZ(point->GetZ());
      if(PlanePoints[p].X() !=-1000.){break;}
    }
  }
}

//////////////////////////////////////////////////////
float  CbmRichGeoOpt::GetIntersectionPointsLS( TVector3 MirrCenter,  TVector3 G_P1,  TVector3 G_P2, float R){
  float A=(G_P1-MirrCenter)*(G_P1-MirrCenter);
  float B=(G_P2-G_P1)*(G_P2-G_P1);
  float P=2.*( (G_P1-MirrCenter)*(G_P2-G_P1) )/(B);
  float q=(A-R*R)/B;

  float t1=-1.*P/2.-TMath::Sqrt( (P/2.)*(P/2.) -q);
  float t2=-1.*P/2.+TMath::Sqrt( (P/2.)*(P/2.) -q);
  //Check if nan --> no intersection
  if(! (t1==1. || t1 >1.) ){return -1.;}

  TVector3 IntersectP1;  TVector3 IntersectP2;
  IntersectP1.SetX( G_P1.X()+t1*(G_P2.X()-G_P1.X()) );
  IntersectP1.SetY( G_P1.Y()+t1*(G_P2.Y()-G_P1.Y()) );
  IntersectP1.SetZ( G_P1.Z()+t1*(G_P2.Z()-G_P1.Z()) );

  IntersectP2.SetX( G_P1.X()+t2*(G_P2.X()-G_P1.X()) );
  IntersectP2.SetY( G_P1.Y()+t2*(G_P2.Y()-G_P1.Y()) );
  IntersectP2.SetZ( G_P1.Z()+t2*(G_P2.Z()-G_P1.Z()) );

  TVector3 Line1=IntersectP1-G_P1;
  float Length1=TMath::Sqrt(Line1.X()*Line1.X() + Line1.Y()*Line1.Y() + Line1.Z()*Line1.Z());
  TVector3 Line2=IntersectP2-G_P1;
  float Length2=TMath::Sqrt(Line2.X()*Line2.X() + Line2.Y()*Line2.Y() + Line2.Z()*Line2.Z());
  
  if(Length1<Length2){return Length1;}else{return Length2;}
  
}
//////////////////////////////////////////////////////
float  CbmRichGeoOpt::GetDistanceMirrorCenterToPMTPoint(TVector3 PMTpoint)
{
  float XTerm= (PMTpoint.X()-MirrorCenter.X())*(PMTpoint.X()-MirrorCenter.X());
  float YTerm= (PMTpoint.Y()-MirrorCenter.Y())*(PMTpoint.Y()-MirrorCenter.Y());
  float ZTerm= (PMTpoint.Z()-MirrorCenter.Z())*(PMTpoint.Z()-MirrorCenter.Z());
  return TMath::Sqrt(XTerm+YTerm+ZTerm);

}
//////////////////////////////////////////////////////
void CbmRichGeoOpt::Finish()
{
  double TolaratedDiff=0.001;
  double ProdP0WithNorm=p0.Dot(norm); //cout<<"ProdP0WithNorm = "<<ProdP0WithNorm;
  double ProdPWithNorm=Point.Dot(norm); //cout<<"  ProdPWithNorm = "<<ProdPWithNorm<<endl;
  return TMath::Abs(ProdP0WithNorm - ProdPWithNorm) <= ( (TMath::Abs(ProdP0WithNorm) < TMath::Abs(ProdPWithNorm) ? TMath::Abs(ProdPWithNorm) : TMath::Abs(ProdP0WithNorm)) * TolaratedDiff);
}
//////////////////////////////////////////////////////
void CbmRichGeoOpt::GetPMTRotAngels()
{
  RotX=fGP.fPmtTheta; RotY=fGP.fPmtPhi;
}

//////////////////////////////////////////////////////
void  CbmRichGeoOpt::GetPlaneCenter(float rotMir, float rotX, float rotY)
{
  float MinX=-110., MaxX=0., MinY=120., MaxY=220.;  

  if(rotMir < 1.1 && rotMir > 0.9 ){//rotMir==1){
    MinX=-90., MaxX=-1.; MinY=70., MaxY=130.;
    //if( rotX > 0 && rotX <= 5){MinY=125., MaxY=192.;}
    if( rotX > 5 && rotX <= 15){MinY=74., MaxY=134.;}
    if( rotX > 15 && rotX <=25){MinY=79., MaxY=134.;}
    if( rotX > 25 && rotX <=35){MinY=81., MaxY=134.;}
    if( rotX > 35){MinY=85., MaxY=134.;}
    
    //if( rotY > 0 && rotY <= 5){MinX=-100., MaxX=-1.;}
    if( rotY > 5 && rotY <= 10){MinX=-88., MaxX=-1.;}
    if( rotY > 10 && rotY <20){MinX=-88., MaxX=-1.;}
    if( rotY >= 20 && rotY <35){MinX=-85., MaxX=-4.;}
    if( rotY >= 35 && rotY <45){MinX=-82., MaxX=-12.;}
    if( rotY >= 45){MinX=-81., MaxX=-20.;}
   }else if(rotMir < -9.9 && rotMir > -10.1 ){//rotMir==-10){
    MinX=-100., MaxX=-1.; MinY=125., MaxY=192.;

    //if( rotX > 0 && rotX <= 5){MinY=125., MaxY=192.;}
    if( rotX > 5 && rotX <= 10){MinY=126., MaxY=191.;}
    if( rotX > 10 && rotX <=15){MinY=128., MaxY=190.;}
    if( rotX > 15 && rotX <=20){MinY=132., MaxY=189.;}
    if( rotX > 20 && rotX <=25){MinY=134., MaxY=187.;}
    if( rotX > 25 && rotX <=30){MinY=138., MaxY=187.;}
    if( rotX > 30 && rotX <=35){MinY=139., MaxY=187.;}
    if( rotX > 35 && rotX <=40){MinY=141., MaxY=185.;}
    if( rotX > 40 && rotX <=45){MinY=142., MaxY=185.;}
    if( rotX > 45 && rotX <=50){MinY=145., MaxY=185.;}
    
    //if( rotY > 0 && rotY <= 5){MinX=-100., MaxX=-1.;}
    if( rotY > 5 && rotY <= 10){MinX=-99., MaxX=-1.;}
    if( rotY > 10 && rotY <=15){MinX=-98., MaxX=-2.;}
    if( rotY > 15 && rotY <=20){MinX=-97., MaxX=-2.;}
    if( rotY > 20 && rotY <=25){MinX=-95., MaxX=-5.;}
    if( rotY > 25 && rotY <=30){MinX=-95., MaxX=-7.;}
    if( rotY > 30 && rotY <=35){MinX=-92., MaxX=-8.;}
    if( rotY > 35 && rotY <=40){MinX=-90., MaxX=-12.;}
    if( rotY > 40 && rotY <=45){MinX=-88., MaxX=-16.;}
    if( rotY > 45 && rotY <=50){MinX=-87., MaxX=-19.;}
  }

  PMTPlaneX=MinX+(MaxX-MinX)/2.; PMTPlaneY=MinY+(MaxY-MinY)/2.;
}

//////////////////////////////////////////////////////
bool  CbmRichGeoOpt::CheckPointLiesOnSphere(TVector3 Point)
{
  
  WriteHistograms();
}



ClassImp(CbmRichGeoOpt)
