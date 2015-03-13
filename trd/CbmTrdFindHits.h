/** CbmTrdFindHits.h
 **
 **/


#ifndef CBMTRDFINDHITS_H
#define CBMTRDFINDHITS_H

#include "FairTask.h"

//class TClonesArray;

class CbmTrdFindHits : public FairTask
{
 public:

    /** Default constructor **/
    CbmTrdFindHits();

    /** Constructor **/
    CbmTrdFindHits(const char *name, const char *title="CBM Task", Int_t iVerbose=1);

    /** Destructor **/
    virtual ~CbmTrdFindHits();


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);


 private:


  ClassDef(CbmTrdFindHits,1);

};

#endif /* CBMTRDFINDHITS_H */
