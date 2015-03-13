// -------------------------------------------------------------------------
// -----                 CbmPsdReactionPlaneMaker.h header file             -----
// -----                  Created 15/05/12  by Marina & Alla                   -----
// -------------------------------------------------------------------------


/** CbmPsdReactionPlaneMaker.h
 *@authors Alla  <alla@inr.ru> Marina <marina@inr.ru>
 **
 **/


#ifndef CBMPSDReactionPlaneMaker_H
#define CBMPSDReactionPlaneMaker_H 1


#include "FairTask.h"
#include "CbmPsdEventData.h"

class TClonesArray;



class CbmPsdReactionPlaneMaker : public FairTask
{

 public:

  /** Default constructor **/  
  CbmPsdReactionPlaneMaker();


  /** Destructor **/
  ~CbmPsdReactionPlaneMaker();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);




 private:
  Int_t             fNHits;
  Float_t fMean;
  Float_t fXi[44];
  Float_t fYi[44];
   
  /** Input array of CbmPsdHit **/
  TClonesArray* fHitArray;
  /** Output object CbmPsdEventData **/
  CbmPsdEventData* fCbmPsdEvent;

  CbmPsdReactionPlaneMaker(const CbmPsdReactionPlaneMaker&);
  CbmPsdReactionPlaneMaker operator=(const CbmPsdReactionPlaneMaker&);

  void Reset();

  ClassDef(CbmPsdReactionPlaneMaker,1);

};

#endif
