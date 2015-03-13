#ifndef CBMTRDONLINEDISPLAY_H
#define CBMTRDONLINEDISPLAY_H

#include "FairTask.h"

class TClonesArray;
class TCanvas;
class TH2;

class CbmTrdOnlineDisplay : public FairTask
{
  public:

    /** Default constructor **/
    CbmTrdOnlineDisplay();

    /** Destructor **/
    ~CbmTrdOnlineDisplay();


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

    void SetUpdateInterval(Int_t val) { fUpdateInterval = val; }

  private:

    TCanvas* fSpadic1[3][3]; //!
    TCanvas* fSpadic1a[3][3]; //!

    Int_t fUpdateInterval;
    Int_t fEventCounter;
               
    CbmTrdOnlineDisplay(const CbmTrdOnlineDisplay&);
    CbmTrdOnlineDisplay operator=(const CbmTrdOnlineDisplay&);

    ClassDef(CbmTrdOnlineDisplay,1);
};

#endif
