#include "drawStyle.C"


void plot_sts()
{
    TPstyle();

    char *ver = getenv("CBMVER");

    char strFileName[1000];
    sprintf(strFileName, "/lustre_alpha/cbm/user/kresan/reco/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.0000.recoSts.root", ver);
    TFile *file = new TFile(strFileName);
    TDirectory *hdir = (TDirectory*) file->Get("STSFindTracksQA");
    TH1F *h_eff_all = (TH1F*) hdir->Get("hMomEffAll");
    TH1F *h_eff_prim = (TH1F*) hdir->Get("hMomEffPrim");
    Style(h_eff_all, "p (GeV/c)", "efficiency");
    h_eff_all->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_all->SetMarkerStyle(20);
    h_eff_all->SetMarkerColor(2);
    Style(h_eff_prim, "p (GeV/c)", "efficiency");
    h_eff_prim->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_prim->SetMarkerStyle(21);
    h_eff_prim->SetMarkerColor(2);

    TF1 *f1 = new TF1("f1", "1", 0., 10.);
    f1->SetLineWidth(1.2);

    TF1 *f2 = new TF1("f2", "[0]", 1., 10.);
    TF1 *f3 = new TF1("f3", "[0]", 1., 10.);

    h_eff_all->Fit(f2, "QR+");
    h_eff_prim->Fit(f3, "QR+");

    TString strPer = "%";
    char strEffAll[50];
    char strEffPrim[50];
    sprintf(strEffAll, "all tracks: %3.1f %s",
	    100.*f2->GetParameter(0),
	    strPer.Data());
    sprintf(strEffPrim, "prim. tracks: %3.1f %s",
	    100*f3->GetParameter(0),
	    strPer.Data());

    gStyle->SetPaperSize(32, 16);
    TCanvas *c1 = new TCanvas("c1", "STS", 10, 10, 1020, 510);
    char strPsFile[500];
    sprintf(strPsFile, "plots/%s/sts_eff.eps", ver);
    TPostScript *ps1 = new TPostScript(strPsFile, -113);
    ps1->Range(32, 16);
    c1->Divide(2, 1);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_all->Draw("pe1");
    f1->Draw("same");
    h_eff_all->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l1 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffAll, "NDC");
    Style(l1);
    l1->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_prim->Draw("pe1");
    f1->Draw("same");
    h_eff_prim->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l2 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffPrim, "NDC");
    Style(l2);
    l2->Draw();
    c1->cd(0);
    ps1->Close();
}


void plot_sts_L1()
{
    TPstyle();

    TFile *file = new TFile("histo.root");
    TProfile *h_eff_all = (TProfile*) file->Get("p_eff_all_vs_mom");
    TProfile *h_eff_prim = (TProfile*) file->Get("p_eff_prim_vs_mom");
    Style(h_eff_all, "p (GeV/c)", "efficiency (%)");
    h_eff_all->GetYaxis()->SetRangeUser(0., 100.);
    h_eff_all->SetMarkerStyle(20);
    h_eff_all->SetMarkerColor(2);
    Style(h_eff_prim, "p (GeV/c)", "efficiency (%)");
    h_eff_prim->GetYaxis()->SetRangeUser(0., 100.);
    h_eff_prim->SetMarkerStyle(21);
    h_eff_prim->SetMarkerColor(2);

    TF1 *f1 = new TF1("f1", "100", 0., 10.);
    f1->SetLineWidth(1.2);

    TString strPer = "%";
    char strEffAll[50];
    char strEffPrim[50];
    sprintf(strEffAll, "all tracks: %3.1f %s",
	    100*h_eff_all->Integral()/40./100.,
	    strPer.Data());
    sprintf(strEffPrim, "prim. tracks: %3.1f %s",
	    100*h_eff_prim->Integral()/40./100.,
	    strPer.Data());

    gStyle->SetPaperSize(32, 16);
    TCanvas *c1 = new TCanvas("c1", "STS", 10, 10, 1020, 510);
    TPostScript *ps1 = new TPostScript("plots/sts_eff_L1.eps", -113);
    ps1->Range(32, 16);
    c1->Divide(2, 1);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_all->Draw("pe1");
    f1->Draw("same");
    h_eff_all->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l1 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffAll, "NDC");
    Style(l1);
    l1->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_prim->Draw("pe1");
    f1->Draw("same");
    h_eff_prim->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l2 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffPrim, "NDC");
    Style(l2);
    l2->Draw();
    c1->cd(0);
    ps1->Close();
}



