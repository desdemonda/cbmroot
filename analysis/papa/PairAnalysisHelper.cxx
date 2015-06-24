/*************************************************************************
* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
**************************************************************************/

//
// PairAnalysis helper functions wrapped in a namespace
//
//
// Authors: 
//   Julian Book <Julian.Book@cern.ch>




#include <TError.h>
#include <TPad.h>
#include <TMath.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TVectorD.h>
#include <TH1.h>
#include <TF1.h>
#include <TRandom.h>
#include <TProfile.h>
#include <TFormula.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <TMCProcess.h>
#include <THashList.h>
#include <TGraph.h>

#include <CbmDetectorList.h>

#include "PairAnalysisVarCuts.h"
#include "PairAnalysisVarManager.h"
#include "PairAnalysisHelper.h"

//_____________________________________________________________________________
TVectorD* PairAnalysisHelper::MakeLogBinning(Int_t nbinsX, Double_t xmin, Double_t xmax)
{
  //
  // Make logarithmic binning
  // the user has to delete the array afterwards!!!
  //
  
  //check limits
  if (xmin<1e-20 || xmax<1e-20){
    Error("PairAnalysisHelper::MakeLogBinning","For Log binning xmin and xmax must be > 1e-20. Using linear binning instead!");
    return PairAnalysisHelper::MakeLinBinning(nbinsX, xmin, xmax);
  }
  if (xmax<xmin){
    Double_t tmp=xmin;
    xmin=xmax;
    xmax=tmp;
  }
  TVectorD *binLim=new TVectorD(nbinsX+1);
  Double_t first=xmin;
  Double_t last=xmax;
  Double_t expMax=TMath::Log(last/first);
  for (Int_t i=0; i<nbinsX+1; ++i){
    (*binLim)[i]=first*TMath::Exp(expMax/nbinsX*(Double_t)i);
  }
  return binLim;
}

//_____________________________________________________________________________
TVectorD* PairAnalysisHelper::MakeLinBinning(Int_t nbinsX, Double_t xmin, Double_t xmax)
{
  //
  // Make linear binning
  // the user has to delete the array afterwards!!!
  //
  if (xmax<xmin){
    Double_t tmp=xmin;
    xmin=xmax;
    xmax=tmp;
  }
  TVectorD *binLim=new TVectorD(nbinsX+1);
  Double_t first=xmin;
  Double_t last=xmax;
  Double_t binWidth=(last-first)/nbinsX;
  for (Int_t i=0; i<nbinsX+1; ++i){
    (*binLim)[i]=first+binWidth*(Double_t)i;
  }
  return binLim;
}

//_____________________________________________________________________________
TVectorD* PairAnalysisHelper::MakeArbitraryBinning(const char* bins)
{
  //
  // Make arbitrary binning, bins separated by a ','
  //
  TString limits(bins);
  if (limits.IsNull()){
    Error("PairAnalysisHelper::MakeArbitraryBinning","Bin Limit string is empty, cannot add the variable");
    return 0x0;
  }
  
  TObjArray *arr=limits.Tokenize(",");
  Int_t nLimits=arr->GetEntries();
  if (nLimits<2){
    Error("PairAnalysisHelper::MakeArbitraryBinning","Need at leas 2 bin limits, cannot add the variable");
    delete arr;
    return 0x0;
  }
  
  TVectorD *binLimits=new TVectorD(nLimits);
  for (Int_t iLim=0; iLim<nLimits; ++iLim){
    (*binLimits)[iLim]=(static_cast<TObjString*>(arr->At(iLim)))->GetString().Atof();
  }
  
  delete arr;
  return binLimits;
}


//_____________________________________________________________________________
TVectorD* PairAnalysisHelper::MakePdgBinning()
{
  //
  // Make arbitrary binning using defined PDG codes
  //

  // array of pdgcodes stored in TDatabasePDG
  TDatabasePDG *pdg = new TDatabasePDG();//::Instance();
  pdg->ReadPDGTable();
  TIter next(pdg->ParticleList() );
  TGraph gr;
  TParticlePDG *p;
  Int_t i=0;
  while ((p = (TParticlePDG *)next())) {
    if(TMath::Abs(p->PdgCode()) < 1e+6) {
      // printf("%s -> %d \n",p->GetName(),p->PdgCode());
      gr.SetPoint(i++, p->PdgCode(),1.);
    }
  }
  gr.Sort();
  TVectorD *vec = new TVectorD(gr.GetN(), gr.GetX());
  //  vec->Print();
  delete pdg;
  return vec;

}

//_____________________________________________________________________________
Double_t PairAnalysisHelper::EvalFormula(TFormula *form, const Double_t *values)
{
  //
  // evaluate return value for formula with current parameter values
  //
  Double_t params[10] = {0.};
  //put parameter values into array using variables stored as the parameters
  for(Int_t ip=0; ip<form->GetNpar(); ip++) params[ip]=values[(UInt_t)form->GetParameter(ip)];
  return (form->EvalPar(0x0,params));
}

