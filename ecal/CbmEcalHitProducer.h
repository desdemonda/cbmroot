// -------------------------------------------------------------------------
// -----                  CbmEcalHitProducer header file               -----
// -----                  Created 10/03/05  by Yu.Kharlov              -----
// -------------------------------------------------------------------------


/* $Id: CbmEcalHitProducer.h,v 1.8 2006/07/24 21:45:02 prokudin Exp $ */

/**  CbmEcalHitProducer.h
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
 ** CbmEcalHitProducer* hp=new CbmEcalHitProducer("EcalHitProducer");
 ** hp->SetMCPoints2Hits();
 ** fRun->AddTask(hp);
 **
 ** 2. To produce summable hits from MC points:
 ** CbmEcalHitProducer* hp=new CbmEcalHitProducer("EcalHitProducer");
 ** hp->SetMCPoints2SumHits();
 ** fRun->AddTask(hp);
 **
 ** 3. To produce hits from summable hits:
 ** CbmEcalHitProducer* hp=new CbmEcalHitProducer("EcalHitProducer");
 ** hp->SetSumHits2Hits();
 ** fRun->AddTask(hp);
 **/

/* History of cvs commits:
 *
 * $Log: CbmEcalHitProducer.h,v $
 * Revision 1.8  2006/07/24 21:45:02  prokudin
 * Now we write information to parameter file
 *
 * Revision 1.7  2006/07/19 12:31:54  prokudin
 * Commenting
 *
 * Revision 1.6  2006/07/19 11:49:39  prokudin
 * Commenting, optimizing hitproducer
 *
 * Revision 1.5  2006/07/18 20:38:36  prokudin
 * MCPointsToUHits() Added. Corrected for fVerbose
 *
 * Revision 1.4  2006/07/13 14:42:45  prokudin
 * New version of hit producer
 *
 * Revision 1.3  2006/04/25 06:52:18  kharlov
 * Remove CbmEcalv[1,2] and leave the only class CbmEcal
 *
 */

#ifndef CBMECALHITPRODUCER_H
#define CBMECALHITPRODUCER_H

#include "CbmEcalHit.h"
#include "CbmEcalInf.h"
#include "CbmEcalStructure.h"

#include "FairTask.h"
#include "CbmStack.h"

#include "TClonesArray.h"
#include "TString.h"
#include "TVector3.h"

class CbmEcalHitProducer : public FairTask {

public:

  /** Default constructor **/
  CbmEcalHitProducer();

  /** Standard constructor **/
  CbmEcalHitProducer(const char *name, const Int_t iVerbose=1, const char* fileGeo="ecal_FastMC.geo");

  /** Destructor **/
  virtual ~CbmEcalHitProducer();

  /** Initialization of the task **/  
  virtual InitStatus Init();

  /** Executed task **/ 
  virtual void Exec(Option_t* option);

  /** Finish task **/ 
  virtual void Finish();

  /** Process MCPoints to summable hits (hits per individual track)
   ** MCPoints -> Summable hits **/
  void SetMCPoints2SumHits();
  /** Process summable hits to hits
   ** Summable sits -> Hits **/
  void SetSumHits2Hits();
  /** Process MCPoints directly to hits (hits for all tracks)
   ** MCPoints -> Hits **/
  void SetMCPoints2Hits();

  /** Set noise for ECAL (GeV) **/
  void SetNoise(Float_t noise);
  /** Set noise for PS (GeV)**/
  void SetPSNoise(Float_t noiseps);

  Float_t GetNoise() const;
  Float_t GetPSNoise() const;

  /** Set threshold for ECAL (GeV)**/
  void SetThreshold(Float_t threshold);
  /** Set threshold for PS (GeV)**/
  void SetPSThreshold(Float_t thresholdps);

  Float_t GetThreshold() const;
  Float_t GetPSThreshold() const;

  /** Sets type of output hits. If UseHits is set, only
   ** unsummable hits can be produced
   ** kTRUE --- produce summable hits
   ** kFALSE --- produce unsummable hits **/
  void SetProduceSummableHits(Bool_t ProduceSummableHits);

  /** method AddHit
   **
   ** Adds a EcalHit to the HitCollection
   **/
  //Using x and y is deprecated! Will be removed in time!
  void AddHit(Int_t cellnumber, Float_t energy, Float_t psenergy, Int_t trackID, Double32_t time=0);

  /** Set data source for hit producer.
   ** This must be called before Init()
   ** (No effect other case)! **/
  void SetUseMCPoints(Bool_t UseMCPoints);
  void SetUseSummableHits(Bool_t UseSummableHits);
  /** Can be set kTRUE only if ProduceSummableHits false **/
  void SetUseHits(Bool_t UseHits);

  Bool_t GetUseMCPoints() const;
  Bool_t GetUseSummableHits() const;
  Bool_t GetUseHits() const;
 
  Bool_t GetProduceSummableHits() const;

  /** Get/set stand alone status **/
  inline Bool_t StandAlone() const {return fStandAlone;}
  inline void SetStandAlone(Bool_t alone) {fStandAlone=alone;} 

  /** Get/set digitizer bin sizes **/
  inline Float_t GetEcalBinSize() const {return fEcalBinSize;}
  inline void SetEcalBinSize(Float_t binsize) {fEcalBinSize=binsize;}

  inline Float_t GetPSBinSize() const {return fPSBinSize;}
  inline void SetPSBinSize(Float_t binsize) {fPSBinSize=binsize;}

  inline Bool_t ProduceHits() const {return fProduceHits;}
  inline void SetProduceHits(Bool_t produce) {fProduceHits=produce;} 
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
  /** Gets hits from cell **/
  void CellToHits(CbmEcalCell* cell);
  CbmEcalStructure* fStr;
  CbmEcalInf* fInf;		//ECAL geometry container

