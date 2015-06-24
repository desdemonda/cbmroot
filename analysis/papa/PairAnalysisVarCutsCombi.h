#ifndef PAIRANALYSISVARCUTSCOMBI_H
#define PAIRANALYSISVARCUTSCOMBI_H
//#############################################################
//#                                                           # 
//#         Class PairAnalysisVarCutsCombi                     #
//#                                                           #
//#  Authors:                                                 #
//#   Julian    Book,     Uni Ffm / Julian.Book@cern.ch       #
//#                                                           #
//#############################################################

#include <TBits.h>
#include <TFormula.h>
#include <TString.h>

#include <AnalysisCuts.h>

class TList;
class PairAnalysisVarManager;
class PairAnalysisVarCuts;

class PairAnalysisVarCutsCombi : public AnalysisCuts {
public:
  // Whether all cut criteria have to be fulfilled of just any
  enum CutType { kAll=0, kAny };

  PairAnalysisVarCutsCombi();
  PairAnalysisVarCutsCombi(const char*name, const char* title);

  virtual ~PairAnalysisVarCutsCombi();

  void AddCut(PairAnalysisVarManager::ValueTypes type,
	      Double_t min, Double_t max, Bool_t exclude,
	      PairAnalysisVarManager::ValueTypes typeR,
	      Double_t rmin, Double_t rmax, Bool_t excludeRange=kFALSE);

  void AddCut(PairAnalysisVarManager::ValueTypes type,
	      Double_t min, Double_t max, Bool_t exclude,
	      const char *formulaR,
	      Double_t rmin, Double_t rmax, Bool_t excludeRange=kFALSE);

  void AddCut(const char *formula,
	      Double_t min, Double_t max, Bool_t exclude,
	      PairAnalysisVarManager::ValueTypes typeR,
	      Double_t rmin, Double_t rmax, Bool_t excludeRange=kFALSE);

  void AddCut(const char *formula,
	      Double_t min, Double_t max, Bool_t exclude,
	      const char *formulaR,
	      Double_t rmin, Double_t rmax, Bool_t excludeRange=kFALSE);

  // main/final function
  void AddCut(const char *formula,
	      Double_t min, Double_t max, Bool_t exclude=kFALSE,
	      PairAnalysisVarCuts *varcuts=0x0 );

  void AddCut(PairAnalysisVarManager::ValueTypes type,
	      Double_t min, Double_t max, Bool_t exclude=kFALSE,
	      PairAnalysisVarCuts *varcuts=0x0 );

  // setters
  void    SetCutType(CutType type)         { fCutType=type;    }
  //  void SetDefaults(Int_t def);

  // getters
  CutType GetCutType()      const { return fCutType;      }
  Int_t GetNCuts() {                return fNActiveCuts;  }

  //
  //Analysis cuts interface
  //const
  virtual Bool_t IsSelected(TObject* track);
  virtual Bool_t IsSelected(TList*   /* list */ ) {return kFALSE;}

  //
  // Cut information
  //
  virtual UInt_t GetSelectedCutsMask() const { return fSelectedCutsMask; }

  virtual void Print(const Option_t* option = "") const;


private:
  enum {kNmaxCuts=30};

  TBits     *fUsedVars;             // list of used variables
  UShort_t  fActiveCuts[kNmaxCuts];  // list of activated cuts
  UShort_t  fNActiveCuts;                      // number of acive cuts
  UInt_t    fActiveCutsMask;        // mask of active cuts

  UInt_t   fSelectedCutsMask;       // Maks of selected cuts, is available after calling IsSelected
  CutType  fCutType;                // type of the cut: any, all

  Double_t  fCutMin[kNmaxCuts];      // lower cut limit
  Double_t  fCutMax[kNmaxCuts];      // upper cut limit
  Bool_t    fCutExclude[kNmaxCuts];  // use as exclusion band
  TFormula *fFormulaCut[kNmaxCuts];  // formula cuts

  PairAnalysisVarCuts *fVarCuts[kNmaxCuts];     // varcuts

  PairAnalysisVarCutsCombi(const PairAnalysisVarCutsCombi &c);
  PairAnalysisVarCutsCombi &operator=(const PairAnalysisVarCutsCombi &c);

  ClassDef(PairAnalysisVarCutsCombi,1)         // PairAnalysis VarCutsCombi
};

#endif