//_____________________________________________________________________________
TString PairAnalysisHelper::GetFormulaTitle(TFormula *form)
{
  //
  // evaluate the formula in a readable way (for axis etc)
  //
  // TODO: add units, switch to TMathText, get ride of obsolete parentheses
  TString tform(form->GetExpFormula());
  // TMathText
  //  tform.ReplaceAll("*","\\cdot");   // multiplication sign
  //  tform.ReplaceAll("TMath::","\\"); // get ride of TMath::
  // TLatex
  tform.ReplaceAll("*","#upoint");   // multiplication sign
  //  tform.ReplaceAll("Sqrt","sqrt"); // sqrt sign
  tform.ReplaceAll("TMath::",""); // get ride of TMath::
  tform.ReplaceAll("()","");        // remove function parenthesis
  tform.ToLower();                  // lower cases (e.g. Cos -> cos)
  // replace parameter variables with proper labels
  for(Int_t j=0;j<form->GetNpar();j++)
    tform.ReplaceAll(Form("[%d]",j),
		     PairAnalysisVarManager::GetValueLabel((UInt_t)form->GetParameter(j))
		     );
  return (tform);
}

//_____________________________________________________________________________
TString PairAnalysisHelper::GetFormulaName(TFormula *form)
{
  //
  // build formula key with parameter names
  //
  TString tform("f(");
  for(Int_t j=0;j<form->GetNpar();j++) {
    tform+=PairAnalysisVarManager::GetValueName((UInt_t)form->GetParameter(j));
    if(j!=form->GetNpar()-1) tform+=",";
  }
  tform+=")";
  return (tform);
}

//_____________________________________________________________________________
void PairAnalysisHelper::SetPDGBinLabels( TH1 *hist) {
  //
  // build formula key with parameter names
  //
  TDatabasePDG *pdg = TDatabasePDG::Instance();
  TAxis *xaxis = hist->GetXaxis();
  for(Int_t i=1; i<hist->GetNbinsX()+1; i++) {
    // printf("bin %d: low edge: %.0f --> %s \n",i,xaxis->GetBinLowEdge(i), pdg->GetParticle(xaxis->GetBinLowEdge(i))->GetName());
    xaxis->SetBinLabel(i,pdg->GetParticle((Int_t)xaxis->GetBinLowEdge(i))->GetName());
  }
}

//_____________________________________________________________________________
const char* PairAnalysisHelper::GetPDGlabel(Int_t pdg) {
  //
  // return the label in latex format corresponding to pdg code
  //

  TString name=TDatabasePDG::Instance()->GetParticle(pdg)->GetName();
  name.ReplaceAll("dd_1_bar","primary");
  // correct greek letters
  if(name.Contains("delta",TString::kIgnoreCase) ||
     name.Contains("sigma",TString::kIgnoreCase) ||
     name.Contains("xi",TString::kIgnoreCase) ||
     name.Contains("lambda",TString::kIgnoreCase) ||
     name.Contains("omega",TString::kIgnoreCase) ||
     name.Contains("eta",TString::kIgnoreCase) ||
     name.Contains("tau",TString::kIgnoreCase) ||
     name.Contains("phi",TString::kIgnoreCase) ||
     name.Contains("eta",TString::kIgnoreCase) ||
     name.Contains("upsilon",TString::kIgnoreCase) ||
     name.Contains("pi",TString::kIgnoreCase) ||
     name.Contains("rho",TString::kIgnoreCase) ) name.Prepend("#");
  // correct anti particles
  if(name.Contains("_bar")) { name.ReplaceAll("_bar","}"); name.Prepend("#bar{"); }
    // correct indices
  name.ReplaceAll("+","^{+}");       name.ReplaceAll("-","^{-}");         name.ReplaceAll("0","^{0}");
  name.ReplaceAll("_s","_{s}");       name.ReplaceAll("_c","_{c}");       name.ReplaceAll("_b","_{b}");
  name.ReplaceAll("_1","_{1}");
  // specials
  name.ReplaceAll("/psi","/#psi");
  //  Printf(" %d = %s \n",pdg,name.Data());
  return name.Data();
}

//_____________________________________________________________________________
void PairAnalysisHelper::SetGEANTBinLabels( TH1 *hist) {
  //
  // build formula key with parameter names
  //
  TAxis *xaxis = hist->GetXaxis();
  for(Int_t i=1; i<hist->GetNbinsX()+1; i++) {
    xaxis->SetBinLabel(i,TMCProcessName[i-1]);
  }
  if(gPad) {
    gPad->SetTopMargin(0.01);
    gPad->SetBottomMargin(0.4);
  }
}


//_____________________________________________________________________________
TString PairAnalysisHelper::GetDetName(DetectorId det) {
  //
  // get detector name
  //
  TString name="";
  CbmDetectorList::GetSystemNameCaps(det, name);
  return name;
}
