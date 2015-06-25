/**
* \file CbmRichHitProducer.cxx
*
* \author B. Polichtchouk
* \date 2004
**/
//____________________________________________________________________________________
// updated with QE from measurements, July 2012
// and AddCrossTalkHits() (implementation from Dima as in CBM-RICH simulation, May 2011)
// from LED measurements: fCrossTalkHitProb = 0.02 for H8500
// QE curves extrapolated to 180 nm using pol3 for clean and pol2 for WLS coated MAPMTs
//
//  0) det_type == 0
//     Hamamatsu H8500 with constant QE=0.99
//  1) det_type == 1
//     Protvino-type PMT
//  2) det_type == 2
//     Hamamatsu H8500 from data sheet
//  3) det_type == 3
//     CsI photocathode, NIM A 433 (1999) 201 (HADES)
//  4) det_type == 4
//     Hamamatsu H8500-03 from data sheet, with UV window
//  5) det_type == 5
//     Hamamatsu H8500 + WLS film with UV window, estimation
//  6) det_type == 6
//     Hamamatsu H8500-03 posF in CernOct2011 setup (BUW measurement)
//  10) det_type == 10
//      Hamamatsu H8500-03 posH with dipcoated WLS film (BUW measurement)
//  11) det_type == 11
//      Hamamatsu H8500-03 posH with dipcoated WLS film -3.8 % due to inhomogeneity (BUW measurement)
//  12) det_type == 12
//      Hamamatsu H8500-03 posD in CernOct2011 setup (BUW measurement)
//  13) det_type == 13
//      Hamamatsu R11265, average from ZN0590, ZN0593, ZN0731, ZN0733 (BUW measurement)
//  14) det_type == 14
//      Hamamatsu R11265, average from ZN0590, ZN0593, ZN0731, ZN0733 with dipcoated WLS film (BUW measurement)
//  15) det_type == 15
//      Hamamatsu H8500D-03, posC in CernOct2012 setup, DA0141 ,DA0142, DA0147, DA0154 (BUW measurement)
//  16) det_type == 16
//      Hamamatsu H8500D-03, posC in CernOct2012 setup, DA0141, DA0142, DA0147, DA0154 with dipcoated WLS film (BUW measurement)
//  17) det_type == 17
//      Hamamatsu H10966A-103, posE in CernOct2012 setup, ZL0003 (BUW measurement)
//  18) det_type == 18
//      Hamamatsu H10966A-103, posE in CernOct2012 setup, ZL0003 with dipcoated WLS film (BUW measurement)
//____________________________________________________________________________________

#include "CbmRichHitProducer.h"

#include "CbmRichPoint.h"
#include "CbmRichHit.h"
#include "CbmGeoRichPar.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"

#include "TGeoMatrix.h"
#include "TGeoBBox.h"

#include "TVector3.h"
#include "TRandom.h"
#include "TFormula.h"
#include "TString.h"
#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

const Double_t CbmRichHitProducer::c = 2.998E8; // speed of light
const Double_t CbmRichHitProducer::h = 6.626E-34; // Planck constant
const Double_t CbmRichHitProducer::e = 1.6022E-19; // elementary charge

CbmRichHitProducer::CbmRichHitProducer():
   FairTask("CbmRichHitProducer"),
   fRichPoints(NULL),
   fRichHits(NULL),
   fMcTracks(NULL),
   fNHits(0),
   fNDoubleHits(0),
   fGP(),
   fDetection(0),
   fNEvents(0),
   fPhotomulRadius(0.0),
   fPhotomulDist(0.),
   fDetType(4),
   fNofNoiseHits(220),
   fCollectionEfficiency(1.),
   fSigmaMirror(0.06),
   fCrossTalkHitProb(0.02),
   fNofCrossTalkHits(0)
{

}

CbmRichHitProducer::~CbmRichHitProducer()
{
   FairRootManager *manager =FairRootManager::Instance();
   manager->Write();
}

void CbmRichHitProducer::SetParContainers()
{
}

InitStatus CbmRichHitProducer::Init()
{
   fGP =  InitGeometry();

   fGP.Print();

   FairRootManager* manager = FairRootManager::Instance();

   fRichPoints = (TClonesArray*)manager->GetObject("RichPoint");
   if (NULL == fRichPoints) { Fatal("CbmRichHitProducer::Init","No RichPoint array!"); }

   fMcTracks = (TClonesArray *)manager->GetObject("MCTrack");
   if (NULL == fMcTracks) { Fatal("CbmRichHitProducer::Init","No MCTrack array!"); }

   fRichHits = new TClonesArray("CbmRichHit");
   manager->Register("RichHit","RICH", fRichHits, kTRUE);

   // Set photodetector parameters according to its type
   if (fDetType == 0){
      fPhotomulRadius = 0.;
      fPhotomulDist = 0.;
   }
   if (fDetType == 0 || fDetType == 2 || fDetType == 4 || fDetType == 5 || fDetType == 6 || fDetType == 10 || fDetType == 11 || fDetType == 12 || fDetType == 13 || fDetType == 14 || fDetType == 15 || fDetType == 16 || fDetType == 17 || fDetType == 18) {
      fPhotomulRadius = 0.6125;
      fPhotomulDist = 0.25; // corresponds to 2mm spacing
     // fCrossTalkHitProb = 0.02;
   }
   if (fDetType == 3) {
      fPhotomulRadius = 0.8;
      fPhotomulDist = 0.5;
   }

   return kSUCCESS;
}

CbmRichRecGeoPar CbmRichHitProducer::InitGeometry()
{
	FairRunAna* ana = FairRunAna::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	CbmGeoRichPar* par = (CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));
	TObjArray* sensNodes = par->GetGeoSensitiveNodes();
	TObjArray* passNodes = par->GetGeoPassiveNodes();

	if (sensNodes->GetEntriesFast() > 0 && passNodes->GetEntriesFast() > 0 ) {
		return InitAsciiGeometry();
	} else {

		return InitRootGeometry();
	}
}

CbmRichRecGeoPar CbmRichHitProducer::InitAsciiGeometry()
{
   cout << "-I- CbmRichHitProducer::InitAsciiGeometry" << endl;

   CbmRichRecGeoPar gp;

   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();
   CbmGeoRichPar* par = (CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));

   TObjArray* sensNodes = par->GetGeoSensitiveNodes();
   TObjArray* passNodes = par->GetGeoPassiveNodes();

   // get detector position:
   FairGeoNode *det= dynamic_cast<FairGeoNode*> (sensNodes->FindObject("rich1d#1"));
   if (NULL == det) cout << " -I no RICH Geo Node  found !!!!!  " << endl;
   FairGeoTransform* detTr=det->getLabTransform(); // detector position in labsystem
   FairGeoVector detPosLab=detTr->getTranslation(); // ... in cm
   FairGeoTransform detCen=det->getCenterPosition(); // center in Detector system
   FairGeoVector detPosCen=detCen.getTranslation();
   gp.fPmtZOrig = detPosLab.Z() + detPosCen.Z(); // z coordinate of photodetector (Labsystem, cm)
   gp.fPmtYOrig = detPosLab.Y() + detPosCen.Y(); // y coordinate of photodetector (Labsystem, cm)
   gp.fPmtXOrig = detPosLab.X() + detPosCen.X(); // x coordinate of photodetector (Labsystem, cm)

   TArrayD *fdetA=det->getParameters(); // get other geometry parameters: width in x, width in y, thickness
   gp.fPmtWidthX = fdetA->At(0);
   gp.fPmtWidthY = fdetA->At(1);
   for(Int_t i = 0; i < fdetA->GetSize(); i++) cout << "Array detector " << fdetA->At(i)<< endl;
   FairGeoRotation fdetR=detTr->getRotMatrix();

   // possible tilting around x-axis (theta) and y-axis (phi)
   // fdetR(0) = cos(phi)
   // fdetR(1) = 0
   // fdetR(2) = -sin(phi)
   // fdetR(3) = -sin(theta)sin(phi)
   // fdetR(4) = cos(theta)
   // fdetR(5) = -sin(theta)cos(phi)
   // fdetR(6) = cos(theta)sin(phi)
   // fdetR(7) = sin(theta)
   // fdetR(8) = cos(theta)cos(phi)

   gp.fPmtTheta = TMath::ASin(fdetR(7)); // tilting angle around x-axis
   gp.fPmtPhi = -1.*TMath::ASin(fdetR(2)); // tilting angle around y-axis

   // get refractive index of gas
   FairGeoNode *gas= dynamic_cast<FairGeoNode*> (passNodes->FindObject("rich1gas1"));
   if (NULL == gas) cout << " -I no RICH Geo Node  found !!!!!  " << endl;
   FairGeoMedium* med = gas->getMedium();

   Int_t npckov = med->getNpckov();
   Double_t cerpar[4];
   med->getCerenkovPar(0,cerpar);

   //   for(Int_t i=0;i<4;i++) {
   //      if (i==0) cout << " photon energy " << cerpar[0] << endl;
   //      if (i==1) cout << " absorption lenght" << cerpar[1] << endl;
   //      if (i==2) cout << " detection efficiency " << cerpar[2] << endl;
   //      if (i==3) cout << " refractive index (n-1)*10000 " << (cerpar[3]-1.)*10000. << endl;
   //   }

   gp.fNRefrac = cerpar[3];

   // transform nominal detector position (for tilted photodetector), x>0, y>0:
   gp.fPmtX = gp.fPmtXOrig*TMath::Cos(gp.fPmtPhi)+gp.fPmtZOrig*TMath::Sin(gp.fPmtPhi);
   gp.fPmtY = -gp.fPmtXOrig*TMath::Sin(gp.fPmtTheta)*TMath::Sin(gp.fPmtPhi) + gp.fPmtYOrig*TMath::Cos(gp.fPmtTheta) + gp.fPmtZOrig*TMath::Sin(gp.fPmtTheta)*TMath::Cos(gp.fPmtPhi);
   gp.fPmtZ = -gp.fPmtXOrig*TMath::Cos(gp.fPmtTheta)*TMath::Sin(gp.fPmtPhi) - gp.fPmtYOrig*TMath::Sin(gp.fPmtTheta) + gp.fPmtZOrig*TMath::Cos(gp.fPmtTheta)*TMath::Cos(gp.fPmtPhi);

   // get mirror position:
   //FairGeoNode *mir= (FairGeoNode *) fPassNodes->FindObject("rich1mgl#1");
   FairGeoNode *mir= (FairGeoNode *) sensNodes->FindObject("rich1mgl#1");
   FairGeoTransform* mirTr=mir->getLabTransform();  // position of mirror center in labsystem
   FairGeoVector mirPosLab=mirTr->getTranslation(); // ... in cm
   gp.fMirrorX = mirPosLab.X();
   gp.fMirrorY = mirPosLab.Y();
   gp.fMirrorZ = mirPosLab.Z();

   TArrayD *fmirA=mir->getParameters();  // get other geometry parameters: radius,
   gp.fMirrorR = fmirA->At(0); // mirror radius
