/** @file CbmFindClusters.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 16.06.2014
 **/


#ifndef CBMSTSFINDCLUSTERS_H
#define CBMSTSFINDCLUSTERS_H 1

#include <map>
#include <set>
#include "TStopwatch.h"
#include "FairTask.h"
#include "CbmStsDigi.h"
#include "setup/CbmStsModule.h"

class TClonesArray;
class CbmStsSetup;
class CbmStsClusterFinderSimple;

using std::map;



/** @class CbmStsFindClusters
 ** @brief Task class for finding STS clusters
 ** @author Volker Friese <v.friese@gsi.de>
 ** @since 16.06.2014
 ** @version 1.0
 **
 ** This task groups StsDigis into clusters. Digis are first sorted w.r.t.
 ** the module they are registered by; the cluster finding is then performed
 ** in each module.
 **
 ** The actual cluster finding algorithm is defined in an engine class
 ** CbmStsClusterFinderSimple. Other, alternative algorithms can be
 ** plugged in.
 **/
class CbmStsFindClusters : public FairTask
{

	public:
    /** Constructor **/
    CbmStsFindClusters();


    /** Destructor  **/
    virtual ~CbmStsFindClusters();


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

    TClonesArray* fDigis;             ///< Input array of CbmStsDigi
    TClonesArray* fClusters;          ///< Output array of CbmStsCluster
    CbmStsSetup*  fSetup;             ///< Instance of STS setup
    CbmStsClusterFinderSimple* fFinder;  ///< Cluster finder
    TStopwatch    fTimer;             ///< ROOT timer

    // --- Run counters
    Int_t     fNofEvents;       ///< Total number of events processed
    Double_t  fNofDigisTot;     ///< Total number of digis processed
    Double_t  fNofClustersTot;  ///< Total number of clusters produced
    Double_t  fTimeTot;         ///< Total execution time

    /** Set of active modules in the current event **/
    set<CbmStsModule*> fActiveModules;

    /** Map from module address to map of digis in channels **/
//    map<CbmStsModule*, map<Int_t, CbmStsDigi*> > fDigiMap;

    /** Sort digis into module digi maps
     ** @return Number of digis sorted
     **/
    Int_t SortDigis();


    /** Prevent usage of copy constructor and assignment operator **/
    CbmStsFindClusters(const CbmStsFindClusters&);
    CbmStsFindClusters operator=(const CbmStsFindClusters&);


    ClassDef(CbmStsFindClusters, 1);
};

#endif
