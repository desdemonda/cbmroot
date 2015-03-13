#ifndef CBMHODORAWHISTOS_H
#define CBMHODORAWHISTOS_H

#include "CbmHistManager.h"

#include "FairTask.h"

class TClonesArray;
class TH2F;

class CbmHodoRawHistos : public FairTask
{
  public:

    /** Default constructor **/
    CbmHodoRawHistos();

    /** Destructor **/
    ~CbmHodoRawHistos();


    /** Initiliazation of task at the beginning of a run **/
    virtual InitStatus Init();

    /** ReInitiliazation of task when the runID changes **/
    virtual InitStatus ReInit();


    /** Executed for each event. **/
    virtual void Exec(Option_t* opt);

    /** Load the parameter container from the runtime database **/
    virtual void SetParContainers();

    /** Finish task called at the end of the run **/
    virtual void Finish();

  private:

    /** Input array from previous already existing data level **/
    //  
    TClonesArray* fHodoDigi;

    CbmHistManager* fHM;

    void CreateHistograms();

    CbmHodoRawHistos(const CbmHodoRawHistos&);
    CbmHodoRawHistos operator=(const CbmHodoRawHistos&);

    ClassDef(CbmHodoRawHistos,1);
};

#endif
