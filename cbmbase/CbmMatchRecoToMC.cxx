/*
 * CbmMatchRecoToMC.cxx
 *
 *  Created on: Oct 17, 2013
 *      Author: andrey
 */

#include "CbmMatchRecoToMC.h"
#include "CbmMatch.h"
#include "CbmTrackMatchNew.h"
#include "CbmCluster.h"
#include "CbmBaseHit.h"
#include "CbmStsHit.h"
#include "CbmTrack.h"
#include "FairMCPoint.h"
#include "FairLogger.h"
#include "TClonesArray.h"

#include "CbmMCTrack.h"////
#include "CbmStsAddress.h"////

CbmMatchRecoToMC::CbmMatchRecoToMC() :
   FairTask(),
   fMCTracks(NULL),
   fStsPoints(NULL),
   fStsDigis(NULL),
   fStsClusters(NULL),
   fStsHits(NULL),
   fStsTracks(NULL),
   fStsDigiMatches(NULL),
   fStsClusterMatches(NULL),
   fStsHitMatches(NULL),
   fStsTrackMatches(NULL),
   fTrdPoints(NULL),
   fTrdDigis(NULL),
   fTrdClusters(NULL),
   fTrdHits(NULL),
   fTrdTracks(NULL),
   fTrdDigiMatches(NULL),
   fTrdClusterMatches(NULL),
   fTrdHitMatches(NULL),
   fTrdTrackMatches(NULL),
   fMuchPoints(NULL),
   fMuchPixelDigis(NULL),
   fMuchStrawDigis(NULL),
   fMuchClusters(NULL),
   fMuchPixelHits(NULL),
   fMuchStrawHits(NULL),
   fMuchTracks(NULL),
   fMuchPixelDigiMatches(NULL),
   fMuchStrawDigiMatches(NULL),
   fMuchClusterMatches(NULL),
   fMuchPixelHitMatches(NULL),
   fMuchStrawHitMatches(NULL),
   fMuchTrackMatches(NULL),
   fMvdHits(NULL),
   fMvdDigiMatches(NULL),
   fMvdHitMatches(NULL),
   fMvdCluster(NULL),
   fMvdClusterMatches(NULL)
{

}

CbmMatchRecoToMC::~CbmMatchRecoToMC()
{
   if (fStsClusterMatches != NULL) {
      fStsClusterMatches->Delete();
      delete fStsClusterMatches;
   }
   if (fStsHitMatches != NULL) {
      fStsHitMatches->Delete();
      delete fStsHitMatches;
   }
   if (fStsTrackMatches) {
      fStsTrackMatches->Delete();
      delete fStsTrackMatches;
   }

   if (fTrdClusterMatches != NULL) {
      fTrdClusterMatches->Delete();
      delete fTrdClusterMatches;
   }
   if (fTrdHitMatches != NULL) {
      fTrdHitMatches->Delete();
      delete fTrdHitMatches;
   }
   if (fTrdTrackMatches) {
      fTrdTrackMatches->Delete();
      delete fTrdTrackMatches;
   }

   if (fMuchClusterMatches != NULL) {
      fMuchClusterMatches->Delete();
      delete fMuchClusterMatches;
   }
   if (fMuchPixelHitMatches != NULL) {
      fMuchPixelHitMatches->Delete();
      delete fMuchPixelHitMatches;
   }
   if (fMuchStrawHitMatches != NULL) {
      fMuchStrawHitMatches->Delete();
      delete fMuchStrawHitMatches;
   }
   if (fMuchTrackMatches) {
      fMuchTrackMatches->Delete();
      delete fMuchTrackMatches;
   }
	
   if (fMvdClusterMatches != NULL)
      {
      fMvdClusterMatches->Delete(); 
      delete fMvdClusterMatches; 
      }
   if (fMvdHitMatches != NULL) {
      fMvdHitMatches->Delete();
      delete fMvdHitMatches;
   }


}

InitStatus CbmMatchRecoToMC::Init()
{
   ReadAndCreateDataBranches();

   return kSUCCESS;
}

