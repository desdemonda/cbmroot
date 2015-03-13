/**
 * \file CbmStsHit.h
 * \author Volker Friese <v.friese@gsi.de>
 * \date 30.08.06
 * \brief Data class for a reconstructed hit in the STS
 *
 * Updated 14/03/2014 by Andrey Lebedev <andrey.lebedev@gsi.de>.
 **/

#ifndef CBMSTSSHIT_H
#define CBMSTSSHIT_H 1

#include "CbmPixelHit.h"

class TVector3;

class CbmStsHit : public CbmPixelHit
{

public:
    CbmStsHit();

    CbmStsHit(Int_t address, const TVector3& pos, const TVector3& dpos,
    		      Double_t dxy, Int_t frontClusterId, Int_t backClusterId,
    		      Int_t frontDigiId, Int_t backDigiId, Int_t sectorNr,
    		      Double_t time = 0., Double_t timeError = 0.);

    virtual ~CbmStsHit();

    Int_t GetFrontDigiId() const
    {
       return fFrontDigiId;
    }

    Int_t GetBackDigiId() const
    {
       return fBackDigiId;
    }

    // FIXME: Kept for backward compatibility only !!!
    Int_t GetSectorNr() const
    {
        return fSectorNr;
    }

    /** Time of hit
     ** @return Hit time [ns]
     **/
    Double_t GetTime() const { return fTime; }


    /** Time error of hit
     ** @return Hit time error [ns]
     **/
    Double_t GetTimeError() const { return fTimeError; }


    Int_t GetFrontClusterId() const
    {
       return fFrontClusterId;
    }

    Int_t GetBackClusterId() const
    {
       return fBackClusterId;
    }

    /** Set hit time
     ** @param time  Hit time [ns]
     **/
    void SetTime(Double_t time) { fTime = time; }

    /** Set hit time error
     ** @param time  Hit time error [ns]
     **/
    void SetTimeError(Double_t error) { fTimeError = error; }


private:
    // FIXME: In principle we should keep references to clusters and not digis, since hits are created from clusters.
    // FIXME: However for the moment this digi indices are used in L1 so we keep it for backward compatibility.
    Int_t fFrontDigiId; // Channel number front side
    Int_t fBackDigiId;  // Channel number back side
    // FIXME: Kept for backward compatibility only !!!
    Int_t fSectorNr;

    Int_t fFrontClusterId; // Cluster number front side
    Int_t fBackClusterId;  // Cluster number back side

    Double_t fTime;       ///< Hit time [ns]
    Double_t fTimeError;  ///< Hit time error [ms]

    ClassDef(CbmStsHit, 4);
};

#endif
