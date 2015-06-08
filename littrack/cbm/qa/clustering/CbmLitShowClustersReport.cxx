/*
 * \file CbmLitShowClustersReport.cxx
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date Mai, 2015
 * \brief
 */


#include "CbmLitShowClustersReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "CbmUtils.h"
#include "TH1.h"
#include "TF1.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TList.h"
#include "TPaletteAxis.h"
#include <boost/assign/list_of.hpp>
#include <vector>
using std::vector;
using std::cout;
using std::endl;
using std::stringstream;
using boost::assign::list_of;
using Cbm::NumberToString;
using Cbm::Split;
using Cbm::FindAndReplace;


CbmLitShowClustersReport::CbmLitShowClustersReport():
		CbmSimulationReport()
{
   SetReportName("clustering_qa_show_clusters");
}

CbmLitShowClustersReport::~CbmLitShowClustersReport()
{
  vector<TCanvas*> cVector = this->GetCanvases();
  Int_t nofCanvases = cVector.size();
  for (Int_t i = 0; i < nofCanvases; i++) {
    TCanvas* canvas = cVector[i];
    stringstream cPng;
    cPng << this->GetOutputDir() << "png/" << canvas->GetName() << ".png";
    canvas->Print(cPng.str().c_str());
  }

}

void CbmLitShowClustersReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   PrintCanvases();
   Out() <<  R()->DocumentEnd();
}


void CbmLitShowClustersReport::Draw()
{
//   DrawH2ByPattern("hhh_.*Clustering_visualisation.*col_H2", kLinear, kLinear, kLinear, "colz");
  DrawH1ByPattern("hno_NofObjects_digis_per_cluster_H1");

  vector<TH2*> hists00 = HM()->H2Vector("hhh_Layer1_Module.+_Time_visualisation_col_H2_lin");
  Int_t nofModules1 = hists00.size();
  cout << "creating merged canvases for " << nofModules1 << " Modules" << endl;
  for(Int_t i=0; i < nofModules1; ++i){
     vector<string> logV = list_of("_lin")("_log");

     // Creating Sector Visualization (charge, time inside sectors [row,col])
     for(Int_t logC = 0; logC < 2; logC++){
       stringstream histsName;
       histsName << "hhs_Layer1_Module" << i << "_Sector.*Clustering_visualisation_col_H2" << logV[logC];
       // e.g.: hhs_Layer1_Module0_Sector0_Clustering_visualisation_col_H2_lin
       vector<TH2*> hists = HM()->H2Vector(histsName.str()); // ->H2(histName);

       cout << "creating merged canvas (" << histsName.str() << ", n=" << hists.size() << ")" << endl;
       if(hists.size() != 0){
	 string canvasName = GetReportName() + "_" + hists[0]->GetName() + "_merged";
	 TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 900);
	 Int_t j = 1;
	 canvas->Divide(1, 3);
	 for(const auto& h : hists){
	    if(j>3) break;
	    canvas->cd(j++);
	    DrawH2(h, kLinear, kLinear, (logC==1)?kLog:kLinear, "colz");
	 }
       }
     }

     // creating Module (X/Y) canvas with lin/log comparison
     stringstream canvasName2;
     canvasName2 << GetReportName() << "_hhh_mod" << i << "_Charge_Visualisation_merged";
     TCanvas* canvas2 = CreateCanvas(canvasName2.str().c_str(), canvasName2.str().c_str(), 1600, 900);
     Int_t j = 1;
     canvas2->Divide(2, 1);
     for(Int_t logC = 0; logC < 2; logC++){
	stringstream histsName2;
	histsName2 << "hhh_Layer1_Module" << i << "_Charge_visualisation_col_H2" << logV[logC];
	// e.g.: hhh_Layer1_Module0_Charge_visualisation_col_H2_lin
	vector<TH2*> hists2 = HM()->H2Vector(histsName2.str());
	cout << "creating merged canvas (" << histsName2.str() << ", n=" << hists2.size() << ")" << endl;
	if(hists2.size() != 0){
	    canvas2->cd(j++);
	    DrawH2(hists2[0], kLinear, kLinear, (logC==1)?kLog:kLinear, "colz");
	}
     }

     // Creating Cluster Visualization (Charge and Time)
     vector<string> flavour = list_of("Charge")("Time");

     for(Int_t flavourCounter = 0; flavourCounter < flavour.size(); flavourCounter++){
	 stringstream canvasName, canvasTitle, histNames0;
	 canvasName << "Module" << i << flavour[flavourCounter] << "ClusteringVisualization";
	 canvasTitle << "Module " << i << " " << flavour[flavourCounter] << " Clustering visualization";

	 histNames0 << "hhc_Layer1_Module" << i << "_Clustering_visualisation" << flavourCounter << "_col_H2_cluster.*";
	 // e.g.: hhc_Layer1_Module0_Clustering_visualisation0_col_H2_cluster1
	 vector<TH2*> hists0 = HM()->H2Vector(histNames0.str());
	 cout << "creating cluster canvas (" << histNames0.str() << ", n=" << hists0.size() << ")" << endl;
	 if(hists0.size() == 0) continue;

	 TCanvas *c1 = CreateCanvas(canvasName.str().c_str(), canvasTitle.str().c_str(), 1600, 900);
	 vector<string> labels;
	 for(const auto& h : hists0){
	     labels.insert(labels.end(), h->GetTitle());
	 }

	 cout << "Filling " << canvasName.str().c_str() << " canvas with " << hists0.size() << " histograms." << endl;

	 string drawOpt = "COLZ";
	 Double_t max = std::numeric_limits<Double_t>::min();
	 Int_t nofHistos = hists0.size();
	 TH2* tempH = hists0[0];
	 TLegend* legend = new TLegend( 0.65, 0.75, 0.95, 0.99 );
	 legend->SetFillColor(kWhite);
	 for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
	    TH2* hist = hists0[iHist];
	    string opt = (iHist == 0) ? drawOpt : ("SAME" + drawOpt);
	    DrawH2(hist, kLinear, kLinear, kLinear, opt);
	    max = std::max(max, hist->GetMaximum());
	    legend->AddEntry(hist, labels[iHist].c_str(), "lp");
	 }
	 hists0[0]->SetMaximum(max * 1.17);
	 legend->Draw();
	 TPaletteAxis *p = (TPaletteAxis*)hists0[0]->GetListOfFunctions()->FindObject("palette");
	 TGaxis *a = p->GetAxis();
	 //a->SetGridLength(nofHistos/4.0);
	 c1->Update();
     }
  } // end for (i < nofModules1)

}

ClassImp(CbmLitShowClustersReport)
