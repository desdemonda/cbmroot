
#include "CbmRichProt.h"

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
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TParticlePDG.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TGeoNode.h"

#include <iostream>

using std::cout;
using std::endl;

CbmRichProt::CbmRichProt() :
   FairDetector("RICH", kTRUE, kRICH),
   fPosIndex(0),
   volDetector(0),
   volRefPlane(0),
   volMir(0),
   volMir1(0),
   volMir2(0),
   fRichCollection(NULL),
   fRichRefPlaneCollection(NULL),
   fRichMirrorCollection(NULL)
{
  fRichCollection = new TClonesArray("CbmRichPoint");
  fRichRefPlaneCollection = new TClonesArray("CbmRichPoint");
  fRichMirrorCollection  = new TClonesArray("CbmRichPoint");
  
  fVerboseLevel = 1;
}

CbmRichProt::CbmRichProt(
      const char* name,
      Bool_t active):
   FairDetector(name, active, kRICH),
   fPosIndex(0),
   volDetector(0),
   volRefPlane(0),
   volMir(0),
   volMir1(0),
   volMir2(0),
   fRichCollection(NULL),
   fRichRefPlaneCollection(NULL),
   fRichMirrorCollection(NULL)
{
   fRichCollection = new TClonesArray("CbmRichPoint");
   fRichRefPlaneCollection = new TClonesArray("CbmRichPoint");
   fRichMirrorCollection  = new TClonesArray("CbmRichPoint");
  
   fVerboseLevel = 1;
}

CbmRichProt::~CbmRichProt()
{
   if (NULL != fRichCollection) {
      fRichCollection->Delete();
      delete fRichCollection;
   }
   if (NULL != fRichRefPlaneCollection) {
      fRichRefPlaneCollection->Delete();
      delete fRichRefPlaneCollection;
   }
   if (NULL != fRichMirrorCollection) {
      fRichMirrorCollection->Delete();
      delete fRichMirrorCollection;
   }
}

