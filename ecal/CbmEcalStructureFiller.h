/** Creates a CbmEcalStructure and fill it with Hits/SHits/MCPoints **/

/**  CbmEcalStructureFiller.h
 *@author Yu.Kharlov
 **
 ** Class for producing either ECAL hits
 ** or ECAL summable hits from MCPoints,
 **  Definitions:
 ** ECAL hit is total deposited energy by all track in ECAL cell
 ** ECAL summable hit is a deposited energy by each tracks in ECAL cell
 **  Use cases:
 **
 ** 1. To produce hits from MC points:
 ** CbmEcalStructureFiller* hp=new CbmEcalStructureFiller("EcalHitProducer");
 ** hp->SetMCPoints2Hits();
 ** fRun->AddTask(hp);
 **
 ** 2. To produce summable hits from MC points:
 ** CbmEcalStructureFiller* hp=new CbmEcalStructureFiller("EcalHitProducer");
 ** hp->SetMCPoints2SumHits();
 ** fRun->AddTask(hp);
 **
 ** 3. To produce hits from summable hits:
 ** CbmEcalStructureFiller* hp=new CbmEcalStructureFiller("EcalHitProducer");
 ** hp->SetSumHits2Hits();
 ** fRun->AddTask(hp);
 **/

#ifndef CBMECALSTRUCTUREFILLER_H
#define CBMECALSTRUCTUREFILLER_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "CbmEcalHit.h"
#include "CbmStack.h"
#include "CbmEcalInf.h"
#include "CbmEcalStructure.h"
#include "TString.h"
#include "TVector3.h"

class CbmEcalStructureFiller : public FairTask {

public:
  /** Default constructor **/
  CbmEcalStructureFiller();

  /** Standard constructor **/
  CbmEcalStructureFiller(const char *name, const Int_t iVerbose=1, const char* fileGeo="ecal_FullMC.geo");

  /** Destructor **/
  virtual ~CbmEcalStructureFiller();

  /** Initialization of the task **/  
  virtual InitStatus Init();

  /** Executed task **/ 
  virtual void Exec(Option_t* option);

  /** Finish task **/ 
  virtual void Finish();

  /** Set data source for hit producer.
   ** This must be called before Init()
   ** (No effect other case)! **/
  void SetUseMCPoints(Bool_t UseMCPoints);
  void SetUseSummableHits(Bool_t UseSummableHits);
  /** Can be set kTRUE only if ProduceSummableHits false **/
  void SetUseHits(Bool_t UseHits);

  CbmEcalStructure* GetStructure() const;
  void StoreTrackInformation(Bool_t storetrackinfo=kTRUE);
  Bool_t GetStoreTrackInformation() const;

  Bool_t GetUseMCPoints() const;
  Bool_t GetUseSummableHits() const;
  Bool_t GetUseHits() const;
protected:

private:
  /** Init parameter containers **/
  void SetParContainers();
  /** Loop over MCPoints **/
  void LoopForMCPoints();
  /** Loop over summable hits **/
  void LoopForSummableHits();
  /** Loop over hits **/
  void LoopForUnSummableHits();
  CbmEcalStructure* fStr;
  CbmEcalInf* fInf;		//ECAL geometry container

  TClonesArray* fListECALpts;   // ECAL MC points
  TClonesArray* fListUHits;	// ECAL hits
  TClonesArray* fListHits;	// ECAL summable hits
//  TClonesArray* fListStack;     // Tracks
  Int_t fEvent;                 //! Internal event counter

  /** Is Init() already done? **/
  Bool_t fInited;
  /** Should we take data from MCPoints? **/
  Bool_t fUseMCPoints;
  /** Should we take data from summable hits? **/
  Bool_t fUseSummableHits;
  /** Should we take data from hits? **/
  Bool_t fUseUnSummableHits;

  /** Should we store information about tracks/energy depostion **/
  Bool_t fStoreTrackInfo;
  /** Geo file to use **/
  TString fFileGeo;

  CbmEcalStructureFiller(const CbmEcalStructureFiller&);
  CbmEcalStructureFiller& operator=(const CbmEcalStructureFiller&);

  ClassDef(CbmEcalStructureFiller,1)

};

inline void CbmEcalStructureFiller::SetUseMCPoints(Bool_t UseMCPoints)
{
	if (fInited) return;
	fUseMCPoints=UseMCPoints;
}

inline void CbmEcalStructureFiller::SetUseSummableHits(Bool_t UseSummableHits)
{
	if (fInited) return;
	fUseSummableHits=UseSummableHits;
}

/** Can be set only if ProduceSummableHits false **/
inline void CbmEcalStructureFiller::SetUseHits(Bool_t UseHits)
{
	if (fInited) return;
	fUseUnSummableHits=UseHits;
}

inline Bool_t CbmEcalStructureFiller::GetUseMCPoints() const
{
	return fUseMCPoints;
}

inline Bool_t CbmEcalStructureFiller::GetUseSummableHits() const
{
	return fUseSummableHits;
}

inline Bool_t CbmEcalStructureFiller::GetUseHits() const
{
	return fUseUnSummableHits;
}


inline void CbmEcalStructureFiller::StoreTrackInformation(Bool_t storetrackinfo)
{
  if (fInited) return;
  fStoreTrackInfo=storetrackinfo;
}

inline Bool_t CbmEcalStructureFiller::GetStoreTrackInformation() const
{
  return fStoreTrackInfo;
}

inline CbmEcalStructure* CbmEcalStructureFiller::GetStructure() const
{
  return fStr;
}

#endif