//   Double_t spheTheta = TMath::Abs(90. - fmirA->At(2));   // opening angle for SPHERE in theta (90 degree +- theta)
//   Double_t sphePhi = TMath::Abs(90. - fmirA->At(4));   // opening angle for SPHERE in phi (90 degree +- phi)
//   // from that calculate (with safety factor 1.3) maximum x-y positions for track extrapolation:
//   fMaxXTrackExtr = 1.3*(fR*TMath::Tan(sphePhi*TMath::Pi()/180.));
//   fMaxYTrackExtr = 1.3*(TMath::Abs(fYm) + fR*TMath::Tan(spheTheta*TMath::Pi()/180.));

   // mirror might be rotated by theta around x-axis:
   // note that mirror is by default tilted by 90 degrees in order to get the necessary shape in GEANT
   // the "extra" tilting angle is then: fThetaM =  -1.*TMath::ASin(fmirR(5)) - TMath::Pi()/2.
   FairGeoRotation fmirR=mirTr->getRotMatrix();
   gp.fMirrorTheta = -1.*TMath::ASin(fmirR(5)) - TMath::Pi()/2 ;

   return gp;
}


CbmRichRecGeoPar CbmRichHitProducer::InitRootGeometry()
{
   cout << "-I- CbmRichHitProducer::InitRootGeometry" << endl;
   CbmRichRecGeoPar gp;

   TObjArray* nodesTop = gGeoManager->GetTopNode()->GetNodes();
   for (Int_t i1 = 0; i1 < nodesTop->GetEntriesFast(); i1++) {
      TGeoNode* richNode = (TGeoNode*) nodesTop->At(i1);
      if ( TString(richNode->GetName()).Contains("rich") ) {
         const Double_t *trRich = richNode->GetMatrix()->GetTranslation();
         TObjArray* nodes2 = richNode->GetNodes();
         for (Int_t i2 = 0; i2 < nodes2->GetEntriesFast(); i2++) {
            TGeoNode* gasNode = (TGeoNode*) nodes2->At(i2);
            if ( TString(gasNode->GetName()).Contains("RICH_gas") ) {
               const Double_t *trGas = gasNode->GetMatrix()->GetTranslation();
               //TODO: get refractive index from material
               gp.fNRefrac = 1.000446242;
               const TGeoMaterial* gasMaterial = gasNode->GetVolume()->GetMedium()->GetMaterial();

               TObjArray* nodes3 = gasNode->GetNodes();
               for (Int_t i3 = 0; i3 < nodes3->GetEntriesFast(); i3++) {
                  TGeoNode* pmtNode = (TGeoNode*) nodes3->At(i3);
                  if ( TString(pmtNode->GetName()).Contains("rich1d") ) {
                	 const Double_t *trPmt = pmtNode->GetMatrix()->GetTranslation();
                     const TGeoBBox* pmtShape = (const TGeoBBox*)(pmtNode->GetVolume()->GetShape());
                	 Double_t pmtX = trRich[0] + trGas[0] + trPmt[0];
                	 Double_t pmtY = trRich[1] + trGas[1] + trPmt[1];
                	 if (pmtX > 0. && pmtY > 0) {
						 const Double_t *rm = pmtNode->GetMatrix()->GetRotationMatrix();
						 gp.fPmtTheta = TMath::ASin(rm[7]); // tilting angle around x-axis
						 gp.fPmtPhi = -1.*TMath::ASin(rm[2]); // tilting angle around y-axis

						 gp.fPmtXOrig = pmtX;
						 gp.fPmtYOrig = pmtY;
						 gp.fPmtZOrig = trRich[2] + trGas[2] + trPmt[2] + pmtShape->GetDZ();

						 gp.fPmtWidthX = pmtShape->GetDX();
						 gp.fPmtWidthY = pmtShape->GetDY();

						 gp.fPmtX = gp.fPmtXOrig * TMath::Cos(gp.fPmtPhi) + gp.fPmtZOrig * TMath::Sin(gp.fPmtPhi);
						 gp.fPmtY = -gp.fPmtXOrig * TMath::Sin(gp.fPmtTheta) * TMath::Sin(gp.fPmtPhi) + gp.fPmtYOrig*TMath::Cos(gp.fPmtTheta) + gp.fPmtZOrig*TMath::Sin(gp.fPmtTheta)*TMath::Cos(gp.fPmtPhi);
						 gp.fPmtZ = -gp.fPmtXOrig * TMath::Cos(gp.fPmtTheta) * TMath::Sin(gp.fPmtPhi) - gp.fPmtYOrig*TMath::Sin(gp.fPmtTheta) + gp.fPmtZOrig*TMath::Cos(gp.fPmtTheta)*TMath::Cos(gp.fPmtPhi);

                	 }
                  }
               }
            } // if RICH_gas

            if ( TString(gasNode->GetName()).Contains("RICH_gas") ) {
               const Double_t *trGas = gasNode->GetMatrix()->GetTranslation();
               TObjArray* nodes3 = gasNode->GetNodes();
               for (Int_t i3 = 0; i3 < nodes3->GetEntriesFast(); i3++) {
                  TGeoNode* mirrorNode = (TGeoNode*) nodes3->At(i3);
                  if ( TString(mirrorNode->GetName()).Contains("RICH_mirror_half_total") ) {

                     const Double_t *rm = mirrorNode->GetMatrix()->GetRotationMatrix();
                     gp.fMirrorTheta = TMath::ASin(rm[3]); // tilting angle around x-axis
                     //gp.fPmtPhi = -1.*TMath::ASin(rm[2]); // tilting angle around y-axis

                     const Double_t *trMirror = mirrorNode->GetMatrix()->GetTranslation();
                     const TGeoBBox* mirrorShape = (const TGeoBBox*)(mirrorNode->GetVolume()->GetShape());

                     gp.fMirrorX = trRich[0] + trGas[0] + trMirror[0];
                     gp.fMirrorY = trRich[1] + trGas[1] + trMirror[1];
                     gp.fMirrorZ = trRich[2] + trGas[2] + trMirror[2];// + mirrorShape->GetDZ();
                     //TODO: set mirror radius form geometry
                     gp.fMirrorR = 300;
                  }
               }
            } // if RICH_gas_2
         }
      }// if rich
   }
   return gp;
}