void CbmRichProt::Initialize()
{
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  CbmGeoRichPar *par=(CbmGeoRichPar*)(rtdb->getContainer("CbmGeoRichPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  //	TObjArray *fPassNodes = par->GetGeoPassiveNodes();
  /*
    FairGeoNode *fn;
    for (Int_t i=0; i< fSensNodes->GetEntries();i++)
    {
    fn=(FairGeoNode *)fSensNodes->At(i);
    cout << "Volume Id :" << fn->getMCid()<< "  "  << fn->getCopyNo()<<	endl;
    }
  */
  FairGeoNode *fm1= (FairGeoNode *) fSensNodes->FindObject("rich1detector");
//  FairGeoNode *fm2= (FairGeoNode *) fSensNodes->FindObject("rich1gas2");
  FairGeoNode *fm3= (FairGeoNode *) fSensNodes->FindObject("rich1mgl1");
//  FairGeoNode *fm3a= (FairGeoNode *) fSensNodes->FindObject("rich1mglLU#1");
//  FairGeoNode *fm3b= (FairGeoNode *) fSensNodes->FindObject("rich1mglRU#1");

  volDetector = fm1->getMCid();
//  volRefPlane  = fm2->getMCid();
  volMir  = fm3->getMCid();
//  volMir1  = fm3a->getMCid();
//  volMir2  = fm3b->getMCid();

//  if(volDetector == volRefPlane) {
//    cout<<"Det"<<volDetector<<" ImPl"<<volRefPlane<<endl;
//    Fatal("","");
//  }
}

Bool_t CbmRichProt::ProcessHits(
      FairVolume* vol)
{
  // Get track information
  // gMC is of type TVirtualMC
 
//  Int_t      pdgCode = part->GetPdgCode();
  Int_t      pdgCode = gMC->TrackPid();
  Int_t      iVol    = vol->getMCid(); 
  //cout << "CbmRichProt::ProcessHits"   <<  pdgCode  <<"in volume " << iVol << endl;
  // Treat photodetectors : All particles 
  // Nota bene: Energy loss at the entrance step is zero!

  if (iVol == volDetector) {

    if (gMC->IsTrackEntering()){

      TParticle* part    = gMC->GetStack()->GetCurrentTrack();
      Double_t charge = part->GetPDG()->Charge() / 3. ;

      // Create CbmRichPoint
      Int_t    trackID = gMC->GetStack()->GetCurrentTrackNumber();
      Double_t time    = gMC->TrackTime() * 1.0e09;
      Double_t length  = gMC->TrackLength();
      Double_t eLoss   = gMC->Edep();
      TLorentzVector tPos, tMom;
      gMC->TrackPosition(tPos);
      gMC->TrackMomentum(tMom);

      if ( pdgCode == 50000050) {
      AddHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	     TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
	     time, length, eLoss);
	     
	     
         // Increment number of RichPoints for this track
         CbmStack* stack = (CbmStack*) gMC->GetStack();
         stack->AddPoint(kRICH);
    
    /*
       // Increment number of rich points in TParticle (for Mother of Cherenkovs)
        Int_t motherID = gMC->GetStack()->GetCurrentTrack()->GetFirstMother();
	TParticle *mother = ((CbmStack *) gMC->GetStack())->GetParticle(motherID);
	Int_t pointsM = mother->GetMother(1);
        Int_t nRichPointsM = (pointsM & (15<<4)) >> 4;
        nRichPointsM++;
        if (nRichPointsM > 15) nRichPointsM = 15;        // max number = 15
        pointsM = ( pointsM & ( ~ (15<<4) ) ) | (nRichPointsM << 4);
        mother->SetMother(1,pointsM);

      // Count RichPoint for Cherenkov too - otherwise it would not be stored in CbmMCTrack
      Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
      Int_t nRichPoints = (points & (15<<4)) >> 4;
      nRichPoints++;
      if (nRichPoints > 15) nRichPoints = 15;        // max number = 15
      points = ( points & ( ~ (15<<4) ) ) | (nRichPoints << 4);
      gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
      */
      
      return kTRUE;
    }    // Cherenkovs only
    else
     {
      if (charge == 0.) {
	return kFALSE; // no neutrals
      }
      else {
	   AddHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	   	         TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
		         time, length, eLoss);
			 
	 // Increment number of RichPoints for this track
         CbmStack* stack = (CbmStack*) gMC->GetStack();
         stack->AddPoint(kRICH);

/*
	// Increment number of rich points in TParticle
	Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
        Int_t nRichPoints = (points & (15<<4)) >> 4;
        nRichPoints++;
        if (nRichPoints > 15) nRichPoints = 15;        // max number = 15
        points = ( points & ( ~ (15<<4) ) ) | (nRichPoints << 4);
        gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
	*/
	
	   return kTRUE;
//	}
      }  // charged particles
    }

  }
  }


  if (iVol == volRefPlane) { // Treat imaginary plane : Only charged particles at entrance
    // Collecting points of tracks and imaginary plane intersection
    if ( gMC->IsTrackEntering() ) {
      TParticle* part    = gMC->GetStack()->GetCurrentTrack();
      Double_t charge = part->GetPDG()->Charge() / 3. ;
      if (charge == 0.) {
	return kFALSE; // no neutrals
      }else {

	Int_t    trackID = gMC->GetStack()->GetCurrentTrackNumber();

	Double_t time    = gMC->TrackTime() * 1.0e09;
	Double_t length  = gMC->TrackLength();
	Double_t eLoss   = gMC->Edep();
	TLorentzVector tPos, tMom;

	gMC->TrackPosition(tPos);
	gMC->TrackMomentum(tMom);

	// check number of STS points
	
        //UInt_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
        //Int_t nStsPoints = (points & 15);

        //if (nStsPoints > 0) {              // store only particles with STSpoints (at least 1)
	   AddRefPlaneHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	   	         TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
		         time, length, eLoss);
        			 
        // Increment number of RefPlanePoints for this track
	CbmStack* stack = (CbmStack*) gMC->GetStack();
	stack->AddPoint(kREF);
	

	/* Number of Rich points in ImPlane not needed as they will never be recorded anyhow
	   // Increment number of rich ImPlane points in TParticle
	   Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
	   Int_t nRichPoints = (points & (15<<4));
	   nRichPoints += 10;
	   if (nRichPoints > 15) nRichPoints = 15;
	   points = ( points & ( ~ (15<<4) ) ) | (nRichPoints << 4);
	   gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
    */

	   return kTRUE;
	// }
      }
    }
  }

