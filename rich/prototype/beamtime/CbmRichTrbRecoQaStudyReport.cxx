
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
   FitGausAndDrawH1("fhRadiusCircle", "rich_report_radius");
   FitGausAndDrawH1("fhNofHitsInEvent", "rich_report_hits_in_event");
   FitGausAndDrawH1("fhNofHitsInRing", "rich_report_hits_in_ring");
   FitGausAndDrawH1("fhBaxisEllipse", "rich_report_baxis");
   FitGausAndDrawH1("fhAaxisEllipse", "rich_report_aaxis");
   FitGausAndDrawH1("fhDrCircle", "rich_report_dr");
   FitGausAndDrawH1("fhBoverAEllipse", "rich_report_boa");
   DrawEfficiency();
}

void CbmRichTrbRecoQaStudyReport::FitGausAndDrawH1(
		const string& histName,
		const string& canvasName)
{
   Int_t nofStudies = HM().size();
   TCanvas* canvas2 = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 600);
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
		 str.Form(" (%.2f/%.2f)", mean, sigma);
		 legendNames.push_back(GetStudyNames()[iStudy] + string(str.Data()));
     } else {
    	 TString str;
    	 str.Form(" (%.2f/%.2f)", histos1[iStudy]->GetMean(), histos1[iStudy]->GetRMS());
    	 TF1* fit = histos1[iStudy]->GetFunction("gaus");
    	 if (fit != NULL) fit->Delete();
    	 legendNames.push_back(GetStudyNames()[iStudy] + string(str.Data()));
     }
   }
   DrawH1(histos1, legendNames, kLinear, kLinear);
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
