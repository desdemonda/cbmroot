// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef CBMMCTESTMOMENTUMCOMPARE_H
#define CBMMCTESTMOMENTUMCOMPARE_H


// framework includes
#include "FairTask.h"
#include "CbmMCMatch.h"
#include "CbmDetectorList.h"


#include <vector>
#include <map>

class TClonesArray;

class CbmMCTestMomentumCompare : public FairTask
{
 public:

  /** Default constructor **/
	CbmMCTestMomentumCompare();

  /** Destructor **/
  virtual ~CbmMCTestMomentumCompare();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();



 private:
  CbmMCMatch* fMCMatch;
  TClonesArray* fTrack;
  TClonesArray* fMCTrack;

  void Register();

  void Reset();

  void ProduceHits();


  ClassDef(CbmMCTestMomentumCompare,1);

};

#endif
