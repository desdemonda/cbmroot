/**
 * \file CbmTrd.cxx
 * \author V.Friese <v.friese@gsi.de>
 * \date 27.07.2004
 **/

#include "CbmTrd.h"

#include "CbmGeoTrdPar.h"
#include "CbmTrdPoint.h"
#include "CbmGeoTrd.h"
#include "CbmTrdGeoHandler.h"
#include "CbmStack.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"
#include "FairLogger.h"

#include "TObjArray.h"
#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   ----------------------------------------------
CbmTrd::CbmTrd() 
  : FairDetector("TRD", kTRUE, kTRD),
    fPosIn(),
    fMomIn(),
    fPosOut(),
    fMomOut(),
    fTime(0),
    fLength(0),
    fELoss(0),
    fPosIndex(0),
    fTrdPoints(new TClonesArray("CbmTrdPoint")),
    fGeoHandler(new CbmTrdGeoHandler())
{
  fVerboseLevel = 1;
}
// ----------------------------------------------------------------------------


// -----   Standard constructor   ---------------------------------------------
CbmTrd::CbmTrd(const char* name, Bool_t active)
  : FairDetector(name, active, kTRD),
    fPosIn(),
    fMomIn(),
    fPosOut(),
    fMomOut(),
    fTime(0),
    fLength(0),
    fELoss(0),
    fPosIndex(0),
    fTrdPoints(new TClonesArray("CbmTrdPoint")),
    fGeoHandler(new CbmTrdGeoHandler())
{
  fVerboseLevel = 1;
}
// ----------------------------------------------------------------------------


// -----   Destructor   -------------------------------------------------------
CbmTrd::~CbmTrd() {
   if (fTrdPoints) {
      fTrdPoints->Delete();
      delete fTrdPoints;
   }
   if (fGeoHandler) {
      delete fGeoHandler;
   }
}
// ----------------------------------------------------------------------------


// -----   Initialize   -------------------------------------------------------
void CbmTrd::Initialize()
{
   FairDetector::Initialize();

   // Initialize the CbmTrdGeoHandler helper class from the
   // TVirtualMC interface
   Bool_t isSimulation=kTRUE;
   fGeoHandler->Init(isSimulation);
}
// ----------------------------------------------------------------------------


// -----   SetSpecialPhysicsCuts   --------------------------------------------
void CbmTrd::SetSpecialPhysicsCuts()
{
   FairRun* fRun = FairRun::Instance();

   // Setting the properties for the TRDgas is only tested
   // with TGeant3, so we check for the simulation engine
   // and change the standard properties only for TGeant3
   if (strcmp(fRun->GetName(),"TGeant3") == 0) {

      // Get Material Id and some material properties from
      // the geomanager
      Int_t mat = gGeoManager->GetMaterialIndex("TRDgas");
      TGeoMaterial *trdgas =  gGeoManager->GetMaterial(mat);
      Double_t mass = trdgas->GetA();
      Double_t charge = trdgas->GetZ();

      // Get the material properties for material with id+1
      // (of-by-one problem) from the Virtual Monte Carlo
      Int_t matIdVMC = mat+1;

      TString name="";
      Double_t a=0.;
      Double_t z=0.;
      Double_t dens=0.;
      Double_t radl=0.;
      Double_t inter=0.;
      TArrayD par;

      gMC->GetMaterial(matIdVMC, name, a, z, dens, radl, inter, par);

      // Check if the material properties are the same for TGeoManager
      // and TVirtualMC. If not stop the execution of the simulation.
      // This is done to stop the program whenever the
      // of-by-one problem is fixed in the Virtual Monte Carlo
      if ((TMath::Abs(mass - a) > 0.0001) || (TMath::Abs(charge - z)) > 0.0001) {
         LOG(FATAL) << "**********************************" << FairLogger::endl;
         LOG(FATAL) << TMath::Abs(mass - a) << " , " << TMath::Abs(charge - z) << FairLogger::endl;
         LOG(FATAL) << "Parameters from Geomanager:" << FairLogger::endl;
         trdgas->Print();
         LOG(FATAL) << "Parameters from Virtual Montecarlo:" << FairLogger::endl;
         LOG(FATAL) << "Name " << name << " Aeff=" << a << " Zeff=" << z << FairLogger::endl;
         Fatal("CbmTrd::SetSpecialPhysicsCuts", "Material parameters different between TVirtualMC and TGeomanager");
      }

      // Set new properties, physics cuts etc. for the TRDgas
      gMC->Gstpar(matIdVMC,"STRA",1.0);
      gMC->Gstpar(matIdVMC,"PAIR",1.0);
      gMC->Gstpar(matIdVMC,"COMP",1.0);
      gMC->Gstpar(matIdVMC,"PHOT",1.0);
      gMC->Gstpar(matIdVMC,"LOSS",2.0);
      //gMC->Gstpar(matIdVMC,"PFIS",0.0);
      gMC->Gstpar(matIdVMC,"ANNI",1.0);
      gMC->Gstpar(matIdVMC,"BREM",1.0);
      gMC->Gstpar(matIdVMC,"HADR",1.0);
      //gMC->Gstpar(matIdVMC,"MUNU",1.0);
      gMC->Gstpar(matIdVMC,"DCAY",1.0);
      gMC->Gstpar(matIdVMC,"MULS",1.0);
      //gMC->Gstpar(matIdVMC,"LABS",0.0);
      gMC->Gstpar(matIdVMC,"DRAY",1.0);
      gMC->Gstpar(matIdVMC,"RAYL",1.0);
      gMC->Gstpar(matIdVMC,"CUTELE",10.e-6);
      gMC->Gstpar(matIdVMC,"CUTGAM",10.e-6);
      gMC->Gstpar(matIdVMC,"CUTNEU",10.e-4);
      gMC->Gstpar(matIdVMC,"CUTHAD",10.e-4);
      gMC->Gstpar(matIdVMC,"CUTMUO",10.e-6);
      gMC->Gstpar(matIdVMC,"BCUTE",10.e-6);
      gMC->Gstpar(matIdVMC,"BCUTM",10.e-6);
      gMC->Gstpar(matIdVMC,"DCUTE",10.e-6);
      gMC->Gstpar(matIdVMC,"DCUTM",10.e-6);
      gMC->Gstpar(matIdVMC,"PPCUTM",-1.);
   }
}
// ----------------------------------------------------------------------------