void CbmRichHitProducer::Exec(
      Option_t* option)
{

   Int_t RichDetID = 0;

   fNEvents++;
   cout << "-I- CbmRichHitProducer, event no " << fNEvents << endl;

   // Set photodetector quantum efficiency
   Double_t lambda_min,lambda_max,lambda_step;
   Double_t efficiency[100];
   SetPhotoDetPar(fDetType,lambda_min,lambda_max,lambda_step,efficiency);

   fRichHits->Clear();
   fNHits = 0;
   fNDoubleHits = 0;

   for(Int_t j = 0; j < fRichPoints->GetEntries(); j++){
      CbmRichPoint* pt = (CbmRichPoint*) fRichPoints->At(j);

      TVector3 posPoint;
      pt->Position(posPoint);
      TVector3 detPoint;
      TiltPoint(&posPoint, &detPoint, fGP.fPmtPhi, fGP.fPmtTheta, fGP.fPmtZOrig);

      if (fVerbose > 1) cout << " position in Labsystem " << posPoint.X() << " " << posPoint.Y() << " " << posPoint.Z() << endl;
      if (fVerbose > 1) cout << " tilted position in Labsystem " << detPoint.X() << " " << detPoint.Y() << " " << detPoint.Z() << endl;

      Int_t trackID = pt->GetTrackID();
      CbmMCTrack* p = (CbmMCTrack*) fMcTracks->At(trackID);
      Int_t gcode = TMath::Abs(p->GetPdgCode());

      if ((fVerbose) && ((detPoint.Z() < (fGP.fPmtZ-0.25)) || (detPoint.Z() > (fGP.fPmtZ+0.25)))) {
        // cout << " not at " << fGP.fPmtZ << " but " << detPoint.Z() << endl;
      }

      // hit position as a center of PMT
      Double_t xHit, yHit;
      Int_t pmtID;
      Double_t sigma_0 = 0.09;     // 0.09 for standard MAPMT H8500D-03
      Double_t sigma_wls = 0.164; // sigma (cm) for additional smearing of HitPosition due to WLS film
				  // depending on det_type
				  // 0.164 for standard MAPMT H8500D-03 with WLS coateding

      // FindRichHitPosition
      if (fDetType == 0) {
         xHit = detPoint.X();
         yHit = detPoint.Y();
         pmtID = j;
      }
      if (fDetType == 1) FindRichHitPositionSinglePMT(detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);
      if (fDetType == 0 || fDetType == 2 || fDetType == 4 || fDetType == 5 || fDetType == 6 || fDetType == 10 || fDetType == 11 || fDetType == 12 || fDetType == 13 || fDetType == 14 || fDetType == 15 || fDetType == 16 || fDetType == 17 || fDetType == 18) FindRichHitPositionMAPMT(sigma_0,detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);
      if (fDetType == 3) FindRichHitPositionCsI(detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);

      //Double_t zHit = detPoint.Z();
      Double_t zHit = fGP.fPmtZ; // fix z-position to nominal value: either tilted (fDetZ = zDet) or untilted (fDetZ_org)
      //TVector3 posHit(xHit,yHit,zHit);

      //error of hit position at the moment nothing better than +-tube_radius
      TVector3 posHitErr(fPhotomulRadius,fPhotomulRadius,0.);

      // add Hit: Hit assigned only if xHit and yHit != 0
      if (xHit != 0.0 && yHit != 0.0) {
         if (fDetType == 1) {
            if (fVerbose)
               if (TMath::Sqrt((detPoint.X()-xHit)*(detPoint.X()-xHit)+(detPoint.Y()-yHit)*(detPoint.Y()-yHit)) > (fPhotomulRadius+fPhotomulDist)*1.5) {
                 // cout << "-E- RichHitProducer: wrongly assigned Hits (distance point-hit too large)!" << endl;
               }
         }
         if (fDetType == 0 || fDetType == 2 || fDetType == 3 || fDetType == 4 || fDetType == 6 || fDetType == 12 || fDetType == 13 || fDetType == 14 || fDetType == 15 || fDetType == 16 || fDetType == 17 || fDetType == 18) {
            if (fVerbose)
               if (TMath::Abs(detPoint.X()-xHit) > fPhotomulRadius || TMath::Abs(detPoint.Y()-yHit) > fPhotomulRadius*1.5){
                 // cout << "-E- RichHitProducer: wrongly assigned Hits (distance point-hit too large)! " <<
                 // detPoint.X() << " " << xHit << " " << detPoint.Y() << " " << yHit << endl;
               }
         }
         if (fDetType == 5 || fDetType == 10 || fDetType == 11 || fDetType == 14 || fDetType == 16 || fDetType == 18) { // fDetType 5: additional smearing with RMS=3mm due to WLS film
            if (fVerbose)
               if (TMath::Abs(detPoint.X()-xHit) > fPhotomulRadius+1.5 || TMath::Abs(detPoint.Y()-yHit) > fPhotomulRadius*1.5){
                 // cout << "-E- RichHitProducer: wrongly assigned Hits ? (distance point-hit too large)! " <<
                 // detPoint.X() << " " << xHit << " " << detPoint.Y() << " " << yHit << endl;
               }
         }

         if (gcode == 50000050) {
            // for photons weight with efficiency of PMT
            TVector3 mom;
            pt->Momentum(mom);
            Double_t etot = sqrt(mom.Px()*mom.Px() + mom.Py()*mom.Py() + mom.Pz()*mom.Pz());
            Double_t lambda=c/fGP.fNRefrac*h/e/etot;// wavelength in nm
            fDetection=0;
            if (lambda >= lambda_min && lambda < lambda_max) {
               Int_t ilambda=(Int_t)((lambda-lambda_min)/lambda_step);
               Double_t rand = gRandom->Rndm();
               fDetection = 0;
               if ( (fDetType == 5 || fDetType == 10 || fDetType == 11 || fDetType == 14 || fDetType == 16 || fDetType == 18 ) && lambda < 300.) {// smear Hit position for lambda < 300 nm (WLS film!)
                  FindRichHitPositionMAPMT(sigma_wls,detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);
               }
               if (efficiency[ilambda]*fCollectionEfficiency > rand ) fDetection = 1;
            } // min <= lambda < max
         }// if photon

         // detection efficiency for hadrons crossing the PMT-cathodes?
         // else if (gcode ==  211) detection = 1;  //pi+-
         // else if (gcode ==  321) detection = 1;  //K+-
         // else if (gcode == 2212) detection = 1;  //p+-
         else { // if not photon
            // worst case: assume that all charged particles crossing
            // the PMTplane leave Cherenkov light in the PMTglass which will be detected
            fDetection=1;
         }

         TVector3 posHit(xHit,yHit,zHit);

         if (fDetection == 1){
            Int_t address = pt->GetDetectorID();
            if (RichDetID == 0) RichDetID = address;
            if (RichDetID != address)
               cout << "-E- RichDetID changed from " << RichDetID << " to " << address << endl;
            Double_t ampl = GetAmplitude();
            AddHit(posHit,posHitErr,address,pmtID,ampl,j);

            AddCrossTalkHits(posHit.X(), posHit.Y(), j, RichDetID);
         } // photon detected?
      } // xHit and yHit != 0

      if (fVerbose > 2) {
         cout << " iHit, Point-x, Point-y, Hit-x, Hiy-y, detected, PMT? " << j << " "
               << posPoint.X() << " " << posPoint.Y() << " " << xHit << " "
               << yHit << " " << fDetection << " " << pmtID << endl;
      }
   } // loop over RICH points

   // add noise hits
   for(Int_t j = 0; j < fNofNoiseHits; j++) {
      Double_t rand = gRandom->Rndm();
      Double_t xRand = (fGP.fPmtX-fGP.fPmtZOrig*TMath::Sin(fGP.fPmtPhi))-fGP.fPmtWidthX + rand*2.*fGP.fPmtWidthX;
      rand = gRandom->Rndm();
      if (rand < 0.5 ) xRand = -1.*xRand;
      rand = gRandom->Rndm();
      Double_t yRand = fGP.fPmtY-fGP.fPmtWidthY + rand*2.*fGP.fPmtWidthY;
      rand = gRandom->Rndm();
      if (rand < 0.5 ) yRand = -1.*yRand;

      Double_t xHit, yHit;
      Int_t pmtID;

      //FindRichHitPosition
      if (fDetType == 0) {
         xHit = xRand;
         yHit = yRand;
         pmtID = -j;
      }
      if (fDetType == 1) FindRichHitPositionSinglePMT(xRand,yRand,xHit,yHit,pmtID);
      if (fDetType == 0 || fDetType == 2 || fDetType == 4 || fDetType == 5 || fDetType == 6 || fDetType == 10 || fDetType == 11 || fDetType == 12 || fDetType == 13 || fDetType == 14 || fDetType == 15 || fDetType == 16 || fDetType == 17 || fDetType == 18) FindRichHitPositionMAPMT(0,xRand,yRand,xHit,yHit,pmtID);
      if (fDetType == 3) FindRichHitPositionCsI(xRand,yRand,xHit,yHit,pmtID);

      // add Hit
      if (xHit!=0.0 && yHit!=0.0) {
         Double_t zHit = fGP.fPmtZ;
         TVector3 posHit(xHit,yHit,zHit);
         Double_t ampl = GetAmplitude();

         //error of hit position
         //at the moment nothing better than +-tube_radius
         TVector3 posHitErr(fPhotomulRadius,fPhotomulRadius,0.);

         AddHit(posHit,posHitErr,RichDetID,pmtID,ampl,-1);
      }
   }

   cout << "Nof hits: "<< fRichHits->GetEntries()<< endl;
   cout << "Fraction of double hits: "<<(Double_t)(fNDoubleHits)/(Double_t)(fNHits) << endl;
   cout << "Nof crosstalk hits: " << (Double_t) fNofCrossTalkHits / fNEvents << endl;
}

void CbmRichHitProducer::TiltPoint(
      TVector3 *inPos,
      TVector3 *outPos,
      Double_t phi,
      Double_t theta,
      Double_t detZOrig,
      Bool_t noTilting)
{
   if (noTilting == false){
      Double_t xDet,yDet,zDet;

      if (inPos->X() > 0 && inPos->Y() > 0) {
         xDet = inPos->X()*TMath::Cos(phi) + inPos->Z()*TMath::Sin(phi) - detZOrig*TMath::Sin(phi);
         yDet = -inPos->X()*TMath::Sin(theta)*TMath::Sin(phi) + inPos->Y()*TMath::Cos(theta) + inPos->Z()*TMath::Sin(theta)*TMath::Cos(phi);
         zDet = -inPos->X()*TMath::Cos(theta)*TMath::Sin(phi) - inPos->Y()*TMath::Sin(theta) + inPos->Z()*TMath::Cos(theta)*TMath::Cos(phi);
      }
      if (inPos->X() > 0 && inPos->Y() < 0) {
         xDet = inPos->X()*TMath::Cos(phi) + inPos->Z()*TMath::Sin(phi) - detZOrig*TMath::Sin(phi);
         yDet = inPos->X()*TMath::Sin(theta)*TMath::Sin(phi) + inPos->Y()*TMath::Cos(theta) - inPos->Z()*TMath::Sin(theta)*TMath::Cos(phi);
         zDet = -inPos->X()*TMath::Cos(theta)*TMath::Sin(phi) + inPos->Y()*TMath::Sin(theta) + inPos->Z()*TMath::Cos(theta)*TMath::Cos(phi);
      }
      if (inPos->X() < 0 && inPos->Y() < 0) {
         xDet = inPos->X()*TMath::Cos(phi) - inPos->Z()*TMath::Sin(phi) + detZOrig*TMath::Sin(phi);
         yDet = -inPos->X()*TMath::Sin(theta)*TMath::Sin(phi) + inPos->Y()*TMath::Cos(theta) - inPos->Z()*TMath::Sin(theta)*TMath::Cos(phi);
         zDet = inPos->X()*TMath::Cos(theta)*TMath::Sin(phi) + inPos->Y()*TMath::Sin(theta) + inPos->Z()*TMath::Cos(theta)*TMath::Cos(phi);
      }
      if (inPos->X() < 0 && inPos->Y() > 0) {
         xDet = inPos->X()*TMath::Cos(phi) - inPos->Z()*TMath::Sin(phi) + detZOrig*TMath::Sin(phi);
         yDet = inPos->X()*TMath::Sin(theta)*TMath::Sin(phi) + inPos->Y()*TMath::Cos(theta) + inPos->Z()*TMath::Sin(theta)*TMath::Cos(phi);
         zDet = inPos->X()*TMath::Cos(theta)*TMath::Sin(phi) - inPos->Y()*TMath::Sin(theta) + inPos->Z()*TMath::Cos(theta)*TMath::Cos(phi);
      }
      outPos->SetXYZ(xDet,yDet,zDet);
   } else {
      outPos->SetXYZ(inPos->X(), inPos->Y(), inPos->Z());
   }
}


