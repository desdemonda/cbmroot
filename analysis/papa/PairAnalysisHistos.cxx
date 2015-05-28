/*************************************************************************
 * Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 **************************************************************************/

///////////////////////////////////////////////////////////////////////////
//
// Generic Histogram container with support for groups and filling of groups
// by passing a vector of data
//
// Authors:
//   Julian Book   <Julian.Book@cern.ch>
/*

  TOADD: reserved words, MC signals

  Histograms such as THxF, TProfile(2D,3D) and n-dimensonal objects such as
  (THn, THnSparse) can be added via the various functions:
  - UserHistogram
  - UserProfile + different error option (Mean,Rms,.. see: TProfile::BuildOptions(..))
  - UserSparse
  In addition histograms can be filled with weights.

  Different kind of binnings can be passed (linear, arbitrary, logarithmic)
  either via the PairAnalysisHelper functionalities (recommended):
  - PairAnalysisHelper::MakeLinearBinning(nbins,low,high)
  - PairAnalysisHelper::MakeLogBinning(nbins,low,high)
  - PairAnalysisHelper::MakeArbitraryBinning("1.,4.,10.,..")
  or via one of the provided functions and switchers therein.

  The 'name' and 'title;titleX;titleY;...' arguments of the objects are
  recommended to be left free (i.e. ""), they are only kept for backward
  compatibility. Then all axis labels, units and names and object names
  are set in a consistent way and it is ensured that they are unique.

  Variables are added via the enumerator of PairAnalysisVarManager. Any
  computation of variables (max=10) are implemented according to TFormula
  using the TMath functions and the passage of an array of variables, e.g.:

  UInt_t vars[]={ PairAnalysisVarManager::kRefMult, PairAnalysisVarManager::kRefMultTPConly };
  histos->UserHistogram("Event","test","", PairAnalysisHelper::MakeLinBinning(100,1.e-1,1.e+1),
  "[0]*[1]/TMath::Pi()+TMath::Cos([1])", varEpm);



*/
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <typeinfo>

#include <TH1.h>
#include <TH1F.h>
#include <TH2.h>
#include <TH3.h>
#include <THnBase.h>
#include <THn.h>
#include <THnSparse.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TProfile3D.h>
#include <TCollection.h>
#include <THashList.h>
#include <TString.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TFile.h>
#include <TError.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TROOT.h>
#include <TLegend.h>
#include <TKey.h>
#include <TAxis.h>
#include <TVirtualPS.h>
#include <TVectorD.h>
#include <TFormula.h>

#include "PairAnalysisHelper.h"
#include "PairAnalysisSignalMC.h"
#include "PairAnalysisStyler.h"
#include "PairAnalysisVarManager.h"
#include "PairAnalysisHistos.h"

ClassImp(PairAnalysisHistos)


PairAnalysisHistos::PairAnalysisHistos() :
//   TCollection(),
TNamed("PairAnalysisHistos","PairAnalysis Histogram Container"),
  fHistoList(),
  fList(0x0),
  fUsedVars(new TBits(PairAnalysisVarManager::kNMaxValues)),
  fReservedWords(new TString)
{
  //
  // Default constructor
  //
  fHistoList.SetOwner(kTRUE);
  fHistoList.SetName("PairAnalysis_Histos");
  PairAnalysisVarManager::InitFormulas();
  PairAnalysisStyler::LoadStyle();
}

//_____________________________________________________________________________
PairAnalysisHistos::PairAnalysisHistos(const char* name, const char* title) :
  //   TCollection(),
  TNamed(name, title),
  fHistoList(),
  fList(0x0),
  fUsedVars(new TBits(PairAnalysisVarManager::kNMaxValues)),
  fReservedWords(new TString)
{
  //
  // TNamed constructor
  //
  fHistoList.SetOwner(kTRUE);
  fHistoList.SetName(name);
  PairAnalysisVarManager::InitFormulas();
  PairAnalysisStyler::LoadStyle();
}

//_____________________________________________________________________________
PairAnalysisHistos::~PairAnalysisHistos()
{
  //
  // Destructor
  //
  fHistoList.Clear();
  if (fUsedVars) delete fUsedVars;
  if (fList) fList->Clear();
  delete fReservedWords;
}

//_____________________________________________________________________________
void PairAnalysisHistos::UserHistogram(const char* histClass, Int_t ndim, TObjArray *limits, UInt_t *vars, UInt_t valTypeW)
{
  //
  // Histogram creation n>3 dimension only with non-linear binning
  //

  Bool_t isOk=kTRUE;
  isOk&=(ndim<21 && ndim>3);
  if(!isOk) { Warning("UserHistogram","Array sizes should be between 3 and 20. Not adding Histogram to '%s'.", histClass); return; }
  isOk&=(ndim==limits->GetEntriesFast());
  if(!isOk) return;

  // set automatic histo name
  TString name;
  for(Int_t iv=0; iv < ndim; iv++)
    name+=Form("%s_",PairAnalysisVarManager::GetValueName(vars[iv]));
  name.Resize(name.Length()-1);

  isOk&=IsHistogramOk(histClass,name);

  THnD *hist;
  Int_t bins[ndim];
  if (isOk) {
    // get number of bins
    for(Int_t idim=0 ;idim<ndim; idim++) {
      TVectorD *vec = (TVectorD*) limits->At(idim);
      bins[idim]=vec->GetNrows()-1;
    }

    hist=new THnD(name.Data(),"", ndim, bins, 0x0, 0x0);

    // set binning
    for(Int_t idim=0 ;idim<ndim; idim++) {
      TVectorD *vec = (TVectorD*) limits->At(idim);
      hist->SetBinEdges(idim,vec->GetMatrixArray());
    }

    // store variales in axes
    StoreVariables(hist, vars);
    hist->SetUniqueID(valTypeW); // store weighting variable

    // store which variables are used
    for(Int_t i=0; i<20; i++)   fUsedVars->SetBitNumber(vars[i],kTRUE);
    fUsedVars->SetBitNumber(valTypeW,kTRUE);

    Bool_t isReserved=fReservedWords->Contains(histClass);
    if (isReserved)
      UserHistogramReservedWords(histClass, hist);
    else
      UserHistogram(histClass, hist);

  }
}

