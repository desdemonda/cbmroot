// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef CBMMCTESTHITCOMPARE_H
#define CBMMCTESTHITCOMPARE_H


// framework includes
#include "FairTask.h"

class CbmMCMatch;

class TClonesArray;
class TH2F;
class TList;

class CbmMCTestHitCompare : public FairTask
{
 public:

  /** Default constructor **/
	CbmMCTestHitCompare();

  /** Destructor **/
  virtual ~CbmMCTestHitCompare();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Create histograms **/
  void CreateHistos();

  /** Reset histograms and counters **/
  void Reset();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();



 private:
  CbmMCMatch* fMCMatch;
  TClonesArray* fStripHit;
  TClonesArray* fMCPoint;
  TH2F* fhHitPointCorrelation;
  TH2F* fhHitPointSCorrelationSmall;
  TH2F* fhHitPointSCorrelationLarge;
  TList* fHistoList;
  void Register();

  void ProduceHits();

  CbmMCTestHitCompare(const CbmMCTestHitCompare&);
  CbmMCTestHitCompare& operator=(const CbmMCTestHitCompare&);

  ClassDef(CbmMCTestHitCompare,1);

};

#endif
