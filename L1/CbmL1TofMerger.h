// ------------------------------------------------------------------
// -----                     CbmL1TofMerger                     -----
// -----             Created 24-01-2006 by D.Kresan             -----
// ------------------------------------------------------------------
#ifndef _CBML1TOFMERGER_
#define _CBML1TOFMERGER_

#include "CbmTofMerger.h"

class TClonesArray;
class CbmKFTrack;
class CbmTofHit;
class FairTrackParam;


class CbmL1TofMerger : public CbmTofMerger {
  private:
    CbmL1TofMerger(const CbmL1TofMerger&); // CHECKME
    CbmL1TofMerger operator=(const CbmL1TofMerger&);
private:
    TClonesArray *fArrayTrdTrack; // Array of TRD tracks

    Bool_t Overlap(CbmKFTrack& track,
		   const CbmTofHit* tofHit);
    Double_t GetChi2ToHit(CbmKFTrack& track,
			  const CbmTofHit* tofHit);

public:
    CbmL1TofMerger();
    CbmL1TofMerger(Int_t verbose);
    ~CbmL1TofMerger();

    void Init();
    Int_t DoMerge(TClonesArray* glbTracks,
		  TClonesArray* tofHits);

    ClassDef(CbmL1TofMerger, 1);
};


#endif

