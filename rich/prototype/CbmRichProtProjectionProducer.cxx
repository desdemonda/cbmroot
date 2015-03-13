// -------------------------------------------------------------------------
// -----              CbmRichProtProjectionProducer source file            -----
// -------------------------------------------------------------------------

#include "CbmRichProtProjectionProducer.h"
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
#include "TGeoManager.h"
#include "TMatrixFSym.h"

#include <iostream>

using std::cout;
using std::endl;


CbmRichProtProjectionProducer::CbmRichProtProjectionProducer(
      Int_t zflag):
   CbmRichProjectionProducerBase(zflag),
   fListRICHImPlanePoint(NULL),

   fNHits(0),
   fEvent(0),

   fDetX(0.0),
   fDetY(0.0),
   fDetZ(0.0),
   fDetWidthX(0.0),
   fDetWidthY(0.0),
   fThetaDet(0.0),
   fPhiDet(0.0),

   fDetXTransf(0.0),
   fDetYTransf(0.0),
   fDetZTransf(0.0),

   fZm(),
   fYm(),
   fXm(),
   fR(),

   fMaxXTrackExtr(0.0),
   fMaxYTrackExtr(0.0),

   fSensNodes(NULL),
   fPassNodes(NULL),
   fPar(NULL)
{
}

CbmRichProtProjectionProducer::~CbmRichProtProjectionProducer()
{
   FairRootManager *fManager =FairRootManager::Instance();
   fManager->Write();
}

void CbmRichProtProjectionProducer::SetParContainers()
{

  FairRunAna* sim = FairRunAna::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  fPar=(CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));

}

void CbmRichProtProjectionProducer::Init()
{
   FairRootManager* fManager = FairRootManager::Instance();

   fSensNodes = fPar->GetGeoSensitiveNodes();
   fPassNodes = fPar->GetGeoPassiveNodes();

   // get detector position:
   FairGeoNode *det= (FairGeoNode *) fSensNodes->FindObject("rich1detector");
   FairGeoTransform* detTr=det->getLabTransform();  // detector position in labsystem
   FairGeoVector detPosLab=detTr->getTranslation(); // ... in cm
   FairGeoTransform detCen=det->getCenterPosition();  // center in Detector system
   FairGeoVector detPosCen=detCen.getTranslation();
   fDetZ = detPosLab.Z() + detPosCen.Z(); // z coordinate of photodetector (Labsystem, cm)
   fDetY = detPosLab.Y() + detPosCen.Y(); // y coordinate of photodetector (Labsystem, cm)
   fDetX = detPosLab.X() + detPosCen.X(); // x coordinate of photodetector (Labsystem, cm)

   TArrayD *fdetA=det->getParameters(); // get other geometry parameters: width in x, width in y, thickness
   fDetWidthX = fdetA->At(0);
   fDetWidthY = fdetA->At(1);
   // detector might be rotated by theta around x-axis:
   FairGeoRotation fdetR = detTr->getRotMatrix();

   fThetaDet = TMath::ASin(fdetR(7)); // tilting angle around x-axis
   fPhiDet = -1.*TMath::ASin(fdetR(2)); // tilting angle around y-axis

   // transform nominal detector position (for tilted photodetector):
   // shift x back by fDetZ_org*TMath::Sin(phi) in order to avoid overlap
   /*    fDetX_transf = fDetX*TMath::Cos(fPhiDet)+fDetZ*TMath::Sin(fPhiDet)-fDetZ*TMath::Sin(fPhiDet);
   fDetY_transf = -fDetX*TMath::Sin(fThetaDet)*TMath::Sin(fPhiDet) + fDetY*TMath::Cos(fThetaDet) + fDetZ*TMath::Sin(fThetaDet)*TMath::Cos(fPhiDet);
   fDetZ_transf = -fDetX*TMath::Cos(fThetaDet)*TMath::Sin(fPhiDet) - fDetY*TMath::Sin(fThetaDet) + fDetZ*TMath::Cos(fThetaDet)*TMath::Cos(fPhiDet);*/
   fDetXTransf = fDetX;
   fDetYTransf = fDetY;
   fDetZTransf = fDetZ;

   // get mirror position:
   //FairGeoNode *mir= (FairGeoNode *) fPassNodes->FindObject("rich1mgl#1");
   FairGeoNode *mir[4];
   mir[0] = (FairGeoNode *) fSensNodes->FindObject("rich1mgl1");
   mir[1] = (FairGeoNode *) fSensNodes->FindObject("rich1mgl2");
   mir[2] = (FairGeoNode *) fSensNodes->FindObject("rich1mgl3");
   mir[3] = (FairGeoNode *) fSensNodes->FindObject("rich1mgl4");
   FairGeoTransform* mirTr[4];
   FairGeoVector mirPosLab[4];
   TArrayD *fmirA[4];
   Double_t spheTheta[4];
   Double_t sphePhi[4];
   for(Int_t i = 0; i < 4; i++) {
      mirTr[i]=mir[i]->getLabTransform();  // position of mirror center in labsystem
      mirPosLab[i]=mirTr[i]->getTranslation(); // ... in cm
      fZm[i] = mirPosLab[i].Z();
      fYm[i] = mirPosLab[i].Y();
      fXm[i] = mirPosLab[i].X();
      fmirA[i]=mir[i]->getParameters();
      fR[i] = fmirA[i]->At(0); // mirror radius
      spheTheta[i] = TMath::Abs(90. - fmirA[i]->At(3)); // opening angle for SPHERE in theta (90 degree +- theta)
      sphePhi[i] = TMath::Abs(90. - fmirA[i]->At(5)); // opening angle for SPHERE in phi (90 degree +- phi)
   }
   // from that calculate (with safety factor 1.3) maximum x-y positions for track extrapolation:
   fMaxXTrackExtr = 1.3*(fR[2]*TMath::Tan(sphePhi[2]*TMath::Pi()/180.));
   fMaxYTrackExtr = 1.3*(TMath::Abs(fYm[2]) + fR[2]*TMath::Tan(spheTheta[2]*TMath::Pi()/180.));

   // mirror might be rotated by theta around x-axis:
   FairGeoRotation fmirR[4];
   Double_t thetaM[4];
   for(Int_t i = 0; i < 4; i++) {
      fmirR[i]=mirTr[i]->getRotMatrix();
      thetaM[i] = -1.*TMath::ASin(fmirR[i](5)) - TMath::Pi()/2 ;
   }

   fEvent = 0;

   fListRICHImPlanePoint = (TClonesArray*)fManager->GetObject("RichTrackParamZ"); // imaginary plane in Rich detector for track extrapolation

   if ( NULL == fListRICHImPlanePoint) {
      cout << "-W- CbmRichProtProjectionProducer::Init: No Rich Z-Point array!" << endl;
   }

}

