#ifndef PAIRANALYSISHELPER_H
#define PAIRANALYSISHELPER_H
/* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

///////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       //
// PairAnalysis helpers                                                                    //
//                                                                                       //
//                                                                                       //
// Authors:                                                                              //
//   Julian Book <Julian.Book@cern.ch>                                                   //
//                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////


#include <TVectorDfwd.h>

#include <CbmDetectorList.h>

//class AliKFParticle;
/* class AliVEvent; */
/* class AliMCEvent; */
///class PairAnalysisEventNEW;
class TFormula;
class TH1;

namespace PairAnalysisHelper
{

  TVectorD* MakeLogBinning(Int_t nbinsX, Double_t xmin, Double_t xmax);
  TVectorD* MakeLinBinning(Int_t nbinsX, Double_t xmin, Double_t xmax);
  TVectorD* MakeArbitraryBinning(const char* bins);

  TVectorD* MakePdgBinning();

  //  void RotateKFParticle(AliKFParticle * kfParticle,Double_t angle/*, const PairAnalysisEventNEW * const ev=0x0*/);

  // tformula functions
  Double_t EvalFormula(    TFormula *form, const Double_t *values);
  TString  GetFormulaTitle(TFormula *form);
  TString  GetFormulaName( TFormula *form);

  // pdg labels
  void SetPDGBinLabels( TH1 *hist);
  const char* GetPDGlabel(Int_t pdg);
  void SetGEANTBinLabels( TH1 *hist);

  // misc
  TString GetDetName(DetectorId det);

}

#endif
