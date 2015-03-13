/** CbmAnaTimingStsTracking.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-07-11
 **
 **/

#ifndef CbmAnaTimingStsTracking_H_
#define CbmAnaTimingStsTracking_H_ 1

#include "FairTask.h"
#include "TString.h"
class TH1D;
class TClonesArray;
class CbmMCEpoch;

class CbmAnaTimingStsTracking : public FairTask{
public:
  CbmAnaTimingStsTracking();
  CbmAnaTimingStsTracking(const char* name,TString histoName);
  virtual ~CbmAnaTimingStsTracking();
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
  TClonesArray* fStsTracks;     //!
  TH1D* fTrackTime;
  TH1D* fTrackTimeFromHit;
  TH1D* fTrackTimeFromLastHit;
  ClassDef(CbmAnaTimingStsTracking,1);
};

#endif
