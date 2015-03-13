// -----------------------------------------------------------------------
// -----                  CbmTrdTrackFitterKF                        -----
// -----             Created 29/11/05  by D. Kresan                  -----
// -----------------------------------------------------------------------
#ifndef CBMTRDTRACKFITTERKF
#define CBMTRDTRACKFITTERKF

#include "CbmTrdTrackFitter.h"

class TClonesArray;
class CbmTrdTrack;
class CbmKFTrack;


class CbmTrdTrackFitterKF : public CbmTrdTrackFitter {

private:
    TClonesArray *fArrayTrdHit;   // Array of TRD hits
    Int_t         fVerbose;       // Verbosity level
    Int_t         fPid;           // Mass hypothesis
    CbmKFTrack   *fKfTrack;       // KF track

public:
    CbmTrdTrackFitterKF();
    CbmTrdTrackFitterKF(Int_t verbose, Int_t pid);
    virtual ~CbmTrdTrackFitterKF();

    void Init();
    Int_t DoFit(CbmTrdTrack* pTrack);

    inline Int_t GetVerbose() {  return fVerbose; }
    inline Int_t GetPid()     {  return fPid;     }

    inline void SetVerbose(Int_t verbose) {  fVerbose = verbose; }
    inline void SetPid    (Int_t pid)     {  fPid = pid;         }


    ClassDef(CbmTrdTrackFitterKF, 1);
 private:
  CbmTrdTrackFitterKF(const CbmTrdTrackFitterKF&);
  void operator=(const CbmTrdTrackFitterKF&);
};


#endif


