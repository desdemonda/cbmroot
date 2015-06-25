#include "CbmRich.h"

#include "CbmGeoRichPar.h"
#include "CbmGeoRich.h"
#include "CbmRichPoint.h"

#include "CbmDetectorList.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "CbmStack.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

#include "TObjArray.h"
#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TGeoMedium.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TParticlePDG.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TGeoNode.h"

#include "FairGeoMedia.h"
#include "FairGeoBuilder.h"

#include "TGDMLParse.h"

#include <iostream>

using std::cout;
using std::endl;

std::map<TString, TGeoMedium*> CbmRich::fFixedMedia;

CbmRich::CbmRich() :
   FairDetector("RICH", kTRUE, kRICH),
   fPosIndex(0),
   fRegisterPhotonsOnSensitivePlane(false),
   fRichPoints(new TClonesArray("CbmRichPoint")),
   fRichRefPlanePoints(new TClonesArray("CbmRichPoint")),
   fRichMirrorPoints(new TClonesArray("CbmRichPoint")),
   fRotation(NULL),
   fPositionRotation(NULL)
{
  /*
   fRichPoints = ;
   fRichRefPlanePoints = ;
   fRichMirrorPoints = ;
   fPosIndex = 0;
  */

   fVerboseLevel = 1;
}

CbmRich::CbmRich(
      const char* name,
      Bool_t active,
      Double_t px,
      Double_t py,
      Double_t pz,
      Double_t rx,
      Double_t ry,
      Double_t rz):
   FairDetector(name, active, kRICH),
   fPosIndex(0),
   fRegisterPhotonsOnSensitivePlane(false),
   fRichPoints(new TClonesArray("CbmRichPoint")),
   fRichRefPlanePoints(new TClonesArray("CbmRichPoint")),
   fRichMirrorPoints(new TClonesArray("CbmRichPoint")),
   fRotation(new TGeoRotation("", rx, ry, rz)),
   fPositionRotation(new TGeoCombiTrans(px, py, pz, fRotation))
{
   fVerboseLevel = 1;
}

CbmRich::~CbmRich()
{
   if (NULL != fRichPoints) {
      fRichPoints->Delete();
      delete fRichPoints;
   }
   if (NULL != fRichRefPlanePoints) {
      fRichRefPlanePoints->Delete();
      delete fRichRefPlanePoints;
   }

   if (NULL != fRichMirrorPoints) {
      fRichMirrorPoints->Delete();
      delete fRichMirrorPoints;
   }
}

void CbmRich::Initialize()
{
   //return;
   FairDetector::Initialize();
   FairRun* sim = FairRun::Instance();
   FairRuntimeDb* rtdb=sim->GetRuntimeDb();
   CbmGeoRichPar *par=(CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));
   TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
}


Bool_t CbmRich::CheckIfSensitive(std::string name)
{
   TString volName = name;
   if ( volName.Contains("rich1d") || volName.Contains("Sens_plane")){
      return kTRUE;
   }
   return kFALSE;
}


