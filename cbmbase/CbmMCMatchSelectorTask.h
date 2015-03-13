// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef CBMMCMATCHSELECTORTASK_H
#define CBMMCMATCHSELECTORTASK_H


// framework includes
#include "FairTask.h"
//#include "CbmMCMatch.h"
#include "CbmDetectorList.h"


#include <vector>
#include <list>

class CbmMCMatch;
class TClonesArray;

class CbmMCMatchSelectorTask : public FairTask
{
 public:

  /** Default constructor **/
	CbmMCMatchSelectorTask();

	CbmMCMatchSelectorTask(Cbm::DataType start, Cbm::DataType stop);

  /** Destructor **/
  virtual ~CbmMCMatchSelectorTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();

  virtual void SetStart(Cbm::DataType type){fStart = type;}
  virtual void SetStop(Cbm::DataType type){fStop = type;}

  virtual void SetAllWeights(Float_t weight) {fCommonWeight = weight;}
  virtual void SetWeightStage(Int_t type, Float_t weight){
	  fStageWeights.push_back(std::pair<Cbm::DataType, Float_t>(static_cast<Cbm::DataType>(type), weight));
  }

  virtual void SetWeights();



 private:
  CbmMCMatch* fMCMatch;
  Cbm::DataType fStart;
  Cbm::DataType fStop;

  std::vector<std::pair<Cbm::DataType, Float_t> > fStageWeights;
  Float_t fCommonWeight;

  void Register();

  void Reset();

  void ProduceHits();

  CbmMCMatchSelectorTask(const CbmMCMatchSelectorTask&);
  CbmMCMatchSelectorTask& operator=(const CbmMCMatchSelectorTask&);

  ClassDef(CbmMCMatchSelectorTask,1);

};

#endif
