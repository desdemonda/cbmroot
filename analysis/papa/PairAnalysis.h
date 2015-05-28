#ifndef PAIRANALYSIS_H
#define PAIRANALYSIS_H

#include <TNamed.h>
#include <TObjArray.h>
#include <THnBase.h>
#include <TSpline.h>

#include "AnalysisFilter.h"
#include "PairAnalysisHistos.h"
#include "PairAnalysisHF.h"
#include "PairAnalysisCutQA.h"

class PairAnalysisEvent;
class THashList;
//class PairAnalysisCF;
//class PairAnalysisDebugTree;
class PairAnalysisTrackRotator;
class PairAnalysisPair;
class PairAnalysisSignalMC;
////class PairAnalysisMixingHandler;

//________________________________________________________________
class PairAnalysis : public TNamed {

public:
  enum EPairType { kSEPP=0, kSEPM, kSEMM,
		   kMEPP, kMEMP, kMEPM, kMEMM,
		   kSEPMRot, kPairTypes };
  enum ELegType  { kSEP, kSEM, kLegTypes };

  PairAnalysis();
  PairAnalysis(const char* name, const char* title);
  virtual ~PairAnalysis();

  void Init();

  void Process(/*PairAnalysisEvent *ev1, */TObjArray *arr);
  Bool_t Process(PairAnalysisEvent *ev1);

  const AnalysisFilter& GetEventFilter() const { return fEventFilter; }
  const AnalysisFilter& GetTrackFilter() const { return fTrackFilter; }
  const AnalysisFilter& GetPairFilter()  const { return fPairFilter;  }

  const AnalysisFilter& GetTrackFilterMC() const { return fTrackFilterMC; }
  const AnalysisFilter& GetPairFilterMC()  const { return fPairFilterMC; }

  AnalysisFilter& GetEventFilter()       { return fEventFilter;       }
  AnalysisFilter& GetTrackFilter()       { return fTrackFilter;       }
  AnalysisFilter& GetPairFilter()        { return fPairFilter;        }
  AnalysisFilter& GetPairPreFilter()     { return fPairPreFilter;     }
  AnalysisFilter& GetPairPreFilterLegs() { return fPairPreFilterLegs; }
  AnalysisFilter& GetEventPlanePreFilter(){ return fEventPlanePreFilter; }
  AnalysisFilter& GetEventPlanePOIPreFilter(){ return fEventPlanePOIPreFilter; }

  AnalysisFilter& GetTrackFilterMC()       { return fTrackFilterMC;       }
  AnalysisFilter& GetPairFilterMC()       { return fPairFilterMC;       }

  void SetCutQA(Bool_t qa=kTRUE) { fCutQA=qa; }
  void SetNoPairing(Bool_t noPairing=kTRUE) { fNoPairing=noPairing; }
  Bool_t IsNoPairing() { return fNoPairing; }
  void SetProcessLS(Bool_t doLS=kTRUE) { fProcessLS=doLS; }
  Bool_t DoProcessLS() { return fNoPairing; }
  void SetUseKF(Bool_t useKF=kTRUE) { fUseKF=useKF; }
  const TObjArray* GetTrackArray(Int_t i) const {return (i>=0&&i<2)?&fTracks[i]:0;}
  const TObjArray* GetPairArray(Int_t i)  const {return (i>=0&&i<8)?
      static_cast<TObjArray*>(fPairCandidates->UncheckedAt(i)):0;}

  TObjArray** GetPairArraysPointer() { return &fPairCandidates; }
  void SetPairArraysPointer( TObjArray *arr) { fPairCandidates=arr; }

  // outputs - hist array
  void SetHistogramArray(PairAnalysisHF * const histoarray) { fHistoArray=histoarray; }
  const TObjArray * GetHistogramArray() const { return fHistoArray?fHistoArray->GetHistArray():0x0; }
  const TObjArray * GetQAHistArray() const { return fQAmonitor?fQAmonitor->GetQAHistArray():0x0; }
  // outputs - histos
  void SetHistogramManager(PairAnalysisHistos * const histos) { fHistos=histos; }
  PairAnalysisHistos* GetHistoManager() const { return fHistos; }
  const THashList * GetHistogramList() const { return fHistos?fHistos->GetHistogramList():0x0; }
  // outputs - CF container
  //  void SetCFManagerPair(PairAnalysisCF * const cf) { fCfManagerPair=cf; }
  //  PairAnalysisCF* GetCFManagerPair() const { return fCfManagerPair; }
  // outputs - debug tree
  //  void SetDebugTree(PairAnalysisDebugTree * const tree) { fDebugTree=tree; }
  void SaveDebugTree();


