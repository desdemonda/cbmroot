// -----------------------------------------------------------------------
// -----                  CbmTrdTrackFitterKF                        -----
// -----             Created 29/11/05  by D. Kresan                  -----
// -----------------------------------------------------------------------
#ifndef CBMTRDTRACKFITTERKF_CA
#define CBMTRDTRACKFITTERKF_CA

#include "CbmTrdTrackFitter.h"

class TClonesArray;
class CbmTrdTrack;
class CbmKFTrack;

//Section added 28.04.2008 by Maciej Krauze
class CbmVertex;
class FairTrackParam;
//end of added section

class CbmTrdTrackFitterKF_CA : public CbmTrdTrackFitter {

private:
    TClonesArray *fArrayTrdHit;   // Array of TRD hits
    Int_t         fVerbose;       // Verbosity level
    Int_t         fPid;           // Mass hypothesis
    CbmKFTrack   *fKfTrack;       // KF track

public:
    CbmTrdTrackFitterKF_CA();
    CbmTrdTrackFitterKF_CA(Int_t verbose, Int_t pid);
    virtual ~CbmTrdTrackFitterKF_CA();

    void Init();
    Int_t DoFit(CbmTrdTrack* pTrack);

    inline Int_t GetVerbose() {  return fVerbose; }
    inline Int_t GetPid()     {  return fPid;     }

    inline void SetVerbose(Int_t verbose) {  fVerbose = verbose; }
    inline void SetPid    (Int_t pid)     {  fPid = pid;         }

   //Section added 28.04.2008 by Maciej Krauze
   Double_t FitToVertex( CbmTrdTrack* track, CbmVertex *vtx, FairTrackParam *v_track );
   
   /** Extrapolate track to any Z position
    *     *    *
    *     *    * @param track    already fitted STS track
    *     *    * @param z        new z-position
    *     *    * @param e_track  (return value) extrapolated track parameters
    *     *    */
   void Extrapolate( CbmTrdTrack* track, Double_t z, FairTrackParam *e_track );
   
   //end of added section
   
   CbmTrdTrackFitterKF_CA(const CbmTrdTrackFitterKF_CA&);
   CbmTrdTrackFitterKF_CA& operator=(const CbmTrdTrackFitterKF_CA&);

   ClassDef(CbmTrdTrackFitterKF_CA, 1);
};


#endif


