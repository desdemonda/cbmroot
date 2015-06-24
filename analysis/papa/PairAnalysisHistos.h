#ifndef PAIRANALYSISHISTOS_H
#define PAIRANALYSISHISTOS_H
/* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

///////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       //
// Generic Histogram container with support for groups and filling of groups by passing  //
// a vector of data                                                                      //
//                                                                                       //
// Authors:                                                                              //
//   Julian Book   <Julian.Book@cern.ch>                                                 //
//                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#ifdef __APPLE__
#ifdef OLD_MAC
#include <stdint.h>
#else
#include <_types/_uint8_t.h>
#include <_types/_uint16_t.h>
#include <_types/_uint32_t.h>
#include <_types/_uint64_t.h>
#endif  
#else
#include <stdint.h>
#endif

#include <Rtypes.h>

#include <TNamed.h>
// #include <TCollection.h>
#include <THashList.h>
#include <TVectorD.h> //Dfwd
#include <THnBase.h>
#include <TBits.h>
#include <TFormula.h>
#include <TH1.h> //new
#include <TList.h> //new

#include "PairAnalysisHelper.h"
#include "PairAnalysisVarManager.h"

class TH1;
class TString;
class TList;
// class TVectorT<double>;

class PairAnalysisHistos : public TNamed {
public:

  PairAnalysisHistos();
  PairAnalysisHistos(const char* name, const char* title);
  virtual ~PairAnalysisHistos();

  enum {kNoAutoFill=1000000000, kNoProfile=999, kNoWeights=998, kNone=997};

  // functions for object creation
  TBits *GetUsedVars()  const { return fUsedVars; }
  void SetReservedWords(const char* words);
  void AddClass(        const char* histClass);
  void UserHistogram(   const char* histClass, TObject* hist);
  static void AdaptNameTitle(TH1 *hist, const char* histClass);
  static void StoreVariables(TObject *obj, UInt_t valType[20]);
  static void StoreVariables(TH1 *obj,     UInt_t valType[20]);
  static void StoreVariables(THnBase *obj, UInt_t valType[20]);

  void UserHistogram(const char* histClass, Int_t ndim, TObjArray *limits, UInt_t *vars, UInt_t valTypeW=kNoWeights);
  void AddSparse(   const char* histClass, Int_t ndim, TObjArray *limits, UInt_t *vars, UInt_t valTypeW=kNoWeights);

  // templates
  template <typename valX, typename valY, typename valZ, typename valP, typename valW>
    TString UserObject(const char* histClass, const char *name, const char* title,
		    const TVectorD * const binsX, valX valTypeX,
		    const TVectorD * const binsY, valY valTypeY,
		    const TVectorD * const binsZ, valZ valTypeZ,
		    valP valTypeP,
		    valW valTypeW,
		    TString option  );

  // 1D
  template <typename valX, typename valW>
    TString AddHistogram(const char* histClass, const TVectorD * const binsX, valX valTypeX, valW valTypeW)
  {  return UserObject(histClass,"","", binsX,valTypeX, 0x0,(UInt_t)kNone, 0x0,(UInt_t)kNone, (UInt_t)kNoProfile, valTypeW,""); }

  template <typename valX>
    TString AddHistogram(const char* histClass, const TVectorD * const binsX, valX valTypeX)
    { return AddHistogram(histClass,binsX,valTypeX,(UInt_t)kNoWeights); }

  template <typename valX, typename valP, typename valW>
    TString AddProfile(const char* histClass, const TVectorD * const binsX, valX valTypeX,
		    valP valTypeP, TString option, valW valTypeW)
  {    return UserObject(histClass,"","", binsX,valTypeX, 0x0,valTypeP, 0x0,(UInt_t)kNone, valTypeP, valTypeW,option); }

  template <typename valX, typename valP>
    TString AddProfile(const char* histClass, const TVectorD * const binsX, valX valTypeX, valP valTypeP, TString option)
  { return AddProfile(histClass,binsX,valTypeX,valTypeP,option,(UInt_t)kNoWeights); }

  // 2D
  template <typename valX, typename valY, typename valW>
    TString AddHistogram(const char* histClass,
		      const TVectorD * const binsX, valX valTypeX,
		      const TVectorD * const binsY, valY valTypeY,
		      valW valTypeW)
  { return UserObject(histClass,"","", binsX,valTypeX, binsY,valTypeY, 0x0,(UInt_t)kNone, (UInt_t)kNoProfile, valTypeW,"");  }

  template <typename valX, typename valY>
    TString AddHistogram(const char* histClass,
		      const TVectorD * const binsX, valX valTypeX,
		      const TVectorD * const binsY, valY valTypeY)
  {    return AddHistogram(histClass,binsX,valTypeX,binsY,valTypeY,(UInt_t)kNoWeights);  }

  template <typename valX, typename valY, typename valP, typename valW>
    TString AddProfile(const char* histClass,
		    const TVectorD * const binsX, valX valTypeX,
		    const TVectorD * const binsY, valY valTypeY,
		    valP valTypeP, TString option,
		    valW valTypeW)
  { return UserObject(histClass,"","", binsX,valTypeX, binsY,valTypeY, 0x0,valTypeP, valTypeP,valTypeW,option);  }

  template <typename valX, typename valY, typename valP>
    TString AddProfile(const char* histClass,
		    const TVectorD * const binsX, valX valTypeX,
		    const TVectorD * const binsY, valY valTypeY,
		    valP valTypeP, TString option)
  {    return AddProfile(histClass,binsX,valTypeX,binsY,valTypeY,valTypeP,option,(UInt_t)kNoWeights);  }

  // 3D
  template <typename valX, typename valY, typename valZ, typename valW>
    TString AddHistogram(const char* histClass, const TVectorD * const binsX, valX valTypeX,
		      const TVectorD * const binsY, valY valTypeY, const TVectorD * const binsZ, valZ valTypeZ,
		      valW valTypeW )
  { return UserObject(histClass,"","", binsX,valTypeX, binsY,valTypeY, binsZ,valTypeZ, (UInt_t)kNoProfile,valTypeW,"");  }

  template <typename valX, typename valY, typename valZ>
    TString AddHistogram(const char* histClass, const TVectorD * const binsX, valX valTypeX,
		      const TVectorD * const binsY, valY valTypeY, const TVectorD * const binsZ, valZ valTypeZ)
  {    return AddHistogram(histClass,binsX,valTypeX,binsY,valTypeY,binsZ,valTypeZ,(UInt_t)kNoWeights);  }

  //profs
  template <typename valX, typename valY, typename valZ, typename valP, typename valW>
    TString AddProfile(const char* histClass,
		    const TVectorD * const binsX, valX valTypeX,
		    const TVectorD * const binsY, valY valTypeY,
		    const TVectorD * const binsZ, valZ valTypeZ,
		    valP valTypeP, TString option,valW valTypeW )
  { return UserObject(histClass,"","", binsX,valTypeX, binsY,valTypeY, binsZ,valTypeZ, valTypeP, valTypeW,option);  }

  template <typename valX, typename valY, typename valZ, typename valP>
    TString AddProfile(const char* histClass,
		    const TVectorD * const binsX, valX valTypeX,
		    const TVectorD * const binsY, valY valTypeY,
		    const TVectorD * const binsZ, valZ valTypeZ,
		    valP valTypeP, TString option)
  { return AddProfile(histClass,binsX,valTypeX,binsY,valTypeY,binsZ,valTypeZ, valTypeP,option, (UInt_t)kNoWeights);  }

  // functions to fill objects
  void FillClass(const char* histClass, const Double_t *values);
  static void FillValues(TObject *obj,  const Double_t *values);
  static void FillValues(TH1 *obj,      const Double_t *values);
  static void FillValues(THnBase *obj,  const Double_t *values);

  // functions to get and draw histograms
  void ReadFromFile(const char* file="histos.root", const char *task="", const char *config="");
  void DumpToFile(const char* file="histos.root");
  void SetList(TList * const list)          { fList=list; }
  TList *GetList()                    const { return fList; }
  Bool_t SetCutClass(const char* cutClass);
  Bool_t HasHistClass(TString histClass) const {return fHistoList.FindObject(histClass.Data()); }
  void SetHistogramList(THashList &list, Bool_t setOwner=kTRUE);
  void ResetHistogramList()                 { fHistoList.Clear(); }
  const THashList* GetHistogramList() const { return &fHistoList; }

  TObject* GetHist(const char* histClass, const char* name) const;
  TH1* GetHistogram(const char* histClass, const char* name) const;
  TObject* GetHist(const char* cutClass, const char* histClass, const char* name) const;
  TH1* GetHistogram(const char* cutClass, const char* histClass, const char* name) const;

  virtual void Print(const Option_t* option = "") const;
  virtual void Draw(const Option_t* option = "");
  virtual void DrawSame(TString histName, const Option_t *opt="leg can");

  static Int_t GetPrecision(Double_t value);

protected:
  THashList fHistoList;             //-> list of histograms


private:

  TFormula *GetFormula(const char *name, const char* formula);
  TH1 *GetTHist(const char* histClass, const char *name, const char* title,
		const TVectorD * const binsX, const TVectorD * const binsY, const TVectorD * const binsZ);
  TH1 *GetTProf(const char* histClass, const char *name, const char* title,
		const TVectorD * const binsX, const TVectorD * const binsY, const TVectorD * const binsZ,
		TString option="i");

  void FillVarArray(TObject *obj, UInt_t *valType);

  //  THashList fHistoList;             //-> list of histograms
  TList    *fList;                  //! List of list of histograms
  TBits     *fUsedVars;            // list of used variables

  TString *fReservedWords;          //! list of reserved words
  void UserHistogramReservedWords(const char* histClass, const TObject *hist);
  void FillClass(THashTable *classTable, Int_t nValues, Double_t *values);

  void PrintPDF(Option_t* opt);
  void PrintStructure() const;

  Bool_t IsHistogramOk(const char* classTable, const char* name);


  PairAnalysisHistos(const PairAnalysisHistos &hist);
  PairAnalysisHistos& operator = (const PairAnalysisHistos &hist);

  ClassDef(PairAnalysisHistos,1)
};

template <typename valX, typename valY, typename valZ, typename valP, typename valW>
  TString PairAnalysisHistos::UserObject(const char* histClass,
				       const char *name, const char* title,
				       const TVectorD * const binsX, valX valTypeX,
				       const TVectorD * const binsY, valY valTypeY,
				       const TVectorD * const binsZ, valZ valTypeZ,
				       valP valTypeP,
				       valW valTypeW,
				       TString option  )
{
  //
  // main function to setup the histogram with given variables, binning and dimensions
  //
  TH1 *hist = 0x0;
  TString err="err";
  //profile or histogram
  if(typeid(valTypeP)==typeid(UInt_t) && (uintptr_t)valTypeP==kNoProfile)
                    hist = GetTHist(histClass,name,title,binsX,binsY,binsZ);
  else              hist = GetTProf(histClass,name,title,binsX,binsY,binsZ,option);
  if(!hist) { delete binsX; return err; }

  // add formulas to the histograms list of functions
  // and store variales in uniqueIDs
  UInt_t valType[20] = {0};
  TString func="";  func+=valTypeX;
  if(!func.Atoi())  hist->GetListOfFunctions()->Add( GetFormula("xFormula",func) );
  else              valType[0]=func.Atoi();

  func="";  func+=valTypeY;
  if(!func.Atoi())  hist->GetListOfFunctions()->Add( GetFormula("yFormula",func) );
  else              valType[1]=func.Atoi();

  func="";  func+=valTypeZ;
  if(!func.Atoi())  hist->GetListOfFunctions()->Add( GetFormula("zFormula",func) );
  else              valType[2]=func.Atoi();

  func="";  func+=valTypeP;
  if(!func.Atoi())  hist->GetListOfFunctions()->Add( GetFormula("pFormula",func) );
  else              valType[3]=func.Atoi();

  TString func2="";  func2+=valTypeX;
  func="";  func+=valTypeW;
  if(!func.Atoi())  hist->GetListOfFunctions()->Add( GetFormula("wFormula",func) );
  else if(func2.Atoi()!=kNoWeights) {
    hist->SetUniqueID(func.Atoi()); // store weighting variable
    fUsedVars->SetBitNumber(func.Atoi(),kTRUE);
  }

  // store variables and their usage
  StoreVariables(hist, valType);
  for(Int_t i=0; i<4; i++)            fUsedVars->SetBitNumber(valType[i],kTRUE);

  // adapt the name and title of the histogram in case they are empty
  AdaptNameTitle(hist, histClass);

  // save histname
  TString key = hist->GetName();
  // set pdg code labels
  /*
  if(hist->GetDimension()==1) {
    func=""; func+=valTypeX;
    if(func.Contains("PdgCode") || 
       func.Atoi()==PairAnalysisVarManager::kPdgCode ||
       func.Atoi()==PairAnalysisVarManager::kPdgCodeMother ||
       func.Atoi()==PairAnalysisVarManager::kPdgCodeGrandMother ||
       ) PairAnalysisHelper::SetPDGBinLabels(hist);
  }
  */

  Bool_t isReserved=fReservedWords->Contains(histClass);
  if(func2.Atoi() && func2.Atoi()==kNoAutoFill) hist->SetUniqueID(func2.Atoi());
  if (isReserved)
    UserHistogramReservedWords(histClass, hist);
  else
    UserHistogram(histClass, hist);

  delete binsX;
  delete binsY;
  delete binsZ;

  return key;

}

#endif

