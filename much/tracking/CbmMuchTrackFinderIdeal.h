/** CbmMuchTrackFinderIdeal.h
 * @author A.Lebedev <Andrey.Lebedev@gsi.de>
 * @since 2007
 **
 ** Ideal MUCH track finder. Finds tracks using MC information.
 ** Finds only signal muon tracks, i.e. PDG +/- 13 and MotherId = -1
 **/

#ifndef CBMMUCHTRACKFINDERIDEAL_H_
#define CBMMUCHTRACKFINDERIDEAL_H_ 1

#include "CbmMuchTrackFinder.h"

#include "CbmHit.h"

#include <map>

class TClonesArray;
class CbmMCTrack;
class FairMCPoint;
class CbmMuchTrack;

class CbmMuchTrackFinderIdeal : public CbmMuchTrackFinder
{
public:
	CbmMuchTrackFinderIdeal();
	virtual ~CbmMuchTrackFinderIdeal();

	virtual void Init();

	Int_t DoFind(
			TClonesArray* trackArray);

private:
	void ProcessDigiMatches(
			const TClonesArray* digiMatches,
			Int_t digiIndex,
			Int_t hitIndex,
			HitType hitType);

	void SetTrackParam(
			const CbmMCTrack* mcTrack,
			const FairMCPoint* mcPoint,
			CbmMuchTrack* track);

	TClonesArray* fMCTracks;
	TClonesArray* fMCPoints;
	TClonesArray* fPixelHits;
	TClonesArray* fStrawHits;
	TClonesArray* fTracks;
	TClonesArray* fPixelDigiMatches;
	TClonesArray* fStrawDigiMatches;
	TClonesArray* fClusters;

	// MCTrack index to MuchTrack index
	std::map<Int_t, Int_t> fTrackMap;

	Int_t fVerbose;
	Int_t fEvents;

	CbmMuchTrackFinderIdeal(const CbmMuchTrackFinderIdeal&);
	CbmMuchTrackFinderIdeal& operator=(const CbmMuchTrackFinderIdeal&);

	ClassDef(CbmMuchTrackFinderIdeal, 1);
};

#endif