Bool_t CbmRich::ProcessHits(
      FairVolume* vol)
{
   Int_t pdgCode = gMC->TrackPid();
   Int_t iVol = vol->getMCid();
   TString volName = TString(vol->GetName());
  // cout << volName << endl;
   //Treat photodetectors : All particles
   if (volName.Contains("rich1d")){
      if (gMC->IsTrackEntering()){

         TParticle* part = gMC->GetStack()->GetCurrentTrack();
         Double_t charge = part->GetPDG()->Charge() / 3. ;
         Int_t trackID = gMC->GetStack()->GetCurrentTrackNumber();
         Double_t time = gMC->TrackTime() * 1.0e09;
         Double_t length = gMC->TrackLength();
         Double_t eLoss = gMC->Edep();
         TLorentzVector tPos, tMom;
         gMC->TrackPosition(tPos);
         gMC->TrackMomentum(tMom);

         if ( pdgCode == 50000050) { // Cherenkovs only
            AddHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()), TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, eLoss);

            // Increment number of RichPoints for this track
            CbmStack* stack = (CbmStack*) gMC->GetStack();
            stack->AddPoint(kRICH);
            return kTRUE;
         } else {
            if (charge == 0.) {
               return kFALSE; // no neutrals
            } else { // charged particles
               AddHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()), TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, eLoss);

               // Increment number of RichPoints for this track
               CbmStack* stack = (CbmStack*) gMC->GetStack();
               stack->AddPoint(kRICH);
               return kTRUE;
            }
         }
      }
   }

   // Treat imaginary plane in front of the mirrors: Only charged particles at entrance
   if (volName == "rich1gas2" || volName.Contains("Sens_plane")) {
	   //  cout << volName << endl;
      // Collecting points of tracks and imaginary plane intersection
      if ( gMC->IsTrackEntering() ) {
         TParticle* part    = gMC->GetStack()->GetCurrentTrack();
         Double_t charge = part->GetPDG()->Charge() / 3. ;

         if ( (fRegisterPhotonsOnSensitivePlane && pdgCode == 50000050) || (!fRegisterPhotonsOnSensitivePlane && charge != 0.) ) {
            Int_t trackID = gMC->GetStack()->GetCurrentTrackNumber();
            Double_t time = gMC->TrackTime() * 1.0e09;
            Double_t length = gMC->TrackLength();
            Double_t eLoss = gMC->Edep();
            TLorentzVector tPos, tMom;

            gMC->TrackPosition(tPos);
            gMC->TrackMomentum(tMom);

            AddRefPlaneHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()), TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, eLoss);

            //Increment number of RefPlanePoints for this track
            CbmStack* stack = (CbmStack*) gMC->GetStack();
            stack->AddPoint(kREF);
            return kTRUE;
         } else {
        	 return kFALSE;
         }
      }
   }

   // Treat mirror points
   if (volName.Contains("rich1mgl") || volName.Contains("rich1mglLU") || volName.Contains("rich1mglRU") ) {

      // Collecting points of tracks and imaginary plane intersection
      if (gMC->IsTrackEntering()) {
         TParticle* part = gMC->GetStack()->GetCurrentTrack();
         Double_t charge = part->GetPDG()->Charge() / 3.;
         if (charge == 0.) {
            return kFALSE; // no neutrals
         } else {

            Int_t trackID = gMC->GetStack()->GetCurrentTrackNumber();

            Double_t time = gMC->TrackTime() * 1.0e09;
            Double_t length = gMC->TrackLength();
            Double_t eLoss = gMC->Edep();
            TLorentzVector tPos, tMom;

            gMC->TrackPosition(tPos);
            gMC->TrackMomentum(tMom);

            // check number of STS points
            //UInt_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
            //Int_t nStsPoints = (points & 15);

            //if (nStsPoints > 0) { // store only particles with STSpoints (at least 1)
               AddMirrorHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()), TVector3(tMom.Px(), tMom.Py(), tMom.Pz()), time, length, eLoss);
               return kTRUE;
            //}
         }
      }
   }

   return kFALSE;
}

void CbmRich::EndOfEvent()
{
  if (fVerboseLevel)  Print();
  Reset();
}

void CbmRich::Register()
{
   FairRootManager::Instance()->Register("RichPoint","Rich", fRichPoints, kTRUE);
   FairRootManager::Instance()->Register("RefPlanePoint","RichRefPlane", fRichRefPlanePoints, kTRUE);
   FairRootManager::Instance()->Register("RichMirrorPoint","RichMirror", fRichMirrorPoints, kFALSE);
}

TClonesArray* CbmRich::GetCollection(
      Int_t iColl) const
{
   if (iColl == 0) return fRichPoints;
   if (iColl == 1) return fRichRefPlanePoints;
   if (iColl == 2) return fRichMirrorPoints;
   return NULL;
}

void CbmRich::Print() const
{
   Int_t nHits = fRichPoints->GetEntriesFast();
   LOG(INFO) << fName << ": " << nHits << " points registered in this event." << FairLogger::endl;

   if (fVerboseLevel > 1) for (Int_t i=0; i<nHits; i++) (*fRichPoints)[i]->Print();
}

void CbmRich::Reset()
{
   fRichPoints->Delete();
   fRichRefPlanePoints->Delete();
   fRichMirrorPoints->Delete();
   fPosIndex = 0;
}

void CbmRich::CopyClones(
      TClonesArray* cl1,
      TClonesArray* cl2,
      Int_t offset )
{
   Int_t nEntries = cl1->GetEntriesFast();
   LOG(INFO) << "CbmRich: " << nEntries << " entries to add." << FairLogger::endl;
   TClonesArray& clref = *cl2;
   CbmRichPoint* oldpoint = NULL;
   for (Int_t i=0; i< nEntries ; i++ ) {
      oldpoint = (CbmRichPoint*) cl1->At(i);
      Int_t index =  oldpoint->GetTrackID() + offset;
      oldpoint->SetTrackID(index);
      new (clref[fPosIndex]) CbmRichPoint(*oldpoint);
      fPosIndex++;
   }
   LOG(INFO) << "CbmRich: " << cl2->GetEntriesFast() << " merged entries." << FairLogger::endl;
}

