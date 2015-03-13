#ifndef CBMECALANALYSISJPSI_H
#define CBMECALANALYSISJPSI_H

#include "FairTask.h"

#include "TString.h"


class TTree;
class TClonesArray;
class CbmVertex;
class CbmStsKFTrackFitter;
class CbmEcalStructure;

class CbmEcalAnalysisJPsi : public FairTask
{
public:
  CbmEcalAnalysisJPsi(const char* name, const Int_t iVerbose, const char* fname);
public:
  /** Default constructor **/
  CbmEcalAnalysisJPsi();

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalAnalysisJPsi() {};
private:
  TString fName;

  void InitTree();
  void InitVariables();
  /** Output tree **/
  TTree* fOut;
  /** Event number**/
  Int_t fEv;
  /** Momentum of the particle **/
  Double_t fPX;
  Double_t fPY;
  Double_t fPZ;
  Double_t fPT;
  Double_t fP;
  /** Probability of electron using only energy deposition in calorimeter **/
  Double_t fEProb;
  /** Shape analysis result **/
  Double_t fShape;
  /** Particle electron **/
  Int_t fIsE;
  /** MC PDG of the particle **/
  Int_t fMCPDG;
  /** MC PDG of mother of the particle **/
  Int_t fMotherMCPDG;
  /** Charge of the particle **/
  Int_t fCharge;
  /** Energy in calorimeter **/
  Double_t fCaloE;
  /** \chi^2 of the cluster **/
  Double_t fChi2;
  Int_t fCellType;

  TClonesArray* fMC;			//!
  TClonesArray* fStsTracks;		//!
  TClonesArray* fStsTracksMatch;	//!
  TClonesArray* fEcalId;		//!
  CbmVertex* fPrimVertex;		//!
  CbmStsKFTrackFitter* fFitter;		//!
  CbmEcalStructure* fStr;		//!

  CbmEcalAnalysisJPsi(const CbmEcalAnalysisJPsi&);
  CbmEcalAnalysisJPsi& operator=(const CbmEcalAnalysisJPsi&);

  ClassDef(CbmEcalAnalysisJPsi,1)
};

#endif 