//_____________________________________________________________________________
void PairAnalysisHistos::AddSparse(const char* histClass, Int_t ndim, TObjArray *limits, UInt_t *vars, UInt_t valTypeW)
{
  //
  // THnSparse creation with non-linear binning
  //

  Bool_t isOk=kTRUE;
  isOk&=(ndim==limits->GetEntriesFast());
  if(!isOk) return;

  // set automatic histo name
  TString name;
  for(Int_t iv=0; iv < ndim; iv++)
    name+=Form("%s_",PairAnalysisVarManager::GetValueName(vars[iv]));
  name.Resize(name.Length()-1);

  isOk&=IsHistogramOk(histClass,name);

  THnSparseD *hist;
  Int_t bins[ndim];
  if (isOk) {
    // get number of bins
    for(Int_t idim=0 ;idim<ndim; idim++) {
      TVectorD *vec = (TVectorD*) limits->At(idim);
      bins[idim]=vec->GetNrows()-1;
    }

    hist=new THnSparseD(name.Data(),"", ndim, bins, 0x0, 0x0);

    // set binning
    for(Int_t idim=0 ;idim<ndim; idim++) {
      TVectorD *vec = (TVectorD*) limits->At(idim);
      hist->SetBinEdges(idim,vec->GetMatrixArray());
    }

    // store variales in axes
    StoreVariables(hist, vars);
    hist->SetUniqueID(valTypeW); // store weighting variable

    // store which variables are used
    for(Int_t i=0; i<20; i++)   fUsedVars->SetBitNumber(vars[i],kTRUE);
    fUsedVars->SetBitNumber(valTypeW,kTRUE);

    Bool_t isReserved=fReservedWords->Contains(histClass);
    if (isReserved)
      UserHistogramReservedWords(histClass, hist);
    else
      UserHistogram(histClass, hist);

  }
}

//_____________________________________________________________________________
void PairAnalysisHistos::UserHistogram(const char* histClass, TObject* hist)
{
  //
  // Add any type of user histogram
  //

  //special case for the calss Pair. where histograms will be created for all pair classes
  Bool_t isReserved=fReservedWords->Contains(histClass);
  if (isReserved) {
    UserHistogramReservedWords(histClass, hist);
    return;
  }

  if (!IsHistogramOk(histClass,hist->GetName())) return;
  THashList *classTable=(THashList*)fHistoList.FindObject(histClass);
  //  hist->SetDirectory(0);

  // store variables axis
  UInt_t valType[20] = {0};
  // extract variables from axis
  FillVarArray(hist, valType);
  // change to mc truth variables if available
  TString hclass=histClass;
  if(hclass.Contains("MCtruth")) {
    for(Int_t i=0;i<2;i++) {
      //	Printf("SWITCH TO MC: before: %d %s ---->",valType[i],PairAnalysisVarManager::GetValueName(valType[i]));
      valType[i] = PairAnalysisVarManager::GetValueTypeMC(valType[i]);
      // if theres no corresponding MCtruth variable, skip adding this histogram
      if(valType[i] < PairAnalysisVarManager::kNMaxValues && valType[i]>0) return;
      // request filling of mc variable
      fUsedVars->SetBitNumber(valType[i],kTRUE);
      //	Printf("after: %d %s",valType[i],PairAnalysisVarManager::GetValueName(valType[i]));
    }
    StoreVariables(hist, valType);
    hist->SetUniqueID(valType[19]); // store weighting variable
    // check for formulas
    TIter next(((TH1*)hist)->GetListOfFunctions());
    TFormula *f=0;
    while ( (f=dynamic_cast<TFormula*>(next()) ) ) {
      for(Int_t i=0; i<f->GetNpar(); i++) {
	Int_t parMC=PairAnalysisVarManager::GetValueTypeMC(f->GetParameter(i));
	// if theres none corresponding MCtruth variable, skip adding this histogram
	if(parMC < PairAnalysisVarManager::kNMaxValues) return;
	f->SetParameter(   i, parMC );
	f->SetParName(     i, PairAnalysisVarManager::GetValueName(parMC)   );
	fUsedVars->SetBitNumber(parMC,kTRUE);
      }
    }
    // change histogram key according to mctruth information
    //      Printf("SWITCH TO MC NAME: before: %s ---->",hist->GetName());
    AdaptNameTitle((TH1*)hist, histClass);
    //      Printf("after:  %s",hist->GetName());
  }
  else {
    StoreVariables(hist, valType);
    hist->SetUniqueID(valType[19]); // store weighting variable
  }

  classTable->Add(hist);
  return;
}

//_____________________________________________________________________________
TH1 *PairAnalysisHistos::GetTHist(const char* histClass, const char *name, const char* title,
				const TVectorD * const binsX, const TVectorD * const binsY, const TVectorD * const binsZ
				)
{
  //
  // retrieve n-dimensional Hist depending on arguments
  //
  Bool_t isOk=kTRUE;
  isOk&=IsHistogramOk(histClass,name);
  isOk&=(binsX!=0x0);
  if (!isOk) return 0x0;
  if(!binsY)      return (new TH1F(name,title,binsX->GetNrows()-1,binsX->GetMatrixArray()) );
  else if(!binsZ) return (new TH2F(name,title,binsX->GetNrows()-1,binsX->GetMatrixArray(),
				   binsY->GetNrows()-1,binsY->GetMatrixArray())            );
  else            return (new TH3F(name,title,binsX->GetNrows()-1,binsX->GetMatrixArray(),
				   binsY->GetNrows()-1,binsY->GetMatrixArray(),
				   binsZ->GetNrows()-1,binsZ->GetMatrixArray())            );
}

//_____________________________________________________________________________
TH1 *PairAnalysisHistos::GetTProf(const char* histClass, const char *name, const char* title,
				const TVectorD * const binsX, const TVectorD * const binsY, const TVectorD * const binsZ,
				TString option)
{
  //
  // retrieve n-dimensional profile histogram with error options depending on arguments
  //
  Bool_t isOk=kTRUE;
  isOk&=IsHistogramOk(histClass,name);
  isOk&=(binsX!=0x0);
  if (!isOk) return 0x0;
  // parse error option
  TString opt=""; Double_t pmin=0., pmax=0.;
  if(!option.IsNull()) {
    TObjArray *arr=option.Tokenize(";");
    arr->SetOwner();
    opt=((TObjString*)arr->At(0))->GetString();
    if(arr->GetEntriesFast()>1) pmin=(((TObjString*)arr->At(1))->GetString()).Atof();
    if(arr->GetEntriesFast()>2) pmax=(((TObjString*)arr->At(2))->GetString()).Atof();
    delete arr;
  }
  // build profile with error options and return it
  TH1 *prof=0x0;
  if(!binsY)      return (new TProfile(name,title,binsX->GetNrows()-1,binsX->GetMatrixArray(),pmin,pmax,opt.Data()) );
  else if(!binsZ) {
    prof = new TProfile2D(name,title,binsX->GetNrows()-1,binsX->GetMatrixArray(),
			  binsY->GetNrows()-1,binsY->GetMatrixArray());
    ((TProfile2D*)prof)->BuildOptions(pmin,pmax,opt.Data());
    return prof;
  }
  else            {
    prof = new TProfile3D(name,title,binsX->GetNrows()-1,binsX->GetMatrixArray(),
			  binsY->GetNrows()-1,binsY->GetMatrixArray(),
			  binsZ->GetNrows()-1,binsZ->GetMatrixArray());
    ((TProfile3D*)prof)->BuildOptions(pmin,pmax,opt.Data());
    return prof;
  }

}

