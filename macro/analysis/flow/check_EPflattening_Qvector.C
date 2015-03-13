// -------------------------------------------------------------------------
// ----- This task draws the average X-component of the Q-vector versus detector multiplicity - here for the PSD: <Qx> vs Energy

void check_EPflattening_Qvector()
{
    TFile *f1 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au2au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8.root","R");
    TFile *f2 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au4au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8.root","R");
    TFile *f3 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au6au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8.root","R");
    TFile *f4 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au10au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8.root","R");
    TFile *f5 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au15au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/15m/ana/ana_10Kevt_RECOtracks_STScutY_0.8.root","R");
    TFile *f6 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au25au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/15m/ana/ana_10Kevt_RECOtracks_STScutY_0.8.root","R");
    TFile *f7 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au35au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/15m/ana/ana_10Kevt_RECOtracks_STScutY_0.8.root","R");
    
    TString Q[4];
    Q[0] =  "PsdEvent.fQx";
    Q[1] = "PsdEvent.fQx_sub1";
    Q[2] = "PsdEvent.fQx_sub2";
    Q[3] = "PsdEvent.fQx_sub3";
    TString Q_tmp;

    TString E[4];
    E[0] = "PsdEvent.fedep_centralmod_hole+PsdEvent.fedep_1stCorona+PsdEvent.fedep_2ndCorona";
    E[1] = "PsdEvent.fedep_centralmod_hole";
    E[2] = "PsdEvent.fedep_1stCorona";
    E[3] = "PsdEvent.fedep_2ndCorona";
    TString E_tmp;

    TString name_Yaxis = "<Qx> [GeV]";
    TString sel = "StsEvent.fmult>40";
    //sel="McEvent.fB<10.";
    TString num, range;
    range="";
    //range ="(10, -3.15, 3.15)";

    TTree* t1 = (TTree*) f1->Get("cbmsim");
    TTree* t2 = (TTree*) f2->Get("cbmsim");
    TTree* t3 = (TTree*) f3->Get("cbmsim");
    TTree* t4 = (TTree*) f4->Get("cbmsim");
    TTree* t5 = (TTree*) f5->Get("cbmsim");
    TTree* t6 = (TTree*) f6->Get("cbmsim");
    TTree* t7 = (TTree*) f7->Get("cbmsim");

    TCanvas* c2 = new TCanvas("c3"," Qx vs E (PSD) ",200,10,700,500);
    gStyle->SetOptStat(0);
    c2->Divide(4, 2);

    TH2F* h[4];
    TProfile *p[4];
    TString repeat="";

    c2->cd(1);

    for (int i=0;i<4;i++)
    {
        //if (i>0) continue;
	Q_tmp = Q[i];
        E_tmp = E[i];
	if (i==0) repeat="";
	else repeat="same";
	num="";
        num+=i;

	t1->Draw(Q_tmp +":"+ E_tmp +">>h1"+num+range, sel, "goff");
	h[i]=(TH2F*)t1->GetHistogram();
        p[i] = h[i]->ProfileX("p1"+num, 0, -1);

	p[i]->SetLineColor(i+1);
	p[i]->Draw(repeat);

	if (i==0) p[i]->GetYaxis()->SetRangeUser(-2, 50);
        if (i==0) p[i]->GetXaxis()->SetTitle("PSD energy [GeV]");
	if (i==0) p[i]->GetYaxis()->SetTitle(name_Yaxis);
    }

    c2->cd(2);

    for (int i=0;i<4;i++)
    {
        //if (i>0) continue;
	Q_tmp = Q[i];
        E_tmp = E[i];
	if (i==0) repeat="";
	else repeat="same";
	num="";
        num+=i;

	t2->Draw(Q_tmp +":"+ E_tmp +">>h2"+num+range, sel, "goff");
	h[i]=(TH2F*)t2->GetHistogram();
        p[i] = h[i]->ProfileX("p2"+num, 0, -1);

	p[i]->SetLineColor(i+1);
	p[i]->Draw(repeat);

	if (i==0) p[i]->GetYaxis()->SetRangeUser(-2, 50);
        if (i==0) p[i]->GetXaxis()->SetTitle("PSD energy [GeV]");
	if (i==0) p[i]->GetYaxis()->SetTitle(name_Yaxis);
    }

    c2->cd(3);

    for (int i=0;i<4;i++)
    {
        //if (i>0) continue;
	Q_tmp = Q[i];
        E_tmp = E[i];
	if (i==0) repeat="";
	else repeat="same";
	num="";
        num+=i;

	t3->Draw(Q_tmp +":"+ E_tmp +">>h3"+num+range, sel, "goff");
	h[i]=(TH2F*)t3->GetHistogram();
        p[i] = h[i]->ProfileX("p3"+num, 0, -1);

	p[i]->SetLineColor(i+1);
	p[i]->Draw(repeat);

	if (i==0) p[i]->GetYaxis()->SetRangeUser(-2, 30);
        if (i==0) p[i]->GetXaxis()->SetTitle("PSD energy [GeV]");
	if (i==0) p[i]->GetYaxis()->SetTitle(name_Yaxis);
    }

    c2->cd(4);

    for (int i=0;i<4;i++)
    {
        //if (i>0) continue;
	Q_tmp = Q[i];
        E_tmp = E[i];
	if (i==0) repeat="";
	else repeat="same";
	num="";
        num+=i;

	t4->Draw(Q_tmp +":"+ E_tmp +">>h4"+num+range, sel, "goff");
	h[i]=(TH2F*)t4->GetHistogram();
        p[i] = h[i]->ProfileX("p4"+num, 0, -1);

	p[i]->SetLineColor(i+1);
	p[i]->Draw(repeat);

	if (i==0) p[i]->GetYaxis()->SetRangeUser(-2, 50);
        if (i==0) p[i]->GetXaxis()->SetTitle("PSD energy [GeV]");
	if (i==0) p[i]->GetYaxis()->SetTitle(name_Yaxis);
    }

    c2->cd(5);

    for (int i=0;i<4;i++)
    {
        //if (i>0) continue;
	Q_tmp = Q[i];
        E_tmp = E[i];
	if (i==0) repeat="";
	else repeat="same";
	num="";
        num+=i;

	t5->Draw(Q_tmp +":"+ E_tmp +">>h5"+num+range, sel, "goff");
	h[i]=(TH2F*)t5->GetHistogram();
        p[i] = h[i]->ProfileX("p5"+num, 0, -1);

	p[i]->SetLineColor(i+1);
	p[i]->Draw(repeat);

	if (i==0) p[i]->GetYaxis()->SetRangeUser(-2, 50);
        if (i==0) p[i]->GetXaxis()->SetTitle("PSD energy [GeV]");
	if (i==0) p[i]->GetYaxis()->SetTitle(name_Yaxis);
    }

    c2->cd(6);

    for (int i=0;i<4;i++)
    {
        //if (i>0) continue;
	Q_tmp = Q[i];
        E_tmp = E[i];
	if (i==0) repeat="";
	else repeat="same";
	num="";
        num+=i;

	t6->Draw(Q_tmp +":"+ E_tmp +">>h6"+num+range, sel, "goff");
	h[i]=(TH2F*)t6->GetHistogram();
        p[i] = h[i]->ProfileX("p6"+num, 0, -1);

	p[i]->SetLineColor(i+1);
	p[i]->Draw(repeat);

	if (i==0) p[i]->GetYaxis()->SetRangeUser(-2, 50);
        if (i==0) p[i]->GetXaxis()->SetTitle("PSD energy [GeV]");
	if (i==0) p[i]->GetYaxis()->SetTitle(name_Yaxis);
    }

    c2->cd(7);

    for (int i=0;i<4;i++)
    {
        //if (i>0) continue;
	Q_tmp = Q[i];
        E_tmp = E[i];
	if (i==0) repeat="";
	else repeat="same";
	num="";
        num+=i;

	t7->Draw(Q_tmp +":"+ E_tmp +">>h7"+num+range, sel, "goff");
	h[i]=(TH2F*)t7->GetHistogram();
        p[i] = h[i]->ProfileX("p7"+num, 0, -1);

	p[i]->SetLineColor(i+1);
	p[i]->Draw(repeat);

	if (i==0) p[i]->GetYaxis()->SetRangeUser(-2, 50);
        if (i==0) p[i]->GetXaxis()->SetTitle("PSD energy [GeV]");
	if (i==0) p[i]->GetYaxis()->SetTitle(name_Yaxis);
    }

    break;

    c2->SaveAs("plot/EP/Qx_profile_beforeflat_2_4_6_10_15_25_35AGeV_colorcodePSD_sub1_2_3.C");
    c2->SaveAs("plot/EP/Qx_profile_beforeflat_2_4_6_10_15_25_35AGeV_colorcodePSD_sub1_2_3.eps");
    c2->SaveAs("plot/EP/Qx_profile_beforeflat_2_4_6_10_15_25_35AGeV_colorcodePSD_sub1_2_3.gif");

}