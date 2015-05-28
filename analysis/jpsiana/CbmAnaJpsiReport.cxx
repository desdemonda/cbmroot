
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

	  //e+/- MC
	  {
		  TCanvas* c = CreateCanvas("jpsi_McEpmRapidityPt","jpsi_McEpmRapidityPt",600,600);
		  DrawH2(H2("fhMcEpmRapidityPt"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_McEpmMinv","jpsi_McEpmMinv",600,600);
	  	  H1("fhMcEpmMinv")->Scale(1. / H1("fhMcEpmMinv")->Integral());//Scale Yield
	  	  DrawH1(H1("fhMcEpmMinv"));
	  }

	  //e+/- Accepted
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
	  	  TCanvas* c = CreateCanvas("jpsi_AccEpmMomentum","jpsi_AccEpmMomentum",600,600);
	  	  H1("fhAccEpmMomentum")->Scale(1. / H1("fhAccEpmMomentum")->Integral());//Scale Yield
	  	  DrawH1(H1("fhAccEpmMomentum"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_AccEpmRapidity","jpsi_AccEpmRapidity",600,600);
	  	  H1("fhAccEpmRapidity")->Scale(1. / H1("fhAccEpmRapidity")->Integral());//Scale Yield
	  	  DrawH1(H1("fhAccEpmRapidity"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_AccEpmPt","jpsi_AccEpmPt",600,600);
	  	  H1("fhAccEpmPt")->Scale(1. / H1("fhAccEpmPt")->Integral());//Scale Yield
	  	  DrawH1(H1("fhAccEpmPt"));
	  }

	  //e+/- Candidate reconstructed, using Mc to identify y and P_t
	  {
	  	  TCanvas* c = CreateCanvas("jpsi_CandMcEpmPtY","jpsi_CandMcEpmPtY",600,600);
	  	  DrawH2(H2("fhCandMcEpmPtY"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_CandEpmMinv","jpsi_CandEpmMinv",600,600);
	  	  H1("fhCandEpmMinv")->Scale(1. / H1("fhCandEpmMinv")->Integral());//Scale Yield
	  	  DrawH1(H1("fhCandEpmMinv"));
	  }
	  //e+/- Candidate reconstructed, using Mc to identify y and P_t AND Chi2PrimCut
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
	  	  TCanvas* c = CreateCanvas("jpsi_CandEpmMomentumChi2PrimCut","jpsi_CandEpmMomentumChi2PrimCut",600,600);
	  	  H1("fhCandEpmMomentumChi2PrimCut")->Scale(1. / H1("fhCandEpmMomentumChi2PrimCut")->Integral());//Scale Yield
	  	  DrawH1(H1("fhCandEpmMomentumChi2PrimCut"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_CandMcEpmRapidityChi2PrimCut","jpsi_CandMcEpmRapidityChi2PrimCut",600,600);
	  	  H1("fhCandMcEpmRapidityChi2PrimCut")->Scale(1. / H1("fhCandMcEpmRapidityChi2PrimCut")->Integral());//Scale Yield
	  	  DrawH1(H1("fhCandMcEpmRapidityChi2PrimCut"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_CandMcEpmPtChi2PrimCut","jpsi_CandMcEpmPtChi2PrimCut",600,600);
	  	  H1("fhCandMcEpmPtChi2PrimCut")->Scale(1. / H1("fhCandMcEpmPtChi2PrimCut")->Integral());//Scale Yield
	  	  DrawH1(H1("fhCandMcEpmPtChi2PrimCut"));
	  }

	  //e+/- reconstructed, using IsElectron -> CUTS
	  {
	  	  TCanvas* c = CreateCanvas("jpsi_RecoCandEpmPtYChi2PrimCut","jpsi_RecoCandEpmPtYChi2PrimCut",600,600);
	  	  DrawH2(H2("fhRecoCandEpmPtYChi2PrimCut"));
	  }

	  {
	  	  TCanvas* c = CreateCanvas("jpsi_RecoCandEpmMinvChi2PrimCut","jpsi_RecoCandEpmMinvChi2PrimCut",600,600);
	  	  H1("fhRecoCandEpmMinvChi2PrimCut")->Scale(1. / H1("fhRecoCandEpmMinvChi2PrimCut")->Integral());//Scale Yield
	      DrawH1(H1("fhRecoCandEpmMinvChi2PrimCut"));
	  }

	  {
	   	  TCanvas* c = CreateCanvas("jpsi_RecoCandEpmMomentumChi2PrimCut","jpsi_RecoCandEpmMomentumChi2PrimCut",600,600);
	   	  H1("fhRecoCandEpmMomentumChi2PrimCut")->Scale(1. / H1("fhRecoCandEpmMomentumChi2PrimCut")->Integral());//Scale Yield
	   	  DrawH1(H1("fhRecoCandEpmMomentumChi2PrimCut"));
	  }

	  {
	   	  TCanvas* c = CreateCanvas("jpsi_RecoCandMcEpmRapidityChi2PrimCut","jpsi_RecoCandMcEpmRapidityChi2PrimCut",600,600);
	   	  H1("fhRecoCandMcEpmRapidityChi2PrimCut")->Scale(1. / H1("fhRecoCandMcEpmRapidityChi2PrimCut")->Integral());//Scale Yield
	   	  DrawH1(H1("fhRecoCandMcEpmRapidityChi2PrimCut"));
 	  }

	  {
	   	  TCanvas* c = CreateCanvas("jpsi_RecoCandMcEpmPtChi2PrimCut","jpsi_RecoCandMcEpmPtChi2PrimCut",600,600);
	   	  H1("fhRecoCandMcEpmPtChi2PrimCut")->Scale(1. / H1("fhRecoCandMcEpmPtChi2PrimCut")->Integral());//Scale Yield
	   	  DrawH1(H1("fhRecoCandMcEpmPtChi2PrimCut"));
	  }

	  //Number of Candidates after several cuts
	  {
	  	  TCanvas* c = CreateCanvas("jpsi_NofCandidatesAfterCuts","jpsi_NofCandidatesAfterCuts",600,600);
	      DrawH2(H2("fhNofCandidatesAfterCuts"));
	  }

	  //inv. Mass of Gamma Conversion pairs
	  {
	   	  TCanvas* c = CreateCanvas("jpsi_RecoGammaConvEpmMinv","jpsi_RecoGammaConvEpmMinv",600,600);
	   	  H1("fhRecoGammaConvEpmMinv")->Scale(1. / H1("fhRecoGammaConvEpmMinv")->Integral());//Scale Yield
	      DrawH1(H1("fhRecoGammaConvEpmMinv"));
	  }

	  //inv. Mass of e+- pairs from Pi0
	  {
	   	  TCanvas* c = CreateCanvas("jpsi_RecoPi0EpmMinv","jpsi_RecoPi0EpmMinv",600,600);
	   	  H1("fhRecoPi0EpmMinv")->Scale(1. / H1("fhRecoPi0EpmMinv")->Integral());//Scale Yield
	      DrawH1(H1("fhRecoPi0EpmMinv"));
	  }

	  //inv. Mass of e+- pairs from Pi0
	  {
	   	  TCanvas* c = CreateCanvas("jpsi_BgIdentificationRightWrong","jpsi_BgIdentificationRightWrong",600,600);
	   	  H1("fhBgIdentificationRightWrong")->Scale(1. / H1("fhBgIdentificationRightWrong")->Integral());//Scale Yield
	      DrawH1(H1("fhBgIdentificationRightWrong"));
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

