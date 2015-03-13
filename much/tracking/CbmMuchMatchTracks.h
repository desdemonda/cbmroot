/** CbmMuchMatchTracks.h
 *@author A.Lebedev <Andrey.Lebedev@gsi.de>
 *
 * 2009-03-16 E. Kryshen
 * Matching of hits to clusters with several MC points
 * 2009-06-01 A.Lebedev
 * Include matching for straw hits
 * 2011-05-19 A.Lebedev
 * Correct matching in case of clustering
 **/

#ifndef CBMMUCHMATCHTRACKS_H_
#define CBMMUCHMATCHTRACKS_H_ 1

#include "FairTask.h"
class CbmMCEpoch;
#include <map>

class TClonesArray;

class CbmMuchMatchTracks : public FairTask
{
public:
	CbmMuchMatchTracks();
	virtual ~CbmMuchMatchTracks();

	virtual InitStatus Init();
	virtual void Exec(
		  Option_t* opt);
	virtual void Finish();
        void SetEpoch(Bool_t isEpoch) { fIsEpoch = isEpoch; }
private:
	void ExecPixel(
			std::map<Int_t, Int_t> &matchMap,
			Int_t index);

	void ExecStraw(
			std::map<Int_t, Int_t> &matchMap,
			Int_t index);

	TClonesArray* fTracks;
	TClonesArray* fPoints;
	TClonesArray* fPixelHits;
	TClonesArray* fStrawHits;
	TClonesArray* fMatches;
	TClonesArray* fPixelDigiMatches;
	TClonesArray* fStrawDigiMatches;
	TClonesArray* fClusters;
        CbmMCEpoch*   fMcEpoch;
	
	Int_t fNofHits;
	Int_t fNofTrueHits;
	Int_t fNofWrongHits;
	Int_t fNofFakeHits;

	Int_t fNEvents;

	Bool_t fIsEpoch; // = 1 in the epoch approach
	
	CbmMuchMatchTracks(const CbmMuchMatchTracks&);
	CbmMuchMatchTracks& operator=(const CbmMuchMatchTracks&);

	ClassDef(CbmMuchMatchTracks,1);
};

#endif
