/** Matching between MC photons and reconstructed photons **/

#ifndef CBMECALMATCHING_H
#define CBMECALMATCHING_H

#include "FairTask.h"
#include <list>
#include <map>
#include "TArrayD.h"

class TClonesArray;
class CbmEcalCell;
class CbmEcalRecParticle;
class TTree;
class CbmEcalStructure;
class CbmEcalShLib;

class CbmEcalMatching : public FairTask
{
public:
  /** Only to comply with framework **/
  CbmEcalMatching();
  /** Standard constructor **/
  CbmEcalMatching(const char* name, const Int_t verbose, const char* configname);

  /** Task initialization **/
  virtual InitStatus Init();

  /** Exec for task **/
  virtual void Exec(Option_t* opt);
  
  /** Finish task **/
  virtual void Finish();

  /** Get/set threshold **/
  void SetThreshold(Double_t threshold=0.8) {fThreshold=threshold;}
  Double_t GetThreshold() const {return fThreshold;}
  /** Get/set photon threshold **/
  void SetPhotonThreshold(Double_t threshold=0.95) {fPhotonThr=threshold;}
  Double_t GetPhotonThreshold() const {return fPhotonThr;}
  /** Get/set mother threshold **/
  void SetMotherThreshold(Double_t threshold=1.05) {fMotherThr=threshold;}
  Double_t GetMotherThreshold() const {return fMotherThr;}
  /** virtual destructor **/
  ~CbmEcalMatching();

  /** Set output to tree **/
  void SetOutToTree(Bool_t totree=kFALSE) {fToTree=totree;}
private:
  /** Form a precluster **/
  void FormPreCluster(CbmEcalRecParticle* p);
  /** Form a map of energies for precluster **/
  void FormE();
  /** Add energy to track and all its mothers **/
  void AddTrackE(Int_t track, Double_t e);
  /** Match MCTrack and reconstructed particle **/
  void Match(CbmEcalRecParticle* p);
  /** Get form of energy deposition of the particle **/
  void FormEpred(CbmEcalRecParticle* p);
  /** Match MCTrack and reconstructed partile using shower shape **/
  void MatchP(CbmEcalRecParticle* p);
  /** First realization of matching **/
  void MatchP1(CbmEcalRecParticle* p);
  /** Second realization of matching **/
  void MatchP2(CbmEcalRecParticle* p);
  /** Third realization of matching **/
  void MatchP3(CbmEcalRecParticle* p);
  /** Add to particle constants to track and all its mothers **/
  void AddTrackP(Int_t track, Double_t e, Int_t cell);
  /** An array of reconstructed photons **/
  TClonesArray* fReco;			//!
  /** An array of MC tracks **/
  TClonesArray* fMCTracks;		//!
  /** An array of calorimeter points **/
  TClonesArray* fPoints;		//!
  /** An array of calorimeter clustes **/
  TClonesArray* fClusters;		//!
  /** Structure of the calorimeter system **/
  CbmEcalStructure* fStr;		//!
  /** Shower library. A new implementation **/
  CbmEcalShLib* fShLib;			//!

  /** A threshold: if energy of photon is more than threshold*Ecluster
   ** than cluster is formed by that photon **/
  Double_t fThreshold;
  /** If energy of max photon > fPhotonThr*energy of any other particle will match with photon **/
  Double_t fPhotonThr;
  /** If energy of mother > fMotherThr*Energy_{doughtier} then use mother for cluster matching instead
   ** of doughtier**/
  Double_t fMotherThr;

  /** An event number **/
  Int_t fEv;

  /** A precluster **/
  std::list<CbmEcalCell*> fPreCluster;	//!
  /** An energy of precluster **/
  Double_t fClusterE;
  /** An energy associated with track number **/
  std::map<Int_t, Double_t> fE;		//!
  /** Predicted by shower library energy in every cell of the cluster **/
  TArrayD fEpred;
  /** Total predicted energy depostion of the particle in the cluster **/
  Double_t fEsum;
  /** A cluster constant **/
  Double_t fS;
  /** An MC particle constant associated with track number **/
  std::map<Int_t, Double_t> fP;		//!

  /** Current cell **/
  CbmEcalCell* fCell;			//!

  /** Shoul write information to tree? **/
  Bool_t fToTree;
  /** An output tree **/
  TTree* fTree;				//!
  /** Init tree **/
  void InitTree();
  /** Number of corresponding MC track **/
  Int_t fMCNum;
  /** Reconstructed information **/
  Double_t fPx;
  Double_t fPy;
  Double_t fPz;
  Double_t fRE;
  /** MC information **/
  Double_t fMCPx;
  Double_t fMCPy;
  Double_t fMCPz;
  Double_t fMCE;
  Int_t fPDG;
  /** Information about cluster **/
  Double_t fChi2;
  /** Number of maximums **/
  Int_t fNM;
  /** Number cells **/
  Int_t fNC;

  /** Name of configuration file **/
  TString fConfigName;
  /** Type of algorithm to use **/
  Int_t fAlgo;

  CbmEcalMatching(const CbmEcalMatching&);
  CbmEcalMatching& operator=(const CbmEcalMatching&);

  ClassDef(CbmEcalMatching, 1)
};

#endif