void CbmRichHitProducer::AddHit(
      TVector3 &posHit,
      TVector3 &posHitErr,
		Int_t address,
		Int_t pmtID,
		Double_t ampl,
		Int_t index)
{
   // Add this hit to existing one, if in the same PMT,
   // otherwise create a new one.
   Bool_t hitMerged = kFALSE;
   CbmRichHit *hit;
   // Check if there was any hit in the same PMT
   for (Int_t iHit = 0; iHit < fNHits; iHit++) {
      hit = (CbmRichHit*) fRichHits->At(iHit);
      if (pmtID == hit->GetPmtId() && address==hit->GetAddress()) {
         hit->SetNPhotons(hit->GetNPhotons()+1);
         hit->SetAmplitude(GetAmplitude()+ampl);
         hitMerged = kTRUE;
         fNDoubleHits++;
         break;
      }
   }

   // If no hits found in this PMT, add a new one
   if (!hitMerged) {
      new((*fRichHits)[fNHits]) CbmRichHit();
      hit = (CbmRichHit*)fRichHits->At(fNHits);
      hit->SetPosition(posHit);
      hit->SetPositionError(posHitErr);
      hit->SetAddress(address);
      hit->SetPmtId(pmtID);
      hit->SetNPhotons(1);
      hit->SetAmplitude(GetAmplitude());
      hit->SetRefId(index);
      fNHits++;
   }
}

void CbmRichHitProducer::AddCrossTalkHits(
      Double_t x,
      Double_t y,
      Int_t pointInd,
      Int_t RichDetID)
{
   // only for MAMPT
   if (fDetType != 0 && fDetType != 2 && fDetType != 4 && fDetType != 5 && fDetType != 6 && fDetType != 10 && fDetType != 11 && fDetType != 12 && fDetType != 13 && fDetType != 14 && fDetType != 15 && fDetType != 16 && fDetType != 17 && fDetType != 18) return;

   Double_t xHit = 0.0, yHit = 0.0;
   Int_t pmtID = -1;

   Double_t r = fPhotomulRadius;

   // closest neighbors
   Double_t rand = gRandom->Rndm();
   if (rand < fCrossTalkHitProb && xHit == 0.0 && yHit == 0.0 && pmtID == -1)
      FindRichHitPositionMAPMT(0., x + r, y, xHit, yHit, pmtID);

   rand = gRandom->Rndm();
   if (rand < fCrossTalkHitProb && xHit == 0.0 && yHit == 0.0 && pmtID == -1)
      FindRichHitPositionMAPMT(0., x - r, y, xHit, yHit, pmtID);

   rand = gRandom->Rndm();
   if (rand < fCrossTalkHitProb && xHit == 0.0 && yHit == 0.0 && pmtID == -1)
      FindRichHitPositionMAPMT(0., x, y + r, xHit, yHit, pmtID);

   rand = gRandom->Rndm();
   if (rand < fCrossTalkHitProb && xHit == 0.0 && yHit == 0.0 && pmtID == -1)
      FindRichHitPositionMAPMT(0., x, y - r, xHit, yHit, pmtID);

   // diagonal neighbors
   rand = gRandom->Rndm();
   if (rand < fCrossTalkHitProb / 4. && xHit == 0.0 && yHit == 0.0 && pmtID == -1)
      FindRichHitPositionMAPMT(0., x + r, y + r, xHit, yHit, pmtID);

   rand = gRandom->Rndm();
   if (rand < fCrossTalkHitProb / 4. && xHit == 0.0 && yHit == 0.0 && pmtID == -1)
      FindRichHitPositionMAPMT(0., x - r, y - r, xHit, yHit, pmtID);

   rand = gRandom->Rndm();
   if (rand < fCrossTalkHitProb / 4. && xHit == 0.0 && yHit == 0.0 && pmtID == -1)
      FindRichHitPositionMAPMT(0., x - r, y + r, xHit, yHit, pmtID);

   rand = gRandom->Rndm();
   if (rand < fCrossTalkHitProb / 4.&& xHit == 0.0 && yHit == 0.0 && pmtID == -1)
      FindRichHitPositionMAPMT(0., x + r, y - r, xHit, yHit, pmtID);


   if (xHit != 0.0 && yHit != 0.0) {
      Double_t zHit = fGP.fPmtZ;
      TVector3 posHit(xHit,yHit,zHit);
      Double_t ampl = GetAmplitude();
      TVector3 posHitErr(fPhotomulRadius,fPhotomulRadius,0.);

      AddHit(posHit, posHitErr, RichDetID, pmtID, ampl, pointInd);
      fNofCrossTalkHits++;
   }
}

void CbmRichHitProducer::Finish()
{
  fRichHits->Clear();
}

