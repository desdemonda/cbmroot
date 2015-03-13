// ------------------------------------------------------------------
// -----                    CbmL1TrackMerger                    -----
// -----             Created 2006-01-24 by D.Kresan             -----
// ------------------------------------------------------------------
#ifndef _CBM_L1_TRACK_MERGER_
#define _CBM_L1_TRACK_MERGER_

#include "CbmTrackMerger.h"

class TClonesArray;
class CbmKFTrack;
class FairTrackParam;
class TH2F;


class CbmL1TrackMerger : public CbmTrackMerger {
  private:
    CbmL1TrackMerger(const CbmL1TrackMerger&); // CHECKME
    CbmL1TrackMerger operator=(const CbmL1TrackMerger&);
private:
    Int_t         fMethod;         // Merging method: 1 - based on StsTrackIndex from TRD track
                                   //                 2 - merging of tracks at the first
    TClonesArray *fArrayStsTrackM; // Array of STS track matches
    TClonesArray *fArrayTrdTrackM; // Array of TRD track matches
    TH2F         *fh_dx_true;      // Control histogramm
    TH2F         *fh_dx_false;     // Control histogramm
    TH2F         *fh_dy_true;      // Control histogramm
    TH2F         *fh_dy_false;     // Control histogramm
    TH2F         *fh_dtx_true;     // Control histogramm
    TH2F         *fh_dtx_false;    // Control histogramm
    TH2F         *fh_dty_true;     // Control histogramm
    TH2F         *fh_dty_false;    // Control histogramm

    Double_t GetChi2XY(CbmKFTrack &kfTrack, FairTrackParam *trackParam);
    void CreateHistogramms();
    Int_t MergeSimple(TClonesArray *stsTracks,
		      TClonesArray *trdTracks,
		      TClonesArray *glbTracks);
    Int_t MergeImPlane(TClonesArray *stsTracks,
		       TClonesArray *trdTracks,
		       TClonesArray *glbTracks);

public:
    CbmL1TrackMerger();
    CbmL1TrackMerger(Int_t verbose);
    virtual ~CbmL1TrackMerger();

    void  Init();
    Int_t DoMerge(TClonesArray* stsTracks,
		  TClonesArray* trdTracks,
		  TClonesArray* glbTracks);
    void WriteHistogramms();

    inline Int_t GetVerbose() const {  return fVerbose; }
    inline Int_t GetMethod()  const {  return fMethod;  }

    inline void SetVerbose(Int_t verbose) {  fVerbose = verbose; }
    inline void SetMethod(Int_t method)   {  fMethod = method;   }

    ClassDef(CbmL1TrackMerger, 1);
};


#endif

