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
#include "CbmTrdAddress.h"
#include "CbmCluster.h"
// #include "CbmLitLWClusterManager.h"
// #include "CbmLitClusterDistance.h"

using namespace std;

CbmLitFindLWClusters::CbmLitFindLWClusters():
	 FairTask("CbmLitFindLWClusters",1),
	 fDigis(NULL),
	 fClusters(NULL),
	 fUnmergedClusters(NULL),
	 fHits(NULL),
	 fModuleInfo(NULL),
	 fModules(NULL),
	 fDigiPar(NULL),
	 fModuleDetails(NULL),
	 fSeenDigis()
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

   fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

   return kSUCCESS;
}

void CbmLitFindLWClusters::Exec(Option_t *option)
{
  typedef std::map<Int_t,std::vector<CbmTrdDigi*>> ModuleMap_t;
  typedef std::vector<std::vector<CbmTrdDigi*>> DigiVector_t;
   fClusters->Delete();
   fHits->Delete();
   fSeenDigis.clear();

   Int_t uniqueModID = 0;
   TStopwatch timer;
   timer.Start();
   cout << "================CbmLitFindLWClusters::Exec===============" << endl;
   ModuleMap_t moduleMap;
   map<Int_t, Int_t> digiIdMap;

   for (Int_t iDigi=0; iDigi < fDigis->GetEntries(); iDigi++ ){
      CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
      Int_t digiAddress = digi->GetAddress();
      Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);

      moduleMap[moduleAddress].push_back(digi);
      digiIdMap.insert(pair<Int_t,Int_t>(digiAddress, iDigi));
   }

   DigiVector_t digiArray;
   Int_t index=0;
   Int_t nofDigisChecked = 0;
   cout << "Points in total: " << fDigis->GetEntries() << endl;
   Double_t chkEnergy = 1e-6;
   for (const auto& module : moduleMap){
     Int_t moduleAddress = module.first;
     const vector<CbmTrdDigi*>& digiVector = module.second;
     fModuleInfo = fDigiPar->GetModule(moduleAddress);
     Int_t nofCols = fModuleInfo->GetNofColumns();
     Int_t nofRows = fModuleInfo->GetNofRows();
     digiArray.resize(nofCols);
     for(Int_t i=0; i < nofCols; i++)
       digiArray[i].resize(nofRows, nullptr);

     for (const auto& v : digiVector){
	Double_t ELoss = v->GetCharge();
	if(ELoss < chkEnergy) continue;
	nofDigisChecked++;

	Int_t digiAddress = v->GetAddress();
	Int_t digiId = digiIdMap[digiAddress];
	Int_t digiColumn = CbmTrdAddress::GetColumnId(digiAddress);
	Int_t digiRow = CbmTrdAddress::GetRowId(digiAddress);

	digiArray[digiColumn][digiRow] = v;
     }
     cout << "Points above threshold: " << nofDigisChecked << endl;
     LOG(INFO) << "Module " << moduleAddress << ": searching for Pairs" << FairLogger::endl;
     for (Int_t col=0; col<nofCols; ++col ){
	 for (Int_t row=0; row<nofRows; ++row ){
	     CbmTrdDigi* d = digiArray[col][row];
	     if(d == nullptr || fSeenDigis.find(d->GetAddress()) != fSeenDigis.end()) continue;
	     vector<CbmTrdDigi*> result;
	     FindRecursive(digiArray, col, row, result, digiIdMap);
	 }
     }
/*     if( moduleAddress == 11269 or moduleAddress == 11077 or moduleAddress == 11013)
       LOG(INFO) << "Module " << moduleAddress << ": ; rows: " << nofRows << "; cols: " << nofCols << FairLogger::endl;*/
   }

   HitFinder();

   timer.Stop();
   LOG(INFO) << "CbmLitFindLWClusters::Exec : real time=" << timer.RealTime()
             << " CPU time=" << timer.CpuTime() << FairLogger::endl;
}

void CbmLitFindLWClusters::FindRecursive(const DigiVector_t &digiArray,
					 Int_t col,
					 Int_t row,
					 vector<CbmTrdDigi*> &result,
					 const map<Int_t, Int_t> &digiIdMap)
{
  CbmTrdDigi* d = digiArray[col][row];
  Int_t digiAddress = d->GetAddress();
  Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);
  Int_t nofCols = digiArray.size();
  Int_t nofRows = digiArray[0].size();

  CbmTrdDigi* d2 = nullptr;
  if( col != nofCols-1 && digiArray[col+1][row] != nullptr ){
	 d2 = digiArray[col+1][row];
  }
  if( (row != nofRows-1 && col != nofCols-1) && digiArray[col+1][row+1] != nullptr ){
	 d2 = digiArray[col+1][row+1];
  }
  if( (row != 0 && col != nofCols-1) && digiArray[col+1][row-1] != nullptr ){
	 d2 = digiArray[col+1][row-1];
  }
  if( row != nofRows-1 && digiArray[col][row+1] != nullptr ){
	 d2 = digiArray[col][row+1];
  }

  if( d2 != nullptr && CheckLink(result, d, d2) ){
      result.push_back(d);

      Int_t col2 = CbmTrdAddress::GetColumnId(d2->GetAddress());
      Int_t row2 = CbmTrdAddress::GetRowId(d2->GetAddress());
      FindRecursive(digiArray, col2, row2, result, digiIdMap);
  }else{
      result.push_back(d);
      LOG(INFO) << "Creating Cluster with " << result.size() << " Digis ..." << FairLogger::endl;
      // Adding a new Cluster into fClusters
      Int_t clusterNo = fClusters->GetEntries();
      CbmTrdCluster* cluster = new ((*fClusters)[clusterNo]) CbmTrdCluster();
      cluster->SetAddress(digiAddress);

      vector<CbmTrdDigi*>::iterator lnk = result.begin();
      while( lnk != result.end() ){
	  CbmTrdDigi* digi = (CbmTrdDigi*) *lnk;
	  digiAddress = digi->GetAddress();
	  Int_t iDigi = digiIdMap.at(digiAddress);
	  cluster->AddDigi(iDigi);
	  fSeenDigis.insert(digiAddress);
	  ++lnk;
      }
      ++clusterNo;
  }
}

