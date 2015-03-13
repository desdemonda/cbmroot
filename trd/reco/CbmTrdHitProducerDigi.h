/**
 * \file CbmTrdHitProducerDigi.h
 * \author F. Uhlig <f.uhlig@gsi.de>
 * \date 04.03.2009
 * Updated 06.05.2013 by A. Lebedev <andrey.lebedev@gsi.de>
 **/

#ifndef CBMTRDHITPRODUCERDIGI_H
#define CBMTRDHITPRODUCERDIGI_H

#include "FairTask.h"

class CbmTrdDigiPar;
class CbmTrdModule;
class TClonesArray;

class CbmTrdHitProducerDigi : public FairTask
{
public:

   /**
    * \brief Default constructor.
    **/
   CbmTrdHitProducerDigi();

   /**
    * \brief Destructor.
    **/
   virtual ~CbmTrdHitProducerDigi();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void SetParContainers();

   /**
    * \brief Inherited from FairTask.
    **/
   virtual void Exec(Option_t * option);

   /**
    * \brief Inherited form FairTask.
    **/
   virtual void Finish();

private:

   CbmTrdHitProducerDigi& operator=(const CbmTrdHitProducerDigi&);
   CbmTrdHitProducerDigi(const CbmTrdHitProducerDigi&);

   TClonesArray* fTrdDigis; //! TRD digi array
   TClonesArray* fTrdClusters; //! TRD cluster array
   TClonesArray* fTrdHits; //! TRD hits array

   CbmTrdDigiPar* fDigiPar; //!
   CbmTrdModule* fModuleInfo; //!

   ClassDef(CbmTrdHitProducerDigi, 4);
};
#endif // CBMTRDHITPRODUCERDIGI_H
