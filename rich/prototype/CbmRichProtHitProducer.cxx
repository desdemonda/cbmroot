/**
* \file CbmRichProtHitProducer.cxx
*
* \author D. Kresan
* \date 2010
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

#include "CbmRichProtHitProducer.h"

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

#include "TVector3.h"
#include "TRandom.h"
#include "TFormula.h"
#include "TString.h"
#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

const Double_t CbmRichProtHitProducer::c = 2.998E8; // speed of light
const Double_t CbmRichProtHitProducer::h = 6.626E-34; // Planck constant
const Double_t CbmRichProtHitProducer::e = 1.6022E-19; // elementary charge

CbmRichProtHitProducer::CbmRichProtHitProducer():
   FairTask("CbmRichProtHitProducer"),
   fRichPoints(NULL),
   fRichHits(NULL),
   fMcTracks(NULL),
   fNHits(0),
   fNDoubleHits(0),
   fNRefrac(0.),
   fDetection(0),
   fNEvents(0),
   fDetX(0.),
   fDetY(0.),
   fDetZ(0.),
   fDetZ_org(0.),
   fDetWidthX(0.),
   fDetWidthY(0.),
   fSensNodes(NULL),
   fPassNodes(NULL),
   fPar(NULL),
   fPhotomulRadius(0.0),
   fPhotomulDist(0.),
   fDetType(4),
   fNofNoiseHits(220),
   fNofNoiseHitsCounter(0),
   fCollectionEfficiency(1.),
   fSigmaMirror(0.06),
   fTheta(0.),
   fPhi(0.),
   fCrossTalkHitProb(0.),
   fNofCrossTalkHits(0)// = 0; // posF: 10.2% (20*10.2%=2.04hits), posH: 9,8% (20*9,8%=1.96hits) ==> in fCrossTalkHitProb
   //fCrossTalkHitProb(0.02),
{

}

CbmRichProtHitProducer::~CbmRichProtHitProducer()
{
   FairRootManager *manager =FairRootManager::Instance();
   manager->Write();
}

void CbmRichProtHitProducer::SetParContainers()
{
   // Get Base Container
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();
   fPar=(CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));
   //fPar1=(FairBaseParSet*)(rtdb->getContainer("FairBaseParSet"));
   //fPar->print();
   //fPar->setStatic();    // setting the parameters on static mode: <explanation>
}

InitStatus CbmRichProtHitProducer::Init()
{
   FairRootManager* fManager = FairRootManager::Instance();

   fSensNodes = fPar->GetGeoSensitiveNodes();
   fPassNodes = fPar->GetGeoPassiveNodes();
   //fSensNodes->ls();

   // get detector position:
   FairGeoNode *det = dynamic_cast<FairGeoNode*> (fSensNodes->FindObject("rich1detector"));
   if (NULL == det) cout << " -I no RICH Geo Node  found !!!!!  " << endl;
   //fDet->Dump();
   //fDet->print();
   FairGeoTransform* detTr = det->getLabTransform(); // detector position in labsystem
   FairGeoVector detPosLab = detTr->getTranslation(); // ... in cm
   FairGeoTransform detCen = det->getCenterPosition(); // center in Detector system
   FairGeoVector detPosCen = detCen.getTranslation();
   fDetX = detPosLab.X() + detPosCen.X(); // x coordinate of photodetector (Labsystem, cm)
   fDetY = detPosLab.Y() + detPosCen.Y(); // y coordinate of photodetector (Labsystem, cm)
   fDetZ = detPosLab.Z() + detPosCen.Z(); // z coordinate of photodetector (Labsystem, cm)

   TArrayD *fdetA=det->getParameters(); // get other geometry parameters: width in x, width in y, thickness
   fDetWidthX = fdetA->At(0);
   fDetWidthY = fdetA->At(1);
   for(Int_t i=0;i<fdetA->GetSize();i++) cout << "Array detector " << fdetA->At(i)<< endl;
   FairGeoRotation fdetR=detTr->getRotMatrix();
   // detector might be rotated by theta around x-axis:
   if (fVerbose){
      cout << "Rotation matrix of photodetector " << endl;
      for(Int_t i = 0;i < 9; i++) cout << "Rot(" << i << ") = " << fdetR(i) << endl;
   }

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

   fTheta = TMath::ASin(fdetR(7)); // tilting angle around x-axis
   fPhi = -1.*TMath::ASin(fdetR(2)); // tilting angle around y-axis
   if (fVerbose) cout << "Rich Photodetector was tilted around x by " << fTheta*180./TMath::Pi() << " degrees" << endl;
   if (fVerbose) cout << "Rich Photodetector was tilted around y by " << fPhi*180./TMath::Pi()<< " degrees" << endl;

   // get refractive index of gas
   FairGeoNode *gas= dynamic_cast<FairGeoNode*> (fPassNodes->FindObject("rich1gas1"));
   if (NULL == gas) cout << " -I no RICH Geo Node  found !!!!!  " << endl;
   FairGeoMedium* med = gas->getMedium();
   //med->Dump();
   Int_t npckov = med->getNpckov();
   Double_t* cerpar;
   cerpar = new Double_t[4];
   if (fVerbose) cout << "Number of optical parameters for Cherenkov " << npckov << endl;
   med->getCerenkovPar(0,cerpar);
   /*
   for(Int_t i=0;i<4;i++) {
    if (i==0) cout << " photon energy " << cerpar[0] << endl;
    if (i==1) cout << " absorption lenght" << cerpar[1] << endl;
    if (i==2) cout << " detection efficiency " << cerpar[2] << endl;
    if (i==3) cout << " refractive index (n-1)*10000 " << (cerpar[3]-1.)*10000. << endl;
    }
   */
   fNRefrac = cerpar[3];
   if (fVerbose) cout << " refractive index for lowest photon energies (n-1)*10000  " << (fNRefrac-1.0)*10000.0 << endl;

   // transform nominal detector position (for tilted photodetector), x>0, y>0:
   Double_t fDetY_org, fDetX_org;
   fDetX_org = fDetX;
   fDetY_org = fDetY;
   fDetZ_org = fDetZ;
   /*    fDetX = fDetX_org*TMath::Cos(phi)+fDetZ_org*TMath::Sin(phi);
    fDetY = -fDetX_org*TMath::Sin(theta)*TMath::Sin(phi) + fDetY_org*TMath::Cos(theta) + fDetZ_org*TMath::Sin(theta)*TMath::Cos(phi);
    fDetZ = -fDetX_org*TMath::Cos(theta)*TMath::Sin(phi) - fDetY_org*TMath::Sin(theta) + fDetZ_org*TMath::Cos(theta)*TMath::Cos(phi);*/

   if (fVerbose > 0) {
      cout << "---------------------- RICH Hit Producer ---------------------------------------" << endl;
      cout << "   detector position in (x,y,z) [cm]: " << fDetX_org << "  " << fDetY_org << "  " << fDetZ_org << endl;
      cout << "   tilted detector position in (x,y,z) [cm]: " << fDetX << "  " << fDetY << "  " << fDetZ << endl;
      cout << "   detector size in x and y [cm]: " << fDetWidthX << "  " << fDetWidthY << endl;
      if (fDetType==0)
         cout << "   ideal detector " << endl;
      if (fDetType==1)
         cout << "   detector type: Protvino PMT with PMT radius = " << fPhotomulRadius << " cm, distance between PMTs = " << fPhotomulDist << " cm" << endl;
      if (fDetType==3)
         cout << "   detector type: CSI with pad size = " << fPhotomulRadius << " cm, distance between panels = " << fPhotomulDist << " cm" << endl;
      if (fDetType == 0 || fDetType == 2 || fDetType == 4 || fDetType == 5 || fDetType == 6 || fDetType == 10 || fDetType == 11 || fDetType == 12 || fDetType == 13 || fDetType == 14 || fDetType == 15 || fDetType == 16 || fDetType == 17 || fDetType == 18)
         cout << "   detector type: Hamamatsu H8500 with pad size = " << fPhotomulRadius << " cm, distance between elements = " << fPhotomulDist << " cm" << endl;
      cout << "   number of noise hits (to be reduced by geometrical efficiency) " << fNofNoiseHits << endl;
      cout << "--------------------------------------------------------------------------------" << endl;
   }
   //------------- example for getting more parameters from the data base: -------------------
   /*
   // 1) get and print medium
     FairGeoMedium* med = fDet->getMedium();
     med->Dump();

   // 2) retrieve relevant parameter
   // Shape
     Int_t npoints = fDet->getNumPoints();
     Double_t para[npoints][3];
     TString shapeName = fDet->getShape();
     for (Int_t i=0;i<npoints;i++) {
         for (Int_t j=0;j<3; j++){
              para[i][j] = fDet->getVolParameter(i,j);
              cout << "i: " << i << "j: " << j << "par: " << para[i][j] << endl;
         }
     }

   // 3) Lab Transform
     FairGeoTransform* transf =  fDet->getLabTransform();
     FairGeoRotation rot = transf->getRotMatrix();
     FairGeoVector trans = transf->getTransVector();

     Double_t rotp[9];
     cout << " Lab rotation : " << endl;
     for (Int_t i=0; i<9; i++) {
                                  rotp[i] = rot(i);
                                  cout << " i: " << i << " val: " << rotp[i] ;
     }
     cout << endl;
     cout << " Lab translation : " << endl;
     cout << " tX: " << trans.X() << " tY: " << trans.Y() << " tZ: " << trans.Z() << endl;
   */
   //------------------------------------------------------------------------------------------

   fRichPoints = (TClonesArray*)fManager->GetObject("RichPoint");
   if (NULL == fRichPoints) { Fatal("CbmRichHitProducer::Init","No RichPoint array!"); }

   fMcTracks = (TClonesArray *)fManager->GetObject("MCTrack");
   if (NULL == fMcTracks) { Fatal("CbmRichHitProducer::Init","No MCTrack array!"); }

   fRichHits = new TClonesArray("CbmRichHit");
   fManager->Register("RichHit","RICH", fRichHits, kTRUE);

   // Set photodetector parameters according to its type
   if (fDetType == 0){
      fPhotomulRadius = 0.;
      fPhotomulDist = 0.;
   }
   if (fDetType == 0 || fDetType == 2 || fDetType == 4 || fDetType == 5 || fDetType == 6 || fDetType == 10 || fDetType == 11 || fDetType == 12 || fDetType == 13 || fDetType == 14 || fDetType == 15 || fDetType == 16 || fDetType == 17 || fDetType == 18) {
      fPhotomulRadius = 0.6125;
      fPhotomulDist = 0.25;//0.25; // corresponds to 2mm spacing
     // fCrossTalkHitProb = 0.02; // posF: 10.2%, posH: 9,8%, that is 2 percent in direct neighbours and 0.5% in diagonal neighbours
   }
   if (fDetType == 3) {
      fPhotomulRadius = 0.8;
      fPhotomulDist = 0.5;
   }
   return kSUCCESS;

}

