
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

	  {
		  TCanvas* c = CreateCanvas("jpsi_McEpmRapidityPt","jpsi_McEpmRapidityPt",600,600);
		  DrawH2(H2("fhMcEpmRapidityPt"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_McEpmMinv","jpsi_McEpmMinv",600,600);
	  	  H1("fhMcEpmMinv")->Scale(1. / H1("fhMcEpmMinv")->Integral());//Scale Yield
	  	  DrawH1(H1("fhMcEpmMinv"));
	  }

	  {
		  TCanvas* c = CreateCanvas("jpsi_AccEpmRapidityPt","jpsi_AccEpmRapidityPt",600,600);
		  DrawH2(H2("fhAccEpmRapidityPt"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_AccEpmMinv","jpsi_AccEpmMinv",600,600);
	  	  H1("fhAccEpmMinv")->Scale(1. / H1("fhAccEpmMinv")->Integral());//Scale Yield
	  	  DrawH1(H1("fhAccEpmMinv"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_CandMcEpmPtY","jpsi_CandMcEpmPtY",600,600);
	  	  DrawH2(H2("fhCandMcEpmPtY"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_CandEpmMinv","jpsi_CandEpmMinv",600,600);
	  	  H1("fhCandEpmMinv")->Scale(1. / H1("fhCandEpmMinv")->Integral());//Scale Yield
	  	  DrawH1(H1("fhCandEpmMinv"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_CandMcEpmPtYChi2PrimCut","jpsi_CandMcEpmPtYChi2PrimCut",600,600);
	  	  DrawH2(H2("fhCandMcEpmPtYChi2PrimCut"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_CandEpmMinvChi2PrimCut","jpsi_CandEpmMinvChi2PrimCut",600,600);
	  	  H1("fhCandEpmMinvChi2PrimCut")->Scale(1. / H1("fhCandEpmMinvChi2PrimCut")->Integral());//Scale Yield
	  	  DrawH1(H1("fhCandEpmMinvChi2PrimCut"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_RecoCandEpmPtYChi2PrimCut","jpsi_RecoCandEpmPtYChi2PrimCut",600,600);
	  	  DrawH2(H2("fhRecoCandEpmPtYChi2PrimCut"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_RecoCandEpmMinvChi2PrimCut","jpsi_RecoCandEpmMinvChi2PrimCut",600,600);
	  	  H1("fhRecoCandEpmMinvChi2PrimCut")->Scale(1. / H1("fhRecoCandEpmMinvChi2PrimCut")->Integral());//Scale Yield
	      DrawH1(H1("fhRecoCandEpmMinvChi2PrimCut"));
	  }

	  DrawCutDistributions();
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


ClassImp(CbmAnaJpsiReport)

