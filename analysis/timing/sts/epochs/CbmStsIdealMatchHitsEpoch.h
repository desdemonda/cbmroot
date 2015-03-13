
#ifndef CBMSTSIDEALMATCHHITEPOCH_H
#define CBMSTSIDEALMATCHHITEPOCH_H 1

#include "FairTask.h"

#include "TStopwatch.h"

#include <map>

class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class CbmStsDigiMatch;
class CbmStsHit;
class CbmMCEpoch;


class CbmStsIdealMatchHitsEpoch : public FairTask 
{

 public:

  /** Default constructor **/
  CbmStsIdealMatchHitsEpoch();


  /** Standard constructor **/
  CbmStsIdealMatchHitsEpoch(Int_t iVerbose);


  /** Constructor with name **/
  CbmStsIdealMatchHitsEpoch(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsIdealMatchHitsEpoch();


  /** Execution **/
  virtual void Exec(Option_t* opt);



 private:

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Run summary **/
  virtual void Finish();


  // -----  Data members ----------
  CbmGeoStsPar*     fGeoPar;      /** Geometry parameter container **/
  CbmStsDigiPar*    fDigiPar;     /** Digitisation parameter container **/
  CbmStsDigiScheme* fDigiScheme;  /** Digitisation scheme **/
  TClonesArray*     fPoints;      /** Array of CbmStsPoint **/
  TClonesArray*     fDigis;       /** Array of CbmStsDigi **/
  TClonesArray*     fDigiMatches; /** Array of CbmStsDigiMatch **/
  TClonesArray*     fHits;        /** Array of CbmStsHit **/
  CbmMCEpoch*       fMcEpoch;

  TStopwatch        fTimer;       /** Timer **/
  Int_t    fNEvents;        /** Number of events with success **/
  Int_t    fNEventsFailed;  /** Number of events with failure **/
  Double_t fTime;           /** Total real time used for good events **/
  Double_t fNHits;          /** Total number of hits **/
  Double_t fNMatched;       /** Total number of matched hits **/
  Double_t fNDistant;       /** Total number of displaced hits **/
  Double_t fNBackgrd;       /** Total number of background hits **/

  /** Map from candidate point to distance to hit **/
  std::map<Double_t, Int_t> fCandMap;            //!
  std::map<Double_t, Int_t>::iterator fIter;     //!

  CbmStsIdealMatchHitsEpoch(const CbmStsIdealMatchHitsEpoch&);
  CbmStsIdealMatchHitsEpoch operator=(const CbmStsIdealMatchHitsEpoch&);

  ClassDef(CbmStsIdealMatchHitsEpoch,1);

};

#endif

