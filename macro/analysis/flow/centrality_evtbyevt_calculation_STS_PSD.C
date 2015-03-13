// -------------------------------------------------------------------------
// ----- This task calculate the collision centrality for each event, based on STS and PSD information


void centrality_evtbyevt_calculation_STS_PSD()
{
    TFile *f1 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au2au/sts_13d_PSDhole6cm_44mods_beampipe/v15/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8.root","R");
    TTree* t1 = (TTree*) f1->Get("cbmsim");

    //TString outName = "/hera/cbm/users/sseddiki/auau_TDR/shield/au10au/sts_13d_PSDhole6cm_44mods_beampipe/v15/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8_centrality.root";
    //cout << "output: " << outName << endl;
    //TFile *outFile = new TFile(outName, "RECREATE");

    // variable declaration

    Double_t b[20];
    for (int i=0;i<20;i++) b[i] = 0.;
    int bin[20];
    for (int i=0;i<20;i++) bin[i] = 0;

    double Nstep = 20.;
    double step = 100./Nstep;
    cout << "step = " << step << endl;

    int color_ind;
    TF1* funct;
    double stat_sample = 0.;

    TString detinfo = "";
    TString sPreSel = ""; // baseline
    TString upRange = "";

    TString cut  = "";
    TString sPSDsub1 = "centralmod_hole";
    TString sPSDsub2 = "1stCorona";
    TString sPSDsub3 = "2ndCorona";

    Double_t entry = 0.;
    double Lcte = 0.05;
    double LcteX;

    TString alpha[20];
    TString cte[20];
    double tmp1[20];
    double tmp2[20];

    TString alpha_tmp = "";
    TString cte_tmp = "";
    double tmp1_tmp = 0.;
    double tmp2_tmp = 0.;

    TString cut_nodc[20];

    for (i=0;i<20;i++)
    {
	alpha[i] = "";
	cte[i] = "";
	tmp1[i] = 0.;
	tmp2[i] = 0.;

        cut_nodc[i] = "";
    }

    TF1* fcut;
    TF1* fcut2;
    TString fname = "";
    TString alpha_middle = "";
    TString cte_middle = "";

    TString hname = "";
    TString cut_r_PSD = "";
    TString cut_l_PSD = "";
    TString cut_r_PSD1vsSTS = "";
    TString cut_l_PSD1vsSTS = "";
    TString cut_r_PSD2vsSTS = "";
    TString cut_l_PSD2vsSTS = "";
    TString cut_r_PSD3vsSTS = "";
    TString cut_l_PSD3vsSTS = "";
    TString cut_rl = "";

    // Histos
    TH1F* hPSD_base;

    TH2F* hPSD = new TH2F("hPSD","for fit function", 1000, 0., 1., 1000, 0., 1.);
    TH2F* hPSD_updown;
    TH1F* hPSD_tmp;
    TH1F* hPSD_b;

    TH2F* hPSD1vsSTS = new TH2F("hPSD1vsSTS","for fit function (sub1)", 1000, 0., 1., 1000, 0., 1.);
    TH2F* hPSD2vsSTS = new TH2F("hPSD2vsSTS","for fit function (sub2)", 1000, 0., 1., 1000, 0., 1.);
    TH2F* hPSD3vsSTS = new TH2F("hPSD3vsSTS","for fit function (sub3)", 1000, 0., 1., 1000, 0., 1.);

    TH2F* hPSD1vsSTS_updown;
    TH2F* hPSD2vsSTS_updown;
    TH2F* hPSD3vsSTS_updown;

    TH1F* hPSDvsSTS_tmp;
    TH1F* hPSDvsSTS_b;

    TH1F* hb_rej_PSD = new TH1F("hb_rej_PSD","rej - b",170,0.,17.);//,100,0.,1.);
    TH2F* hb_rej_PSD1_corrSTS = new TH2F("hb_rej_PSD1_corrSTS","rej - corr STS",100,0.,1.,100,0.,1.);
    TH2F* hb_rej_PSD2_corrSTS = new TH2F("hb_rej_PSD2_corrSTS","rej - corr STS",100,0.,1.,100,0.,1.);

    TH1F* hb_rej_PSDvsSTS = new TH1F("rej - b (PSD vs STS)","rej - b",170,0.,17.);


    // TTree
    TBranch *b_E_sub1;
    TBranch *b_E_sub2;
    TBranch *b_E_sub3;
    TBranch *b_M_STS;
    TBranch *b_mcB;

    TBranch *b_E_sub12;
    TBranch *b_E_sub23;
    TBranch *b_STS;
    TBranch *b_PSD;
    TBranch *b_PSD1vsSTS;
    TBranch *b_PSD2vsSTS;
    TBranch *b_PSD3vsSTS;

    float E_sub1, E_sub2, E_sub3;
    int M_STS;
    double mcB;

    double M_STS_bis;
    double E_sub1_bis;
    double E_sub2_bis;
    double E_sub3_bis;
    double E_sub12_bis;
    double E_sub23_bis;

    Double_t maxE_1 = 0.;
    Double_t maxE_2 = 0.;
    Double_t maxE_3 = 0.;
    Double_t maxE_12 = 0.;
    Double_t maxE_23 = 0.;

    TString smaxE_1 = "";
    TString smaxE_2 = "";
    TString smaxE_3 = "";
    TString smaxE_12 = "";
    TString smaxE_23 = "";

    double cutE1_cleanSub1 = 0.;
    double cutE23_cleanSub1 = 0.;

    bool kSTS =  1;
    bool kPSD =  1;
    bool kPSD1vsSTS =  1;
    bool kPSD2vsSTS =  1;
    bool kPSD3vsSTS =  1;

    double c_STS =  0.;
    double c_PSD =  0.;
    double c_PSD1vsSTS =  0.;
    double c_PSD2vsSTS =  0.;
    double c_PSD3vsSTS =  0.;

    double c_STS_err =  0.;
    double c_PSD_err =  0.;
    double c_PSD1vsSTS_err =  0.;
    double c_PSD2vsSTS_err =  0.;
    double c_PSD3vsSTS_err =  0.;

    // Fit functions
    TF1* pol_PSD;
    TF1* pol_up_PSD;
    TF1* pol_down_PSD;
    TF1* pol_up_PSD_2;
    TF1* pol_down_PSD_2;

    TF1* pol_PSD1vsSTS;
    TF1* pol_up_PSD1vsSTS;
    TF1* pol_down_PSD1vsSTS;
    TF1* pol_up_PSD1vsSTS_2;
    TF1* pol_down_PSD1vsSTS_2;

    TF1* pol_PSD2vsSTS;
    TF1* pol_up_PSD2vsSTS;
    TF1* pol_down_PSD2vsSTS;
    TF1* pol_up_PSD2vsSTS_2;
    TF1* pol_down_PSD2vsSTS_2;

    TF1* pol_PSD3vsSTS;
    TF1* pol_up_PSD3vsSTS;
    TF1* pol_down_PSD3vsSTS;
    TF1* pol_up_PSD3vsSTS_2;
    TF1* pol_down_PSD3vsSTS_2;

    double cut_middle_PSD = 0.;
    double cut_middle_PSD1vsSTS = 0.;
    double cut_middle_PSD2vsSTS = 0.;
    double cut_middle_PSD3vsSTS = 0.;
    TString scut_middle[5];
    for (int i=0;i<5;i++) scut_middle[i]="";

    double PSDmin = 0.;
    double PSDvsSTSmin = 0.;

    double it_prec = 0.001;
    double it_X = 0.;
    double it_test = -10.;

    double range_middlecut_PSD[2];
    double range_middlecut_PSD1vsSTS[2];
    double range_middlecut_PSD2vsSTS[2];
    double range_middlecut_PSD3vsSTS[2];
    for (int i=0;i<2;i++) range_middlecut_PSD[i]=0.;
    for (int i=0;i<2;i++) range_middlecut_PSD1vsSTS[i]=0.;
    for (int i=0;i<2;i++) range_middlecut_PSD2vsSTS[i]=0.;
    for (int i=0;i<2;i++) range_middlecut_PSD3vsSTS[i]=0.;

    // fine cuts
    float scale1 =0.;
    float scale2 =0.;
    double cut_downup = 0.;

    TF1 *fit[100];
    TH2F *hIn;
    TProfile *mean;
    TH1D *meanH;

    TH1D *gaus = new TH1D("gaus","gaus",100,0,1);
    TH1D *rms = new TH1D("rms","rms",100,0,1);
    TH1D *gaus2 = new TH1D("gaus","gaus",100,0,1);
    TH1D *rms2 = new TH1D("rms","rms",100,0,1);
    TH1D *sl[100];

    TH1D *range;
    TH1D *cut_up;
    TH1D *cut_down;
    TH1D *cut_up2;
    TH1D *cut_down2;

    // centrality determination, evt by evt
    double X0=0.;
    double Y0=0.;
    double X1=0.;
    double Y1=0.;

    TF1 *fdistance;

    // ==================================== Distributions & Fit functions for centrality calculation
    // ========== STS

    sPreSel = "(PsdEvent.fedep_centralmod_hole+PsdEvent.fedep_1stCorona+PsdEvent.fedep_2ndCorona != 0. || StsEvent.fmult != 0.)";
    detinfo = "StsEvent.fmult";
    upRange = "1000.";
    
    t1->Draw(detinfo +">>hSTS_base(1000, 0., " + upRange + ")", sPreSel, "goff");
    TH1F* hSTS_base = (TH1F*) t1->GetHistogram();

    Double_t Nentries = hSTS_base->Integral();
    cout << "% event selected: " <<  Nentries << endl;

    Double_t maxMult = 0.;
    for (int ibin = 0; ibin<1000; ibin++)
    {
      if (hSTS_base->GetBinContent(ibin) != 0. && maxMult < hSTS_base->GetBinCenter(ibin)) maxMult = hSTS_base->GetBinCenter(ibin);
    }
    cout << "max STS mult = " << maxMult << endl;

    // ============ Process TTree: normalized Psd.Edep & Sts.mult to compute 2D correlation cuts

    // sub1
    //TCanvas* c2 = new TCanvas();
    upRange = "250.";
    detinfo = "PsdEvent.fedep_" + sPSDsub1;

    t1->Draw(detinfo +">>hPSD_" + sPSDsub1 + "(2500, 0., " + upRange + ")", sPreSel, "goff");
    hPSD_base = (TH1F*) t1->GetHistogram();

    for (int ibin = 0; ibin<2500; ibin++)
    {
      if (hPSD_base->GetBinContent(ibin) != 0. && maxE_1 < hPSD_base->GetBinCenter(ibin)) maxE_1 = hPSD_base->GetBinCenter(ibin);
    }
    cout << "max PSD Edep (sub1) = " << maxE_1 << endl;

    // sub2
    //TCanvas* c3 = new TCanvas();
    upRange = "100.";
    detinfo = "PsdEvent.fedep_" + sPSDsub2;

    t1->Draw(detinfo +">>hPSD_" + sPSDsub2 + "(1000, 0., " + upRange + ")", sPreSel, "goff");
    hPSD_base = (TH1F*) t1->GetHistogram();

    for (int ibin = 0; ibin<1000; ibin++)
    {
      if (hPSD_base->GetBinContent(ibin) != 0. && maxE_2 < hPSD_base->GetBinCenter(ibin)) maxE_2 = hPSD_base->GetBinCenter(ibin);
    }
    cout << "max PSD Edep (sub2) = " << maxE_2 << endl;

    // sub3
    //TCanvas* c4 = new TCanvas();
    upRange = "100.";
    detinfo = "PsdEvent.fedep_" + sPSDsub3;

    t1->Draw(detinfo +">>hPSD_" + sPSDsub3 + "(1000, 0., " + upRange + ")", sPreSel, "goff");
    hPSD_base = (TH1F*) t1->GetHistogram();

    for (int ibin = 0; ibin<1000; ibin++)
    {
      if (hPSD_base->GetBinContent(ibin) != 0. && maxE_3 < hPSD_base->GetBinCenter(ibin)) maxE_3 = hPSD_base->GetBinCenter(ibin);
    }
    cout << "max PSD Edep (sub3) = " << maxE_3 << endl;

    // sub1+sub2
    //TCanvas* c12 = new TCanvas();
    upRange = "100.";
    detinfo = "PsdEvent.fedep_" + sPSDsub1 + "+PsdEvent.fedep_" + sPSDsub2;

    t1->Draw(detinfo +">>hPSD_sub12(2000, 0., 500.)", sPreSel, "goff");
    hPSD_base = (TH1F*) t1->GetHistogram();

    for (int ibin = 0; ibin<1000; ibin++)
    {
      if (hPSD_base->GetBinContent(ibin) != 0. && maxE_12 < hPSD_base->GetBinCenter(ibin)) maxE_12 = hPSD_base->GetBinCenter(ibin);
    }
    cout << "max PSD Edep (sub1+sub2) = " << maxE_12 << endl;

    // sub2+sub3
    //TCanvas* c23 = new TCanvas();
    upRange = "100.";
    detinfo = "PsdEvent.fedep_" + sPSDsub2 + "+PsdEvent.fedep_" + sPSDsub3;

    t1->Draw(detinfo +">>hPSD_sub23(2000, 0., 500.)", sPreSel, "goff");
    hPSD_base = (TH1F*) t1->GetHistogram();

    for (int ibin = 0; ibin<1000; ibin++)
    {
      if (hPSD_base->GetBinContent(ibin) != 0. && maxE_23 < hPSD_base->GetBinCenter(ibin)) maxE_23 = hPSD_base->GetBinCenter(ibin);
    }
    cout << "max PSD Edep (sub2+sub3) = " << maxE_23 << endl;

    smaxE_1 += maxE_1;
    smaxE_2 += maxE_2;
    smaxE_3 += maxE_3;
    smaxE_12 += maxE_12;
    smaxE_23 += maxE_23;

    t1->SetMakeClass(1);
    t1->SetBranchAddress("PsdEvent.fedep_" + sPSDsub1, &E_sub1, &b_E_sub1);
    t1->SetBranchAddress("PsdEvent.fedep_" + sPSDsub2, &E_sub2, &b_E_sub2);
    t1->SetBranchAddress("PsdEvent.fedep_" + sPSDsub3, &E_sub3, &b_E_sub3);
    t1->SetBranchAddress("StsEvent.fmult", &M_STS, &b_M_STS);
    t1->SetBranchAddress("McEvent.fB", &mcB, &b_mcB);

    TTree* t1_norm = new TTree("t1_norm","t1_norm");
    t1_norm->SetDirectory(0);
    t1_norm->Branch("StsEvent.fmult", &M_STS_bis, "M_STS_bis/D");
    t1_norm->Branch("PsdEvent.fedep_" + sPSDsub1, &E_sub1_bis, "E_sub1_bis/D");
    t1_norm->Branch("PsdEvent.fedep_" + sPSDsub2, &E_sub2_bis, "E_sub2_bis/D");
    t1_norm->Branch("PsdEvent.fedep_" + sPSDsub3, &E_sub3_bis, "E_sub3_bis/D");
    t1_norm->Branch("PsdEvent.fedep_sub12", &E_sub12_bis, "E_sub12_bis/D");
    t1_norm->Branch("PsdEvent.fedep_sub23", &E_sub23_bis, "E_sub23_bis/D");
    t1_norm->Branch("McEvent.fB", &mcB, "mcB/D");

    int nentries_nosel = (Int_t) t1->GetEntriesFast();
    cout << "entries (tot tree) = " << nentries_nosel << endl;

    for (int i=0;i<nentries_nosel;i++)
    {
        t1->GetEntry(i);

	if (M_STS == 0 && E_sub1+E_sub2+E_sub3 == 0) continue;    // baseline ~ sPreSel
	                                                          
	M_STS_bis = (double) M_STS;                               // data format conversion
	E_sub1_bis = (double) E_sub1;                             
	E_sub2_bis = (double) E_sub2;                             
        E_sub3_bis = (double) E_sub3;

	E_sub12_bis = E_sub1_bis+E_sub2_bis;
        E_sub23_bis = E_sub2_bis+E_sub3_bis;

	//cout << "E, Ebis: " << E_sub2 << ", " << E_sub2_bis << endl;
        //cout << "M, Mbis: " << M_STS << ", " << M_STS_bis << endl;

	M_STS_bis /= maxMult;                                      // normalization for fit functon
        E_sub1_bis /= maxE_1;
	E_sub2_bis /= maxE_2;
	E_sub3_bis /= maxE_3;

	E_sub12_bis /= maxE_12;
        E_sub23_bis /= maxE_23;

        t1_norm->Fill();
    }
    cout << "entries (selection baseline) = " << t1_norm->GetEntriesFast() << endl;

    // ============ Clean sample from "junk" events (!! caution to be taken to avoid bias in centrality estimation)

    // Cleaning of PSD E (sub1) -> ~monotoneous function of b, for PSD stand-alone at 2 - 15 AGeV. To comment otherwise.
    // at 2 AGeV: cut1 = 2., cut2 = 0.5 (Ilya), 1., 1.5 -> most "optimized" = 0.5
    // at 4 AGeV: cut1 = 15, cut2 = 3.
    // at 10 AGeV: cut1 = 40, cut2 = 15
    // at 15 AGeV: cut1 = 50, cut2 = 20
    // exception
    cutE1_cleanSub1 = 2./maxE_1;
    cutE23_cleanSub1 = 0.5/maxE_23;

    TTree* t1_norm_cleaned = new TTree("t1_norm_cleaned","t1_norm_cleaned");
    t1_norm_cleaned->SetDirectory(0);
    t1_norm_cleaned->Branch("StsEvent.fmult", &M_STS_bis, "M_STS_bis/D");
    t1_norm_cleaned->Branch("PsdEvent.fedep_" + sPSDsub1, &E_sub1_bis, "E_sub1_bis/D");
    t1_norm_cleaned->Branch("PsdEvent.fedep_" + sPSDsub2, &E_sub2_bis, "E_sub2_bis/D");
    t1_norm_cleaned->Branch("PsdEvent.fedep_" + sPSDsub3, &E_sub3_bis, "E_sub3_bis/D");
    t1_norm_cleaned->Branch("PsdEvent.fedep_sub12", &E_sub12_bis, "E_sub12_bis/D");
    t1_norm_cleaned->Branch("PsdEvent.fedep_sub23", &E_sub23_bis, "E_sub23_bis/D");
    t1_norm_cleaned->Branch("McEvent.fB", &mcB, "mcB/D");
    t1_norm_cleaned->Branch("kSTS", &kSTS, "kSTS/O");
    t1_norm_cleaned->Branch("kPSD", &kPSD, "kPSD/O");
    t1_norm_cleaned->Branch("kPSD1vsSTS", &kPSD1vsSTS, "kPSD1vsSTS/O");
    t1_norm_cleaned->Branch("kPSD2vsSTS", &kPSD2vsSTS, "kPSD2vsSTS/O");
    t1_norm_cleaned->Branch("kPSD3vsSTS", &kPSD3vsSTS, "kPSD3vsSTS/O");

    nentries_nosel = (Int_t) t1_norm->GetEntriesFast();
    for (int i=0;i<nentries_nosel;i++)
    {
        t1_norm->GetEntry(i);

        kSTS =  1;
	kPSD =  1;
	kPSD1vsSTS =  1;
	kPSD2vsSTS =  1;
	kPSD3vsSTS =  1;

	// exception: none at 35 AGeV
	if (E_sub1_bis < cutE1_cleanSub1 && E_sub23_bis < cutE23_cleanSub1)
	{
	    kPSD = 0;
            kPSD1vsSTS = 0;
            hb_rej_PSD->Fill(mcB);
	}

	t1_norm_cleaned->Fill();
    }
    entry = t1_norm_cleaned->Draw("McEvent.fB","kPSD == 1 || kPSD1vsSTS == 1","goff");
    cout << "entries (selection - t1_norm_cleaned - sub1 cleaned) = " << entry << endl;

    // ============================================== Fit function for domain definition (& additional up/down cuts)

    TString pol_order = "";
    TString smethod = "";
    TF1* ftmp;
    TF1* ftmptmp;

    for (int imeth=0;imeth<4;imeth++)
    {
        //if (imeth==0) imeth=1;                             
	smethod ="";
        smethod +=imeth;

	// == PSD
	if (imeth==0)
	{
	    TCanvas* cupdownCut_PSD = new TCanvas("c_updownCut_PSD"," up down cuts (PSD)",200,10,500,500);

	    detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":PsdEvent.fedep_" + sPSDsub1;
	    // exception: 35AGeV
	    //detinfo = "PsdEvent.fedep_" + sPSDsub1 + ":PsdEvent.fedep_" + sPSDsub2;
	    t1_norm_cleaned->Draw(detinfo+">>hPSD_updown(100, 0., 1., 100, 0., 1.)","kPSD == 1","goff");
	    hPSD_updown = (TH2F*) t1_norm_cleaned->GetHistogram();
	}

	// PSD vs STS
        if (imeth==1)  // sub1
	{
	    TCanvas* cupdownCut_PSD1vsSTS = new TCanvas("c_updownCut_PSD1vsSTS"," up down cuts (PSD sub1 vs STS)",200,10,500,500);

	    detinfo = "PsdEvent.fedep_" + sPSDsub1 + ":StsEvent.fmult";
	    t1_norm_cleaned->Draw(detinfo+">>hPSD1vsSTS_updown(100, 0., 1., 100, 0., 1.)", "kPSD1vsSTS == 1","goff");
	    hPSD1vsSTS_updown = (TH2F*) t1_norm_cleaned->GetHistogram();
	}

	if (imeth==2)  // sub2
	{
	    TCanvas* cupdownCut_PSD2vsSTS = new TCanvas("c_updownCut_PSD2vsSTS"," up down cuts (PSD sub2 vs STS)",200,10,500,500);

	    detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":StsEvent.fmult";
	    t1_norm_cleaned->Draw(detinfo+">>hPSD2vsSTS_updown(100, 0., 1., 100, 0., 1.)", "kPSD2vsSTS == 1","goff");
	    hPSD2vsSTS_updown = (TH2F*) t1_norm_cleaned->GetHistogram();
	}

	if (imeth==3)  // sub3
	{
	    TCanvas* cupdownCut_PSD3vsSTS = new TCanvas("c_updownCut_PSD3vsSTS"," up down cuts (PSD sub3 vs STS)",200,10,500,500);

	    detinfo = "PsdEvent.fedep_" + sPSDsub3 + ":StsEvent.fmult";
	    t1_norm_cleaned->Draw(detinfo+">>hPSD3vsSTS_updown(100, 0., 1., 100, 0., 1.)", "kPSD3vsSTS == 1","goff");
	    hPSD3vsSTS_updown = (TH2F*) t1_norm_cleaned->GetHistogram();
	}

	//========= fine cut
	// exception
	scale1 =2.;
	scale2 =1.;
	cut_downup = 0.2;

	if (imeth==0) hIn = hPSD_updown;
	if (imeth==1) hIn = hPSD1vsSTS_updown;
        if (imeth==2) hIn = hPSD2vsSTS_updown;
        if (imeth==3) hIn = hPSD3vsSTS_updown;

	mean = hIn->ProfileX();
	meanH = mean->ProjectionX("meanH"+smethod);

	hIn->ProjectionY("_tmp",0,1)->Draw();
	for (int i=0;i<hIn->GetNbinsX();i++)
	{
	    sl[i] = (TH1D*)hIn->ProjectionY(Form("%i",i),i,i+1);
	    sl[i]->Draw("same, goff");
	    sl[i]->SetLineColor(i+1);
	    if(sl[i]->GetEntries())
	    {
		sl[i]->Fit("gaus","0, Q");
		fit[i] = (TF1*)sl[i]->GetFunction("gaus");
		gaus->SetBinContent(i,scale1*fit[i]->GetParameter(2));
		rms->SetBinContent(i,scale1*sl[i]->GetRMS());
		gaus2->SetBinContent(i,scale2*fit[i]->GetParameter(2));
		rms2->SetBinContent(i,scale2*sl[i]->GetRMS());
	    }
	}

	hIn->Draw("colz");

        // !!!!!!!! HERE
	if (imeth!=0)
	    meanH->Draw("hist, same");
	else
	{
	    ftmp = new TF1("ftmp"+smethod,"pol9", 0., 1.);
	    meanH->Fit("ftmp"+smethod,"0, Q","",0., 1.);
	    ftmp->SetRange(0., 0.4);
	    ftmp->Draw("same");
	}

	// soft cut
	range = rms; // gaus

	cut_up = (TH1D*) meanH->Clone("cut_up");
	cut_up->Add(range,1);
	cut_down = (TH1D*) meanH->Clone("cut_down");
	cut_down->Add(range,-1);

         // !!!!!!!! HERE
	if (imeth!=0)
	{
	    cut_up->Draw("hist,same");
	    cut_up->SetLineColor(2);
	    cut_down->Draw("hist,same");
	    cut_down->SetLineColor(2);
	}
        else
	{
            ftmptmp = new TF1("ftmptmp"+smethod,"pol9", 0., 1.);
	    cut_down->Fit("ftmptmp"+smethod,"0, Q","",0., 1.);
	    ftmptmp->SetRange(0., 0.3);
	    ftmptmp->Draw("same, FC");
	}

	// stronger cuts for fit
	range = rms2; // rms2

	cut_up2 = (TH1D*) meanH->Clone("cut_up2");
	cut_up2->Add(range,1);
	cut_down2 = (TH1D*) meanH->Clone("cut_down2");
	cut_down2->Add(range,-1);

        // !!!!!!!! HERE
	if (imeth!=0)
	{
	    cut_up2->Draw("hist,same");
	    cut_up2->SetLineColor(2);
	    cut_down2->Draw("hist,same");
	    cut_down2->SetLineColor(2);

            cut_up2->SetLineStyle(2);
	    cut_down2->SetLineStyle(2);
	}

	if (imeth==1) pol_order = "pol6";
        else pol_order = "pol9";

	// up down fit functions
	cut_down->Fit(pol_order,"0, Q");  // DO pol2 ?
	cut_up->Fit(pol_order,"0, Q");
	cut_down2->Fit(pol_order,"0, Q");
	cut_up2->Fit(pol_order,"0, Q");

        // == PSD
	if (imeth==0)
	{
	    pol_down_PSD = cut_down->GetFunction(pol_order);
	    pol_up_PSD = cut_up->GetFunction(pol_order);

	    pol_down_PSD_2 = cut_down2->GetFunction(pol_order);
	    pol_up_PSD_2 = cut_up2->GetFunction(pol_order);
	}

	// == PSD vs STS
        if (imeth==1)
	{
	    pol_down_PSD1vsSTS = cut_down->GetFunction(pol_order);
	    pol_up_PSD1vsSTS = cut_up->GetFunction(pol_order);

	    pol_down_PSD1vsSTS_2 = cut_down2->GetFunction(pol_order);
	    pol_up_PSD1vsSTS_2 = cut_up2->GetFunction(pol_order);
	}
        if (imeth==2)
	{
	    pol_down_PSD2vsSTS = cut_down->GetFunction(pol_order);
	    pol_up_PSD2vsSTS = cut_up->GetFunction(pol_order);

	    pol_down_PSD2vsSTS_2 = cut_down2->GetFunction(pol_order);
	    pol_up_PSD2vsSTS_2 = cut_up2->GetFunction(pol_order);
	}
	if (imeth==3)
	{
	    pol_down_PSD3vsSTS = cut_down->GetFunction(pol_order);
	    pol_up_PSD3vsSTS = cut_up->GetFunction(pol_order);

	    pol_down_PSD3vsSTS_2 = cut_down2->GetFunction(pol_order);
	    pol_up_PSD3vsSTS_2 = cut_up2->GetFunction(pol_order);
	}
    }

    break;

    // == fine Cleaning & histos for fit functions

    TTree* t1_norm_finecleaned = new TTree("t1_norm_finecleaned","t1_norm_finecleaned");
    t1_norm_finecleaned->SetDirectory(0);
    t1_norm_finecleaned->Branch("StsEvent.fmult", &M_STS_bis, "M_STS_bis/D");
    t1_norm_finecleaned->Branch("PsdEvent.fedep_" + sPSDsub1, &E_sub1_bis, "E_sub1_bis/D");
    t1_norm_finecleaned->Branch("PsdEvent.fedep_" + sPSDsub2, &E_sub2_bis, "E_sub2_bis/D");
    t1_norm_finecleaned->Branch("PsdEvent.fedep_" + sPSDsub3, &E_sub3_bis, "E_sub3_bis/D");
    t1_norm_finecleaned->Branch("PsdEvent.fedep_sub12", &E_sub12_bis, "E_sub12_bis/D");
    t1_norm_finecleaned->Branch("PsdEvent.fedep_sub23", &E_sub23_bis, "E_sub23_bis/D");
    t1_norm_finecleaned->Branch("McEvent.fB", &mcB, "mcB/D");
    t1_norm_finecleaned->Branch("kSTS", &kSTS, "kSTS/O");
    t1_norm_finecleaned->Branch("kPSD", &kPSD, "kPSD/O");
    t1_norm_finecleaned->Branch("kPSD1vsSTS", &kPSD1vsSTS, "kPSD1vsSTS/O");
    t1_norm_finecleaned->Branch("kPSD2vsSTS", &kPSD2vsSTS, "kPSD2vsSTS/O");
    t1_norm_finecleaned->Branch("kPSD3vsSTS", &kPSD3vsSTS, "kPSD3vsSTS/O");

    //t1_norm_finecleaned->SetCacheSize(1000.);
    //t1_norm_finecleaned->SetMaxTreeSize(20000000000LL);

    nentries_nosel = (Int_t) t1_norm_cleaned->GetEntriesFast();
    for (int i=0;i<nentries_nosel;i++)
    {
        t1_norm_cleaned->GetEntry(i);
	//if (i<10) cout << "M, E: " << M_STS_bis << ", " << E_sub1_bis << endl;

        // ++++++
        // 2 agev

	// PSD
	
	if ( E_sub2_bis < pol_down_PSD->Eval(E_sub1_bis) )
        {
            kPSD = 0;
	    hb_rej_PSD->Fill(mcB);
	}

	// PSD vs STS

        if ( M_STS_bis < 0.2 && E_sub1_bis < pol_down_PSD1vsSTS->Eval(M_STS_bis) )
	{
            kPSD1vsSTS = 0;
            hb_rej_PSDvsSTS->Fill(mcB);
	}

        if ( M_STS_bis < 0.26 && E_sub2_bis > pol_up_PSD2vsSTS->Eval(M_STS_bis) )
	{
            kPSD2vsSTS = 0;
            hb_rej_PSDvsSTS->Fill(mcB);
	}

        if ( M_STS_bis < 0.35 && E_sub3_bis > pol_up_PSD3vsSTS->Eval(M_STS_bis) )
	{
            kPSD3vsSTS = 0;
            hb_rej_PSDvsSTS->Fill(mcB);
	}


        // +++++++
        // 10 agev
        /*
        // PSD

        if ( E_sub1_bis > 0.39 && (E_sub2_bis < pol_down_PSD->Eval(E_sub1_bis) || E_sub2_bis > pol_up_PSD->Eval(E_sub1_bis)) )  //  ||
	{
            kPSD = 0;
	    hb_rej_PSD->Fill(mcB);
	}

        // PSD vs STS

	if ( M_STS_bis < 0.25 && E_sub1_bis < pol_down_PSD1vsSTS->Eval(M_STS_bis) )
	{
            kPSD1vsSTS = 0;
            hb_rej_PSDvsSTS->Fill(mcB);
	}

        if ( M_STS_bis < 0.41 && (E_sub2_bis > pol_up_PSD2vsSTS->Eval(M_STS_bis) || E_sub2_bis < pol_down_PSD2vsSTS->Eval(M_STS_bis)) )
	{
            kPSD2vsSTS = 0;
            hb_rej_PSDvsSTS->Fill(mcB);
	}

        if ( M_STS_bis < 0.25 && E_sub3_bis > pol_up_PSD3vsSTS->Eval(M_STS_bis) )
	{
            kPSD3vsSTS = 0;
            hb_rej_PSDvsSTS->Fill(mcB);
	}
	*/

        // +++++++
        // 35 agev
        /*
        // PSD

	if (E_sub2_bis < 0.65 && E_sub1_bis > pol_up_PSD->Eval(E_sub2_bis))
	{
            kPSD = 0;
            hb_rej_PSD->Fill(mcB);
	}

        // PSD vs STS

	if (M_STS_bis < 0.2 && E_sub1_bis > pol_up_PSD1vsSTS->Eval(M_STS_bis))
	{
            kPSD1vsSTS = 0;
            hb_rej_PSDvsSTS->Fill(mcB);	//
	}

        if (M_STS_bis < 0.15 && E_sub2_bis > pol_up_PSD2vsSTS->Eval(M_STS_bis))
	{
            kPSD2vsSTS = 0;
            hb_rej_PSDvsSTS->Fill(mcB);	//
	}

        if (M_STS_bis < 0.15 && E_sub3_bis > pol_up_PSD3vsSTS->Eval(M_STS_bis))
	{
            kPSD3vsSTS = 0;
            hb_rej_PSDvsSTS->Fill(mcB);	//
	}
        */

        // ++++++++++++++++++++++++++++++++++++++++
        // Histos for Fit functions - stronger cuts

        if ( E_sub2_bis > pol_down_PSD_2->Eval(E_sub1_bis) && E_sub2_bis < pol_up_PSD_2->Eval(E_sub1_bis) ) hPSD->Fill(E_sub1_bis, E_sub2_bis);
	//exception: at 35
        //if ( E_sub1_bis > pol_down_PSD_2->Eval(E_sub2_bis) && E_sub1_bis < pol_up_PSD_2->Eval(E_sub2_bis) ) hPSD->Fill(E_sub2_bis, E_sub1_bis);

	if ( E_sub1_bis > pol_down_PSD1vsSTS_2->Eval(M_STS_bis) && E_sub1_bis < pol_up_PSD1vsSTS_2->Eval(M_STS_bis) )  hPSD1vsSTS->Fill(M_STS_bis, E_sub1_bis);
	if ( E_sub2_bis > pol_down_PSD2vsSTS_2->Eval(M_STS_bis) && E_sub2_bis < pol_up_PSD2vsSTS_2->Eval(M_STS_bis) )  hPSD2vsSTS->Fill(M_STS_bis, E_sub2_bis);
	if ( E_sub3_bis > pol_down_PSD3vsSTS_2->Eval(M_STS_bis) && E_sub3_bis < pol_up_PSD3vsSTS_2->Eval(M_STS_bis) )  hPSD3vsSTS->Fill(M_STS_bis, E_sub3_bis);

	t1_norm_finecleaned->Fill();
    }
    entry = t1_norm_finecleaned->Draw("McEvent.fB","kPSD == 1","goff");     
    cout << "entries (selection - t1_norm_cleaned - fine cleaned) = " << entry << endl;
    //t1_norm_finecleaned->Write();


    // Middle cuts                                                            

    // PSD
    TCanvas* c_PSD_fit = new TCanvas("fit (PSD)","fit (PSD)",200,10,500,500);

    hPSD->Draw("profile");
    hPSD->Fit("pol9");
    pol_PSD = hPSD->GetFunction("pol9");

    cut_middle_PSD = pol_PSD->GetMaximumX(0., 0.7);             // exception
    scut_middle[1] += cut_middle_PSD;

    cut_r_PSD = " || (PsdEvent.fedep_" + sPSDsub1 + " < " + scut_middle[1] + ")";      // ||
    cut_l_PSD = " && (PsdEvent.fedep_" + sPSDsub1 + " < " + scut_middle[1] + ")";
    // exception: 35 agev
    //cut_r_PSD = " && (PsdEvent.fedep_" + sPSDsub2 + " > " + scut_middle[1] + ")";      // ||
    //cut_l_PSD = " || (PsdEvent.fedep_" + sPSDsub2 + " > " + scut_middle[1] + ")";

    cout << "PSD max = " << scut_middle[1]  << endl;

    // PSD sub1 vs STS
    TCanvas* c_PSD1vsSTS_fit = new TCanvas("fit (PSD sub1 vs STS)","fit (PSD sub1 vs STS)",200,10,500,500);
    hPSD1vsSTS->Draw("profile");

    pol_PSD1vsSTS = new TF1("polfit_PSD1vsSTS","pol9", 0., 1.);
    hPSD1vsSTS->Fit("polfit_PSD1vsSTS","","", 0., 1.);

    cut_middle_PSD1vsSTS = pol_PSD1vsSTS->GetMaximumX(0., 0.7);   // exception
    scut_middle[2] += cut_middle_PSD1vsSTS;

    cut_r_PSD1vsSTS = " && (StsEvent.fmult > " + scut_middle[2] + ")";      // ||
    cut_l_PSD1vsSTS = " || (StsEvent.fmult > " + scut_middle[2] + ")";
    cout << "PSD sub1 vs STS max = " << scut_middle[2]  << endl;

    // PSD sub2 vs STS
    TCanvas* c_PSD2vsSTS_fit = new TCanvas("fit (PSD sub2 vs STS)","fit (PSD sub2 vs STS)",200,10,500,500);
    hPSD2vsSTS->Draw("profile");

    pol_PSD2vsSTS = new TF1("polfit_PSD2vsSTS","pol9", 0., 1.);
    hPSD2vsSTS->Fit("polfit_PSD2vsSTS","","", 0., 1.);

    cut_middle_PSD2vsSTS = pol_PSD2vsSTS->GetMaximumX(0., 1.);  // exception
    scut_middle[3] += cut_middle_PSD2vsSTS;

    cut_r_PSD2vsSTS = " && (StsEvent.fmult > " + scut_middle[3] + ")";      // ||
    cut_l_PSD2vsSTS = " || (StsEvent.fmult > " + scut_middle[3] + ")";
    cout << "PSD sub2 vs STS max = " << scut_middle[3]  << endl;

    // PSD sub3 vs STS
    TCanvas* c_PSD3vsSTS_fit = new TCanvas("fit (PSD sub3 vs STS)","fit (PSD sub3 vs STS)",200,10,500,500);
    hPSD3vsSTS->Draw("profile");

    pol_PSD3vsSTS = new TF1("polfit_PSD3vsSTS","pol9", 0., 1.);
    hPSD3vsSTS->Fit("polfit_PSD3vsSTS","","", 0.03, 1.);

    cut_middle_PSD3vsSTS = pol_PSD3vsSTS->GetMaximumX(0., 1.);   // exception
    scut_middle[4] += cut_middle_PSD3vsSTS;

    cut_r_PSD3vsSTS = " && (StsEvent.fmult > " + scut_middle[4] + ")";      // ||
    cut_l_PSD3vsSTS = " || (StsEvent.fmult > " + scut_middle[4] + ")";
    cout << "PSD sub3 vs STS max = " << scut_middle[4]  << endl;


    // Seach region where middle cut useless (even spurious) - HERE
    // PSD
    /*
    it_test = -10.;
    it_X = 0.;
    while ( it_test!=0. && it_X<=cut_middle_PSD )
    {
	tmp1_tmp = -1/pol_PSD->Derivative(it_X);
	tmp2_tmp = pol_PSD->Eval(it_X) - tmp1_tmp*it_X;

	// cast cuts into string
	alpha_tmp = "";
	cte_tmp = "";
	alpha_tmp += tmp1_tmp;
	cte_tmp += tmp2_tmp;

	cut = "";
        cut = "(PsdEvent.fedep_" + sPSDsub2 + " <= (" + alpha_tmp + " * PsdEvent.fedep_" + sPSDsub1 + " + " + cte_tmp + "))";
        cut += "&& (PsdEvent.fedep_" + sPSDsub1 + " > " + scut_middle[1] + ")";
        if (it_X==0.) cout << "cut = " << cut << endl;

        hname = "hPSD_middlecut";
        detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":PsdEvent.fedep_" + sPSDsub1;
	it_test = t1_norm_finecleaned->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD == 1 &&"+cut, "goff");

	it_X+=it_prec;
        //cout << "it_X = " << it_X << endl;
    }
    range_middlecut_PSD[0] = it_X;
    cout << "it_X = " << it_X << endl;

    it_test = -10.;
    it_X = 0.3;
    while ( it_test!=0. && it_X>=cut_middle_PSD )
    {
	tmp1_tmp = -1/pol_PSD->Derivative(it_X);
	tmp2_tmp = pol_PSD->Eval(it_X) - tmp1_tmp*it_X;

	// cast cuts into string
	alpha_tmp = "";
	cte_tmp = "";
	alpha_tmp += tmp1_tmp;
	cte_tmp += tmp2_tmp;

	cut = "";
        cut = "(PsdEvent.fedep_" + sPSDsub2 + " <= (" + alpha_tmp + " * PsdEvent.fedep_" + sPSDsub1 + " + " + cte_tmp + "))";
        cut += "&& (PsdEvent.fedep_" + sPSDsub1 + " < " + scut_middle[1] + ")";
        if (it_X==0.3) cout << "cut = " << cut << endl;

        hname = "hPSD_middlecut";
        detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":PsdEvent.fedep_" + sPSDsub1;
	it_test = t1_norm_finecleaned->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD == 1 &&"+cut, "goff");

	it_X-=it_prec;
        //cout << "it_X = " << it_X << endl;
    }
    range_middlecut_PSD[1] = it_X;
    cout << "it_X = " << it_X << endl;

    // PSD sub2 vs STS
    it_test = -10.;
    it_X = 0.1;
    while ( it_test!=0. && it_X<=cut_middle_PSD2vsSTS )
    {
	tmp1_tmp = -1/pol_PSD2vsSTS->Derivative(it_X);
	tmp2_tmp = pol_PSD2vsSTS->Eval(it_X) - tmp1_tmp*it_X;

	// cast cuts into string
	alpha_tmp = "";
	cte_tmp = "";
	alpha_tmp += tmp1_tmp;
	cte_tmp += tmp2_tmp;

	cut = "";
        cut = "PsdEvent.fedep_" + sPSDsub2 + " <= (" + alpha_tmp + " * StsEvent.fmult" + " + " + cte_tmp + ")";
        cut += "&& (StsEvent.fmult > " + scut_middle[3] + ")";
        if (it_X==0.) cout << "cut = " << cut << endl;

        hname = "hPSD2vsSTS_middlecut";
        detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":StsEvent.fmult";
	it_test = t1_norm_finecleaned->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD2vsSTS == 1 &&"+cut, "goff");

	it_X+=it_prec;
        //cout << "it_X = " << it_X << endl;
    }
    range_middlecut_PSD2vsSTS[0] = it_X;
    cout << "it_X = " << it_X << endl;

    it_test = -10.;
    it_X = 0.4;
    while ( it_test!=0. && it_X>=cut_middle_PSD )
    {
	tmp1_tmp = -1/pol_PSD2vsSTS->Derivative(it_X);
	tmp2_tmp = pol_PSD2vsSTS->Eval(it_X) - tmp1_tmp*it_X;

	// cast cuts into string
	alpha_tmp = "";
	cte_tmp = "";
	alpha_tmp += tmp1_tmp;
	cte_tmp += tmp2_tmp;

	cut = "";
        cut = "PsdEvent.fedep_" + sPSDsub2 + " <= (" + alpha_tmp + " * StsEvent.fmult" + " + " + cte_tmp + ")";
        cut += "&& (StsEvent.fmult < " + scut_middle[3] + ")";
        if (it_X==0.3) cout << "cut = " << cut << endl;

        hname = "hPSD2vsSTS_middlecut";
        detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":StsEvent.fmult";
	it_test = t1_norm_finecleaned->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD2vsSTS == 1 &&"+cut, "goff");

	it_X-=it_prec;
        //cout << "it_X = " << it_X << endl;
    }
    range_middlecut_PSD2vsSTS[1] = it_X;
    cout << "it_X = " << it_X << endl;
    */

    //break;

    // Save canvas
    /*
    // ===== eps
    cupdownCut_PSD->SaveAs("plot/centrality_310714/PSD_sub2vssub1_cutupdown_au10au.eps");
    cupdownCut_PSD1vsSTS->SaveAs("plot/centrality_310714/PSD1vsSTS_cutupdown_au10au.eps");
    cupdownCut_PSD2vsSTS->SaveAs("plot/centrality_310714/PSD2vsSTS_cutupdown_au10au.eps");
    cupdownCut_PSD3vsSTS->SaveAs("plot/centrality_310714/PSD3vsSTS_cutupdown_au10au.eps");

    c_PSD_fit->SaveAs("plot/centrality_310714/PSD_sub2vssub1_fitfunction_au10au.eps");
    c_PSD1vsSTS_fit->SaveAs("plot/centrality_310714/PSD1vsSTS_fitfunction_au10au.eps");
    c_PSD2vsSTS_fit->SaveAs("plot/centrality_310714/PSD2vsSTS_fitfunction_au10au.eps");
    c_PSD3vsSTS_fit->SaveAs("plot/centrality_310714/PSD3vsSTS_fitfunction_au10au.eps");

    // ===== .C
    cupdownCut_PSD->SaveAs("plot/centrality_310714/PSD_sub2vssub1_cutupdown_au10au.C");
    cupdownCut_PSD1vsSTS->SaveAs("plot/centrality_310714/PSD1vsSTS_cutupdown_au10au.C");
    cupdownCut_PSD2vsSTS->SaveAs("plot/centrality_310714/PSD2vsSTS_cutupdown_au10au.C");
    cupdownCut_PSD3vsSTS->SaveAs("plot/centrality_310714/PSD3vsSTS_cutupdown_au10au.C");

    c_PSD_fit->SaveAs("plot/centrality_310714/PSD_sub2vssub1_fitfunction_au10au.C");
    c_PSD1vsSTS_fit->SaveAs("plot/centrality_310714/PSD1vsSTS_fitfunction_au10au.C");
    c_PSD2vsSTS_fit->SaveAs("plot/centrality_310714/PSD2vsSTS_fitfunction_au10au.C");
    c_PSD3vsSTS_fit->SaveAs("plot/centrality_310714/PSD3vsSTS_fitfunction_au10au.C");

    // ===== gif
    cupdownCut_PSD->SaveAs("plot/centrality_310714/PSD_sub2vssub1_cutupdown_au10au.gif");
    cupdownCut_PSD1vsSTS->SaveAs("plot/centrality_310714/PSD1vsSTS_cutupdown_au10au.gif");
    cupdownCut_PSD2vsSTS->SaveAs("plot/centrality_310714/PSD2vsSTS_cutupdown_au10au.gif");
    cupdownCut_PSD3vsSTS->SaveAs("plot/centrality_310714/PSD3vsSTS_cutupdown_au10au.gif");

    c_PSD_fit->SaveAs("plot/centrality_310714/PSD_sub2vssub1_fitfunction_au10au.gif");
    c_PSD1vsSTS_fit->SaveAs("plot/centrality_310714/PSD1vsSTS_fitfunction_au10au.gif");
    c_PSD2vsSTS_fit->SaveAs("plot/centrality_310714/PSD2vsSTS_fitfunction_au10au.gif");
    c_PSD3vsSTS_fit->SaveAs("plot/centrality_310714/PSD3vsSTS_fitfunction_au10au.gif");
    */

    //break;


    // =======================================
    // == centrality determination, evt by evt

    //TCanvas* c_centr = new TCanvas("c_centr","centrality",200,10,500,500);
    TH1F* htest = new TH1F("htest","htest",1000,0.,1.);

    // STS
    t1_norm_finecleaned->Draw("StsEvent.fmult>>hSTS_norm(1000, 0., 1.)", "kSTS==1", "goff");
    TH1F* hSTS_norm = (TH1F*) t1_norm_finecleaned->GetHistogram();
    TAxis* Xaxis_STS = hSTS_norm->GetXaxis();


    TTree* t1_centr = new TTree("t1_centr","t1_centr");
    t1_centr->SetDirectory(0);
    t1_centr->Branch("StsEvent.fmult", &M_STS_bis, "M_STS_bis/D");
    t1_centr->Branch("PsdEvent.fedep_" + sPSDsub1, &E_sub1_bis, "E_sub1_bis/D");
    t1_centr->Branch("PsdEvent.fedep_" + sPSDsub2, &E_sub2_bis, "E_sub2_bis/D");
    t1_centr->Branch("PsdEvent.fedep_" + sPSDsub3, &E_sub3_bis, "E_sub3_bis/D");
    t1_centr->Branch("PsdEvent.fedep_sub12", &E_sub12_bis, "E_sub12_bis/D");
    t1_centr->Branch("PsdEvent.fedep_sub23", &E_sub23_bis, "E_sub23_bis/D");
    t1_centr->Branch("McEvent.fB", &mcB, "mcB/D");
    t1_centr->Branch("kSTS", &kSTS, "kSTS/O");
    t1_centr->Branch("kPSD", &kPSD, "kPSD/O");
    t1_centr->Branch("kPSD1vsSTS", &kPSD1vsSTS, "kPSD1vsSTS/O");
    t1_centr->Branch("kPSD2vsSTS", &kPSD2vsSTS, "kPSD2vsSTS/O");
    t1_centr->Branch("kPSD3vsSTS", &kPSD3vsSTS, "kPSD3vsSTS/O");
    t1_centr->Branch("c_STS", &c_STS, "c_STS/D");
    t1_centr->Branch("c_PSD", &c_PSD, "c_PSD/D");
    t1_centr->Branch("c_PSD1vsSTS", &c_PSD1vsSTS, "c_PSD1vsSTS/D");
    t1_centr->Branch("c_PSD2vsSTS", &c_PSD2vsSTS, "c_PSD2vsSTS/D");
    t1_centr->Branch("c_PSD3vsSTS", &c_PSD3vsSTS, "c_PSD3vsSTS/D");
    t1_centr->Branch("c_STS_err", &c_STS_err, "c_STS_err/D");
    t1_centr->Branch("c_PSD_err", &c_PSD_err, "c_PSD_err/D");
    t1_centr->Branch("c_PSD1vsSTS_err", &c_PSD1vsSTS_err, "c_PSD1vsSTS_err/D");
    t1_centr->Branch("c_PSD2vsSTS_err", &c_PSD2vsSTS_err, "c_PSD2vsSTS_err/D");
    t1_centr->Branch("c_PSD3vsSTS_err", &c_PSD3vsSTS_err, "c_PSD3vsSTS_err/D");

    //t1_centr->SetCacheSize(1000.);
    //t1_centr->SetMaxTreeSize(20000000000LL);

    TTree* t_cut_STS = t1_norm_finecleaned->CopyTree("kSTS == 1");
    TTree* t_cut_PSD = t1_norm_finecleaned->CopyTree("kPSD == 1");
    TTree* t_cut_PSD1vsSTS = t1_norm_finecleaned->CopyTree("kPSD1vsSTS == 1");
    TTree* t_cut_PSD2vsSTS = t1_norm_finecleaned->CopyTree("kPSD2vsSTS == 1");
    TTree* t_cut_PSD3vsSTS = t1_norm_finecleaned->CopyTree("kPSD3vsSTS == 1");

    cout << "stat for methods: STS, PSD, PSD 1/2/3 vs STS: " << t_cut_STS->GetEntriesFast() << ", " << t_cut_PSD->GetEntriesFast() << ", " << t_cut_PSD1vsSTS->GetEntriesFast() << ", " << t_cut_PSD2vsSTS->GetEntriesFast() << ", " << t_cut_PSD3vsSTS->GetEntriesFast() << endl;

    nentries_nosel = (Int_t) t1_norm_finecleaned->GetEntriesFast();
    for (int i=0;i<nentries_nosel;i++)
    {
	//if (i>0) continue; //HERE + pb fit functions for c close to 100% + harder cut sub2 vs mult + in draw: variable step for cut draw sub2 vs mult + comb with correct values once more

        c_STS =  0.;
	c_PSD =  0.;
	c_PSD1vsSTS =  0.;
	c_PSD2vsSTS =  0.;
	c_PSD3vsSTS =  0.;

        c_STS_err =  0.;
	c_PSD_err =  0.;
	c_PSD1vsSTS_err =  0.;
	c_PSD2vsSTS_err =  0.;
	c_PSD3vsSTS_err =  0.;

	tmp1_tmp=0.;
        tmp2_tmp=0.;

	//STS
        t1_norm_finecleaned->GetEntry(i);
        //if (i%10 == 0) cout << "M_STS_bis = " << M_STS_bis << ", E_sub1_bis = " << E_sub1_bis << ", E_sub2_bis = " << E_sub2_bis << ", E_sub3_bis = " << E_sub3_bis << endl;
	//cout << "X0 = " << M_STS_bis << endl;

	bin[0] = Xaxis_STS->FindBin(M_STS_bis);
        bin[1] = Xaxis_STS->GetLast();

        if (kSTS == 0) c_STS = 0.;
	else
	{
	    c_STS = 100*hSTS_norm->Integral(bin[0], bin[1])/Nentries;
	    c_STS_err = (100*hSTS_norm->Integral(bin[0], bin[1])/Nentries)*TMath::Sqrt(1./hSTS_norm->Integral(bin[0], bin[1]) + 1./Nentries);
	}

	// PSD
        t1_norm_finecleaned->GetEntry(i);
        //if (i%10 == 0) cout << "M_STS_bis = " << M_STS_bis << ", E_sub1_bis = " << E_sub1_bis << ", E_sub2_bis = " << E_sub2_bis << ", E_sub3_bis = " << E_sub3_bis << endl;

	// exception 35 AGeV 1<->2
	if (kPSD == 0) c_PSD = 0.;
	else
	{
	    X0=E_sub1_bis;
	    Y0=E_sub2_bis;
	    //exception: at 35 agev
            //X0=E_sub2_bis;
	    //Y0=E_sub1_bis;
            cutCalc(i, X0, Y0, pol_PSD, tmp1_tmp, tmp2_tmp);

	    // cast cuts into string
	    alpha_tmp = "";
	    cte_tmp = "";
	    alpha_tmp += tmp1_tmp;
	    cte_tmp += tmp2_tmp;

	    cut = "";
	    if (tmp1_tmp<0.) cut = "(PsdEvent.fedep_" + sPSDsub2 + " <= (" + alpha_tmp + " * PsdEvent.fedep_" + sPSDsub1 + " + " + cte_tmp + "))";
	    if (tmp1_tmp>0.) cut = "(PsdEvent.fedep_" + sPSDsub2 + " > (" + alpha_tmp + " * PsdEvent.fedep_" + sPSDsub1 + " + " + cte_tmp + "))";
            //exception: at 35 agev
            //if (tmp1_tmp>0.) cut = "(PsdEvent.fedep_" + sPSDsub1 + " <= (" + alpha_tmp + " * PsdEvent.fedep_" + sPSDsub2 + " + " + cte_tmp + "))";
	    //if (tmp1_tmp<0.) cut = "(PsdEvent.fedep_" + sPSDsub1 + " > (" + alpha_tmp + " * PsdEvent.fedep_" + sPSDsub2 + " + " + cte_tmp + "))";

	    if (X0 < cut_middle_PSD ) cut_rl = cut_l_PSD;
	    else cut_rl = cut_r_PSD;
	    cut += cut_rl;
	    //cout << "cut = " << cut_rl << endl;
            //cout << "X0 = " << X0 << endl;

	    // === histo
	    hname = "htmp_PSD";
	    //hname += i+1;
	    detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":PsdEvent.fedep_" + sPSDsub1;
	    //exception: at 35 agev
            //detinfo = "PsdEvent.fedep_" + sPSDsub1 + ":PsdEvent.fedep_" + sPSDsub2;

	    //t1_norm_finecleaned->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD == 1 &&"+cut, "goff");  //
	    //hPSD_tmp = (TH1F*) t1_norm_finecleaned->GetHistogram();

            t_cut_PSD->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", cut, "goff");  //
            hPSD_tmp = (TH1F*) t_cut_PSD->GetHistogram();

	    c_PSD = 100*hPSD_tmp->Integral()/Nentries;
	    c_PSD_err = (100*hPSD_tmp->Integral()/Nentries)*TMath::Sqrt(1./hPSD_tmp->Integral() + 1./Nentries);
	}
        //E_sub1_bis = X0;

        //continue;

        // PSD sub1 vs STS
        t1_norm_finecleaned->GetEntry(i);
        //if (i%10 == 0) cout << "M_STS_bis = " << M_STS_bis << ", E_sub1_bis = " << E_sub1_bis << ", E_sub2_bis = " << E_sub2_bis << ", E_sub3_bis = " << E_sub3_bis << endl;

	if (kPSD1vsSTS == 0) c_PSD1vsSTS = 0.;
	else
	{
            X0=M_STS_bis;
            //cout << "X0 = " << X0 << endl;
	    Y0=E_sub1_bis;
            cutCalc(i, X0, Y0, pol_PSD1vsSTS, tmp1_tmp, tmp2_tmp);
            //cutCalc(i, M_STS_bis, E_sub1_bis, pol_PSD1vsSTS, tmp1_tmp, tmp2_tmp);
	    //X0 = M_STS_bis;

	    // cast cuts into string
	    alpha_tmp = "";
	    cte_tmp = "";
	    alpha_tmp += tmp1_tmp;
	    cte_tmp += tmp2_tmp;

	    cut = "";
	    if (tmp1_tmp>0.) cut = "(PsdEvent.fedep_" + sPSDsub1 + " <= (" + alpha_tmp + " * StsEvent.fmult + " + cte_tmp + "))";
	    if (tmp1_tmp<0.) cut = "(PsdEvent.fedep_" + sPSDsub1 + " > (" + alpha_tmp + " * StsEvent.fmult + " + cte_tmp + "))";

	    if (X0 < cut_middle_PSD1vsSTS ) cut_rl = cut_l_PSD1vsSTS;
	    else cut_rl = cut_r_PSD1vsSTS;
	    cut += cut_rl;
	    //cout << "cut = " << cut_rl << endl;
            //cout << "X0 = " << X0 << endl;

	    // === histo
	    hname = "htmp_PSD1vsSTS";
	    //hname += i+1;
	    detinfo = "PsdEvent.fedep_" + sPSDsub1 + ":StsEvent.fmult";

	    //t1_norm_finecleaned->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD1vsSTS == 1 &&"+cut, "goff");  //
            //hPSDvsSTS_tmp = (TH1F*) t1_norm_finecleaned->GetHistogram();

	    t_cut_PSD1vsSTS->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", cut, "goff");  //
	    hPSDvsSTS_tmp = (TH1F*) t_cut_PSD1vsSTS->GetHistogram();

	    c_PSD1vsSTS = 100*hPSDvsSTS_tmp->Integral()/Nentries;
            c_PSD1vsSTS_err = (100*hPSDvsSTS_tmp->Integral()/Nentries)*TMath::Sqrt(1./hPSDvsSTS_tmp->Integral() + 1./Nentries);
	}
        //M_STS_bis = X0;

        // PSD sub2 vs STS
        t1_norm_finecleaned->GetEntry(i);
        //if (i%10 == 0) cout << "M_STS_bis = " << M_STS_bis << ", E_sub1_bis = " << E_sub1_bis << ", E_sub2_bis = " << E_sub2_bis << ", E_sub3_bis = " << E_sub3_bis << endl;

	if (kPSD2vsSTS == 0) c_PSD2vsSTS = 0.;
	else
	{
            X0=M_STS_bis;
            //cout << "X0 = " << X0 << endl;
	    Y0=E_sub2_bis;
            cutCalc(i, X0, Y0, pol_PSD2vsSTS, tmp1_tmp, tmp2_tmp);
            //cutCalc(i, M_STS_bis, E_sub2_bis, pol_PSD2vsSTS, tmp1_tmp, tmp2_tmp);
	    //X0 = M_STS_bis;

	    // cast cuts into string
	    alpha_tmp = "";
	    cte_tmp = "";
	    alpha_tmp += tmp1_tmp;
	    cte_tmp += tmp2_tmp;

	    cut = "";
	    if (tmp1_tmp>0.) cut = "(PsdEvent.fedep_" + sPSDsub2 + " <= (" + alpha_tmp + " * StsEvent.fmult + " + cte_tmp + "))";
	    if (tmp1_tmp<0.) cut = "(PsdEvent.fedep_" + sPSDsub2 + " > (" + alpha_tmp + " * StsEvent.fmult + " + cte_tmp + "))";

	    if (X0 < cut_middle_PSD2vsSTS ) cut_rl = cut_l_PSD2vsSTS;
	    else cut_rl = cut_r_PSD2vsSTS;
	    cut += cut_rl;
	    //cout << "cut = " << cut_rl << endl;
            //cout << "X0 = " << X0 << endl;

	    // === histo
	    hname = "htmp_PSD2vsSTS";
	    //hname += i+1;
	    detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":StsEvent.fmult";

	    //t1_norm_finecleaned->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD2vsSTS == 1 &&"+cut, "goff");  //
	    //hPSDvsSTS_tmp = (TH1F*) t1_norm_finecleaned->GetHistogram();

	    t_cut_PSD2vsSTS->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", cut, "goff");  //
	    hPSDvsSTS_tmp = (TH1F*) t_cut_PSD2vsSTS->GetHistogram();

	    c_PSD2vsSTS = 100*hPSDvsSTS_tmp->Integral()/Nentries;
            c_PSD2vsSTS_err = (100*hPSDvsSTS_tmp->Integral()/Nentries)*TMath::Sqrt(1./hPSDvsSTS_tmp->Integral() + 1./Nentries);
	}
        //M_STS_bis = X0;
	//E_sub2_bis = Y0;

        // PSD sub3 vs STS
        t1_norm_finecleaned->GetEntry(i);
        //if (i%10 == 0) cout << "M_STS_bis = " << M_STS_bis << ", E_sub1_bis = " << E_sub1_bis << ", E_sub2_bis = " << E_sub2_bis << ", E_sub3_bis = " << E_sub3_bis << endl;

	if (kPSD3vsSTS == 0) c_PSD3vsSTS = 0.;
	else
	{
            X0=M_STS_bis;
            //cout << "X0 = " << X0 << endl;
	    Y0=E_sub3_bis;
            cutCalc(i, X0, Y0, pol_PSD3vsSTS, tmp1_tmp, tmp2_tmp);
            //cutCalc(i, M_STS_bis, E_sub3_bis, pol_PSD3vsSTS, tmp1_tmp, tmp2_tmp);
	    //X0 = M_STS_bis;

	    // cast cuts into string
	    alpha_tmp = "";
	    cte_tmp = "";
	    alpha_tmp += tmp1_tmp;
	    cte_tmp += tmp2_tmp;

	    cut = "";
	    if (tmp1_tmp>0.) cut = "(PsdEvent.fedep_" + sPSDsub3 + " <= (" + alpha_tmp + " * StsEvent.fmult + " + cte_tmp + "))";
	    if (tmp1_tmp<0.) cut = "(PsdEvent.fedep_" + sPSDsub3 + " > (" + alpha_tmp + " * StsEvent.fmult + " + cte_tmp + "))";

	    if (X0 < cut_middle_PSD3vsSTS ) cut_rl = cut_l_PSD3vsSTS;
	    else cut_rl = cut_r_PSD3vsSTS;
	    cut += cut_rl;
	    //cout << "cut = " << cut_rl << endl;
            //cout << "X0 = " << X0 << endl;

	    // === histo
	    hname = "htmp_PSD3vsSTS";
	    //hname += i+1;
	    detinfo = "PsdEvent.fedep_" + sPSDsub3 + ":StsEvent.fmult";

	    //t1_norm_finecleaned->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD3vsSTS == 1 &&"+cut, "goff");  //
	    //hPSDvsSTS_tmp = (TH1F*) t1_norm_finecleaned->GetHistogram();

            t_cut_PSD3vsSTS->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", cut, "goff");  //
	    hPSDvsSTS_tmp = (TH1F*) t_cut_PSD3vsSTS->GetHistogram();

	    c_PSD3vsSTS = 100*hPSDvsSTS_tmp->Integral()/Nentries;
            c_PSD3vsSTS_err = (100*hPSDvsSTS_tmp->Integral()/Nentries)*TMath::Sqrt(1./hPSDvsSTS_tmp->Integral() + 1./Nentries);
	}
        //M_STS_bis = X0;
	//E_sub3_bis = Y0;
	//htest->Fill(E_sub1_bis);

        t1_norm_finecleaned->GetEntry(i);
        //if (i%10 == 0) cout << "M_STS_bis = " << M_STS_bis << ", E_sub1_bis = " << E_sub1_bis << ", E_sub2_bis = " << E_sub2_bis << ", E_sub3_bis = " << E_sub3_bis << endl;
	if (i%100 == 0)  cout << "c_STS = " << c_STS<<" +/- "<<c_STS_err << ", c_PSD = " << c_PSD<<" +/- "<<c_PSD_err <<", c_PSD1vsSTS = " << c_PSD1vsSTS<<" +/- "<<c_PSD1vsSTS_err << ", c_PSD2vsSTS = " << c_PSD2vsSTS<<" +/- "<<c_PSD2vsSTS_err << ", c_PSD3vsSTS = " << c_PSD3vsSTS<<" +/- "<<c_PSD3vsSTS_err << endl;
	//if (i%10 == 0)
	//cout << "===========" << endl;
        //cout << "===========" << endl;

	t1_centr->Fill();
    }

    //t1_centr->Write();
}

