
#include "CbmRichTrbRecoQaStudyReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "CbmUtils.h"
#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include <vector>
#include <iostream>
#include <boost/assign/list_of.hpp>
#include "CbmUtils.h"

using boost::assign::list_of;
using namespace std;


CbmRichTrbRecoQaStudyReport::CbmRichTrbRecoQaStudyReport():
		CbmStudyReport(),
		fFitHist(true)
{
   SetReportName("rich_trb_reco_study_report");
   SetReportTitle("RICH study report");
}

CbmRichTrbRecoQaStudyReport::~CbmRichTrbRecoQaStudyReport()
{
}

void CbmRichTrbRecoQaStudyReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());
   PrintCanvases();
   Out() <<  R()->DocumentEnd();
}

void CbmRichTrbRecoQaStudyReport::Draw()
{
   SetDefaultDrawStyle();
   {
	   TCanvas* canvas2 = CreateCanvas("rich_report_radius", "rich_report_radius", 600, 600);
	   FitGausAndDrawH1("fhRadiusCircle");
   }

   {
	   TCanvas* canvas2 = CreateCanvas("rich_report_hits_in_event", "rich_report_hits_in_event", 600, 600);
	   FitGausAndDrawH1("fhNofHitsInEvent");
   }

   {
	   TCanvas* canvas2 = CreateCanvas("rich_report_hits_in_ring", "rich_report_hits_in_ring", 600, 600);
	   FitGausAndDrawH1("fhNofHitsInRing");
   }

   {
	   TCanvas* canvas2 = CreateCanvas("rich_report_baxis", "rich_report_baxis", 600, 600);
	   FitGausAndDrawH1("fhBaxisEllipse");
   }

   {
	   TCanvas* canvas2 = CreateCanvas("rich_report_aaxis", "rich_report_aaxis", 600, 600);
	   FitGausAndDrawH1("fhAaxisEllipse");
   }

   {
	   TCanvas* canvas2 = CreateCanvas("rich_report_dr", "rich_report_dr", 600, 600);
	   FitGausAndDrawH1("fhDrCircle");
   }

   {
	   TCanvas* canvas2 = CreateCanvas("rich_report_boa", "rich_report_boa", 600, 600);
	   FitGausAndDrawH1("fhBoverAEllipse");
   }

   { // position G
	   TCanvas* canvas2 = CreateCanvas("rich_report_nofhits_pmt_posG", "rich_report_nofhits_pmt_posG", 1200, 1200);
	   canvas2->Divide(2,2);
	   canvas2->cd(1);
	   FitGausAndDrawH1("fhNofHitsInRingPmt_4");
	   canvas2->cd(2);
	   FitGausAndDrawH1("fhNofHitsInRingPmt_0");
	   canvas2->cd(3);
	   FitGausAndDrawH1("fhNofHitsInRingPmt_5");
	   canvas2->cd(4);
	   FitGausAndDrawH1("fhNofHitsInRingPmt_1");
   }

   { // position H
	   TCanvas* canvas2 = CreateCanvas("rich_report_nofhits_pmt_posH", "rich_report_nofhits_pmt_posH", 1200, 1200);
	   canvas2->Divide(2,2);
	   canvas2->cd(1);
	   FitGausAndDrawH1("fhNofHitsInRingPmt_5");
	   canvas2->cd(2);
	   FitGausAndDrawH1("fhNofHitsInRingPmt_1");
	   canvas2->cd(3);
	   FitGausAndDrawH1("fhNofHitsInRingPmt_6");
	   canvas2->cd(4);
	   FitGausAndDrawH1("fhNofHitsInRingPmt_2");
   }


   DrawEfficiency();

}

void CbmRichTrbRecoQaStudyReport::FitGausAndDrawH1(
		const string& histName)
{
   Int_t nofStudies = HM().size();
   vector<TH1*> histos1(nofStudies);
   vector<string> legendNames;
   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
     histos1[iStudy] = HM()[iStudy]->H1(histName);
     histos1[iStudy]->Scale(1./histos1[iStudy]->Integral());
     histos1[iStudy]->SetMaximum(histos1[iStudy]->GetMaximum() * 1.20);

     if (fFitHist) {
		 histos1[iStudy]->Fit("gaus", "Q");
		 TF1* fit = histos1[iStudy]->GetFunction("gaus");
		 Double_t sigma = (NULL != fit) ? fit->GetParameter(2) : 0.;
		 Double_t mean = (NULL != fit) ? fit->GetParameter(1) : 0.;
		 TString str;
		 str.Form(" (%.2f/%.3f)", mean, sigma);
		 legendNames.push_back(GetStudyNames()[iStudy] + string(str.Data()));
     } else {
    	 TString str;
    	 str.Form(" (%.2f/%.3f)", histos1[iStudy]->GetMean(), histos1[iStudy]->GetRMS());
    	 TF1* fit = histos1[iStudy]->GetFunction("gaus");
    	 if (fit != NULL) fit->Delete();
    	 legendNames.push_back(GetStudyNames()[iStudy] + string(str.Data()));
     }
   }
   DrawH1(histos1, legendNames, kLinear, kLinear, true, .5, .7, .99, .99);
   if (fFitHist) {
	   for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
		  histos1[iStudy]->GetFunction("gaus")->SetLineColor(histos1[iStudy]->GetLineColor());
	   }
   }
   gPad->SetGridx(true);
   gPad->SetGridy(true);
}

void CbmRichTrbRecoQaStudyReport::DrawEfficiency()
{
	Int_t nofStudies = HM().size();
	TCanvas* canvas2 = CreateCanvas("rich_report_efficiency", "rich_report_efficiency", 600, 600);
	vector<TH1*> histos1(nofStudies);
	vector<string> legendNames;
	for (UInt_t iStudy = 0; iStudy < nofStudies; iStudy++) {
	    histos1[iStudy] = Cbm::DivideH1(HM()[iStudy]->H1("fhNofHitsInEventWithRing"), HM()[iStudy]->H1("fhNofHitsInEventAll"));
		TString str;
		//str.Form(" (%.2f/%.2f)", mean, sigma);
		legendNames.push_back(GetStudyNames()[iStudy] + string(str.Data()));
	}
	DrawH1(histos1, legendNames, kLinear, kLinear);
}

ClassImp(CbmRichTrbRecoQaStudyReport)