void CbmMatchRecoToMC::Exec(
    Option_t* opt)
{
   if (fStsClusterMatches != NULL) fStsClusterMatches->Delete();
   if (fStsHitMatches != NULL) fStsHitMatches->Delete();
   if (fStsTrackMatches != NULL) fStsTrackMatches->Delete();
   if (fTrdClusterMatches != NULL) fTrdClusterMatches->Delete();
   if (fTrdHitMatches != NULL) fTrdHitMatches->Delete();
   if (fTrdTrackMatches != NULL) fTrdTrackMatches->Delete();
   if (fMuchClusterMatches != NULL) fMuchClusterMatches->Delete();
   if (fMuchPixelHitMatches != NULL) fMuchPixelHitMatches->Delete();
   if (fMuchStrawHitMatches != NULL) fMuchStrawHitMatches->Delete();
   if (fMuchTrackMatches != NULL) fMuchTrackMatches->Delete();
   if (fMvdHitMatches != NULL) fMvdHitMatches->Delete();
   if (fMvdClusterMatches != NULL) fMvdClusterMatches->Delete();

   // STS
   if (fStsDigis && fStsClusters && fStsHits) { // MC->digi->cluster->hit->track
      MatchClusters(fStsDigiMatches, fStsClusters, fStsClusterMatches);
      MatchHitsSts(fStsClusterMatches, fStsHits, fStsHitMatches);
      MatchTracks(fStsHitMatches, fStsPoints, fStsTracks, fStsTrackMatches);
   }

   // TRD
   if (fTrdDigis && fTrdClusters && fTrdHits) { // MC->digi->cluster->hit->track
      MatchClusters(fTrdDigiMatches, fTrdClusters, fTrdClusterMatches);
      MatchHits(fTrdClusterMatches, fTrdHits, fTrdHitMatches);
      MatchTracks(fTrdHitMatches, fTrdPoints, fTrdTracks, fTrdTrackMatches);
   } else if (fTrdHits) { // MC->hit->track
      MatchHitsToPoints(fTrdPoints, fTrdHits, fTrdHitMatches);
      MatchTracks(fTrdHitMatches, fTrdPoints, fTrdTracks, fTrdTrackMatches);
   }

   // MUCH
   MatchHits(fMuchStrawDigiMatches, fMuchStrawHits, fMuchStrawHitMatches);
   if (fMuchPixelDigis && fMuchClusters && fMuchPixelHits) {
      MatchClusters(fMuchPixelDigiMatches, fMuchClusters, fMuchClusterMatches);
      MatchHits(fMuchClusterMatches, fMuchPixelHits, fMuchPixelHitMatches);
   } else {
      MatchHitsToPoints(fMuchPoints, fMuchPixelHits, fMuchPixelHitMatches);
   }
   MatchTracks(fMuchPixelHitMatches, fMuchPoints, fMuchTracks, fMuchTrackMatches);
   MatchTracks(fMuchStrawHitMatches, fMuchPoints, fMuchTracks, fMuchTrackMatches);

   //MVD  
   if (fMvdDigiMatches && fMvdHits && fMvdHitMatches && !fMvdCluster) {// MC->digi->hit
       MatchHitsMvd(fMvdDigiMatches, fMvdHits, fMvdHitMatches);
   }
   else if(fMvdDigiMatches && fMvdCluster && fMvdClusterMatches)
	{
	MatchClusters(fMvdDigiMatches, fMvdCluster, fMvdClusterMatches);// MC->digi->cluster->hit
	MatchHits(fMvdClusterMatches, fMvdHits, fMvdHitMatches);
	}

   static Int_t eventNo = 0;
   LOG(INFO) << "CbmMatchRecoToMC::Exec eventNo=" << eventNo++ << FairLogger::endl;
}

void CbmMatchRecoToMC::Finish()
{

}

