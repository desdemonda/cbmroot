#define MPION 0.1395700
#define MKAON 0.493677
#define MPROT 0.938272

#define M2PION 0.0194797849
#define M2KAON 0.243716980329
#define M2PROT 0.880354345984

#include "drawStyle.C"

#define C1 0.3989422804014327

Double_t fun_m2_mc(Double_t *x, Double_t *par)
{
    return (par[1]*C1/par[0]*exp(-0.5*pow((x[0]-M2PION)/par[0], 2)) +
	    par[2]*C1/par[0]*exp(-0.5*pow((x[0]-M2KAON)/par[0], 2)) +
	    par[3]*C1/par[0]*exp(-0.5*pow((x[0]-M2PROT)/par[0], 2)));
}

Double_t fun_m2_pika(Double_t *x, Double_t *par)
{
    return (par[1]*C1/par[0]*exp(-0.5*pow((x[0]-par[3])/par[0], 2)) +
	    par[2]*C1/par[0]*exp(-0.5*pow((x[0]-M2KAON)/par[0], 2)));
}

Double_t fun_m2(Double_t *x, Double_t *par)
{
    return (par[1]*C1/par[0]*exp(-0.5*pow((x[0]-par[2])/par[0], 2)));
}


// ------------------------------------------------------------------
void plot_hadron_m2mom(Int_t level)
{
    // Check level
    // 1 - MC truth
    // 2 - RECO
    // 3 - RECO truth (proper match)
    if(level<1 || level>3) return;

    // Drawing style
    TPstyle();

    // CBM software version
    char *ver = getenv("CBMVER");

    // Define filenames
    char fileName[3][1000];
    sprintf(fileName[0], "/home/kresan/data/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.m2mom.root", ver);
    sprintf(fileName[1], "/home/kresan/data/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.m2mom.reco.root", ver);
    sprintf(fileName[2], "/home/kresan/data/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.m2mom.reco.root", ver);

    // Define PS file names
    char psName[3][1000];
    sprintf(psName[0], "plots/%s/hadron.m2mom.mc.eps", ver);
    sprintf(psName[1], "plots/%s/hadron.m2mom.reco.eps", ver);
    sprintf(psName[2], "plots/%s/hadron.m2mom.reco.true.eps", ver);

    // Labels on the plots for diff. levels
    char *label[] = {"MC", "RECO", "RECO TRUE"};

    // Open the file
    TFile* file = new TFile(fileName[level-1]);
    TH2F* h_m2mom;
    if(level < 3) {
	h_m2mom = (TH2F*) file->Get("h_m2mom_hadron");
    } else if(3 == level) {
	h_m2mom = (TH2F*) file->Get("h_m2mom_hadron_true");
    }
    Style(h_m2mom, "p (GeV/c)", "m^{2} (GeV^{2}/c^{4})");
    h_m2mom->Scale(1./1000.);
    h_m2mom->GetXaxis()->SetRangeUser(0., 6.);
    h_m2mom->GetYaxis()->SetRangeUser(-0.5, 1.5);
    h_m2mom->GetZaxis()->SetRangeUser(1, 1e4);
    h_m2mom->GetZaxis()->SetTitle("dn/dp/dm^{2}");

    // Make three projections
    Int_t bins[] = {111, 131, 151};
    char *strBins[] = {"1 GeV/c", "3 GeV/c", "5 GeV/c"};
    TString strNames[] = {"_1", "_3", "_5"};
    TH1D *h_m2[3];
    for(Int_t i = 0; i < 3; i++) {
	h_m2[i] = h_m2mom->ProjectionY(("h_m2"+strNames[i]).Data(),
				       bins[i], bins[i]);
	Style(h_m2[i], "m^{2} (GeV^{2}/c^{4})", "dn/dm^{2}");
	h_m2[i]->Scale(0.1);
	h_m2[i]->GetXaxis()->SetRangeUser(-0.5, 1.5);
//	h_m2[i]->GetYaxis()->SetRangeUser(1e-1, 3e2);
    }

    // Fitting functions
    TF1 *f1;
    TF1 *f_pika_f;
    TF1 *f_prot_f;
    if(level < 2) {
	f1 = new TF1("f1", fun_m2_mc, -0.5, 1.5, 4);
	f1->SetParameters(0.1, 150, 20, 80);
    } else {
	f_pika_f = new TF1("f_pika_f", fun_m2_pika, -0.2, 0.5, 4);
        f_pika_f->SetParameters(0.1, 1., 1., 0.001);
	f_prot_f = new TF1("f_prot_f", fun_m2, 0.7, 1.3, 3);
	f_prot_f->SetParameters(0.3, 1., M2PROT);
    }
    TF1 *f_pion = new TF1("f_pion", fun_m2, -0.5, 1.5, 3);
    // f_pion->SetLineWidth(1.2);
    f_pion->SetLineColor(2);
    TF1 *f_kaon = new TF1("f_kaon", fun_m2, -0.5, 1.5, 3);
    // f_kaon->SetLineWidth(1.2);
    f_kaon->SetLineColor(4);
    TF1 *f_prot = new TF1("f_prot", fun_m2, -0.5, 1.5, 3);
    // f_prot->SetLineWidth(1.2);
    f_prot->SetLineColor(3);


    // Plot hitogramms
    TCanvas* c1; 
    if(level < 2) {
	gStyle->SetPaperSize(32, 16);
	c1 = new TCanvas("c1", "", 10, 10, 1020, 510);
	c1->Divide(2, 1);
    } else {
	gStyle->SetPaperSize(32, 32);
	c1 = new TCanvas("c1", "", 10, 10, 1020, 1020);
	c1->Divide(2, 2);
    }
    TPostScript* ps1 = new TPostScript(psName[level-1], -113);
    if(level < 2) {
	ps1->Range(32, 16);
    } else {
	ps1->Range(32, 32);
    }
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.17);
    gPad->SetLogz();
    gPad->SetGridx();
    gPad->SetGridy();
    h_m2mom->Draw("colz");
    TPaveLabel* l1 = new TPaveLabel(0.37, 0.90, 0.80, 0.98, label[level-1], "NDC");
    Style(l1);
    l1->SetTextColor(2);
    l1->Draw();
    if(level < 2) {
	c1->cd(2);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.05);
	gPad->SetLogy();
	gPad->SetGridx();
	gPad->SetGridy();
	h_m2[1]->Draw();
        h_m2[1]->Fit(f1, "QN");
	f_pion->SetParameters(f1->GetParameter(0), f1->GetParameter(1), M2PION);
	f_kaon->SetParameters(f1->GetParameter(0), f1->GetParameter(2), M2KAON);
	f_prot->SetParameters(f1->GetParameter(0), f1->GetParameter(3), M2PROT);
	f_pion->Draw("same");
	f_kaon->Draw("same");
	f_prot->Draw("same");
	TPaveLabel* l2 = new TPaveLabel(0.41, 0.79, 0.73, 0.88, strBins[1], "NDC");
	Style(l2);
	l2->SetTextColor(4);
	l2->Draw();
	TLegend *leg1 = new TLegend(0.73, 0.62, 0.94, 0.86);
	Style(leg1);
	leg1->AddEntry(f_pion, "  #pi^{+}", "l");
	leg1->AddEntry(f_kaon, "  K^{+}", "l");
	leg1->AddEntry(f_prot, "  p", "l");
	leg1->Draw();
    } else {
	for(Int_t i = 0; i < 3; i++) {
	    c1->cd(i+2);
//	    c1->cd(2);
	    gPad->SetLeftMargin(0.15);
	    gPad->SetRightMargin(0.05);
	    gPad->SetLogy();
	    gPad->SetGridx();
	    gPad->SetGridy();
	    h_m2[i]->SetLineColor(1);
	    h_m2[i]->Draw();
	    h_m2[i]->Fit(f_pika_f, "QNR+");
	    h_m2[i]->Fit(f_prot_f, "QNR+");
	    f_pion->SetParameters(f_pika_f->GetParameter(0), f_pika_f->GetParameter(1), f_pika_f->GetParameter(3));
	    f_kaon->SetParameters(f_pika_f->GetParameter(0), f_pika_f->GetParameter(2), M2KAON);
	    f_prot->SetParameters(f_prot_f->GetParameter(0), f_prot_f->GetParameter(1), f_prot_f->GetParameter(2));
	    f_pion->DrawClone("same");
	    f_kaon->DrawClone("same");
	    f_prot->DrawClone("same");
	    // f_pika_f->SetLineColor(2);
	    // f_pika_f->DrawClone("same");
	    TPaveLabel* l2 = new TPaveLabel(0.41, 0.79, 0.73, 0.88, strBins[i], "NDC");
	    Style(l2);
	    l2->SetTextColor(4);
	    l2->Draw();
	    if(0 == i) {
		TLegend *leg1 = new TLegend(0.765, 0.652, 0.935, 0.862);
		Style(leg1);
		leg1->AddEntry(f_pion, "  #pi^{+}", "l");
		leg1->AddEntry(f_kaon, "  K^{+}", "l");
		leg1->AddEntry(f_prot, "  p", "l");
		leg1->Draw();
	    }
	}
    }
    c1->cd(0);
    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_hadron_m2mom_simple(Int_t level)
{
    // Check level
    if(level<1 || level>4) return;

    // Drawing style
    TPstyle();

    // CBM software version
    char *ver = getenv("CBMVER");

    // Define filenames
    char fileName[4][1000];
    sprintf(fileName[0], "/lustre/cbm/user/kresan/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.m2mom.root", ver);
    sprintf(fileName[1], "/lustre/cbm/user/kresan/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.m2mom.recoIdeal.root", ver);
    sprintf(fileName[2], "/lustre/cbm/user/kresan/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.m2mom.reco.root", ver);
    sprintf(fileName[3], "/lustre/cbm/user/kresan/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.m2mom.reco.root", ver);

    // Define PS file names
    char psName[4][1000];
    sprintf(psName[0], "plots/%s/m2mom.mc.eps", ver);
    sprintf(psName[1], "plots/%s/m2mom.recoIdeal.eps", ver);
    sprintf(psName[2], "plots/%s/m2mom.reco.eps", ver);
    sprintf(psName[3], "plots/%s/m2mom.reco.true.eps", ver);

    // Labels on the plots for diff. levels
    char *label[] = {"MC", "RECO ideal", "RECO", "RECO TRUE"};

    // Open the file
    TFile* file = new TFile(fileName[level-1]);
    TH2F* h_m2mom;
    if(level < 4) {
	h_m2mom = (TH2F*) file->Get("h_m2mom_hadron");
    } else if(4 == level) {
	h_m2mom = (TH2F*) file->Get("h_m2mom_hadron_true");
    }
    Style(h_m2mom, "p (GeV/c)", "m^{2} (GeV^{2}/c^{4})");
    h_m2mom->Scale(1./200000.);
    h_m2mom->GetXaxis()->SetRangeUser(0., 6.);
    h_m2mom->GetYaxis()->SetRangeUser(-0.5, 1.5);
    h_m2mom->GetZaxis()->SetRangeUser(1, 1e4);
    h_m2mom->GetZaxis()->SetTitle("dn/dp/dm^{2}");


    // Plot hitogramms
    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript* ps1 = new TPostScript(psName[level-1], -113);
    ps1->Range(16, 16);

    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.17);
    gPad->SetLogz();
    gPad->SetGridx();
    gPad->SetGridy();

    h_m2mom->Draw("colz");

    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_hadron_m2mom_GSI(Int_t level)
{
    // Check level
    if(level<1 || level>3) return;

    // Drawing style
    TPstyle();

    // CBM software version
    char *ver = getenv("CBMVER");

    // Define filenames
    char fileName[3][1000];
    sprintf(fileName[0], "/lustre/cbm/user/kresan/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.m2mom.root", ver);
    sprintf(fileName[1], "/lustre/cbm/user/kresan/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.m2mom.recoIdeal.root", ver);
    sprintf(fileName[2], "/lustre/cbm/user/kresan/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.m2mom.reco.root", ver);

    // Define PS file names
    char psName[3][1000];
    sprintf(psName[0], "plots/%s/hadron.m2mom.mc.eps", ver);
    sprintf(psName[1], "plots/%s/hadron.m2mom.recoIdeal.eps", ver);
    sprintf(psName[2], "plots/%s/hadron.m2mom.reco.GSI.eps", ver);

    // Labels on the plots for diff. levels
    char *label[] = {"MC", "RECO ideal", "RECO"};

    // Open the file
    TFile* file = new TFile(fileName[level-1]);
    TH2F* h_m2mom = (TH2F*) file->Get("h_m2mom_hadron");
    Style(h_m2mom, "p (GeV/c)", "m^{2} (GeV^{2}/c^{4})");
    h_m2mom->Scale(1./200000.);
    h_m2mom->GetXaxis()->SetRangeUser(0., 6.);
    h_m2mom->GetYaxis()->SetRangeUser(-0.5, 1.5);
    h_m2mom->GetZaxis()->SetRangeUser(1, 1e4);
    h_m2mom->GetZaxis()->SetTitle("dn/dp/dm^{2}");

    // Make three projections
    Int_t bins[] = {111, 131, 151};
    char *strBins[] = {"1 GeV/c", "3 GeV/c", "5 GeV/c"};
    TString strNames[] = {"_1", "_3", "_5"};
    TH1D *h_m2[3];
    for(Int_t i = 0; i < 3; i++) {
	h_m2[i] = h_m2mom->ProjectionY(("h_m2"+strNames[i]).Data(),
				       bins[i], bins[i]);
	Style(h_m2[i], "m^{2} (GeV^{2}/c^{4})", "dn/dm^{2}");
	h_m2[i]->Scale(0.1);
	h_m2[i]->GetXaxis()->SetRangeUser(-0.5, 1.5);
//	h_m2[i]->GetYaxis()->SetRangeUser(1e-1, 3e2);
    }

    // Fitting functions
    TF1 *f1;
    TF1 *f_pika_f;
    TF1 *f_prot_f;
    if(level < 2) {
	f1 = new TF1("f1", fun_m2_mc, -0.5, 1.5, 4);
	f1->SetParameters(0.1, 150, 20, 80);
    } else {
	f_pika_f = new TF1("f_pika_f", fun_m2_pika, -0.5, 0.5, 3);
        f_pika_f->SetParameters(0.01, 1., 1.);
	f_prot_f = new TF1("f_prot_f", fun_m2, 0.7, 1.3, 3);
	f_prot_f->SetParameters(0.3, 1., M2PROT);
    }
    TF1 *f_pion = new TF1("f_pion", fun_m2, -0.5, 1.5, 3);
    f_pion->SetLineWidth(1.2);
    f_pion->SetLineColor(2);
    TF1 *f_kaon = new TF1("f_kaon", fun_m2, -0.5, 1.5, 3);
    f_kaon->SetLineWidth(1.2);
    f_kaon->SetLineColor(4);
    TF1 *f_prot = new TF1("f_prot", fun_m2, -0.5, 1.5, 3);
    f_prot->SetLineWidth(1.2);
    f_prot->SetLineColor(3);


    // Plot hitogramms
    TCanvas* c1; 
    if(level < 2) {
	gStyle->SetPaperSize(32, 16);
	c1 = new TCanvas("c1", "", 10, 10, 1020, 510);
	c1->Divide(2, 1);
    } else {
	gStyle->SetPaperSize(32, 16);
	c1 = new TCanvas("c1", "", 10, 10, 1020, 510);
	c1->Divide(2, 1);
    }
    TPostScript* ps1 = new TPostScript(psName[level-1], -113);
    if(level < 2) {
	ps1->Range(32, 16);
    } else {
	ps1->Range(32, 16);
    }
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.17);
    gPad->SetLogz();
    gPad->SetGridx();
    gPad->SetGridy();
    h_m2mom->Draw("colz");
    TPaveLabel* l1 = new TPaveLabel(0.37, 0.90, 0.80, 0.98, label[level-1], "NDC");
    Style(l1);
    l1->SetTextColor(2);
    l1->Draw();
    if(level < 2) {
	c1->cd(2);
	gPad->SetLeftMargin(0.15);
	gPad->SetRightMargin(0.05);
	gPad->SetLogy();
	gPad->SetGridx();
	gPad->SetGridy();
	h_m2[2]->Draw();
        h_m2[2]->Fit(f1, "QN");
	f_pion->SetParameters(f1->GetParameter(0), f1->GetParameter(1), M2PION);
	f_kaon->SetParameters(f1->GetParameter(0), f1->GetParameter(2), M2KAON);
	f_prot->SetParameters(f1->GetParameter(0), f1->GetParameter(3), M2PROT);
	f_pion->Draw("same");
	f_kaon->Draw("same");
	f_prot->Draw("same");
	TPaveLabel* l2 = new TPaveLabel(0.41, 0.79, 0.73, 0.88, strBins[2], "NDC");
	Style(l2);
	l2->SetTextColor(4);
	l2->Draw();
	TLegend *leg1 = new TLegend(0.71, 0.62, 0.92, 0.86);
	Style(leg1);
	leg1->AddEntry(f_pion, "  #pi^{+}", "l");
	leg1->AddEntry(f_kaon, "  K^{+}", "l");
	leg1->AddEntry(f_prot, "  p", "l");
	leg1->Draw();
    } else {
	for(Int_t i = 0; i < 1; i++) {
	    c1->cd(i+2);
//	    c1->cd(2);
	    gPad->SetLeftMargin(0.15);
	    gPad->SetRightMargin(0.05);
	    gPad->SetLogy();
	    gPad->SetGridx();
	    gPad->SetGridy();
	    h_m2[i+1]->Draw();
	    h_m2[i+1]->Fit(f_pika_f, "QNR+");
	    h_m2[i+1]->Fit(f_prot_f, "QNR+");
	    f_pion->SetParameters(f_pika_f->GetParameter(0), f_pika_f->GetParameter(1), M2PION);
	    f_kaon->SetParameters(f_pika_f->GetParameter(0), f_pika_f->GetParameter(2), M2KAON);
	    f_prot->SetParameters(f_prot_f->GetParameter(0), f_prot_f->GetParameter(1), f_prot_f->GetParameter(2));
	    f_pion->DrawClone("same");
	    f_kaon->DrawClone("same");
	    f_prot->DrawClone("same");
	    TPaveLabel* l2 = new TPaveLabel(0.41, 0.79, 0.73, 0.88, strBins[i+1], "NDC");
	    Style(l2);
	    l2->SetTextColor(4);
	    l2->Draw();
	    if(0 == i) {
		TLegend *leg1 = new TLegend(0.705, 0.652, 0.935, 0.862);
		Style(leg1);
		leg1->AddEntry(f_pion, "  #pi^{+}", "l");
		leg1->AddEntry(f_kaon, "  K^{+}", "l");
		leg1->AddEntry(f_prot, "  p", "l");
		leg1->Draw();
	    }
	}
    }
    c1->cd(0);
    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_hadron_m2mom_ghost(Int_t level)
{
    // Check level
    if(level<1 || level>3) return;

    // Drawing style
    TPstyle();

    // CBM software version
    char *ver = getenv("CBMVER");

    // Define filenames
    char fileName[3][1000];
    sprintf(fileName[0], "/d/cbm02/kresan/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.99percent.hadron.pdf.root", ver);
    sprintf(fileName[1], "/d/cbm02/kresan/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.99percent.hadron.pdf.recoIdeal.root", ver);
    sprintf(fileName[2], "/d/cbm02/kresan/hadron/%s/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.99percent.hadron.pdf.reco.root", ver);

    // Define PS file names
    char psName[3][1000];
    sprintf(psName[0], "plots/%s/hadron.m2mom.ghost.mc.eps", ver);
    sprintf(psName[1], "plots/%s/hadron.m2mom.ghost.recoIdeal.eps", ver);
    sprintf(psName[2], "plots/%s/hadron.m2mom.ghost.reco.eps", ver);

    // Labels on the plots for diff. levels
    char *label[] = {"MC", "RECO ideal", "RECO"};

    // Open the file
    TFile* file = new TFile(fileName[level-1]);

    // All tracks
    TH2F* h_m2mom = (TH2F*) file->Get("h_m2mom_good");
    Style(h_m2mom, "p (GeV/c)", "m^{2} (GeV^{2}/c^{4})");
    h_m2mom->Scale(1./10000.);
    h_m2mom->GetXaxis()->SetRangeUser(0., 6.);
    h_m2mom->GetYaxis()->SetRangeUser(-0.5, 1.5);
    h_m2mom->GetZaxis()->SetRangeUser(1e-1, 1e2);
    h_m2mom->GetZaxis()->SetTitle("dn/dp/dm^{2}");

    // Ghost tracks
    TH2F* h_m2mom_ghost = (TH2F*) file->Get("h_m2mom_fake");
    Style(h_m2mom_ghost, "p (GeV/c)", "m^{2} (GeV^{2}/c^{4})");
    h_m2mom_ghost->Scale(1./10000.);
    h_m2mom_ghost->GetXaxis()->SetRangeUser(0., 6.);
    h_m2mom_ghost->GetYaxis()->SetRangeUser(-0.5, 1.5);
    h_m2mom_ghost->GetZaxis()->SetRangeUser(1e-1, 1e2);
    h_m2mom_ghost->GetZaxis()->SetTitle("dn/dp/dm^{2}");

    cout << h_m2mom_ghost->GetEntries() /
	(h_m2mom->GetEntries()+h_m2mom_ghost->GetEntries())*100 << endl;

    // Tracks with TOF double hits
    TH2F* h_m2mom_tdh = (TH2F*) file->Get("h_m2mom_tdh");
    Style(h_m2mom_tdh, "p (GeV/c)", "m^{2} (GeV^{2}/c^{4})");
    h_m2mom_tdh->Scale(1./10000.);
    h_m2mom_tdh->GetXaxis()->SetRangeUser(0., 6.);
    h_m2mom_tdh->GetYaxis()->SetRangeUser(-0.5, 1.5);
    h_m2mom_tdh->GetZaxis()->SetRangeUser(1e-1, 1e2);
    h_m2mom_tdh->GetZaxis()->SetTitle("dn/dp/dm^{2}");

    // Ghost tracks without TDH
    TH2F *h_m2mom_gwtdh = new TH2F("h_m2mom_gwtdh", "",
				   200, -10., 10.,
				   900, -1.5, 3.0);
    Style(h_m2mom_gwtdh, "p (GeV/c)", "m^{2} (GeV^{2}/c^{4})");
    h_m2mom_gwtdh->Scale(1./10000.);
    h_m2mom_gwtdh->GetZaxis()->SetTitle("dn/dp/dm^{2}");
    h_m2mom_gwtdh->Add(h_m2mom_ghost);
    h_m2mom_gwtdh->Add(h_m2mom_tdh, -1);
    h_m2mom_gwtdh->GetXaxis()->SetRangeUser(0., 6.);
    h_m2mom_gwtdh->GetYaxis()->SetRangeUser(-0.5, 1.5);
    h_m2mom_gwtdh->GetZaxis()->SetRangeUser(1e-1, 1e2);

    gStyle->SetPaperSize(32, 32);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 1020, 1020);
    c1->Divide(2, 2);
    TPostScript* ps1 = new TPostScript(psName[level-1], -113);
    ps1->Range(32, 32);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.17);
    gPad->SetLogz();
    gPad->SetGridx();
    gPad->SetGridy();
    h_m2mom->Draw("colz");
    TPaveLabel* l1 = new TPaveLabel(0.37, 0.90, 0.80, 0.98, label[level-1], "NDC");
    Style(l1);
    l1->SetTextColor(2);
    l1->Draw();
    TPaveLabel *l2 = new TPaveLabel(0.25, 0.80, 0.75, 0.88, "good", "NDC");
    Style(l2);
    l2->DrawClone();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.17);
    gPad->SetLogz();
    gPad->SetGridx();
    gPad->SetGridy();
    h_m2mom_ghost->Draw("colz");
    l2->SetLabel("ghost");
    l2->DrawClone();
    c1->cd(3);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.17);
    gPad->SetLogz();
    gPad->SetGridx();
    gPad->SetGridy();
    h_m2mom_tdh->Draw("colz");
    l2->SetLabel("ghost (TDH)");
    l2->DrawClone();
    c1->cd(4);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.17);
    if(h_m2mom_gwtdh->GetMaximum() > 1) {
	gPad->SetLogz();
    }
    gPad->SetGridx();
    gPad->SetGridy();
    h_m2mom_gwtdh->Draw("colz");
    l2->SetLabel("ghost (w/o TDH)");
    l2->DrawClone();
    c1->cd(0);
    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_hadron_sm2()
{
    TPstyle();

    TFile* file = new TFile("/d/cbm06/user/kresan/hadron/head/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.99percent.fitted.root");
    TFile* file_r = new TFile("/d/cbm06/user/kresan/hadron/head/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.99percent.fitted.reco.root");
    TH1F* h_sm2 = (TH1F*) file->Get("h_sm2");
//    h_sm2->GetXaxis()->SetRangeUser(1., 5.);
//    h_sm2->GetYaxis()->SetRangeUser(0., 0.6);
    TH1F* h_sm2_r = (TH1F*) file_r->Get("h_sm2");
    h_sm2_r->GetXaxis()->SetRangeUser(1., 5.);
    h_sm2_r->GetYaxis()->SetRangeUser(0., 0.6);
//    Style(h_sm2, "p (GeV/c)", "#sigma_{m^{2}} (GeV^{2}/c^{4})");
    Style(h_sm2_r, "p (GeV/c)", "#sigma_{m^{2}} (GeV^{2}/c^{4})");
//    h_sm2_r->SetLineColor(3);

    TF1 *f1 = new TF1("f1", "0.05605928625", 1., 10.);
    f1->SetLineWidth(2);
    f1->SetLineColor(6);
    f1->SetLineStyle(2);
    TF1 *f2 = new TF1("f2", "0.21521862875", 1., 10.);
    f2->SetLineWidth(2);
    f2->SetLineColor(4);
    f2->SetLineStyle(2);

    gStyle->SetPaperSize(16, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript* ps1 = new TPostScript("plots/head/hadron.sm2.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
//    h_sm2->Draw();
//    h_sm2_r->Draw("same");
    h_sm2_r->Draw();
    TLegend* leg1 = new TLegend(0.2648, 0.693, 0.504, 0.844);
    Style(leg1);
    leg1->AddEntry(h_sm2, "MC", "pl");
    leg1->AddEntry(h_sm2_r, "RECO", "pl");
//    leg1->Draw();
    f1->Draw("same");
    f2->Draw("same");
    TPaveLabel* label1 = new TPaveLabel(0.178, 0.402, 0.573, 0.475, "2#sigma separation", "NDC");
    Style(label1);
    label1->Draw();
    TPaveLabel* label2 = new TPaveLabel(0.759, 0.317, 0.949, 0.376, "p and #pi", "NDC");
    Style(label2);
    label2->SetTextColor(4);
    label2->Draw();
    TPaveLabel* label3 = new TPaveLabel(0.751, 0.164, 0.949, 0.226, "K and #pi", "NDC");
    Style(label3);
    label3->SetTextColor(6);
    label3->Draw();
    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_hadron_kaoneff()
{
    TPstyle();

    TFile* file = new TFile("/d/cbm06/user/kresan/hadron/head/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.99percent.fitted.root");
    TFile* file_r = new TFile("/d/cbm06/user/kresan/hadron/head/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.99percent.fitted.reco.root");
    TH1F* h_eff = (TH1F*) file->Get("h_eff_ka");
    TH1F* h_eff_r = (TH1F*) file_r->Get("h_eff_ka");
    Style(h_eff, "p (GeV/c)", "efficiency (%)");
    h_eff->GetXaxis()->SetRangeUser(1., 8.);
    h_eff_r->SetLineColor(3);

    gStyle->SetPaperSize(16, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript* ps1 = new TPostScript("plots/head/hadron.kaoneff.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_eff->Draw();
    h_eff_r->Draw("same");
    TLegend* leg1 = new TLegend(0.692, 0.737, 0.877, 0.865);
    Style(leg1);
    leg1->AddEntry(h_eff, "MC", "l");
    leg1->AddEntry(h_eff_r, "RECO", "l");
    leg1->Draw();
    ps1->Close();
}
// ------------------------------------------------------------------












// ------------------------------------------------------------------
void plot_hadron_kaonmom()
{
    TPstyle();

    TFile* file = new TFile("/lustre/cbm/user/kresan/hadron/head/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.90percent.hadron.spectra.root");

    TH1F* h_all = (TH1F*) file->Get("h_mom_all_kap");
    TH1F* h_acc = (TH1F*) file->Get("h_mom_acc_kap");
    TH1F* h_rpid = (TH1F*) file->Get("h_mom_rpid_kap");
    Style(h_all, "p (GeV/c)", "dn/dp");
    h_all->SetLineStyle(2);
    h_all->GetXaxis()->SetRangeUser(1., 10.);
    h_rpid->SetFillColor(42);

    TH1F* h1 = new TH1F("h1", "", 40, 0., 10.);
    for(Int_t i = 0; i < 100; i++) {
	h1->SetBinContent(i+1, h_rpid->GetBinContent(i+1));
    }
    h1->SetFillColor(41);

    gStyle->SetPaperSize(16, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript* ps1 = new TPostScript("plots/head/hadron.kaonmom.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_all->Draw();
    h1->Draw("same");
    h_acc->Draw("same");
    TLegend* leg1 = new TLegend(0.443, 0.463, 0.943, 0.892);
    Style(leg1);
    leg1->AddEntry(h_all, "ALL", "pl");
    leg1->AddEntry(h_acc, "ACCEPTED", "pl");
    leg1->AddEntry(h1, "RECO + PID", "f");
    leg1->Draw();
    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_hadron_pty()
{
    TPstyle();

    TFile* file = new TFile("/lustre/cbm/user/kresan/hadron/head/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.50percent.hadron.spectra.root");
    TH2F* h_all_pip = (TH2F*) file->Get("h_pty_all_pip");
    TH2F* h_all_kap = (TH2F*) file->Get("h_pty_all_kap");
    TH2F* h_all_prot = (TH2F*) file->Get("h_pty_all_prot");
    TH2F* h_acc_pip = (TH2F*) file->Get("h_pty_acc_pip");
    TH2F* h_acc_kap = (TH2F*) file->Get("h_pty_acc_kap");
    TH2F* h_acc_prot = (TH2F*) file->Get("h_pty_acc_prot");
    TH2F* h_rpid_pip = (TH2F*) file->Get("h_pty_rpid_pip");
    TH2F* h_rpid_kap = (TH2F*) file->Get("h_pty_rpid_kap");
    TH2F* h_rpid_prot = (TH2F*) file->Get("h_pty_rpid_prot");
    Style(h_all_pip, "y", "p_{t} (GeV/c)");
    Style(h_all_kap, "y", "p_{t} (GeV/c)");
    Style(h_all_prot, "y", "p_{t} (GeV/c)");
    Style(h_acc_pip, "y", "p_{t} (GeV/c)");
    Style(h_acc_kap, "y", "p_{t} (GeV/c)");
    Style(h_acc_prot, "y", "p_{t} (GeV/c)");
    Style(h_rpid_pip, "y", "p_{t} (GeV/c)");
    Style(h_rpid_kap, "y", "p_{t} (GeV/c)");
    Style(h_rpid_prot, "y", "p_{t} (GeV/c)");

    h_all_pip->GetXaxis()->SetTitleSize(0.07);
    h_all_pip->GetXaxis()->SetLabelSize(0.07);
    h_all_pip->GetXaxis()->SetTitleOffset(1.1);
    h_all_pip->GetYaxis()->SetTitleSize(0.07);
    h_all_pip->GetYaxis()->SetLabelSize(0.07);
    h_all_pip->GetYaxis()->SetTitleOffset(1.1);

    h_all_kap->GetXaxis()->SetTitleSize(0.07);
    h_all_kap->GetXaxis()->SetLabelSize(0.07);
    h_all_kap->GetXaxis()->SetTitleOffset(1.1);
    h_all_kap->GetYaxis()->SetTitleSize(0.07);
    h_all_kap->GetYaxis()->SetLabelSize(0.07);
    h_all_kap->GetYaxis()->SetTitleOffset(1.1);

    h_all_prot->GetXaxis()->SetTitleSize(0.07);
    h_all_prot->GetXaxis()->SetLabelSize(0.07);
    h_all_prot->GetXaxis()->SetTitleOffset(1.1);
    h_all_prot->GetYaxis()->SetTitleSize(0.07);
    h_all_prot->GetYaxis()->SetLabelSize(0.07);
    h_all_prot->GetYaxis()->SetTitleOffset(1.1);

    h_acc_pip->GetXaxis()->SetTitleSize(0.07);
    h_acc_pip->GetXaxis()->SetLabelSize(0.07);
    h_acc_pip->GetXaxis()->SetTitleOffset(1.1);
    h_acc_pip->GetYaxis()->SetTitleSize(0.07);
    h_acc_pip->GetYaxis()->SetLabelSize(0.07);
    h_acc_pip->GetYaxis()->SetTitleOffset(1.1);

    h_acc_kap->GetXaxis()->SetTitleSize(0.07);
    h_acc_kap->GetXaxis()->SetLabelSize(0.07);
    h_acc_kap->GetXaxis()->SetTitleOffset(1.1);
    h_acc_kap->GetYaxis()->SetTitleSize(0.07);
    h_acc_kap->GetYaxis()->SetLabelSize(0.07);
    h_acc_kap->GetYaxis()->SetTitleOffset(1.1);

    h_acc_prot->GetXaxis()->SetTitleSize(0.07);
    h_acc_prot->GetXaxis()->SetLabelSize(0.07);
    h_acc_prot->GetXaxis()->SetTitleOffset(1.1);
    h_acc_prot->GetYaxis()->SetTitleSize(0.07);
    h_acc_prot->GetYaxis()->SetLabelSize(0.07);
    h_acc_prot->GetYaxis()->SetTitleOffset(1.1);

    h_rpid_pip->GetXaxis()->SetTitleSize(0.07);
    h_rpid_pip->GetXaxis()->SetLabelSize(0.07);
    h_rpid_pip->GetXaxis()->SetTitleOffset(1.1);
    h_rpid_pip->GetYaxis()->SetTitleSize(0.07);
    h_rpid_pip->GetYaxis()->SetLabelSize(0.07);
    h_rpid_pip->GetYaxis()->SetTitleOffset(1.1);

    h_rpid_kap->GetXaxis()->SetTitleSize(0.07);
    h_rpid_kap->GetXaxis()->SetLabelSize(0.07);
    h_rpid_kap->GetXaxis()->SetTitleOffset(1.1);
    h_rpid_kap->GetYaxis()->SetTitleSize(0.07);
    h_rpid_kap->GetYaxis()->SetLabelSize(0.07);
    h_rpid_kap->GetYaxis()->SetTitleOffset(1.1);

    h_rpid_prot->GetXaxis()->SetTitleSize(0.07);
    h_rpid_prot->GetXaxis()->SetLabelSize(0.07);
    h_rpid_prot->GetXaxis()->SetTitleOffset(1.1);
    h_rpid_prot->GetYaxis()->SetTitleSize(0.07);
    h_rpid_prot->GetYaxis()->SetLabelSize(0.07);
    h_rpid_prot->GetYaxis()->SetTitleOffset(1.1);

    TF1* f1 = new TF1("f1", fun_pty_p, 0., 4., 2);
    f1->SetParameter(0, MKAON);
    f1->SetParameter(1, 5.0);
    f1->SetLineWidth(2);
    f1->SetLineColor(2);

    TF1* f_pi_27 = new TF1("f_pi_27", fun_pty_theta, 0., 4., 2);
    f_pi_27->SetParameter(0, MPION);
    f_pi_27->SetParameter(1, 27*TMath::Pi()/180.);
    f_pi_27->SetLineWidth(2);
    TF1* f_pi_3 = new TF1("f_pi_3", fun_pty_theta, 0., 4., 2);
    f_pi_3->SetParameter(0, MPION);
    f_pi_3->SetParameter(1, 2.5*TMath::Pi()/180.);
    f_pi_3->SetLineWidth(2);

    TF1* f_ka_27 = new TF1("f_ka_27", fun_pty_theta, 0., 4., 2);
    f_ka_27->SetParameter(0, MKAON);
    f_ka_27->SetParameter(1, 27*TMath::Pi()/180.);
    f_ka_27->SetLineWidth(2);
    TF1* f_ka_3 = new TF1("f_ka_3", fun_pty_theta, 0., 4., 2);
    f_ka_3->SetParameter(0, MKAON);
    f_ka_3->SetParameter(1, 2.5*TMath::Pi()/180.);
    f_ka_3->SetLineWidth(2);

    TF1* f_prot_27 = new TF1("f_prot_27", fun_pty_theta, 0., 4., 2);
    f_prot_27->SetParameter(0, MPROT);
    f_prot_27->SetParameter(1, 27*TMath::Pi()/180.);
    f_prot_27->SetLineWidth(2);
    TF1* f_prot_3 = new TF1("f_prot_3", fun_pty_theta, 0., 4., 2);
    f_prot_3->SetParameter(0, MPROT);
    f_prot_3->SetParameter(1, 2.5*TMath::Pi()/180.);
    f_prot_3->SetLineWidth(2);

    cout << "pi" << endl;
    cout << "Acceptance: " << 100*(Double_t)h_acc_pip->GetEntries()/(Double_t)h_all_pip->GetEntries() << endl;
    cout << "RECO + PID: " << 100*(Double_t)h_rpid_pip->GetEntries()/(Double_t)h_acc_pip->GetEntries() << endl;
    cout << endl;
    cout << "K" << endl;
    cout << "Acceptance: " << 100*(Double_t)h_acc_kap->GetEntries()/(Double_t)h_all_kap->GetEntries() << endl;
    cout << "RECO + PID: " << 100*(Double_t)h_rpid_kap->GetEntries()/(Double_t)h_acc_kap->GetEntries() << endl;
    cout << endl;
    cout << "p" << endl;
    cout << "Acceptance: " << 100*(Double_t)h_acc_prot->GetEntries()/(Double_t)h_all_prot->GetEntries() << endl;
    cout << "RECO + PID: " << 100*(Double_t)h_rpid_prot->GetEntries()/(Double_t)h_acc_prot->GetEntries() << endl;
    cout << endl;

    gStyle->SetPaperSize(48, 48);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 1020, 1020);
    c1->Divide(3, 3, 0, 0);
    TPostScript* ps1 = new TPostScript("plots/head/hadron.pty.eps", -113);
    ps1->Range(48, 48);

    c1->cd(1);
    gPad->SetLeftMargin(0.2);
    gPad->SetRightMargin(0.);
    h_all_pip->Draw("col");
    TPaveLabel* label_all = new TPaveLabel(0.220, 0.788, 0.485, 0.981, "ALL", "NDC");
    Style(label_all);
    label_all->Draw();
    TPaveLabel* label_pip = new TPaveLabel(0.821, 0.813, 0.987, 0.988, "#pi", "NDC");
    Style(label_pip);
    label_pip->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.);
    gPad->SetRightMargin(0.);
    h_all_kap->Draw("col");
    TPaveLabel* label_kap = new TPaveLabel(0.821, 0.813, 0.987, 0.988, "K", "NDC");
    Style(label_kap);
    label_kap->Draw();
    c1->cd(3);
    gPad->SetLeftMargin(0.);
    gPad->SetRightMargin(0.01);
    h_all_prot->Draw("col");
    TPaveLabel* label_prot = new TPaveLabel(0.821, 0.813, 0.987, 0.988, "p", "NDC");
    Style(label_prot);
    label_prot->Draw();

    c1->cd(4);
    gPad->SetLeftMargin(0.2);
    gPad->SetRightMargin(0.);
    h_acc_pip->Draw("col");
    f_pi_27->Draw("same");
    f_pi_3->Draw("same");
    TPaveLabel* label_acc = new TPaveLabel(0.220, 0.788, 0.485, 0.981, "ACC", "NDC");
    Style(label_acc);
    label_acc->Draw();
    c1->cd(5);
    gPad->SetLeftMargin(0.);
    gPad->SetRightMargin(0.);
    h_acc_kap->Draw("col");
    f_ka_27->Draw("same");
    f_ka_3->Draw("same");
    c1->cd(6);
    gPad->SetLeftMargin(0.);
    gPad->SetRightMargin(0.01);
    h_acc_prot->Draw("col");
    f_prot_27->Draw("same");
    f_prot_3->Draw("same");

    c1->cd(7);
    gPad->SetLeftMargin(0.2);
    gPad->SetRightMargin(0.);
    gPad->SetBottomMargin(0.15);
    h_rpid_pip->Draw("col");
    f_pi_27->Draw("same");
    f_pi_3->Draw("same");
    TPaveLabel* label_pid = new TPaveLabel(0.220, 0.788, 0.485, 0.981, "PID", "NDC");
    Style(label_pid);
    label_pid->Draw();
    c1->cd(8);
    gPad->SetLeftMargin(0.);
    gPad->SetRightMargin(0.);
    gPad->SetBottomMargin(0.15);
    h_rpid_kap->Draw("col");
    f_ka_27->Draw("same");
    f_ka_3->Draw("same");
    f1->Draw("same");
    TPaveLabel* label1 = new TPaveLabel(0.39, 0.82, 0.90, 0.96, "p=5.0 GeV/c", "NDC");
    Style(label1);
    label1->SetTextColor(2);
    label1->Draw();
    c1->cd(9);
    gPad->SetLeftMargin(0.);
    gPad->SetRightMargin(0.01);
    gPad->SetBottomMargin(0.15);
    h_rpid_prot->Draw("col");
    f_prot_27->Draw("same");
    f_prot_3->Draw("same");

    c1->cd(0);
    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_hadron_kaonpty0()
{
    TPstyle();

    TFile* file = new TFile("/lustre/cbm/user/kresan/hadron/head/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.80ps.50percent.hadron.spectra.root");
    TH2F* h_all_kap = (TH2F*) file->Get("h_pty_all_kap");
    TH2F* h_acc_kap = (TH2F*) file->Get("h_pty_acc_kap");
    TH2F* h_rpid_kap = (TH2F*) file->Get("h_pty_rpid_kap");

//    TH1D *h_all_kap_pt = new TH1D("h_all_kap_pt", "", 20, 0., 2.);
//    TH1D *h_acc_kap_pt = new TH1D("h_acc_kap_pt", "", 20, 0., 2.);
//    TH1D *h_rpid_kap_pt = new TH1D("h_rpid_kap_pt", "", 20, 0., 2.);

    TH1D *h_all_kap_pt = h_all_kap->ProjectionY("h_all_kap_pt", 19, 19);
    h_all_kap_pt->Scale(1./0.01 * 1./200000.);
    TH1D *h_acc_kap_pt = h_acc_kap->ProjectionY("h_acc_kap_pt", 19, 19);
    h_acc_kap_pt->Scale(1./0.01 * 1./200000.);
    TH1D *h_rpid_kap_pt = h_rpid_kap->ProjectionY("h_rpid_kap_pt", 19, 19);
    h_rpid_kap_pt->Scale(1./0.01 * 1./200000.);
    Style(h_all_kap_pt, "p_{t} (GeV/c)", "d^{2}n/dp_{t}/dy (GeV/c)^{-1}");
    h_all_kap_pt->SetLineStyle(2);
    h_rpid_kap_pt->SetFillColor(42);

    gStyle->SetPaperSize(16, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript* ps1 = new TPostScript("plots/head/hadron.kaon.pty0.eps", -113);
    ps1->Range(16, 16);

    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    h_all_kap_pt->Draw();
    h_rpid_kap_pt->Draw("same");
    h_acc_kap_pt->Draw("same");
    TLegend *leg1 = new TLegend(0.561, 0.616, 0.939, 0.894);
    Style(leg1);
    leg1->AddEntry(h_all_kap_pt, "4#pi", "l");
    leg1->AddEntry(h_acc_kap_pt, "accepted", "l");
    leg1->AddEntry(h_rpid_kap_pt, "RECO+PID", "f");
    leg1->Draw();

    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_hadron_kaonmt()
{
    TPstyle();

    TFile* file = new TFile("/d/cbm02/kresan/hadron/JAN06/urqmd/auau/35gev/centr/\
urqmd.auau.35gev.centr.80ps.99percent.hadron.spectra.root");
    TH2F* h_mty_all = (TH2F*) file->Get("h_mty_all_kap");
    TH2F* h_mty_acc = (TH2F*) file->Get("h_mty_acc_kap");
    TH2F* h_mty_reco = (TH2F*) file->Get("h_mty_reco_kap");
    TH2F* h_mty_pid = (TH2F*) file->Get("h_mty_recopid_kap");
    TH1D* h_mt_all = h_mty_all->ProjectionY("h_mt_all", 21, 21);
    TH1D* h_mt_acc = h_mty_acc->ProjectionY("h_mt_acc", 21, 21);
    TH1D* h_mt_reco = h_mty_reco->ProjectionY("h_mt_reco", 21, 21);
    TH1D* h_mt_pid = h_mty_pid->ProjectionY("h_mt_pid", 21, 21);
    Style(h_mt_all, "m_{t} at ycm (GeV^{2}/c^{4})", "dn/dm_{t}");
    h_mt_all->SetLineStyle(2);
    h_mt_all->SetMarkerStyle(25);
    h_mt_acc->SetMarkerStyle(20);
    h_mt_reco->SetLineColor(2);
    h_mt_reco->SetMarkerStyle(21);
    h_mt_reco->SetMarkerColor(2);
    h_mt_pid->SetLineColor(4);
    h_mt_pid->SetMarkerStyle(22);
    h_mt_pid->SetMarkerColor(4);

    gStyle->SetPaperSize(16, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript* ps1 = new TPostScript("plots/hadron.kaonmt.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    h_mt_all->Draw("pe1");
    h_mt_acc->Draw("pe1same");
    h_mt_reco->Draw("pe1same");
    h_mt_pid->Draw("pe1same");
    TLegend* leg1 = new TLegend(0.542, 0.595, 0.943, 0.894);
    Style(leg1);
    leg1->AddEntry(h_mt_all, "all", "p");
    leg1->AddEntry(h_mt_acc, "accepted", "p");
    leg1->AddEntry(h_mt_reco, "reconstructed", "p");
    leg1->AddEntry(h_mt_pid, "identified", "p");
    leg1->Draw();
    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_stseff()
{
    TPstyle();

    TFile* file = new TFile("/misc/kresan/jan06/cbmroot2/macro/hadron/reco/histo.root");
    TProfile* p_eff = (TProfile*) file->Get("p_eff_all_vs_mom");
    p_eff->SetLineColor(2);
    p_eff->SetMarkerColor(2);

    gStyle->SetPaperSize(16, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript* ps1 = new TPostScript("plots/sts.eff.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetGridx();
    gPad->SetGridy();
    p_eff->Draw();
    Style(p_eff, "p (GeV/c)", "efficiency (%)");
    p_eff->GetYaxis()->SetRangeUser(1, 101);
    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_stsmomres()
{
    TPstyle();

    TFile* file = new TFile("/misc/kresan/jan06/cbmroot2/macro/hadron/reco/histo.root");
    TH2F* h_momres = (TH2F*) file->Get("P_vs_P");
    Style(h_momres, "p (GeV/c)", "#deltap/p");

    TH1F* h_sigma = new TH1F("h_sigma", "", 20, 0., 20.);
    Style(h_sigma, "p (GeV/c)", "resolution");
    h_sigma->SetMarkerStyle(21);
    h_sigma->SetMarkerColor(2);
    h_sigma->GetXaxis()->SetRangeUser(0., 10.);
    h_sigma->GetYaxis()->SetRangeUser(0., 0.02);
    TF1* f1 = new TF1("f1", "gaus", -0.05, 0.05);
    TH1D* h_py;
    for(Int_t i = 0; i < 20; i++) {
	h_py = h_momres->ProjectionY("h_py", i+1, i+1);
	h_py->Fit(f1, "QN");
        h_sigma->SetBinContent(i+1, f1->GetParameter(2));
        h_sigma->SetBinError(i+1, f1->GetParError(2));
    }

    gStyle->SetPaperSize(32, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 1020, 510);
    c1->Divide(2, 1);
    TPostScript* ps1 = new TPostScript("plots/sts.momres.eps", -113);
    ps1->Range(32, 16);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_momres->Draw("cont0");
    h_momres->GetXaxis()->SetRangeUser(0., 10.);
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetGridx();
    gPad->SetGridy();
    h_sigma->Draw("pe1");
    c1->cd(0);
    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_trdeff()
{
    TPstyle();

    TFile* file = new TFile("/d/cbm02/kresan/reco/JAN06/urqmd/auau/35gev/centr/80ps/\
urqmd.auau.35gev.centr.0002.80ps.reco.root");
    TH1F* h_eff = (TH1F*) file->Get("h_mom_eff_good");
    Style(h_eff, "p (GeV/c)", "efficiency");
    h_eff->Rebin(5);
    h_eff->Scale(1./5.);
    h_eff->GetYaxis()->SetRangeUser(0.01, 1.01);
    h_eff->SetLineColor(2);
    h_eff->SetMarkerColor(2);
//    h_eff->SetMarkerStyle(21);

    gStyle->SetPaperSize(16, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript* ps1 = new TPostScript("plots/trd.eff.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetGridx();
    gPad->SetGridy();
    h_eff->Draw("");
    ps1->Close();
}
// ------------------------------------------------------------------



// ------------------------------------------------------------------
void plot_glbeff()
{
    TPstyle();

    TFile* file = new TFile("/d/cbm02/kresan/reco/JAN06/urqmd/auau/35gev/centr/80ps/\
urqmd.auau.35gev.centr.0002.80ps.reco.root");
    TH1F* h_eff = (TH1F*) file->Get("h_mom_eff_good_glb");
    Style(h_eff, "p (GeV/c)", "efficiency");
    h_eff->Rebin(5);
    h_eff->Scale(1./5.);
    h_eff->GetYaxis()->SetRangeUser(0.01, 1.01);
    h_eff->SetLineColor(4);
    h_eff->SetMarkerColor(4);
//    h_eff->SetMarkerStyle(21);

    gStyle->SetPaperSize(16, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript* ps1 = new TPostScript("plots/glb.eff.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetGridx();
    gPad->SetGridy();
    h_eff->Draw("");
    ps1->Close();
}
// ------------------------------------------------------------------






Double_t fun_pty_theta(Double_t *x, Double_t *par)
{
  Double_t shy = TMath::SinH(x[0]);
  Double_t chy = TMath::CosH(x[0]);
  Double_t sint = TMath::Sin(par[1]);
  Double_t cost = TMath::Cos(par[1]);
  Double_t temp = cost*cost*chy*chy - shy*shy;
  if(temp > 0)
    return par[0]*sint*shy / TMath::Sqrt(temp);
  else
    return 2.6;
}

Double_t fun_pty_p(Double_t *x, Double_t *par)
{
  Double_t m2 = par[0] * par[0];
  Double_t p2 = par[1] * par[1];
  Double_t sh2y = TMath::Power(TMath::SinH(x[0]), 2);
  Double_t chy = TMath::CosH(x[0]);
  if( (m2*sh2y) > p2 ) {
    return 0.;
  }
  return TMath::Sqrt( p2 - m2*sh2y ) / chy;
}