void plot_sts_big()
{
    TPstyle();

    TH1F *H_mom_acc_all = new TH1F("h_mom_acc_all", "",
				   40, 0., 10.);
    TH1F *H_mom_rec_all = new TH1F("h_mom_rec_all", "",
				   40, 0., 10.);
    TH1F *H_mom_acc_prim = new TH1F("h_mom_acc_prim", "",
				    40, 0., 10.);
    TH1F *H_mom_rec_prim = new TH1F("h_mom_rec_prim", "",
				    40, 0., 10.);

    char *ver = getenv("CBMVER");
    char strFileName[1000];

    for(Int_t i = 0; i < 100; i++) {
        sprintf(strFileName, "/d/cbm02/kresan/reco/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.%4d.recoSts.root", ver, i);
        for(Int_t ii = 0; ii < 1000; ii++) if(' ' == strFileName[ii]) strFileName[ii] = '0';
	TFile *file = new TFile(strFileName);
	TDirectory *hdir = (TDirectory*) file->Get("StsFinderQa");
	TH1F *h_mom_acc_all = (TH1F*) hdir->Get("hMomAccAll");
	TH1F *h_mom_rec_all = (TH1F*) hdir->Get("hMomRecAll");
	TH1F *h_mom_acc_prim = (TH1F*) hdir->Get("hMomAccPrim");
	TH1F *h_mom_rec_prim = (TH1F*) hdir->Get("hMomRecPrim");
        H_mom_acc_all->Add(h_mom_acc_all);
        H_mom_rec_all->Add(h_mom_rec_all);
        H_mom_acc_prim->Add(h_mom_acc_prim);
	H_mom_rec_prim->Add(h_mom_rec_prim);
        file->Close();
    }

    TH1F *h_eff_all = new TH1F("h_eff_all", "", 40, 0., 10.);
    h_eff_all->Sumw2();
    h_eff_all->Divide(H_mom_rec_all, H_mom_acc_all,
		      1, 1, "B");
    TH1F *h_eff_prim = new TH1F("h_eff_prim", "", 40, 0., 10.);
    h_eff_prim->Sumw2();
    h_eff_prim->Divide(H_mom_rec_prim, H_mom_acc_prim,
		       1, 1, "B");
    Style(h_eff_all, "p (GeV/c)", "efficiency");
    h_eff_all->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_all->SetMarkerStyle(20);
    h_eff_all->SetMarkerColor(2);
    Style(h_eff_prim, "p (GeV/c)", "efficiency");
    h_eff_prim->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_prim->SetMarkerStyle(21);
    h_eff_prim->SetMarkerColor(2);

    TF1 *f1 = new TF1("f1", "1", 0., 10.);
    f1->SetLineWidth(1.2);

    TString strPer = "%";
    char strEffAll[50];
    char strEffPrim[50];
    sprintf(strEffAll, "all tracks: %3.1f %s",
	    100.*h_eff_all->Integral()/40.,
	    strPer.Data());
    sprintf(strEffPrim, "prim. tracks: %3.1f %s",
	    100*h_eff_prim->Integral()/40.,
	    strPer.Data());

    gStyle->SetPaperSize(32, 16);
    TCanvas *c1 = new TCanvas("c1", "STS", 10, 10, 1020, 510);
    char strPsFile[500];
    sprintf(strPsFile, "plots/%s/sts_eff.eps", ver);
    TPostScript *ps1 = new TPostScript(strPsFile, -113);
    ps1->Range(32, 16);
    c1->Divide(2, 1);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_all->Draw("pe1");
    f1->Draw("same");
    h_eff_all->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l1 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffAll, "NDC");
    Style(l1);
    l1->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_prim->Draw("pe1");
    f1->Draw("same");
    h_eff_prim->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l2 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffPrim, "NDC");
    Style(l2);
    l2->Draw();
    c1->cd(0);
    ps1->Close();
}