void CbmMatchRecoToMC::ReadAndCreateDataBranches()
{
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) {
      LOG(FATAL) << "CbmMatchRecoToMC::ReadAndCreateDataBranches() NULL FairRootManager." << FairLogger::endl;
   }

   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");

   // STS
   fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
   fStsDigis = (TClonesArray*) ioman->GetObject("StsDigi");
   fStsClusters = (TClonesArray*) ioman->GetObject("StsCluster");
   fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
   fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
   fStsDigiMatches = (TClonesArray*) ioman->GetObject("StsDigiMatch");
   if (fStsClusters != NULL) {
      fStsClusterMatches = new TClonesArray("CbmMatch", 100);
      ioman->Register("StsClusterMatch", "STS", fStsClusterMatches, kTRUE);
   }
   if (fStsHits != NULL) {
      fStsHitMatches = new TClonesArray("CbmMatch", 100);
      ioman->Register("StsHitMatch", "STS", fStsHitMatches, kTRUE);
   }
   if (fStsTracks != NULL) {
      fStsTrackMatches = new TClonesArray("CbmTrackMatchNew", 100);
      ioman->Register("StsTrackMatch", "STS", fStsTrackMatches, kTRUE);
   }

   // TRD
   fTrdPoints = (TClonesArray*) ioman->GetObject("TrdPoint");
   fTrdDigis = (TClonesArray*) ioman->GetObject("TrdDigi");
   fTrdClusters = (TClonesArray*) ioman->GetObject("TrdCluster");
   fTrdHits = (TClonesArray*) ioman->GetObject("TrdHit");
   fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
   fTrdDigiMatches = (TClonesArray*) ioman->GetObject("TrdDigiMatch");
   if (fTrdClusters != NULL) {
	  fTrdClusterMatches = (TClonesArray*) ioman->GetObject("TrdClusterMatch");
	  if (NULL == fTrdClusterMatches) {
	     fTrdClusterMatches = new TClonesArray("CbmMatch", 100);
		 ioman->Register("TrdClusterMatch", "TRD", fTrdClusterMatches, kTRUE);
	  }
   }
   if (fTrdHits != NULL) {
	  fTrdHitMatches = (TClonesArray*) ioman->GetObject("TrdHitMatch");
	  if (NULL == fTrdHitMatches) {
         fTrdHitMatches = new TClonesArray("CbmMatch", 100);
         ioman->Register("TrdHitMatch", "TRD", fTrdHitMatches, kTRUE);
	  }
   }
   if (fTrdTracks != NULL) {
	  fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
	  if (NULL == fTrdTrackMatches) {
	     fTrdTrackMatches = new TClonesArray("CbmTrackMatchNew", 100);
         ioman->Register("TrdTrackMatch", "TRD", fTrdTrackMatches, kTRUE);
	  }
   }

   // MUCH
   fMuchPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
   fMuchPixelDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
   fMuchStrawDigis = (TClonesArray*) ioman->GetObject("MuchStrawDigi");
   fMuchClusters = (TClonesArray*) ioman->GetObject("MuchCluster");
   fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
   fMuchStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
   fMuchTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
   fMuchPixelDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
   fMuchStrawDigiMatches = (TClonesArray*) ioman->GetObject("MuchStrawDigiMatch");
   if (fMuchClusters != NULL) {
      fMuchClusterMatches = new TClonesArray("CbmMatch", 100);
      ioman->Register("MuchClusterMatch", "MUCH", fMuchClusterMatches, kTRUE);
   }
   if (fMuchPixelHits) {
      fMuchPixelHitMatches = new TClonesArray("CbmMatch", 100);
      ioman->Register("MuchPixelHitMatch", "MUCH", fMuchPixelHitMatches, kTRUE);
   }
   if (fMuchStrawHits) {
      fMuchStrawHits = new TClonesArray("CbmMatch", 100);
      ioman->Register("MuchStrawHitMatch", "MUCH", fMuchStrawHits, kTRUE);
   }
   if (fMuchTracks) {
      fMuchTrackMatches = new TClonesArray("CbmTrackMatchNew", 100);
      ioman->Register("MuchTrackMatch", "MUCH", fMuchTrackMatches, kTRUE);
   }

   // MVD
   fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");
   fMvdCluster = (TClonesArray*) ioman->GetObject("MvdCluster");
   fMvdDigiMatches = (TClonesArray*) ioman->GetObject("MvdDigiMatch");
   if (fMvdCluster != NULL) {
      fMvdClusterMatches = new TClonesArray("CbmMatch", 100);
      ioman->Register("MvdClusterMatch", "MVD", fMvdClusterMatches, kTRUE);
   }

   if (fMvdHits != NULL) {
      fMvdHitMatches = new TClonesArray("CbmMatch", 100);
      ioman->Register("MvdHitMatch", "MVD", fMvdHitMatches, kTRUE);
   }
}


