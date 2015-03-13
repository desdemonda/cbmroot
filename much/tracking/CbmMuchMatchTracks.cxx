#include "CbmMuchMatchTracks.h"

#include "CbmMuchStrawHit.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchTrack.h"
#include "CbmTrackMatch.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchCluster.h"
#include "CbmMCEpoch.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <iomanip>
#include <map>

CbmMuchMatchTracks::CbmMuchMatchTracks()
  : FairTask("CbmMuchMatchTracks"),
    fTracks(NULL),
    fPoints(NULL),
    fPixelHits(NULL),
    fStrawHits(NULL),
    fMatches(NULL),
    fPixelDigiMatches(NULL),
    fStrawDigiMatches(NULL),
    fClusters(NULL),
    fMcEpoch(NULL),
    fNofHits(0),
    fNofTrueHits(0),
    fNofWrongHits(0),
    fNofFakeHits(0),
    fNEvents(0),
    fIsEpoch(0)
{
}

CbmMuchMatchTracks::~CbmMuchMatchTracks()
{
}

InitStatus CbmMuchMatchTracks::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (ioman == NULL) Fatal("CbmMuchMatchTracks::Init", "RootManager not instantised!");

	fPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
	fStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
	if (fPixelHits == NULL && fStrawHits == NULL) Fatal("CbmMuchMatchTracks::Init", "No MuchPixelHit && MuchStrawHit array!");

	fTracks = (TClonesArray*) ioman->GetObject("MuchTrack");
	if (fTracks == NULL) Fatal("CbmMuchMatchTracks::Init", "No MuchTrack array!");

	if (!fIsEpoch){
	  fPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
	  if (fPoints == NULL) Fatal("CbmMuchMatchTracks::Init", "No MuchPoint array!");
	} else fMcEpoch = (CbmMCEpoch*) ioman->GetObject("MCEpoch.");
	
	
	fPixelDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
	fStrawDigiMatches = (TClonesArray*) ioman->GetObject("MuchStrawDigiMatch");
	if (fPixelDigiMatches == NULL && fStrawDigiMatches == NULL) Fatal("CbmMuchMatchTracks::Init", "No MuchDigiMatch && MuchStrawDigiMatch array!");

	fClusters = (TClonesArray*) ioman->GetObject("MuchCluster");
	if (fClusters == NULL) Info("CbmMuchMatchTracks::Init", "No cluster array -- simple hit to digi matching will be used");

	fMatches = new TClonesArray("CbmTrackMatch",100);
	ioman->Register("MuchTrackMatch", "MUCH", fMatches, kTRUE);

	return kSUCCESS;
}

void CbmMuchMatchTracks::Exec(
		Option_t* opt)
{
        if (fIsEpoch) fPoints = fMcEpoch->GetPoints(kMUCH);

        fMatches->Clear();

	Int_t nofTracks = fTracks->GetEntriesFast();
	for (Int_t iTrack = 0; iTrack < nofTracks; iTrack++) {	// Loop over tracks
	   // std::map stores MC track id to number of contributions of this MC track
	   std::map<Int_t, Int_t> matchMap;

		CbmMuchTrack* pTrack = static_cast<CbmMuchTrack*>(fTracks->At(iTrack));
		if (pTrack == NULL) continue;

		Int_t nofHits = pTrack->GetNofHits();
		for (Int_t iHit = 0; iHit < nofHits; iHit++) { // Loop over hits
			HitType hitType = pTrack->GetHitType(iHit);
			if (hitType == kMUCHPIXELHIT) {
				ExecPixel(matchMap, pTrack->GetHitIndex(iHit));
			} else if (hitType == kMUCHSTRAWHIT){
				ExecStraw(matchMap, pTrack->GetHitIndex(iHit));
			} else {
				TObject::Fatal("CbmMuchMatchTracks::Exec","Hit type not supported!");
			}
		} // Loop over hits

      Int_t nofTrue = 0;
      Int_t bestMcTrackId = -1;
      Int_t nPoints = 0;
      for (std::map<Int_t, Int_t>::iterator it = matchMap.begin(); it != matchMap.end(); it++) {
         if (it->first != -1 && it->second >= nofTrue) {
            bestMcTrackId = it->first;
            nofTrue = it->second;
         }
         nPoints += it->second;
      }

      Int_t nofFake = 0;
      Int_t nofWrong = nofHits - nofTrue - nofFake;
      Int_t nofMcTracks = matchMap.size() - 1;

		new ((*fMatches)[iTrack]) CbmTrackMatch(
				bestMcTrackId, nofTrue, nofWrong, nofFake, nofMcTracks);

		fNofHits += nofHits;
		fNofTrueHits += nofTrue;
		fNofWrongHits += nofWrong;
		fNofFakeHits += nofFake;

		if (fVerbose > 1)
		std::cout << "iTrack=" << iTrack << " mcTrack=" << bestMcTrackId
			<< " nPoints=" << nPoints << " nofTrue=" << nofTrue
			<< " nofWrong=" << nofWrong << " nofFake=" << nofFake
			<< " nofMcTracks=" << nofMcTracks << std::endl;
	} // Loop over tracks

	fNEvents++;
}

