///////////////////////////////////////////////////////////////////////////
//                PairAnalysis VarCutsCombi                                //
//                                                                       //
//                                                                       //
/*
Add Detailed description


*/
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include <TFormula.h>

#include "PairAnalysisVarManager.h"
#include "PairAnalysisVarCuts.h"

#include "PairAnalysisVarCutsCombi.h"

ClassImp(PairAnalysisVarCutsCombi)

PairAnalysisVarCutsCombi::PairAnalysisVarCutsCombi() :
  AnalysisCuts(),
  fUsedVars(new TBits(PairAnalysisVarManager::kNMaxValuesMC)),
  fNActiveCuts(0),
  fActiveCutsMask(0),
  fSelectedCutsMask(0),
  fCutType(kAll)
{
  //
  // Default Constructor
  //
  for (Int_t i=0; i<kNmaxCuts; ++i){
    fActiveCuts[i]=0;
    fCutMin[i]=0;
    fCutMax[i]=0;
    fCutExclude[i]=kFALSE;
    fFormulaCut[i]=0x0;
    fVarCuts[i]=0x0;
  }
  PairAnalysisVarManager::InitFormulas();

}

//______________________________________________
PairAnalysisVarCutsCombi::PairAnalysisVarCutsCombi(const char* name, const char* title) :
  AnalysisCuts(name, title),
  fUsedVars(new TBits(PairAnalysisVarManager::kNMaxValuesMC)),
  fNActiveCuts(0),
  fActiveCutsMask(0),
  fSelectedCutsMask(0),
  fCutType(kAll)
{
  //
  // Named Constructor
  //
  for (Int_t i=0; i<kNmaxCuts; ++i){
    fActiveCuts[i]=0;
    fCutMin[i]=0;
    fCutMax[i]=0;
    fCutExclude[i]=kFALSE;
    fFormulaCut[i]=0x0;
    fVarCuts[i]=0x0;
  }
  PairAnalysisVarManager::InitFormulas();
}

//______________________________________________
PairAnalysisVarCutsCombi::~PairAnalysisVarCutsCombi()
{
  //
  // Default Destructor
  //
  if (fUsedVars) delete fUsedVars;
  for (Int_t icut=0; icut<kNmaxCuts; ++icut){
    if(fFormulaCut[icut]) delete fFormulaCut[icut];
    if(fVarCuts[icut])    delete fVarCuts[icut];
  }
}

//______________________________________________
void PairAnalysisVarCutsCombi::AddCut(PairAnalysisVarManager::ValueTypes type,
			   Double_t min, Double_t max, Bool_t exclude,
			   PairAnalysisVarManager::ValueTypes typeR,
			   Double_t rmin, Double_t rmax, Bool_t excludeRange)
{
  //
  // add VarCutsCombi cuts
  //
  PairAnalysisVarCuts *cut = 0x0;
  if( (UShort_t)typeR<(UShort_t)PairAnalysisVarManager::kNMaxValuesMC) {
    cut = new PairAnalysisVarCuts("cut","cut");
    cut->AddCut(typeR,rmin,rmax,excludeRange);
  }
  AddCut(type,min,max,exclude,cut);

}

//______________________________________________
void PairAnalysisVarCutsCombi::AddCut(PairAnalysisVarManager::ValueTypes type,
			   Double_t min, Double_t max, Bool_t exclude,
			   const char *formulaR,
			   Double_t rmin, Double_t rmax, Bool_t excludeRange)
{
  //
  // add VarCutsCombi cuts
  //
  PairAnalysisVarCuts *cut = 0x0;
  if(strlen(formulaR)) {
    cut = new PairAnalysisVarCuts("cut","cut");
    cut->AddCut(formulaR,rmin,rmax,excludeRange);
  }
  AddCut(type,min,max,exclude,cut);

}

//______________________________________________
void PairAnalysisVarCutsCombi::AddCut(const char *formula,
			   Double_t min, Double_t max, Bool_t exclude,
			   PairAnalysisVarManager::ValueTypes typeR,
			   Double_t rmin, Double_t rmax, Bool_t excludeRange)
{
  //
  // add VarCutsCombi cuts
  //
  PairAnalysisVarCuts *cut = 0x0;
  if( (UShort_t)typeR<(UShort_t)PairAnalysisVarManager::kNMaxValuesMC) {
    cut = new PairAnalysisVarCuts("cut","cut");
    cut->AddCut(typeR,rmin,rmax,excludeRange);
  }
  AddCut(formula,min,max,exclude,cut);

}

//______________________________________________
void PairAnalysisVarCutsCombi::AddCut(const char *formula,
			   Double_t min, Double_t max, Bool_t exclude,
			   const char *formulaR,
			   Double_t rmin, Double_t rmax, Bool_t excludeRange)
{
  //
  // add VarCutsCombi cuts
  //
  PairAnalysisVarCuts *cut = 0x0;
  if(strlen(formulaR)) {
    cut = new PairAnalysisVarCuts("cut","cut");
    cut->AddCut(formulaR,rmin,rmax,excludeRange);
  }
  AddCut(formula,min,max,exclude,cut);

}

//______________________________________________
void PairAnalysisVarCutsCombi::AddCut(PairAnalysisVarManager::ValueTypes type,
			   Double_t min, Double_t max, Bool_t exclude,
			   PairAnalysisVarCuts *varcuts )
{
  //
  // add VarCutsCombi cuts - main function
  //

  fCutMin[fNActiveCuts]         = min;
  fCutMax[fNActiveCuts]         = max;
  fCutExclude[fNActiveCuts]     = exclude;
  SETBIT(fActiveCutsMask,fNActiveCuts);
  fActiveCuts[fNActiveCuts]     = (UShort_t)type;
  fUsedVars->SetBitNumber(type,kTRUE);

  fFormulaCut[fNActiveCuts]     = 0x0;
  fVarCuts[fNActiveCuts]        = varcuts;
  ++fNActiveCuts;

}