void CbmMatchRecoToMC::MatchClusters(
      const TClonesArray* digiMatches,
      const TClonesArray* clusters,
      TClonesArray* clusterMatches)
{
   if (!(digiMatches && clusters && clusterMatches)) return;
   Int_t nofClusters = clusters->GetEntriesFast();
   for (Int_t iCluster = 0; iCluster < nofClusters; iCluster++) {
      const CbmCluster* cluster = static_cast<const CbmCluster*>(clusters->At(iCluster));
      CbmMatch* clusterMatch = new ((*clusterMatches)[iCluster]) CbmMatch();
      Int_t nofDigis = cluster->GetNofDigis();
      for (Int_t iDigi = 0; iDigi < nofDigis; iDigi++) {
         const CbmMatch* digiMatch = static_cast<const CbmMatch*>(digiMatches->At(cluster->GetDigi(iDigi)));
         clusterMatch->AddLinks(*digiMatch);
      }
   }
}

void CbmMatchRecoToMC::MatchHits(
      const TClonesArray* matches,
      const TClonesArray* hits,
      TClonesArray* hitMatches)
{
   if (!(matches && hits && hitMatches)) return;
   Int_t nofHits = hits->GetEntriesFast();
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(hits->At(iHit));
      CbmMatch* hitMatch = new ((*hitMatches)[iHit]) CbmMatch();
      const CbmMatch* clusterMatch = static_cast<const CbmMatch*>(matches->At(hit->GetRefId()));
      hitMatch->AddLinks(*clusterMatch);
    //  std::cout << "hit " << iHit << " " << hitMatch->ToString();
   }
}

void CbmMatchRecoToMC::MatchHitsSts(
      const TClonesArray* matches,
      const TClonesArray* hits,
      TClonesArray* hitMatches)
{
   if (!(matches && hits && hitMatches)) return;
   Int_t nofHits = hits->GetEntriesFast();
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      const CbmStsHit* hit = static_cast<const CbmStsHit*>(hits->At(iHit));
      CbmMatch* hitMatch = new ((*hitMatches)[iHit]) CbmMatch();
      const CbmMatch* frontClusterMatch = static_cast<const CbmMatch*>(matches->At(hit->GetFrontClusterId()));
      const CbmMatch* backClusterMatch = static_cast<const CbmMatch*>(matches->At(hit->GetBackClusterId()));
	  hitMatch->AddLinks(*frontClusterMatch);
      hitMatch->AddLinks(*backClusterMatch);
	  
    //  std::cout << "hit " << iHit << " " << hitMatch->ToString();
   }
}

void CbmMatchRecoToMC::MatchHitsMvd(
      const TClonesArray* matches,
      const TClonesArray* hits,
      TClonesArray* hitMatches)
{
  if (!(matches && hits && hitMatches)) return;
  Int_t nofHits = hits->GetEntriesFast();
  for (Int_t iHit = 0; iHit < nofHits; iHit++) {
	const CbmPixelHit* hit = static_cast<const CbmPixelHit*>(hits->At(iHit));
 	CbmMatch* hitMatch = new ((*hitMatches)[iHit]) CbmMatch();
	const CbmMatch* digiMatch = static_cast<const CbmMatch*>(matches->At(hit->GetRefId()));
	hitMatch->AddLinks(*digiMatch);
  }
}

