/*
 * \file CbmLitFindLWClusters.cxx
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date January 2015
 * \brief Class to create clusters from digis using the Agglomerative Hierachical Algorithm
 */

#include "CbmLitFindLWClusters.h"

// Standard Library
#include<iostream>

// Root Classes
#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TMath.h"
#include "TVector3.h"

// Fair Classes
#include "FairRootManager.h"
#include "FairLogger.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Trd Classes
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdModule.h"
#include "CbmTrdDigiPar.h"
#include "CbmTrdGeoHandler.h"
#include "CbmTrdAddress.h"
#include "CbmCluster.h"
#include "CbmLitLWClusterManager.h"
// #include "CbmLitClusterDistance.h"

using namespace std;

CbmLitFindLWClusters::CbmLitFindLWClusters():
	 FairTask("CbmLitFindLWClusters",1),
	 fDigis(NULL),
	 fClusters(NULL),
	 fUnmergedClusters(NULL),
	 fHits(NULL),
	 fModuleInfo(NULL),
	 fDigiPar(NULL),
	 fModuleDetails(NULL),
	 fClusterManager(NULL),
	 fGeoHandler(NULL)
{
}

CbmLitFindLWClusters::~CbmLitFindLWClusters()
{
}

InitStatus CbmLitFindLWClusters::Init()
{

   cout << "CbmLitFindLWClusters :: Init() " << endl;

   FairRootManager* ioman = FairRootManager::Instance();

   // Fetch Digis from FairRootManager
   fDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   if (fDigis == NULL) LOG(FATAL) << "CbmLitFindLWClusters::Init No TrdDigi array" << FairLogger::endl;

   // Initialize TClonesArray for Clusters
   fClusters = new TClonesArray("CbmTrdCluster", 1000);
   ioman->Register("TrdCluster","Trd Cluster",fClusters,kTRUE);

   // Initialize TClonesArray for Hits
   fHits = new TClonesArray("CbmTrdHit", 1000);
   ioman->Register("TrdHit","Trd Hit",fHits,kTRUE);

   // Get Base Container
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb = ana->GetRuntimeDb();

   fClusterManager = new CbmLitLWClusterManager();

   fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

   return kSUCCESS;
}

void CbmLitFindLWClusters::Exec(Option_t *option)
{
  typedef std::map<Int_t,std::vector<CbmTrdDigi*>> ModuleMap_t;
  typedef vector<vector<CbmTrdDigi*>> DigiVector_t;
   fClusters->Delete();
   fHits->Delete();
   Int_t uniqueModID = 0;
   TStopwatch timer;
   timer.Start();
   cout << "================CbmLitFindLWClusters::Exec===============" << endl;
   ModuleMap_t moduleMap;
   DigiVector_t digiArray;
   map<Int_t, Int_t> digiIdMap;

   for (Int_t iDigi=0; iDigi < fDigis->GetEntries(); iDigi++ ){
      CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
      Int_t digiAddress = digi->GetAddress();
      Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
      Int_t digiColumn = CbmTrdAddress::GetColumnId(digiAddress);
      Int_t digiRow = CbmTrdAddress::GetRowId(digiAddress);

      moduleMap[moduleAddress].push_back(digi);
      digiIdMap.insert(pair<Int_t,Int_t>(digiAddress, iDigi));
   }

   Int_t index=0;
   Double_t chkEnergy = pow(10,-6);
   for (const auto& module : moduleMap){
     Int_t moduleAddress = module.first;
     const vector<CbmTrdDigi*>& digiVector = module.second;
     for (const auto& v : digiVector){
	Double_t ELoss = v->GetCharge();
	if(ELoss < chkEnergy) continue;

	Int_t digiAddress = v->GetAddress();
	Int_t digiId = digiIdMap[digiAddress];
	Int_t digiColumn = CbmTrdAddress::GetColumnId(digiAddress);
	Int_t digiRow = CbmTrdAddress::GetRowId(digiAddress);
	CbmTrdCluster* cluster = new ((*fClusters)[index]) CbmTrdCluster();
	cluster->SetAddress(digiAddress);
	cluster->AddDigi(digiId);

	digiArray[digiColumn][digiRow] = 1;

	Int_t Col, Row, Plane;
	Int_t Layer, moduleAddress, Sector;
	Double_t xHit, yHit, zHit;
	Double_t xHitErr, yHitErr, zHitErr;
	TVector3 posHit;
	TVector3 padSize;
	// Retrieving Hit Informations
	Col = CbmTrdAddress::GetColumnId(digiAddress);
	Row = CbmTrdAddress::GetRowId(digiAddress);
	Layer = CbmTrdAddress::GetLayerId(digiAddress);
	Sector = CbmTrdAddress::GetSectorId(digiAddress);
	moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
	fModuleInfo = fDigiPar->GetModule(moduleAddress);
	fModuleInfo->GetPosition(moduleAddress, Sector, Col, Row, posHit, padSize);
	// Calculate the hit error from the pad sizes
	padSize*=(1/TMath::Sqrt(12.));

	CbmTrdHit *hit = new((*fHits)[index]) CbmTrdHit(digiAddress, posHit, padSize, 0., index, 0., 0., ELoss);
	++index;
     }
   }
   timer.Stop();
   LOG(INFO) << "CbmLitFindLWClusters::Exec : real time=" << timer.RealTime()
             << " CPU time=" << timer.CpuTime() << FairLogger::endl;
}

void CbmLitFindLWClusters::Finish()
{
}
/*
void CbmLitFindLWClusters::create_dm() const
{
//   for (Int_t i=0; i < fClusters->GetEntries(); i++ ){
//      for (Int_t j=i+1; j < fClusters->GetEntries(); j++ ){
//	 fDistanceMap.insert(i, j, CbmLitClusterDistance(fClusters[i], fClusters[j]));  // calculate distance between clusters
//      }
//   }
}

void CbmLitFindLWClusters::update_dm(Int_t p, Int_t q, Int_t r) const
{
//   fDistanceMap[p,q] = CbmLitClusterDistance(fClusters[p], fClusters[q]);  // update distance between clusters
}*/


ClassImp(CbmLitFindLWClusters)
