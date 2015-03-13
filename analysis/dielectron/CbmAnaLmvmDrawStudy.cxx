/** CbmAnaLmvmDrawStudy.cxx
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 2.0
 **/

#include "CbmAnaLmvmDrawStudy.h"

#include "CbmDrawHist.h"
#include "CbmUtils.h"
#include "CbmHistManager.h"

#include <string>
#include <iostream>
#include <iomanip>

#include <boost/assign/list_of.hpp>

#include "TText.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TMath.h"
#include "TKey.h"
#include "TClass.h"
#include "TF1.h"
#include "TEllipse.h"
#include "TStyle.h"
#include "TSystem.h"

using namespace std;
using namespace Cbm;
using boost::assign::list_of;



void CbmAnaLmvmDrawStudy::DrawFromFile(
      const vector<string>& fileNames,
      const vector<string>& fileNamesMean,
      const vector<string>& studyNames,
      const string& outputDir)
{
   fOutputDir = outputDir;

   SetDefaultDrawStyle();

   fNofStudies = fileNames.size();
   fHM.resize(fNofStudies);
   for (int i = 0; i < fNofStudies; i++){
      fHM[i] = new CbmHistManager();
      TFile* file = new TFile(fileNames[i].c_str());
      fHM[i]->ReadFromFile(file);
      Int_t fNofEvents = (int) H1(i, "fh_event_number")->GetEntries();
      fHM[i]->ScaleByPattern(".*", 1./fNofEvents);
   }

   // files with mean histograms
   //fMeanFiles.resize(fileNamesMean.size());
   //for (int i = 0; i < fileNamesMean.size(); i++){
      //fMeanFiles[i] = new TFile(fileNamesMean[i].c_str(), "READ");
   //}
   fMeanFiles = fileNamesMean;
   fStudyNames = studyNames;

   DrawMinv();
   DrawNofBgTracks();
   DrawBgSourceTracks();
   DrawBgSourcePairs();
   DrawBgSourceMinv();
   DrawDistributions("lmvm_study_source_mom_", "fh_source_mom_", kTtCut, kPi0);
   DrawDistributions("lmvm_study_source_mom_", "fh_source_mom_",kTtCut, kGamma);

   DrawDistributions("lmvm_study_source_pt_", "fh_source_pt_", kTtCut, kPi0);
   DrawDistributions("lmvm_study_source_pt_", "fh_source_pt_",kTtCut, kGamma);

   SaveCanvasToImage();
}

TCanvas* CbmAnaLmvmDrawStudy::CreateCanvas(
      const string& name,
      const string& title,
      int width,
      int height)
{
   TCanvas *c = new TCanvas(name.c_str(), title.c_str(), width, height);
   fCanvas.push_back(c);
   return c;
}

void CbmAnaLmvmDrawStudy::DrawTextOnHist(
      const string& text,
      Double_t x1,
      Double_t y1,
      Double_t x2,
      Double_t y2)
{
   TLegend* leg = new TLegend(x1, y1, x2, y2);
   leg->AddEntry(new TH2D(), text.c_str(), "");
   leg->SetFillColor(kWhite);
   leg->SetFillStyle(0);
   leg->SetBorderSize(0);
   leg->Draw();
}

TH1D* CbmAnaLmvmDrawStudy::H1(
      int studyNum,
      const string& name)
{
   return (TH1D*) fHM[studyNum]->H1(name);
}

TH2D* CbmAnaLmvmDrawStudy::H2(
      int studyNum,
      const string& name)
{
   return (TH2D*) fHM[studyNum]->H1(name);
}

void CbmAnaLmvmDrawStudy::SaveCanvasToImage()
{
   for (int i = 0; i < fCanvas.size(); i++){
      Cbm::SaveCanvasAsImage(fCanvas[i], fOutputDir);
   }
}

void CbmAnaLmvmDrawStudy::SetAnalysisStepLabels(
      TH1* h)
{
   h->GetXaxis()->SetLabelSize(0.06);
   for (Int_t step = 0; step < CbmLmvmHist::fNofAnaSteps; step++){
      h->GetXaxis()->SetBinLabel(step + 1, CbmLmvmHist::fAnaStepsLatex[step].c_str());
   }
}