  Bool_t HasCandidates()   const { return GetPairArray( 1)?GetPairArray( 1)->GetEntriesFast()>0:0; }
  Bool_t HasCandidatesTR() const { return GetPairArray(7)?GetPairArray(7)->GetEntriesFast()>0:0; }
  Bool_t HasCandidatesLikeSign() const {
    return (GetPairArray(0)&&GetPairArray(2)) ? (GetPairArray(0)->GetEntriesFast()>0 || GetPairArray(2)->GetEntriesFast()>0) : 0;
  }

  // prefilter
  void SetPreFilterEventPlane(Bool_t setValue=kTRUE){fPreFilterEventPlane=setValue;};
  void SetLikeSignSubEvents(Bool_t setValue=kTRUE){fLikeSignSubEvents=setValue;};
  void SetPreFilterUnlikeOnly(Bool_t setValue=kTRUE){fPreFilterUnlikeOnly=setValue;};
  void SetPreFilterAllSigns(Bool_t setValue=kTRUE){fPreFilterAllSigns=setValue;};

  // background estimator - track rotation
  void SetTrackRotator(PairAnalysisTrackRotator * const rot) { fTrackRotator=rot; }
  PairAnalysisTrackRotator* GetTrackRotator() const { return fTrackRotator; }
  void SetStoreRotatedPairs(Bool_t storeTR) {fStoreRotatedPairs = storeTR;}
  // background estimator - mixed events
  ///  void SetMixingHandler(PairAnalysisMixingHandler *mix) { fMixing=mix; }
  ///  PairAnalysisMixingHandler* GetMixingHandler() const { return fMixing; }
  ////////////////////////////////////////////////// TEMPORAER
  TObject* GetMixingHandler() const { return 0x0; }
  ////////////////////////////////////////////////// TEMPORAER


  void SetDontClearArrays(Bool_t dontClearArrays=kTRUE) { fDontClearArrays=dontClearArrays; }
  Bool_t DontClearArrays() const { return fDontClearArrays; }

  // mc specific
  void SetHasMC(Bool_t hasMC) { fHasMC = hasMC; }
  Bool_t GetHasMC() const     { return fHasMC;  }
  void AddSignalMC(PairAnalysisSignalMC* signal);
  const TObjArray* GetMCSignals() const { return fSignalsMC; }
  void  SetMotherPdg( Int_t pdgMother ) { fPdgMother=pdgMother; }
  Int_t GetMotherPdg() const { return fPdgMother; }
  void  SetLegPdg(Int_t pdgLeg1, Int_t pdgLeg2) { fPdgLeg1=pdgLeg1; fPdgLeg2=pdgLeg2; }
  Int_t GetLeg1Pdg()   const { return fPdgLeg1;   }
  Int_t GetLeg2Pdg()   const { return fPdgLeg2;   }



  static const char* TrackClassName(Int_t i) { return (i>=0&&i<2)?fgkTrackClassNames[i]:""; }
  static const char* PairClassName(Int_t i)  { return (i>=0&&i<8)?fgkPairClassNames[i]:""; }

  Bool_t DoEventProcess() const { return fEventProcess; }
  void SetEventProcess(Bool_t setValue=kTRUE) { fEventProcess=setValue; }
  void  FillHistogramsFromPairArray(Bool_t pairInfoOnly=kFALSE);

private:

  Bool_t fCutQA;                    // monitor cuts
  PairAnalysisCutQA *fQAmonitor;   // monitoring of cuts
  AnalysisFilter fEventFilter;    // Event cuts
  AnalysisFilter fTrackFilter;    // leg cuts
  AnalysisFilter fPairPreFilter;  // pair prefilter cuts
  AnalysisFilter fPairPreFilterLegs; // Leg filter after the pair prefilter cuts
  AnalysisFilter fPairFilter;     // pair cuts
  AnalysisFilter fEventPlanePreFilter;  // event plane prefilter cuts  
  AnalysisFilter fEventPlanePOIPreFilter;  // PoI cuts in the event plane prefilter  

  AnalysisFilter fTrackFilterMC;    // MCtruth leg cuts
  AnalysisFilter fPairFilterMC;     // MCtruth pair cuts

  Int_t fPdgMother;     // pdg code of mother tracks
  Int_t fPdgLeg1;       // pdg code leg1
  Int_t fPdgLeg2;       // pdg code leg2

  TObjArray* fSignalsMC;      // array of PairAnalysisSignalMC

