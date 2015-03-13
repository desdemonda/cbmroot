/** CbmAnaTimingStsHitFinder.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-07-07
 **
 **/

#ifndef CbmAnaTimingStsHitFinder_H_
#define CbmAnaTimingStsHitFinder_H_ 1

#include "FairTask.h"
#include "TString.h"
class TH1D;
class TClonesArray;
class CbmMCEpoch;

class CbmAnaTimingStsHitFinder : public FairTask{
public:
  CbmAnaTimingStsHitFinder();
  CbmAnaTimingStsHitFinder(const char* name,TString histoName);
  virtual ~CbmAnaTimingStsHitFinder();
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  virtual void SetParContainers();

private:
  Int_t         fEvent;         //!
  TString       fHistoName;
  TClonesArray* fPoints;        //!
  TClonesArray* fDigis;         //!
  TClonesArray* fDigiMatches;   //!
  TClonesArray* fHits;          //!
  CbmMCEpoch*   fMcEpoch;       //!
  TH1D* fHitTimeAll;
  TH1D* fHitTimeCorrected;

  ClassDef(CbmAnaTimingStsHitFinder,1);
};

#endif