void CbmAnaLmvmDrawStudy::DrawMinv()
{
   vector<TH1*> hPtCut, hTtCut;
   hPtCut.resize(fMeanFiles.size());
   hTtCut.resize(fMeanFiles.size());
   int nRebin = 20;
   for (int i = 0; i < fMeanFiles.size(); i++){
      TFile* f = new TFile(fMeanFiles[i].c_str(), "READ");

      hPtCut[i] = (TH1D*)f->Get("fh_bg_minv_ptcut")->Clone();
      hPtCut[i]->Rebin(nRebin);
      hPtCut[i]->SetMinimum(1e-6);
      hTtCut[i] = (TH1D*)f->Get("fh_bg_minv_ttcut")->Clone();
      hTtCut[i]->Rebin(nRebin);
      hPtCut[i]->SetMinimum(1e-6);
      //f->Close();
   }
   TCanvas *c1 = CreateCanvas("lmvm_study_minv_bg_ttcut", "lmvm_study_minv_bg_ttcut", 600, 600);
   DrawH1(hTtCut, fStudyNames, kLinear, kLog, true, 0.70, 0.55, 0.99, 0.99, "");

   TCanvas *c2 = CreateCanvas("lmvm_study_minv_bg_ptcut", "lmvm_study_minv_bg_ptcut", 600, 600);
   DrawH1(hPtCut, fStudyNames, kLinear, kLog, true, 0.70, 0.55, 0.99, 0.99, "");


  /* TCanvas *c1 = CreateCanvas("lmvm_study_minv_urqmd", "lmvm_study_minv_urqmd", 900, 900);
   vector<TH1*> hbg;
   hbg.resize(fNofStudies);
   for (int i = 0; i < fNofStudies; i++){
      hbg[i] = (TH1D*)H1(i, "fh_bg_minv_ptcut")->Clone();
      hbg[i]->Rebin(20);
     // hbg[i]->GetYaxis()->SetTitle("Tracks/event x10^{-1}");
      //hbg[i]->GetXaxis()->SetRange(kElId + 1, kPtCut + 1);
      hbg[i]->SetMinimum(1e-6);
   }
   //SetAnalysisStepLabels(hbg[0]);
   DrawH1(hbg, fStudyNames, kLinear, kLog, true, 0.70, 0.75, 0.99, 0.99, "");
   for (int i = 0; i < fNofStudies; i++){
      hbg[i]->SetLineWidth(3.);
   */
}

void CbmAnaLmvmDrawStudy::DrawNofBgTracks()
{
   TCanvas *c1 = CreateCanvas("lmvm_study_nof_bg_tracks", "lmvm_study_nof_bg_tracks", 600, 600);
   vector<TH1*> hbg;
   hbg.resize(fNofStudies);
   for (int i = 0; i < fNofStudies; i++){
      hbg[i] = (TH1D*)H1(i, "fh_nof_bg_tracks")->Clone();
      hbg[i]->Scale(10);
      hbg[i]->GetYaxis()->SetTitle("Tracks/event x10^{-1}");
      hbg[i]->GetXaxis()->SetRange(kElId + 1, kPtCut + 1);
      hbg[i]->SetMinimum(0.0);
   }
   SetAnalysisStepLabels(hbg[0]);
   DrawH1(hbg, fStudyNames, kLinear, kLinear, true, 0.70, 0.75, 0.99, 0.99, "");
   for (int i = 0; i < fNofStudies; i++){
      hbg[i]->SetLineWidth(3.);
   }
}

void CbmAnaLmvmDrawStudy::DrawBgSourceTracks()
{
   TCanvas *c = CreateCanvas("lmvm_study_source_tracks_abs", "lmvm_study_source_tracks_abs", 1200, 400);
   c->Divide(3, 1);
   for (int iP = 0; iP < 3; iP++){
      c->cd(iP+1);
      vector<TH1*> habsPx;
      habsPx.resize(fNofStudies);
      for (int i = 0; i < fNofStudies; i++){
         TH2D* habs = (TH2D*)H2(i, "fh_source_tracks")->Clone();
         int min = iP + 1;
         int max = iP + 1;
         if (iP == 2) max = 9;
         stringstream ss;
         ss << "fh_source_tracks_" << i << "_" << min << "_" << max;
         habsPx[i] = habs->ProjectionX(ss.str().c_str(), min, max);
         habsPx[i]->GetXaxis()->SetRange(kElId + 1, kPtCut + 1);
         habsPx[i]->GetYaxis()->SetTitle("Tracks per event x10^{-2}");
         habsPx[i]->Scale(100);
      }
      DrawH1(habsPx, fStudyNames, kLinear, kLinear, true, 0.70, 0.75, 0.99, 0.99, "");
      SetAnalysisStepLabels(habsPx[0]);
      for (int i = 0; i < fNofStudies; i++){
         habsPx[i]->SetMinimum(0.);
         habsPx[i]->SetLineWidth(3.);
      }
      string txt = "#gamma";
      if (iP == 1) txt = "#pi^{0}";
      if (iP == 2) txt = "oth.";
      DrawTextOnHist(txt, 0.5, 0.9, 0.6, 0.99);
   }
}

