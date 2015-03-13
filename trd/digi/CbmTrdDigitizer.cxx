#include "CbmTrdDigitizer.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"
#include "CbmTrdAddress.h"

#include "CbmMCTrack.h"
#include "CbmMatch.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TRandom.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "FairLogger.h"

#include <iostream>
#include <iomanip>
#include <cmath>
using std::cout;
using std::endl;
using std::pair;
using std::make_pair;
using std::map;
using std::vector;
using std::max;

CbmTrdDigitizer::CbmTrdDigitizer(
      CbmTrdRadiator* radiator):
    FairTask("CbmTrdDigitizer"),
    fEfficiency(1.),
    fTrdPoints(NULL),
    fMCTracks(NULL),
    fTrdDigis(NULL),
    fTrdDigiMatches(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fRadiator(radiator),
    fDigiMap()
{
}

CbmTrdDigitizer::~CbmTrdDigitizer()
{
   fTrdDigis->Clear("C");
   delete fTrdDigis;
   fTrdDigiMatches->Clear("C");
   delete fTrdDigiMatches;
}

void CbmTrdDigitizer::SetParContainers()
{
   fDigiPar = static_cast<CbmTrdDigiPar*>(FairRunAna::Instance()->GetRuntimeDb()->getContainer("CbmTrdDigiPar"));
}

InitStatus CbmTrdDigitizer::Init()
{
   FairRootManager* ioman = FairRootManager::Instance();

   fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   if (NULL == fTrdPoints) LOG(FATAL) << "CbmTrdDigitizer::Init(): No TrdPoint array!" << FairLogger::endl;

   fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");
   if (NULL == fMCTracks) LOG(FATAL) << "CbmTrdDigitizer::Init(): No MCTrack array!" << FairLogger::endl;

   fTrdDigis = new TClonesArray("CbmTrdDigi", 100);
   ioman->Register("TrdDigi", "TRD Digis", fTrdDigis, kTRUE);

   fTrdDigiMatches = new TClonesArray("CbmMatch", 100);
   ioman->Register("TrdDigiMatch", "TRD Digis", fTrdDigiMatches, kTRUE);

   fRadiator->Init();

   return kSUCCESS;
}

void CbmTrdDigitizer::Exec(Option_t * option)
{
   fTrdDigis->Clear();
   fTrdDigiMatches->Clear();

   Int_t nofTrdPoints = fTrdPoints->GetEntriesFast();
   for (Int_t iPoint = 0; iPoint < nofTrdPoints ; iPoint++ ) {
      if (gRandom->Rndm() > fEfficiency ) continue; // If random value above fEfficency reject point
      AddDigi(iPoint);
   }

   // Fill data from internally used stl map into output TClonesArray
   Int_t iDigi = 0;
   map<Int_t, pair<CbmTrdDigi*, CbmMatch*> >::iterator it;
   for (it = fDigiMap.begin() ; it != fDigiMap.end(); it++) {
      CbmTrdDigi* digi = it->second.first;
      new ((*fTrdDigis)[iDigi]) CbmTrdDigi(*digi);
      CbmMatch* digiMatch = it->second.second;
      new ((*fTrdDigiMatches)[iDigi]) CbmMatch(*digiMatch);
      delete digi;
      delete digiMatch;
      iDigi++;
   }
   fDigiMap.clear();

   static Int_t eventNo = 0;
   LOG(INFO) << "CbmTrdDigitizer::Exec: eventNo=" << eventNo++ << ",  points="
         << fTrdPoints->GetEntriesFast() << ", digis=" << fTrdDigis->GetEntriesFast() << FairLogger::endl;
}

void CbmTrdDigitizer::Finish()
{

}

void CbmTrdDigitizer::AddDigi(
      Int_t pointId)
{
   CbmTrdPoint* mcPoint = static_cast<CbmTrdPoint*>(fTrdPoints->At(pointId));
   if(NULL == mcPoint) return;

   CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcPoint->GetTrackID());
   if(NULL == mcTrack) return;

   TVector3 mom;
   mcPoint->Momentum(mom);

   Bool_t isElectron = abs(mcTrack->GetPdgCode()) == 11;
   Double_t energyLoss = (isElectron) ? mcPoint->GetEnergyLoss() + fRadiator->GetTR(mom) :  mcPoint->GetEnergyLoss();
   Double_t time = mcPoint->GetTime();

   Int_t sectorId = 0, columnId = 0, rowId = 0;
   fModuleInfo = fDigiPar->GetModule(mcPoint->GetDetectorID());
   fModuleInfo->GetPadInfo(mcPoint, sectorId, columnId, rowId);

   if (sectorId < 0 || columnId < 0 || rowId < 0) {
      LOG(ERROR) << "CbmTrdDigitizer::AddDigi: Could not find local point in any of the sectors\n";
      return;
   }

   // Form address which contains layerId, moduleId, sectorId, columnId and rowId.
   // Address in the MC point contains only information about layerId and moduleId.
   Int_t layerId = CbmTrdAddress::GetLayerId(mcPoint->GetDetectorID());
   Int_t moduleId = CbmTrdAddress::GetModuleId(mcPoint->GetDetectorID());
   Int_t address = CbmTrdAddress::GetAddress(layerId, moduleId, sectorId, rowId, columnId);

   // Add digi with address "address" for intermediate storage.
   // In case the digi for this address does not exists it is added to the map.
   // In case it exists already the information about another MC point in this
   // digi is added. Also the additional energy loss is added to the digi.

   map<Int_t, pair<CbmTrdDigi*, CbmMatch*> >::iterator it = fDigiMap.find(address);
   if (it == fDigiMap.end()) { // Pixel not yet in map -> Add new pixel
      CbmMatch* digiMatch = new CbmMatch();
      digiMatch->AddLink(CbmLink(energyLoss, pointId));
      fDigiMap[address] = make_pair(new CbmTrdDigi(address, energyLoss, time), digiMatch);
   } else { // Pixel already in map -> Add charge
      CbmTrdDigi* digi = it->second.first;
      digi->AddCharge(energyLoss);
      digi->SetTime(max(time, digi->GetTime()));
      CbmMatch* digiMatch = it->second.second;
      digiMatch->AddLink(CbmLink(energyLoss, pointId));
   }
}

ClassImp(CbmTrdDigitizer)