void CbmRich::ConstructOpGeometry() 
{
   LOG(INFO) << "CbmRich::ConstructOpGeometry()" << FairLogger::endl;
}

void CbmRich::ConstructGeometry()
{
   TString fileName = GetGeometryFileName();
   if ( fileName.EndsWith(".root") ) {
       LOG(INFO) << "Constructing RICH geometry from ROOT  file " << fileName.Data() << FairLogger::endl;
       ConstructRootGeometry();
   } else if ( fileName.EndsWith(".geo") ) {
       LOG(INFO) << "Constructing RICH geometry from ASCII file " << fileName.Data() << FairLogger::endl;
       ConstructAsciiGeometry();
   } else if (fileName.EndsWith(".gdml") ) {
       LOG(INFO) << "Constructing RICH geometry from GDML  file " << fileName.Data() << FairLogger::endl;
       ConstructGdmlGeometry(fPositionRotation);
   } else {
      LOG(FATAL) << "Geometry format of RICH file " << fileName.Data()
                 << " not supported." << FairLogger::endl;
   }
}

void CbmRich::ConstructAsciiGeometry()
{
   FairGeoLoader* geoLoad = FairGeoLoader::Instance();
   FairGeoInterface* geoFace = geoLoad->getGeoInterface();
   CbmGeoRich* richGeo = new CbmGeoRich();
   richGeo->setGeomFile(GetGeometryFileName());
   geoFace->addGeoModule(richGeo);

   Bool_t rc = geoFace->readSet(richGeo);
   if (rc) richGeo->create(geoLoad->getGeoBuilder());
   TList* volList = richGeo->getListOfVolumes();

   // store geo parameter
   FairRun *fRun = FairRun::Instance();
   FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
   CbmGeoRichPar* par=(CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));
   TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
   TObjArray *fPassNodes = par->GetGeoPassiveNodes();

   TListIter iter(volList);
   FairGeoNode* node   = NULL;
   FairGeoVolume *aVol=NULL;

   while( (node = (FairGeoNode*)iter.Next()) ) {
      aVol = dynamic_cast<FairGeoVolume*> ( node );
      if ( node->isSensitive()  ) {
         fSensNodes->AddLast( aVol );
      }else{
         fPassNodes->AddLast( aVol );
      }
   }
   par->setChanged();
   par->setInputVersion(fRun->GetRunId(),1);

   ProcessNodes ( volList );

   // add support structure for mirrors
   TGeoMaterial * matAl = new TGeoMaterial("Al", 26.98, 13, 2.7);
   TGeoMedium * Al = new TGeoMedium("Al",1, matAl);
   TGeoVolume * volume = gGeoManager->MakeTube("grid", Al, 1.3, 1.5, 180);

   gGeoManager->Matrix(123456, 180, 0, 90, 90 , 90 , 0);//z rotation
   gGeoManager->Matrix(123457, 90, 0, 180, 0, 90, 90);// y rotation

   Double_t * buf = 0;
   for (Int_t i = 0; i< 11; i++) {
      if (i == 5) continue;
      gGeoManager->Node("grid", 2*i+1, "rich1gas3", 36*i - 180, 0, 40, 123457, kTRUE, buf, 0);
      gGeoManager->Node("grid", 2*i+2, "rich1gas3", 0, 36*i - 180, 48, 123456, kTRUE, buf, 0);
   }
}