//_____________________________________________________________________________
TFormula *PairAnalysisHistos::GetFormula(const char *name, const char* formula)
{
  //
  // build a TFormula object
  //
  TFormula *form = new TFormula(name,formula);
  // compile function
  if(form->Compile()) return 0x0;
  //set parameter/variable identifier
  for(Int_t i=0; i<form->GetNpar(); i++) {
    form->SetParName(  i, PairAnalysisVarManager::GetValueName(form->GetParameter(i)) );
    fUsedVars->SetBitNumber((Int_t)form->GetParameter(i),kTRUE);
  }
  return form;

}

//_____________________________________________________________________________
void PairAnalysisHistos::AddClass(const char* histClass)
{
  //
  // Add a class of histograms
  // Several classes can be added by separating them by a ';' e.g. 'class1;class2;class3'
  //
  TString hists(histClass);
  TObjArray *arr=hists.Tokenize(";");
  TIter next(arr);
  TObject *o=0;
  while ( (o=next()) ){
    if (fHistoList.FindObject(o->GetName())){
      Warning("AddClass","Cannot create class '%s' it already exists.",histClass);
      continue;
    }
    if (fReservedWords->Contains(o->GetName())){
      Error("AddClass","Pair is a reserved word, please use another name");
      continue;
    }
    THashList *table=new THashList;
    table->SetOwner(kTRUE);
    table->SetName(o->GetName());
    fHistoList.Add(table);
  }
  delete arr;
}

//_____________________________________________________________________________
void PairAnalysisHistos::FillClass(const char* histClass, const Double_t *values)
{
  //
  // Fill class 'histClass' (by name)
  //

  THashList *classTable=(THashList*)fHistoList.FindObject(histClass);
  if (!classTable){
    Warning("FillClass","Cannot fill class '%s' its not defined.",histClass);
    return;
  }

  TIter nextHist(classTable);
  TObject *obj=0;
  while ( (obj=(TObject*)nextHist()) )  FillValues(obj, values);

  return;
}

//_____________________________________________________________________________
void PairAnalysisHistos::UserHistogramReservedWords(const char* histClass, const TObject *hist)
{
  //
  // Creation of histogram for all pair or track types
  //
  TString title(hist->GetTitle());
  // Same Event Like Sign
  TIter nextClass(&fHistoList);
  THashList *l=0;
  while ( (l=static_cast<THashList*>(nextClass())) ){
    TString name(l->GetName());
    if (name.Contains(histClass)){
      TObject *h=hist->Clone();
      // Tobject has no function SetDirectory, didn't we need this???
      //      h->SetDirectory(0);
      ((TH1*)h)->SetTitle(Form("%s %s",title.Data(),l->GetName()));

      UserHistogram(l->GetName(),h);
    }
  }
  delete hist;

  return;
}

//_____________________________________________________________________________
void PairAnalysisHistos::DumpToFile(const char* file)
{
  //
  // Dump the histogram list to a newly created root file
  //
  TFile f(file,"recreate");
  fHistoList.Write(fHistoList.GetName(),TObject::kSingleKey);
  f.Close();
}

//_____________________________________________________________________________
TObject* PairAnalysisHistos::GetHist(const char* histClass, const char* name) const
{
  //
  // return object 'name' in 'histClass'
  //
  THashList *classTable=(THashList*)fHistoList.FindObject(histClass);
  if (!classTable) return 0x0;
  return classTable->FindObject(name);
}

//_____________________________________________________________________________
TH1* PairAnalysisHistos::GetHistogram(const char* histClass, const char* name) const
{
  //
  // return histogram 'name' in 'histClass'
  //
  return ((TH1*) GetHist(histClass, name));
}

//_____________________________________________________________________________
TObject* PairAnalysisHistos::GetHist(const char* cutClass, const char* histClass, const char* name) const
{
  //
  // return object from list of list of histograms
  // this function is thought for retrieving histograms if a list of PairAnalysisHistos is set
  //

  if (!fList) return 0x0;
  THashList *h=dynamic_cast<THashList*>(fList->FindObject(cutClass));
  if (!h)return 0x0;
  THashList *classTable=dynamic_cast<THashList*>(h->FindObject(histClass));
  if (!classTable) return 0x0;
  return classTable->FindObject(name);
}

//_____________________________________________________________________________
TH1* PairAnalysisHistos::GetHistogram(const char* cutClass, const char* histClass, const char* name) const
{
  //
  // return histogram from list of list of histograms
  // this function is thought for retrieving histograms if a list of PairAnalysisHistos is set
  //
  return ((TH1*) GetHist(cutClass, histClass, name));
}

