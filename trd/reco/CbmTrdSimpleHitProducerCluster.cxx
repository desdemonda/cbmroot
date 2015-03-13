#include "CbmTrdSimpleHitProducerCluster.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdRadiator.h"
#include "CbmTrdPoint.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"
#include "CbmTrdDigiMatch.h"
#include "CbmTrdGeoHandler.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::setprecision;

// ---- Default constructor -------------------------------------------
CbmTrdSimpleHitProducerCluster::CbmTrdSimpleHitProducerCluster()
  : FairTask("TrdHitProducer",1),
    fTrdCluster(NULL),
    fTrdDigi(NULL),
    fHitCollection(NULL),
    fMCStack(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fGeoHandler(new CbmTrdGeoHandler())
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdSimpleHitProducerCluster::CbmTrdSimpleHitProducerCluster(const char *name, const char *title, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fTrdCluster(NULL),
    fTrdDigi(NULL),
    fHitCollection(NULL),
    fMCStack(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fGeoHandler(new CbmTrdGeoHandler())
{

}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdSimpleHitProducerCluster::~CbmTrdSimpleHitProducerCluster()
{
    fHitCollection->Clear("C");
    delete fHitCollection;

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdSimpleHitProducerCluster::SetParContainers()
{
    cout<<" * HitProducer * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdSimpleHitProducerCluster::ReInit(){

    cout<<" * HitProducer * :: ReInit() "<<endl;


    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

    return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdSimpleHitProducerCluster::Init()
{

    cout<<" * HitProducer * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();

    fTrdDigi =(TClonesArray *)  ioman->GetObject("TrdDigi");
    if ( ! fTrdDigi ) {
      cout << "-W CbmTrdSimpleHitProducerCluster::Init: No TrdDigi array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }


    fTrdCluster =(TClonesArray *)  ioman->GetObject("TrdCluster");
    if ( ! fTrdDigi ) {
      cout << "-W CbmTrdSimpleHitProducerCluster::Init: No TrdCluster array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fHitCollection = new TClonesArray("CbmTrdHit", 100);
    ioman->Register("TrdHit","TRD",fHitCollection,kTRUE);

    fGeoHandler->Init();

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdSimpleHitProducerCluster::Exec(Option_t * option)
{

  fHitCollection->Clear();
  CbmTrdDigi *digi=NULL;
  CbmTrdCluster *cluster=NULL;
  
  Int_t Col, Row, DetId, moduleId, Plane;
  Int_t Station, Layer, ModuleType, ModuleCopy, Sector;
  Double_t xHit, yHit, zHit;
  Double_t xHitErr, yHitErr, zHitErr;
  Double_t ELoss;
  TVector3 posHit;
  TVector3 padSize;
    
  Int_t nentries = fTrdCluster->GetEntries();
  cout<<" ** "<<nentries<<" Trd hits to be created in this event** "<<endl;
  
  for (int j=0; j < nentries; j++ ) {
    
    cluster =  (CbmTrdCluster*) fTrdCluster->At(j);

    Int_t nofDigis=cluster->GetNofDigis();

    if(nofDigis != 1 ) {
      cout<<"--E-- CbmTrdSimpleHitProducerCluster can only work with one digi per cluster. For this cluster ther have been "<<nofDigis<<" clusters"<<endl;

    }

    for (int k=0; k < nofDigis; k++ ) {
      Int_t digiIndex=cluster->GetDigi(k);
      digi =  (CbmTrdDigi*) fTrdDigi->At(digiIndex);
    
      Col = digi->GetCol();
      Row = digi->GetRow();
      ELoss = digi-> GetCharge();
      DetId = digi->GetDetId();

      // The digi contains the information of the detector Id + the
      // sector number. The digitization parameters are stored per
      // module with arrays holding the information about the sectors.
      // So we have to extract the information about the module Id and
      // the sector from the detector Id.
      Station = fGeoHandler->GetStation(DetId);
      Layer = fGeoHandler->GetLayer(DetId);
      ModuleType = fGeoHandler->GetModuleType(DetId);
      ModuleCopy = fGeoHandler->GetModuleCopyNr(DetId);
      Sector = fGeoHandler->GetSector(DetId);
      moduleId = fGeoHandler->GetModuleId(DetId);

      Plane = fGeoHandler->GetPlane(DetId);

      fModuleInfo = fDigiPar->GetModule(moduleId);
      fModuleInfo->GetPosition(Col, Row, moduleId, Sector, posHit, padSize);
        
      // Calculate the hit error from the pad sizes
      padSize*=(1/TMath::Sqrt(12.));
      
      AddHit(posHit, padSize, DetId, Plane, j , ELoss, 0., 0.);
    }
  }
  
}
// --------------------------------------------------------------------

// ---- Add Hit to HitCollection --------------------------------------

void CbmTrdSimpleHitProducerCluster::AddHit(TVector3 &posHit, TVector3 &posHitErr,
			       Int_t detId, Int_t Plane, Int_t ref,
			       Double_t ELoss, Double_t ELossTR,
			       Double_t ELossdEdX){

    TClonesArray& clref = *fHitCollection;
    Int_t size = clref.GetEntriesFast();

    
//    new((*fHitCollection)[size]) CbmTrdHit(fDetId, posHit, posHitErr, ref,
//					   Plane, 0., 0., ELoss);

    new((*fHitCollection)[size]) CbmTrdHit(detId, posHit, posHitErr, 0., 
                            ref, Plane, 0., 0., ELoss);
}

// --------------------------------------------------------------------

// ---- Finish --------------------------------------------------------
void CbmTrdSimpleHitProducerCluster::Finish()
{
}
// --------------------------------------------------------------------

// ---- Register ------------------------------------------------------
void CbmTrdSimpleHitProducerCluster::Register(){

  FairRootManager::Instance()->Register("TrdHit","Trd", fHitCollection, kTRUE);

}

ClassImp(CbmTrdSimpleHitProducerCluster)
