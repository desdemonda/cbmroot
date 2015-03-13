#ifndef CBMTRDRAWBEAMPROFILE_H
#define CBMTRDRAWBEAMPROFILE_H

#include "FairTask.h"

#include "CbmHistManager.h"

#include "TClonesArray.h"

class CbmTrdRawBeamProfile : public FairTask
{
  public:

    /** Default constructor **/
    CbmTrdRawBeamProfile();

    /** Constructor with parameters (Optional) **/
    //  CbmTrdRawBeamProfile(Int_t verbose);


    /** Destructor **/
    ~CbmTrdRawBeamProfile();


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
    TClonesArray* fRawSpadic;

    CbmHistManager* fHM;

    /** Output array to  new data level**/
    //  TClonesArray* <OutputDataLevel>;

    void CreateHistograms();

    CbmTrdRawBeamProfile(const CbmTrdRawBeamProfile&);
    CbmTrdRawBeamProfile operator=(const CbmTrdRawBeamProfile&);

    ClassDef(CbmTrdRawBeamProfile,1);
};

#endif