void plot_trd()
{
    TPstyle();

    char *ver = getenv("CBMVER");
    char strFileName[1000];
    sprintf(strFileName, "/d/cbm06/user/kresan/reco/%s/urqmd/auau/25gev/centr/80ps/\
urqmd.auau.25gev.centr.0000.80ps.reco.root", ver);
    TFile *file = new TFile(strFileName);
    TDirectory *hdir = (TDirectory*) file->Get("TrdFinderQa");
    TH1F *h_eff_all = (TH1F*) hdir->Get("hMomEffAll");
    TH1F *h_eff_prim = (TH1F*) hdir->Get("hMomEffPrim");
    Style(h_eff_all, "p (GeV/c)", "efficiency");
    h_eff_all->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_all->SetMarkerStyle(20);
    h_eff_all->SetMarkerColor(4);
    Style(h_eff_prim, "p (GeV/c)", "efficiency");
    h_eff_prim->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_prim->SetMarkerStyle(21);
    h_eff_prim->SetMarkerColor(4);

    TF1 *f1 = new TF1("f1", "1", 0., 10.);
    f1->SetLineWidth(1.2);

    TF1 *f2 = new TF1("f2", "[0]", 1., 10.);
    TF1 *f3 = new TF1("f3", "[0]", 1., 10.);
    h_eff_all->Fit(f2, "QR+");
    h_eff_prim->Fit(f3, "QR+");

    TString strPer = "%";
    char strEffAll[50];
    char strEffPrim[50];
    sprintf(strEffAll, "all tracks: %3.1f %s",
	    100.*f2->GetParameter(0),
	    strPer.Data());
    sprintf(strEffPrim, "prim. tracks: %3.1f %s",
	    100.*f3->GetParameter(0),
	    strPer.Data());

    gStyle->SetPaperSize(32, 16);
    TCanvas *c1 = new TCanvas("c1", "TRD", 10, 10, 1020, 510);
    char strPsFile[500];
    sprintf(strPsFile, "plots/%s/trd_eff.eps", ver);
    TPostScript *ps1 = new TPostScript(strPsFile, -113);
    ps1->Range(32, 16);
    c1->Divide(2, 1);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_all->Draw("pe1");
    f1->Draw("same");
    h_eff_all->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l1 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffAll, "NDC");
    Style(l1);
    l1->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_prim->Draw("pe1");
    f1->Draw("same");
    h_eff_prim->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l2 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffPrim, "NDC");
    Style(l2);
    l2->Draw();
    c1->cd(0);
    ps1->Close();
}