void CbmRichHitProducer::SetPhotoDetPar(
      Int_t det_type,
      Double_t& fLambdaMin,
      Double_t& fLambdaMax,
      Double_t& fLambdaStep,
      Double_t fEfficiency[])
{
   // gives parameters for a chosen photodetector type
   if (fVerbose > 0) cout << "SetPhotoDetPar routine called for PMT type " << fDetType << endl;

   if (det_type == 1){
      // PMT efficiencies for Protvino-type PMT
      // corresponding range in lambda: (100nm)120nm - 700nm in steps of 20nm

      fLambdaMin = 120.;
      fLambdaMax = 700.;
      fLambdaStep = 20.;

      fEfficiency[0] = 0.216;
      fEfficiency[1] = 0.216;
      fEfficiency[2] = 0.216;
      fEfficiency[3] = 0.216;
      fEfficiency[4] = 0.216;
      fEfficiency[5] = 0.216;
      fEfficiency[6] = 0.216;
      fEfficiency[7] = 0.216;
      fEfficiency[8] = 0.216;
      fEfficiency[9] = 0.216;
      fEfficiency[10] = 0.216;
      fEfficiency[11] = 0.227;
      fEfficiency[12] = 0.23;
      fEfficiency[13] = 0.227;
      fEfficiency[14] = 0.216;
      fEfficiency[15] = 0.2;
      fEfficiency[16] = 0.176;
      fEfficiency[17] = 0.15;
      fEfficiency[18] = 0.138;
      fEfficiency[19] = 0.1;
      fEfficiency[20] = 0.082;
      fEfficiency[21] = 0.06;
      fEfficiency[22] = 0.044;
      fEfficiency[23] = 0.032;
      fEfficiency[24] = 0.022;
      fEfficiency[25] = 0.015;
      fEfficiency[26] = 0.01;
      fEfficiency[27] = 0.006;
      fEfficiency[28] = 0.004;

      fLambdaMin = 100.;
      fLambdaMax = 700.;
      fLambdaStep = 20.;

//      fEfficiency[0] = 0.216;
//      fEfficiency[1] = 0.216;
//      fEfficiency[2] = 0.216;
//      fEfficiency[3] = 0.216;
//      fEfficiency[4] = 0.216;
//      fEfficiency[5] = 0.216;
//      fEfficiency[6] = 0.216;
//      fEfficiency[7] = 0.216;
//      fEfficiency[8] = 0.216;
//      fEfficiency[9] = 0.216;
//      fEfficiency[10] = 0.216;
//      fEfficiency[11] = 0.216;
//      fEfficiency[12] = 0.227;
//      fEfficiency[13] = 0.23;
//      fEfficiency[14] = 0.227;
//      fEfficiency[15] = 0.216;
//      fEfficiency[16] = 0.2;
//      fEfficiency[17] = 0.176;
//      fEfficiency[18] = 0.15;
//      fEfficiency[19] = 0.138;
//      fEfficiency[20] = 0.1;
//      fEfficiency[21] = 0.082;
//      fEfficiency[22] = 0.06;
//      fEfficiency[23] = 0.044;
//      fEfficiency[24] = 0.032;
//      fEfficiency[25] = 0.022;
//      fEfficiency[26] = 0.015;
//      fEfficiency[27] = 0.01;
//      fEfficiency[28] = 0.006;
//      fEfficiency[29] = 0.004;
   } else if (det_type == 3){
      // quantum efficiency for CsI photocathode
      // approximately read off from fig.3 in NIM A 433 (1999) 201 (HADES)

      fLambdaMin = 130.;
      fLambdaMax = 210.;
      fLambdaStep = 10.;

      fEfficiency[0] = 0.45;
      fEfficiency[1] = 0.4;
      fEfficiency[2] = 0.35;
      fEfficiency[3] = 0.32;
      fEfficiency[4] = 0.25;
      fEfficiency[5] = 0.2;
      fEfficiency[6] = 0.1;
      fEfficiency[7] = 0.03;

   } else if (det_type == 2){
      // PMT efficiencies for Hamamatsu H8500
      // (Flat type Multianode Photomultiplier)
      // corresponding range in lambda: 260nm - 740nm in steps of 20nm

      fLambdaMin = 260.;
      fLambdaMax = 740.;
      fLambdaStep = 20.;

      fEfficiency[0] = 0.06;
      fEfficiency[1] = 0.12;
      fEfficiency[2] = 0.2;
      fEfficiency[3] = 0.22;
      fEfficiency[4] = 0.22;
      fEfficiency[5] = 0.22;
      fEfficiency[6] = 0.21;
      fEfficiency[7] = 0.2;
      fEfficiency[8] = 0.18;
      fEfficiency[9] = 0.16;
      fEfficiency[10] = 0.14;
      fEfficiency[11] = 0.11;
      fEfficiency[12] = 0.1;
      fEfficiency[13] = 0.06;
      fEfficiency[14] = 0.047;
      fEfficiency[15] = 0.03;
      fEfficiency[16] = 0.021;
      fEfficiency[17] = 0.012;
      fEfficiency[18] = 0.006;
      fEfficiency[19] = 0.0023;
      fEfficiency[20] = 0.0008;
      fEfficiency[21] = 0.00022;
      fEfficiency[22] = 0.00007;
      fEfficiency[23] = 0.00002;

   } else if (det_type == 4){
      // PMT efficiencies for Hamamatsu H8500-03
      // (Flat type Multianode Photomultiplier with UV window)
      // corresponding range in lambda: 200nm - 640nm in steps of 20nm

      fLambdaMin = 200.;
      fLambdaMax = 640.;
      fLambdaStep = 20.;

      fEfficiency[0] = 0.095;
      fEfficiency[1] = 0.13;
      fEfficiency[2] = 0.16;
      fEfficiency[3] = 0.2;
      fEfficiency[4] = 0.23;
      fEfficiency[5] = 0.24;
      fEfficiency[6] = 0.25;
      fEfficiency[7] = 0.25;
      fEfficiency[8] = 0.24;
      fEfficiency[9] = 0.24;
      fEfficiency[10] = 0.23;
      fEfficiency[11] = 0.22;
      fEfficiency[12] = 0.2;
      fEfficiency[13] = 0.16;
      fEfficiency[14] = 0.14;
      fEfficiency[15] = 0.1;
      fEfficiency[16] = 0.065;
      fEfficiency[17] = 0.045;
      fEfficiency[18] = 0.02;
      fEfficiency[19] = 0.017;
      fEfficiency[20] = 0.007;
      fEfficiency[21] = 0.0033;
   } else if (det_type == 5){
      // PMT efficiencies for Hamamatsu H8500 + WLS film
      // (Flat type Multianode Photomultiplier with UV window)
      // corresponding range in lambda: 150nm - 650nm in steps of 20nm

      fLambdaMin = 160.;
      fLambdaMax = 640.;
      fLambdaStep = 20.;

      fEfficiency[0] = 0.1;
      fEfficiency[1] = 0.2;
      fEfficiency[2] = 0.2;
      fEfficiency[3] = 0.2;
      fEfficiency[4] = 0.2;
      fEfficiency[5] = 0.2;
      fEfficiency[6] = 0.23;
      fEfficiency[7] = 0.24;
      fEfficiency[8] = 0.25;
      fEfficiency[9] = 0.25;
      fEfficiency[10] = 0.24;
      fEfficiency[11] = 0.24;
      fEfficiency[12] = 0.23;
      fEfficiency[13] = 0.22;
      fEfficiency[14] = 0.2;
      fEfficiency[15] = 0.16;
      fEfficiency[16] = 0.14;
      fEfficiency[17] = 0.1;
      fEfficiency[18] = 0.065;
      fEfficiency[19] = 0.045;
      fEfficiency[20] = 0.02;
      fEfficiency[21] = 0.017;
      fEfficiency[22] = 0.007;
      fEfficiency[23] = 0.0033;
      }

// -------------------------------------------------------------------------------------------
//   QE measured at Wuppertal University (BUW), spring 2011
// -------------------------------------------------------------------------------------------

    else if(det_type == 6) {
       /**  Measured PMT efficiencies for MAPMTs (H8500D-03) at posF (BUW measurement) ##### CernOct2011 #####
	    (Flat type Multianode Photomultiplier with BA cathode + UV window)
	    corresponding range in lambda: 160nm  - 700nm in steps of 10nm */

       fLambdaMin = 160.;
       fLambdaMax = 700.;
       fLambdaStep = 10.;
	   
       fEfficiency[0] = 0.0;
       fEfficiency[1] = 0.0;
       fEfficiency[2] = 0.0324;
       fEfficiency[3] = 0.0586;
       fEfficiency[4] = 0.0945;
       fEfficiency[5] = 0.1061;
       fEfficiency[6] = 0.1265;
       fEfficiency[7] = 0.1482;
       fEfficiency[8] = 0.1668;
       fEfficiency[9] = 0.1887;
       fEfficiency[10] = 0.2093;
       fEfficiency[11] = 0.2134;
       fEfficiency[12] = 0.2303;
       fEfficiency[13] = 0.2482;
       fEfficiency[14] = 0.2601;
       fEfficiency[15] = 0.2659;
       fEfficiency[16] = 0.2702;
       fEfficiency[17] = 0.283;
       fEfficiency[18] = 0.2863;
       fEfficiency[19] = 0.2863;
       fEfficiency[20] = 0.2884;
       fEfficiency[21] = 0.286;
       fEfficiency[22] = 0.2811;
       fEfficiency[23] = 0.2802;
       fEfficiency[24] = 0.272;
       fEfficiency[25] = 0.2638;
       fEfficiency[26] = 0.2562;
       fEfficiency[27] = 0.2472;
       fEfficiency[28] = 0.2368;
       fEfficiency[29] = 0.2218;
       fEfficiency[30] = 0.2032;
       fEfficiency[31] = 0.186;
       fEfficiency[32] = 0.1735;
       fEfficiency[33] = 0.1661;
       fEfficiency[34] = 0.1483;
       fEfficiency[35] = 0.121;
       fEfficiency[36] = 0.0959;
       fEfficiency[37] = 0.0782;
       fEfficiency[38] = 0.0647;
       fEfficiency[39] = 0.0538;
       fEfficiency[40] = 0.0372;
       fEfficiency[41] = 0.0296;
       fEfficiency[42] = 0.0237;
       fEfficiency[43] = 0.0176;
       fEfficiency[44] = 0.0123;
       fEfficiency[45] = 0.0083;
       fEfficiency[46] = 0.005;
       fEfficiency[47] = 0.003;
       fEfficiency[48] = 0.0017;
       fEfficiency[49] = 0.0008;
       fEfficiency[50] = 0.0006;
       fEfficiency[51] = 0.0003;
       fEfficiency[52] = 0.0003;
       fEfficiency[53] = 0.0002;
       fEfficiency[54] = 0.0001;
       
       }

 else if(det_type == 10) {
       /**  Measured PMT efficiencies for MAPMTs (H8500D-03) at posH (BUW measurement) --dipcoated WLS film -- ##### CernOct2011 #####
	    (Flat type Multianode Photomultiplier with UV window)
	    corresponding range in lambda: 180nm  - 640nm in steps of 10nm */

       fLambdaMin = 180.;
       fLambdaMax = 640.;
       fLambdaStep = 10.;
	   
       fEfficiency[0] = 0.178;
       fEfficiency[1] = 0.200;
       fEfficiency[2] = 0.218;
       fEfficiency[3] = 0.222;
       fEfficiency[4] = 0.226;
       fEfficiency[5] = 0.228;
       fEfficiency[6] = 0.214;
       fEfficiency[7] = 0.210;
       fEfficiency[8] = 0.229;
       fEfficiency[9] = 0.231;
       fEfficiency[10] = 0.244;
       fEfficiency[11] = 0.253;
       fEfficiency[12] = 0.259;
       fEfficiency[13] = 0.263;
       fEfficiency[14] = 0.266;
       fEfficiency[15] = 0.277;
       fEfficiency[16] = 0.280;
       fEfficiency[17] = 0.274;
       fEfficiency[18] = 0.275;
       fEfficiency[19] = 0.270;
       fEfficiency[20] = 0.264;
       fEfficiency[21] = 0.263;
       fEfficiency[22] = 0.254;
       fEfficiency[23] = 0.246;
       fEfficiency[24] = 0.239;
       fEfficiency[25] = 0.229;
       fEfficiency[26] = 0.219;
       fEfficiency[27] = 0.207;
       fEfficiency[28] = 0.193;
       fEfficiency[29] = 0.179;
       fEfficiency[30] = 0.161;
       fEfficiency[31] = 0.149;
       fEfficiency[32] = 0.135;
       fEfficiency[33] = 0.117;
       fEfficiency[34] = 0.103;
       fEfficiency[35] = 0.082;
       fEfficiency[36] = 0.065;
       fEfficiency[37] = 0.056;
       fEfficiency[38] = 0.036;
       fEfficiency[39] = 0.030;
       fEfficiency[40] = 0.024;
       fEfficiency[41] = 0.018;
       fEfficiency[42] = 0.013;
       fEfficiency[43] = 0.009;
       fEfficiency[44] = 0.006;
       fEfficiency[45] = 0.004;
       fEfficiency[46] = 0.002;
     }

 else if(det_type == 11) {
       /**  Measured PMT efficiencies for MAPMTs (H8500D-03) at posH (BUW measurement) --dipcoated WLS film -- ##### CernOct2011 #####
	    (Flat type Multianode Photomultiplier with UV window)		   ##### -3.8 % due to inhomogeneity #####
	    corresponding range in lambda: 180nm  - 640nm in steps of 10nm */

       fLambdaMin = 180.;
       fLambdaMax = 640.;
       fLambdaStep = 10.;
	   
       fEfficiency[0] = 0.202;
       fEfficiency[1] = 0.207;
       fEfficiency[2] = 0.210;
       fEfficiency[3] = 0.214;
       fEfficiency[4] = 0.218;
       fEfficiency[5] = 0.219;
       fEfficiency[6] = 0.206;
       fEfficiency[7] = 0.202;
       fEfficiency[8] = 0.220;
       fEfficiency[9] = 0.222;
       fEfficiency[10] = 0.235;
       fEfficiency[11] = 0.243;
       fEfficiency[12] = 0.249;
       fEfficiency[13] = 0.253;
       fEfficiency[14] = 0.256;
       fEfficiency[15] = 0.266;
       fEfficiency[16] = 0.270;
       fEfficiency[17] = 0.264;
       fEfficiency[18] = 0.265;
       fEfficiency[19] = 0.260;
       fEfficiency[20] = 0.254;
       fEfficiency[21] = 0.253;
       fEfficiency[22] = 0.244;
       fEfficiency[23] = 0.237;
       fEfficiency[24] = 0.229;
       fEfficiency[25] = 0.221;
       fEfficiency[26] = 0.210;
       fEfficiency[27] = 0.199;
       fEfficiency[28] = 0.186;
       fEfficiency[29] = 0.172;
       fEfficiency[30] = 0.155;
       fEfficiency[31] = 0.143;
       fEfficiency[32] = 0.129;
       fEfficiency[33] = 0.113;
       fEfficiency[34] = 0.099;
       fEfficiency[35] = 0.079;
       fEfficiency[36] = 0.063;
       fEfficiency[37] = 0.054;
       fEfficiency[38] = 0.035;
       fEfficiency[39] = 0.028;
       fEfficiency[40] = 0.023;
       fEfficiency[41] = 0.018;
       fEfficiency[42] = 0.013;
       fEfficiency[43] = 0.009;
       fEfficiency[44] = 0.006;
       fEfficiency[45] = 0.004;
       fEfficiency[46] = 0.002;
     }

 else if(det_type == 12) {
       /**  Measured PMT efficiencies for MAPMTs (H8500D-03) at posD (BUW measurement) ##### CernOct2011 #####
	    (Flat type Multianode Photomultiplier with UV window)
	    corresponding range in lambda: 180nm  - 640nm in steps of 10nm */

       fLambdaMin = 180.;
       fLambdaMax = 640.;
       fLambdaStep = 10.;
	   
       fEfficiency[0] = 0.060;
       fEfficiency[1] = 0.080;
       fEfficiency[2] = 0.096;
       fEfficiency[3] = 0.109;
       fEfficiency[4] = 0.130;
       fEfficiency[5] = 0.152;
       fEfficiency[6] = 0.172;
       fEfficiency[7] = 0.194;
       fEfficiency[8] = 0.214;
       fEfficiency[9] = 0.218;
       fEfficiency[10] = 0.235;
       fEfficiency[11] = 0.253;
       fEfficiency[12] = 0.265;
       fEfficiency[13] = 0.271;
       fEfficiency[14] = 0.275;
       fEfficiency[15] = 0.288;
       fEfficiency[16] = 0.291;
       fEfficiency[17] = 0.292;
       fEfficiency[18] = 0.294;
       fEfficiency[19] = 0.292;
       fEfficiency[20] = 0.287;
       fEfficiency[21] = 0.286;
       fEfficiency[22] = 0.278;
       fEfficiency[23] = 0.269;
       fEfficiency[24] = 0.262;
       fEfficiency[25] = 0.252;
       fEfficiency[26] = 0.242;
       fEfficiency[27] = 0.227;
       fEfficiency[28] = 0.208;
       fEfficiency[29] = 0.178;
       fEfficiency[30] = 0.170;
       fEfficiency[31] = 0.155;
       fEfficiency[32] = 0.129;
       fEfficiency[33] = 0.102;
       fEfficiency[34] = 0.083;
       fEfficiency[35] = 0.069;
       fEfficiency[36] = 0.058;
       fEfficiency[37] = 0.041;
       fEfficiency[38] = 0.033;
       fEfficiency[39] = 0.027;
       fEfficiency[40] = 0.020;
       fEfficiency[41] = 0.015;
       fEfficiency[42] = 0.010;
       fEfficiency[43] = 0.006;
       fEfficiency[44] = 0.004;
       fEfficiency[45] = 0.002;
       fEfficiency[46] = 0.001;
     }

 else if(det_type == 13) {
       /**  Measured PMT efficiencies for R11265 (BUW measurement)
	    (Flat type Multianode Photomultiplier with UV window, SBA, 1 square inch)
	    corresponding range in lambda: 180nm  - 640nm in steps of 10nm */

       fLambdaMin = 180.;
       fLambdaMax = 640.;
       fLambdaStep = 10.;
	   
       fEfficiency[0] = 0.043;
       fEfficiency[1] = 0.078;
       fEfficiency[2] = 0.123;
       fEfficiency[3] = 0.146;
       fEfficiency[4] = 0.173;
       fEfficiency[5] = 0.202;
       fEfficiency[6] = 0.225;
       fEfficiency[7] = 0.253;
       fEfficiency[8] = 0.281;
       fEfficiency[9] = 0.290;
       fEfficiency[10] = 0.315;
       fEfficiency[11] = 0.344;
       fEfficiency[12] = 0.366;
       fEfficiency[13] = 0.378;
       fEfficiency[14] = 0.384;
       fEfficiency[15] = 0.400;
       fEfficiency[16] = 0.403;
       fEfficiency[17] = 0.404;
       fEfficiency[18] = 0.407;
       fEfficiency[19] = 0.403;
       fEfficiency[20] = 0.396;
       fEfficiency[21] = 0.395;
       fEfficiency[22] = 0.383;
       fEfficiency[23] = 0.370;
       fEfficiency[24] = 0.359;
       fEfficiency[25] = 0.347;
       fEfficiency[26] = 0.331;
       fEfficiency[27] = 0.310;
       fEfficiency[28] = 0.285;
       fEfficiency[29] = 0.263;
       fEfficiency[30] = 0.244;
       fEfficiency[31] = 0.232;
       fEfficiency[32] = 0.213;
       fEfficiency[33] = 0.182;
       fEfficiency[34] = 0.151;
       fEfficiency[35] = 0.126;
       fEfficiency[36] = 0.106;
       fEfficiency[37] = 0.092;
       fEfficiency[38] = 0.069;
       fEfficiency[39] = 0.060;
       fEfficiency[40] = 0.051;
       fEfficiency[41] = 0.042;
       fEfficiency[42] = 0.034;
       fEfficiency[43] = 0.026;
       fEfficiency[44] = 0.019;
       fEfficiency[45] = 0.014;
       fEfficiency[46] = 0.009;
     }

 else if(det_type == 14) {
       /**  Measured PMT efficiencies for R11265 -- dipcoated WLS film -- (BUW measurement)
	    (Flat type Multianode Photomultiplier with UV window, SBA, 1 square inch)
	    corresponding range in lambda: 180nm  - 640nm in steps of 10nm */

       fLambdaMin = 180.;
       fLambdaMax = 640.;
       fLambdaStep = 10.;
	   
       fEfficiency[0] = 0.239;
       fEfficiency[1] = 0.294;
       fEfficiency[2] = 0.332;
       fEfficiency[3] = 0.351;
       fEfficiency[4] = 0.352;
       fEfficiency[5] = 0.338;
       fEfficiency[6] = 0.303;
       fEfficiency[7] = 0.286;
       fEfficiency[8] = 0.307;
       fEfficiency[9] = 0.307;
       fEfficiency[10] = 0.324;
       fEfficiency[11] = 0.340;
       fEfficiency[12] = 0.354;
       fEfficiency[13] = 0.364;
       fEfficiency[14] = 0.371;
       fEfficiency[15] = 0.390;
       fEfficiency[16] = 0.389;
       fEfficiency[17] = 0.392;
       fEfficiency[18] = 0.395;
       fEfficiency[19] = 0.393;
       fEfficiency[20] = 0.388;
       fEfficiency[21] = 0.388;
       fEfficiency[22] = 0.378;
       fEfficiency[23] = 0.367;
       fEfficiency[24] = 0.358;
       fEfficiency[25] = 0.347;
       fEfficiency[26] = 0.333;
       fEfficiency[27] = 0.310;
       fEfficiency[28] = 0.384;
       fEfficiency[29] = 0.265;
       fEfficiency[30] = 0.248;
       fEfficiency[31] = 0.238;
       fEfficiency[32] = 0.220;
       fEfficiency[33] = 0.188;
       fEfficiency[34] = 0.150;
       fEfficiency[35] = 0.123;
       fEfficiency[36] = 0.104;
       fEfficiency[37] = 0.089;
       fEfficiency[38] = 0.068;
       fEfficiency[39] = 0.058;
       fEfficiency[40] = 0.050;
       fEfficiency[41] = 0.041;
       fEfficiency[42] = 0.033;
       fEfficiency[43] = 0.025;
       fEfficiency[44] = 0.018;
       fEfficiency[45] = 0.013;
       fEfficiency[46] = 0.008;
     }

 else if(det_type == 15) {
       /**  Measured PMT efficiencies for MAPMTs (H8500D-03) at posC (BUW measurement) ##### CernOct2012 #####
	    (Flat type Multianode Photomultiplier with UV window)
	    corresponding range in lambda: 180nm  - 640nm in steps of 10nm */

       fLambdaMin = 180.;
       fLambdaMax = 640.;
       fLambdaStep = 10.;
	   
       fEfficiency[0] = 0.037;
       fEfficiency[1] = 0.063;
       fEfficiency[2] = 0.103;
       fEfficiency[3] = 0.110;
       fEfficiency[4] = 0.131;
       fEfficiency[5] = 0.153;
       fEfficiency[6] = 0.172;
       fEfficiency[7] = 0.195;
       fEfficiency[8] = 0.215;
       fEfficiency[9] = 0.217;
       fEfficiency[10] = 0.232;
       fEfficiency[11] = 0.249;
       fEfficiency[12] = 0.261;
       fEfficiency[13] = 0.267;
       fEfficiency[14] = 0.271;
       fEfficiency[15] = 0.285;
       fEfficiency[16] = 0.286;
       fEfficiency[17] = 0.285;
       fEfficiency[18] = 0.287;
       fEfficiency[19] = 0.285;
       fEfficiency[20] = 0.280;
       fEfficiency[21] = 0.279;
       fEfficiency[22] = 0.272;
       fEfficiency[23] = 0.264;
       fEfficiency[24] = 0.256;
       fEfficiency[25] = 0.248;
       fEfficiency[26] = 0.239;
       fEfficiency[27] = 0.223;
       fEfficiency[28] = 0.204;
       fEfficiency[29] = 0.189;
       fEfficiency[30] = 0.177;
       fEfficiency[31] = 0.170;
       fEfficiency[32] = 0.155;
       fEfficiency[33] = 0.130;
       fEfficiency[34] = 0.105;
       fEfficiency[35] = 0.087;
       fEfficiency[36] = 0.073;
       fEfficiency[37] = 0.060;
       fEfficiency[38] = 0.041;
       fEfficiency[39] = 0.033;
       fEfficiency[40] = 0.027;
       fEfficiency[41] = 0.020;
       fEfficiency[42] = 0.015;
       fEfficiency[43] = 0.010;
       fEfficiency[44] = 0.006;
       fEfficiency[45] = 0.004;
       fEfficiency[46] = 0.003;
     }

 else if(det_type == 16) {
       /**  Measured PMT efficiencies for MAPMTs (H8500D-03) at posC -- optimized dipcoated WLS film -- (BUW measurement) ##### CernOct2012 #####
	    (Flat type Multianode Photomultiplier with UV window)
	    corresponding range in lambda: 180nm  - 640nm in steps of 10nm */

       fLambdaMin = 180.;
       fLambdaMax = 640.;
       fLambdaStep = 10.;
	   
       fEfficiency[0] = 0.202;
       fEfficiency[1] = 0.240;
       fEfficiency[2] = 0.269;
       fEfficiency[3] = 0.277;
       fEfficiency[4] = 0.279;
       fEfficiency[5] = 0.273;
       fEfficiency[6] = 0.245;
       fEfficiency[7] = 0.228;
       fEfficiency[8] = 0.243;
       fEfficiency[9] = 0.243;
       fEfficiency[10] = 0.253;
       fEfficiency[11] = 0.259;
       fEfficiency[12] = 0.262;
       fEfficiency[13] = 0.263;
       fEfficiency[14] = 0.265;
       fEfficiency[15] = 0.278;
       fEfficiency[16] = 0.279;
       fEfficiency[17] = 0.281;
       fEfficiency[18] = 0.283;
       fEfficiency[19] = 0.281;
       fEfficiency[20] = 0.277;
       fEfficiency[21] = 0.275;
       fEfficiency[22] = 0.267;
       fEfficiency[23] = 0.260;
       fEfficiency[24] = 0.253;
       fEfficiency[25] = 0.245;
       fEfficiency[26] = 0.234;
       fEfficiency[27] = 0.219;
       fEfficiency[28] = 0.201;
       fEfficiency[29] = 0.187;
       fEfficiency[30] = 0.175;
       fEfficiency[31] = 0.167;
       fEfficiency[32] = 0.150;
       fEfficiency[33] = 0.124;
       fEfficiency[34] = 0.098;
       fEfficiency[35] = 0.080;
       fEfficiency[36] = 0.066;
       fEfficiency[37] = 0.055;
       fEfficiency[38] = 0.040;
       fEfficiency[39] = 0.033;
       fEfficiency[40] = 0.026;
       fEfficiency[41] = 0.020;
       fEfficiency[42] = 0.014;
       fEfficiency[43] = 0.010;
       fEfficiency[44] = 0.006;
       fEfficiency[45] = 0.004;
       fEfficiency[46] = 0.002;
     }

 else if(det_type == 17) {
       /**  Measured PMT efficiencies for one MAPMT (H10966A-103) at posE (BUW measurement) ##### CernOct2012 #####
	    (Flat type Multianode Photomultiplier with UV window)
	    corresponding range in lambda: 180nm  - 640nm in steps of 10nm */

       fLambdaMin = 180.;
       fLambdaMax = 640.;
       fLambdaStep = 10.;
	   
       fEfficiency[0] = 0.007;
       fEfficiency[1] = 0.040;
       fEfficiency[2] = 0.085;
       fEfficiency[3] = 0.103;
       fEfficiency[4] = 0.130;
       fEfficiency[5] = 0.160;
       fEfficiency[6] = 0.186;
       fEfficiency[7] = 0.215;
       fEfficiency[8] = 0.244;
       fEfficiency[9] = 0.256;
       fEfficiency[10] = 0.281;
       fEfficiency[11] = 0.310;
       fEfficiency[12] = 0.332;
       fEfficiency[13] = 0.344;
       fEfficiency[14] = 0.355;
       fEfficiency[15] = 0.376;
       fEfficiency[16] = 0.382;
       fEfficiency[17] = 0.386;
       fEfficiency[18] = 0.390;
       fEfficiency[19] = 0.390;
       fEfficiency[20] = 0.387;
       fEfficiency[21] = 0.386;
       fEfficiency[22] = 0.376;
       fEfficiency[23] = 0.365;
       fEfficiency[24] = 0.356;
       fEfficiency[25] = 0.345;
       fEfficiency[26] = 0.328;
       fEfficiency[27] = 0.302;
       fEfficiency[28] = 0.278;
       fEfficiency[29] = 0.257;
       fEfficiency[30] = 0.241;
       fEfficiency[31] = 0.227;
       fEfficiency[32] = 0.191;
       fEfficiency[33] = 0.153;
       fEfficiency[34] = 0.128;
       fEfficiency[35] = 0.112;
       fEfficiency[36] = 0.098;
       fEfficiency[37] = 0.085;
       fEfficiency[38] = 0.064;
       fEfficiency[39] = 0.055;
       fEfficiency[40] = 0.047;
       fEfficiency[41] = 0.039;
       fEfficiency[42] = 0.030;
       fEfficiency[43] = 0.023;
       fEfficiency[44] = 0.017;
       fEfficiency[45] = 0.011;
       fEfficiency[46] = 0.007;
     }

 else if(det_type == 18) {
       /**  Measured PMT efficiencies for one MAPMT (H10966A-103) at posE -- optimized dipcoated WLS film -- (BUW measurement) ##### CernOct2012 #####
	    (Flat type Multianode Photomultiplier with UV window)
	    corresponding range in lambda: 180nm  - 640nm in steps of 10nm */

       fLambdaMin = 180.;
       fLambdaMax = 640.;
       fLambdaStep = 10.;
	   
       fEfficiency[0] = 0.241;
       fEfficiency[1] = 0.304;
       fEfficiency[2] = 0.351;
       fEfficiency[3] = 0.364;
       fEfficiency[4] = 0.368;
       fEfficiency[5] = 0.357;
       fEfficiency[6] = 0.311;
       fEfficiency[7] = 0.279;
       fEfficiency[8] = 0.299;
       fEfficiency[9] = 0.304;
       fEfficiency[10] = 0.321;
       fEfficiency[11] = 0.329;
       fEfficiency[12] = 0.336;
       fEfficiency[13] = 0.342;
       fEfficiency[14] = 0.350;
       fEfficiency[15] = 0.370;
       fEfficiency[16] = 0.374;
       fEfficiency[17] = 0.379;
       fEfficiency[18] = 0.383;
       fEfficiency[19] = 0.384;
       fEfficiency[20] = 0.381;
       fEfficiency[21] = 0.382;
       fEfficiency[22] = 0.372;
       fEfficiency[23] = 0.362;
       fEfficiency[24] = 0.354;
       fEfficiency[25] = 0.344;
       fEfficiency[26] = 0.327;
       fEfficiency[27] = 0.300;
       fEfficiency[28] = 0.275;
       fEfficiency[29] = 0.259;
       fEfficiency[30] = 0.244;
       fEfficiency[31] = 0.231;
       fEfficiency[32] = 0.195;
       fEfficiency[33] = 0.155;
       fEfficiency[34] = 0.130;
       fEfficiency[35] = 0.113;
       fEfficiency[36] = 0.097;
       fEfficiency[37] = 0.083;
       fEfficiency[38] = 0.065;
       fEfficiency[39] = 0.055;
       fEfficiency[40] = 0.046;
       fEfficiency[41] = 0.038;
       fEfficiency[42] = 0.030;
       fEfficiency[43] = 0.022;
       fEfficiency[44] = 0.016;
       fEfficiency[45] = 0.011;
       fEfficiency[46] = 0.007;
     }

else if (det_type == 0){
      //   ideal detector
      fLambdaMin = 160.;
      fLambdaMax = 640.;
      fLambdaStep = 20.;

      fEfficiency[0] = 0.99;
      fEfficiency[1] = 0.99;
      fEfficiency[2] = 0.99;
      fEfficiency[3] = 0.99;
      fEfficiency[4] = 0.99;
      fEfficiency[5] = 0.99;
      fEfficiency[6] = 0.99;
      fEfficiency[7] = 0.99;
      fEfficiency[8] = 0.99;
      fEfficiency[9] = 0.99;
      fEfficiency[10] = 0.99;
      fEfficiency[11] = 0.99;
      fEfficiency[12] = 0.99;
      fEfficiency[13] = 0.99;
      fEfficiency[14] = 0.99;
      fEfficiency[15] = 0.99;
      fEfficiency[16] = 0.99;
      fEfficiency[17] = 0.99;
      fEfficiency[18] = 0.99;
      fEfficiency[19] = 0.99;
      fEfficiency[20] = 0.99;
      fEfficiency[21] = 0.99;
      fEfficiency[22] = 0.99;
      fEfficiency[23] = 0.99;
    }

    else {
      cout << "ERROR: photodetector type not specified" << endl;

      fLambdaMin = 100.;
      fLambdaMax = 100.;
      fLambdaStep = 100.;

      fEfficiency[0] = 0.;
    }
}

