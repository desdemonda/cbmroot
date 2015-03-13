/**
 * \file draw_rec_qa.C
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 */

#include "../../littrack/cbm/utils/CbmLitDrawHist.cxx"
#include "../../littrack/std/utils/CbmLitUtils.cxx"
#include "TH1.h"
#include "TFile.h"
#include "TString.h"

const TString dir  = "/d/cbm02/andrey/phd/much1_10mu_urqmd/";
const TFile *file1 = new TFile(dir + "global.tracks.branch.0000.root");
const TFile *file2 = new TFile(dir + "global.tracks.nn.0000.root");
const TFile *file3 = new TFile(dir + "global.tracks.weight.0000.root");

void draw_rec_qa()
{
	SetStyles();

		DrawEffMethodsMuch();
//        DrawEffMethodsTrd();
}

void DrawEffMethodsMuch()
{
	TCanvas *canvas = new TCanvas("tracking_qa_eff_methods_signal","tracking_qa_eff_methods_signal",600,500);
	canvas->SetGrid();

	std::string names[] = {"hRecMomEffMuon", "hRecMomAccMuon", "hRecMomRecMuon"};
//	std::string names[] = {"hRecMomEffAll", "hRecMomAccAll", "hRecMomRec"};

	TH1* eff1 = (TH1*)file1->Get(names[0].c_str());
	TH1* eff2 = (TH1*)file2->Get(names[0].c_str());
	TH1* eff3 = (TH1*)file3->Get(names[0].c_str());

	TH1* acc1 = (TH1*)file1->Get(names[1].c_str());
	TH1* acc2 = (TH1*)file2->Get(names[1].c_str());
	TH1* acc3 = (TH1*)file3->Get(names[1].c_str());

	TH1* rec1 = (TH1*)file1->Get(names[2].c_str());
	TH1* rec2 = (TH1*)file2->Get(names[2].c_str());
	TH1* rec3 = (TH1*)file3->Get(names[2].c_str());

	std::string hname1, hname2, hname3;
	hname1 = "branch (" + CalcEfficiency(rec1, acc1) + ")";
	hname2 = "nn (" + CalcEfficiency(rec2, acc2) + ")";
	hname3 = "weight (" + CalcEfficiency(rec3, acc3) + ")";

	canvas->cd();
	eff1->SetMinimum(0.);
	eff1->SetMaximum(100.);
	DrawH1(eff1, eff2, eff3, NULL, hname1, hname2, hname3, "",
			kLitLinear, kLitLinear, true, 0.3, 0.3, 0.85, 0.6);
	lit::SaveCanvasAsImage(canvas, "./test/");
}

void DrawEffMethodsTrd()
{
	TCanvas *canvas = new TCanvas("tracking_qa_eff_methods","tracking_qa_eff_methods",600,500);
	canvas->SetGrid();

	TCanvas *canvas_all = new TCanvas("tracking_qa_eff_all_methods","tracking_qa_eff_all_methods",600,500);
	canvas_all->SetGrid();

	TCanvas *canvas_electrons = new TCanvas("tracking_qa_eff_electrons_methods","tracking_qa_eff_electrons_methods",600,500);
	canvas_electrons->SetGrid();

	TH1* eff1_1 = (TH1*)file1->Get("hRecMomEffAll");
	TH1* eff2_1 = (TH1*)file2->Get("hRecMomEffAll");
   TH1* eff1_2 = (TH1*)file1->Get("hRecMomEffElectron");
	TH1* eff2_2 = (TH1*)file2->Get("hRecMomEffElectron");
	
	TH1* acc1_1 = (TH1*)file1->Get("hRecMomAccAll");
	TH1* acc2_1 = (TH1*)file2->Get("hRecMomAccAll");
   TH1* acc1_2 = (TH1*)file1->Get("hRecMomAccElectron");
	TH1* acc2_2 = (TH1*)file2->Get("hRecMomAccElectron");

   TH1* rec1_1 = (TH1*)file1->Get("hRecMomRecAll");
	TH1* rec2_1 = (TH1*)file2->Get("hRecMomRecAll");
   TH1* rec1_2 = (TH1*)file1->Get("hRecMomRecElectron");
	TH1* rec2_2 = (TH1*)file2->Get("hRecMomRecElectron");

	std::string hname1, hname2, hname3, hname4;
//	hname1 = "all branch  (" + CalcEfficiency(rec1_1, acc1_1) + ")";
//	hname2 = "all nn (" + CalcEfficiency(rec2_1, acc2_1) + ")";
//	hname3 = "electron branch (" + CalcEfficiency(rec1_2, acc1_2) + ")";
//    hname4 = "electron nn (" + CalcEfficiency(rec2_2, acc2_2) + ")";
	hname1 = "branch  (" + CalcEfficiency(rec1_1, acc1_1) + ")";
	hname2 = "nn (" + CalcEfficiency(rec2_1, acc2_1) + ")";
	hname3 = "branch (" + CalcEfficiency(rec1_2, acc1_2) + ")";
	hname4 = "nn (" + CalcEfficiency(rec2_2, acc2_2) + ")";


	canvas->cd();
	eff1_1->SetMinimum(0.);
	eff1_1->SetMaximum(100.);
	DrawH1(eff1_1, eff2_1, eff1_2, eff2_2, hname1, hname2, hname3, hname4,
			kLitLinear, kLitLinear, true, 0.2, 0.2, 0.85, 0.6);
	lit::SaveCanvasAsImage(canvas, "./test/");

	canvas_all->cd();
	eff1_1->SetMinimum(0.);
	eff1_1->SetMaximum(1.);
	DrawH1(eff1_1, eff2_1, NULL, NULL, hname1, hname2, "", "",
			kLitLinear, kLitLinear, true, 0.3, 0.3, 0.85, 0.6);
	lit::SaveCanvasAsImage(canvas_all, "./test/");

	canvas_electrons->cd();
	eff1_2->SetMinimum(0.);
	eff1_2->SetMaximum(1.);
	DrawH1(eff1_2, eff2_2, NULL, NULL, hname3, hname4, "", "",
			kLitLinear, kLitLinear, true, 0.3, 0.3, 0.85, 0.6);
	lit::SaveCanvasAsImage(canvas_electrons, "./test/");
}


std::string CalcEfficiency(
		TH1* histRec,
		TH1* histAcc)
{
	if (histAcc->GetEntries() == 0) return "0.0";
	else {
		Double_t eff = Double_t(histRec->GetEntries()) / Double_t(histAcc->GetEntries());
		std::stringstream ss;
		ss.precision(3);
		ss << eff;
		return ss.str();
	}
}

