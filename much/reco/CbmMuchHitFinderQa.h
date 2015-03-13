// -------------------------------------------------------------------------
// -----                  CbmMuchHitFinderQa header file               -----
// -----                  Created 16/11/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

#ifndef CbmMuchHitFinderQa_H
#define CbmMuchHitFinderQa_H

#include "FairTask.h"

#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TClonesArray.h"

class CbmMuchGeoScheme;
class TObjArray;
class TVector2;

Double_t LandauMPV(Double_t *x, Double_t *par);

class CbmMuchHitFinderQa : public FairTask {

public:
  CbmMuchHitFinderQa(const char* name = "MuchHitFinderQa", Int_t verbose = 1);
  virtual ~CbmMuchHitFinderQa();
  virtual InitStatus Init();
  virtual void Exec(Option_t * option);
  virtual void FinishTask();
  virtual void SetParContainers();
  void SetGeoFileName        (TString fileName) { fGeoFileName = fileName; }
  void SetPerformanceFileName(TString fileName) { fFileName    = fileName; }

  void SetPullsQa(Bool_t on) { fPullsQaOn = on; }
  void SetOccupancyQa(Bool_t on) { fOccupancyQaOn = on; }
  void SetDigitizerQa(Bool_t on) { fDigitizerQaOn = on; }
  void SetStatisticsQa(Bool_t on) { fStatisticsQaOn = on; }
  void SetClusterDeconvQa(Bool_t on) { fClusterDeconvQaOn = on; }


  void SetPrintToFile(Bool_t on) { fPrintToFileOn = on; }

protected:
  /* Analysis of hit uncertainty (pull) distributions
   * as function of pad size and cluster shape
   */
  void PullsQa();

  /* Occupance analysis - all pads,fired pads,
   * and fired/all distributions as functions of radius
   */
  void OccupancyQa();

  /* DigitizerQa - analysis of digitizer performance - charge distributions
   * for tracks. Track length distrivutions. Statistics on particle types
   */
  void DigitizerQa();

  /* Information on clusters - number of pads in a cluster, number of points, contributed to
   * a cluster, number of hits, created from a cluster
   */
  void StatisticsQa();

  /* Number of points and hits in a cluster for signal muons (MotherId = 0) */
  void ClusterDeconvQa();

private:
  CbmMuchGeoScheme* fGeoScheme;
  TString fGeoFileName;
  TString fFileName;
  Int_t fSignalPoints;           // Number of signal MC points
  Int_t fSignalHits;             // Number of signal hits
  Int_t fVerbose;
  Int_t fEvent;
  TClonesArray *fPoints;
  TClonesArray *fDigis;
  TClonesArray *fDigiMatches;
  TClonesArray *fClusters;
  TClonesArray *fHits;
  TClonesArray *fMCTracks;
  TClonesArray *fPointInfos;

  Int_t  fNstations;

  TObjArray* fChargeHistos;
  TH2D* fhChargeEnergyLog;
  TH2D* fhChargeEnergyLogPi;
  TH2D* fhChargeEnergyLogPr;
  TH2D* fhChargeEnergyLogEl;
  TH2D* fhChargeTrackLength;
  TH2D* fhChargeTrackLengthPi;
  TH2D* fhChargeTrackLengthPr;
  TH2D* fhChargeTrackLengthEl;
  TH1D* fhCharge;
  TH1D* fhChargeLog;
  TH1D* fhChargePr_1GeV_3mm;
  TH2D* fhNpadsVsS;

  TH1D** fhOccupancyR;
  TH1D** fhPadsTotalR;
  TH1D** fhPadsFiredR;
  TH1D** fhPullXpads1;
  TH1D** fhPullYpads1;
  TH1D** fhPullXpads2;
  TH1D** fhPullYpads2;
  TH1D** fhPullXpads3;
  TH1D** fhPullYpads3;
  Int_t  fnPadSizesX;
  Int_t  fnPadSizesY;

  TH1D** fhPullT;
  Int_t fNTimingPulls;
  
  TH1D* fhPullX;
  TH1D* fhPullY;

  TH1I* fhPointsInCluster;
  TH1I* fhDigisInCluster;
  TH1I* fhHitsInCluster;

  Int_t *fNall;         // number of all tracks at the first station
  Int_t *fNpr;          // number of protons at the first station
  Int_t *fNpi;          // number of pions at the first station
  Int_t *fNel;          // number of electrons at the first station
  Int_t *fNmu;          // number of muons at the first station
  Int_t *fNka;          // number of kaons at the first station
  Int_t *fNprimary;     // number of primary tracks at the first station
  Int_t *fNsecondary;   // number of secondary tracks at the first station

  Int_t fPointsTotal;
  Int_t fPointsUnderCounted;
  Int_t fPointsOverCounted;

  Bool_t fOccupancyQaOn;
  Bool_t fPullsQaOn;
  Bool_t fDigitizerQaOn;
  Bool_t fStatisticsQaOn;
  Bool_t fClusterDeconvQaOn;

  Bool_t fPrintToFileOn;

  Double_t fPadMinLx;
  Double_t fPadMinLy;
  Double_t fPadMaxLx;
  Double_t fPadMaxLy;


  /** Defines whether the point with the given index is signal point. **/
  Bool_t IsSignalPoint(Int_t iPoint);

  Int_t GetNChannels(Int_t iStation);
  Int_t GetNSectors(Int_t iStation);
  TVector2 GetMinPadSize(Int_t iStation);
  TVector2 GetMaxPadSize(Int_t iStation);
  FILE * pointsFile;
  FILE * padsFile;

  CbmMuchHitFinderQa(const CbmMuchHitFinderQa&);
  CbmMuchHitFinderQa& operator=(const CbmMuchHitFinderQa&);

  ClassDef(CbmMuchHitFinderQa,1)
};

#endif

