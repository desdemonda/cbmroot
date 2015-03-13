/** CbmMuchTrackFinderIdeal.cxx
 * @author A.Lebedev <Andrey.Lebedev@gsi.de>
 * @since 2007
 **/
#include "CbmMuchTrackFinderIdeal.h"

#include "CbmMuchStrawHit.h"
#include "CbmMuchPixelHit.h"
#include "CbmMuchTrack.h"
#include "CbmMuchCluster.h"
#include "CbmMuchDigiMatch.h"

#include "CbmDetectorList.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairTrackParam.h"

#include "TClonesArray.h"

#include <iostream>
#include <map>
#include <cmath>

CbmMuchTrackFinderIdeal::CbmMuchTrackFinderIdeal()
  : CbmMuchTrackFinder(),
    fMCTracks(NULL),
    fMCPoints(NULL),
    fPixelHits(NULL),
    fStrawHits(NULL),
    fTracks(NULL),
    fPixelDigiMatches(NULL),
    fStrawDigiMatches(NULL),
    fClusters(NULL),
    fTrackMap(),
    fVerbose(1),
    fEvents(0)
{
}

CbmMuchTrackFinderIdeal::~CbmMuchTrackFinderIdeal()
{
}

void CbmMuchTrackFinderIdeal::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();
	if (ioman == NULL) Fatal("CbmMuchTrackFinderIdeal::Init", "No FairRootManager!");

	fMCTracks  = (TClonesArray*) ioman->GetObject("MCTrack");
	if (fMCTracks == NULL) Fatal("CbmMuchTrackFinderIdeal::Init", "No MCTrack array!");

	fMCPoints  = (TClonesArray*) ioman->GetObject("MuchPoint");
	if (fMCPoints == NULL) Fatal("CbmMuchTrackFinderIdeal::Init", "No MuchPoint array!");

	fPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
	fStrawHits = (TClonesArray*) ioman->GetObject("MuchStrawHit");
	if (fPixelHits == NULL && fStrawHits == NULL) Fatal("CbmMuchTrackFinderIdeal::Init", "No MuchPixelHit && MuchStrawHit array!");

	fPixelDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
	fStrawDigiMatches = (TClonesArray*) ioman->GetObject("MuchStrawDigiMatch");
	if (fPixelDigiMatches == NULL && fStrawDigiMatches == NULL) Fatal("CbmMuchTrackFinderIdeal::Init", "No MuchDigiMatch && MuchStrawDigiMatch array!");

	fClusters = (TClonesArray*) ioman->GetObject("MuchCluster");
	if (fClusters == NULL) Info("CbmMuchTrackFinderIdeal::Init", "No cluster array -- simple hit to digi matching will be used");
}

Int_t CbmMuchTrackFinderIdeal::DoFind(
        TClonesArray* trackArray)
{
	fTracks = trackArray;

	fTrackMap.clear();
	Int_t nofTracks = 0;
	Int_t nofMCTracks = fMCTracks->GetEntriesFast();
	for (Int_t iMCTrack=0; iMCTrack < nofMCTracks; iMCTrack++) {
		CbmMCTrack*  mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrack);
		if (mcTrack == NULL) continue;
		if (std::abs(mcTrack->GetPdgCode()) != 13 ) continue;
		if (mcTrack->GetMotherId() != -1) continue;

		new((*fTracks)[nofTracks]) CbmMuchTrack();
		fTrackMap[iMCTrack] = nofTracks;
		nofTracks++;
	}

	// Process MUCH pixel hits
	for (Int_t iHit = 0; iHit < fPixelHits->GetEntriesFast(); iHit++) {
		CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fPixelHits->At(iHit);
		if (!hit) continue;
		Int_t clusterId = hit->GetRefId();
		CbmMuchCluster* cluster = (CbmMuchCluster*) fClusters->At(clusterId);
		for (Int_t iDigi = 0; iDigi < cluster->GetNofDigis(); iDigi++){
			ProcessDigiMatches(fPixelDigiMatches, cluster->GetDigi(iDigi), iHit, kMUCHPIXELHIT);
		} // loop over digis in cluster
	} // loop over hits

	// Process MUCH straw hits
	for (Int_t iHit = 0; iHit < fStrawHits->GetEntriesFast(); iHit++) {
		CbmMuchStrawHit* hit = (CbmMuchStrawHit*) fStrawHits->At(iHit);
		if (!hit) continue;
		ProcessDigiMatches(fStrawDigiMatches, hit->GetRefId(), iHit, kMUCHSTRAWHIT);
	} // loop over hits

	std::cout << "Event: " << fEvents++ << std::endl;
	return nofTracks;
}

void CbmMuchTrackFinderIdeal::ProcessDigiMatches(
		const TClonesArray* digiMatches,
		Int_t digiIndex,
		Int_t hitIndex,
		HitType hitType)
{
	CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*) digiMatches->At(digiIndex);
	if (!digiMatch) return;

	for (Int_t iDigi = 0; iDigi < digiMatch->GetNofLinks(); iDigi++){
		Int_t pointIndex = digiMatch->GetLink(iDigi).GetIndex();
		if (pointIndex < 0) return;
		FairMCPoint* mcPoint = (FairMCPoint*) (fMCPoints->At(pointIndex));
		if (mcPoint == NULL) return;
		Int_t mcTrackIndex = mcPoint->GetTrackID();
		if (mcTrackIndex < 0) return;
		CbmMCTrack*  mcTrack = (CbmMCTrack*) fMCTracks->At(mcTrackIndex);
		if (mcTrack == NULL) return;

		if (fTrackMap.find(mcTrackIndex) == fTrackMap.end()) continue;

		Int_t trackIndex = fTrackMap[mcTrackIndex];
		CbmMuchTrack* track = (CbmMuchTrack*) fTracks->At(trackIndex);
		track->AddHit(hitIndex, hitType);

		if (track->GetNofHits() == 1){
			SetTrackParam(mcTrack, mcPoint, track);
		}
	} // loop over digis
}

void CbmMuchTrackFinderIdeal::SetTrackParam(
		const CbmMCTrack* mcTrack,
		const FairMCPoint* mcPoint,
		CbmMuchTrack* track)
{
	FairTrackParam par;
	par.SetX(mcPoint->GetX());
    par.SetY(mcPoint->GetY());
    par.SetTx(mcPoint->GetPx()/mcPoint->GetPz());
    par.SetTy(mcPoint->GetPy()/mcPoint->GetPz());
    if (mcTrack->GetPdgCode() == 13) par.SetQp(-1./mcTrack->GetP());
    else
    if (mcTrack->GetPdgCode() == -13) par.SetQp(1./mcTrack->GetP());
    par.SetZ(mcPoint->GetZ());

    Double_t cov[15];
    cov[0] = 1e-9; cov[1] = 1e-9;  cov[2] = 1e-9; cov[3] = 1e-9; cov[4] = 1e-9;
                   cov[5] = 1e-9;  cov[6] = 1e-9; cov[7] = 1e-9; cov[8] = 1e-9;
                                   cov[9] = 1e-9; cov[10]= 1e-9; cov[11]= 1e-9;
                                                  cov[12]= 1e-9; cov[13]= 1e-9;
                                                                 cov[14]= 1e-9;
    par.SetCovMatrix(cov);
    track->SetParamLast(&par);
    track->SetParamFirst(&par);
}

ClassImp(CbmMuchTrackFinderIdeal);