void CbmRichHitProducer::FindRichHitPositionSinglePMT(
      Double_t xPoint,
      Double_t yPoint,
		Double_t& xHit,
		Double_t& yHit,
		Int_t & pmtID)
{
   // calculate Hits for Protvino type of PMT (single PMTs with circle surface, hexagonal packing):
   // hexagonal packing of round phototubes with radius fPhotomulRadius,
   // distance between phototubes 2*fPhotomulDist -> effective radius is
   // (fPhotomulRadius+fPhotomulDist) assuming that the gap is covered by Al funnels

   xHit = 0.;
   yHit = 0.;

   // Transformation of global (x,y) coordinates to local coordinates in photodetector plane (u,v)
   // the center of (u,v) CS is in the lower left corner of each photodetector
   // u is parrallel to x, v tilted by 30degree (alpha = 30 degree)

   Double_t uPoint, vPoint;
   Double_t uPMT, vPMT;
   Double_t alpha = TMath::Pi()/6.;
   Double_t distance;

   // smear points due to light scattering in mirror
   xPoint = xPoint + gRandom->Gaus(0,fSigmaMirror);
   yPoint = yPoint + gRandom->Gaus(0,fSigmaMirror);

   uPoint = 2.*fGP.fPmtWidthX - (fPhotomulRadius+fPhotomulDist) + xPoint;
   if (yPoint > 0)
      vPoint = (- fGP.fPmtY + fGP.fPmtWidthY - (fPhotomulRadius+fPhotomulDist) + yPoint)/ TMath::Cos(alpha);
   if (yPoint < 0)
      vPoint = (fGP.fPmtY + fGP.fPmtWidthY - (fPhotomulRadius+fPhotomulDist) + yPoint)/ TMath::Cos(alpha);

   // Calculate Position of nearest PMT
   uPMT = (fPhotomulRadius+fPhotomulDist)*((Int_t)(uPoint/(fPhotomulRadius+fPhotomulDist)+0.999));
   vPMT = (fPhotomulRadius+fPhotomulDist)*((Int_t)(vPoint/(fPhotomulRadius+fPhotomulDist)+0.999));

   // Calculate distance between PMT and Point
   distance = TMath::Sqrt((uPMT-uPoint)*(uPMT-uPoint)+(vPMT-vPoint)*(vPMT-vPoint)*TMath::Cos(alpha)*TMath::Cos(alpha));

   // if distance < (fPhotomulRadius+fPhotomulDist)
   // ==> retransform to global (x,y) and store Hit (center of PMT)
   if (distance <= (fPhotomulRadius+fPhotomulDist)){
      xHit = uPMT - 2.*fGP.fPmtWidthX + (fPhotomulRadius+fPhotomulDist);
      if (yPoint > 0)
         yHit = vPMT*TMath::Cos(alpha) + fGP.fPmtY - fGP.fPmtWidthY + (fPhotomulRadius+fPhotomulDist);
      if (yPoint < 0)
         yHit = vPMT*TMath::Cos(alpha) - fGP.fPmtY - fGP.fPmtWidthY + (fPhotomulRadius+fPhotomulDist);

      pmtID = (Int_t)(uPMT/(fPhotomulRadius+fPhotomulDist))*100000+(Int_t)(vPMT/(fPhotomulRadius+fPhotomulDist));
      if (yPoint<0.) pmtID = -1*pmtID;
   }// if hit
}

