/** @file CbmStsFindHits.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17.06.2014
 **/


#ifndef CBMSTSFINDHITS_H
#define CBMSTSFINDHITS_H 1

#include <set>
#include "TStopwatch.h"
#include "FairTask.h"
#include "CbmStsModule.h"

class TClonesArray;
class CbmStsSetup;

using std::set;


/** @class CbmStsFindHits
 ** @brief Task class for finding STS hits
 ** @author Volker Friese <v.friese@gsi.de>
 ** @since 17.06.2014
 ** @version 1.0
 **
 ** This task constructes hits (3-d points) from clusters. In each module,
 ** the intersection points from each pair of front and back side
 ** clusters are calculated and stored as hit.
 **
 ** The actual cluster finding algorithm is defined in an engine class
 ** CbmStsClusterFinderSimple. Other, alternative algorithms can be
 ** plugged in.
 **/
class CbmStsFindHits : public FairTask
{

	public:
    /** Constructor **/
    CbmStsFindHits();


    /** Destructor  **/
    virtual ~CbmStsFindHits();


    /** Task execution
     ** Inherited from FairTask.
     **/
    virtual void Exec(Option_t* opt);


    /** End-of-run action
     ** Inherited from FairTask.
     **/
    virtual void Finish();


    /** End-of-event action
     ** Inherited from FairTaks
     **/
    virtual void FinishEvent();


    /** Initialisation
     ** Inherited from FairTask.
     **/
    virtual InitStatus Init();


	private:

    TClonesArray* fClusters;             ///< Input array of CbmStsCluster
    TClonesArray* fHits;                 ///< Output array of CbmStsHits
    CbmStsSetup*  fSetup;             ///< Instance of STS setup
    TStopwatch    fTimer;             ///< ROOT timer

    // --- Run counters
    Int_t    fNofEvents;       ///< Total number of events processed
    Double_t fNofClustersTot;  ///< Total number of clusters processed
    Double_t fNofHitsTot;      ///< Total number of hits produced
    Double_t fTimeTot;         ///< Total execution time

    /** Set of active modules in the current event **/
    set<CbmStsModule*> fActiveModules;

    /** Sort clusters into modules
     ** @return Number of clusters sorted
     **/
    Int_t SortClusters();


    /** Prevent usage of copy constructor and assignment operator **/
    CbmStsFindHits(const CbmStsFindHits&);
    CbmStsFindHits operator=(const CbmStsFindHits&);


    ClassDef(CbmStsFindHits, 2);
};

#endif
