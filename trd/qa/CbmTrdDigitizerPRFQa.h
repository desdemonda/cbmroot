#ifndef CBMTRDDIGITIZERPRFQA_H
#define CBMTRDDIGITIZERPRFQA_H 1

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class CbmTrdDigitizerPRFQa : public FairTask
{
public:

    /* Defaul constructor */
    CbmTrdDigitizerPRFQa();

    /* Standard constructor */
    CbmTrdDigitizerPRFQa(const char* name,
        		const char* title = "FairTask");

    /* Destructor */
    virtual ~CbmTrdDigitizerPRFQa();

    /* Initialisation */
    InitStatus Init();

    /* Execution */
    virtual void Exec(Option_t* option);

    /* Finish at the end of each event */
    virtual void Finish();


private:

    /* Prepare the QA histograms */
    void PrepareHistograms();


    /* Write the QA histograms to file*/
  void WriteHistograms();
  
  CbmTrdDigitizerPRFQa(const CbmTrdDigitizerPRFQa&);
  CbmTrdDigitizerPRFQa& operator=(const CbmTrdDigitizerPRFQa&);

    ClassDef(CbmTrdDigitizerPRFQa, 1)
};

#endif