void plot_trd_big()
{
    TPstyle();

    TH1F *H_mom_acc_all = new TH1F("h_mom_acc_all", "",
				   40, 0., 10.);
    TH1F *H_mom_rec_all = new TH1F("h_mom_rec_all", "",
				   40, 0., 10.);
    TH1F *H_mom_acc_prim = new TH1F("h_mom_acc_prim", "",
				    40, 0., 10.);
    TH1F *H_mom_rec_prim = new TH1F("h_mom_rec_prim", "",
				    40, 0., 10.);

    char *ver = getenv("CBMVER");
    char strFileName[1000];

    for(Int_t i = 0; i < 10; i++) {
        sprintf(strFileName, "/d/cbm02/kresan/reco/%s/urqmd/auau/25gev/centr/80ps/\
urqmd.auau.25gev.centr.%4d.80ps.reco.root", ver, i);
        for(Int_t ii = 0; ii < 1000; ii++) if(' ' == strFileName[ii]) strFileName[ii] = '0';
	TFile *file = new TFile(strFileName);
	TDirectory *hdir = (TDirectory*) file->Get("TrdFinderQa");
	TH1F *h_mom_acc_all = (TH1F*) hdir->Get("hMomAccAll");
	TH1F *h_mom_rec_all = (TH1F*) hdir->Get("hMomRecAll");
	TH1F *h_mom_acc_prim = (TH1F*) hdir->Get("hMomAccPrim");
	TH1F *h_mom_rec_prim = (TH1F*) hdir->Get("hMomRecPrim");
        H_mom_acc_all->Add(h_mom_acc_all);
        H_mom_rec_all->Add(h_mom_rec_all);
        H_mom_acc_prim->Add(h_mom_acc_prim);
	H_mom_rec_prim->Add(h_mom_rec_prim);
        file->Close();
    }

    TH1F *h_eff_all = new TH1F("h_eff_all", "", 40, 0., 10.);
    h_eff_all->Sumw2();
    h_eff_all->Divide(H_mom_rec_all, H_mom_acc_all,
		      1, 1, "B");
    TH1F *h_eff_prim = new TH1F("h_eff_prim", "", 40, 0., 10.);
    h_eff_prim->Sumw2();
    h_eff_prim->Divide(H_mom_rec_prim, H_mom_acc_prim,
		       1, 1, "B");
    Style(h_eff_all, "p (GeV/c)", "efficiency");
    h_eff_all->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_all->SetMarkerStyle(20);
    h_eff_all->SetMarkerColor(4);
    Style(h_eff_prim, "p (GeV/c)", "efficiency");
    h_eff_prim->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_prim->SetMarkerStyle(21);
    h_eff_prim->SetMarkerColor(4);

    TF1 *f1 = new TF1("f1", "1", 0., 10.);
    f1->SetLineWidth(1.2);

    TString strPer = "%";
    char strEffAll[50];
    char strEffPrim[50];
    sprintf(strEffAll, "all tracks: %3.1f %s",
	    100.*h_eff_all->Integral()/40.,
	    strPer.Data());
    sprintf(strEffPrim, "prim. tracks: %3.1f %s",
	    100*h_eff_prim->Integral()/40.,
	    strPer.Data());

    gStyle->SetPaperSize(32, 16);
    TCanvas *c1 = new TCanvas("c1", "TRD", 10, 10, 1020, 510);
    char strPsFile[500];
    sprintf(strPsFile, "plots/%s/trd_eff.eps", ver);
    TPostScript *ps1 = new TPostScript(strPsFile, -113);
    ps1->Range(32, 16);
    c1->Divide(2, 1);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_all->Draw("pe1");
    f1->Draw("same");
    h_eff_all->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l1 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffAll, "NDC");
    Style(l1);
    l1->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_prim->Draw("pe1");
    f1->Draw("same");
    h_eff_prim->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l2 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffPrim, "NDC");
    Style(l2);
    l2->Draw();
    c1->cd(0);
    ps1->Close();
}