//_____________________________________________________________________________
void PairAnalysisHistos::Draw(const Option_t* option)
{
  //
  // Draw histograms
  //

  TString drawStr(option);
  TObjArray *arr=drawStr.Tokenize(";");
  arr->SetOwner();
  TIter nextOpt(arr);

  TString drawClasses;
  TObjString *ostr=0x0;

  TString currentOpt;
  TString testOpt;
  while ( (ostr=(TObjString*)nextOpt()) ){
    currentOpt=ostr->GetString();
    currentOpt.Remove(TString::kBoth,'\t');
    currentOpt.Remove(TString::kBoth,' ');

    testOpt="classes=";
    if ( currentOpt.Contains(testOpt.Data()) ){
      drawClasses=currentOpt(testOpt.Length(),currentOpt.Length());
    }
  }

  delete arr;
  drawStr.ToLower();
  //optionsfList
  //   Bool_t same=drawOpt.Contains("same"); //FIXME not yet implemented

  TCanvas *c=0x0;
  if (gVirtualPS) {
    if (!gPad){
      Error("Draw","When writing to a file you have to create a canvas before opening the file!!!");
      return;
    }
    c=gPad->GetCanvas();
    c->cd();
    //     c=new TCanvas;
  }

  TIter nextClass(&fHistoList);
  THashList *classTable=0;
  //   Bool_t first=kTRUE;
  while ( (classTable=(THashList*)nextClass()) ){
    //test classes option
    if (!drawClasses.IsNull() && !drawClasses.Contains(classTable->GetName())) continue;
    //optimised division
    Int_t nPads = classTable->GetEntries();
    Int_t nCols = (Int_t)TMath::Ceil( TMath::Sqrt(nPads) );
    Int_t nRows = (Int_t)TMath::Ceil( (Double_t)nPads/(Double_t)nCols );

    //create canvas
    if (!gVirtualPS){
      TString canvasName;
      canvasName.Form("c%s_%s",GetName(),classTable->GetName());
      c=(TCanvas*)gROOT->FindObject(canvasName.Data());
      if (!c) c=new TCanvas(canvasName.Data(),Form("%s: %s",GetName(),classTable->GetName()));
      c->Clear();
    } else {
      //       if (first){
      //         first=kFALSE;
      //         if (nPads>1) gVirtualPS->NewPage();
      //       } else {
      if (nPads>1) c->Clear();
      //       }
    }
    if (nCols>1||nRows>1) c->Divide(nCols,nRows);

    //loop over histograms and draw them
    TIter nextHist(classTable);
    Int_t iPad=0;
    TH1 *h=0;
    while ( (h=(TH1*)nextHist()) ){
      TString drawOpt;
      if ( (h->InheritsFrom(TH2::Class())) ) drawOpt="colz";
      if (nCols>1||nRows>1) c->cd(++iPad);
      if ( TMath::Abs(h->GetXaxis()->GetBinWidth(1)-h->GetXaxis()->GetBinWidth(2))>1e-10 ) gPad->SetLogx();
      if ( TMath::Abs(h->GetYaxis()->GetBinWidth(1)-h->GetYaxis()->GetBinWidth(2))>1e-10 ) gPad->SetLogy();
      if ( TMath::Abs(h->GetZaxis()->GetBinWidth(1)-h->GetZaxis()->GetBinWidth(2))>1e-10 ) gPad->SetLogz();
      TString histOpt=h->GetOption();
      histOpt.ToLower();
      if (histOpt.Contains("logx")) gPad->SetLogx();
      if (histOpt.Contains("logy")) gPad->SetLogy();
      if (histOpt.Contains("logz")) gPad->SetLogz();
      histOpt.ReplaceAll("logx","");
      histOpt.ReplaceAll("logy","");
      histOpt.ReplaceAll("logz","");
      h->Draw(drawOpt.Data());
    }
    if (gVirtualPS) {
      c->Update();
    }

  }
  //   if (gVirtualPS) delete c;
}

//_____________________________________________________________________________
void PairAnalysisHistos::Print(const Option_t* option) const
{
  //
  // Print classes and histograms
  //
  TString optString(option);

  if (optString.IsNull()) PrintStructure();



}

//_____________________________________________________________________________
void PairAnalysisHistos::PrintStructure() const
{
  //
  // Print classes and histograms in the class to stdout
  //
  if (!fList){
    TIter nextClass(&fHistoList);
    THashList *classTable=0;
    while ( (classTable=(THashList*)nextClass()) ){
      TIter nextHist(classTable);
      TObject *o=0;
      Printf("+ %s\n",classTable->GetName());
      while ( (o=nextHist()) )
	Printf("| ->%s\n",o->GetName());
    }
  } else {
    TIter nextCutClass(fList);
    THashList *cutClass=0x0;
    while ( (cutClass=(THashList*)nextCutClass()) ) {
      Printf("+ %s\n",cutClass->GetName());
      TIter nextClass(cutClass);
      THashList *classTable=0;
      while ( (classTable=(THashList*)nextClass()) ){
	TIter nextHist(classTable);
	TObject *o=0;
	Printf("|  + %s\n",classTable->GetName());
	while ( (o=nextHist()) )
	  Printf("|  | ->%s\n",o->GetName());
      }

    }
  }
}

//_____________________________________________________________________________
void PairAnalysisHistos::SetHistogramList(THashList &list, Bool_t setOwner/*=kTRUE*/)
{
  //
  // set histogram classes and histograms to this instance. It will take onwnership!
  //
  ResetHistogramList();
  TString name(GetName());
  if (name == "PairAnalysisHistos") SetName(list.GetName());
  TIter next(&list);
  TObject *o;
  while ( (o=next()) ){
    fHistoList.Add(o);
  }
  if (setOwner){
    list.SetOwner(kFALSE);
    fHistoList.SetOwner(kTRUE);
  } else {
    fHistoList.SetOwner(kFALSE);
  }
}

//_____________________________________________________________________________
Bool_t PairAnalysisHistos::SetCutClass(const char* cutClass)
{
  //
  // Assign histogram list according to cutClass
  //

  if (!fList) return kFALSE;
  ResetHistogramList();
  THashList *h=dynamic_cast<THashList*>(fList->FindObject(cutClass));
  if (!h) {
    Warning("SetCutClass","cutClass '%s' not found", cutClass);
    return kFALSE;
  }
  SetHistogramList(*h,kFALSE);
  return kTRUE;
}
//_____________________________________________________________________________
Bool_t PairAnalysisHistos::IsHistogramOk(const char* histClass, const char* name)
{
  //
  // check whether the histogram class exists and the histogram itself does not exist yet
  //
  Bool_t isReserved=fReservedWords->Contains(histClass);
  if (!fHistoList.FindObject(histClass)&&!isReserved){
    Warning("IsHistogramOk","Cannot create histogram. Class '%s' not defined. Please create it using AddClass before.",histClass);
    return kFALSE;
  }
  if (GetHist(histClass,name)){
    Warning("IsHistogramOk","Cannot create histogram '%s' in class '%s': It already exists!",name,histClass);
    return kFALSE;
  }
  return kTRUE;
}

// //_____________________________________________________________________________
// TIterator* PairAnalysisHistos::MakeIterator(Bool_t dir) const
// {
//   //
//   //
//   //
//   return new TListIter(&fHistoList, dir);
// }

//_____________________________________________________________________________
void PairAnalysisHistos::ReadFromFile(const char* file, const char *task, const char *config)
{
  //
  // Read histos from file
  //
  TFile f(file);
  TIter nextKey(f.GetListOfKeys());
  TKey *key=0;
  while ( (key=(TKey*)nextKey()) ){
    TString name=key->GetName();
    if(!name.Contains("PairAnalysisHistos"))    continue;
    if(!strlen(task) && !name.Contains(task)) continue;
    TObject *o=f.Get(key->GetName());
    TList *list=dynamic_cast<TList*>(o);
    if (!list) continue;
    else fList=list;
    THashList *listCfg=dynamic_cast<THashList*>(list->FindObject(config));
    if (!listCfg) continue;
    SetHistogramList(*listCfg);
    break;
  }
  f.Close();
  // load style
  PairAnalysisStyler::LoadStyle();
}