  TClonesArray* fListECALpts;   // ECAL MC points
  TClonesArray* fListUHits;	// ECAL hits
  TClonesArray* fListHits;	// ECAL summable hits
  TClonesArray* fHitCollection;	// ECAL output hits
  TClonesArray* fListStack;     // Tracks
  Int_t fNHits;                 // Number of hits
  Int_t fEvent;                 //! Internal event counter

  /** Is Init() already done? **/
  Bool_t fInited;
  /** Should we take data from MCPoints? **/
  Bool_t fUseMCPoints;
  /** Should we take data from summable hits? **/
  Bool_t fUseSummableHits;
  /** Should we take data from hits? **/
  Bool_t fUseUnSummableHits;

  /** Threshold for ECAL **/
  Float_t fThreshold;
  /** Threshold for PS **/
  Float_t fThresholdPS;
  /** Noise in ECAL **/
  Float_t fNoise;
  /** Noise in PS **/
  Float_t fNoisePS;

  /** Geo file to use **/
  TString fFileGeo;
  /** Should we produce summable hits?**/
  Bool_t fProduceSummableHits;

  /** Hit producer works in stand alone mode? **/
  Bool_t fStandAlone;
  /** Digitization bin size for calorimter and PS **/
  Float_t fEcalBinSize;
  Float_t fPSBinSize;
  /** Should we produce hits or just modify calorimter
   ** structure content **/
  Bool_t fProduceHits;

  CbmEcalHitProducer(const CbmEcalHitProducer&);
  CbmEcalHitProducer& operator=(const CbmEcalHitProducer&);

  ClassDef(CbmEcalHitProducer,1)

};

/** Process MCPoints directly to hits (hits for all tracks)
 ** MCPoints -> Hits **/
inline void CbmEcalHitProducer::SetMCPoints2Hits()
{
	if (fInited) return;
	fUseMCPoints=kTRUE;
	fUseSummableHits=kFALSE;
	fUseUnSummableHits=kFALSE;
	fProduceSummableHits=kFALSE;
}

/** Process MCPoints to summable hits (hits per individual track)
 ** MCPoints -> Summable hits **/
inline void CbmEcalHitProducer::SetMCPoints2SumHits()
{
	if (fInited) return;
	fUseMCPoints=kTRUE;
	fUseSummableHits=kFALSE;
	fUseUnSummableHits=kFALSE;
	fProduceSummableHits=kTRUE;
}

/** Process summable hits to hits
 ** Summable hits -> Hits **/
inline void CbmEcalHitProducer::SetSumHits2Hits()
{
	if (fInited) return;
	fUseMCPoints=kFALSE;
	fUseSummableHits=kTRUE;
	fUseUnSummableHits=kFALSE;
	fProduceSummableHits=kFALSE;
}


/** Set noise for ECAL (GeV) **/
inline void CbmEcalHitProducer::SetNoise(Float_t noise)
{
	fNoise=noise;
}

/** Set noise for PS (GeV)**/
inline void CbmEcalHitProducer::SetPSNoise(Float_t noiseps)
{
	fNoisePS=noiseps;
}

inline Float_t CbmEcalHitProducer::GetNoise() const
{
	return fNoise;
}

inline Float_t CbmEcalHitProducer::GetPSNoise() const
{
	return fNoisePS;
}

/** Set threshold for ECAL (GeV)**/
inline void CbmEcalHitProducer::SetThreshold(Float_t threshold)
{
	fThreshold=threshold;
}
/** Set threshold for PS (GeV)**/
inline void CbmEcalHitProducer::SetPSThreshold(Float_t thresholdps)
{
	fThresholdPS=thresholdps;
}

inline Float_t CbmEcalHitProducer::GetThreshold() const
{
	return fThreshold;
}

inline Float_t CbmEcalHitProducer::GetPSThreshold() const
{
	return fThresholdPS;
}

inline void CbmEcalHitProducer::SetUseMCPoints(Bool_t UseMCPoints)
{
	if (fInited) return;
	fUseMCPoints=UseMCPoints;
}

inline void CbmEcalHitProducer::SetUseSummableHits(Bool_t UseSummableHits)
{
	if (fInited) return;
	fUseSummableHits=UseSummableHits;
}

/** Can be set only if ProduceSummableHits false **/
inline void CbmEcalHitProducer::SetUseHits(Bool_t UseHits)
{
	if (fInited) return;
	if (UseHits==kTRUE&&fProduceSummableHits==kTRUE)
		return;
	fUseUnSummableHits=UseHits;
}

/** Sets type of output hits. If UseHits is set, only
 ** unsummable hits can be produced
 ** kTRUE --- produce summable hits
 ** kFALSE --- produce unsummable hits **/
inline void CbmEcalHitProducer::SetProduceSummableHits(Bool_t ProduceSummableHits)
{
	if (fInited) return;
	if (fUseUnSummableHits==kTRUE&&ProduceSummableHits==kTRUE)
		return;
	fProduceSummableHits=ProduceSummableHits;
}

inline Bool_t CbmEcalHitProducer::GetUseMCPoints() const
{
	return fUseMCPoints;
}

inline Bool_t CbmEcalHitProducer::GetUseSummableHits() const
{
	return fUseSummableHits;
}

inline Bool_t CbmEcalHitProducer::GetUseHits() const
{
	return fUseUnSummableHits;
}

inline Bool_t CbmEcalHitProducer::GetProduceSummableHits() const
{
	return fProduceSummableHits; 
}

#endif