bool CbmLitFindLWClusters::CheckLink(vector<CbmTrdDigi*> &list, CbmTrdDigi* digi, CbmTrdDigi* oldDigi)
{
//  if( digi->GetCharge() < oldDigi->GetCharge() )
//    return false;
  return true;
}

void CbmLitFindLWClusters::HitFinder()
{
  Int_t hitNo = 0;
  for (Int_t iCluster=0; iCluster < fClusters->GetEntries(); iCluster++ ){
    CbmTrdCluster* cluster = (CbmTrdCluster*) fClusters->At(iCluster);
    Int_t nofDigis = cluster->GetNofDigis();
    TVector3 posHit, testPosHit, padSize;
    testPosHit.SetXYZ(0.0, 0.0, 0.0);
    padSize.SetXYZ(0.0, 0.0, 0.0);
    Double_t ELoss, maxELoss;
    maxELoss = 0;
    Int_t digiAddress2;
    LOG(INFO) << "Retrieving Hit Informations of " << nofDigis << " Digis ..." << FairLogger::endl;
//    LOG(INFO) << "Processing Hit digiAddress (posHit.x, posHit.y, posHit.z, maxELoss, padSize)" << FairLogger::endl;
    printf("digiAddress, dCol, dRow, posHit2.x(), posHit2.y(), posHit2.z(), ELoss2, padSize.x(), padSize.y(), padSize.z()\n");
    for(Int_t iDigi=0; iDigi < nofDigis; ++iDigi){
      CbmTrdDigi* digi = (CbmTrdDigi*) fDigis->At(cluster->GetDigi(iDigi));
      Int_t digiAddress = digi->GetAddress();

      // Retrieving Hit Informations
      Int_t dCol = CbmTrdAddress::GetColumnId(digiAddress);
      Int_t dRow = CbmTrdAddress::GetRowId(digiAddress);
      Double_t ELoss2 = digi->GetCharge();

      Int_t Layer = CbmTrdAddress::GetLayerId(digiAddress);
      Int_t Sector = CbmTrdAddress::GetSectorId(digiAddress);
      Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);

      TVector3 posHit2;
      TVector3 padSize2;
      fModuleInfo = fDigiPar->GetModule(moduleAddress);
      fModuleInfo->GetPosition(moduleAddress, Sector, dCol, dRow, posHit2, padSize2);

      padSize += padSize2;
      testPosHit += posHit2;
      ELoss += ELoss2;
      if(max(abs(ELoss2),abs(maxELoss)) == ELoss2){
	  maxELoss = ELoss2;
	  posHit = posHit2;
	  digiAddress2 = digiAddress;
      }
      printf("%d, %d, %d, %e, %e, %e, %e, %e, %e, %e\n", digiAddress, dCol, dRow, posHit2.x(), posHit2.y(), posHit2.z(), ELoss2, padSize.x(), padSize.y(), padSize.z());
    }

    // Calculate the hit error from the pad sizes
    testPosHit *= 1/nofDigis;
    ELoss   /= nofDigis;
    padSize *= 1/nofDigis;
    padSize *= 1/TMath::Sqrt(12.);

    // Adding a new hit into fHits
    LOG(INFO) << "Processing Hit digiAddress (posHit.x, posHit.y, posHit.z, ELoss, padSize.x, padSize.y, padSize.z)" << FairLogger::endl;
    printf("Processing Hit %d (%e, %e, %e, %e, %e, %e, %e)\n", digiAddress2, posHit.x(), posHit.y(), posHit.z(), maxELoss, padSize.x(), padSize.y(), padSize.z());
    printf("Hit Info Test: %d (%e, %e, %e, %e, %e, %e, %e)\n", digiAddress2, testPosHit.x(), testPosHit.y(), testPosHit.z(), ELoss, padSize.x(), padSize.y(), padSize.z());
    LOG(INFO) << "Creating Hit with " << nofDigis << " Digis ..." << FairLogger::endl;
    CbmTrdHit *hit = new((*fHits)[hitNo++]) CbmTrdHit(digiAddress2, posHit, padSize, 0., iCluster, 0., 0., maxELoss);
  }
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
