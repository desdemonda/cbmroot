/** Cluster finder for calorimeter into muon option
 ** Produces a CbmEcalClusterV1. **/

#ifndef CBMECALCLUSTERFINDERV2_H
#define CBMECALCLUSTERFINDERV2_H

#include "FairTask.h"

#include <list>

class TClonesArray;
class CbmEcalStructure;
class CbmEcalCell;
class CbmEcalInf;
class CbmEcalCalibration;
class CbmEcalPreCluster;
class CbmEcalMaximum;
class TTree;

class CbmEcalClusterFinderV2 : public FairTask
{
public:
  /** Standard constructor **/
  CbmEcalClusterFinderV2(const char* name, const Int_t verbose, const char* cfg);
  /** Only to comply with frame work. **/
  CbmEcalClusterFinderV2();

  /** Destructor **/
  virtual ~CbmEcalClusterFinderV2();

  /** Finish a task **/
  virtual void Finish();

  /** Exec a task **/
  virtual void Exec(Option_t* option);

  /** Initialization **/
  virtual InitStatus Init();
private:
  /** Create an output tree **/
  void CreateTree();
  /** Get region number of the cell **/
  Int_t GetRegion(CbmEcalCell* cls);
  /** Build preclusters **/
  void FormPreClusters();
  /** A default algorithm of precluster formation **/
  void FormPreCluster(CbmEcalMaximum* cell);
  /** A new algorithm of precluster formation **/
  void FormPreClusterNew(CbmEcalMaximum* cell);
  /** Find a common area of ellipse and cell**/
  Double_t SolveEllipse(CbmEcalCell* cell, Double_t cx, Double_t cy, Int_t reg);

  /** Build clusters from preclusters **/
  void FormClusters();
  /** Clear a preclusters list **/
  void ClearPreClusters();

  /** Current event **/
  Int_t fEv;

  /** A calorimeter structure **/
  CbmEcalStructure* fStr;		//!
  /** An information about calorimeter **/
  CbmEcalInf* fInf;			//!
  /** A calibration **/
  CbmEcalCalibration* fCal;		//!
  /** Tracks before the calorimeter **/
  TClonesArray* fTracks;		//!
  /** Array of maximums in calorimeter.
   ** Maximums belong to charged tracks excluded. **/
  TClonesArray* fMaximums;		//!
  /** An array of clusters **/
  TClonesArray* fClusters;		//!

  /** A list of preclusters
   ** May be better use TClonesArray? **/
  std::list<CbmEcalPreCluster*> fPreClusters;		//!

  /** Maximums atrributed to charged tracks should be removed? **/
  Int_t fRemoveCharged;
  /** Should we use calibration?
   ** If no, than a cluster finder will be used for calibration **/
  Int_t fUseCalibration;
  /** Number of calorimeter regions **/
  Int_t fRegions;
  /** An algorithm for preclustering
   ** 0 --- default
   ** 1 --- new algorithm with ellipses **/
  Int_t fPreClusterAlgo[10];
  /** Minimum energy of precluster **/
  Double_t fMinClusterE[10];
  /** Minimum energy of precluster maximum for consideration **/
  Double_t fMinMaxE[10];
  /** Maximum theta angle for cluster algorithm **/
  Double_t fTheta[10];
  /** Just form preclusters and store information in tree **/
  Int_t fCalibration;
  /** Should we add a minimum cell to precluster? **/
  Int_t fUseMinimumCell;
  /** Parameters for cluster construction **/
  Double_t fPar1[10];
  Double_t fPar2[10];
  Double_t fPar3[10];
  Int_t fParI[10];
  Int_t fIntSteps;
  

  TTree* fTree;				//!
  Double_t fE[10];			//!
  Int_t fAlgo;
  Int_t fAlgoType;
  Int_t fCurAlgo;
  Double_t fX;
  Double_t fY;
  Double_t fThetaOut;

  CbmEcalClusterFinderV2(const CbmEcalClusterFinderV2&);
  CbmEcalClusterFinderV2& operator=(const CbmEcalClusterFinderV2&);

  ClassDef(CbmEcalClusterFinderV2, 1)
};

#endif

