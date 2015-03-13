#include "CbmTrdHitProducerDigi.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdHit.h"
#include "CbmTrdCluster.h"
#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TMath.h"
#include "TVector3.h"
#include "TClonesArray.h"

CbmTrdHitProducerDigi::CbmTrdHitProducerDigi()
  : FairTask("CbmTrdHitProducerDigi"),
   fTrdDigis(NULL),
   fTrdClusters(NULL),
   fTrdHits(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL)
{
}

CbmTrdHitProducerDigi::~CbmTrdHitProducerDigi()
{
   fTrdClusters->Clear("C");
   delete fTrdClusters;
   fTrdHits->Clear("C");
   delete fTrdHits;
}

void CbmTrdHitProducerDigi::SetParContainers()
{
   fDigiPar = (CbmTrdDigiPar*)(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdHitProducerDigi::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();

   fTrdDigis = (TClonesArray *) ioman->GetObject("TrdDigi");
   if (NULL == fTrdDigis) LOG(FATAL) << "CbmTrdHitProducerDigi::Init: No TrdDigi array!" << FairLogger::endl;

   fTrdClusters = new TClonesArray("CbmTrdCluster", 100);
   ioman->Register("TrdCluster", "TRD", fTrdClusters, kTRUE);

   fTrdHits = new TClonesArray("CbmTrdHit", 100);
   ioman->Register("TrdHit", "TRD", fTrdHits, kTRUE);

   return kSUCCESS;
}

void CbmTrdHitProducerDigi::Exec(
      Option_t * option)
{
   fTrdClusters->Clear();
   fTrdHits->Clear();
   Int_t nofDigis = fTrdDigis->GetEntriesFast();
   for (Int_t iDigi = 0; iDigi < nofDigis; iDigi++) {
      CbmTrdDigi* digi = (CbmTrdDigi*) fTrdDigis->At(iDigi);

      Double_t charge = digi->GetCharge();
      Int_t address = digi->GetAddress();
      Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(address);
      Int_t sectorId = CbmTrdAddress::GetSectorId(address);
      Int_t columnId = CbmTrdAddress::GetColumnId(address);
      Int_t rowId = CbmTrdAddress::GetRowId(address);

      TVector3 hitPos, padSize, hitError;
      fModuleInfo = fDigiPar->GetModule(moduleAddress);
      fModuleInfo->GetPosition(moduleAddress, sectorId, columnId, rowId, hitPos, padSize);
        
      // Calculate the hit error from the pad sizes
      hitError = padSize;
      hitError *= 1. / TMath::Sqrt(12.);

      // Take into account module orientation for errors
      fModuleInfo->TransformHitError(hitError);

      // For each digi create one hit and one cluster.
      CbmTrdCluster* cluster = new ((*fTrdClusters)[iDigi]) CbmTrdCluster();
      cluster->SetAddress(address);
      cluster->AddDigi(iDigi);

      new ((*fTrdHits)[iDigi]) CbmTrdHit(address, hitPos, hitError, 0., iDigi, 0., 0., charge);
   }

   static Int_t eventNo = 0;
   LOG(INFO) << "CbmTrdHitProducerDigi::Exec: eventNo=" << eventNo++ << ",  digis=" << fTrdDigis->GetEntriesFast()
         << ", clusters=" << fTrdClusters->GetEntriesFast() << ", hits="  << fTrdHits->GetEntriesFast() << FairLogger::endl;
}

void CbmTrdHitProducerDigi::Finish()
{
}

ClassImp(CbmTrdHitProducerDigi)
