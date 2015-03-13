// -------------------------------------------------------------------------
// -----                        CbmMvd source file                     -----
// -----                  Created 26/07/04  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmMvd.h"

#include "CbmMvdGeo.h"
#include "CbmMvdGeoPar.h"
#include "CbmMvdPoint.h"

#include "CbmDetectorList.h"
#include "CbmStack.h"
#include "tools/CbmMvdGeoHandler.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoVolume.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "FairVolume.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TList.h"
#include "TObjArray.h"
#include "TParticle.h"
#include "TString.h"
#include "TVirtualMC.h"
#include "TVirtualMCStack.h"

#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvd::CbmMvd() 
  : FairDetector("MVD", kTRUE, kMVD), 
    fTrackID(0),
    fPdg(0),
    fVolumeID(0),
    fPosIn(0.0, 0.0, 0.0, 0.0),
    fPosOut(0.0, 0.0, 0.0, 0.0),
    fMomIn(0.0, 0.0, 0.0, 0.0),
    fMomOut(0.0, 0.0, 0.0, 0.0),
    fTime(0.),
    fLength(0.),
    fELoss(0.),
    fPosIndex(0),
    fCollection(new TClonesArray("CbmMvdPoint")),
    kGeoSaved(kFALSE),
    fGeoPar(new TList()),
    fStationMap()
{
  ResetParameters();
  fGeoPar->SetName( GetName());
  fVerboseLevel = 1;

}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvd::CbmMvd(const char* name, Bool_t active) 
  : FairDetector(name, active, kMVD),
    fTrackID(0),
    fPdg(0),
    fVolumeID(0),
    fPosIn(0.0, 0.0, 0.0, 0.0),
    fPosOut(0.0, 0.0, 0.0, 0.0),
    fMomIn(0.0, 0.0, 0.0, 0.0),
    fMomOut(0.0, 0.0, 0.0, 0.0),
    fTime(0.),
    fLength(0.),
    fELoss(0.),
    fPosIndex(0),
    fCollection(new TClonesArray("CbmMvdPoint")),
    kGeoSaved(kFALSE),
    fGeoPar(new TList()),
    fStationMap()
 {
  fGeoPar->SetName( GetName());
  fVerboseLevel = 1;
 
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvd::~CbmMvd() {
  if ( fGeoPar ) {
    fGeoPar->Delete();
    delete fGeoPar;
  }
  if (fCollection) {
    fCollection->Delete();
    delete fCollection;
  }
}
// -------------------------------------------------------------------------



// -----   Virtual public method ProcessHits  ------------------------------
Bool_t CbmMvd::ProcessHits(FairVolume* vol) {


  // Store track parameters at entrance of sensitive volume
  if ( gMC->IsTrackEntering() ) {
    fPdg    = gMC->TrackPid();
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create CbmMvdPoint.
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
    if (fELoss == 0. ) return kFALSE;
    AddHit(fTrackID, fPdg, fStationMap[fVolumeID],
	   TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	   TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	   TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	   TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	   fTime, fLength, fELoss);

    // Increment number of MvdPoints for this track
    CbmStack* stack = (CbmStack*) gMC->GetStack();
    stack->AddPoint(kMVD);
    
    ResetParameters();
  }

  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Public method BeginEvent   --------------------------------------
void CbmMvd::BeginEvent() { }
// -------------------------------------------------------------------------



// -----   Virtual public method EndOfEvent   ------------------------------
void CbmMvd::EndOfEvent() {
  if (fVerboseLevel) Print();
  //  fCollection->Clear();
  fCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Virtual public method Register   --------------------------------
void CbmMvd::Register() {
  FairRootManager::Instance()->Register("MvdPoint", GetName(), 
				       fCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -----   Virtual public method GetCollection   ---------------------------
TClonesArray* CbmMvd::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fCollection;
  else return NULL;
}
// -------------------------------------------------------------------------



// -----   Virtual public method Print   -----------------------------------
void CbmMvd::Print() const {
  Int_t nHits = fCollection->GetEntriesFast();
  LOG(INFO) << fName << ": " << nHits << " points registered in this event." << FairLogger::endl;
}
// -------------------------------------------------------------------------



// -----   Virtual public method Reset   -----------------------------------
void CbmMvd::Reset() {
  //  fCollection->Clear();
  fCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -----   Virtual public method CopyClones   ------------------------------
void CbmMvd::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  LOG(INFO) << "CbmMvd: " << nEntries << " entries to add." << FairLogger::endl;
  TClonesArray& clref = *cl2;
  CbmMvdPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (CbmMvdPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) CbmMvdPoint(*oldpoint);
    fPosIndex++;
  }
  LOG(INFO) << "CbmMvd: " << cl2->GetEntriesFast() << " merged entries." << FairLogger::endl;
}
// -------------------------------------------------------------------------

// --------Pulic method ConstructGeometry()-----------------------------------------------------------------
void CbmMvd::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if ( fileName.EndsWith(".root") ) 
	{
	LOG(INFO) << "Constructing MVD  geometry from ROOT  file "
	<< fileName.Data() << FairLogger::endl;
    	ConstructRootGeometry();
  	}
  else if ( fileName.EndsWith(".geo") ) 
	{
    	LOG(INFO) <<  "Constructing MVD  geometry from ASCII file "
	<< fileName.Data() << FairLogger::endl;
    	ConstructAsciiGeometry();
 	}
  else
    LOG(FATAL) <<  "Geometry format of MVD file " << fileName.Data()
		 << " not supported." << FairLogger::endl;
}


// -----   Virtual public method ConstructAsciiGeometry   -----------------------
void CbmMvd::ConstructAsciiGeometry() {
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  CbmMvdGeo*       mvdGeo  = new CbmMvdGeo();
  mvdGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(mvdGeo);

  Bool_t rc = geoFace->readSet(mvdGeo);
  if (rc) mvdGeo->create(geoLoad->getGeoBuilder());
  TList* volList = mvdGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmMvdGeoPar* geoPar = (CbmMvdGeoPar*)(rtdb->getContainer("CbmMvdGeoPar"));
  TObjArray *fSensNodes = geoPar->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = geoPar->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode*   node = NULL;
  FairGeoVolume* vol  = NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
    vol = dynamic_cast<FairGeoVolume*> ( node );
    if ( node->isSensitive()  ) {
	   LOG(DEBUG) << "Add: "<< vol->GetName() << FairLogger::endl;
           fSensNodes->AddLast( vol );
       }else{
           fPassNodes->AddLast( vol );
       }
  }
  geoPar->setChanged();
  geoPar->setInputVersion(fRun->GetRunId(),1);
  ProcessNodes( volList );
  
  // Fill map of station numbers
  Int_t iStation =  1;
  Int_t volId    = -1;
  do {
    TString volName = Form("mvdstation%02i", iStation);
    volId = gGeoManager->GetUID(volName);
    if (volId > -1 ) {
      fStationMap[volId] = iStation;
      LOG(INFO) << GetName() << "::ConstructAsciiGeometry: "
           << "Station No. " << iStation << ", volume ID " << volId 
	   << ", volume name " << volName << FairLogger::endl;
      iStation++;
    }
  } while ( volId > -1 );

 
}
// -------------------------------------------------------------------------

// --------    Public method ConstructRootGeometry     ---------------------
void CbmMvd::ConstructRootGeometry() // added 05.05.14 by P. Sitzmann
{
FairDetector::ConstructRootGeometry();
CbmMvdGeoHandler* mvdHandler = new CbmMvdGeoHandler();
mvdHandler->Init(kTRUE);
mvdHandler->Fill();
fStationMap = mvdHandler->GetMap();
}
// -------------------------------------------------------------------------


// -----   Private method AddHit   --------------------------------------------
CbmMvdPoint* CbmMvd::AddHit(Int_t trackID, Int_t pdg, Int_t stationNr, 
			    TVector3 posIn, TVector3 posOut, 
			    TVector3 momIn, TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    LOG(INFO) << "CbmMvd: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  station " << stationNr << ", track "
         << trackID << ", energy loss " << eLoss*1e06 << " keV" << FairLogger::endl;
  return new(clref[size]) CbmMvdPoint(trackID, pdg, stationNr, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}
// ----------------------------------------------------------------------------

Bool_t CbmMvd::CheckIfSensitive(std::string name)
{
  TString tsname = name; 
  if (tsname.Contains("MimosaActive") || tsname.Contains("mvdstation") && !(tsname.Contains("PartAss")) ){
    //cout<<"*** Register "<<tsname<<" as active volume."<<endl;
    return kTRUE;
  }
  else if(tsname.EndsWith("-P0"))    
      {
	if(fVerboseLevel>1)
      std::cout<<"*** Register "<<tsname<<" as active volume."<<endl;
      
      return kTRUE;
      }
  return kFALSE;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------



ClassImp(CbmMvd)
