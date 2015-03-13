#ifndef CBMFIBERHODOBLCALIBRATOR_H
#define CBMFIBERHODOBLCALIBRATOR_H

#include "FairTask.h"

#include "TH1F.h"
#include <vector>
using std::vector;

class TClonesArray;

class CbmFiberHodoBLCalibrator : public FairTask
{
  public:

    /** Default constructor **/
    CbmFiberHodoBLCalibrator();

    /** Destructor **/
    ~CbmFiberHodoBLCalibrator();

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
    TClonesArray* fBaselineDigis;

    vector< vector < vector < TH1F * > > > fBaselines;

    static Double_t GetBlPos( TH1F * hist );

    CbmFiberHodoBLCalibrator(const CbmFiberHodoBLCalibrator&);
    CbmFiberHodoBLCalibrator operator=(const CbmFiberHodoBLCalibrator&);

    ClassDef(CbmFiberHodoBLCalibrator,1);
};

#endif
