#ifndef PAIRANALYSISCUTGROUP_H
#define PAIRANALYSISCUTGROUP_H

/* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//#################################################################
//#                                                               #
//#             Class PairAnalysisCutGroup                       #
//#              PairAnalysis Group of cuts                         #
//#                                                               #
//#  Authors:                                                     #
//#   Julian    Book,     Uni Ffm / Julian.Book@cern.ch           #
//#                                                               #
//#################################################################

#include <TList.h>

#include "AnalysisCuts.h"

class TCollection;

class PairAnalysisCutGroup : public AnalysisCuts {
  
public:
  enum TruthValues {
    kCompAND = kTRUE,
    kCompOR = kFALSE
  };
  
  PairAnalysisCutGroup(Bool_t compOperator=kCompOR);
  PairAnalysisCutGroup(const char*name, const char* title, Bool_t compOperator=kCompOR);
  
  virtual ~PairAnalysisCutGroup();
  
  //Analysis cuts interface
  //
  virtual void Init();
  virtual Bool_t IsSelected(TObject* track);
  virtual Bool_t IsSelected(TList*   /* list */ ) {return kFALSE;}
  
  void AddCut(AnalysisCuts* fCut);
  void SetCompOperator(Bool_t compOperator);

  virtual void Print(const Option_t* option = "") const;

  
private:
  TList  fCutGroupList;  //for saving the different cuts
  Bool_t fCompOperator;  //determines whether the cuts are AND/OR compared
  
  ClassDef(PairAnalysisCutGroup,1) //Group of cuts
};

#endif
