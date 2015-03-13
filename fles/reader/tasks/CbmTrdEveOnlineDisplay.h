#ifndef CBMTRDEVEONLINEDISPLAY_H
#define CBMTRDEVEONLINEDISPLAY_H

#include "FairTask.h"

//#include "TGFileBrowser.h"


class TClonesArray;
class TCanvas;
class TH2;

class CbmTrdEveOnlineDisplay : public FairTask
{
  public:

    /** Default constructor **/
    CbmTrdEveOnlineDisplay();

    /** Destructor **/
    ~CbmTrdEveOnlineDisplay();


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

    TCanvas* fSpadic1; //!
    TCanvas* fSpadic1a; //!

    Int_t fUpdateInterval;
    Int_t fEventCounter;
               
    CbmTrdEveOnlineDisplay(const CbmTrdEveOnlineDisplay&);
    CbmTrdEveOnlineDisplay operator=(const CbmTrdEveOnlineDisplay&);

    ClassDef(CbmTrdEveOnlineDisplay,1);
};

#endif
