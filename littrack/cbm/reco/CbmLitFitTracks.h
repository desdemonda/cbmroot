/**
 * \file CbmLitFitTracks.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 * \brief Task for testing of standard track fit algorithms.
 */

#ifndef CBMLITFITTRACKS_H_
#define CBMLITFITTRACKS_H_

#include "FairTask.h"
#include "TStopwatch.h"

class CbmLitFitTracks : public FairTask {
public:
   /**
    * \brief Constructor.
    */
   CbmLitFitTracks();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFitTracks();

   /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(
       Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

private:
    /**
     * \brief Read data branches.
     */
    void ReadDataBranches();

    /**
     * \brief Performs track fit.
     */
    void DoFit();

    /**
     * \brief Print stopwatch statistics.
     */
    void PrintStopwatchStatistics();

    // Data branches
    TClonesArray* fGlobalTracks;
    TClonesArray* fStsTracks;
    TClonesArray* fTrdTracks;
    TClonesArray* fMuchTracks;
    TClonesArray* fTrdHits;
    TClonesArray* fMuchPixelHits;
    TClonesArray* fMuchStrawHits;

    // stopwatches
    TStopwatch fFitWatch; // stopwatch for track fit without IO
    TStopwatch fFitWithIOWatch; // stopwatch for track fit including IO

    CbmLitFitTracks(const CbmLitFitTracks&);
    CbmLitFitTracks& operator=(const CbmLitFitTracks&);
    
    ClassDef(CbmLitFitTracks, 1);
};

#endif /* CBMLITFITTRACKSPARALLEL_H_ */