void CbmMatchRecoToMC::MatchHitsToPoints(
      const TClonesArray* points,
      const TClonesArray* hits,
      TClonesArray* hitMatches)
{
   if (!(hits && hitMatches)) return;
   Int_t nofHits = hits->GetEntriesFast();
   for (Int_t iHit = 0; iHit < nofHits; iHit++) {
      const CbmBaseHit* hit = static_cast<const CbmBaseHit*>(hits->At(iHit));
      CbmMatch* hitMatch = new ((*hitMatches)[iHit]) CbmMatch();
      const FairMCPoint* point = static_cast<const FairMCPoint*>(points->At(hit->GetRefId()));
      hitMatch->AddLink(CbmLink(point->GetEnergyLoss(), hit->GetRefId()));
    //  std::cout << "hit " << iHit << " " << hitMatch->ToString();
   }
}

void CbmMatchRecoToMC::MatchTracks(
      const TClonesArray* hitMatches,
      const TClonesArray* points,
      const TClonesArray* tracks,
      TClonesArray* trackMatches)
{
   if (!(hitMatches && points && tracks && trackMatches)) return;

   Bool_t editMode = (trackMatches->GetEntriesFast() != 0);

   Int_t nofTracks = tracks->GetEntriesFast();
   for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {
      const CbmTrack* track = static_cast<const CbmTrack*>(tracks->At(iTrack));
      CbmTrackMatchNew* trackMatch = (editMode) ?
            static_cast<CbmTrackMatchNew*>(trackMatches->At(iTrack)) :
               new ((*trackMatches)[iTrack]) CbmTrackMatchNew();
      Int_t nofHits = track->GetNofHits();
      for (Int_t iHit = 0; iHit < nofHits; iHit++) {
         const CbmMatch* hitMatch = static_cast<CbmMatch*>(hitMatches->At(track->GetHitIndex(iHit)));
         Int_t nofLinks = hitMatch->GetNofLinks();
         for (Int_t iLink = 0; iLink < nofLinks; iLink++) {
            const FairMCPoint* point = static_cast<const FairMCPoint*>(points->At(hitMatch->GetLink(iLink).GetIndex()));
            if (NULL == point) continue;
			////fix low energy cut case on STS
			if (CbmStsAddress::GetSystemId(point->GetDetectorID()) == kSTS ){
				Int_t mcTrackId = point->GetTrackID();
				CbmMCTrack *mcTrack = (CbmMCTrack*) fMCTracks->At(mcTrackId);
				if(mcTrack->GetNPoints(kSTS) < 2)
					continue;
			}
			////
            trackMatch->AddLink(CbmLink(1., point->GetTrackID()));
         }
      }
      if ( ! trackMatch->GetNofLinks() ) continue;
      // Calculate number of true and wrong hits
      Int_t trueCounter = trackMatch->GetNofTrueHits();
      Int_t wrongCounter = trackMatch->GetNofWrongHits();
      for (Int_t iHit = 0; iHit < nofHits; iHit++) {
         const CbmMatch* hitMatch = static_cast<CbmMatch*>(hitMatches->At(track->GetHitIndex(iHit)));
         Int_t nofLinks = hitMatch->GetNofLinks();
         Bool_t hasTrue = false;
         for (Int_t iLink = 0; iLink < nofLinks; iLink++) {
            const FairMCPoint* point = static_cast<const FairMCPoint*>(points->At(hitMatch->GetLink(iLink).GetIndex()));
            if (NULL == point) continue;
            if (point->GetTrackID() == trackMatch->GetMatchedLink().GetIndex()) {
               hasTrue = true;
               break;
            }
         }
         if (hasTrue) trueCounter++; else wrongCounter++;
      }
      trackMatch->SetNofTrueHits(trueCounter);
      trackMatch->SetNofWrongHits(wrongCounter);
     // std::cout << iTrack << " "; track->Print(); std::cout << " " << trackMatch->ToString();
   }
}

ClassImp(CbmMatchRecoToMC);