// -----   Public method ProcessHits   ----------------------------------------
Bool_t  CbmTrd::ProcessHits(
      FairVolume* vol)
{
   // Set parameters at entrance of volume. Reset ELoss.
   if ( gMC->IsTrackEntering() ) {
      fELoss   = 0.;
      fTime    = gMC->TrackTime() * 1.0e09;
      fLength  = gMC->TrackLength();
      gMC->TrackPosition(fPosIn);
      gMC->TrackMomentum(fMomIn);
   }

   // Sum energy loss for all steps in the active volume
   fELoss += gMC->Edep();

   // Create CbmTrdPoint at exit of active volume
   if ( gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()) {

      gMC->TrackPosition(fPosOut);
      gMC->TrackMomentum(fMomOut);

      if (fELoss == 0.) return kFALSE;  // no neutrals
      
      Int_t trackId = gMC->GetStack()->GetCurrentTrackNumber();
      Int_t moduleAddress = fGeoHandler->GetModuleAddress();

      Int_t size = fTrdPoints->GetEntriesFast();
      new ((*fTrdPoints)[size]) CbmTrdPoint(trackId, moduleAddress,
            TVector3(fPosIn.X(),  fPosIn.Y(),  fPosIn.Z()), TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
            TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()), TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
            fTime, fLength, fELoss);

      // Increment number of trd points in TParticle
      CbmStack* stack = (CbmStack*) gMC->GetStack();
      stack->AddPoint(kTRD);
      
      ResetParameters();
   }

   return kTRUE;
}
// ----------------------------------------------------------------------------


// -----   Public method EndOfEvent   -----------------------------------------
void CbmTrd::EndOfEvent()
{
   if (fVerboseLevel) Print();
   fTrdPoints->Delete();
   fPosIndex = 0;
}
// ----------------------------------------------------------------------------


// -----   Public method Register   -------------------------------------------
void CbmTrd::Register()
{
   FairRootManager::Instance()->Register("TrdPoint", "Trd", fTrdPoints, kTRUE);
}
// ----------------------------------------------------------------------------


// -----   Public method GetCollection   --------------------------------------
TClonesArray* CbmTrd::GetCollection(
      Int_t iColl) const
{
   if (iColl == 0) 
     return fTrdPoints; 
   else 
     return NULL;
}
// ----------------------------------------------------------------------------