void CbmMuchMatchTracks::Finish()
{
	Double_t trueHits = 100. * Double_t(fNofTrueHits)/Double_t(fNofHits);
	Double_t wrongHits = 100. * Double_t(fNofWrongHits)/Double_t(fNofHits);
	Double_t fakeHits = 100. * Double_t(fNofFakeHits)/Double_t(fNofHits);
	std::cout << "=================================================" << std::endl;
	std::cout << "=====   " << GetName() << ": Run summary " << std::endl;
	std::cout << "True hits: " << trueHits << "%" << std::endl;
	std::cout << "Wrong hits: " << wrongHits << "%" << std::endl;
	std::cout << "Fake hits: " << fakeHits << "%" << std::endl;
	std::cout << "=================================================" << std::endl;
}

void CbmMuchMatchTracks::ExecPixel(
		std::map<Int_t, Int_t> &matchMap,
		Int_t index)
{
   // std::set stores MC track indices contributed to a certain hit
   std::set<Int_t> mcIdHit;
   CbmMuchPixelHit* hit = static_cast<CbmMuchPixelHit*>(fPixelHits->At(index));
   if (hit == NULL) return;

   Int_t clusterId = hit->GetRefId();
   CbmMuchCluster* cluster = static_cast<CbmMuchCluster*>(fClusters->At(clusterId));
   if (cluster == NULL) return;

   for (Int_t iDigi = 0; iDigi < cluster->GetNofDigis(); iDigi++){
       Int_t digiId = cluster->GetDigi(iDigi);
       CbmMuchDigiMatch* digiMatch = static_cast<CbmMuchDigiMatch*>(fPixelDigiMatches->At(digiId));
       if (digiMatch == NULL) continue;
       for (Int_t iPoint = 0; iPoint < digiMatch->GetNofLinks(); iPoint++) {
         Int_t pointIndex = digiMatch->GetLink(iPoint).GetIndex();
         if (pointIndex < 0) { // Fake or background hit
            mcIdHit.insert(-1);
            continue;
         }
         FairMCPoint* point = static_cast<FairMCPoint*>(fPoints->At(pointIndex));
         if (point == NULL) continue;
         mcIdHit.insert(point->GetTrackID());
       }
   } // loop over digis

   for (std::set<Int_t>::iterator it = mcIdHit.begin(); it != mcIdHit.end(); it++) {
      matchMap[*it]++;
   }
}

void CbmMuchMatchTracks::ExecStraw(
		std::map<Int_t, Int_t> &matchMap,
		Int_t index)
{
	CbmMuchStrawHit* hit = static_cast<CbmMuchStrawHit*>(fStrawHits->At(index));
	if (hit == NULL) return;

	Int_t digiIndex = hit->GetRefId();
   CbmMuchDigiMatch* digiMatch = static_cast<CbmMuchDigiMatch*>(fStrawDigiMatches->At(digiIndex));
   if (digiMatch == NULL) return;

   for (Int_t iPoint = 0; iPoint < digiMatch->GetNofLinks(); iPoint++) {
      Int_t pointIndex = digiMatch->GetLink(iPoint).GetIndex();
      if (pointIndex < 0) { // Fake or background hit
         matchMap[-1]++;
         continue;
      }
      FairMCPoint* point = static_cast<FairMCPoint*>(fPoints->At(pointIndex));
      if (point == NULL) continue;
      matchMap[point->GetTrackID()]++;
   }
}

ClassImp(CbmMuchMatchTracks);
