#ifndef CbmStsIdealDigitizeEpoch_H
#define CbmStsIdealDigitizeEpoch_H 1

#include "FairTask.h"

#include "TStopwatch.h"

#include <list>
#include <map>

class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class CbmStsStation;
class CbmMCEpoch;


class CbmStsIdealDigitizeEpoch : public FairTask
{

 public:

  /** Default constructor **/
  CbmStsIdealDigitizeEpoch();


  /** Standard constructor **/
  CbmStsIdealDigitizeEpoch(Int_t iVerbose);


  /** Constructor with name **/
  CbmStsIdealDigitizeEpoch(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsIdealDigitizeEpoch();


  /** Execution **/
  virtual void Exec(Option_t* opt);

  // epoch
  void SetEpoch(Bool_t epoch) {fEpoch=epoch;}
  void SetDeadTime(Double_t deadTime) {fDeadTime = deadTime; }
  void SetTimeSmearing(Double_t dtime)  {fDtime = dtime; }
 private:
  // epochs
  Bool_t            fEpoch; 
  CbmMCEpoch*       fMcEpoch;
  Double_t          fDtime;
  Double_t          fDeadTime;
  
  CbmGeoStsPar*     fGeoPar;       /** Geometry parameter container **/
  CbmStsDigiPar*    fDigiPar;      /** Digitisation parameter container **/
  CbmStsDigiScheme* fDigiScheme;   /** Digitisation scheme **/
  TClonesArray*     fPoints;       /** Input array of CbmStsPoint **/
  TClonesArray*     fDigis;        /** Output array of CbmStsDigi **/
  TClonesArray*     fDigiMatches;  /** Output array of CbmStsDigiMatches**/
  Int_t             fNPoints;     
  Int_t             fNFailed;
  Int_t             fNOutside;
  Int_t             fNMulti;
  Int_t             fNDigis;
  TStopwatch        fTimer;

  /** Map of active channels (pair detectorId, channel number) 
   ** to index of StsDigi **/
  std::map<std::pair<Int_t, Int_t>, Int_t> fChannelMap;     //!

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Reset eventwise counters **/
  void Reset();

  CbmStsIdealDigitizeEpoch(const CbmStsIdealDigitizeEpoch&);
  CbmStsIdealDigitizeEpoch operator=(const CbmStsIdealDigitizeEpoch&);


  ClassDef(CbmStsIdealDigitizeEpoch,1);

};

#endif