//_____________________________________________________________________________
void PairAnalysisHistos::DrawSame(TString histName, const Option_t *opt)
{
  //
  // Draw all histograms with the same name into one canvas
  // if option contains 'leg' a legend will be created with the class name as caption
  // if option contains 'can' a new canvas is created
  // if option contains 'rebin' the objects are rebinned by 2
  // if option contains 'norm' the objects are normalized to 1
  // if option contains 'logx,y,z' the axis are plotted in log
  //

  Printf("hist: %s",histName.Data());
  TString optString(opt);
  optString.ToLower();
  Bool_t optNoMC=optString.Contains("nomc");
  Bool_t optRbn=optString.Contains("rebin");
  Bool_t optLeg=optString.Contains("leg");
  Bool_t optCan=optString.Contains("can");
  Bool_t optNorm=optString.Contains("norm");
  optString.ReplaceAll("nomc","");
  optString.ReplaceAll("rebin","");
  optString.ReplaceAll("leg","");
  optString.ReplaceAll("can","");
  optString.ReplaceAll("norm","");

  TLegend *leg=0;
  TCanvas *c=0;
  if (optCan){
    c=(TCanvas*)gROOT->FindObject(Form("c%s",histName.Data()));
    if (!c) c=new TCanvas(Form("c%s",histName.Data()),Form("All '%s' histograms",histName.Data()));
    //    c->Clear();
    c->cd();
  }
  Int_t nobj = gPad->GetListOfPrimitives()->GetEntries();
  // if(nobj) {Printf("canvas has %d entries",nobj);
  //   gPad->GetListOfPrimitives()->ls();
  // }

  if (optLeg) leg=new TLegend(.75,.3,1.-gPad->GetTopMargin(),1-gPad->GetRightMargin(),GetName(),"nbNDC");
  //  else if(optLeg)      leg=(TLegend*)gPad->GetListOfPrimitives()->Last();

  // logaritmic style
  if(optString.Contains("logx")) gPad->SetLogx();
  if(optString.Contains("logy")) gPad->SetLogy();
  if(optString.Contains("logz")) gPad->SetLogz();
  optString.ReplaceAll("logx","");
  optString.ReplaceAll("logy","");
  optString.ReplaceAll("logz","");

  //  Int_t i=(nobj ? nobj-1 : 0);
  Int_t i=(nobj ? 10 : 0);
  TIter next(&fHistoList);
  THashList *classTable=0;
  Double_t max=-1e10;
  TH1 *hFirst=0x0;
  while ( (classTable=(THashList*)next()) ){
    TString iname=classTable->GetName();
    if(optNoMC) {
      TObjArray *arr = iname.Tokenize("_");
      if(arr->GetEntriesFast()>2) {
	delete arr;
	continue;
      }
      else
	delete arr;
    }

    if ( TH1 *h=(TH1*)classTable->FindObject(histName.Data()) ){
      if(iname.Contains("Hit")) Printf("class name: %s optMC %d ",iname.Data(),optNoMC);
      if (i==0) hFirst=h;
      h->UseCurrentStyle();
      h->SetTitle("");
      PairAnalysisStyler::Style(h,i);
      // set geant process labels
      // if(!histName.CompareTo("GeantId")) PairAnalysisHelper::SetGEANTBinLabels(h);
      if(optRbn)  h->Rebin();
      if(optNorm) h=h->DrawNormalized(i>0?(optString+"same").Data():optString.Data());
      else        h->Draw(i>0?(optString+"same").Data():optString.Data());

      // protection e.g. normalization not possible TProfile
      if(h && h->GetEntries()>0.) {

	// adapt legend name - remove reserved works prefixes
	iname.ReplaceAll("_"," ");
	// remove reserved words
	TObjArray *reservedWords = fReservedWords->Tokenize(":;");
	for(Int_t ir=0; ir<reservedWords->GetEntriesFast(); ir++)
	  iname.ReplaceAll( ((TObjString*)reservedWords->At(ir))->GetString(), "");
	delete reservedWords;
	// change default signal names to titles
	for(Int_t isig=0; isig<PairAnalysisSignalMC::kNSignals; isig++)
	  iname.ReplaceAll(PairAnalysisSignalMC::fgkSignals[isig][0],PairAnalysisSignalMC::fgkSignals[isig][1]);
	iname.Remove(TString::kBoth,' ');

	// modify legend option
	TString legOpt = optString+"L";
	legOpt.ReplaceAll("hist","");
	if (nobj) iname="";
	if (leg) leg->AddEntry(h,iname.Data(),legOpt.Data());

	//      if (leg) leg->AddEntry(h,classTable->GetName(),(optString+"L").Data());
	max=TMath::Max(max,h->GetMaximum());

      }
      else if(nobj&&leg) leg->AddEntry(hFirst,"","");

      ++i;

    }
  }

  if (leg){
    leg->SetFillStyle(0);
    leg->SetTextSize(0.02);
    // shift second legend if needed
    if(nobj) {
      Float_t shift=(leg->GetX2()-leg->GetX1())*leg->GetMargin();
      leg->SetX1(leg->GetX1()-shift);
      leg->SetX2(leg->GetX2()-shift);
      leg->SetY1(leg->GetY2()-(nobj-1)*.05);
    }
    else
      leg->SetY1(leg->GetY2()-i*.05);
    leg->Draw();
  }

  TIter nextObj(gPad->GetListOfPrimitives());
  TObject *obj;
  while ((obj = nextObj())) {
    if(obj->InheritsFrom(TH1::Class())) {
      //      Printf("max%f \t %f",max,static_cast<TH1*>(obj)->GetMaximum());
      static_cast<TH1*>(obj)->SetMaximum(max*1.1);
      break;
    }
  }

  /*
  if (hFirst&&(hFirst->GetYaxis()->GetXmax()<max)){
    hFirst->SetMaximum(max);
  }
  */

}

//_____________________________________________________________________________
void PairAnalysisHistos::SetReservedWords(const char* words)
{
  //
  // set reserved words
  //

  (*fReservedWords)=words;
}

