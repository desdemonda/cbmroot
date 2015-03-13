// ------------------------------------------------------------------
// -----                      CbmProduceDst                     -----
// -----             Created 2008-01-18 by D.Kresan             -----
// ------------------------------------------------------------------

#ifndef CBM_PRODUCE_DST
#define CBM_PRODUCE_DST


#include "FairTask.h"


class TClonesArray;
class CbmVertex;
class CbmStsKFTrackFitter;
class TH2F;


class CbmProduceDst : public FairTask {

public:
    CbmProduceDst();
    CbmProduceDst(Int_t iVerbose);
    virtual ~CbmProduceDst();

private:
    Int_t         fEvents;
    Int_t         fHadrons;
    TClonesArray *fArrayMCTrack;
    TClonesArray *fArrayTofPoint;
    TClonesArray *fArrayStsTrack;
    TClonesArray *fArrayStsTrackM;
    TClonesArray *fArrayTrdTrack;
    TClonesArray *fArrayTrdTrackM;
    TClonesArray *fArrayTofHit;
    TClonesArray *fArrayGlobalTrack;
    CbmVertex    *fPrimVertex;
    CbmStsKFTrackFitter *fTrackFitter;
    TClonesArray *fArrayHadron;

    TH2F *fh_pdl;

    CbmProduceDst(const CbmProduceDst&);
    CbmProduceDst operator=(const CbmProduceDst&);

public:
    virtual InitStatus Init();
    virtual InitStatus ReInit();
    virtual void SetParContainers();
    virtual void Exec(Option_t *option);
    virtual void Finish();
    void WriteHisto();

    ClassDef(CbmProduceDst, 1);
};


#endif