void cutCalc(int iloop, double X0in, double Y0in, TF1* polin, double &tmp1_tmp, double &tmp2_tmp)
{
    bool verbose = 0;

    double X0 = X0in;
    double Y0 = Y0in;

    TString fname = "";
    TString hname = "";

    hname = "h2D";
    hname += iloop;
    TH2F* h2D = new TH2F(hname,"2D correlation", 1000, 0., 1., 1000, 0., 1.);

    hname = "hdistance";
    hname += iloop;
    TH1F* hdistance = new TH1F(hname,"distance", 1000, 0., 1.);
    TAxis* Xaxis = hdistance->GetXaxis();

    double dist=0.;
    double subrange1 = 0.;
    double subrange2 = 0.;

    double X1=0.;
    double Y1=0.;

    for (int j=Xaxis->GetFirst(); j<=Xaxis->GetLast();j++)
    {
	dist = TMath::Sqrt( TMath::Power(polin->Eval(Xaxis->GetBinCenter(j)) - Y0,2) + TMath::Power(Xaxis->GetBinCenter(j) - X0,2) );
	hdistance->Fill(Xaxis->GetBinCenter(j), dist);
    }
    //hdistance->Draw();

    X1 = Xaxis->GetBinCenter(hdistance->GetMinimumBin());
    if (verbose==1) cout << "(X0, X1) = (" << X0 << ", " << X1 << ")" << endl;

    subrange1 = (X0+X1)/2. - 1.5*TMath::Abs(X1-X0);
    subrange2 = (X0+X1)/2. + 1.5*TMath::Abs(X1-X0);

    if (subrange1<0.) subrange1=0.;
    if (subrange2>1.) subrange2=1.;

    hdistance->Delete();
    hname = "hdistance_zoom";
    hname += iloop;
    hdistance = new TH1F(hname,"distance", 1000, subrange1, subrange2);    // play on Nbin>10000 ??
    Xaxis = hdistance->GetXaxis();

    for (int j=Xaxis->GetFirst(); j<=Xaxis->GetLast();j++)
    {
	dist = TMath::Sqrt( TMath::Power(polin->Eval(Xaxis->GetBinCenter(j)) - Y0,2) + TMath::Power(Xaxis->GetBinCenter(j) - X0,2) );
	hdistance->Fill(Xaxis->GetBinCenter(j), dist);
    }
    //hdistance->Draw();

    X1 = Xaxis->GetBinCenter(hdistance->GetMinimumBin());
    Y1 = polin->Eval(X1);

    //if (verbose==1)
    //if (iloop%10 == 0)
    //cout << "(X0, Y0) = (" << X0 << ", " << Y0 << "), (X1, Y1) = (" << X1 << ", " << Y1 << ")" << endl;

    // == use of fit functions
    if (verbose==1)
    {
	h2D->Fill(X0, Y0);
	h2D->SetMarkerStyle(20);
	if (iloop==0) h2D->Draw();
	else  h2D->Draw("same");
	if (iloop==0) polin->Draw("same");

	fname = "fcut_";
	fname += iloop;
	TF1* fcut = new TF1(fname,"[0]*x+[1]", 0., 1.);
	fname = "fcut2_";
	fname += iloop;
	TF1* fcut2 = new TF1(fname,"[0]*x+[1]", 0., 1.);
    }

    // simple 2 points method
    tmp1_tmp = (Y1 - Y0)/(X1 - X0);
    tmp2_tmp = Y0 - tmp1_tmp*X0;

    if (verbose==1)
    {
	fcut->SetParameter(0, tmp1_tmp);
	fcut->SetParameter(1, tmp2_tmp);
	fcut->Draw("same");
	fcut->SetLineColor(kBlue);
    }

    // normal to tangent of fit fucntion method
    tmp1_tmp = -1/polin->Derivative(X1);
    tmp2_tmp = polin->Eval(X1) - tmp1_tmp*X1;

    if (verbose==1)
    {
	fcut2->SetParameter(0, tmp1_tmp);
	fcut2->SetParameter(1, tmp2_tmp);
	fcut2->Draw("same");
	fcut2->SetLineColor(kRed);
    }

    h2D->Delete();
    hdistance->Delete();
}