//_____________________________________________________________________________
void PairAnalysisHistos::StoreVariables(TObject *obj, UInt_t valType[20])
{
  //
  //
  //
  if (!obj) return;
  if      (obj->InheritsFrom(TH1::Class()))         StoreVariables(static_cast<TH1*>(obj), valType);
  else if (obj->InheritsFrom(THnBase::Class()))     StoreVariables(static_cast<THnBase*>(obj), valType);

  return;

}


//_____________________________________________________________________________
void PairAnalysisHistos::StoreVariables(TH1 *obj, UInt_t valType[20])
{
  //
  // store variables in the axis (special for TProfile3D)
  //

  Int_t dim   = obj->GetDimension();

  // dimension correction for profiles
  if(obj->IsA() == TProfile::Class() || obj->IsA() == TProfile2D::Class() || obj->IsA() == TProfile3D::Class()) {
    dim++;
  }

  switch( dim ) {
  case 4:    obj->SetUniqueID(valType[3]); // Tprofile3D variable
  case 3:    obj->GetZaxis()->SetUniqueID(valType[2]);
  case 2:    obj->GetYaxis()->SetUniqueID(valType[1]);
  case 1:    obj->GetXaxis()->SetUniqueID(valType[0]);
  }

  return;
}

//_____________________________________________________________________________
void PairAnalysisHistos::StoreVariables(THnBase *obj, UInt_t valType[20])
{
  //
  // store variables in the axis
  //

  Int_t dim = obj->GetNdimensions();

  for(Int_t it=0; it<dim; it++) {
    obj->GetAxis(it)->SetUniqueID(valType[it]);
    obj->GetAxis(it)->SetName(Form("%s", PairAnalysisVarManager::GetValueName(valType[it])));
    obj->GetAxis(it)->SetTitle(Form("%s %s", PairAnalysisVarManager::GetValueLabel(valType[it]), PairAnalysisVarManager::GetValueUnit(valType[it])));
  }
  obj->Sumw2();
  return;
}

//_____________________________________________________________________________
void PairAnalysisHistos::FillValues(TObject *obj, const Double_t *values)
{
  //
  //
  //
  if (!obj) return;
  if      (obj->InheritsFrom(TH1::Class()))       FillValues(static_cast<TH1*>(obj), values);
  else if (obj->InheritsFrom(THnBase::Class()))   FillValues(static_cast<THnBase*>(obj), values);

  return;

}

//_____________________________________________________________________________
void PairAnalysisHistos::FillValues(TH1 *obj, const Double_t *values)
{
  //
  // fill values for TH1 inherted classes
  //

  Int_t dim   = obj->GetDimension();
  Bool_t bprf = kFALSE;
  //  UInt_t nValues = (UInt_t) PairAnalysisVarManager::kNMaxValues;

  UInt_t valueTypes=obj->GetUniqueID();
  if (valueTypes==(UInt_t)PairAnalysisHistos::kNoAutoFill) return;
  Bool_t weight = (valueTypes!=kNoWeights);

  // check if tprofile
  if(obj->IsA() == TProfile::Class()   ||
     obj->IsA() == TProfile2D::Class() ||
     obj->IsA() == TProfile3D::Class()  )    bprf=kTRUE;

  // TO BEAUTIFY: switch off manually weighting of profile3Ds
  if(obj->IsA() == TProfile3D::Class()) weight=kFALSE;

  // check for formulas
  TList *list = obj->GetListOfFunctions();
  TFormula *xform = dynamic_cast<TFormula*>(list->FindObject("xFormula"));
  TFormula *yform = dynamic_cast<TFormula*>(list->FindObject("yFormula"));
  TFormula *zform = dynamic_cast<TFormula*>(list->FindObject("zFormula"));
  TFormula *pform = dynamic_cast<TFormula*>(list->FindObject("pFormula"));
  TFormula *wform = dynamic_cast<TFormula*>(list->FindObject("wFormula"));
  //  Bool_t bform = (xform || yform || zform /*|| wform*/);

  // get variables from axis unique IDs
  UInt_t value1=obj->GetXaxis()->GetUniqueID();
  UInt_t value2=obj->GetYaxis()->GetUniqueID();
  UInt_t value3=obj->GetZaxis()->GetUniqueID();
  UInt_t value4=obj->GetUniqueID();            // get weighting/profile var stored in the unique ID

  Double_t fvals[4] = { values[value1], values[value2], values[value3], values[value4] };

  // use formulas to update fill values
  if(xform) fvals[0] = PairAnalysisHelper::EvalFormula(xform, values);
  if(yform) fvals[1] = PairAnalysisHelper::EvalFormula(yform, values);
  if(zform) fvals[2] = PairAnalysisHelper::EvalFormula(zform, values);
  if(wform) fvals[3] = PairAnalysisHelper::EvalFormula(wform, values);
  if(pform) fvals[3] = PairAnalysisHelper::EvalFormula(pform, values); // weighting overwriting for Profile3D

  // ask for inclusive trigger map variables
  if(value1!=PairAnalysisVarManager::kTriggerInclONL && value1!=PairAnalysisVarManager::kTriggerInclOFF &&
     value2!=PairAnalysisVarManager::kTriggerInclONL && value2!=PairAnalysisVarManager::kTriggerInclOFF &&
     value3!=PairAnalysisVarManager::kTriggerInclONL && value3!=PairAnalysisVarManager::kTriggerInclOFF &&
     value4!=PairAnalysisVarManager::kTriggerInclONL && value4!=PairAnalysisVarManager::kTriggerInclOFF ) {
    // no trigger map variable selected
    switch ( dim ) {
    case 1:
      if(!bprf && !weight)     obj->Fill(fvals[0]);                 // histograms
      else if(!bprf && weight) obj->Fill(fvals[0], fvals[3]); // weighted histograms
      else if(bprf && !weight) ((TProfile*)obj)->Fill(fvals[0],fvals[1]);   // profiles
      else                     ((TProfile*)obj)->Fill(fvals[0],fvals[1], fvals[3]); // weighted profiles
      break;
    case 2:
      if(!bprf && !weight)     obj->Fill(fvals[0], fvals[1]);                 // histograms
      else if(!bprf && weight) ((TH2*)obj)->Fill(fvals[0], fvals[1], fvals[3]); // weighted histograms
      else if(bprf && !weight) ((TProfile2D*)obj)->Fill(fvals[0], fvals[1], fvals[2]); // profiles
      else                     ((TProfile2D*)obj)->Fill(fvals[0], fvals[1], fvals[2], fvals[3]); // weighted profiles
      break;
    case 3:
      if(!bprf && !weight)     ((TH3*)obj)->Fill(fvals[0], fvals[1], fvals[2]);                 // histograms
      else if(!bprf && weight) ((TH3*)obj)->Fill(fvals[0], fvals[1], fvals[2], fvals[3]); // weighted histograms
      else if(bprf && !weight) ((TProfile3D*)obj)->Fill(fvals[0], fvals[1], fvals[2], fvals[3]); // profiles
      else                     Printf(" WARNING: weighting NOT yet possible for TProfile3Ds !");
      break;
    }
  }
  else {
    // fill inclusive trigger map variables
    if(weight) return;
    switch ( dim ) {
    case 1:
      for(Int_t i=0; i<30; i++) { if(TESTBIT((UInt_t)fvals[0],i)) obj->Fill(i); }
      break;
    case 2:
      if((value1==PairAnalysisVarManager::kTriggerInclOFF && value2==PairAnalysisVarManager::kTriggerInclONL) ||
	 (value1==PairAnalysisVarManager::kTriggerInclONL && value2==PairAnalysisVarManager::kTriggerInclOFF) ) {
	for(Int_t i=0; i<30; i++) {
	  if((UInt_t)fvals[0]==BIT(i)) {
	    for(Int_t i2=0; i2<30; i2++) {
	      if((UInt_t)fvals[1]==BIT(i2)) {
		obj->Fill(i, i2);
	      } // bit fired
	    } //loop 2
	  }//bit fired
	} // loop 1
      }
      else if(value1==PairAnalysisVarManager::kTriggerInclONL || value1==PairAnalysisVarManager::kTriggerInclOFF) {
	for(Int_t i=0; i<30; i++) { if(TESTBIT((UInt_t)fvals[0],i)) obj->Fill(i, fvals[1]); }
      }
      else if(value2==PairAnalysisVarManager::kTriggerInclONL || value2==PairAnalysisVarManager::kTriggerInclOFF) {
	for(Int_t i=0; i<30; i++) { if(TESTBIT((UInt_t)fvals[1],i)) obj->Fill(fvals[0], i); }
      }
      else //makes no sense
	return;
      break;
    default: return;
    }

  } //end: trigger filling


  return;
}