void CbmRichProtHitProducer::Exec(
      Option_t* option)
{
   Int_t RichDetID = 0;

   fNEvents++;
   cout << "-I- CbmRichProtHitProducer, event no " << fNEvents << endl;

   Double_t lambda_min,lambda_max,lambda_step;
   Double_t efficiency[100];// 40
   SetPhotoDetPar(fDetType,lambda_min,lambda_max,lambda_step,efficiency);

   if (fVerbose > 0) cout <<"-I- Number of input MC points: "<< fRichPoints->GetEntries()<<endl;

   fRichHits->Clear();
   fNHits = 0;
   fNDoubleHits = 0;

   for(Int_t j = 0; j < fRichPoints->GetEntries(); j++) {
      CbmRichPoint* pt = (CbmRichPoint*) fRichPoints->At(j);

      TVector3 posPoint;
      pt->Position(posPoint);

      Double_t xDet,yDet,zDet;

      const Double_t ptGlobal[] = {posPoint.X(), posPoint.Y(), posPoint.Z()};
      Double_t ptLocal[3];
      gGeoManager->FindNode(ptGlobal[0], ptGlobal[1], ptGlobal[2]);
      gGeoManager->MasterToLocal(ptGlobal, ptLocal);
      xDet = ptLocal[0];
      yDet = ptLocal[1] + fDetY;
      zDet = ptLocal[2];
      TVector3 detPoint(xDet,yDet,zDet);

      if (fVerbose > 1) cout << " position in Labsystem " << posPoint.X() << " " << posPoint.Y() << " " << posPoint.Z() << endl;
      if (fVerbose > 1) cout << " tilted position in Labsystem " << detPoint.X() << " " << detPoint.Y() << " " << detPoint.Z() << endl;

      Int_t trackID = pt->GetTrackID();
      CbmMCTrack *p = (CbmMCTrack*) fMcTracks->At(trackID);
      Int_t gcode = TMath::Abs(p->GetPdgCode());

      //hit position as a center of PMT
      Double_t xHit, yHit;
      Int_t pmtID;
      Double_t sigma_0 = 0.09;
      Double_t sigma_wls = 0.164;//0.19; // sigma (cm) for additional smearing of HitPosition due to WLS film

      //FindRichHitPosition
      if (fDetType == 0) {
         xHit = detPoint.X();
         yHit = detPoint.Y();
         pmtID = j;
      }
      if (fDetType == 1) FindRichHitPositionSinglePMT(detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);
      if (fDetType == 0 || fDetType == 2 || fDetType == 4 || fDetType == 5 || fDetType == 6 || fDetType == 10 || fDetType == 11 || fDetType == 12 || fDetType == 13 || fDetType == 14 || fDetType == 15 || fDetType == 16 || fDetType == 17 || fDetType == 18) FindRichHitPositionMAPMT(sigma_0,detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);
      if (fDetType == 3) FindRichHitPositionCsI(detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);

      //Double_t zHit = detPoint.Z();
      Double_t zHit = fDetZ; // fix z-position to nominal value: either tilted (fDetZ = zDet) or untilted (fDetZ_org)
      // TVector3 posHit(xHit,yHit,zHit);

      //error of hit position
      //at the moment nothing better than +-tube_radius
      TVector3 posHitErr(fPhotomulRadius, fPhotomulRadius, 0.);

      // add Hit: Hit assigned only if xHit and yHit != 0
      if (xHit!=0.0 && yHit!=0.0) {
         if (fDetType == 1) {
            if (fVerbose)
               if (TMath::Sqrt((detPoint.X()-xHit)*(detPoint.X()-xHit)+(detPoint.Y()-yHit)*(detPoint.Y()-yHit)) > (fPhotomulRadius+fPhotomulDist)*1.5)
                  cout << "-E- RichHitProducer: wrongly assigned Hits (distance point-hit too large)!" << endl;
         }
         if (fDetType == 0 || fDetType == 2 || fDetType == 3 || fDetType == 4 || fDetType == 6 || fDetType == 12 || fDetType == 13 || fDetType == 14 || fDetType == 15 || fDetType == 16 || fDetType == 17 || fDetType == 18) {
            if (fVerbose)
               if (TMath::Abs(detPoint.X()-xHit) > fPhotomulRadius || TMath::Abs(detPoint.Y()-yHit) > fPhotomulRadius*1.5)
                  cout << "-E- RichHitProducer: wrongly assigned Hits (distance point-hit too large)! " <<
                  detPoint.X() << " " << xHit << " " << detPoint.Y() << " " << yHit << endl;
         }
         if (fDetType == 5 || fDetType == 10 || fDetType == 11 || fDetType == 14 || fDetType == 16 || fDetType == 18) {      // fDetType 5: additional smearing with RMS=3mm due to WLS film
            if (fVerbose)
               if (TMath::Abs(detPoint.X()-xHit) > fPhotomulRadius+1.5 || TMath::Abs(detPoint.Y()-yHit) > fPhotomulRadius*1.5)
               if (TMath::Abs(detPoint.X()-xHit) > fPhotomulRadius || TMath::Abs(detPoint.Y()-yHit) > fPhotomulRadius*1.5) // --- ADDITIONAL SMEARING MAKES HIT MULT DISTRIBUTION UNSYMMETRIC, therefore taken out ---
                  cout << "-E- RichHitProducer: wrongly assigned Hits ? (distance point-hit too large)! " <<
                  detPoint.X() << " " << xHit << " " << detPoint.Y() << " " << yHit << endl;
         }

         if (gcode == 50000050) {
            //for photons weight with efficiency of PMT
            TVector3 mom;
            pt->Momentum(mom);
            Double_t etot = sqrt(mom.Px()*mom.Px()+mom.Py()*mom.Py()+ mom.Pz()*mom.Pz());
            Double_t lambda=c / fNRefrac * h / e/ etot; // wavelength in nm
            fDetection=0;
            if (lambda >= lambda_min && lambda < lambda_max) {
               Int_t ilambda=(Int_t)((lambda-lambda_min)/lambda_step);
               Double_t rand = gRandom->Rndm();
               fDetection = 0;
               if (fDetType == 5 || fDetType == 10 || fDetType == 11 || fDetType == 14 || fDetType == 16 || fDetType == 18 && lambda < 300.) {// smear Hit position for lambda < 300 nm (WLS film!)
                  FindRichHitPositionMAPMT(sigma_wls,detPoint.X(),detPoint.Y(),xHit,yHit,pmtID);
               }
               if (efficiency[ilambda] * fCollectionEfficiency > rand ) fDetection = 1;
               //detection = 1;
            } // min <= lambda < max
         } // if photon

         // detection efficiency for hadrons crossing the PMT-cathodes?
         //      else if (gcode ==  211) detection=1;  //pi+-
         //      else if (gcode ==  321) detection=1;  //K+-
         //      else if (gcode == 2212) detection=1;  //p+-
         else {
            // worst case: assume that all charged particles crossing the PMTplane leave Cherenkov light in the PMTglass
            // which will be detected
            fDetection=1;
         }

         TVector3 posHit(xHit,yHit,zHit);
         if (fDetection == 1) {
            Int_t address = pt->GetDetectorID();
            if (RichDetID == 0) RichDetID = address;
            if (RichDetID != address) cout << " - E - RichDetID changed from " << RichDetID <<" to " << address << endl;
            Double_t ampl = GetAmplitude();
            AddHit(posHit,posHitErr,address,pmtID,ampl,j);

            AddCrossTalkHits(posHit.X(), posHit.Y(), j, RichDetID);
         }// photon detected?
      }

      if (fVerbose > 2) {
         cout << " iHit, Point(x,y), Hit(x,y), detected, PMT? " << j << " " << posPoint.X() << " "
            << posPoint.Y() << " " << xHit << " " << yHit << " " << fDetection << " " << pmtID << endl;
      }
   } // loop over input points

   // add noise hits
   // modification for fNofNoiseHits < 1 (fNofNoiseHits=0.001024 in CernOct2011)
   Double_t randnoise = gRandom->Rndm();
   if(randnoise < fNofNoiseHits) {
      for(Int_t j=0; j < fNofNoiseHits; j++) {
         Double_t rand = gRandom->Rndm();
         Double_t xRand = -fDetWidthX + rand*2.*fDetWidthX;
         rand = gRandom->Rndm();
         Double_t yRand = fDetY - fDetWidthY + rand*2.*fDetWidthY;
         //Double_t yRand = -fDetWidthY + rand*2.*fDetWidthY;

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
            Double_t zHit = fDetZ;
            TVector3 posHit(xHit,yHit,zHit);
            Double_t ampl = GetAmplitude();
            //error of hit position, at the moment nothing better than +-tube_radius
            TVector3 posHitErr(fPhotomulRadius,fPhotomulRadius,0.);

            AddHit(posHit,posHitErr,RichDetID,pmtID,ampl,-1);
            fNofNoiseHitsCounter++;
         }
      } // noise hits
   }

   if (fVerbose > 0) {
      cout <<"  --->  Number of hits: "<< fRichHits->GetEntries() << endl;
      cout <<"  --->  Fraction of double hits: "<<(Double_t)(fNDoubleHits)/(Double_t)(fNHits) <<endl;
      cout <<"-------------------------------------------------------------------"<<endl;
      cout << endl;
   }
   cout << "nof cross talk hits = " << fNofCrossTalkHits << ", per event = " <<
         (Double_t) fNofCrossTalkHits / fNEvents << endl;
   cout << "nof noise hits = " << fNofNoiseHitsCounter << ", per event = " <<
         (Double_t) fNofNoiseHitsCounter / fNEvents << endl;
}

