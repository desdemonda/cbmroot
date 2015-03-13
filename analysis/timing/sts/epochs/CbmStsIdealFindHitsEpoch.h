#ifndef CbmStsIdealFindHitsEpoch_H
#define CbmStsIdealFindHitsEpoch_H 1


#include "FairTask.h"

#include "TStopwatch.h"

#include <map>
#include <set>


class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class CbmStsSector;
class CbmStsStation;



class CbmStsIdealFindHitsEpoch : public FairTask
{


 public :

  /** Default constructor **/
  CbmStsIdealFindHitsEpoch();


  /** Standard constructor **/
  CbmStsIdealFindHitsEpoch(Int_t iVerbose);


  /** Constructor with task name **/
  CbmStsIdealFindHitsEpoch(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsIdealFindHitsEpoch();


  /** Execution **/
  virtual void Exec(Option_t* opt);

  void SetSliceSeparationTime(Int_t time) {fSliceSeparationTime=time;} 


 private:

  CbmGeoStsPar*     fGeoPar;      /** Geometry parameters **/
  CbmStsDigiPar*    fDigiPar;     /** Digitisation parameters **/
  CbmStsDigiScheme* fDigiScheme;  /** Digitisation scheme **/
  TClonesArray*     fDigis;       /** Input array of CbmStsDigi **/
  TClonesArray*     fHits;        /** Output array of CbmStsHit **/
  std::map<CbmStsSector*, std::set<Int_t> > fDigiMapF;  /** sector digis (front) **/
  std::map<CbmStsSector*, std::set<Int_t> > fDigiMapB;  /** sector digis (back)  **/
  TStopwatch fTimer;

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Make sectorwise sets for sigis  **/
  void MakeSets();


  /** Sort digis sectorwise  **/
  void SortDigis();
  
//  virtual void Finish() { };


  /** Find hits in one sector **/
  Int_t FindHits(CbmStsStation* station, CbmStsSector* sector,
		 std::set<Int_t>& fSet, std::set<Int_t>& bSet);

  CbmStsIdealFindHitsEpoch(const CbmStsIdealFindHitsEpoch&);
  CbmStsIdealFindHitsEpoch operator=(const CbmStsIdealFindHitsEpoch&);

  
  ClassDef(CbmStsIdealFindHitsEpoch,1);
  
  Int_t    fNStations;
  Int_t    fNEvents;        /** Number of events with success **/
  Double_t  fTime1;         /** Total real time used for good events **/
   
  Int_t fSliceSeparationTime;
};

#endif
