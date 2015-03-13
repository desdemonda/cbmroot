// ------------------------------------------------------------------
// -----                   CbmTofMergerIdeal                    -----
// -----            Created 06/03/2006 by D.Kresan              -----
// ------------------------------------------------------------------
#ifndef CBMTOFMERGERIDEAL
#define CBMTOFMERGERIDEAL

#include "CbmTofMerger.h"


class CbmTofMergerIdeal : public CbmTofMerger {

private:
    TClonesArray* fArrayTofPoint;      // Array of TOF points
    TClonesArray* fArrayStsTrackMatch; // Array of STS track matches
    TClonesArray* fArrayTrdTrackMatch; // Array of TRD track matches

    CbmTofMergerIdeal(const CbmTofMergerIdeal&);
    CbmTofMergerIdeal& operator=(const CbmTofMergerIdeal&);

public:
    CbmTofMergerIdeal();
    ~CbmTofMergerIdeal();

    void Init();
    Int_t DoMerge(TClonesArray* glbTracks,
		  TClonesArray* tofHits);

    ClassDef(CbmTofMergerIdeal, 1);
};


#endif

