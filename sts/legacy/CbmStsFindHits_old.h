/**
 * \file CbmStsFindHits_old.h
 * \author Volker Friese <v.friese@gsi.de>
 * \since 11.09.06
 * \brief CBM task class for finding clusters in the STS
 *
 * Updated 26/06/2008 by R. Karabowicz.
 * Updated 03/03.2014 by A. Lebedev <andrey.lebedev@gsi.de>
 *
 **/

#ifndef CBMSTSFINDHITS_OLD_H
#define CBMSTSFINDHITS_OLD_H 1

#include "FairTask.h"

#include <map>
#include <set>

class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class CbmStsSector;
class CbmStsStation;

/**
 * \class CbmStsFindHits_old
 * \brief CBM task class for finding hits in the STS
 **/
class CbmStsFindHits_old : public FairTask
{

public:
    /**
     * \brief Default constructor
     **/
    CbmStsFindHits_old();

    /**
     * \brief Destructor
     **/
    virtual ~CbmStsFindHits_old();

    /**
     * \brief Execution
     **/
    virtual void Exec(Option_t* opt);

    /**
     * \brief Inherited from FairTask.
     **/
    virtual void Finish();

    /**
     * \brief Inherited from FairTask.
     **/
    virtual void SetParContainers();

    /**
     * \brief Inherited from FairTask.
     **/
    virtual InitStatus Init();

private:

    void MakeSets();

    void SortClusters();

    void FindHits(CbmStsStation* station, CbmStsSector* sector, const std::set<Int_t>& fSet, const std::set<Int_t>& bSet);

    CbmStsFindHits_old(const CbmStsFindHits_old&);
    CbmStsFindHits_old operator=(const CbmStsFindHits_old&);

    CbmGeoStsPar* fGeoPar;         ///> Geometry parameters
    CbmStsDigiPar* fDigiPar;       ///> Digitisation parameters
    CbmStsDigiScheme* fDigiScheme; ///> Digitisation scheme

    TClonesArray* fClusters; ///< Input array of CbmStsCluster
    TClonesArray* fHits;     ///< Output array of CbmStsHit

    /// FIXME: Same problem as in CbmStsClusterFinder.
    /// FIXME: Pointer maps to set.
    std::map<const CbmStsSector*, std::set<Int_t> > fClusterMapF; ///> sector clusters (front)
    std::map<const CbmStsSector*, std::set<Int_t> > fClusterMapB; ///> sector clusters (back)

    ClassDef(CbmStsFindHits_old, 1);
};

#endif