//______________________________________________
void PairAnalysisVarCutsCombi::AddCut(const char *formula,
			   Double_t min, Double_t max, Bool_t exclude,
			   PairAnalysisVarCuts *varcuts )
{
  //
  // add VarCutsCombi cuts - main function
  //

  fCutMin[fNActiveCuts]         = min;
  fCutMax[fNActiveCuts]         = max;
  fCutExclude[fNActiveCuts]     = exclude;
  SETBIT(fActiveCutsMask,fNActiveCuts);
  fActiveCuts[fNActiveCuts]     = 0;

  // construct a TFormula
  TFormula *form = new TFormula("varFormula",formula);
  // compile function
  if(form->Compile()) return;
  //set parameter identifier and activate variables in the fill map
  for(Int_t i=0; i<form->GetNpar(); i++) {
    form->SetParName(  i, PairAnalysisVarManager::GetValueName(form->GetParameter(i)) );
    fUsedVars->SetBitNumber((Int_t)form->GetParameter(i),kTRUE);
  }

  fFormulaCut[fNActiveCuts]     = form;
  fVarCuts[fNActiveCuts]        = varcuts;
  ++fNActiveCuts;

}


//______________________________________________
Bool_t PairAnalysisVarCutsCombi::IsSelected(TObject* track)
{
  //
  // perform VarCutsCombi cuts
  //

  //reset
  fSelectedCutsMask=0;
  SetSelected(kFALSE);

  if (!track) return kFALSE;

  //Fill values
  Double_t values[PairAnalysisVarManager::kNMaxValuesMC];
  PairAnalysisVarManager::SetFillMap(fUsedVars);
  PairAnalysisVarManager::Fill(track,values);

  // loop overe all cuts
  for (Int_t iCut=0; iCut<fNActiveCuts; ++iCut){
    Int_t cut=fActiveCuts[iCut];
    SETBIT(fSelectedCutsMask,iCut);

    // check the range(s) where pid cuts should be applied if any set
    if ( fVarCuts[iCut] ) {
      if ( !fVarCuts[iCut]->IsSelected(track) ) continue;
    }

    // standard var cuts
    if ( !fFormulaCut[iCut] &&
	 ((values[cut]<fCutMin[iCut]) || (values[cut]>fCutMax[iCut]))^fCutExclude[iCut] ) {
      CLRBIT(fSelectedCutsMask,iCut);
    }
    else if(fFormulaCut[iCut]){
      // formula cut
      TFormula *form = static_cast<TFormula*>(fFormulaCut[iCut]);
      Double_t val   = PairAnalysisHelper::EvalFormula(form,values);
      if ( ((val<fCutMin[iCut]) || (val>fCutMax[iCut]))^fCutExclude[iCut] ) {
	CLRBIT(fSelectedCutsMask,iCut);
      }
    }

    // cut type and intermediate decision
    if ( fCutType==kAll && !TESTBIT(fSelectedCutsMask,iCut) ) return kFALSE;

  }

  // cut type and final decision
  Bool_t isSelected=(fSelectedCutsMask==fActiveCutsMask);
  if ( fCutType==kAny ) isSelected=(fSelectedCutsMask>0);
  SetSelected(isSelected);
  return isSelected;

}

//______________________________________________
/*
void PairAnalysisVarCutsCombi::SetDefaults(Int_t def){
  //
  // initialise default pid strategies
  //
  return;

}
*/

//________________________________________________________________________
void PairAnalysisVarCutsCombi::Print(const Option_t* /*option*/) const
{
  //
  // Print cuts and the range
  //
  printf("cut ranges for '%s'\n",GetTitle());
  if (fCutType==kAll){
    printf("All Cuts have to be fulfilled\n");
  } else {
    printf("Any Cut has to be fulfilled\n");
  }
  for (Int_t iCut=0; iCut<fNActiveCuts; ++iCut){
    Int_t cut=(Int_t)fActiveCuts[iCut];
    Bool_t inverse=fCutExclude[iCut];
    Bool_t fcut   =fFormulaCut[iCut];
    Bool_t range  =fVarCuts[iCut];

    if(!fcut) {
      // standard cut
      if (!inverse){
	printf("Cut %02d: %f < %s < %f\n", iCut,
	       fCutMin[iCut], PairAnalysisVarManager::GetValueName((Int_t)cut), fCutMax[iCut]);
      } else {
	printf("Cut %02d: !(%f < %s < %f)\n", iCut,
	       fCutMin[iCut], PairAnalysisVarManager::GetValueName((Int_t)cut), fCutMax[iCut]);
      }
    }
    else if(fcut){
      // variable defined by a formula
      TFormula *form = fFormulaCut[iCut];
      TString tit(form->GetExpFormula());
      // replace parameter variables with names labels
      for(Int_t j=0;j<form->GetNpar();j++) tit.ReplaceAll(Form("[%d]",j),form->GetParName(j));
      if (!inverse) printf("Cut %02d: %f < %s < %f\n", iCut, fCutMin[iCut], tit.Data(), fCutMax[iCut]);
      else          printf("Cut %02d: !(%f < %s < %f)\n", iCut, fCutMin[iCut], tit.Data(), fCutMax[iCut]);
    }
    else
      printf("cut class not found\n");

    if(range) {
      printf("\t for the following conditions:\n");
      fVarCuts[iCut]->Print();
    }
  }

}



