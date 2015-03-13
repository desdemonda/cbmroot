/**
 * \file CbmLitTrackingQaReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTrackingQaReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "CbmUtils.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLine.h"
#include <boost/assign/list_of.hpp>
#include <vector>
#include <set>

using Cbm::NumberToString;
using Cbm::FindAndReplace;
using Cbm::Split;
using std::vector;
using std::set;
using std::endl;
using std::make_pair;
using std::pair;
using boost::assign::list_of;

string DefaultEfficiencyLabelFormatter(
      const string& histName,
      Double_t efficiency)
{
   vector<string> split = Split(histName, '_');
   return split[1] + ":" + split[3] + "(" + NumberToString<Double_t>(efficiency, 1) + ")";
}

string ElectronIdEfficiencyLabelFormatter(
      const string& histName,
      Double_t efficiency)
{
   vector<string> split = Split(histName, '_');
   return FindAndReplace(split[1], "Sts", "") + " (" + NumberToString<Double_t>(efficiency, 1) + ")";
}

string DefaultPionSuppressionLabelFormatter(
      const string& histName,
      Double_t efficiency)
{
   vector<string> split = Split(histName, '_');
   return split[1] + " (" + NumberToString<Double_t>(efficiency, 1) + ")";
}

CbmLitTrackingQaReport::CbmLitTrackingQaReport():
		CbmSimulationReport(),
		fGlobalTrackVariants()
{
	SetReportName("tracking_qa");
}

CbmLitTrackingQaReport::~CbmLitTrackingQaReport()
{
}

void CbmLitTrackingQaReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   Out() << "Number of events: " << HM()->H1("hen_EventNo_TrackingQa")->GetEntries() << endl;

   Out() << PrintNofObjects();
   Out() << PrintTrackHits();
   Out() << PrintNofGhosts();
   Out() << PrintTrackingEfficiency(false, false);
   Out() << PrintTrackingEfficiency(true, false);
   Out() << PrintTrackingEfficiency(false, true);
   Out() << PrintTrackingEfficiency(true, true);
   Out() << PrintPionSuppression();

   PrintCanvases();

   Out() <<  R()->DocumentEnd();
}

string CbmLitTrackingQaReport::PrintNofObjects() const
{
	vector<TH1*> histos = HM()->H1Vector("hno_NofObjects_.+");
	Int_t nofHistos = histos.size();
	string str = R()->TableBegin("Number of objects per event", list_of("Name")("Value"));
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string cellName = Split(histos[iHist]->GetName(), '_')[2];
		str += R()->TableRow(list_of(cellName)(NumberToString<Int_t>(histos[iHist]->GetMean())));
	}
   str += R()->TableEnd();
   return str;
}

string CbmLitTrackingQaReport::PrintTrackHits() const
{
	vector<TH1*> histos = HM()->H1Vector("hth_.+_TrackHits_All");
	Int_t nofHistos = histos.size();
	string str = R()->TableBegin("Number of all, true and fake hits in tracks and rings",
	   	         list_of("")("all")("true")("fake")("true/all")("fake/all"));
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string name = histos[iHist]->GetName();
		string cellName = Split(name, '_')[1];
  	    string all = NumberToString<Double_t>(histos[iHist]->GetMean(), 2);
  	    string trueh = NumberToString<Double_t>(HM()->H1(FindAndReplace(name, "_All", "_True"))->GetMean(), 2);
  	    string fakeh = NumberToString<Double_t>(HM()->H1(FindAndReplace(name, "_All", "_Fake"))->GetMean(), 2);
  	    string toa = NumberToString<Double_t>(HM()->H1(FindAndReplace(name, "_All", "_TrueOverAll"))->GetMean(), 2);
  	    string foa = NumberToString<Double_t>(HM()->H1(FindAndReplace(name, "_All", "_FakeOverAll"))->GetMean(), 2);
  	    str += R()->TableRow(list_of(cellName)(all)(trueh)(fakeh)(toa)(foa));
	}
  	str += R()->TableEnd();
  	return str;
}

string CbmLitTrackingQaReport::PrintNofGhosts() const
{
	Double_t nofEvents = HM()->H1("hen_EventNo_TrackingQa")->GetEntries();
	vector<TH1*> histos = HM()->H1Vector("hng_NofGhosts_.+");
	Int_t nofHistos = histos.size();
	string str = R()->TableBegin("Number of ghosts per event", list_of("Name")("Value"));
	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
		string cellName = Split(histos[iHist]->GetName(), '_')[2];
		str += R()->TableRow(list_of(cellName)(NumberToString<Double_t>(histos[iHist]->GetEntries() / nofEvents, 2)));
	}
	str += R()->TableEnd();
	return str;
}

string CbmLitTrackingQaReport::PrintTrackingEfficiency(
		Bool_t includeRich,
		Bool_t isPidEfficiency) const
{
  	// If includeRich == true than search for tracking efficiency histograms which contain "Rich"
  	// otherwise search for tracking efficiency histograms excluding those which contain "Rich"
  	string effRegex = "";
  	if (isPidEfficiency) effRegex = (includeRich) ? "hte_.*Rich.*_Eff_p" : "hte_((?!Rich).)*_Eff_p";
  	else effRegex = (includeRich) ? "hpe_.*Rich.*_Eff_p" : "hpe_((?!Rich).)*_Eff_p";

  	vector<TH1*> histos = HM()->H1Vector(effRegex);
  	Int_t nofHistos = histos.size();
  	if (nofHistos == 0) return "";

  	// Find track and ring categories from the histogram names
  	map<string, Int_t> catToCell;
  	for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
  	   string effName = histos[iHist]->GetName();
           pair<string, Int_t> tmp = make_pair(Split(effName, '_')[3], catToCell.size());
  	   catToCell.insert(tmp);
  	}
  	Int_t nofCats = catToCell.size();
	Int_t nofRows = nofHistos / nofCats;

  	vector<string> cat(nofCats);
  	map<string, Int_t>::const_iterator it;
  	for (it = catToCell.begin(); it != catToCell.end(); it++) { cat[(*it).second] = (*it).first; }

	Int_t nofEvents = HM()->H1("hen_EventNo_TrackingQa")->GetEntries();
	string str = R()->TableBegin("Tracking efficiency", list_of(string("")).range(cat));
	Int_t histCounter = 0;
  	for (Int_t iRow = 0; iRow < nofRows; iRow++) {
  		vector<string> cells(nofCats);
  		string rowName;
  		for (Int_t iCat = 0; iCat < nofCats; iCat++) {
			string effName = histos[histCounter]->GetName();
			string accName = FindAndReplace(effName, "_Eff_", "_Acc_");
			string recName = FindAndReplace(effName, "_Eff_", "_Rec_");
			Double_t acc = HM()->H1(accName)->GetEntries() / nofEvents;
			Double_t rec = HM()->H1(recName)->GetEntries() / nofEvents;
			Double_t eff = (acc != 0.) ? 100. * rec / acc : 0.;
			string accStr = NumberToString<Double_t>(acc);
			string recStr = NumberToString<Double_t>(rec);
			string effStr = NumberToString<Double_t>(eff);
			vector<string> split = Split(effName, '_');
			cells[catToCell[split[3]]] = effStr + "(" + recStr + "/" + accStr + ")";
			histCounter++;
			rowName = split[1] + " (" + split[2] + ")";
  		}
  		str += R()->TableRow(list_of(rowName).range(cells));
	}
  	str += R()->TableEnd();
  	return str;
}

string CbmLitTrackingQaReport::PrintPionSuppression() const
{
   vector<TH1*> histos = HM()->H1Vector("hps_.*_PionSup_p");
   Int_t nofHistos = histos.size();
   if (nofHistos == 0) return "";

   Int_t nofEvents = HM()->H1("hen_EventNo_TrackingQa")->GetEntries();
   string str = R()->TableBegin("Pion suppression", list_of(string(""))("Pion suppression"));
   for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
         string psName = histos[iHist]->GetName();
         string recName = FindAndReplace(psName, "_PionSup_", "_Rec_");
         string recPionsName = FindAndReplace(psName, "_PionSup_", "_RecPions_");
         Double_t rec = HM()->H1(recName)->GetEntries() / nofEvents;
         Double_t recPions = HM()->H1(recPionsName)->GetEntries() / nofEvents;
         Double_t pionSup = (rec != 0.) ? recPions / rec : 0.;
         string pionSupStr = NumberToString<Double_t>(pionSup) + " (" + NumberToString<Double_t>(recPions) + "/" + NumberToString<Double_t>(rec) + ")";
         vector<string> split = Split(psName, '_');
         string rowName = split[1];
         string category = split[2];
         str += R()->TableRow(list_of(rowName+ " - " + category)(pionSupStr));
   }
   str += R()->TableEnd();
   return str;
}


void CbmLitTrackingQaReport::Draw()
{
   CalculateEfficiencyHistos();
   CalculatePionSuppressionHistos();
   FillGlobalTrackVariants();
	SetDefaultDrawStyle();
	DrawEfficiencyHistos();
	DrawYPtHistos();
	DrawHitsHistos();
}

void CbmLitTrackingQaReport::DrawEfficiencyHistos()
{
	// Draw global tracking efficiency
	for (UInt_t i = 0; i < fGlobalTrackVariants.size(); i++) {
		string variant = fGlobalTrackVariants[i];
		DrawEfficiency("tracking_qa_global_tracking_efficiency_all_" + variant + "_p", "hte_Sts.*_" + variant + "_All_Eff_p", DefaultEfficiencyLabelFormatter);
		DrawEfficiency("tracking_qa_global_tracking_efficiency_muon_" + variant + "_p", "hte_Sts.*_" + variant + "_Muon_Eff_p", DefaultEfficiencyLabelFormatter);
		DrawEfficiency("tracking_qa_global_tracking_efficiency_electron_" + variant + "_p", "hte_Sts.*_" + variant + "_Electron_Eff_p", DefaultEfficiencyLabelFormatter);
		DrawEfficiency("tracking_qa_pid_efficiency_electron_" + variant + "_p", "hpe_((?!Sts_))Sts.*_" + variant + "_Electron_Eff_p", ElectronIdEfficiencyLabelFormatter);
		//DrawEfficiency("tracking_qa_global_tracking_efficiency_" + variant + "_pt", "hte_Sts.*_" + variant + "_All_Eff_pt");
		//DrawEfficiency("tracking_qa_global_tracking_efficiency_" + variant + "_y", "hte_Sts.*_" + variant + "_All_Eff_y");
		DrawEfficiency("tracking_qa_global_tracking_efficiency_" + variant + "_angle", "hte_Sts.*_" + variant + "_All_Eff_Angle", DefaultEfficiencyLabelFormatter);
	}

	// Draw local tracking efficiency
	vector<string> localTrackVariants = list_of("Sts")("Trd")("Much")("Rich")("Tof");
	for (UInt_t i = 0; i < localTrackVariants.size(); i++) {
		string variant = localTrackVariants[i];
		string re = (variant == "Sts") ? "hte_Sts_Sts_(All|Muon|Electron)_Eff" : "hte_" + variant + "_.*_(All|Muon|Electron)_Eff";
		if (variant == "Rich") re = "hte_" + variant + "_.*_(Electron|ElectronReference)_Eff";
		DrawEfficiency("tracking_qa_local_tracking_efficiency_" + variant + "_p", re + "_p", DefaultEfficiencyLabelFormatter);
		//DrawEfficiency("tracking_qa_local_tracking_efficiency_" + variant + "_pt", re + "_pt");
		//DrawEfficiency("tracking_qa_local_tracking_efficiency_" + variant + "_y", "hte_" + variant + "_.*" + variant + ".*_(All|Electron)_Eff_y");

		string re2 = (variant == "Sts") ? "hte_Sts_Sts_All_Eff" : "hte_" + variant + "_.*_All_Eff";
		DrawEfficiency("tracking_qa_local_tracking_efficiency_" + variant + "_angle", re2 + "_Angle", DefaultEfficiencyLabelFormatter);
	}

	// Draw RICH efficiency in dependence on different parameters
	DrawEfficiency("tracking_qa_local_tracking_efficiency_Rich_RingNh", "hte_Rich_Rich_Electron_Eff_RingNh", DefaultEfficiencyLabelFormatter);
   DrawEfficiency("tracking_qa_local_tracking_efficiency_Rich_BoA", "hte_Rich_Rich_Electron_Eff_BoA", DefaultEfficiencyLabelFormatter);
   DrawYPt("tracking_qa_Rich_Rich_Electron_Eff_RingXcYc", "hte_Rich_Rich_Electron_Eff_RingXcYc", true);

	// Draw local accepted and reconstructed tracks vs number of points
	HM()->ShrinkEmptyBinsH1ByPattern("hte_.+_.+_.+_.+_Np");
	vector<string> accRecTracks = list_of("Sts")("Trd")("Much")("Tof");
	for (UInt_t i = 0; i < accRecTracks.size(); i++) {
		string variant = accRecTracks[i];

		string re = (variant == "Sts") ? "hte_Sts_Sts_(All|Muon|Electron)_(Acc|Rec)_Np" : "hte_" + variant + "_.*_(All|Muon|Electron)_(Acc|Rec)_Np";
		DrawAccAndRec("tracking_qa_local_acc_and_rec_" + variant + "_Np", re);

		re = (variant == "Sts") ? "hte_Sts_Sts_(All|Muon|Electron)_(Acc|Rec)_p" : "hte_" + variant + "_.*_(All|Muon|Electron)_(Acc|Rec)_p";
		DrawAccAndRec("tracking_qa_local_acc_and_rec_" + variant + "_p", re);
	}

	//
   DrawPionSuppression("tracking_qa_pion_suppression_wo_Rich_p", "hps_((?!Rich)).*All_PionSup_p", DefaultPionSuppressionLabelFormatter);
   DrawPionSuppression("tracking_qa_pion_suppression_with_rich_p", "hps_Rich.*All_PionSup_p", DefaultPionSuppressionLabelFormatter);


   // Draw ghost RICH rings vs position on photodetector plane
   if (HM()->Exists("hng_NofGhosts_Rich_RingXcYc")){
      TCanvas* canvas = CreateCanvas("tracking_qa_hng_NofGhosts_Rich_RingXcYc", "tracking_qa_hng_NofGhosts_Rich_RingXcYc", 800, 800);
      Int_t nofEvents = HM()->H1("hen_EventNo_TrackingQa")->GetEntries();
      HM()->H2("hng_NofGhosts_Rich_RingXcYc")->Scale(1./nofEvents);
      DrawH2(HM()->H2("hng_NofGhosts_Rich_RingXcYc"));
   }

}

void CbmLitTrackingQaReport::DrawEfficiency(
      const string& canvasName,
      const string& histNamePattern,
      string (*labelFormatter)(const string&, Double_t))
{
	vector<TH1*> histos = HM()->H1Vector(histNamePattern);
	if (histos.size() == 0) return;

	TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
	canvas->SetGrid();
	canvas->cd();

	Int_t nofHistos = histos.size();
	vector<string> labels(nofHistos);
	vector<Double_t> efficiencies(nofHistos);
	for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
		string name = histos[iHist]->GetName();
		efficiencies[iHist] = CalcEfficiency(HM()->H1(FindAndReplace(name, "_Eff_", "_Rec_")), HM()->H1(FindAndReplace(name, "_Eff_", "_Acc_")), 100.);
		labels[iHist] = labelFormatter(name, efficiencies[iHist]);
	}

	DrawH1(histos, labels, kLinear, kLinear, true, 0.3, 0.3, 0.85, 0.6, "PE1");
	DrawMeanEfficiencyLines(histos, efficiencies);
}

void CbmLitTrackingQaReport::DrawPionSuppression(
      const string& canvasName,
      const string& histNamePattern,
      string (*labelFormatter)(const string&, Double_t))
{
   vector<TH1*> histos = HM()->H1Vector(histNamePattern);
   if (histos.size() == 0) return;

   TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
   canvas->SetGrid();
   canvas->cd();

   Int_t nofHistos = histos.size();
   vector<string> labels(nofHistos);
   vector<Double_t> ps(nofHistos);
   for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
      string name = histos[iHist]->GetName();
      ps[iHist] = CalcEfficiency(HM()->H1(FindAndReplace(name, "_PionSup_", "_RecPions_")), HM()->H1(FindAndReplace(name, "_PionSup_", "_Rec_")), 1.);
      labels[iHist] = labelFormatter(name, ps[iHist]);
   }

   DrawH1(histos, labels, kLinear, kLog, true, 0.3, 0.3, 0.85, 0.6, "PE1");
   DrawMeanEfficiencyLines(histos, ps);
}

void CbmLitTrackingQaReport::DrawMeanEfficiencyLines(
   const vector<TH1*>& histos,
   const vector<Double_t>& efficiencies)
{
   assert(histos.size() != 0 && efficiencies.size() == histos.size());

   Double_t minX = histos[0]->GetXaxis()->GetXmin();
   Double_t maxX = histos[0]->GetXaxis()->GetXmax();
   Int_t nofHistos = histos.size();
   for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
      TLine* line = new TLine(minX, efficiencies[iHist], maxX, efficiencies[iHist]);
      line->SetLineWidth(1);
      line->SetLineColor(histos[iHist]->GetLineColor());
      line->Draw();
   }
}

void CbmLitTrackingQaReport::DrawAccAndRec(
      const string& canvasName,
      const string& histNamePattern)
{
	vector<TH1*> histos = HM()->H1Vector(histNamePattern);
	if (histos.size() == 0) return;

	TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
	canvas->SetGrid();
	canvas->cd();

	Int_t nofEvents = HM()->H1("hen_EventNo_TrackingQa")->GetEntries();
	Int_t nofHistos = histos.size();
	vector<string> labels(nofHistos);
	for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
		TH1* hist = histos[iHist];
		hist->Scale(1./nofEvents);
		string name = hist->GetName();
		vector<string> split = Split(name, '_');
		labels[iHist] = split[4] + ":" + split[3] + "(" + NumberToString<Double_t>(hist->GetEntries() / nofEvents, 1) + ")";
	}

	DrawH1(histos, labels, kLinear, kLinear, true, 0.2, 0.75, 0.5, 0.99);
}

void CbmLitTrackingQaReport::DrawYPtHistos()
{
   // Draw global tracking efficiency
   for (UInt_t i = 0; i < fGlobalTrackVariants.size(); i++) {
      string variant = fGlobalTrackVariants[i];
      string effHistName = "hte_" + variant + "_" + variant;
      DrawYPt("tracking_qa_" + variant + "_all_ypt", effHistName + "_All_Eff_YPt");
      DrawYPt("tracking_qa_" + variant + "_proton_ypt", effHistName + "_Proton_Eff_YPt");
      DrawYPt("tracking_qa_" + variant + "_pion_plus_ypt", effHistName + "_PionPlus_Eff_YPt");
      DrawYPt("tracking_qa_" + variant + "_pion_minus_ypt", effHistName + "_PionMinus_Eff_YPt");
      DrawYPt("tracking_qa_" + variant + "_kaon_plus_ypt", effHistName + "_KaonPlus_Eff_YPt");
      DrawYPt("tracking_qa_" + variant + "_kaon_minus_ypt", effHistName + "_KaonMinus_Eff_YPt");
   }
}

void CbmLitTrackingQaReport::DrawYPt(
      const string& canvasName,
      const string& effHistName,
      Bool_t drawOnlyEfficiency)
{
   string accHistName = FindAndReplace(effHistName, "_Eff_", "_Acc_");
   string recHistName = FindAndReplace(effHistName, "_Eff_", "_Rec_");

   if (!(HM()->Exists(effHistName) && HM()->Exists(accHistName) && HM()->Exists(recHistName))) return;

   TH2* accHist = HM()->H2(accHistName);
   TH2* recHist = HM()->H2(recHistName);
   TH2* effHist = HM()->H2(effHistName);

   if (drawOnlyEfficiency){
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 800, 800);
      //canvas->SetGrid();
      DrawH2(effHist);
   } else {
      TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1800, 600);
      canvas->Divide(3, 1);
      canvas->SetGrid();
      canvas->cd(1);
      DrawH2(accHist);

      canvas->cd(2);
      DrawH2(recHist);

      canvas->cd(3);
      DrawH2(effHist);
   }
}

void CbmLitTrackingQaReport::DrawHitsHistos()
{
   HM()->ShrinkEmptyBinsH1ByPattern("hth_.*(_All|_True|_Fake)");
   DrawHitsHistos("tracking_qa_mvd_hits", "hth_Mvd_TrackHits");
   DrawHitsHistos("tracking_qa_sts_hits", "hth_Sts_TrackHits");
   DrawHitsHistos("tracking_qa_trd_hits", "hth_Trd_TrackHits");
   DrawHitsHistos("tracking_qa_much_hits", "hth_Much_TrackHits");
   DrawHitsHistos("tracking_qa_rich_hits", "hth_Rich_TrackHits");
}

void CbmLitTrackingQaReport::DrawHitsHistos(
   const string& canvasName,
   const string& hist)
{
   if (!(HM()->Exists(hist + "_All") && HM()->Exists(hist + "_True") &&
         HM()->Exists(hist + "_Fake") && HM()->Exists(hist + "_TrueOverAll") &&
         HM()->Exists(hist + "_FakeOverAll"))) return;

   TCanvas* canvas = CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1200, 600);
   canvas->Divide(2,1);
   canvas->SetGrid();

   canvas->cd(1);
   TH1* hAll = HM()->H1(hist + "_All");
   TH1* hTrue = HM()->H1(hist + "_True");
   TH1* hFake = HM()->H1(hist + "_Fake");
   DrawH1(list_of(hAll)(hTrue)(hFake),
      list_of("all: " + NumberToString<Double_t>(hAll->GetMean(), 1))
             ("true: " + NumberToString<Double_t>(hTrue->GetMean(), 1))
             ("fake: " + NumberToString<Double_t>(hFake->GetMean(), 1)),
              kLinear, kLog, true, 0.25, 0.99, 0.55, 0.75);

   canvas->cd(2);
   TH1* hTrueOverAll = HM()->H1(hist + "_TrueOverAll");
   TH1* hFakeOverAll = HM()->H1(hist + "_FakeOverAll");
   DrawH1(list_of(hTrueOverAll)(hFakeOverAll),
      list_of("true/all: " + NumberToString<Double_t>(hTrueOverAll->GetMean()))
             ("fake/all: " + NumberToString<Double_t>(hFakeOverAll->GetMean())),
             kLinear, kLog, true, 0.25, 0.99, 0.55, 0.75);
}

Double_t CbmLitTrackingQaReport::CalcEfficiency(
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

void CbmLitTrackingQaReport::FillGlobalTrackVariants()
{
   fGlobalTrackVariants.clear();
   vector<TH1*> histos = HM()->H1Vector("hte_.*_Eff_p");
   Int_t nofHistos = histos.size();
   set<string> variants;
   for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
      string effName = histos[iHist]->GetName();
      variants.insert(Split(effName, '_')[2]);
   }
   fGlobalTrackVariants.assign(variants.begin(), variants.end());
}

void CbmLitTrackingQaReport::DivideHistos(
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

void CbmLitTrackingQaReport::CalculateEfficiencyHistos()
{
    vector<TH1*> effHistos = HM()->H1Vector("(hte|hpe)_.+_Eff_.+");
    Int_t nofEffHistos = effHistos.size();
    for (Int_t iHist = 0; iHist < nofEffHistos; iHist++) {
      TH1* effHist = effHistos[iHist];
      string effHistName = effHist->GetName();
      string accHistName = FindAndReplace(effHistName, "_Eff_", "_Acc_");
      string recHistName = FindAndReplace(effHistName, "_Eff_", "_Rec_");
      DivideHistos(HM()->H1(recHistName), HM()->H1(accHistName), effHist, 100.);
      effHist->SetMinimum(0.);
      effHist->SetMaximum(100.);
    }
}

void CbmLitTrackingQaReport::CalculatePionSuppressionHistos()
{
    vector<TH1*> histos = HM()->H1Vector("hps_.+_PionSup_.+");
    Int_t nofHistos = histos.size();
    for (Int_t iHist = 0; iHist < nofHistos; iHist++) {
      TH1* psHist = histos[iHist];
      string psHistName = psHist->GetName();
      string recHistName = FindAndReplace(psHistName, "_PionSup_", "_Rec_");
      string pionRecHistName = FindAndReplace(psHistName, "_PionSup_", "_RecPions_");
      DivideHistos(HM()->H1(pionRecHistName), HM()->H1(recHistName), psHist, 1.);
    //  psHist->SetMinimum(1.);
    //  psHist->SetMaximum(20000.);
    }
}

ClassImp(CbmLitTrackingQaReport)
