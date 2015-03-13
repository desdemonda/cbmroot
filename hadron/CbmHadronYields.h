// ------------------------------------------------------------------
// -----                     CbmHadronYields                    -----
// -----              Created 18/09/2006 by D.Kresan            -----
// ------------------------------------------------------------------
#ifndef _CBMHADRONYIELDS_H_
#define _CBMHADRONYIELDS_H_

#include <map>

#include "FairTask.h"

class TClonesArray;


class CbmHadronYields : public FairTask {

private:
    Int_t         fEvents;            // Number of processed events
    TClonesArray *fArrayMCTrack;      // MC tracks
    TClonesArray *fArrayStsTrack;     // STS tracks
    TClonesArray *fArrayStsTrackM;    // STS track matches
    TClonesArray *fArrayTrdTrack;     // TRD tracks
    TClonesArray *fArrayTrdTrackM;    // TRD track matches
    TClonesArray *fArrayTofHit;       // TOF hits
    TClonesArray *fArrayTofPoint;     // TOF points
    TClonesArray *fArrayGlbTrack;     // Global tracks

    std::map<Int_t, Int_t> fmap_all;

    std::map<Int_t, Int_t> fmap_sts_acc;
    std::map<Int_t, Int_t> fmap_trd_acc;
    std::map<Int_t, Int_t> fmap_glb_acc;

    std::map<Int_t, Int_t> fmap_sts_rec;
    std::map<Int_t, Int_t> fmap_trd_rec;
    std::map<Int_t, Int_t> fmap_glb_rec;

    CbmHadronYields(const CbmHadronYields&);
    CbmHadronYields operator=(const CbmHadronYields&);

public:
    CbmHadronYields();
    CbmHadronYields(const char* name, Int_t verbose = 1);
    virtual ~CbmHadronYields();

    virtual InitStatus Init();
    virtual void Exec(Option_t* option);
    virtual void Finish();

    ClassDef(CbmHadronYields, 1);
};


#endif
