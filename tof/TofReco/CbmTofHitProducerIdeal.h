// -------------------------------------------------------------------------
// -----                CbmTofHitProducerIdeal header file             -----
// -----                  Created 07/03/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmTofHitProducerIdeal.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** The ideal hit producer produces hits of type CbmTofHit by copying
 ** the MCPoint position. The position error is set to 1 mum, much 
 ** smaller than can be obtained by any detector. Using the hits from 
 ** this HitProducer is thus equivalent to using MC information
 ** directly, but with the correct data interface.
 **/


#ifndef CBMTOFHITPRODUCERIDEAL_H
#define CBMTOFHITPRODUCERIDEAL_H 1


#include "FairTask.h"

class TClonesArray;



class CbmTofHitProducerIdeal : public FairTask
{

 public:

  /** Default constructor **/  
  CbmTofHitProducerIdeal();


  /** Destructor **/
  ~CbmTofHitProducerIdeal();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


 private:

  /** Input array of CbmTofPoints **/
  TClonesArray* fPointArray;

  /** Output array of CbmTofHits **/
  TClonesArray* fHitArray;  

 private:

  CbmTofHitProducerIdeal(const CbmTofHitProducerIdeal&);
  CbmTofHitProducerIdeal& operator=(const CbmTofHitProducerIdeal&);

  ClassDef(CbmTofHitProducerIdeal,1);

};

#endif
