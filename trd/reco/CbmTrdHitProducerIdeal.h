/**
 * \file CbmTrdHitProducerIdeal.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 06.05.2013
 * \brief Ideal hit producer for TRD.
 */

#ifndef CBMTRDHITPRODUCERIDEAL_H
#define CBMTRDHITPRODUCERIDEAL_H

#include "FairTask.h"

class TClonesArray;

class CbmTrdHitProducerIdeal : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
    CbmTrdHitProducerIdeal();

    /**
     * \brief Destructor.
     */
    virtual ~CbmTrdHitProducerIdeal();

    /**
     * \brief Inherited form FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Exec(Option_t* option = "");

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Finish();

private:
    TClonesArray* fTrdPoints; // Array of TRD points
    TClonesArray* fTrdHits; // Array of TRD hits

    CbmTrdHitProducerIdeal(const CbmTrdHitProducerIdeal&);
    CbmTrdHitProducerIdeal& operator=(const CbmTrdHitProducerIdeal&);

    ClassDef(CbmTrdHitProducerIdeal, 2);
};

#endif

