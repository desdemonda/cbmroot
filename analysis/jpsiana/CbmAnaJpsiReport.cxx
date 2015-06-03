
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "CbmUtils.h"
#include "TH1.h"
#include "TF1.h"
#include "TPad.h"
#include "TDirectory.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TMarker.h"
#include <boost/assign/list_of.hpp>
#include <map>
#include <iostream>
#include "CbmAnaJpsiReport.h"
#include "CbmAnaJpsiHist.h"
#include "CbmAnaJpsiCuts.h"
#include "TLine.h"

using boost::assign::list_of;
using Cbm::NumberToString;
using Cbm::Split;
using std::map;
using std::endl;
using std::cout;

CbmAnaJpsiReport::CbmAnaJpsiReport():
		CbmSimulationReport()
{
	SetReportName("jpsi_qa");
}

CbmAnaJpsiReport::~CbmAnaJpsiReport()
{
}

void CbmAnaJpsiReport::Create()
{
   //Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   PrintCanvases();

   Out() << R()->DocumentEnd();
}

void CbmAnaJpsiReport::Draw()
{
	  SetDefaultDrawStyle();

	  // Draw RICH hits distribution
	   Draw2DCut("fhRichPmtXY");


	  //Number of Candidates after several cuts
	  {
	  	  TCanvas* c = CreateCanvas("jpsi_NofCandidatesAfterCuts","jpsi_NofCandidatesAfterCuts",600,600);
	      DrawH2(H2("fhNofCandidatesAfterCuts"));
	  }


	  //inv. Mass of e+- pairs from Pi0
	  {
	   	  TCanvas* c = CreateCanvas("jpsi_BgIdentificationRightWrong","jpsi_BgIdentificationRightWrong",600,600);
	   	  H1("fhBgIdentificationRightWrong")->Scale(1. / H1("fhBgIdentificationRightWrong")->Integral());//Scale Yield
	      DrawH1(H1("fhBgIdentificationRightWrong"));
	  }

	  DrawCutDistributions();

	  DrawDistributions();

	   // Number of BG and signal tracks after each cut
	  {
	   	  TCanvas* c = CreateCanvas("jpsi_nof_bg_tracks","jpsi_nof_bg_tracks",600,600);
	   	  H1("fh_nof_bg_tracks")->Scale(1. / H1("fh_nof_bg_tracks")->Integral());//Scale Yield
	      DrawH1(H1("fh_nof_bg_tracks"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_nof_el_tracks","jpsi_nof_el_tracks",600,600);
	  	  H1("fh_nof_el_tracks")->Scale(1. / H1("fh_nof_el_tracks")->Integral());//Scale Yield
	  	  DrawH1(H1("fh_nof_el_tracks"));
	  }

	  {
	   	  TCanvas* c = CreateCanvas("jpsi_fh_nof_el_tracks","jpsi_fh_nof_el_tracks",600,600);
	  	  DrawH2(H2("fh_source_tracks"));
	  }

	  //mismatches
	  {
	  	  TCanvas* c = CreateCanvas("jpsi_nof_mismatches","jpsi_nof_mismatches",600,600);
	  	  H1("fh_nof_mismatches")->Scale(1. / H1("fh_nof_mismatches")->Integral());//Scale Yield
	  	  DrawH1(H1("fh_nof_mismatches"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_nof_mismatches_rich","jpsi_nof_mismatches_rich",600,600);
	  	  H1("fh_nof_mismatches_rich")->Scale(1. / H1("fh_nof_mismatches_rich")->Integral());//Scale Yield
	  	  DrawH1(H1("fh_nof_mismatches_rich"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_nof_mismatches_trd","jpsi_nof_mismatches_trd",600,600);
	  	  H1("fh_nof_mismatches_trd")->Scale(1. / H1("fh_nof_mismatches_trd")->Integral());//Scale Yield
	  	  DrawH1(H1("fh_nof_mismatches_trd"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_nof_mismatches_tof","jpsi_nof_mismatches_tof",600,600);
	  	  H1("fh_nof_mismatches_tof")->Scale(1. / H1("fh_nof_mismatches_tof")->Integral());//Scale Yield
	  	  DrawH1(H1("fh_nof_mismatches_tof"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_nof_ghosts","jpsi_nof_ghosts",600,600);
	  	  //H1("fh_nof_ghosts")->Scale(1. / H1("fh_nof_ghosts")->Integral());//Scale Yield
	  	  DrawH1(H1("fh_nof_ghosts"));
	  }
}

void CbmAnaJpsiReport::DrawAnalysisStepsSourceTypesH1(
	      const string& hName,
	      bool doScale)
{
	for (int i=0; i<CbmAnaJpsiHist::fNofSourceTypes;i++)
	{

		{
		TCanvas *c = CreateCanvas( ("jpsi_" + hName + "_" + CbmAnaJpsiHist::fSourceTypes[i]).c_str(), ("jpsi_" + hName + "_" + CbmAnaJpsiHist::fSourceTypes[i]).c_str(), 600, 600);
		vector<TH1*> h;
		vector<string> hLegend;
		string hNameNew = "";
		hNameNew = hName + "_" + CbmAnaJpsiHist::fSourceTypes[i];
		DrawAnalysisStepsH1( hNameNew ,doScale);
		}
	}

}



void CbmAnaJpsiReport::DrawAnalysisStepsH2(
      const string& hName)
{
   for (int i = 0;i< CbmAnaJpsiHist::fNofAnaSteps; i++)
   {
	   {
		  string h = hName+"_"+CbmAnaJpsiHist::fAnaSteps[i];
		  TCanvas* c = CreateCanvas(("jpsi_"+h).c_str(),("jpsi_"+h).c_str(),600,600);
	   	  H1(h)->Scale(1. / H1(h)->Integral());//Scale Yield
	   	  DrawH2(H2(h));
	   }
   }
}

void CbmAnaJpsiReport::DrawAnalysisStepsH1(
      const string& hName,
      bool doScale)
{	TCanvas* c = CreateCanvas( ("jpsi_" + hName).c_str(), ("jpsi_" + hName).c_str(), 600, 600);
   vector<TH1*> h;
   vector<string> hLegend;
   for (int i = 0; i < CbmAnaJpsiHist::fNofAnaSteps; i++){
      string fullName = hName+"_"+CbmAnaJpsiHist::fAnaSteps[i];
      h.push_back( H1(fullName) );
      h[i]->SetLineWidth(2);
      h[i]->SetLineColor(CbmAnaJpsiHist::fAnaStepsColor[i]);
      if (doScale) h[i]->Scale(1. / h[i]->Integral());
      hLegend.push_back( CbmAnaJpsiHist::fAnaStepsLatex[i] );
   }
   DrawH1(h, hLegend, kLinear, kLog, true, 0.90, 0.7, 0.99, 0.99);
}

void CbmAnaJpsiReport::DrawSourceTypesH1(
      const string& hName,
      bool doScale)
{
   vector<TH1*> h;
   vector<string> hLegend;
   for (int i = 0; i < CbmAnaJpsiHist::fNofSourceTypes; i++){
      string fullName = hName+"_"+CbmAnaJpsiHist::fSourceTypes[i];
      h.push_back( H1(fullName) );
      h[i]->SetLineWidth(2);
      h[i]->SetLineColor(CbmAnaJpsiHist::fSourceTypesColor[i]);
      if (doScale) h[i]->Scale(1. / h[i]->Integral());
      hLegend.push_back( CbmAnaJpsiHist::fSourceTypesLatex[i] );
   }
   DrawH1(h, hLegend, kLinear, kLog, true, 0.90, 0.7, 0.99, 0.99);
}

void CbmAnaJpsiReport::DrawCutH1(
      const string& hName,
      double cutValue)
{
   TCanvas *c = CreateCanvas( ("jpsi_" + hName).c_str(), ("jpsi_" + hName).c_str(), 600, 600);
   DrawSourceTypesH1(hName);
   if (cutValue != -999999.){
      TLine* cutLine = new TLine(cutValue, 0.0, cutValue, 1.);
      cutLine->SetLineWidth(2);
      cutLine->Draw();
   }
}

void CbmAnaJpsiReport::Draw2DCut(
      const string& hist)
{
   TCanvas *c = CreateCanvas(("jpsi_" + hist).c_str(), ("jpsi_" + hist).c_str(), 900, 900);
   c->Divide(2,2);
   for (int i = 0; i < CbmAnaJpsiHist::fNofSourceTypes; i++){
      c->cd(i+1);
      DrawH2(H2( hist + "_"+ CbmAnaJpsiHist::fSourceTypes[i]));
      DrawTextOnPad(CbmAnaJpsiHist::fSourceTypesLatex[i], 0.6, 0.89, 0.7, 0.99);
   }
}

void CbmAnaJpsiReport::DrawCutDistributions()
{
   DrawCutH1("fhChi2PrimEl", 2.0);
   DrawCutH1("fhMomEl", 5.0);
   DrawCutH1("fhChi2StsEl", 2.0);
   DrawCutH1("fhRapidityEl", 2.0);
   DrawCutH1("fhPtEl", 2.0);
   DrawCutH1("fhRichAnn", 2.0);
   DrawCutH1("fhTrdAnn", 2.0);
   Draw2DCut("fhTofM2");

}

void CbmAnaJpsiReport::DrawDistributions()
{
	   DrawAnalysisStepsH2("fh_vertex_el_gamma_xz");
	   DrawAnalysisStepsH2("fh_vertex_el_gamma_yz");
	   DrawAnalysisStepsH2("fh_vertex_el_gamma_xy");
	   DrawAnalysisStepsH2("fh_vertex_el_gamma_rz");
	   DrawAnalysisStepsH1("fh_bg_minv",false);
	   DrawAnalysisStepsH1("fh_pi0_minv",true);
	   DrawAnalysisStepsH2("fh_pi0_minv_pt");
	   DrawAnalysisStepsH1("fh_gamma_minv",true);
	   DrawAnalysisStepsH1("fh_bg_truematch_minv",false);
	   DrawAnalysisStepsH1("fh_bg_truematch_el_minv",false);
	   DrawAnalysisStepsH1("fh_bg_truematch_notel_minv",false);
	   DrawAnalysisStepsH1("fh_bg_mismatch_minv",false);
	   DrawAnalysisStepsH2("fh_signal_minv_pt");
	   DrawAnalysisStepsH1("fh_signal_mom",true);
	   DrawAnalysisStepsH1("fh_signal_minv",true);
	   DrawAnalysisStepsH2("fh_signal_pty");
	   DrawAnalysisStepsH1("fh_signal_pt",true);
	   DrawAnalysisStepsH1("fh_signal_rapidity",true);
	   DrawAnalysisStepsSourceTypesH1("fh_source_mom",true);
	   DrawAnalysisStepsSourceTypesH1("fh_source_pt",false);
}

ClassImp(CbmAnaJpsiReport)

