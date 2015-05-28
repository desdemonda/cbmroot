#ifndef PAIRANALYSISHF_H
#define PAIRANALYSISHF_H

/* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//#############################################################
//#                                                           # 
//#         Class PairAnalysisHF                     #
//#                                                           #
//#  Authors:                                                 #
//#   Julian Book, Uni-Frankfurt / Julian.Book@cern.ch        #
//#                                                           #
//#############################################################

#include <TNamed.h>
#include <TObjArray.h>
#include <TBits.h>
#include <THnBase.h>

#include "PairAnalysisVarManager.h"
#include "PairAnalysisHistos.h"

class PairAnalysisHF : public PairAnalysisHistos {
public:
  enum { kMaxCuts=20 };

  PairAnalysisHF();
  PairAnalysisHF(const char*name, const char* title);

  virtual ~PairAnalysisHF();

  void Init();
  Int_t GetNumberOfBins() const;
  const TObjArray * GetHistArray() const { return &fArrDielHistos; }

  // functions to define the grid
  void AddCutVariable(PairAnalysisVarManager::ValueTypes type,
		      TVectorD * binLimits, Bool_t leg=kFALSE);

  // functions to fill objects
  Int_t FindCell(const Double_t *values);
  void FillClass(const char* histClass, const Double_t *values);
  // TODO: check, remove, implement
  void Fill(Int_t pairIndex, const PairAnalysisPair *particle);
  void Fill(Int_t label1, Int_t label2, Int_t nSignal);
  void Fill(Int_t Index, Double_t * const valuesPair, Double_t * const valuesLeg1, Double_t * const valuesLeg2);

  // functions to get and draw histograms
  void ReadFromFile(const char* file="histos.root", const char *task="", const char *config="");

private:
  TBits     *fUsedVars;             // list of used variables

  TObjArray fArrDielHistos;           //-> array of papahistos
  TObjArray* fSignalsMC;            //! array of MC signals to be stupapad

  UShort_t  fVarCuts[kMaxCuts];     // cut variables
  TBits     *fVarCutType;           // array to store leg booleans
  //  Bool_t    fVarCutType[kMaxCuts];  // array to store leg booleans
  TObjArray fAxes;                  // Axis descriptions of the cut binning

  PairAnalysisHF(const PairAnalysisHF &c);
  PairAnalysisHF &operator=(const PairAnalysisHF &c);

  ClassDef(PairAnalysisHF,1)         // PairAnalysis HF
};



#endif