void CbmRichHitProducer::FindRichHitPositionMAPMT(
      Double_t sigma,
      Double_t xPoint,
      Double_t yPoint,
      Double_t& xHit,
      Double_t& yHit,
      Int_t & pmtID)
{
   // calculate Hits for MAPMT (Hamamatsu H8500, 8x8 MAPMT):
   // dimensions:
   // length = 52mm x 52mm, active area = 49mm x 49mm
   // pixel (mean size) = 6.125mm x 6.125mm
   // assume some spacing between single units of s=1mm
   // ==> use as effective values   fPhotomulRadius = 6.125mm = 0.6125cm
   //                               fPhotomulDist = 0.5mm + 1.5mm = 2mm = 0.2cm
   //
   // sigma (cm) : add WLS film --> hits smeared with sigma

   xHit = 0.;
   yHit = 0.;

   Int_t nPixel = 8;
   Double_t length = (Double_t)(nPixel)*fPhotomulRadius + 2 * fPhotomulDist;//effective size

   // Transformation of global (x,y) coordinates to local coordinates in photodetector plane (u,v)
   // the center of (u,v) CS is in the lower left corner of each photodetector
   Double_t uPoint, vPoint;
   Double_t uPMT, vPMT, uPMTs, vPMTs;

   // smear points due to light scattering in mirror
   xPoint = xPoint + gRandom->Gaus(0, fSigmaMirror);
   yPoint = yPoint + gRandom->Gaus(0, fSigmaMirror);

   // smear Point if photon is converted via WLS film:
   if (sigma > 0.) {
      xPoint = xPoint + gRandom->Gaus(0,sigma);
      yPoint = yPoint + gRandom->Gaus(0,sigma);
   }

   uPoint = 2.*fGP.fPmtWidthX + xPoint;
   if (yPoint > 0)
      vPoint = - fGP.fPmtY + fGP.fPmtWidthY + yPoint;
   if (yPoint < 0)
      vPoint = fGP.fPmtY + fGP.fPmtWidthY + yPoint;

   // calculate lower left corner of effective area of MAPMT unit which has been hit
   uPMT = length*(Int_t)(uPoint/length)+fPhotomulDist;
   vPMT = length*(Int_t)(vPoint/length)+fPhotomulDist;

   // reject points not lying in the effective area of the MAPMT units:
   if ((TMath::Abs((uPMT+(Double_t)(nPixel)/2.*fPhotomulRadius)-uPoint) < ((Double_t)(nPixel)/2.*fPhotomulRadius)) &&
      (TMath::Abs((vPMT+(Double_t)(nPixel)/2.*fPhotomulRadius)-vPoint) < ((Double_t)(nPixel)/2.*fPhotomulRadius))) {

      // check that uPoint > uPMT and vPoint > vPMT
      if (uPoint < uPMT) cout << " -E- HitProducer: calculation of MAPMT unit (u) " << uPoint << " " << uPMT << endl;
      if (vPoint < vPMT) cout << " -E- HitProducer: calculation of MAPMT unit (v) " << vPoint << " " << vPMT << endl;

      // calculate center of PMT cell hit in this MAPMT unit -> Hit
      uPMTs = fPhotomulRadius*(Int_t)((uPoint-uPMT)/fPhotomulRadius)+fPhotomulRadius/2. + uPMT;
      vPMTs = fPhotomulRadius*(Int_t)((vPoint-vPMT)/fPhotomulRadius)+fPhotomulRadius/2. + vPMT;

      // ==> retransform to global (x,y) and store Hit
      xHit = uPMTs - 2.*fGP.fPmtWidthX;
      if (yPoint > 0) yHit = vPMTs + fGP.fPmtY - fGP.fPmtWidthY;
      if (yPoint < 0) yHit = vPMTs - fGP.fPmtY - fGP.fPmtWidthY;

      pmtID = ((Int_t)(uPoint/length)*100 + (Int_t)((uPoint-uPMT)/fPhotomulRadius))*100000
            + ((Int_t)(vPoint/length)*100 + (Int_t)((vPoint-vPMT)/fPhotomulRadius));

      if (yPoint<0.) pmtID = -1*pmtID;

   }// point in effective area ?

   if (fVerbose > 3) cout << "FindHitPos_MAPMT: " << xPoint << " " << yPoint << " " << xHit << " " << yHit << endl;
}