//_____________________________________________________________________________
void PairAnalysisHistos::FillValues(THnBase *obj, const Double_t *values)
{
  //
  // fill values for THn inherted classes
  //

  const Int_t dim   = obj->GetNdimensions();

  UInt_t valueTypes=obj->GetUniqueID();
  if (valueTypes==(UInt_t)PairAnalysisHistos::kNoAutoFill) return;
  Bool_t weight = (valueTypes!=kNoWeights);

  UInt_t value4=obj->GetUniqueID();            // weight variable

  Double_t fill[dim];
  for(Int_t it=0; it<dim; it++)   fill[it] = values[obj->GetAxis(it)->GetUniqueID()];
  if(!weight) obj->Fill(fill);
  else obj->Fill(fill, values[value4]);


  return;
}

//_____________________________________________________________________________
void PairAnalysisHistos::FillVarArray(TObject *obj, UInt_t *valType)
{
  //
  // extract variables stored in the axis (special for TProfile3D)
  //


  if (!obj) return;
  //  Printf(" fillvararray %s",obj->GetName());

  if (obj->InheritsFrom(TH1::Class())) {
    valType[0]=((TH1*)obj)->GetXaxis()->GetUniqueID();
    valType[1]=((TH1*)obj)->GetYaxis()->GetUniqueID();
    valType[2]=((TH1*)obj)->GetZaxis()->GetUniqueID();
    valType[3]=((TH1*)obj)->GetUniqueID();  // weighting(profile) var stored in unique ID
  }
  else if (obj->InheritsFrom(THnBase::Class())) {
    for(Int_t it=0; it<((THn*)obj)->GetNdimensions(); it++)
      valType[it]=((THn*)obj)->GetAxis(it)->GetUniqueID();
  }
  valType[19]=obj->GetUniqueID(); //weights
  return;
}