  Bool_t fNoPairing;    // if to skip pairing, can be used for track QA only
  Bool_t fProcessLS; // do the like-sign pairing (default kTRUE)
  Bool_t fUseKF;    // if to skip pairing, can be used for track QA only

  PairAnalysisHF *fHistoArray;   // Histogram framework
  PairAnalysisHistos *fHistos;   // Histogram manager
                                  //  Streaming and merging should be handled
                                  //  by the analysis framework
  TBits *fUsedVars;               // used variables

  TObjArray fTracks[2];           //! Selected track candidates
                                  //  0: SameEvent, positive particles
                                  //  1: SameEvent, negative particles

  TObjArray *fPairCandidates;     //! Pair candidate arrays
                                  //TODO: better way to store it? TClonesArray?

  //  PairAnalysisCF *fCfManagerPair;//Correction Framework Manager for the Pair
  PairAnalysisTrackRotator *fTrackRotator; //Track rotator
  //  PairAnalysisDebugTree *fDebugTree;  // Debug tree output
  //  PairAnalysisMixingHandler *fMixing; // handler for event mixing

  Bool_t fPreFilterEventPlane;  //Filter for the Eventplane determination in TPC
  Bool_t fLikeSignSubEvents;    //Option for dividing into subevents, sub1 ++ sub2 --
  Bool_t fPreFilterUnlikeOnly;  //Apply PreFilter either in +- or to ++/--/+- individually
  Bool_t fPreFilterAllSigns;    //Apply PreFilter find in  ++/--/+- and remove from all
  Bool_t fHasMC;                //If we run with MC, at the moment only needed in AOD
  Bool_t fStoreRotatedPairs;    //It the rotated pairs should be stored in the pair array
  Bool_t fDontClearArrays;      //Don't clear the arrays at the end of the Process function, needed for external use of pair and tracks
  Bool_t fEventProcess;         //Process event (or pair array)

  void FillTrackArrays(PairAnalysisEvent * const ev);
  //  void EventPlanePreFilter(Int_t arr1, Int_t arr2, TObjArray arrTracks1, TObjArray arrTracks2, const PairAnalysisEvent *ev);
  void PairPreFilter(Int_t arr1, Int_t arr2, TObjArray &arrTracks1, TObjArray &arrTracks2);
  void FillPairArrays(Int_t arr1, Int_t arr2);
  void FillPairArrayTR();
  
  Int_t GetPairIndex(Int_t arr1, Int_t arr2) const {return arr1>=arr2?arr1*(arr1+1)/2+arr2:arr2*(arr2+1)/2+arr1;}

  void InitPairCandidateArrays();
  void ClearArrays();
  
  TObjArray* PairArray(Int_t i);
  TObject* InitEffMap(TString filename);
  
  static const char* fgkTrackClassNames[2];   //Names for track arrays
  static const char* fgkPairClassNames[8];   //Names for pair arrays

  void ProcessMC();

  void  FillHistograms(const PairAnalysisEvent *ev, Bool_t pairInfoOnly=kFALSE);
  void  FillMCHistograms(Int_t label1, Int_t label2, Int_t nSignal);
  void  FillHistogramsMC(const PairAnalysisEvent *ev,  PairAnalysisEvent *ev1);
  void  FillHistogramsPair(PairAnalysisPair *pair,Bool_t fromPreFilter=kFALSE);
  void  FillHistogramsTracks(TObjArray **tracks);

  void  FillDebugTree();

  PairAnalysis(const PairAnalysis &c);
  PairAnalysis &operator=(const PairAnalysis &c);

  ClassDef(PairAnalysis,1);
};

inline void PairAnalysis::InitPairCandidateArrays()
{
  //
  // initialise all pair candidate arrays
  //
  fPairCandidates->SetOwner();
  for (Int_t i=0;i<8;++i){
    TObjArray *arr=new TObjArray;
    arr->SetName(fgkPairClassNames[i]);
    fPairCandidates->AddAt(arr,i);
    arr->SetOwner();
  }
}

inline TObjArray* PairAnalysis::PairArray(Int_t i)
{
  //
  // for internal use only: unchecked return of pair array for fast access
  //
  return static_cast<TObjArray*>(fPairCandidates->UncheckedAt(i));
}

inline void PairAnalysis::ClearArrays()
{
  //
  // Reset the Arrays
  //
  for (Int_t i=0;i<2;++i){
    fTracks[i].Clear();
  }
  for (Int_t i=0;i<8;++i){
    if (PairArray(i)) PairArray(i)->Delete();
  }
}

#endif