void CbmRichHitProducer::FindRichHitPositionCsI(
      Double_t xPoint,
      Double_t yPoint,
      Double_t& xHit,
      Double_t& yHit,
      Int_t & pmtID)
{
   // calculate Hits for CsI
   // dimensions (assume 3 panels of 1.4m x 1.067 m) per plane):
   // height = (fDetWidthY - 2* fPhotomulDist) * 2
   // length = (2.*fDetWidthX - 2* fPhotomulDist) * 2 / 3
   // pixels (mean size) = 8mm x 8mm (similar to ALICE)
   // integrate spacing between single panels into fPhotomulDist (= eff. distance between active areas)
   // fPhotomulDist = 0.5cm
   // fPhotomulRadius = 0.8cm

   xHit = 0.;
   yHit = 0.;
   Double_t xlength = 2.*fGP.fPmtWidthX * 2. / 3.;
   Double_t ylength = fGP.fPmtWidthY * 2.;

   // Transformation of global (x,y) coordinates to local coordinates in photodetector plane (u,v)
   // the center of (u,v) CS is in the lower left corner of each photodetector

   Double_t uPoint, vPoint;
   Double_t uPMT, vPMT, uPMTs, vPMTs;

   // smear points due to light scattering in mirror
   xPoint = xPoint + gRandom->Gaus(0,fSigmaMirror);
   yPoint = yPoint + gRandom->Gaus(0,fSigmaMirror);

   uPoint = 2.*fGP.fPmtWidthX + xPoint;
   if (yPoint > 0) vPoint = - fGP.fPmtY + fGP.fPmtWidthY + yPoint;
   if (yPoint < 0) vPoint = fGP.fPmtY + fGP.fPmtWidthY + yPoint;

   // calculate lower left corner of effective area of panel which has been hit
   uPMT = xlength*(Int_t)(uPoint/xlength)+fPhotomulDist;
   vPMT = ylength*(Int_t)(vPoint/ylength)+fPhotomulDist;

   // reject points not lying in the effective area of the panels:
   if ((TMath::Abs(uPMT+(xlength/2.-fPhotomulDist)-uPoint) < (xlength/2.-fPhotomulDist)) &&
      (TMath::Abs(vPMT+(ylength/2.-fPhotomulDist)-vPoint) < (ylength/2.-fPhotomulDist))) {

      // check that uPoint > uPMT and vPoint > vPMT
      if (uPoint < uPMT) cout << " -E- HitProducer: calculation of CsI unit (u) " << uPoint << " " << uPMT << endl;
      if (vPoint < vPMT) cout << " -E- HitProducer: calculation of CsI unit (v) " << vPoint << " " << vPMT << endl;

      // calculate center of PMT cell hit in this MAPMT unit -> Hit
      uPMTs = fPhotomulRadius*(Int_t)((uPoint-uPMT)/fPhotomulRadius)+fPhotomulRadius/2. + uPMT;
      vPMTs = fPhotomulRadius*(Int_t)((vPoint-vPMT)/fPhotomulRadius)+fPhotomulRadius/2. + vPMT;

      // ==> retransform to global (x,y) and store Hit
      xHit = uPMTs - 2.*fGP.fPmtWidthX;
      if (yPoint > 0) yHit = vPMTs + fGP.fPmtY - fGP.fPmtWidthY;
      if (yPoint < 0) yHit = vPMTs - fGP.fPmtY - fGP.fPmtWidthY;

      pmtID = ((Int_t)(uPoint/xlength)*1000 + (Int_t)((uPoint-uPMT)/fPhotomulRadius))*100000
            + ((Int_t)(vPoint/ylength)*1000 + (Int_t)((vPoint-vPMT)/fPhotomulRadius));

      if (yPoint<0.) pmtID = -1*pmtID;
   }  // point in effective area?
}

Double_t CbmRichHitProducer::OnePhotonAmplitude(
      Double_t x)
{
   // Spectrum of the PMT response to one photo-electron
   // after S.Sadovsky, 9 Sep 2004
   const Float_t kn=1.85;
   const Float_t kb=1.75;
   return TMath::Power(x*kb/kn,kn) * TMath::Exp(-(kb*x-kn));
}

Double_t CbmRichHitProducer::GetAmplitude()
{
   // Generate randomly PMT amplitude according to probability density
   // provided by OnePhotonAmplitude(x)

   const Double_t xMin=0;
   const Double_t xMax=6;
   Double_t ampl;
   while (kTRUE)
      if (gRandom->Rndm() < OnePhotonAmplitude(ampl = gRandom->Uniform(xMin,xMax))) break;

   return ampl;
}

ClassImp(CbmRichHitProducer)