void CbmRichProtHitProducer::AddHit(
      TVector3 &posHit,
      TVector3 &posHitErr,
	   Int_t address,
	   Int_t pmtID,
	   Double_t ampl,
	   Int_t index)
{
   // Add this hit to existing one, if in the same PMT, otherwise create a new one.
   Bool_t hitMerged = kFALSE;
   CbmRichHit* hit;
   // Check if there was any hit in the same PMT
   for (Int_t iHit=0; iHit<fNHits; iHit++) {
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

void CbmRichProtHitProducer::AddCrossTalkHits(
      Double_t x,
      Double_t y,
      Int_t pointInd,
      Int_t RichDetID)
{
   // only for MAMPT
   if (fDetType != 0 && fDetType != 2 && fDetType != 4 && fDetType != 5 && fDetType != 6 && fDetType != 10 && fDetType != 11 && fDetType != 12 && fDetType != 13 && fDetType != 14 && fDetType != 15 && fDetType != 16 && fDetType != 17 && fDetType != 18) return;
   //cout << "inside AddCrossTalkHits()" << endl;
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
      Double_t zHit = fDetZ;
      TVector3 posHit(xHit,yHit,zHit);
      Double_t ampl = GetAmplitude();
      TVector3 posHitErr(fPhotomulRadius,fPhotomulRadius,0.);

      AddHit(posHit, posHitErr, RichDetID, pmtID, ampl, pointInd);
    //AddHit(posHit, posHitErr, RichDetID, pmtID, ampl,-1);
      fNofCrossTalkHits++;
      //cout << "(" << x << " " << y << ") (" << xHit << " " << yHit << ")" << endl;
   }
}

void CbmRichProtHitProducer::Finish()
{
  fRichHits->Clear();
}

void CbmRichProtHitProducer::SetPhotoDetPar(
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

   /*
     fLambdaMin = 100.;
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
     fEfficiency[11] = 0.216;
     fEfficiency[12] = 0.227;
     fEfficiency[13] = 0.23;
     fEfficiency[14] = 0.227;
     fEfficiency[15] = 0.216;
     fEfficiency[16] = 0.2;
     fEfficiency[17] = 0.176;
     fEfficiency[18] = 0.15;
     fEfficiency[19] = 0.138;
     fEfficiency[20] = 0.1;
     fEfficiency[21] = 0.082;
     fEfficiency[22] = 0.06;
     fEfficiency[23] = 0.044;
     fEfficiency[24] = 0.032;
     fEfficiency[25] = 0.022;
     fEfficiency[26] = 0.015;
     fEfficiency[27] = 0.01;
     fEfficiency[28] = 0.006;
     fEfficiency[29] = 0.004;
     */
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

      // PMT efficiencies for Hamamatsu H8500 (Flat type Multianode Photomultiplier)
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

      // PMT efficiencies for Hamamatsu H8500-03 (Flat type Multianode Photomultiplier with UV window)
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
      //(Flat type Multianode Photomultiplier with UV window)
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
      cout << "-E- photodetector type not specified" << endl;

      fLambdaMin = 100.;
      fLambdaMax = 100.;
      fLambdaStep = 100.;

      fEfficiency[0] = 0.;

   }
// ------------------- read QE  ---------------------------------
/*	for (Int_t k=0; k<=46; k++) {
  	cout << "QE[" << k << "] " << fEfficiency[k] << endl;
	}*/
}


