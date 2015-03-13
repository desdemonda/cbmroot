// ------------------------------------------------------------------
// -----                  CbmFindGlobalTracksQa                 -----
// -----             Created 26-01-2006 by D.Kresan             -----
// ------------------------------------------------------------------
#ifndef CBMFINDGLOBALTRACKSQA
#define CBMFINDGLOBALTRACKSQA

#include "FairTask.h"

class TClonesArray;
class TH1F;


class CbmFindGlobalTracksQa : public FairTask {

private:
    Int_t         fMinStsPoints;             // Required minimum of STS points
    Int_t         fMinTrdPoints;             // Required minimum of TRD points
    Bool_t        fRequireTof;               // TOF flag
    TClonesArray *fArrayMCTrack;             // MC tracks
    TClonesArray *fArrayTofPoint;            // TOF points
    TClonesArray *fArrayTofHit;              // TOF hit
    TClonesArray *fArrayGlbTrack;            // Global tracks
    TClonesArray *fArrayStsTrackMatch;       // STS track match
    TClonesArray *fArrayTrdTrackMatch;       // TRD track match
    TH1F         *fh_mom_acc;                // Momentum of acc MC tracks
    TH1F         *fh_mom_acc2;               // Momentum of acc MC tracks, which
                                             // are reco in STS and TRD
    TH1F         *fh_mom_rec;                // Momentum of good global tracks
    TH1F         *fh_mom_ghost;              // Momentum of ghost global tracks
    TH1F         *fh_mom_eff;                // Efficiency of good track finding
    TH1F         *fh_mom_eff_ghost;          // Efficiency of ghost track finding
    TH1F         *fh_mom_eff_tof;            // Efficiency of track-TOF merging
    TH1F         *fh_mom_eff_tof_ghost;      // Efficiency of fake track-TOF merging
    void CreateHistogramms();

    CbmFindGlobalTracksQa(const CbmFindGlobalTracksQa&);
    CbmFindGlobalTracksQa& operator=(const CbmFindGlobalTracksQa&);

public:
    CbmFindGlobalTracksQa();
    CbmFindGlobalTracksQa(const char* name, Int_t verbose = 1);
    virtual ~CbmFindGlobalTracksQa();

    virtual InitStatus Init();
    virtual void Exec(Option_t* option = "");
    virtual void Finish();

    void WriteHistogramms();

    inline void SetMinStsPoints(Int_t minStsPoints)  {  fMinStsPoints = minStsPoints; }
    inline void SetMinTrdPoints(Int_t minTrdPoints)  {  fMinTrdPoints = minTrdPoints; }
    inline void SetRequireTof  (Bool_t requireTof)   {  fRequireTof = requireTof;     }

    ClassDef(CbmFindGlobalTracksQa, 1);
};


#endif

