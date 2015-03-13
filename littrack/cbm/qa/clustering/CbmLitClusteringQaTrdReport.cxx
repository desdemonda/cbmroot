/*
 * \file CbmLitClusteringQaTrdReport.cxx
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date Dec, 2014
 * \brief
 */


#include "CbmLitClusteringQaTrdReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "CbmUtils.h"
#include "TH1.h"
#include "TF1.h"
#include "TProfile.h"
#include "TCanvas.h"
#include <boost/assign/list_of.hpp>
#include <vector>
using std::vector;
using std::endl;
using std::stringstream;
using boost::assign::list_of;
using Cbm::NumberToString;
using Cbm::Split;
using Cbm::FindAndReplace;

string TrdDefaultHitEfficiencyLabelFormatter(
      const string& histName,
      const CbmHistManager* hm)
{
   Double_t efficiency = (histName.find("_Eff_") != string::npos) ?
         CbmLitClusteringQaTrdReport::CalcEfficiency(hm->H1(FindAndReplace(histName, "_Eff_", "_Rec_")), hm->H1(FindAndReplace(histName, "_Eff_", "_Acc_")), 100.) :
         CbmLitClusteringQaTrdReport::CalcEfficiency(hm->H1(FindAndReplace(histName, "_CloneProb_", "_Clone_")), hm->H1(FindAndReplace(histName, "_CloneProb_", "_Acc_")), 100.);
   vector<string> split = Split(histName, '_');
   return split[1] + ":" + split[3] + "(" + NumberToString<Double_t>(efficiency, 1) + ")";
}

string TrdDefaultAccAndRecLabelFormatter(
      const string& histName,
      const CbmHistManager* hm)
{
   Int_t nofEvents = hm->H1("hen_EventNo_ClusteringQa")->GetEntries();
   vector<string> split = Split(histName, '_');
   return split[3] + " (" + NumberToString<Double_t>(hm->H1(histName)->GetEntries() / nofEvents, 1) + ")";
}

CbmLitClusteringQaTrdReport::CbmLitClusteringQaTrdReport():
		CbmSimulationReport()
{
   SetReportName("clustering_qa_trd");
}

CbmLitClusteringQaTrdReport::~CbmLitClusteringQaTrdReport()
{

}

void CbmLitClusteringQaTrdReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   Out() << "Number of events: " << HM()->H1("hen_EventNo_ClusteringQa")->GetEntries() << endl;

   Out() << PrintNofObjects();

   PrintCanvases();
   Out() <<  R()->DocumentEnd();
}

string CbmLitClusteringQaTrdReport::PrintNofObjects() const
{
   vector<TH1*> histos = HM()->H1Vector("hno_NofObjects_.+_Event");
   Int_t nofHistos = histos.size();
   string str = R()->TableBegin("Number of objects per event", list_of("Name")("Value"));
   for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
      string cellName = Split(histos[iHist]->GetName(), '_')[2];
      str += R()->TableRow(list_of(cellName)(NumberToString<Int_t>(histos[iHist]->GetMean())));
   }
   str += R()->TableEnd();
   return str;
}

void CbmLitClusteringQaTrdReport::Draw()
{
   ScaleAndShrinkHistograms();
   CalculateEfficiencyHistos("Acc", "Rec", "Eff");
   CalculateEfficiencyHistos("Acc", "Clone", "CloneProb");

   DrawNofObjectsHistograms("Trd", "Event");
   DrawNofObjectsHistograms("Trd", "Station");

   DrawH1ByPattern("hpa_.*Cluster_NofDigisInCluster_H1");
   DrawH2ByPattern("hpa_.*Cluster_NofDigisInCluster_H2", kLinear, kLinear, kLinear, "colz");

   DrawH1ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H1");
   DrawH2ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H2", kLinear, kLinear, kLinear, "colz");

   DrawH1ByPattern("hpa_.*Hit_Sigma.*_H1");
   DrawH2ByPattern("hpa_.*Hit_Sigma.*_H2", kLinear, kLinear, kLinear, "colz");

   DrawH2ByPattern("hhh_.*Clustering_visualisation.*col_H2", kLinear, kLinear, kLinear, "colz");
   DrawH2ByPattern("hhh_.*Clustering_visualisation.*cont_H2", kLinear, kLinear, kLinear, "contz");

   DrawResidualsAndPulls("Trd");

   vector<TH1*> histos = HM()->H1Vector("hhe_Trd_All_(Acc|Rec|Clone)_Station");
   Int_t nofHistos = histos.size();
   for(Int_t iHisto = 0; iHisto < nofHistos; iHisto++){
      histos[iHisto]->SetAxisRange(0.,9.);
      histos[iHisto]->SetMinimum(0.);
      histos[iHisto]->SetMaximum(1400.);
   }
   DrawH1ByPattern("hhe_Trd_All_(Acc|Rec|Clone)_Station", TrdDefaultAccAndRecLabelFormatter);
   DrawH1ByPattern("hhe_Trd_All_(Eff|CloneProb)_Station", TrdDefaultHitEfficiencyLabelFormatter);

}

