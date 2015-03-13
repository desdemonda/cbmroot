// -------------------------------------------------------------------------
// -----                CbmTrdTrackFitterIdeal header file             -----
// -----                  Created 12/05/06  by D. Kresan               -----
// -------------------------------------------------------------------------
#ifndef CBMTRDTRACKFITTERIDEAL
#define CBMTRDTRACKFITTERIDEAL

#include "CbmTrdTrackFitter.h"

class TClonesArray;
class CbmTrdPoint;
class FairTrackParam;
class CbmTrdTrack;


class CbmTrdTrackFitterIdeal : public CbmTrdTrackFitter {

private:
    TClonesArray *fArrayTrdPoint;   // TRD points
    TClonesArray *fArrayTrdHit;     // TRD hits

    CbmTrdTrackFitterIdeal(const CbmTrdTrackFitterIdeal&);
    CbmTrdTrackFitterIdeal& operator=(const CbmTrdTrackFitterIdeal&);

public:
    CbmTrdTrackFitterIdeal();
    virtual ~CbmTrdTrackFitterIdeal();

    void Init();
    Int_t DoFit(CbmTrdTrack* pTrack);

    void SetTrackParam(CbmTrdPoint* point, FairTrackParam* trackParam);

    ClassDef(CbmTrdTrackFitterIdeal,1);
};

#endif