void CbmRich::ConstructGdmlGeometry(TGeoMatrix* geoMatrix)
{
   TFile *old = gFile;
   TGDMLParse parser;
   TGeoVolume* gdmlTop;

   // Before importing GDML
   Int_t maxInd = gGeoManager->GetListOfMedia()->GetEntries() - 1;

   gdmlTop = parser.GDMLReadFile(GetGeometryFileName());

   // Cheating - reassigning media indices after GDML import (need to fix this in TGDMLParse class!!!)
//   for (Int_t i=0; i<gGeoManager->GetListOfMedia()->GetEntries(); i++)
//      gGeoManager->GetListOfMedia()->At(i)->Dump();
   // After importing GDML
   Int_t j = gGeoManager->GetListOfMedia()->GetEntries() - 1;
   Int_t curId;
   TGeoMedium* m;
   do {
      m = (TGeoMedium*)gGeoManager->GetListOfMedia()->At(j);
      curId = m->GetId();
      m->SetId(curId+maxInd);
      j--;
   } while (curId > 1);
//   LOG(DEBUG) << "====================================================================" << FairLogger::endl;
//   for (Int_t i=0; i<gGeoManager->GetListOfMedia()->GetEntries(); i++)
//      gGeoManager->GetListOfMedia()->At(i)->Dump();

   Int_t newMaxInd = gGeoManager->GetListOfMedia()->GetEntries() - 1;

   gGeoManager->GetTopVolume()->AddNode(gdmlTop,1,geoMatrix);
   ExpandNodeForGdml(gGeoManager->GetTopVolume()->GetNode(gGeoManager->GetTopVolume()->GetNdaughters()-1));

   for (Int_t k = maxInd+1; k < newMaxInd+1; k++) {
      TGeoMedium* medToDel = (TGeoMedium*)(gGeoManager->GetListOfMedia()->At(maxInd+1));
      LOG(DEBUG) << "    removing media " << medToDel->GetName() << " with id " << medToDel->GetId() << " (k=" << k << ")" << FairLogger::endl;
      gGeoManager->GetListOfMedia()->Remove(medToDel);
   }
   gGeoManager->SetAllIndex();

   gFile = old;
}

void CbmRich::ExpandNodeForGdml(TGeoNode* node)
{
   LOG(DEBUG) << "----------------------------------------- ExpandNodeForGdml for node " << node->GetName() << FairLogger::endl;

   TGeoVolume* curVol = node->GetVolume();

   LOG(DEBUG) << "    volume: " << curVol->GetName() << FairLogger::endl;

   if (curVol->IsAssembly()) {
      LOG(DEBUG) << "    skipping volume-assembly" << FairLogger::endl;
   } else {

      TGeoMedium* curMed = curVol->GetMedium();
      TGeoMaterial* curMat = curVol->GetMaterial();
      TGeoMedium* curMedInGeoManager = gGeoManager->GetMedium(curMed->GetName());
      TGeoMaterial* curMatOfMedInGeoManager = curMedInGeoManager->GetMaterial();
      TGeoMaterial* curMatInGeoManager = gGeoManager->GetMaterial(curMat->GetName());

      // Current medium and material assigned to the volume from GDML
      LOG(DEBUG2) << "    curMed\t\t\t\t" << curMed << "\t" << curMed->GetName() << "\t" << curMed->GetId() << FairLogger::endl;
      LOG(DEBUG2) << "    curMat\t\t\t\t" << curMat << "\t" << curMat->GetName() << "\t" << curMat->GetIndex() << FairLogger::endl;

      // Medium and material found in the gGeoManager - either the pre-loaded one or one from GDML
      LOG(DEBUG2) << "    curMedInGeoManager\t\t" << curMedInGeoManager
               << "\t" << curMedInGeoManager->GetName() << "\t" << curMedInGeoManager->GetId() << FairLogger::endl;
      LOG(DEBUG2) << "    curMatOfMedInGeoManager\t\t" << curMatOfMedInGeoManager
               << "\t" << curMatOfMedInGeoManager->GetName() << "\t" << curMatOfMedInGeoManager->GetIndex() << FairLogger::endl;
      LOG(DEBUG2) << "    curMatInGeoManager\t\t" << curMatInGeoManager
               << "\t" << curMatInGeoManager->GetName() << "\t" << curMatInGeoManager->GetIndex() << FairLogger::endl;

      TString matName = curMat->GetName();
      TString medName = curMed->GetName();

      if (curMed->GetId() != curMedInGeoManager->GetId()) {
         if (fFixedMedia.find(medName) == fFixedMedia.end()) {
            LOG(DEBUG) << "    Medium needs to be fixed" << FairLogger::endl;
            fFixedMedia[medName] = curMedInGeoManager;
            Int_t ind = curMat->GetIndex();
            gGeoManager->RemoveMaterial(ind);
            LOG(DEBUG) << "    removing material " << curMat->GetName()
               << " with index " << ind << FairLogger::endl;
            for (Int_t i=ind; i<gGeoManager->GetListOfMaterials()->GetEntries(); i++) {
               TGeoMaterial* m = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->At(i);
               m->SetIndex(m->GetIndex()-1);
            }

            LOG(DEBUG) << "    Medium fixed" << FairLogger::endl;
         } else {
            LOG(DEBUG) << "    Already fixed medium found in the list    " << FairLogger::endl;
         }

      } else {
         if (fFixedMedia.find(medName) == fFixedMedia.end()) {
            LOG(DEBUG) << "    There is no correct medium in the memory yet" << FairLogger::endl;

            FairGeoLoader* geoLoad = FairGeoLoader::Instance();
            FairGeoInterface* geoFace = geoLoad->getGeoInterface();
            FairGeoMedia* geoMediaBase =  geoFace->getMedia();
            FairGeoBuilder* geobuild = geoLoad->getGeoBuilder();

            FairGeoMedium* curMedInGeo = geoMediaBase->getMedium(medName);
            if (curMedInGeo == 0)
            {
               LOG(FATAL) << "    Media not found in Geo file." << FairLogger::endl;
               //! This should not happen.
               //! This means that somebody uses material in GDML that is not in the media.geo file.
               //! Most probably this is the sign to the user to check materials' names in the CATIA model.
            }
            else
            {
               LOG(DEBUG) << "    Found media in Geo file" << medName << FairLogger::endl;
               Int_t nmed = geobuild->createMedium(curMedInGeo);
               fFixedMedia[medName] = (TGeoMedium*)gGeoManager->GetListOfMedia()->Last();
               gGeoManager->RemoveMaterial(curMatOfMedInGeoManager->GetIndex());
               LOG(DEBUG) << "    removing material " << curMatOfMedInGeoManager->GetName()
                  << " with index " << curMatOfMedInGeoManager->GetIndex() << FairLogger::endl;
               for (Int_t i=curMatOfMedInGeoManager->GetIndex(); i<gGeoManager->GetListOfMaterials()->GetEntries(); i++) {
                  TGeoMaterial* m = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->At(i);
                  m->SetIndex(m->GetIndex()-1);
               }
            }

            if (curMedInGeo->getSensitivityFlag()) {
               LOG(DEBUG) << "    Adding sensitive  " << curVol->GetName() << FairLogger::endl;
               AddSensitiveVolume(curVol);
            }

         } else {
            LOG(DEBUG) << "    Already fixed medium found in the list" << FairLogger::endl;
            LOG(DEBUG) << "!!! Sensitivity: " << fFixedMedia[medName]->GetParam(0) << FairLogger::endl;
            if (fFixedMedia[medName]->GetParam(0) == 1) {
               LOG(DEBUG) << "    Adding sensitive  " << curVol->GetName() << FairLogger::endl;
               AddSensitiveVolume(curVol);
            }
         }
      }

      curVol->SetMedium(fFixedMedia[medName]);
      gGeoManager->SetAllIndex();

//      gGeoManager->GetListOfMaterials()->Print();
//      gGeoManager->GetListOfMedia()->Print();

   }

   //! Recursevly go down the tree of nodes
   if (curVol->GetNdaughters() != 0)
   {
      TObjArray* NodeChildList = curVol->GetNodes();
      TGeoNode* curNodeChild;
      for (Int_t j=0; j<NodeChildList->GetEntriesFast(); j++)
      {
         curNodeChild = (TGeoNode*)NodeChildList->At(j);
         ExpandNodeForGdml(curNodeChild);
      }
   }
}