void plot_tof()
{
    TPstyle();

    char *ver = getenv("CBMVER");
    char strFileName[1000];
    sprintf(strFileName, "/d/cbm02/kresan/reco/%s/urqmd/auau/25gev/centr/80ps/\
urqmd.auau.25gev.centr.0000.80ps.reco.root", ver);
    TFile *file = new TFile(strFileName);
    TDirectory *dir = (TDirectory*) file->Get("FindGlobalTracksQa");
    TH1F *h_eff_all = (TH1F*) dir->Get("h_mom_eff_tof");
    Style(h_eff_all, "p (GeV/c)", "efficiency");
    h_eff_all->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_all->SetMarkerStyle(20);
    h_eff_all->SetMarkerColor(45);

    TF1 *f1 = new TF1("f1", "1", 0., 10.);
    f1->SetLineWidth(1.2);

    TString strPer = "%";
    char strEffAll[50];
    sprintf(strEffAll, "all tracks: %3.1f %s",
	    100.*h_eff_all->Integral()/40.,
	    strPer.Data());

    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "TOF", 10, 10, 510, 510);
    char strPsFile[500];
    sprintf(strPsFile, "plots/%s/tof_eff.eps", ver);
    TPostScript *ps1 = new TPostScript(strPsFile, -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_all->Draw("pe1");
    f1->Draw("same");
    h_eff_all->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l1 = new TPaveLabel(0.43, 0.36, 0.93, 0.46, strEffAll, "NDC");
    Style(l1);
    l1->Draw();
    ps1->Close();
}


void plot_tof_big()
{
    TPstyle();

    TH1F *H_mom_acc_all = new TH1F("h_mom_acc_all", "",
				   40, 0., 10.);
    TH1F *H_mom_rec_all = new TH1F("h_mom_rec_all", "",
				   40, 0., 10.);

    char *ver = getenv("CBMVER");
    char strFileName[1000];

    for(Int_t i = 0; i < 10; i++) {
        sprintf(strFileName, "/d/cbm02/kresan/reco/%s/urqmd/auau/25gev/centr/80ps/\
urqmd.auau.25gev.centr.%4d.80ps.reco.root", ver, i);
        for(Int_t ii = 0; ii < 1000; ii++) if(' ' == strFileName[ii]) strFileName[ii] = '0';
	TFile *file = new TFile(strFileName);
	TDirectory *dir = (TDirectory*) file->Get("FindGlobalTracksQa");
	TH1F *h_mom_acc_all = (TH1F*) dir->Get("h_mom_acc2");
	TH1F *h_mom_rec_all = (TH1F*) dir->Get("h_mom_rec");
        H_mom_acc_all->Add(h_mom_acc_all);
        H_mom_rec_all->Add(h_mom_rec_all);
        file->Close();
    }

    TH1F *h_eff_all = new TH1F("h_eff_all", "", 40, 0., 10.);
    h_eff_all->Sumw2();
    h_eff_all->Divide(H_mom_rec_all, H_mom_acc_all,
		      1, 1, "B");
    Style(h_eff_all, "p (GeV/c)", "efficiency");
    h_eff_all->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_all->SetMarkerStyle(20);
    h_eff_all->SetMarkerColor(45);

    TF1 *f1 = new TF1("f1", "1", 0., 10.);
    f1->SetLineWidth(1.2);

    TString strPer = "%";
    char strEffAll[50];
    sprintf(strEffAll, "all tracks: %3.1f %s",
	    100.*h_eff_all->Integral()/40.,
	    strPer.Data());

    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "TOF", 10, 10, 510, 510);
    char strPsFile[500];
    sprintf(strPsFile, "plots/%s/tof_eff.eps", ver);
    TPostScript *ps1 = new TPostScript(strPsFile, -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_all->Draw("pe1");
    f1->Draw("same");
    h_eff_all->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l1 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffAll, "NDC");
    Style(l1);
    l1->Draw();
    ps1->Close();
}


