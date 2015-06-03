/*
 * \file CbmLitFindSimpleTrdClusters.cxx
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date Dec, 2014
 * \brief Class to interpret each digi as one cluster and one hit (for reference)
 */

#include "CbmLitFindSimpleTrdClusters.h"

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

using std::cout;
using std::endl;

CbmLitFindSimpleTrdClusters::CbmLitFindSimpleTrdClusters():
	 FairTask("CbmLitFindSimpleTrdClusters",1),
	 fDigis(NULL),
	 fClusters(NULL),
	 fHits(NULL),
	 fModuleInfo(NULL),
	 fDigiPar(NULL)
{
}

CbmLitFindSimpleTrdClusters::~CbmLitFindSimpleTrdClusters()
{
}

InitStatus CbmLitFindSimpleTrdClusters::Init()
{

   cout << "CbmLitFindSimpleTrdClusters :: Init() " << endl;

   FairRootManager* ioman = FairRootManager::Instance();

   // Fetch Digis from FairRootManager
   fDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   if (fDigis == NULL) LOG(FATAL) << "CbmLitFindSimpleTrdClusters::Init No TrdDigi array" << FairLogger::endl;

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

void CbmLitFindSimpleTrdClusters::Exec(Option_t *option)
{
   fClusters->Delete();
   fHits->Delete();

   TStopwatch timer;
   timer.Start();
   cout << "================CbmLitFindSimpleTrdClusters::Exec===============" << endl;

   Int_t Col, Row, Plane;
   Int_t Layer, moduleAddress, Sector;
   Double_t xHit, yHit, zHit;
   Double_t xHitErr, yHitErr, zHitErr;
   Double_t ELoss;
   TVector3 posHit;
   TVector3 padSize;

   for (Int_t iDigi=0; iDigi < fDigis->GetEntries(); iDigi++ ){
      CbmTrdDigi *digi = (CbmTrdDigi*) fDigis->At(iDigi);
      Int_t digiAddress = digi->GetAddress();

      // Adding a new Cluster into fClusters
      CbmTrdCluster* cluster = new ((*fClusters)[iDigi]) CbmTrdCluster();
      cluster->SetAddress(digiAddress);
      cluster->AddDigi(iDigi);

      // Retrieving Hit Informations
      Col = CbmTrdAddress::GetColumnId(digiAddress);
      Row = CbmTrdAddress::GetRowId(digiAddress);
      ELoss = digi-> GetCharge();

      Layer = CbmTrdAddress::GetLayerId(digiAddress);
      Sector = CbmTrdAddress::GetSectorId(digiAddress);
      moduleAddress = CbmTrdAddress::GetModuleAddress(digiAddress);

      fModuleInfo = fDigiPar->GetModule(moduleAddress);

      fModuleInfo->GetPosition(moduleAddress, Sector, Col, Row, posHit, padSize);

      // Calculate the hit error from the pad sizes
      padSize*=(1/TMath::Sqrt(12.));

      // Adding a new hit into fHits
      CbmTrdHit *hit = new((*fHits)[iDigi]) CbmTrdHit(digiAddress, posHit, padSize, 0., iDigi, 0., 0., ELoss);
   }

   timer.Stop();
   LOG(INFO) << "CbmLitFindSimpleTrdClusters::Exec : real time=" << timer.RealTime()
             << " CPU time=" << timer.CpuTime() << FairLogger::endl;
}

void CbmLitFindSimpleTrdClusters::Finish()
{
}

ClassImp(CbmLitFindSimpleTrdClusters)
