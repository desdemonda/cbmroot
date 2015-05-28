#ifndef ANALYSISTASKMULTIPAIRANALYSIS_H
#define ANALYSISTASKMULTIPAIRANALYSIS_H
/* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//#####################################################
//#                                                   # 
//#        Basic Analysis task for PairAnalysis         #
//#          single event analysis                    #
//#                                                   #
//#     Julian Book, Uni Ffm / Julian.Book@cern.ch    #
//#                                                   #
//#####################################################

#include "TList.h"


#include "FairTask.h"
#include "CbmRichElectronIdAnn.h"

#include "AnalysisCuts.h"
#include "PairAnalysisEvent.h"

class PairAnalysis;
class TH1D;
//class AnalysisCuts;

class AnalysisTaskMultiPairAnalysis : public FairTask {
  
public:
  AnalysisTaskMultiPairAnalysis();
  AnalysisTaskMultiPairAnalysis(const char *name);
  virtual ~AnalysisTaskMultiPairAnalysis();

  virtual InitStatus Init();
  virtual void Exec(Option_t *option);
  virtual void FinishTask();

  void SetEventFilter(AnalysisCuts * const filter) {fEventFilter=filter;}

  void AddPairAnalysis(PairAnalysis * const papa) { fListPairAnalysis.Add((TObject*)papa); } //why cast to tobject????????
  void SetBeamEnergy(Double_t beamEbyHand=-1.)   { fBeamEnergy=beamEbyHand;  }
  void SetWeight(    Double_t weightbyHand=-1.)  { fWeight=weightbyHand;     }

protected:
  enum {kAllEvents=0, kSelectedEvents, kV0andEvents, kFilteredEvents, kPileupEvents, kNbinsEvent};
  TObjArray *fPairArray;             //! output array
  TList fListPairAnalysis;             //  List of papa framework instances
  TList fListHistos;                 //! List of histogram manager lists in the framework classes
  //  TList fListCF;                     //! List with CF Managers

  CbmRichElectronIdAnn *fgRichElIdAnn; // neural network for Rich electron ID
  Double_t fBeamEnergy;              // beam energy in GeV (set by hand)
  Double_t fWeight;                  // weight N*BR (set by hand)

  AnalysisCuts *fEventFilter;        // event filter

  TH1D *fEventStat;                  //! Histogram with event statistics

  PairAnalysisEvent *fInputEvent; //! event handler

  AnalysisTaskMultiPairAnalysis(const AnalysisTaskMultiPairAnalysis &c);
  AnalysisTaskMultiPairAnalysis& operator= (const AnalysisTaskMultiPairAnalysis &c);
  
  ClassDef(AnalysisTaskMultiPairAnalysis, 1); //Analysis Task handling multiple instances of PairAnalysis
};
#endif
