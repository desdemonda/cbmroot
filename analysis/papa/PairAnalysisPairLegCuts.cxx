/*************************************************************************
* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
//   Cut class providing cuts for both legs in the PairAnalysisPair     //
//                                                                       //
//                                                                       //
/*
Add any number of leg cuts using e.g. for leg 1
GetFilterLeg1().AddCuts(mycut)
where mycut has to inherit from AnalysisCuts

*/
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <TList.h>

#include "PairAnalysisPair.h"
#include "PairAnalysisTrack.h"

#include "PairAnalysisPairLegCuts.h"

ClassImp(PairAnalysisPairLegCuts)


PairAnalysisPairLegCuts::PairAnalysisPairLegCuts() :
  AnalysisCuts(),
  fFilterLeg1("PairFilterLeg1","PairFilterLeg1"),
  fFilterLeg2("PairFilterLeg2","PairFilterLeg2"),
  fCutType(kBothLegs)
{
  //
  // Default contructor
  //
}

//________________________________________________________________________
PairAnalysisPairLegCuts::PairAnalysisPairLegCuts(const char* name, const char* title) :
  AnalysisCuts(name,title),
  fFilterLeg1("PairFilterLeg1","PairFilterLeg1"),
  fFilterLeg2("PairFilterLeg2","PairFilterLeg2"),
  fCutType(kBothLegs)
{
  //
  // Named contructor
  //
}


//________________________________________________________________________
Bool_t PairAnalysisPairLegCuts::IsSelected(TObject* track)
{
  //
  // check if cuts are fulfilled
  //
  
  //check if we have a PairAnalysisPair
  PairAnalysisPair *pair=dynamic_cast<PairAnalysisPair*>(track);
  if (!pair) return kFALSE;

  //get both legs
  PairAnalysisTrack *leg1=pair->GetFirstDaughter();
  PairAnalysisTrack *leg2=pair->GetSecondDaughter();

  //mask used to require that all cuts are fulfilled
  UInt_t selectedMaskLeg1=(1<<fFilterLeg1.GetCuts()->GetEntries())-1;
  UInt_t selectedMaskLeg2=(1<<fFilterLeg2.GetCuts()->GetEntries())-1;
  
  //test cuts
  Bool_t isLeg1selected=(fFilterLeg1.IsSelected(leg1)==selectedMaskLeg1);
  if(fCutType==kBothLegs && !isLeg1selected) {
    SetSelected(isLeg1selected);
    return isLeg1selected;
  }
  Bool_t isLeg2selected=(fFilterLeg2.IsSelected(leg2)==selectedMaskLeg2);
  
  Bool_t isLeg1selectedMirror=(fFilterLeg1.IsSelected(leg2)==selectedMaskLeg1);
  Bool_t isLeg2selectedMirror=(fFilterLeg2.IsSelected(leg1)==selectedMaskLeg2);
  
  Bool_t isSelected=isLeg1selected&&isLeg2selected;
  if (fCutType==kAnyLeg)
    isSelected=isLeg1selected||isLeg2selected;
  
  if (fCutType==kMixLegs)
    isSelected=(isLeg1selected&&isLeg2selected)||(isLeg1selectedMirror&&isLeg2selectedMirror);
  
  SetSelected(isSelected);
  return isSelected;
}