void CbmRichProtProjectionProducer::DoProjection(
      TClonesArray* richProj)
{
   fEvent++;
   cout << "CbmRichProjectionProducer: event " << fEvent << endl;

   richProj->Clear();
   TMatrixFSym covMat(5);
   for(Int_t i=0;i<5;i++) for(Int_t j=0; j<=i; j++) covMat(i,j) = 0;
   covMat(0,0) = covMat(1,1) = covMat(2,2) = covMat(3,3) = covMat(4,4) = 1.e-4; 

   Double_t rho1,rho2;
   Double_t xX;
   Double_t yY;
   Double_t zZ;
   Double_t normP2,RxP,dist;
   Double_t crossPx,crossPy,crossPz;

   cout << "Number of tracks in Imaginary z-Plane: " << fListRICHImPlanePoint->GetEntriesFast() << endl;

   for(Int_t j = 0; j < fListRICHImPlanePoint->GetEntriesFast(); j++) {
      FairTrackParam* point = (FairTrackParam*)fListRICHImPlanePoint->At(j);
      new((*richProj)[j]) FairTrackParam(0.,0.,0.,0.,0.,0.,covMat);

      // check if Array was filled
      if (point->GetX() == 0 && point->GetY() == 0 && point->GetZ() == 0 &&
            point->GetTx() == 0 && point->GetTy() ==0) continue;
      if (point->GetQp()==0) continue;

      // check that x and y value make sense (sometimes strange extrapolations may appear)
      if (TMath::Abs(point->GetX()) > fMaxXTrackExtr || TMath::Abs(point->GetY()) > fMaxYTrackExtr){
         continue;
      }

      rho1 = rho2 = 0.;
      xX=9999.;
      yY=9999.;
      zZ=0.;
      normP2 = RxP = dist = 0.;

      TVector3 startP, momP, crossP, centerP;

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

         // calculation of intersection of track with selected mirror
         // corresponds to calculation of intersection between a straight line and a sphere:
         // vector: r = startP - mirrorCenter
         // RxP = r*momP
         // normP2 = momP^2
         // dist = r^2 - fR^2
         // -> rho1 = (-RxP+sqrt(RxP^2-normP2*dist))/normP2  extrapolation factor for:
         // intersection point crossP = startP + rho1 * momP

         RxP = (momP.x()*(startP.x()-fXm[0])+momP.y()*(startP.y()-fYm[0])+momP.z()*(startP.z()-fZm[0]));
         normP2 = (momP.x()*momP.x()+momP.y()*momP.y()+momP.z()*momP.z());
         dist = (startP.x()*startP.x()+fXm[0]*fXm[0]+startP.y()*startP.y()+fYm[0]*fYm[0]+startP.z()*startP.z()+fZm[0]*fZm[0]-2*startP.x()*fXm[0]-2*startP.y()*fYm[0]-2*startP.z()*fZm[0]-fR[0]*fR[0]);

         if ((RxP * RxP - normP2 * dist) > 0.) {
            if (normP2 != 0.)  rho1 = (-RxP+TMath::Sqrt(RxP*RxP-normP2*dist))/normP2;
            if (normP2 == 0) cout << " Error in track extrapolation: momentum = 0 " << endl;
         }
         else cout << " -E- RichProjectionProducer:  RxP*RxP-normP2*dist = " << RxP*RxP-normP2*dist << endl;

         crossPx=startP.x()+rho1*momP.x();
         crossPy=startP.y()+rho1*momP.y();
         crossPz=startP.z()+rho1*momP.z();
         crossP.SetXYZ(crossPx,crossPy,crossPz);
      }

      Int_t mirInd;
      if(crossP.X()>0 && crossP.Y()>0) {
         mirInd = 0;
      } else if(crossP.X()<0 && crossP.Y()>0) {
         mirInd = 1;
      } else if(crossP.X()>0 && crossP.Y()<0) {
         mirInd = 2;
      } else if(crossP.X()<0 && crossP.Y()<0) {
         mirInd = 3;
      } else {
         continue;
      }
      centerP.SetXYZ(fXm[mirInd],fYm[mirInd],fZm[mirInd]); // mirror center

      // calculate normal on crosspoint with mirror
      TVector3 normP(crossP.x()-centerP.x(),crossP.y()-centerP.y(),crossP.z()-centerP.z());
      normP = normP.Unit();
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
            rho2 = (-TMath::Sin(fPhiDet)*(fDetX-crossP.x())
                  -TMath::Sin(fThetaDet)*TMath::Cos(fPhiDet)*(fDetY-crossP.y())
                  + TMath::Cos(fThetaDet)*TMath::Cos(fPhiDet)*(fDetZ-crossP.z()))/
                  (-TMath::Sin(fPhiDet)*refX-TMath::Sin(fThetaDet)*TMath::Cos(fPhiDet)*refY + TMath::Cos(fThetaDet)*TMath::Cos(fPhiDet)*refZ);
         }
         if (centerP.y() < 0){
            rho2 = (-TMath::Sin(fPhiDet)*(fDetX-crossP.x())
                  -TMath::Sin(-fThetaDet)*TMath::Cos(fPhiDet)*(+fDetY-crossP.y())
                  + TMath::Cos(-fThetaDet)*TMath::Cos(fPhiDet)*(fDetZ-crossP.z()))/
                  (-TMath::Sin(fPhiDet)*refX-TMath::Sin(+fThetaDet)*TMath::Cos(fPhiDet)*refY + TMath::Cos(-fThetaDet)*TMath::Cos(fPhiDet)*refZ);
         }

         // rho2 = -1*(crossP.z() - fDetZ)/refZ;    // only for theta = 0, phi=0
         xX = crossP.x() + refX * rho2;
         yY = crossP.y() + refY * rho2;
         zZ = crossP.z() + refZ * rho2;

         if (xX < 0) {
            if (centerP.y() > 0){
               rho2 = (-TMath::Sin(-fPhiDet)*(-fDetX-crossP.x())
                     -TMath::Sin(fThetaDet)*TMath::Cos(-fPhiDet)*(fDetY-crossP.y())
                     + TMath::Cos(fThetaDet)*TMath::Cos(-fPhiDet)*(fDetZ-crossP.z()))/
                     (-TMath::Sin(-fPhiDet)*refX-TMath::Sin(fThetaDet)*TMath::Cos(-fPhiDet)*refY + TMath::Cos(fThetaDet)*TMath::Cos(-fPhiDet)*refZ);
            }
            if (centerP.y() < 0){
               rho2 = (-TMath::Sin(-fPhiDet)*(-fDetX-crossP.x())
                     -TMath::Sin(-fThetaDet)*TMath::Cos(-fPhiDet)*(-fDetY-crossP.y())
                     + TMath::Cos(-fThetaDet)*TMath::Cos(-fPhiDet)*(fDetZ-crossP.z()))/
                     (-TMath::Sin(-fPhiDet)*refX-TMath::Sin(-fThetaDet)*TMath::Cos(-fPhiDet)*refY + TMath::Cos(-fThetaDet)*TMath::Cos(-fPhiDet)*refZ);
            }

            xX = crossP.x() + refX * rho2;
            yY = crossP.y() + refY * rho2;
            zZ = crossP.z() + refZ * rho2;
         }
         // Transform intersection point in same way as MCPoints were transformed in HitProducer before stored as Hit:
         Double_t xDet,yDet,zDet;
         const Double_t ptGlobal[] = {xX, yY, zZ};
         Double_t ptLocal[3];
         gGeoManager->FindNode(fDetX, fDetY, fDetZ);
         gGeoManager->MasterToLocal(ptGlobal, ptLocal);
         xDet = ptLocal[0];
         yDet = ptLocal[1] + fDetY;
         zDet = ptLocal[2];

         //check that crosspoint inside the plane
         if( xDet > (-fDetWidthX) && xDet < fDetWidthX) {
            if(yDet > (fDetY-fDetWidthY) && yDet < (fDetY+fDetWidthY)) {

               FairTrackParam richtrack(xDet,yDet,zDet,0.,0.,0.,covMat);
               * (FairTrackParam*)(richProj->At(j)) = richtrack;

            }
         }
      }
   }
}
