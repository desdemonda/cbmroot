/**
 * \file CbmLitClusteringQaStudyReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitClusteringQaStudyReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "CbmUtils.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLine.h"
#include <boost/assign/list_of.hpp>
#include <vector>
#include <iostream>
using std::endl;
using std::vector;
using boost::assign::list_of;
using Cbm::NumberToString;
using Cbm::Split;
using Cbm::FindAndReplace;

CbmLitClusteringQaStudyReport::CbmLitClusteringQaStudyReport():
		CbmStudyReport()
{
	SetReportName("clustering_qa");
}

CbmLitClusteringQaStudyReport::~CbmLitClusteringQaStudyReport()
{

}

void CbmLitClusteringQaStudyReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   Out() << PrintNofObjects();

  // Out() << PrintImages(".*clustering_qa_.*png");
   PrintCanvases();

   Out() <<  R()->DocumentEnd();
}

string CbmLitClusteringQaStudyReport::PrintNofObjects() const
{
   Int_t nofStudies = HM().size();
   vector<vector<TH1*> > histos(nofStudies);
   for (Int_t i = 0; i < nofStudies; i++) {
   		histos[i] = HM(i)->H1Vector("hno_NofObjects_.+_Event");
	}
   string str = R()->TableBegin("Number of objects", list_of(string("")).range(GetStudyNames()));
   for (Int_t iHist = 0; iHist < histos[0].size(); iHist++) {
		string cellName = Split(histos[0][iHist]->GetName(), '_')[2];
		vector<string> cells(nofStudies);
		for (Int_t i = 0; i < nofStudies; i++) {
			cells[i] = NumberToString<Double_t>(histos[i][iHist]->GetMean());
		}
		str += R()->TableRow(list_of(cellName).range(cells));
	}
   str += R()->TableEnd();
   return str;
}

void CbmLitClusteringQaStudyReport::Draw()
{
   DrawAllEfficiencyHistos();

   //DrawByPattern("hhe_Trd_All_(Acc|Rec|Clone)_Station");

   DrawAccRecClones();
//   DrawAccRecCanvas();
   PrintListOfHistograms();
}

void CbmLitClusteringQaStudyReport::PrintListOfHistograms(){
   vector<TH1*> histos1 = HM()[0]->H1Vector(".*");
   Int_t nofHistos = histos1.size();
   for(Int_t iHisto = 0; iHisto < nofHistos; iHisto++){
      printf("%s\n", histos1[iHisto]->GetName());
   }

   vector<TH2*> histos2 = HM()[0]->H2Vector(".*");
   nofHistos = histos2.size();
   for(Int_t iHisto = 0; iHisto < nofHistos; iHisto++){
      printf("%s\n", histos2[iHisto]->GetName());
   }
}

void CbmLitClusteringQaStudyReport::DrawAccRecCanvas(){
   vector<string> par = list_of("Acc")("Rec")("Clone");
   Int_t nofPars = par.size();
   Int_t nofStudies = HM().size();
   string canvasName = GetReportName() + "_AccRec_station";
   TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 900);
   Int_t nofColumns = nofStudies;
   Int_t nofRows = nofPars;
   canvas->Divide(nofColumns, nofRows);
   for (Int_t iStudy = 0; iStudy < nofStudies; iStudy++){
      for (Int_t iPar = 0; iPar < nofPars; iPar++) {
         string histName = "hhe_Trd_All_" + par[iPar] + "_Station";
         vector<TH1*> histos(1);
         histos[0] = HM()[iStudy]->H1Vector(histName)[0];
         Int_t nofHistos = histos.size();
         vector<string> labels(nofHistos);
         canvas->cd(iStudy+iPar*nofColumns);
         for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
            labels[iHist] = GetStudyName(iStudy) + "(" + par[iPar] + ")";
            histos[iHist]->SetAxisRange(0.,9.);
         }
         DrawH1(histos, labels, kLinear, kLinear, true, 0.55, 0.15, 0.95, 0.25);
      }
   }
}

void CbmLitClusteringQaStudyReport::DrawAccRecClones(){
   vector<string> par = list_of("Acc")("Rec")("Clone");
   Int_t nofPars = par.size();
   Int_t nofStudies = HM().size();
   for (Int_t iStudy = 0; iStudy < nofStudies; iStudy++){
      string canvasName = GetReportName() + "hhe_Trd_All_(Acc|Rec|Clone)_Station (" + GetStudyName(iStudy) + ")";
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
      vector<TH1*> histos(nofPars);
      vector<string> labels(nofPars);
//      TString histTitle = GetReportName() + "_Acc|Rec|Clone_" + GetStudyName(iStudy);
//      histos[0]->SetTitle(histTitle.Data());
      for (Int_t iPar = 0; iPar < nofPars; iPar++) {
	 string histName = "hhe_Trd_All_" + par[iPar] + "_Station";
	 histos[iPar] = HM()[iStudy]->H1Vector(histName)[0];
	 labels[iPar] = par[iPar];
	 histos[iPar]->SetAxisRange(0.,9.);
	 histos[iPar]->SetMinimum(0.);
      }
      DrawH1(histos, labels, kLinear, kLinear, true, 0.65, 0.25, 0.8, 0.44);
   }
}

void CbmLitClusteringQaStudyReport::DrawByPattern(string histNamePattern){
   vector<TH1*> histos = HM()[0]->H1Vector(histNamePattern);
   for (UInt_t i = 0; i < histos.size(); i++) {
      string histName = histos[i]->GetName();
      DrawAccAndRec("clustering_qa_study_" + histName, histName);
   }
}

void CbmLitClusteringQaStudyReport::DrawAllEfficiencyHistos(){
   string histNamePattern = "hhe_Trd_All_Eff_.+";

   Int_t nofStudies = HM().size();
   vector<TH1*> effHistos(nofStudies);
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
     effHistos[iStudy] = HM()[iStudy]->H1Vector(histNamePattern)[0];
     TH1* effHist = effHistos[iStudy];
     string effHistName = effHist->GetName();
     string accHistName = FindAndReplace(effHistName, "_Eff_", "_Acc_");
     string cloneHistName = FindAndReplace(effHistName, "_Eff_", "_Clone_");
     string cloneProbHistName = FindAndReplace(effHistName, "_Eff_", "_CloneProb_");
     string recHistName = FindAndReplace(effHistName, "_Eff_", "_Rec_");
     TH1 *accHist = HM()[iStudy]->H1(accHistName);
     TH1 *cloneHist = HM()[iStudy]->H1(cloneHistName);
     TH1 *cloneProbHist = HM()[iStudy]->H1(cloneProbHistName);
     TH1 *recHist = HM()[iStudy]->H1(recHistName);
     DivideHistos(recHist, accHist, effHist, 100.);
     cloneProbHist->Sumw2();
     cloneProbHist->Divide(cloneHist, accHist, 1., 1., "B");
     cloneProbHist->Scale(100.);
     effHist->SetMinimum(0.);
     effHist->SetMaximum(100.);
     cloneProbHist->SetMinimum(0.);
     cloneProbHist->SetMaximum(100.);
   }
   string effHistName = effHistos[0]->GetName();
   DrawEfficiency("clustering_qa_study_" + effHistName, effHistName);
}

void CbmLitClusteringQaStudyReport::DivideHistos(
   TH1* histo1,
   TH1* histo2,
   TH1* histo3,
   Double_t scale)
{
   histo1->Sumw2();
   histo2->Sumw2();
   histo3->Sumw2();
   histo3->Divide(histo1, histo2, 1., 1., "B");
   histo3->Scale(scale);
}

void CbmLitClusteringQaStudyReport::DrawEfficiency(
      const string& canvasName,
      const string& histName)
{
   TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
   canvas->SetGrid();
   canvas->cd();

   Int_t nofStudies = HM().size();
   vector<string> labels(2*nofStudies);
   vector<TH1*> histos(2*nofStudies);
   vector<Double_t> efficiencies(2*nofStudies);
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
      CbmHistManager* hm = HM()[iStudy];
      histos[iStudy] = hm->H1(histName);
      efficiencies[iStudy] = CalcEfficiency(hm->H1(FindAndReplace(histName, "_Eff_", "_Rec_")), hm->H1(FindAndReplace(histName, "_Eff_", "_Acc_")), 100.);
      labels[iStudy] = "Efficiency " + GetStudyName(iStudy) + "(" + NumberToString<Double_t>(efficiencies[iStudy], 1) + ")";
      histos[iStudy]->SetAxisRange(0.,9.);
   }

   // draw the Clone Probability as well
   string cloneProbHistName = FindAndReplace(histName, "_Eff_", "_CloneProb_");
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
      CbmHistManager* hm = HM()[iStudy];
      histos[nofStudies+iStudy] = hm->H1(cloneProbHistName);
      efficiencies[nofStudies+iStudy] = CalcEfficiency(hm->H1(FindAndReplace(cloneProbHistName, "_CloneProb_", "_Clone_")), hm->H1(FindAndReplace(cloneProbHistName, "_CloneProb_", "_Acc_")), 100.);
      labels[nofStudies+iStudy] = "CloneProb " + GetStudyName(iStudy) + "(" + NumberToString<Double_t>(efficiencies[nofStudies+iStudy], 1) + ")";
      histos[nofStudies+iStudy]->SetAxisRange(0.,9.);
   }

   DrawH1(histos, labels, kLinear, kLinear, true, 0.19, 0.25, 0.8, 0.7);//, "PE1");
//   DrawMeanEfficiencyLines(histos, efficiencies);
}

void CbmLitClusteringQaStudyReport::DrawAccAndRec(
      const string& canvasName,
      const string& histName)
{
   TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
   canvas->SetGrid();
   canvas->cd();

   Int_t nofStudies = HM().size();
   vector<string> labels(nofStudies);
   vector<TH1*> histos(nofStudies);
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
      CbmHistManager* hm = HM()[iStudy];
      Int_t nofEvents = hm->H1("hen_EventNo_ClusteringQa")->GetEntries();
      histos[iStudy] = hm->H1(histName);
      histos[iStudy]->Scale(1./nofEvents);
      histos[iStudy]->SetAxisRange(0.,9.);
      Double_t nofObjects = histos[iStudy]->GetEntries() / nofEvents;
      labels[iStudy] = GetStudyName(iStudy) + "(" + NumberToString<Double_t>(nofObjects, 1) + ")";
   }

   DrawH1(histos, labels, kLinear, kLinear, true, 0.65, 0.25, 0.8, 0.44);
}

Double_t CbmLitClusteringQaStudyReport::CalcEfficiency(
   const TH1* histRec,
   const TH1* histAcc,
   Double_t scale) const
{
   if (histAcc->Integral() == 0 || histRec->Integral() == 0) {
      return 0.;
   } else {
      return scale * Double_t(histRec->Integral()) / Double_t(histAcc->Integral());
   }
}

void CbmLitClusteringQaStudyReport::DrawMeanEfficiencyLines(
   const vector<TH1*>& histos,
   const vector<Double_t>& efficiencies)
{
   assert(histos.size() != 0 && efficiencies.size() == histos.size());

   Double_t minX = histos[0]->GetXaxis()->GetXmin();
   Double_t maxX = 9;// histos[0]->GetXaxis()->GetXmax();
   Int_t nofHistos = histos.size();
   for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
      TLine* line = new TLine(minX, efficiencies[iHist], maxX, efficiencies[iHist]);
      line->SetLineWidth(1);
      line->SetLineColor(histos[iHist]->GetLineColor());
      line->Draw();
   }
}


ClassImp(CbmLitClusteringQaStudyReport)
