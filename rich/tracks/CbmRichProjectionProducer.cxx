/**
* \file CbmRichProjectionProducer.cxx
*
* \author P.Stolpovsky
* \date 2005
**/

#include "CbmRichProjectionProducer.h"
#include "CbmRichHitProducer.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairTrackParam.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairGeoNode.h"
#include "CbmGeoRichPar.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairRunAna.h"

#include "TVector3.h"
#include "TClonesArray.h"
#include "TMatrixFSym.h"

#include <iostream>

using std::cout;
using std::endl;


CbmRichProjectionProducer::CbmRichProjectionProducer(
      Int_t zflag):
   CbmRichProjectionProducerBase(zflag),
   fListRICHImPlanePoint(NULL),
   fNHits(0),
   fEvent(0),
   fGP()
{
}

CbmRichProjectionProducer::~CbmRichProjectionProducer()
{
   FairRootManager *fManager =FairRootManager::Instance();
   fManager->Write();
}

void CbmRichProjectionProducer::SetParContainers()
{

}

void CbmRichProjectionProducer::Init()
{
   FairRootManager* manager = FairRootManager::Instance();

   fGP = CbmRichHitProducer::InitGeometry();

   fGP.Print();

   fEvent = 0;

   fListRICHImPlanePoint = (TClonesArray*)manager->GetObject("RichTrackParamZ");
   if (fZflag == 1) cout << "   use tracks in imaginary plane for projection to photodetector plane" << endl;
   if (fZflag == 2) cout << "   use tracks in RICH mirror for projection to photodetector plane" << endl;
   if ( NULL == fListRICHImPlanePoint) {
      cout << "-W- CbmRichProjectionProducer::Init: No Rich Z-Point array!" << endl;
	}
}

