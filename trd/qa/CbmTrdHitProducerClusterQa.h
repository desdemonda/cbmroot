#ifndef CBMTRDHITPRODUCERCLUSTERQA_H
#define CBMTRDHITPRODUCERCLUSTERQA_H 1

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class CbmTrdHitProducerClusterQa : public FairTask
{
public:

    /* Defaul constructor */
    CbmTrdHitProducerClusterQa();

    /* Standard constructor */
    CbmTrdHitProducerClusterQa(const char* name,
        		const char* title = "FairTask");

    /* Destructor */
    virtual ~CbmTrdHitProducerClusterQa();

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
  
  CbmTrdHitProducerClusterQa(const CbmTrdHitProducerClusterQa&);
  CbmTrdHitProducerClusterQa& operator=(const CbmTrdHitProducerClusterQa&);

    ClassDef(CbmTrdHitProducerClusterQa, 1)
};

#endif
