// -------------------------------------------------------------------------
// -----                      CbmTrdTrack header file                  -----
// -----                  Created 11/07/05  by M. Kalisky              -----
// -----                  Modified 04/06/09 by A. Lebedev               -----
// -------------------------------------------------------------------------

/**  CbmTrdTrack.h
 *@author M.Kalisky <m.kalisky@gsi.de>
 **
 ** TRD local track. Holds lists of CbmTrdHits and the fitted
 ** track parameters. The fit parameters are of type FairTrackParam
 ** and can only be accessed and modified via this class.
 **/
#ifndef CBMTRDTRACK_H_
#define CBMTRDTRACK_H_ 1

#include "CbmTrack.h"

class CbmTrdTrack : public CbmTrack
{
public:
    /** Default constructor **/
    CbmTrdTrack();

    /** Destructor **/
    virtual ~CbmTrdTrack();

    /** Accessors  **/
    Double_t GetPidWkn() const { return fPidWkn; }
    Double_t GetPidANN() const { return fPidANN; }
    Double_t GetELoss() const { return fELoss;}
    Double_t GetPidLikeEL() const { return fPidLikeEL; }
    Double_t GetPidLikePI() const { return fPidLikePI; }
    Double_t GetPidLikeKA() const { return fPidLikeKA; }
    Double_t GetPidLikePR() const { return fPidLikePR; }
    Double_t GetPidLikeMU() const { return fPidLikeMU; }

    /** Modifiers  **/
    void SetPidWkn(Double_t pid) { fPidWkn = pid; }
    void SetPidANN(Double_t pid) { fPidANN = pid; }
    void SetELoss(Double_t eLoss){ fELoss = eLoss; }
    void SetPidLikeEL(Double_t value) { fPidLikeEL = value; }
    void SetPidLikePI(Double_t value) { fPidLikePI = value; }
    void SetPidLikeKA(Double_t value) { fPidLikeKA = value; }
    void SetPidLikePR(Double_t value) { fPidLikePR = value; }
    void SetPidLikeMU(Double_t value) { fPidLikeMU = value; }

private:
    /** PID value based on Wkn method **/
    Double_t fPidWkn;

    /** PID value based on ANN method **/
    Double_t fPidANN;

    /** PID values based on Likelihood method **/
    Double_t fPidLikeEL;
    Double_t fPidLikePI;
    Double_t fPidLikeKA;
    Double_t fPidLikePR;
    Double_t fPidLikeMU;

    /** Total energy loss in TRD **/
    Double_t fELoss;

    ClassDef(CbmTrdTrack, 4);
};

#endif