// -----   Public method Print   ----------------------------------------------
void CbmTrd::Print() const
{
  Int_t nHits = fTrdPoints->GetEntriesFast();
  LOG(INFO) << fName << ": " << nHits << " points registered in this event." << FairLogger::endl;

  if (fVerboseLevel > 1)
    for (Int_t i=0; i<nHits; i++)
      (*fTrdPoints)[i]->Print();
}
// ----------------------------------------------------------------------------


// -----   Public method Reset   ----------------------------------------------
void CbmTrd::Reset()
{
   fTrdPoints->Delete();
   ResetParameters();
}
// ----------------------------------------------------------------------------


// -----   Public method CopyClones   -----------------------------------------
void CbmTrd::CopyClones(
      TClonesArray* cl1,
      TClonesArray* cl2,
      Int_t offset)
{
   Int_t nEntries = cl1->GetEntriesFast();
   LOG(INFO) << "CbmTrd: " << nEntries << " entries to add." << FairLogger::endl;
   TClonesArray& clref = *cl2;
   for (Int_t i = 0; i < nEntries; i++) {
      CbmTrdPoint* oldpoint = (CbmTrdPoint*) cl1->At(i);
      Int_t index = oldpoint->GetTrackID() + offset;
      oldpoint->SetTrackID(index);
      new (clref[fPosIndex]) CbmTrdPoint(*oldpoint);
      fPosIndex++;
   }
   LOG(INFO) << "CbmTrd: " << cl2->GetEntriesFast() << " merged entries." << FairLogger::endl;
}
// ----------------------------------------------------------------------------


// -----  ConstructGeometry  --------------------------------------------------
void CbmTrd::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if ( fileName.EndsWith(".root") ) {
    LOG(INFO) << "Constructing TRD  geometry from ROOT  file "
              << fileName.Data() << FairLogger::endl;
    ConstructRootGeometry();
  }
  else if ( fileName.EndsWith(".geo") ) {
    LOG(INFO) <<  "Constructing TRD  geometry from ASCII file "
              << fileName.Data() << FairLogger::endl;
    ConstructAsciiGeometry();
  }
  else
    LOG(FATAL) <<  "Geometry format of TRD file " << fileName.Data()
               << " not supported." << FairLogger::endl;
}
// ----------------------------------------------------------------------------


// -----   ConstructAsciiGeometry   -------------------------------------------
void CbmTrd::ConstructAsciiGeometry()
{
   FairGeoLoader* geoLoad = FairGeoLoader::Instance();
   FairGeoInterface* geoFace = geoLoad->getGeoInterface();
   CbmGeoTrd* trdGeo = new CbmGeoTrd();
   trdGeo->setGeomFile(GetGeometryFileName());
   geoFace->addGeoModule(trdGeo);

   Bool_t rc = geoFace->readSet(trdGeo);
   if (rc) trdGeo->create(geoLoad->getGeoBuilder());
   TList* volList = trdGeo->getListOfVolumes();

   // Store TRD geometry parameter
   FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
   CbmGeoTrdPar* par = (CbmGeoTrdPar*)(rtdb->getContainer("CbmGeoTrdPar"));
   TObjArray* sensitiveNodes = par->GetGeoSensitiveNodes();
   TObjArray* passiveNodes = par->GetGeoPassiveNodes();

   TListIter iter(volList);
   FairGeoNode* node = NULL;
   FairGeoVolume* aVol = NULL;

   while ((node = (FairGeoNode*)iter.Next())) {
      aVol = dynamic_cast<FairGeoVolume*> ( node );
      if (node->isSensitive()) {
         sensitiveNodes->AddLast(aVol);
      } else {
         passiveNodes->AddLast(aVol);
      }
   }
   par->setChanged();
   par->setInputVersion(FairRun::Instance()->GetRunId(), 1);

   ProcessNodes(volList);
}
// ----------------------------------------------------------------------------


// -----   CheckIfSensitive   -------------------------------------------------
Bool_t CbmTrd::CheckIfSensitive(
      string name)
{
   TString tsname = name;
   if (tsname.Contains("gas")){
      LOG(DEBUG) << "CbmTrd::CheckIfSensitive: Register active volume: " << tsname << FairLogger::endl;
      return kTRUE;
   }
   return kFALSE;
}
// ----------------------------------------------------------------------------


ClassImp(CbmTrd)
