/** CbmRichParallelQa.h
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **/

#ifndef CBMRICHPARALLELQA_H_
#define CBMRICHPARALLELQA_H_

#include "CbmRichRing.h"
#include "FairTask.h"
#include "CbmRichRingFinderHough.h"
#include "../../L1/OffLineInterface/CbmL1RichENNRingFinder.h"

#include <vector>

class CbmRichParallelQa : public FairTask
{
    TClonesArray* fRichRings; // Array of CbmRichRings
    TClonesArray* fRichHits; // Array of CbmRichHits

    Int_t fEventNumber;
    Double_t fExecTime;
    static const int kMAX_NOF_THREADS = 80;
    int fNofTasks;
    int fNofEvents;

    //CbmRichRingFinderHough* fHT[kMAX_NOF_THREADS];
    CbmL1RichENNRingFinder* fHT[kMAX_NOF_THREADS];
    std::vector<std::vector<CbmRichHoughHit> > fData;

public:

    CbmRichParallelQa();

    virtual ~CbmRichParallelQa();

    virtual InitStatus Init();

    virtual void Exec(
          Option_t* option);

    void DoTestWithTask();

    void SetNofTasks(int n){fNofTasks = n;}

    void SetNofEvents(int n){fNofEvents = n;}

    /**
     * \brief Copy constructor.
     */
    CbmRichParallelQa(const CbmRichParallelQa&);

    /**
     * \brief Assignment operator.
     */
    CbmRichParallelQa& operator=(const CbmRichParallelQa&);

    ClassDef(CbmRichParallelQa,1)
};

#endif
