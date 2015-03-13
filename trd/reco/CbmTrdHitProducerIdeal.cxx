/**
 * \file CbmTrdHitProducerIdeal.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 06.05.2013
 */

#include "CbmTrdHitProducerIdeal.h"
#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmTrdAddress.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include "FairRootManager.h"
#include "FairLogger.h"

CbmTrdHitProducerIdeal::CbmTrdHitProducerIdeal()
 : FairTask("CbmTrdHitProducerIdeal"),
   fTrdPoints(NULL),
   fTrdHits(NULL)
{
}

CbmTrdHitProducerIdeal::~CbmTrdHitProducerIdeal()
{
    fTrdHits->Clear();
    delete fTrdHits;
}

InitStatus CbmTrdHitProducerIdeal::Init()
{
   FairRootManager* rootMgr = FairRootManager::Instance();
   if (NULL == rootMgr) {
      LOG(ERROR) << "CbmTrdHitProducerIdeal::Init: " << "ROOT manager is not instantiated!" << FairLogger::endl;
      return kFATAL;
   }
  
   fTrdPoints = (TClonesArray*) rootMgr->GetObject("TrdPoint");
   if (NULL == fTrdPoints) {
      LOG(ERROR) << "CbmTrdHitProducerIdeal::Init: " << "no TrdPoint array!" << FairLogger::endl;
      return kFATAL;
   }
  
   fTrdHits = new TClonesArray("CbmTrdHit", 100);
   rootMgr->Register("TrdHit", "TRD", fTrdHits, kTRUE);
  
   return kSUCCESS;
}

void CbmTrdHitProducerIdeal::Exec(
      Option_t *option)
{
   fTrdHits->Clear();

   TVector3 dpos(0., 0., 0.);
   Int_t nofPoints = fTrdPoints->GetEntriesFast();
   for (Int_t iPoint = 0; iPoint < nofPoints; iPoint++) {
      CbmTrdPoint* point = static_cast<CbmTrdPoint*>(fTrdPoints->At(iPoint));

      TVector3 pos;
      point->Position(pos);
      Int_t address = point->GetDetectorID();
      Double_t eloss = point->GetEnergyLoss();

      new ((*fTrdHits)[iPoint]) CbmTrdHit(address, pos, dpos, 0., iPoint, 0., eloss, eloss);
   }

   static Int_t eventNo = 0;
   LOG(INFO) << "CbmTrdHitProducerIdeal::Exec: eventNo=" << eventNo++ << ",  points="
         << fTrdPoints->GetEntriesFast() << ", hits=" << fTrdHits->GetEntriesFast() << FairLogger::endl;
}

void CbmTrdHitProducerIdeal::Finish()
{

}
ClassImp(CbmTrdHitProducerIdeal);