CbmRichPoint* CbmRich::AddHit(
      Int_t trackID,
      Int_t detID,
      TVector3 pos,
      TVector3 mom,
      Double_t time,
      Double_t length,
      Double_t eLoss)
{
   TClonesArray& clref = *fRichPoints;
   Int_t size = clref.GetEntriesFast();
   return new(clref[size]) CbmRichPoint(trackID, detID, pos, mom, time,length, eLoss);
}

CbmRichPoint* CbmRich::AddRefPlaneHit(
      Int_t trackID,
      Int_t detID,
      TVector3 pos,
      TVector3 mom,
      Double_t time,
      Double_t length,
      Double_t eLoss)
{
   TClonesArray& clref = *fRichRefPlanePoints;
   Int_t tsize = clref.GetEntriesFast();
   return new(clref[tsize]) CbmRichPoint(trackID, detID, pos, mom, time,length, eLoss);
}

CbmRichPoint* CbmRich::AddMirrorHit(
      Int_t trackID,
      Int_t detID,
      TVector3 pos,
      TVector3 mom,
      Double_t time,
      Double_t length,
      Double_t eLoss)
{
   TClonesArray& clref = *fRichMirrorPoints;
   Int_t tsize = clref.GetEntriesFast();
   return new(clref[tsize]) CbmRichPoint(trackID, detID, pos, mom, time,length, eLoss);
}

ClassImp(CbmRich)
