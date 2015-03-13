#ifndef CBMSTSHODOCORRELATIONS_H
#define CBMSTSHODOCORRELATIONS_H

#include "FairTask.h"

class TClonesArray;
class TH2F;

class CbmStsHodoCorrelations : public FairTask
{
  public:

    /** Default constructor **/
    CbmStsHodoCorrelations();

    /** Destructor **/
    ~CbmStsHodoCorrelations();


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
    TClonesArray* fStsDigi;

    /** Output array to  new data level**/
    TClonesArray* fHodoCluster;

    TH2F* hodo1_pos_sts0;
    TH2F* hodo2_pos_sts0;
    TH2F* hodo1_pos_sts1;
    TH2F* hodo2_pos_sts1;
    TH2F* hodo1_pos_sts2;
    TH2F* hodo2_pos_sts2;
    
    CbmStsHodoCorrelations(const CbmStsHodoCorrelations&);
    CbmStsHodoCorrelations operator=(const CbmStsHodoCorrelations&);

    ClassDef(CbmStsHodoCorrelations,1);
};

#endif