void CbmRichProtHitProducer::FindRichHitPositionSinglePMT(
      Double_t xPoint,
      Double_t yPoint,
		Double_t& xHit,
		Double_t& yHit,
      Int_t & pmtID)
{
   // calculate Hits for Protvino type of PMT (single PMTs with circle surface, hexagonal packing):
   // hexagonal packing of round phototubes with radius fPhotomulRadius, distance
   // between phototubes 2*fPhotomulDist -> effective radius is
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
   xPoint = xPoint + gRandom->Gaus(0, fSigmaMirror);
   yPoint = yPoint + gRandom->Gaus(0, fSigmaMirror);

   uPoint = 2.*fDetWidthX - (fPhotomulRadius+fPhotomulDist) + xPoint;
   if (yPoint > 0)
      vPoint = (- fDetY + fDetWidthY - (fPhotomulRadius+fPhotomulDist) + yPoint)/ TMath::Cos(alpha);
   if (yPoint < 0)
      vPoint = (fDetY + fDetWidthY - (fPhotomulRadius+fPhotomulDist) + yPoint)/ TMath::Cos(alpha);

   // Calculate Position of nearest PMT
   uPMT = (fPhotomulRadius+fPhotomulDist)*((Int_t)(uPoint/(fPhotomulRadius+fPhotomulDist)+0.999));
   vPMT = (fPhotomulRadius+fPhotomulDist)*((Int_t)(vPoint/(fPhotomulRadius+fPhotomulDist)+0.999));

   // Calculate distance between PMT and Point
   distance = TMath::Sqrt((uPMT-uPoint)*(uPMT-uPoint)+(vPMT-vPoint)*(vPMT-vPoint)*TMath::Cos(alpha)*TMath::Cos(alpha));

   // if distance < (fPhotomulRadius+fPhotomulDist)
   // ==> retransform to global (x,y) and store Hit (center of PMT)
   if (distance <= (fPhotomulRadius+fPhotomulDist)){
      xHit = uPMT - 2.*fDetWidthX + (fPhotomulRadius+fPhotomulDist);
      if (yPoint > 0)
         yHit = vPMT*TMath::Cos(alpha) + fDetY - fDetWidthY + (fPhotomulRadius+fPhotomulDist);
      if (yPoint < 0)
         yHit = vPMT*TMath::Cos(alpha) - fDetY - fDetWidthY + (fPhotomulRadius+fPhotomulDist);

      pmtID = (Int_t)(uPMT/(fPhotomulRadius+fPhotomulDist))*100000+(Int_t)(vPMT/(fPhotomulRadius+fPhotomulDist));
      if (yPoint<0.) pmtID = -1*pmtID;
   }  // if hit
}