void CbmAnaLmvmDrawStudy::DrawBgSourcePairs()
{
   TCanvas *c = CreateCanvas("lmvm_study_source_pairs_abs", "lmvm_study_source_pairs_abs", 1200, 800);
   c->Divide(3, 2);
   for (int iP = 0; iP < 6; iP++){
      c->cd(iP+1);
      vector<TH1*> habsPx;
      habsPx.resize(fNofStudies);
      for (int i = 0; i < fNofStudies; i++){
         TH2D* habs = (TH2D*)H2(i, "fh_source_pairs")->Clone();
         int min = iP + 1;
         int max = iP + 1;
         stringstream ss;
         ss << "fh_source_pairs_" << i << "_" << min << "_" << max;
         habsPx[i] = habs->ProjectionX(ss.str().c_str(), min, max);
         habsPx[i]->GetXaxis()->SetRange(kElId + 1, kPtCut + 1);
         habsPx[i]->GetYaxis()->SetTitle("Pairs per event x10^{-3}");
         habsPx[i]->Scale(1000);
      }
      DrawH1(habsPx, fStudyNames, kLinear, kLinear, true, 0.70, 0.75, 0.99, 0.99, "");
      SetAnalysisStepLabels(habsPx[0]);
      for (int i = 0; i < fNofStudies; i++){
         habsPx[i]->SetMinimum(0.);
         habsPx[i]->SetLineWidth(3.);
      }
      DrawTextOnHist(CbmLmvmHist::fBgPairSourceLatex[iP], 0.4, 0.9, 0.6, 0.99);
   }

   DrawBgSourcePairsStep(kPtCut);
   DrawBgSourcePairsStep(kTtCut);
}

void CbmAnaLmvmDrawStudy::DrawBgSourcePairsStep(
      int step)
{
   stringstream ssC;
   ssC << "lmvm_study_source_pairs_" << CbmLmvmHist::fAnaSteps[step];
   TCanvas *c2 = CreateCanvas(ssC.str().c_str(), ssC.str().c_str(), 600, 600);
   vector<TH1*> habsPx;
   habsPx.resize(fNofStudies);
   for (int i = 0; i < fNofStudies; i++){
      TH2D* habs = (TH2D*)H2(i, "fh_source_pairs")->Clone();
      stringstream ss;
      ss << "fh_source_pairs_" << i << "_" << CbmLmvmHist::fAnaSteps[step];
      habsPx[i] = habs->ProjectionY(ss.str().c_str(), step+1, step+1);
      habsPx[i]->GetYaxis()->SetTitle("Pairs per event x10^{-3}");
      habsPx[i]->Scale(1000);
   }
   DrawH1(habsPx, fStudyNames, kLinear, kLinear, true, 0.70, 0.75, 0.99, 0.99, "");
   for (int i = 0; i < fNofStudies; i++){
      habsPx[i]->SetMinimum(0.);
      habsPx[i]->SetLineWidth(3.);
      for (Int_t y = 1; y <= CbmLmvmHist::fBgPairSourceLatex.size(); y++){
         habsPx[i]->GetXaxis()->SetBinLabel(y, CbmLmvmHist::fBgPairSourceLatex[y-1].c_str());
      }
   }
}

void CbmAnaLmvmDrawStudy::DrawDistributions(
      const string& canvasName,
      const string& histName,
      int step,
      int sourceType)
{
   stringstream ssC;
   ssC << canvasName << CbmLmvmHist::fAnaSteps[step]<<"_"<<CbmLmvmHist::fSourceTypes[sourceType];
   TCanvas *c = CreateCanvas(ssC.str().c_str(), ssC.str().c_str(), 900, 900);

   string s = histName + CbmLmvmHist::fAnaSteps[step]+"_"+CbmLmvmHist::fSourceTypes[sourceType];
   vector<TH1*> h;
   h.resize(fNofStudies);
   for (int i = 0; i < fNofStudies; i++){
      h[i] = (TH1D*)H1(i, s)->Clone();
      h[i]->GetXaxis()->SetRangeUser(0., 3.);
   }
   DrawH1(h, fStudyNames, kLinear, kLinear, true, 0.70, 0.75, 0.99, 0.99, "");
}

void CbmAnaLmvmDrawStudy::DrawBgSourceMinv()
{
   TCanvas *c = CreateCanvas("lmvm_study_source_minv_ptcut", "lmvm_study_source_minv_ptcut", 1200, 800);
   c->Divide(3, 2);
   for (int iP = 0; iP < CbmLmvmHist::fNofBgPairSources; iP++){
      stringstream ss;
      ss << "fh_source_bg_minv_" <<iP << "_" << CbmLmvmHist::fAnaSteps[kPtCut];
      c->cd(iP+1);
      vector<TH1*> habs;
      habs.resize(fNofStudies);
      for (int i = 0; i < fNofStudies; i++){
         habs[i] = (TH1D*)H1(i, ss.str())->Clone();
         habs[i]->Rebin(40);
      }
      DrawH1(habs, fStudyNames, kLinear, kLinear, true, 0.70, 0.75, 0.99, 0.99, "");
      for (int i = 0; i < fNofStudies; i++){
         habs[i]->SetMinimum(0.);
         habs[i]->SetLineWidth(3.);
      }
      DrawTextOnHist(CbmLmvmHist::fBgPairSourceLatex[iP], 0.4, 0.9, 0.6, 0.99);
   }
}

ClassImp(CbmAnaLmvmDrawStudy);