void CbmRichProjectionProducer::DoProjection(
      TClonesArray* richProj)
{
   fEvent++;
   cout << "CbmRichProjectionProducer: event " << fEvent << endl;

   richProj->Clear();
   TMatrixFSym covMat(5);
   for(Int_t i = 0; i < 5; i++){
      for(Int_t j=0; j<=i; j++){
         covMat(i,j) = 0;
      }
   }
   covMat(0,0) = covMat(1,1) = covMat(2,2) = covMat(3,3) = covMat(4,4) = 1.e-4; 

   for(Int_t j = 0; j < fListRICHImPlanePoint->GetEntriesFast(); j++) {
      FairTrackParam* point = (FairTrackParam*)fListRICHImPlanePoint->At(j);
      new((*richProj)[j]) FairTrackParam(0., 0., 0., 0., 0., 0., covMat);
    
      // check if Array was filled
      if (point->GetX() == 0 && point->GetY() == 0 && point->GetZ() == 0 && point->GetTx() == 0 && point->GetTy() ==0) continue;
      if (point->GetQp()==0) continue;

      Double_t rho1 = 0.;
      Double_t rho2 = 0.;
      TVector3 startP, momP, crossP, centerP;

      // operate on ImPlane point
      if (fZflag ==1) {
         Double_t p = 1./TMath::Abs(point->GetQp());
         Double_t pz;
         if  ((1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy()) > 0. )
            pz = p/TMath::Sqrt(1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy());
         else {
            cout << " -E- RichProjectionProducer: strange value for calculating pz: " <<
                  (1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy()) << endl;
            pz = 0.;
         }
         Double_t px = pz*point->GetTx();
         Double_t py = pz*point->GetTy();
         momP.SetXYZ(px,py,pz);
         point->Position(startP);
         if ((fGP.fMirrorY*startP.y())<0) fGP.fMirrorY = -fGP.fMirrorY; // check that mirror center and startP are in same hemisphere

         // calculation of intersection of track with selected mirror
         // corresponds to calculation of intersection between a straight line and a sphere:
         // vector: r = startP - mirrorCenter
         // RxP = r*momP
         // normP2 = momP^2
         // dist = r^2 - fR^2
         // -> rho1 = (-RxP+sqrt(RxP^2-normP2*dist))/normP2  extrapolation factor for:
         // intersection point crossP = startP + rho1 * momP
         Double_t RxP=(momP.x()*(startP.x()-fGP.fMirrorX)+momP.y()*(startP.y()-fGP.fMirrorY)+momP.z()*(startP.z()-fGP.fMirrorZ));
         Double_t normP2=(momP.x()*momP.x()+momP.y()*momP.y()+momP.z()*momP.z());
         Double_t dist=(startP.x()*startP.x()+fGP.fMirrorX*fGP.fMirrorX+startP.y()*startP.y()+fGP.fMirrorY*fGP.fMirrorY+startP.z()*startP.z()+
               fGP.fMirrorZ*fGP.fMirrorZ-2*startP.x()*fGP.fMirrorX-2*startP.y()*fGP.fMirrorY-2*startP.z()*fGP.fMirrorZ-fGP.fMirrorR*fGP.fMirrorR);

         if ((RxP*RxP-normP2*dist) > 0.) {
            if (normP2!=0.)  rho1=(-RxP+TMath::Sqrt(RxP*RxP-normP2*dist))/normP2;
            if (normP2 == 0) cout << " Error in track extrapolation: momentum = 0 " << endl;
         } else {
            //cout << " -E- RichProjectionProducer:  RxP*RxP-normP2*dist = " << RxP*RxP-normP2*dist << endl;
         }

         Double_t crossPx = startP.x() + rho1*momP.x();
         Double_t crossPy = startP.y() + rho1*momP.y();
         Double_t crossPz = startP.z() + rho1*momP.z();
         crossP.SetXYZ(crossPx, crossPy, crossPz);

         // check if crosspoint with mirror and chosen mirrorcenter (y) are in same hemisphere
         // if not recalculate crossing point
         if ((fGP.fMirrorY*crossP.y())<0) {
            fGP.fMirrorY = -fGP.fMirrorY;
            RxP=(momP.x()*(startP.x()-fGP.fMirrorX)+momP.y()*(startP.y()-fGP.fMirrorY)+momP.z()*(startP.z()-fGP.fMirrorZ));
            normP2=(momP.x()*momP.x()+momP.y()*momP.y()+momP.z()*momP.z());
            dist=(startP.x()*startP.x()+fGP.fMirrorX*fGP.fMirrorX+startP.y()*startP.y()+fGP.fMirrorY*fGP.fMirrorY+startP.z()*startP.z()+
                  fGP.fMirrorZ*fGP.fMirrorZ-2*startP.x()*fGP.fMirrorX-2*startP.y()*fGP.fMirrorY-2*startP.z()*fGP.fMirrorZ-fGP.fMirrorR*fGP.fMirrorR);

            if ((RxP*RxP-normP2*dist) > 0.) {
               if (normP2!=0.)  rho1=(-RxP+TMath::Sqrt(RxP*RxP-normP2*dist))/normP2;
               if (normP2 == 0) cout << " Error in track extrapolation: momentum = 0 " << endl;
            } else{
               //cout << " -E- RichProjectionProducer:  RxP*RxP-normP2*dist = " << RxP*RxP-normP2*dist << endl;
            }

            crossPx=startP.x()+rho1*momP.x();
            crossPy=startP.y()+rho1*momP.y();
            crossPz=startP.z()+rho1*momP.z();
            crossP.SetXYZ(crossPx,crossPy,crossPz);
         }

         centerP.SetXYZ(fGP.fMirrorX,fGP.fMirrorY,fGP.fMirrorZ);    // mirror center
      }// if (fZflag ==1)

      // operate on Rich Mirror point
      if (fZflag ==2) {
         Double_t p = 1./TMath::Abs(point->GetQp());
         Double_t pz;
         if  ((1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy()) > 0. ){
            pz = p/TMath::Sqrt(1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy());
         } else {
            //cout << " -E- RichProjectionProducer: strange value for calculating pz: " <<
            //      (1+point->GetTx()*point->GetTx()+point->GetTy()*point->GetTy()) << endl;
            pz = 0.;
         }
         Double_t px = pz*point->GetTx();
         Double_t py = pz*point->GetTy();
         momP.SetXYZ(px,py,pz);
         point->Position(crossP);
         if ((fGP.fMirrorY*crossP.y())<0) fGP.fMirrorY = -fGP.fMirrorY; // check that mirror center and crossP are in same hemisphere

         centerP.SetXYZ(fGP.fMirrorX,fGP.fMirrorY,fGP.fMirrorZ); // mirror center
      } // if (fZflag ==2)

      //   calculate normal on crosspoint with mirror
      TVector3 normP(crossP.x()-centerP.x(),crossP.y()-centerP.y(),crossP.z()-centerP.z());
      normP=normP.Unit();
      // check that normal has same z-direction as momentum
      if ((normP.z()*momP.z())<0.) normP = TVector3(-1.*normP.x(),-1.*normP.y(),-1.*normP.z());

      // reflect track
      Double_t np=normP.x()*momP.x()+normP.y()*momP.y()+normP.z()*momP.z();

      Double_t refX = 2*np*normP.x()-momP.x();
      Double_t refY = 2*np*normP.y()-momP.y();
      Double_t refZ = 2*np*normP.z()-momP.z();

      // crosspoint whith photodetector plane:
      // calculate intersection between straight line and (tilted) plane:
      // normal on plane tilted by theta around x-axis: (0,-sin(theta),cos(theta)) = n
      // normal on plane tilted by phi around y-axis: (-sin(phi),0,cos(phi)) = n
      // normal on plane tilted by theta around x-axis and phi around y-axis: (-sin(phi),-sin(theta)cos(phi),cos(theta)cos(phi)) = n
      // point on plane is (fDetX,fDetY,fDetZ) = p as photodetector is tiled around its center
      // equation of plane for r being point in plane: n(r-p) = 0
      // calculate intersection point of reflected track with plane: r=intersection point
      // intersection point = crossP + rho2 * refl_track
      // take care for all 4 cases:
      //        -> first calculate for case x>0, then check
      if (refZ!=0.) {
         if (centerP.y() > 0){
            rho2 = (-TMath::Sin(fGP.fPmtPhi)*(fGP.fPmtXOrig-crossP.x())
                  -TMath::Sin(fGP.fPmtTheta)*TMath::Cos(fGP.fPmtPhi)*(fGP.fPmtYOrig-crossP.y())
                  + TMath::Cos(fGP.fPmtTheta)*TMath::Cos(fGP.fPmtPhi)*(fGP.fPmtZOrig-crossP.z()))/
                  (-TMath::Sin(fGP.fPmtPhi)*refX-TMath::Sin(fGP.fPmtTheta)*TMath::Cos(fGP.fPmtPhi)*refY + TMath::Cos(fGP.fPmtTheta)*TMath::Cos(fGP.fPmtPhi)*refZ);
         }
         if (centerP.y() < 0){
            rho2 = (-TMath::Sin(fGP.fPmtPhi)*(fGP.fPmtXOrig-crossP.x())
                  -TMath::Sin(-fGP.fPmtTheta)*TMath::Cos(fGP.fPmtPhi)*(-fGP.fPmtYOrig-crossP.y())
                  + TMath::Cos(-fGP.fPmtTheta)*TMath::Cos(fGP.fPmtPhi)*(fGP.fPmtZOrig-crossP.z()))/
                  (-TMath::Sin(fGP.fPmtPhi)*refX-TMath::Sin(-fGP.fPmtTheta)*TMath::Cos(fGP.fPmtPhi)*refY + TMath::Cos(-fGP.fPmtTheta)*TMath::Cos(fGP.fPmtPhi)*refZ);
         }
	      
         //rho2 = -1*(crossP.z() - fDetZ)/refZ;    // only for theta = 0, phi=0
         Double_t xX = crossP.x() + refX * rho2;
         Double_t yY = crossP.y() + refY * rho2;
         Double_t zZ = crossP.z() + refZ * rho2;
      
         if (xX < 0) {
            if (centerP.y() > 0){
               rho2 = (-TMath::Sin(-fGP.fPmtPhi)*(-fGP.fPmtXOrig-crossP.x())
                     -TMath::Sin(fGP.fPmtTheta)*TMath::Cos(-fGP.fPmtPhi)*(fGP.fPmtYOrig-crossP.y())
                     + TMath::Cos(fGP.fPmtTheta)*TMath::Cos(-fGP.fPmtPhi)*(fGP.fPmtZOrig-crossP.z()))/
                     (-TMath::Sin(-fGP.fPmtPhi)*refX-TMath::Sin(fGP.fPmtTheta)*TMath::Cos(-fGP.fPmtPhi)*refY + TMath::Cos(fGP.fPmtTheta)*TMath::Cos(-fGP.fPmtPhi)*refZ);
            }
            if (centerP.y() < 0){
               rho2 = (-TMath::Sin(-fGP.fPmtPhi)*(-fGP.fPmtXOrig-crossP.x())
                     -TMath::Sin(-fGP.fPmtTheta)*TMath::Cos(-fGP.fPmtPhi)*(-fGP.fPmtYOrig-crossP.y())
                     + TMath::Cos(-fGP.fPmtTheta)*TMath::Cos(-fGP.fPmtPhi)*(fGP.fPmtZOrig-crossP.z()))/
                     (-TMath::Sin(-fGP.fPmtPhi)*refX-TMath::Sin(-fGP.fPmtTheta)*TMath::Cos(-fGP.fPmtPhi)*refY + TMath::Cos(-fGP.fPmtTheta)*TMath::Cos(-fGP.fPmtPhi)*refZ);
            }
       
            xX = crossP.x() + refX * rho2;
            yY = crossP.y() + refY * rho2;
            zZ = crossP.z() + refZ * rho2;
         }
      
         // Transform intersection point in same way as MCPoints were
         // transformed in HitProducer before stored as Hit:
         TVector3 inPos(xX, yY, zZ);
         TVector3 outPos;
         CbmRichHitProducer::TiltPoint(&inPos, &outPos, fGP.fPmtPhi, fGP.fPmtTheta, fGP.fPmtZOrig);
         Double_t xDet = outPos.X();
         Double_t yDet = outPos.Y();
         Double_t zDet = outPos.Z();

         //check that crosspoint inside the plane
         if( xDet > (-fGP.fPmtXOrig-fGP.fPmtWidthX) && xDet < (fGP.fPmtXOrig+fGP.fPmtWidthX)){
            if(TMath::Abs(yDet) > (fGP.fPmtY-fGP.fPmtWidthY) && TMath::Abs(yDet) < (fGP.fPmtY+fGP.fPmtWidthY)){
	                 FairTrackParam richtrack(xDet,yDet,zDet,0.,0.,0.,covMat);
	                 * (FairTrackParam*)(richProj->At(j)) = richtrack;
            }
         }
      }// if (refZ!=0.)
   }// j
}