void CbmRichProtHitProducer::FindRichHitPositionMAPMT(
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
   //                                fPhotomulDist = 0.5mm + 1.5mm = 2mm = 0.2cm
   //  sigma (cm) : add WLS film --> hits smeared with sigma
   xHit = 0.;
   yHit = 0.;

   Int_t nPixel = 8;
   Double_t length = (Double_t)(nPixel)*fPhotomulRadius+2*fPhotomulDist;//effective size

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

   uPoint = 2.*fDetWidthX + xPoint;
   //if (yPoint > 0)
      vPoint = - fDetY + fDetWidthY + yPoint;
      //vPoint = 2.*fDetWidthY + yPoint;
   //if (yPoint < 0)
   // vPoint = fDetY + fDetWidthY + yPoint;

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
      xHit = uPMTs - 2.*fDetWidthX;
      yHit = vPMTs + fDetY - fDetWidthY;
      //yHit = vPMTs - 2.*fDetWidthY;

      pmtID = ((Int_t)(uPoint/length)*100 + (Int_t)((uPoint-uPMT)/fPhotomulRadius))*100000
            + ((Int_t)(vPoint/length)*100 + (Int_t)((vPoint-vPMT)/fPhotomulRadius));

   } // point in effective area ?
}

void CbmRichProtHitProducer::FindRichHitPositionCsI(
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
   Double_t xlength = 2.*fDetWidthX * 2. / 3.;
   Double_t ylength = fDetWidthY * 2.;

   // Transformation of global (x,y) coordinates to local coordinates in photodetector plane (u,v)
   // the center of (u,v) CS is in the lower left corner of each photodetector

   Double_t uPoint, vPoint;
   Double_t uPMT, vPMT, uPMTs, vPMTs;

   // smear points due to light scattering in mirror
   xPoint = xPoint + gRandom->Gaus(0, fSigmaMirror);
   yPoint = yPoint + gRandom->Gaus(0, fSigmaMirror);

   uPoint = 2.*fDetWidthX + xPoint;
   if (yPoint > 0)
      vPoint = - fDetY + fDetWidthY + yPoint;
   if (yPoint < 0)
      vPoint = fDetY + fDetWidthY + yPoint;

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
      xHit = uPMTs - 2.*fDetWidthX;
      if (yPoint > 0) yHit = vPMTs + fDetY - fDetWidthY;
      if (yPoint < 0) yHit = vPMTs - fDetY - fDetWidthY;

      pmtID = ((Int_t)(uPoint/xlength)*1000 + (Int_t)((uPoint-uPMT)/fPhotomulRadius))*100000
            + ((Int_t)(vPoint/ylength)*1000 + (Int_t)((vPoint-vPMT)/fPhotomulRadius));

      if (yPoint<0.) pmtID = -1*pmtID;

   }// point in effective area?
}

Double_t CbmRichProtHitProducer::OnePhotonAmplitude(
      Double_t x)
{
   // Spectrum of the PMT response to one photo-electron
   // after S.Sadovsky, 9 Sep 2004
   const Float_t kn=1.85;
   const Float_t kb=1.75;
   return TMath::Power(x*kb/kn,kn) * TMath::Exp(-(kb*x-kn));
}

Double_t CbmRichProtHitProducer::GetAmplitude()
{
   // Generate randomly PMT amplitude according to probability density
   // provided by OnePhotonAmplitude(x)

   const Double_t xMin=0;
   const Double_t xMax=6;
   Double_t ampl;
   while (kTRUE)
      if (gRandom->Rndm() < OnePhotonAmplitude(ampl = gRandom->Uniform(xMin,xMax)))
         break;
   return ampl;
}

ClassImp(CbmRichProtHitProducer)