void plot_glb()
{
    TPstyle();

    char *ver = getenv("CBMVER");
    char strFileName[1000];
    sprintf(strFileName, "/lustre_alpha/cbm/user/kresan/reco/%s/urqmd/auau/4gev/centr/\
urqmd.auau.4gev.centr.0000.reco.root", ver);
    TFile *file = new TFile(strFileName);
    TDirectory *dir = (TDirectory*) file->Get("FindGlobalTracksQA");
    TH1F *h_eff_all = (TH1F*) dir->Get("h_mom_eff");
    Style(h_eff_all, "p (GeV/c)", "efficiency");
    h_eff_all->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_all->SetMarkerStyle(20);
    h_eff_all->SetMarkerColor(42);

    TF1 *f1 = new TF1("f1", "1", 0., 10.);
    f1->SetLineWidth(1.2);

    TF1 *f2 = new TF1("f2", "[0]", 1., 10.);

    h_eff_all->Fit(f2, "QR+");

    TString strPer = "%";
    char strEffAll[50];
    sprintf(strEffAll, "all tracks: %3.1f %s",
	    100.*f2->GetParameter(0),
	    strPer.Data());

    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "Global", 10, 10, 510, 510);
    char strPsFile[500];
    sprintf(strPsFile, "plots/%s/glb_eff_4gev.eps", ver);
    TPostScript *ps1 = new TPostScript(strPsFile, -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_all->Draw("pe1");
    f1->Draw("same");
    h_eff_all->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l1 = new TPaveLabel(0.43, 0.36, 0.93, 0.46, strEffAll, "NDC");
    Style(l1);
    l1->SetTextColor(2);
    l1->Draw();
    TPaveLabel *l2 = new TPaveLabel(0.154, 0.178, 0.945, 0.288, "central Au+Au @ 4 AGeV (SIS100)", "NDC");
    Style(l2);
    l2->Draw();
    ps1->Close();
}



void plot_glb_big()
{
    TPstyle();

    TH1F *H_mom_acc_all = new TH1F("h_mom_acc_all", "",
				   40, 0., 10.);
    TH1F *H_mom_rec_all = new TH1F("h_mom_rec_all", "",
				   40, 0., 10.);

    char *ver = getenv("CBMVER");
    char strFileName[1000];

    for(Int_t i = 0; i < 10; i++) {
        sprintf(strFileName, "/d/cbm02/kresan/reco/%s/urqmd/auau/25gev/centr/80ps/\
urqmd.auau.25gev.centr.%4d.80ps.reco.root", ver, i);
        for(Int_t ii = 0; ii < 1000; ii++) if(' ' == strFileName[ii]) strFileName[ii] = '0';
	TFile *file = new TFile(strFileName);
	TDirectory *dir = (TDirectory*) file->Get("FindGlobalTracksQa");
	TH1F *h_mom_acc_all = (TH1F*) dir->Get("h_mom_acc");
	TH1F *h_mom_rec_all = (TH1F*) dir->Get("h_mom_rec");
        H_mom_acc_all->Add(h_mom_acc_all);
        H_mom_rec_all->Add(h_mom_rec_all);
        file->Close();
    }

    TH1F *h_eff_all = new TH1F("h_eff_all", "", 40, 0., 10.);
    h_eff_all->Sumw2();
    h_eff_all->Divide(H_mom_rec_all, H_mom_acc_all,
		      1, 1, "B");
    Style(h_eff_all, "p (GeV/c)", "efficiency");
    h_eff_all->GetYaxis()->SetRangeUser(0., 1.);
    h_eff_all->SetMarkerStyle(20);
    h_eff_all->SetMarkerColor(42);

    TF1 *f1 = new TF1("f1", "1", 0., 10.);
    f1->SetLineWidth(1.2);

    TString strPer = "%";
    char strEffAll[50];
    sprintf(strEffAll, "all tracks: %3.1f %s",
	    100.*h_eff_all->Integral()/40.,
	    strPer.Data());

    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "Global", 10, 10, 510, 510);
    char strPsFile[500];
    sprintf(strPsFile, "plots/%s/glb_eff.eps", ver);
    TPostScript *ps1 = new TPostScript(strPsFile, -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff_all->Draw("pe1");
    f1->Draw("same");
    h_eff_all->Draw("pe1same");
    gPad->SetGridx();
    gPad->SetGridy();
    TPaveLabel *l1 = new TPaveLabel(0.42, 0.5, 0.92, 0.60, strEffAll, "NDC");
    Style(l1);
    l1->Draw();
    ps1->Close();
}






