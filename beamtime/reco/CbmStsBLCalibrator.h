#ifndef CBMSTSBLCALIBRATOR_H
#define CBMSTSBLCALIBRATOR_H

#include "FairTask.h"

#include "TH1F.h"
#include <vector>
using std::vector;


class TClonesArray;

class CbmStsBLCalibrator : public FairTask
{
  public:

    /** Default constructor **/
    CbmStsBLCalibrator();

    /** Destructor **/
    ~CbmStsBLCalibrator();

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

    static const Int_t kBaselineMinAdc = 0;
    static const Int_t kBaselineMaxAdc = 4096;
    static const Int_t kBaselineNBins = 4096;

    vector< vector < vector < TH1F * > > > fBaselines;

    static Double_t GetBlPos( TH1F * hist );

    CbmStsBLCalibrator(const CbmStsBLCalibrator&);
    CbmStsBLCalibrator operator=(const CbmStsBLCalibrator&);

    ClassDef(CbmStsBLCalibrator,1);
};

#endif
