/**
 * \file CbmStsCluster.h
 * \author V.Friese <v.friese@gsi.de>
 * \since 28.08.06
 * \brief Data class for STS clusters.
 *
 * Updated 25/06/2008 by R. Karabowicz.
 * Updated 04/03/2014 by A. Lebedev <andrey.lebedev@gsi.de>
 **/

#ifndef CBMSTSCLUSTER_H
#define CBMSTSCLUSTER_H 1

#include "CbmCluster.h"

/**
 * \class CbmStsCluster
 * \brief Data class for STS clusters.
 **/
class CbmStsCluster : public CbmCluster
{
public:
    /**
     * \brief Default constructor
     **/
    CbmStsCluster();

    /**
     * \brief Destructor
     **/
    virtual ~CbmStsCluster();

    /** Get cluster centre
     ** @param Cluster centre in channel units
     **/
    Double_t GetCentre() const { return fChannelMean; }


    /** Get cluster charge
     ** @value  Total cluster charge [e]
     **/
    Double_t GetCharge() const { return fCharge; }


    /** Get cluster index
     ** @return Index of cluster in cluster array
     **/
    Int_t GetIndex() const { return fIndex; }


    /** @brief Set cluster index
     ** To keep track of the input during hit finding
     ** @param index  Index of cluster in cluster array
     **/
    void SetIndex(Int_t index) { fIndex = index; }

    /**   Sector number
     **    Kept for bacjwrad compatibility
     **/
    void SetSectorNr(Int_t sectorNr)
    {
        fSectorNr = sectorNr;
    }
    void SetMean(Double_t mean)
    {
        fMean = mean;
    }
    void SetMeanError(Double_t meanError)
    {
        fMeanError = meanError;
    }

    /** Accessors **/
    Double_t GetMean() const
    {
        return fMean;
    }
    Double_t GetMeanError() const
    {
        return fMeanError;
    }
    /**   Sector number
     **    Kept for bacjwrad compatibility
     **/
    Int_t GetSectorNr() const
    {
        return fSectorNr;
    }


    /** Time **/
    Double_t GetTime() const { return fTime; }


    /** Set charge, channel mean and channel mean square
     ** @param charge  Total charge in cluster
     ** @param channelMean  Charge-weighted mean channel number
     ** @param channelMeanSq  Charge-weighted mean square channel number
     **/
    void SetProperties(Double_t charge, Double_t channelMean,
    		               Double_t channelMeanSq, Double_t time = 0.) {
    	fCharge        = charge;
    	fChannelMean   = channelMean;
    	fChannelMeanSq = channelMeanSq;
    	fTime          = time;
    }


private:
    // This two parameters are kept for backward compatibility only.
    Double_t fMean; // FIXME: Modify clustering algorithm and remove this parameter.
    Double_t fMeanError; // FIXME: Modify clustering algorithm and remove this parameter.

    Double_t fCharge;        ///< Total charge
    Double_t fChannelMean;   ///< Charge-weighted mean channel number
    Double_t fChannelMeanSq; ///< Charge-weighted mean square channel number
    Double_t fTime;          ///< Cluster time (average of digi times)
    Int_t    fIndex;         ///< Index of cluster in input array

    // TODO: fSectorNr is here for backward compatibility with classes
    // using CbmStsDigiScheme. Should be eventually removed.
    Int_t fSectorNr; ///< Number of sector within station

    ClassDef(CbmStsCluster, 4);
};

#endif