/*
  if (iVol == volMir || iVol == volMir1 || iVol == volMir2) { // Treat mirror : Only charged particles at entrance
    // Collecting points of tracks and imaginary plane intersection
    if ( gMC->IsTrackEntering() ) {
      TParticle* part    = gMC->GetStack()->GetCurrentTrack();
      Double_t charge = part->GetPDG()->Charge() / 3. ;
      if (charge == 0.) {
	return kFALSE; // no neutrals
      }else {

	Int_t    trackID = gMC->GetStack()->GetCurrentTrackNumber();

	Double_t time    = gMC->TrackTime() * 1.0e09;
	Double_t length  = gMC->TrackLength();
	Double_t eLoss   = gMC->Edep();
	TLorentzVector tPos, tMom;

	gMC->TrackPosition(tPos);
	gMC->TrackMomentum(tMom);


	// check number of STS points
        UInt_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
        Int_t nStsPoints = (points & 15);

        if (nStsPoints > 0) {              // store only particles with STSpoints (at least 1)
	   AddMirrorHit(trackID, iVol, TVector3(tPos.X(), tPos.Y(), tPos.Z()),
	   	         TVector3(tMom.Px(), tMom.Py(), tMom.Pz()),
		         time, length, eLoss);

	 Number of Rich points in ImPlane not needed as they will never be recorded anyhow
	   // Increment number of rich ImPlane points in TParticle
	   Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
	   Int_t nRichPoints = (points & (15<<4));
	   nRichPoints += 10;
	   if (nRichPoints > 15) nRichPoints = 15;
	   points = ( points & ( ~ (15<<4) ) ) | (nRichPoints << 4);
	   gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
    

	   return kTRUE;
	}
      }
    }
  }
*/
  return kFALSE;
}

void CbmRichProt::EndOfEvent()
{
  if (fVerboseLevel)  Print();
  Reset();
}

void CbmRichProt::Register() {
  FairRootManager::Instance()->Register("RichPoint","Rich", fRichCollection, kTRUE);
  FairRootManager::Instance()->Register("RefPlanePoint","RichRefPlane",
  				       fRichRefPlaneCollection, kTRUE);
  FairRootManager::Instance()->Register("RichMirrorPoint","RichMirror",
  				       fRichMirrorCollection, kFALSE);  // fFALSE -> MirrorPoint not saved
}

TClonesArray* CbmRichProt::GetCollection(
      Int_t iColl) const
{
  if (iColl == 0) return fRichCollection;
  if (iColl == 1) return fRichRefPlaneCollection;
  if (iColl == 2) return fRichMirrorCollection;
  return NULL;
}

void CbmRichProt::Print() const
{
   Int_t nHits = fRichCollection->GetEntriesFast();
   cout << "-I- CbmRichProt: " << nHits << " points registered in this event." << endl;
	
   if (fVerboseLevel>1)
     for (Int_t i=0; i<nHits; i++) (*fRichCollection)[i]->Print();
}

void CbmRichProt::Reset()
{
   fRichCollection->Clear();
   fRichRefPlaneCollection->Clear();
   fRichMirrorCollection->Clear();
   fPosIndex = 0;
}

void CbmRichProt::CopyClones(
      TClonesArray* cl1,
      TClonesArray* cl2,
      Int_t offset )
{
   Int_t nEntries = cl1->GetEntriesFast();
   cout << "-I- CbmRichProt: " << nEntries << " entries to add." << endl;
   TClonesArray& clref = *cl2;
   CbmRichPoint* oldpoint = NULL;
   for (Int_t i=0; i< nEntries ; i++ ) {
      oldpoint = (CbmRichPoint*) cl1->At(i);
      Int_t index =  oldpoint->GetTrackID() + offset;
      oldpoint->SetTrackID(index);
      new (clref[fPosIndex]) CbmRichPoint(*oldpoint);
      fPosIndex++;
   }
   cout << "-I- CbmRichProt: " << cl2->GetEntriesFast() << " merged entries." << endl;
}

void CbmRichProt::ConstructOpGeometry() 
{

}

void CbmRichProt::ConstructGeometry()
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
}

CbmRichPoint* CbmRichProt::AddHit(
      Int_t trackID,
      Int_t detID,
      TVector3 pos,
		TVector3 mom,
		Double_t time,
		Double_t length,
		Double_t eLoss)
{
  TClonesArray& clref = *fRichCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) CbmRichPoint(trackID, detID, pos, mom, time,length, eLoss);
}

CbmRichPoint* CbmRichProt::AddRefPlaneHit(
      Int_t trackID,
      Int_t detID,
		TVector3 pos,
		TVector3 mom,
		Double_t time,
		Double_t length,
		Double_t eLoss)
{
   TClonesArray& clref = *fRichRefPlaneCollection;
   Int_t tsize = clref.GetEntriesFast();
   return new(clref[tsize]) CbmRichPoint(trackID, detID, pos, mom, time, length, eLoss);
}

CbmRichPoint* CbmRichProt::AddMirrorHit(
      Int_t trackID,
      Int_t detID,
		TVector3 pos,
		TVector3 mom,
		Double_t time,
		Double_t length,
		Double_t eLoss)
{
   TClonesArray& clref = *fRichMirrorCollection;
   Int_t tsize = clref.GetEntriesFast();
   return new(clref[tsize]) CbmRichPoint(trackID, detID, pos, mom, time,length, eLoss);
}

ClassImp(CbmRichProt)