//_____________________________________________________________________________
void PairAnalysisHistos::AdaptNameTitle(TH1 *hist, const char* histClass) {

  //
  // adapt name and title of the histogram
  //

  Int_t dim            = hist->GetDimension();
  TString currentName  = "";//hist->GetName();
  TString currentTitle = "";//hist->GetTitle();
  TString hclass       = histClass;
  //get reserved class
  TObjArray *arr=hclass.Tokenize("_");
  arr->SetOwner();
  hclass=((TObjString*)arr->At(0))->GetString();
  delete arr;

  Bool_t bname  = (currentName.IsNull());
  Bool_t btitle = (currentTitle.IsNull());
  Bool_t bprf   = kFALSE;
  if(hist->IsA() == TProfile::Class() || hist->IsA() == TProfile2D::Class() || hist->IsA() == TProfile3D::Class())
    bprf=kTRUE;

  // tprofile options
  Double_t pmin=0., pmax=0.;
  TString option = "", calcrange="";
  Bool_t bStdOpt=kTRUE;
  if(bprf) {
    switch( dim ) {
    case 3:
      option = ((TProfile3D*)hist)->GetErrorOption();
      pmin   = ((TProfile3D*)hist)->GetTmin();
      pmax   = ((TProfile3D*)hist)->GetTmax();
      break;
    case 2:
      option = ((TProfile2D*)hist)->GetErrorOption();
      pmin   = ((TProfile2D*)hist)->GetZmin();
      pmax   = ((TProfile2D*)hist)->GetZmax();
      break;
    case 1:
      option = ((TProfile*)hist)->GetErrorOption();
      pmin   = ((TProfile*)hist)->GetYmin();
      pmax   = ((TProfile*)hist)->GetYmax();
      break;
    }
    if(option.Contains("s",TString::kIgnoreCase)) bStdOpt=kFALSE;
    if(pmin!=pmax) calcrange=Form("#cbar_{%+.*f}^{%+.*f}",GetPrecision(pmin),pmin,GetPrecision(pmax),pmax);
  }

  UInt_t varx = hist->GetXaxis()->GetUniqueID();
  UInt_t vary = hist->GetYaxis()->GetUniqueID();
  UInt_t varz = hist->GetZaxis()->GetUniqueID();
  UInt_t varp = hist->GetUniqueID();
  Bool_t weight = (varp!=kNoWeights);
  if(bprf && dim==3) weight=kFALSE; // no weighting for profile3D

  // store titles in the axis
  if(btitle) {
    TString tit = "";
    /////// set NAMES
    hist->GetXaxis()->SetName(PairAnalysisVarManager::GetValueName(varx));
    hist->GetYaxis()->SetName(PairAnalysisVarManager::GetValueName(vary));
    hist->GetZaxis()->SetName(PairAnalysisVarManager::GetValueName(varz));
    // adapt according to formula
    TFormula *xform = dynamic_cast<TFormula*>(hist->GetListOfFunctions()->FindObject("xFormula"));
    TFormula *yform = dynamic_cast<TFormula*>(hist->GetListOfFunctions()->FindObject("yFormula"));
    TFormula *zform = dynamic_cast<TFormula*>(hist->GetListOfFunctions()->FindObject("zFormula"));
    TFormula *wform = dynamic_cast<TFormula*>(hist->GetListOfFunctions()->FindObject("wFormula"));
    if(xform) { hist->GetXaxis()->SetName(PairAnalysisHelper::GetFormulaName(xform).Data()); }
    if(yform) { hist->GetYaxis()->SetName(PairAnalysisHelper::GetFormulaName(yform).Data()); }
    if(zform) { hist->GetZaxis()->SetName(PairAnalysisHelper::GetFormulaName(zform).Data()); }
    /////// set TITLE
    hist->GetXaxis()->SetTitle(PairAnalysisVarManager::GetValueLabel(varx));
    hist->GetYaxis()->SetTitle(PairAnalysisVarManager::GetValueLabel(vary));
    hist->GetZaxis()->SetTitle(PairAnalysisVarManager::GetValueLabel(varz));
    // adapt according to formula
    if(xform){hist->GetXaxis()->SetTitle(PairAnalysisHelper::GetFormulaTitle(xform).Data()); }
    if(yform){hist->GetYaxis()->SetTitle(PairAnalysisHelper::GetFormulaTitle(yform).Data()); }
    if(zform){hist->GetZaxis()->SetTitle(PairAnalysisHelper::GetFormulaTitle(zform).Data()); }
    // profile axis
    if(bprf) {
      TAxis *ax    = hist->GetYaxis();
      switch( dim ) {
	//      case 3: ax = hist->GetZaxis(); break; //TODO title
      case 2: ax = hist->GetZaxis(); break;
      case 1: ax = hist->GetYaxis(); break;
      }
      tit=ax->GetTitle();
      tit.Prepend( (bStdOpt ? "#LT" : "RMS(") );
      tit.Append ( (bStdOpt ? "#GT" : ")")    );
      tit.Append ( calcrange.Data()           );
      ax->SetTitle(tit.Data());
    }
    // append the units for all axes (except formula)
    tit=Form("%s %s",hist->GetXaxis()->GetTitle(), PairAnalysisVarManager::GetValueUnit(varx));
    if(!xform) hist->GetXaxis()->SetTitle(tit.Data());
    tit=Form("%s %s",hist->GetYaxis()->GetTitle(), PairAnalysisVarManager::GetValueUnit(vary));
    if(!yform) hist->GetYaxis()->SetTitle(tit.Data());
    tit=Form("%s %s",hist->GetZaxis()->GetTitle(), PairAnalysisVarManager::GetValueUnit(varz));
    if(!zform) hist->GetZaxis()->SetTitle(tit.Data());
    // overwrite titles with hist class if needed
    if(!bprf) {
      switch( dim ) {
      case 1: hist->GetYaxis()->SetTitle(Form("%ss",hclass.Data())); break;
      case 2: hist->GetZaxis()->SetTitle(Form("%ss",hclass.Data())); break;
      }
    }
    // weighted axis (maximal 2 dimensional)
    if(weight) {
      TAxis *ax    = hist->GetYaxis();
      if(dim==2)ax = hist->GetZaxis();
      tit=PairAnalysisVarManager::GetValueLabel(varp);
      if(wform) { tit=PairAnalysisHelper::GetFormulaTitle(wform); }
      ax->SetTitle(Form("%s weighted %s", tit.Data(), ax->GetTitle()));
    }

    // create an unique name
    if(bname)
      switch(dim) {
      case 3:
	currentName+=Form("%s_",hist->GetXaxis()->GetName());
	currentName+=Form("%s_",hist->GetYaxis()->GetName());
	currentName+=Form("%s", hist->GetZaxis()->GetName());
	if(bprf)   currentName+=Form("-%s%s",PairAnalysisVarManager::GetValueName(varp),(bStdOpt ? "avg" : "rms"));
	if(weight&&!bprf) currentName+=Form("-wght%s",PairAnalysisVarManager::GetValueName(varp));
	break;
      case 2:
	currentName+=Form("%s_",hist->GetXaxis()->GetName());
	currentName+=Form("%s", hist->GetYaxis()->GetName());
	if(bprf)   currentName+=Form("-%s%s",hist->GetZaxis()->GetName(),(bStdOpt ? "avg" : "rms"));
	if(weight && !wform) currentName+=Form("-wght%s",PairAnalysisVarManager::GetValueName(varp));
	if(weight &&  wform) currentName+=Form("-wght%s",PairAnalysisHelper::GetFormulaName(wform).Data());
	break;
      case 1:
	currentName+=Form("%s",hist->GetXaxis()->GetName());
	if(bprf)   currentName+=Form("-%s%s",hist->GetYaxis()->GetName(),(bStdOpt ? "avg" : "rms"));
	if(weight && !wform) currentName+=Form("-wght%s",PairAnalysisVarManager::GetValueName(varp));
	if(weight &&  wform) currentName+=Form("-wght%s",PairAnalysisHelper::GetFormulaName(wform).Data());
	break;
      }
    // to differentiate btw. leg and pair histos
    //    if(!strcmp(histClass,"Pair")) currentName.Prepend("p");
    if(hclass.Contains("Pair")) currentName.Prepend("p");
    hist->SetName(currentName.Data());
  }

}

//_____________________________________________________________________________
Int_t PairAnalysisHistos::GetPrecision(Double_t value)
{
  //
  // computes the precision of a double
  // usefull for axis ranges etc
  // TODO: move to PairAnalysisHelper

  Bool_t bfnd     = kFALSE;
  Int_t precision = 0;
  value*=1e6;
  while(!bfnd) {
    //    Printf(" value %f precision %d bfnd %d",TMath::Abs(value*TMath::Power(10,precision)), precision, bfnd);
    bfnd = ((TMath::Abs(value*TMath::Power(10,precision))/1e6  -  TMath::Floor(TMath::Abs(value*TMath::Power(10,precision))/1e6)) != 0.0
	    ? kFALSE
	    : kTRUE);
    if(!bfnd) precision++;
  }

  //  Printf("precision for %f found to be %d", value, precision);
  return precision;

}
