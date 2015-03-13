// -----------------------------------------------------------------------
// -----                    CbmL1TrdTrackFinderSts                   -----
// -----                 Created 6/12/05  by D. Kresan               -----
// -----------------------------------------------------------------------

#ifndef CBM_L1_TRD_TRACK_FINDER_STS
#define CBM_L1_TRD_TRACK_FINDER_STS

#include "CbmTrdTrackFinder.h"

#include "CbmTrdTrack.h"

#include <map>
#include <vector>

class TClonesArray;
class CbmTrdHit;
class CbmKFTrack;
class TH1F;
class TH2F;


class CbmL1TrdTrackFinderSts : public CbmTrdTrackFinder {
  private:
    CbmL1TrdTrackFinderSts(const CbmL1TrdTrackFinderSts&);
    CbmL1TrdTrackFinderSts operator=(const CbmL1TrdTrackFinderSts&);
private:
    Int_t                fEvents;                   // Number of events processed
    Int_t                fVerbose;                  // Verbosity level
    TClonesArray        *fArrayTrdPoint;            // Array of TRD points
    TClonesArray        *fArrayTrdHit;              // Array of TRD hits
    TClonesArray        *fArrayStsTrack;            // Array of STS tracks
    TClonesArray        *fArrayStsTrackM;           // Array of STS tracks
    TClonesArray        *fArrayKFTrdHit;            // Array of KF TRD hits
    std::vector<CbmTrdTrack*> fvTrdTrack;                // Working array of TRD tracks
    TClonesArray        *fArrayTrdTrack;            // Output Array of TRD tracks
    Int_t                fPid;                      // PID assumption
    Int_t                fNoTrdStations;            // Number of TRD stations
    Int_t                fNoTrdPerStation;          // Number of TRD layers per station
    Int_t                fNoTrdHits[12];            // Number of TRD hits per station
    Int_t                fTrdHitIndex[12][10000];   // Indices of TRD hits per station
    std::map<Int_t, Bool_t>   fmapHitUsed;               // Map from hit index to boolean flag
    TH1F                *fh_chi2hit;                // Control histogramm
    TH2F                *fh_chi2hit_plane;          // Control histogramm
    TH2F                *fh_resx_plane_true;        // Control histogramm
    TH2F                *fh_resy_plane_true;        // Control histogramm
    TH2F                *fh_resx_plane_fake;        // Control histogramm
    TH2F                *fh_resy_plane_fake;        // Control histogramm
    TH2F                *fh_resx_mom_true;          // Control histogramm
    TH2F                *fh_resy_mom_true;          // Control histogramm
    TH2F                *fh_pullx_plane_true;        // Control histogramm
    TH2F                *fh_pully_plane_true;        // Control histogramm
    TH2F                *fh_pullx_plane_fake;        // Control histogramm
    TH2F                *fh_pully_plane_fake;        // Control histogramm
    std::map<Int_t, Bool_t>   fLostTracks;

    void CreateHistogramms();
    void DataBranches();
    void TrdLayout();
    void SortTrdHits();
    void Process();
    void Sts2Trd(Double_t pmin, Double_t pmax,
		 Double_t chi2min, Double_t chi2max);
    void ProcessAllStations();
    void MoveOut();
    void ProcessStation(CbmTrdTrack* pTrack, const Int_t& station);
    void UpdateTrack(Int_t station, CbmTrdTrack* track);
    virtual void Clear(const Option_t* a = 0);
    void RemoveFakes();
    Bool_t Overlap(CbmKFTrack& track, CbmTrdHit* pHit);
    Double_t GetChi2Hit(CbmKFTrack& track, CbmTrdHit* pHit);

public:
    CbmL1TrdTrackFinderSts();
    CbmL1TrdTrackFinderSts(Int_t verbose);
    virtual ~CbmL1TrdTrackFinderSts();

    void Init();
    Int_t DoFind(TClonesArray* hitArray,
                 TClonesArray* trackArray);
    void WriteHistogramms();

    inline void SetVerbose(Int_t verbose) {fVerbose = verbose;}

	static Bool_t CompareChi2(const CbmTrdTrack* a, const CbmTrdTrack* b)
	{
    return ( a->GetChiSq()/static_cast<Double_t>(a->GetNDF()) <
        b->GetChiSq()/static_cast<Double_t>(b->GetNDF()) );
	};

    ClassDef(CbmL1TrdTrackFinderSts, 1);
};


#endif