void CbmLitClusteringQaTrdReport::DrawNofObjectsHistograms(
	  const string& detName,
      const string& parameter)
{
   if (!HM()->Exists("hno_NofObjects_" + detName + "Points_" + parameter)) return;
   string canvasName = GetReportName() + "_NofObjects_" + detName + "_" + parameter;
   TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 500);
   canvas->SetGrid();
   canvas->cd();
   vector<string> labels = list_of("Points")("Digis")("Clusters")("Hits");
   vector<TH1*> histos = list_of(HM()->H1("hno_NofObjects_" + detName + "Points_" + parameter))
      (HM()->H1("hno_NofObjects_" + detName + "Digis_" + parameter))
      (HM()->H1("hno_NofObjects_" + detName + "Clusters_" + parameter));
   if (HM()->Exists("hno_NofObjects_" + detName + "PixelHits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "PixelHits_" + parameter));
   else if (HM()->Exists("hno_NofObjects_" + detName + "StrawHits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "StrawHits_" + parameter));
   else if (HM()->Exists("hno_NofObjects_" + detName + "Hits_" + parameter)) histos.push_back(HM()->H1("hno_NofObjects_" + detName + "Hits_" + parameter));
   DrawH1(histos, labels, kLinear, kLinear, true, 0.65, 0.75, 0.95, 0.99);
}

void CbmLitClusteringQaTrdReport::DrawResidualsAndPulls(
      const string& detName)
{
   if (!(HM()->Exists("hrp_" + detName + "_ResidualX_H2") && HM()->Exists("hrp_" + detName + "_ResidualY_H2")
         && HM()->Exists("hrp_" + detName + "_PullX_H2") && HM()->Exists("hrp_" + detName + "_ResidualX_H2"))) return;
   vector<string> par = list_of("ResidualX")("ResidualY")("PullX")("PullY");
   Int_t nofCanvases = par.size();
   for (Int_t iCanvas = 0; iCanvas < nofCanvases; iCanvas++) {
      string histName = "hrp_" + detName + "_" + par[iCanvas] + "_H2";
      TH2* hist = HM()->H2(histName);
      string canvasName = GetReportName() + "_" + histName + "_station";
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1600, 900);
      Int_t nofBins = 10;//hist->GetXaxis()->GetNbins();
      Int_t nofColumns = 5;
      Int_t nofRows = (nofBins / nofColumns) + ((nofBins % 5 == 0) ? 0 : 1);
      canvas->Divide(nofColumns, nofRows);

      std::cout << nofBins << " " << nofColumns << " " << nofRows << std::endl;
      for (Int_t iBin = 1; iBin <= nofBins; iBin++){
         stringstream ss;
         ss << histName << "_" << iBin << "_py";
         TH1* projY = hist->ProjectionY(ss.str().c_str(), iBin, iBin);
         projY->SetNameTitle(ss.str().c_str(), ss.str().c_str());
         projY->SetXTitle(par[iCanvas].c_str());
         projY->SetYTitle("Yield");
         canvas->cd(iBin);
         DrawH1(projY, kLinear, kLinear);
         projY->Fit("gaus");
      }
   }
   DrawH2ByPattern("hrp_" + detName + "_.*_H2", kLinear, kLinear, kLinear, "colz");
}

Double_t CbmLitClusteringQaTrdReport::CalcEfficiency(
   const TH1* histRec,
   const TH1* histAcc,
   Double_t scale)
{
   if (histAcc->Integral() == 0 || histRec->Integral() == 0) {
      return 0.;
   } else {
      return scale * Double_t(histRec->Integral()) / Double_t(histAcc->Integral());
   }
}

void CbmLitClusteringQaTrdReport::ScaleAndShrinkHistograms()
{
   Int_t nofEvents = HM()->H1("hen_EventNo_ClusteringQa")->GetEntries();

   HM()->ScaleByPattern("hhe_.+_.+_(Acc|Rec|Clone)_Station", 1. / nofEvents);

   HM()->ScaleByPattern("hno_NofObjects_.*_Station", 1. / nofEvents);
   HM()->ShrinkEmptyBinsH1ByPattern("hno_NofObjects_.*_Station");

   HM()->NormalizeToIntegralByPattern("hpa_.*Cluster_NofDigisInCluster_.*");
   HM()->ShrinkEmptyBinsH1ByPattern("hpa_.*Cluster_NofDigisInCluster_H1");
   HM()->ShrinkEmptyBinsH2ByPattern("hpa_.*Cluster_NofDigisInCluster_H2");

   HM()->NormalizeToIntegralByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_.*");
   HM()->ShrinkEmptyBinsH1ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H1");
   HM()->ShrinkEmptyBinsH2ByPattern("hpa_.*(Digi|Cluster|Hit)_NofPointsIn(Digi|Cluster|Hit)_H2");

   HM()->NormalizeToIntegralByPattern("hrp_.*_.*_H2");
   HM()->ShrinkEmptyBinsH2ByPattern("hrp_.*_.*_H2");

   HM()->NormalizeToIntegralByPattern("hpa_.*Hit_Sigma.*_.*");
   HM()->ShrinkEmptyBinsH1ByPattern("hpa_.*Hit_Sigma.*_H1");
   HM()->ShrinkEmptyBinsH2ByPattern("hpa_.*Hit_Sigma.*_H2");
}

void CbmLitClusteringQaTrdReport::DivideHistos(
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

void CbmLitClusteringQaTrdReport::CalculateEfficiencyHistos(
      const string& acc,
      const string& rec,
      const string& eff)
{
    vector<TH1*> effHistos = HM()->H1Vector("hhe_.+_" + eff + "_.+");
    Int_t nofEffHistos = effHistos.size();
    for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
      TH1* effHist = effHistos[iHist];
      string effHistName = effHist->GetName();
      string accHistName = FindAndReplace(effHistName, "_" + eff + "_", "_" + acc + "_");
      string recHistName = FindAndReplace(effHistName, "_" + eff + "_", "_" + rec + "_");
      DivideHistos(HM()->H1(recHistName), HM()->H1(accHistName), effHist, 100.);
      effHist->SetMinimum(0.);
      effHist->SetMaximum(100.);
      effHist->SetAxisRange(0.,9.);
    }
}

ClassImp(CbmLitClusteringQaTrdReport)
