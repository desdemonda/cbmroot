// ----------------------------------------------------------------------------
// -----                   CbmRichProtPrepareExtrapolation                -----
// -----                   Created 26-07-2010 by D.Kresan                 -----
// ----------------------------------------------------------------------------

#ifndef CBM_RICH_PROT_PREPARE_EXTRAPOLATION
#define CBM_RICH_PROT_PREPARE_EXTRAPOLATION

#include "FairTask.h"

class TClonesArray;


class CbmRichProtPrepareExtrapolation : public FairTask {

public:
    CbmRichProtPrepareExtrapolation();
    CbmRichProtPrepareExtrapolation(Int_t verbose);
    virtual ~CbmRichProtPrepareExtrapolation();

private:
    Float_t fPosReso;
    Float_t fThetaReso;
    TClonesArray *fArrayMCTracks;
    TClonesArray *fArrayStsTracks;
    TClonesArray *fArrayGlobalTracks;

public:
    virtual void SetParContainers();
    virtual InitStatus Init();

    virtual void Exec(Option_t *option);

    virtual void Finish();

    inline void SetReso(Float_t pos, Float_t theta) { fPosReso = pos; fThetaReso = theta; }

private:

    CbmRichProtPrepareExtrapolation(const CbmRichProtPrepareExtrapolation&);
    CbmRichProtPrepareExtrapolation operator=(const CbmRichProtPrepareExtrapolation&);

    ClassDef(CbmRichProtPrepareExtrapolation, 1)
};


#endif

