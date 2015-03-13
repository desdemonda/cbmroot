//-------------------------------------------------------------------------
//-----                     CbmFitGlobalTracksQa                      -----
//-----                 Created 07/03/06 by D. Kresan                 -----
//-------------------------------------------------------------------------
#ifndef CBMFITGLOBALTRACKSQA
#define CBMFITGLOBALTRACKSQA

#include "FairTask.h"

class TClonesArray;
class TH1F;


class CbmFitGlobalTracksQa : public FairTask {

private:
    TClonesArray *fArrayStsPoint;     // STS points
    TClonesArray *fArrayTrdPoint;     // TRD points
    TClonesArray *fArrayStsHit;       // STS hits
    TClonesArray *fArrayTrdHit;       // TRD hits
    TClonesArray *fArrayStsTrack;     // STS tracks
    TClonesArray *fArrayTrdTrack;     // TRD tracks
    TClonesArray *fArrayGlbTrack;     // Global tracks
    Int_t         fEvents;            // Number of events processed
    TH1F         *fh_first_resx;      // X resolution at first plane
    TH1F         *fh_first_resy;      // Y resolution at first plane
    TH1F         *fh_first_restx;     // Tx resolution at first plane
    TH1F         *fh_first_resty;     // Ty resolution at first plane
    TH1F         *fh_first_resz;      // Z resolution at first plane
    TH1F         *fh_last_resx;       // X resolution at last plane
    TH1F         *fh_last_resy;       // Y resolution at last plane
    TH1F         *fh_last_restx;      // Tx resolution at last plane
    TH1F         *fh_last_resty;      // Ty resolution at last plane
    TH1F         *fh_last_resz;       // Z resolution at last plane
    TH1F         *fh_first_pullx;     // X pull at first plane
    TH1F         *fh_first_pully;     // Y pull at first plane
    TH1F         *fh_first_pulltx;    // Tx pull at first plane
    TH1F         *fh_first_pullty;    // Ty pull at first plane
    TH1F         *fh_last_pullx;      // X pull at last plane
    TH1F         *fh_last_pully;      // Y pull at last plane
    TH1F         *fh_last_pulltx;     // Tx pull at last plane
    TH1F         *fh_last_pullty;     // Ty pull at last plane
    TH1F         *fh_chi2ndf;         // Chi2/NDF of fitted tracks

    void CreateHistogramms();

    CbmFitGlobalTracksQa(const CbmFitGlobalTracksQa&);
    CbmFitGlobalTracksQa& operator=(const CbmFitGlobalTracksQa&);

public:
    CbmFitGlobalTracksQa();
    CbmFitGlobalTracksQa(const char* name, Int_t verbose = 1);
    virtual ~CbmFitGlobalTracksQa();

    virtual InitStatus Init();
    virtual void Exec(Option_t* option = "");
    virtual void Finish();

    void WriteHistogramms();

    ClassDef(CbmFitGlobalTracksQa, 1);
};


#endif


