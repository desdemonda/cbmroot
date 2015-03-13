// -------------------------------------------------------------------------
// ----- This task draws the centrality resolution versus collision centrality 

centrality_drawReso_STS_PSD()
{
    // show c_STS for kPSD==0 -> biais for centrality, same for kPSD1vsSTS etc
    // use E_PSD distr. w "" and w "kPSD==1" -> norm -> efficiency (1D)
    // show fB w "" and w "PSD==0" and w "PSD==1"

    TFile *f1 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au2au/sts_13d_PSDhole6cm_44mods_beampipe/v15/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8_centrality.root","R");
    TTree* t1_centr = (TTree*) f1->Get("t1_centr");

    //TString outName = "/hera/cbm/users/sseddiki/auau_TDR/shield/au2au/sts_13d_PSDhole6cm_44mods_beampipe/v15/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8_centrality_det_comb.root";
    //cout << "output: " << outName << endl;
    //TFile *outFile = new TFile(outName, "RECREATE");

    // variable declaration

    Float_t mean0[20], emean0[20];
    Float_t sigma0[20], esigma0[20];

    Float_t mean1[20], emean1[20];
    Float_t sigma1[20], esigma1[20];

    Float_t mean2[20], emean2[20];
    Float_t sigma2[20], esigma2[20];

    Float_t mean3[20], emean3[20];
    Float_t sigma3[20], esigma3[20];

    Float_t mean4[20], emean4[20];
    Float_t sigma4[20], esigma4[20];

    Float_t mean5[20], emean5[20];
    Float_t sigma5[20], esigma5[20];

    Float_t mean6[20], emean6[20];
    Float_t sigma6[20], esigma6[20];

    // combine all - iterations 1, 2, 3
    Float_t mean7[20], emean7[20];
    Float_t sigma7[20], esigma7[20];

    Float_t mean8[20], emean8[20];
    Float_t sigma8[20], esigma8[20];

    Float_t mean9[20], emean9[20];
    Float_t sigma9[20], esigma9[20];

    Float_t sigmaovermean0[20], esigmaovermean0[20];
    Float_t sigmaovermean1[20], esigmaovermean1[20];
    Float_t sigmaovermean2[20], esigmaovermean2[20];
    Float_t sigmaovermean3[20], esigmaovermean3[20];
    Float_t sigmaovermean4[20], esigmaovermean4[20];
    Float_t sigmaovermean5[20], esigmaovermean5[20];
    Float_t sigmaovermean6[20], esigmaovermean6[20];
    // combine all - iterations 1, 2, 3
    Float_t sigmaovermean7[20], esigmaovermean7[20];
    Float_t sigmaovermean8[20], esigmaovermean8[20];
    Float_t sigmaovermean9[20], esigmaovermean9[20];

    // ci - c_all dispersion
    Float_t corrDisp[5][20];
    for (int i=0;i<20;i++) for (int j=0;j<5;j++) corrDisp[j][i] = 0.;

    double inc_evt_STS[20];
    double inc_evt_err_STS[20];
    float percent_STS[20];
    float percent_err_STS[20];

    double inc_evt_PSD[20];
    double inc_evt_err_PSD[20];
    float percent_PSD[20];
    float percent_err_PSD[20];

    double inc_evt_PSD1vsSTS[20];
    double inc_evt_err_PSD1vsSTS[20];
    float percent_PSD1vsSTS[20];
    float percent_err_PSD1vsSTS[20];

    double inc_evt_PSD2vsSTS[20];
    double inc_evt_err_PSD2vsSTS[20];
    float percent_PSD2vsSTS[20];
    float percent_err_PSD2vsSTS[20];

    double inc_evt_PSD3vsSTS[20];
    double inc_evt_err_PSD3vsSTS[20];
    float percent_PSD3vsSTS[20];
    float percent_err_PSD3vsSTS[20];

    double inc_evt_comb_meth1_2[20];
    double inc_evt_err_comb_meth1_2[20];
    float percent_comb_meth1_2[20];
    float percent_err_comb_meth1_2[20];

    double inc_evt_comb_meth3_4_5[20];
    double inc_evt_err_comb_meth3_4_5[20];
    float percent_comb_meth3_4_5[20];
    float percent_err_comb_meth3_4_5[20];

    // combine all - iterations 1, 2, 3
    double inc_evt_comb_methall_it0[20];
    double inc_evt_err_comb_methall_it0[20];
    float percent_comb_methall_it0[20];
    float percent_err_comb_methall_it0[20];

    double inc_evt_comb_methall_it1[20];
    double inc_evt_err_comb_methall_it1[20];
    float percent_comb_methall_it1[20];
    float percent_err_comb_methall_it1[20];

    double inc_evt_comb_methall_it2[20];
    double inc_evt_err_comb_methall_it2[20];
    float percent_comb_methall_it2[20];
    float percent_err_comb_methall_it2[20];

    TString sMult[20];

    for (int i=0;i<20;i++)
    {
        sMult[i] = "";

	mean0[i] = emean0[i] = 0.;
	sigma0[i] = esigma0[i] = 0.;

	mean1[i] = emean1[i] = 0.;
	sigma1[i] = esigma1[i] = 0.;

        mean2[i] = emean2[i] = 0.;
	sigma2[i] = esigma2[i] = 0.;

        mean3[i] = emean3[i] = 0.;
	sigma3[i] = esigma3[i] = 0.;

        mean4[i] = emean4[i] = 0.;
	sigma4[i] = esigma4[i] = 0.;

        mean5[i] = emean5[i] = 0.;
	sigma5[i] = esigma5[i] = 0.;

        mean6[i] = emean6[i] = 0.;
	sigma6[i] = esigma6[i] = 0.;

        // combine all - iterations 1, 2, 3
        mean7[i] = emean7[i] = 0.;
	sigma7[i] = esigma7[i] = 0.;

        mean8[i] = emean8[i] = 0.;
	sigma8[i] = esigma8[i] = 0.;

        mean9[i] = emean9[i] = 0.;
	sigma9[i] = esigma9[i] = 0.;

        //
	sigmaovermean0[i] = esigmaovermean0[i] = 0.;
	sigmaovermean1[i] = esigmaovermean1[i] = 0.;
        sigmaovermean2[i] = esigmaovermean2[i] = 0.;
        sigmaovermean3[i] = esigmaovermean3[i] = 0.;
	sigmaovermean4[i] = esigmaovermean4[i] = 0.;
	sigmaovermean5[i] = esigmaovermean5[i] = 0.;
	sigmaovermean6[i] = esigmaovermean6[i] = 0.;

        // combine all - iterations 1, 2, 3
	sigmaovermean7[i] = esigmaovermean7[i] = 0.;
        sigmaovermean8[i] = esigmaovermean8[i] = 0.;
	sigmaovermean9[i] = esigmaovermean9[i] = 0.;

	inc_evt_STS[i] = 0.;
	inc_evt_err_STS[i] = 0.;
	percent_STS[i] = 0.;
	percent_err_STS[i] = 0.;

	inc_evt_PSD[i] = 0.;
	inc_evt_err_PSD[i] = 0.;
	percent_PSD[i] = 0.;
	percent_err_PSD[i] = 0.;

        inc_evt_PSD1vsSTS[i] = 0.;
	inc_evt_err_PSD1vsSTS[i] = 0.;
	percent_PSD1vsSTS[i] = 0.;
	percent_err_PSD1vsSTS[i] = 0.;

        inc_evt_PSD2vsSTS[i] = 0.;
	inc_evt_err_PSD2vsSTS[i] = 0.;
	percent_PSD2vsSTS[i] = 0.;
	percent_err_PSD2vsSTS[i] = 0.;

        inc_evt_PSD3vsSTS[i] = 0.;
	inc_evt_err_PSD3vsSTS[i] = 0.;
	percent_PSD3vsSTS[i] = 0.;
	percent_err_PSD3vsSTS[i] = 0.;

        inc_evt_comb_meth1_2[i] = 0.;
	inc_evt_err_comb_meth1_2[i] = 0.;
	percent_comb_meth1_2[i] = 0.;
	percent_err_comb_meth1_2[i] = 0.;

        inc_evt_comb_meth3_4_5[i] = 0.;
	inc_evt_err_comb_meth3_4_5[i] = 0.;
	percent_comb_meth3_4_5[i] = 0.;
	percent_err_comb_meth3_4_5[i] = 0.;

        // combine all - iterations 1, 2, 3
        inc_evt_comb_methall_it0[i] = 0.;
	inc_evt_err_comb_methall_it0[i] = 0.;
	percent_comb_methall_it0[i] = 0.;
	percent_err_comb_methall_it0[i] = 0.;

        inc_evt_comb_methall_it1[i] = 0.;
	inc_evt_err_comb_methall_it1[i] = 0.;
	percent_comb_methall_it1[i] = 0.;
	percent_err_comb_methall_it1[i] = 0.;

        inc_evt_comb_methall_it2[i] = 0.;
	inc_evt_err_comb_methall_it2[i] = 0.;
	percent_comb_methall_it2[i] = 0.;
	percent_err_comb_methall_it2[i] = 0.;
    }

    Double_t b[20];
    for (int i=0;i<20;i++) b[i] = 0.;
    int bin[20];
    for (int i=0;i<20;i++) bin[i] = 0;

    double Nstep = 20.;
    double step = 100./Nstep;
    cout << "step = " << step << "%" << endl;
    int dom_cut = 20;
    double delta_c = 5.;

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
    double LcteX_1;
    double LcteX_2;

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

    TH1F* hSTS_tmp;
    TH1F* hSTS_b;
    TH1F* hSTS_c;

    TH1F* hPSD_tmp;
    TH1F* hPSD_b;
    TH1F* hPSD_c;

    TH1F* hPSDvsSTS_tmp;
    TH1F* hPSDvsSTS_b;
    TH1F* hPSDvsSTS_c;

    TString index1 = "";
    TString index2 = "";
    TString sstep = "";
    sstep += step;
    TString cut = "";
    TString hname = "";

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
    TBranch *b_c_STS;
    TBranch *b_c_PSD;
    TBranch *b_c_PSD1vsSTS;
    TBranch *b_c_PSD2vsSTS;
    TBranch *b_c_PSD3vsSTS;
    TBranch *b_c_STS_err;
    TBranch *b_c_PSD_err;
    TBranch *b_c_PSD1vsSTS_err;
    TBranch *b_c_PSD2vsSTS_err;
    TBranch *b_c_PSD3vsSTS_err;

    double mcB;
    double M_STS_bis;
    double E_sub1_bis;
    double E_sub2_bis;
    double E_sub3_bis;
    double E_sub12_bis;
    double E_sub23_bis;

    bool kSTS;
    bool kPSD;
    bool kPSD1vsSTS;
    bool kPSD2vsSTS;
    bool kPSD3vsSTS;
                            // Methods
    double c_STS;           // 1
    double c_PSD;           // 2
    double c_PSD1vsSTS;     // 3
    double c_PSD2vsSTS;     // 4
    double c_PSD3vsSTS;     // 5

    double c_comb_meth1_2;
    double c_comb_meth3_4_5;
    double c_comb_methall;

    double c_STS_err;
    double c_PSD_err;
    double c_PSD1vsSTS_err;
    double c_PSD2vsSTS_err;
    double c_PSD3vsSTS_err;

    double factor_reso[5];
    for (i=0;i<5;i++) factor_reso[i] = 0.;

    // == Combined resolutions

    t1_centr->SetMakeClass(1);
    t1_centr->SetBranchAddress("StsEvent.fmult", &M_STS_bis, &b_M_STS);
    t1_centr->SetBranchAddress("PsdEvent.fedep_" + sPSDsub1, &E_sub1_bis, &b_E_sub1);
    t1_centr->SetBranchAddress("PsdEvent.fedep_" + sPSDsub2, &E_sub2_bis, &b_E_sub2);
    t1_centr->SetBranchAddress("PsdEvent.fedep_" + sPSDsub3, &E_sub3_bis, &b_E_sub3);
    t1_centr->SetBranchAddress("PsdEvent.fedep_sub12", &E_sub12_bis, &b_E_sub12);
    t1_centr->SetBranchAddress("PsdEvent.fedep_sub23", &E_sub23_bis, &b_E_sub23);
    t1_centr->SetBranchAddress("McEvent.fB", &mcB, &b_mcB);
    t1_centr->SetBranchAddress("kSTS", &kSTS, &b_STS);
    t1_centr->SetBranchAddress("kPSD", &kPSD, &b_PSD);
    t1_centr->SetBranchAddress("kPSD1vsSTS", &kPSD1vsSTS, &b_PSD1vsSTS);
    t1_centr->SetBranchAddress("kPSD2vsSTS", &kPSD2vsSTS, &b_PSD2vsSTS);
    t1_centr->SetBranchAddress("kPSD3vsSTS", &kPSD3vsSTS, &b_PSD3vsSTS);
    t1_centr->SetBranchAddress("c_STS", &c_STS, &b_c_STS);
    t1_centr->SetBranchAddress("c_PSD", &c_PSD, &b_c_PSD);
    t1_centr->SetBranchAddress("c_PSD1vsSTS", &c_PSD1vsSTS, &b_c_PSD1vsSTS);
    t1_centr->SetBranchAddress("c_PSD2vsSTS", &c_PSD2vsSTS, &b_c_PSD2vsSTS);
    t1_centr->SetBranchAddress("c_PSD3vsSTS", &c_PSD3vsSTS, &b_c_PSD3vsSTS);
    t1_centr->SetBranchAddress("c_STS_err", &c_STS_err, &b_c_STS_err);
    t1_centr->SetBranchAddress("c_PSD_err", &c_PSD_err, &b_c_PSD_err);
    t1_centr->SetBranchAddress("c_PSD1vsSTS_err", &c_PSD1vsSTS_err, &b_c_PSD1vsSTS_err);
    t1_centr->SetBranchAddress("c_PSD2vsSTS_err", &c_PSD2vsSTS_err, &b_c_PSD2vsSTS_err);
    t1_centr->SetBranchAddress("c_PSD3vsSTS_err", &c_PSD3vsSTS_err, &b_c_PSD3vsSTS_err);

    TTree* t2_centr = new TTree("t2_centr","t2_centr");
    t2_centr->SetDirectory(0);
    t2_centr->Branch("StsEvent.fmult", &M_STS_bis, "M_STS_bis/D");
    t2_centr->Branch("PsdEvent.fedep_" + sPSDsub1, &E_sub1_bis, "E_sub1_bis/D");
    t2_centr->Branch("PsdEvent.fedep_" + sPSDsub2, &E_sub2_bis, "E_sub2_bis/D");
    t2_centr->Branch("PsdEvent.fedep_" + sPSDsub3, &E_sub3_bis, "E_sub3_bis/D");
    t2_centr->Branch("PsdEvent.fedep_sub12", &E_sub12_bis, "E_sub12_bis/D");
    t2_centr->Branch("PsdEvent.fedep_sub23", &E_sub23_bis, "E_sub23_bis/D");
    t2_centr->Branch("McEvent.fB", &mcB, "mcB/D");
    t2_centr->Branch("kSTS", &kSTS, "kSTS/O");
    t2_centr->Branch("kPSD", &kPSD, "kPSD/O");
    t2_centr->Branch("kPSD1vsSTS", &kPSD1vsSTS, "kPSD1vsSTS/O");
    t2_centr->Branch("kPSD2vsSTS", &kPSD2vsSTS, "kPSD2vsSTS/O");
    t2_centr->Branch("kPSD3vsSTS", &kPSD3vsSTS, "kPSD3vsSTS/O");
    t2_centr->Branch("c_STS", &c_STS, "c_STS/D");
    t2_centr->Branch("c_PSD", &c_PSD, "c_PSD/D");
    t2_centr->Branch("c_PSD1vsSTS", &c_PSD1vsSTS, "c_PSD1vsSTS/D");
    t2_centr->Branch("c_PSD2vsSTS", &c_PSD2vsSTS, "c_PSD2vsSTS/D");
    t2_centr->Branch("c_PSD3vsSTS", &c_PSD3vsSTS, "c_PSD3vsSTS/D");
    t2_centr->Branch("c_STS_err", &c_STS_err, "c_STS_err/D");
    t2_centr->Branch("c_PSD_err", &c_PSD_err, "c_PSD_err/D");
    t2_centr->Branch("c_PSD1vsSTS_err", &c_PSD1vsSTS_err, "c_PSD1vsSTS_err/D");
    t2_centr->Branch("c_PSD2vsSTS_err", &c_PSD2vsSTS_err, "c_PSD2vsSTS_err/D");
    t2_centr->Branch("c_PSD3vsSTS_err", &c_PSD3vsSTS_err, "c_PSD3vsSTS_err/D");
    // detector info. combination
    t2_centr->Branch("c_comb_meth1_2", &c_comb_meth1_2, "c_comb_meth1_2/D");
    t2_centr->Branch("c_comb_meth3_4_5", &c_comb_meth3_4_5, "c_comb_meth3_4_5/D");
    t2_centr->Branch("c_comb_methall", &c_comb_methall, "c_comb_methall/D");

    int nentries_nosel = (Int_t) t1_centr->GetEntriesFast();
    for (int i=0;i<nentries_nosel;i++)
    {
	t1_centr->GetEntry(i);

	c_comb_meth1_2 = (c_STS+c_PSD)/2.;
        c_comb_meth3_4_5 = (c_PSD1vsSTS+c_PSD2vsSTS+c_PSD3vsSTS)/3.;
        c_comb_methall = (c_STS+c_PSD+c_PSD1vsSTS+c_PSD2vsSTS+c_PSD3vsSTS)/5.;

        t2_centr->Fill();
    }

    //t2_centr->Write();

    /*
    // Refine resolution - iteration 1
    TTree* t3_centr = new TTree("t3_centr","t3_centr");
    t3_centr->SetDirectory(0);
    t3_centr->Branch("StsEvent.fmult", &M_STS_bis, "M_STS_bis/D");
    t3_centr->Branch("PsdEvent.fedep_" + sPSDsub1, &E_sub1_bis, "E_sub1_bis/D");
    t3_centr->Branch("PsdEvent.fedep_" + sPSDsub2, &E_sub2_bis, "E_sub2_bis/D");
    t3_centr->Branch("PsdEvent.fedep_" + sPSDsub3, &E_sub3_bis, "E_sub3_bis/D");
    t3_centr->Branch("PsdEvent.fedep_sub12", &E_sub12_bis, "E_sub12_bis/D");
    t3_centr->Branch("PsdEvent.fedep_sub23", &E_sub23_bis, "E_sub23_bis/D");
    t3_centr->Branch("McEvent.fB", &mcB, "mcB/D");
    t3_centr->Branch("kSTS", &kSTS, "kSTS/O");
    t3_centr->Branch("kPSD", &kPSD, "kPSD/O");
    t3_centr->Branch("kPSD1vsSTS", &kPSD1vsSTS, "kPSD1vsSTS/O");
    t3_centr->Branch("kPSD2vsSTS", &kPSD2vsSTS, "kPSD2vsSTS/O");
    t3_centr->Branch("kPSD3vsSTS", &kPSD3vsSTS, "kPSD3vsSTS/O");
    t3_centr->Branch("c_STS", &c_STS, "c_STS/D");
    t3_centr->Branch("c_PSD", &c_PSD, "c_PSD/D");
    t3_centr->Branch("c_PSD1vsSTS", &c_PSD1vsSTS, "c_PSD1vsSTS/D");
    t3_centr->Branch("c_PSD2vsSTS", &c_PSD2vsSTS, "c_PSD2vsSTS/D");
    t3_centr->Branch("c_PSD3vsSTS", &c_PSD3vsSTS, "c_PSD3vsSTS/D");
    t3_centr->Branch("c_STS_err", &c_STS_err, "c_STS_err/D");
    t3_centr->Branch("c_PSD_err", &c_PSD_err, "c_PSD_err/D");
    t3_centr->Branch("c_PSD1vsSTS_err", &c_PSD1vsSTS_err, "c_PSD1vsSTS_err/D");
    t3_centr->Branch("c_PSD2vsSTS_err", &c_PSD2vsSTS_err, "c_PSD2vsSTS_err/D");
    t3_centr->Branch("c_PSD3vsSTS_err", &c_PSD3vsSTS_err, "c_PSD3vsSTS_err/D");
    // detector info. combination
    t3_centr->Branch("c_comb_meth1_2", &c_comb_meth1_2, "c_comb_meth1_2/D");
    t3_centr->Branch("c_comb_meth3_4_5", &c_comb_meth3_4_5, "c_comb_meth3_4_5/D");
    t3_centr->Branch("c_comb_methall", &c_comb_methall, "c_comb_methall/D");

    // TO DO: iterative methods for meth12 and meth345, save t2, t3, t4

    delta_c = 10.;
    nentries_nosel = (Int_t) t2_centr->GetEntriesFast();
    for (int i=0;i<nentries_nosel;i++)
    {
	t2_centr->GetEntry(i);

	// STS
        index1 = "";
	if (c_STS - delta_c > 0.) index1 += (c_STS - delta_c);
        else index1 += 0.;

	index2 = "";
	if (c_STS + delta_c < 100.) index2 += (c_STS + delta_c);
        else index2 += 100.;

	cut = "c_STS > " + index1 + " && c_STS <= " + index2;
        if (i==0) cout << "cut (reso RMS) = " << cut << endl;

        // fit functions = cuts
        hname = "hSTS_reso";
	detinfo = "c_STS - c_comb_methall";
	entry = t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kSTS == 1 &&"+cut, "goff");
	hSTS_tmp = (TH1F*) t2_centr->GetHistogram();
        factor_reso[0] = hSTS_tmp->GetRMS();

	//if (i==1) hSTS_tmp->Draw();
        //if (i==1) break;

        t2_centr->GetEntry(i);

	// PSD
        index1 = "";
	if (c_PSD - delta_c > 0.) index1 += (c_PSD - delta_c);
        else index1 += 0.;

	index2 = "";
	if (c_PSD + delta_c < 100.) index2 += (c_PSD + delta_c);
        else index2 += 100.;

	cut = "c_PSD > " + index1 + " && c_PSD <= " + index2;
        if (i==0) cout << "cut (reso RMS) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSD_reso";
	detinfo = "c_PSD - c_comb_methall";
	entry = t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD == 1 &&"+cut, "goff");
	hPSD_tmp = (TH1F*) t2_centr->GetHistogram();
        factor_reso[1] = hPSD_tmp->GetRMS();

        //if (i==0) hPSD_tmp->Draw();
        //if (i==0) break;

        t2_centr->GetEntry(i);

	// PSD sub1 vs STS
        index1 = "";
	if (c_PSD1vsSTS - delta_c > 0.) index1 += (c_PSD1vsSTS - delta_c);
        else index1 += 0.;

	index2 = "";
	if (c_PSD1vsSTS + delta_c < 100.) index2 += (c_PSD1vsSTS + delta_c);
        else index2 += 100.;

	cut = "c_PSD1vsSTS > " + index1 + " && c_PSD1vsSTS <= " + index2;
        if (i==0) cout << "cut (reso RMS) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSDvsSTS_reso";
	detinfo = "c_PSD1vsSTS - c_comb_methall";
	entry = t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD1vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t2_centr->GetHistogram();
        factor_reso[2] = hPSDvsSTS_tmp->GetRMS();

        t2_centr->GetEntry(i);

        // PSD sub2 vs STS
        index1 = "";
	if (c_PSD2vsSTS - delta_c > 0.) index1 += (c_PSD2vsSTS - delta_c);
        else index1 += 0.;

	index2 = "";
	if (c_PSD2vsSTS + delta_c < 100.) index2 += (c_PSD2vsSTS + delta_c);
        else index2 += 100.;

	cut = "c_PSD2vsSTS > " + index1 + " && c_PSD2vsSTS <= " + index2;
        if (i==0) cout << "cut (reso RMS) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSDvsSTS_reso";
	detinfo = "c_PSD2vsSTS - c_comb_methall";
	entry = t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD2vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t2_centr->GetHistogram();
        factor_reso[3] = hPSDvsSTS_tmp->GetRMS();

        t2_centr->GetEntry(i);

	// PSD sub3 vs STS
        index1 = "";
	if (c_PSD3vsSTS - delta_c > 0.) index1 += (c_PSD3vsSTS - delta_c);
        else index1 += 0.;

	index2 = "";
	if (c_PSD3vsSTS + delta_c < 100.) index2 += (c_PSD3vsSTS + delta_c);
        else index2 += 100.;

	cut = "c_PSD3vsSTS > " + index1 + " && c_PSD3vsSTS <= " + index2;
        if (i==0) cout << "cut (reso RMS) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSDvsSTS_reso";
	detinfo = "c_PSD3vsSTS - c_comb_methall";
	entry = t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t2_centr->GetHistogram();
        factor_reso[4] = hPSDvsSTS_tmp->GetRMS();

	// Combination all methods: Refine resolution
        t2_centr->GetEntry(i);

	for(int j=0;j<5;j++) factor_reso[j] = 1./TMath::Power(factor_reso[j], 2);
	c_comb_methall = 0.;
        c_comb_methall += c_STS*factor_reso[0] + c_PSD*factor_reso[1] + c_PSD1vsSTS*factor_reso[2] + c_PSD2vsSTS*factor_reso[3] + c_PSD3vsSTS*factor_reso[4];
        for(int j=1;j<5;j++) factor_reso[0] += factor_reso[j];
        c_comb_methall /= factor_reso[0];

        t3_centr->Fill();
    }

    //break;

    t3_centr->Write();
    
    // Refine resolution - iteration 2
    TTree* t4_centr = new TTree("t4_centr","t4_centr");
    t4_centr->SetDirectory(0);
    t4_centr->Branch("StsEvent.fmult", &M_STS_bis, "M_STS_bis/D");
    t4_centr->Branch("PsdEvent.fedep_" + sPSDsub1, &E_sub1_bis, "E_sub1_bis/D");
    t4_centr->Branch("PsdEvent.fedep_" + sPSDsub2, &E_sub2_bis, "E_sub2_bis/D");
    t4_centr->Branch("PsdEvent.fedep_" + sPSDsub3, &E_sub3_bis, "E_sub3_bis/D");
    t4_centr->Branch("PsdEvent.fedep_sub12", &E_sub12_bis, "E_sub12_bis/D");
    t4_centr->Branch("PsdEvent.fedep_sub23", &E_sub23_bis, "E_sub23_bis/D");
    t4_centr->Branch("McEvent.fB", &mcB, "mcB/D");
    t4_centr->Branch("kSTS", &kSTS, "kSTS/O");
    t4_centr->Branch("kPSD", &kPSD, "kPSD/O");
    t4_centr->Branch("kPSD1vsSTS", &kPSD1vsSTS, "kPSD1vsSTS/O");
    t4_centr->Branch("kPSD2vsSTS", &kPSD2vsSTS, "kPSD2vsSTS/O");
    t4_centr->Branch("kPSD3vsSTS", &kPSD3vsSTS, "kPSD3vsSTS/O");
    t4_centr->Branch("c_STS", &c_STS, "c_STS/D");
    t4_centr->Branch("c_PSD", &c_PSD, "c_PSD/D");
    t4_centr->Branch("c_PSD1vsSTS", &c_PSD1vsSTS, "c_PSD1vsSTS/D");
    t4_centr->Branch("c_PSD2vsSTS", &c_PSD2vsSTS, "c_PSD2vsSTS/D");
    t4_centr->Branch("c_PSD3vsSTS", &c_PSD3vsSTS, "c_PSD3vsSTS/D");
    t4_centr->Branch("c_STS_err", &c_STS_err, "c_STS_err/D");
    t4_centr->Branch("c_PSD_err", &c_PSD_err, "c_PSD_err/D");
    t4_centr->Branch("c_PSD1vsSTS_err", &c_PSD1vsSTS_err, "c_PSD1vsSTS_err/D");
    t4_centr->Branch("c_PSD2vsSTS_err", &c_PSD2vsSTS_err, "c_PSD2vsSTS_err/D");
    t4_centr->Branch("c_PSD3vsSTS_err", &c_PSD3vsSTS_err, "c_PSD3vsSTS_err/D");
    // detector info. combination
    t4_centr->Branch("c_comb_meth1_2", &c_comb_meth1_2, "c_comb_meth1_2/D");
    t4_centr->Branch("c_comb_meth3_4_5", &c_comb_meth3_4_5, "c_comb_meth3_4_5/D");
    t4_centr->Branch("c_comb_methall", &c_comb_methall, "c_comb_methall/D");

    delta_c = 10.;
    nentries_nosel = (Int_t) t3_centr->GetEntriesFast();
    for (int i=0;i<nentries_nosel;i++)
    {
	t3_centr->GetEntry(i);

	// STS
        index1 = "";
	if (c_STS - delta_c > 0.) index1 += (c_STS - delta_c);
        else index1 += 0.;

	index2 = "";
	if (c_STS + delta_c < 100.) index2 += (c_STS + delta_c);
        else index2 += 100.;

	cut = "c_STS > " + index1 + " && c_STS <= " + index2;
        if (i==0) cout << "cut (reso RMS) = " << cut << endl;

        // fit functions = cuts
        hname = "hSTS_reso";
	detinfo = "c_STS - c_comb_methall";
	entry = t3_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kSTS == 1 &&"+cut, "goff");
	hSTS_tmp = (TH1F*) t3_centr->GetHistogram();
        factor_reso[0] = hSTS_tmp->GetRMS();

        t3_centr->GetEntry(i);

	// PSD
        index1 = "";
	if (c_PSD - delta_c > 0.) index1 += (c_PSD - delta_c);
        else index1 += 0.;

	index2 = "";
	if (c_PSD + delta_c < 100.) index2 += (c_PSD + delta_c);
        else index2 += 100.;

	cut = "c_PSD > " + index1 + " && c_PSD <= " + index2;
        if (i==0) cout << "cut (reso RMS) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSD_reso";
	detinfo = "c_PSD - c_comb_methall";
	entry = t3_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD == 1 &&"+cut, "goff");
	hPSD_tmp = (TH1F*) t3_centr->GetHistogram();
        factor_reso[1] = hPSD_tmp->GetRMS();

        t3_centr->GetEntry(i);

	// PSD sub1 vs STS
        index1 = "";
	if (c_PSD1vsSTS - delta_c > 0.) index1 += (c_PSD1vsSTS - delta_c);
        else index1 += 0.;

	index2 = "";
	if (c_PSD1vsSTS + delta_c < 100.) index2 += (c_PSD1vsSTS + delta_c);
        else index2 += 100.;

	cut = "c_PSD1vsSTS > " + index1 + " && c_PSD1vsSTS <= " + index2;
        if (i==0) cout << "cut (reso RMS) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSDvsSTS_reso";
	detinfo = "c_PSD1vsSTS - c_comb_methall";
	entry = t3_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD1vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t3_centr->GetHistogram();
        factor_reso[2] = hPSDvsSTS_tmp->GetRMS();

        t3_centr->GetEntry(i);

        // PSD sub2 vs STS
        index1 = "";
	if (c_PSD2vsSTS - delta_c > 0.) index1 += (c_PSD2vsSTS - delta_c);
        else index1 += 0.;

	index2 = "";
	if (c_PSD2vsSTS + delta_c < 100.) index2 += (c_PSD2vsSTS + delta_c);
        else index2 += 100.;

	cut = "c_PSD2vsSTS > " + index1 + " && c_PSD2vsSTS <= " + index2;
        if (i==0) cout << "cut (reso RMS) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSDvsSTS_reso";
	detinfo = "c_PSD2vsSTS - c_comb_methall";
	entry = t3_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD2vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t3_centr->GetHistogram();
        factor_reso[3] = hPSDvsSTS_tmp->GetRMS();

        t3_centr->GetEntry(i);

	// PSD sub3 vs STS
        index1 = "";
	if (c_PSD3vsSTS - delta_c > 0.) index1 += (c_PSD3vsSTS - delta_c);
        else index1 += 0.;

	index2 = "";
	if (c_PSD3vsSTS + delta_c < 100.) index2 += (c_PSD3vsSTS + delta_c);
        else index2 += 100.;

	cut = "c_PSD3vsSTS > " + index1 + " && c_PSD3vsSTS <= " + index2;
        if (i==0) cout << "cut (reso RMS) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSDvsSTS_reso";
	detinfo = "c_PSD3vsSTS - c_comb_methall";
	entry = t3_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t3_centr->GetHistogram();
        factor_reso[4] = hPSDvsSTS_tmp->GetRMS();

	// Combination all methods: Refine resolution
        t3_centr->GetEntry(i);

	for(int j=0;j<5;j++) factor_reso[j] = 1./TMath::Power(factor_reso[j], 2);
	c_comb_methall = 0.;
        c_comb_methall += c_STS*factor_reso[0] + c_PSD*factor_reso[1] + c_PSD1vsSTS*factor_reso[2] + c_PSD2vsSTS*factor_reso[3] + c_PSD3vsSTS*factor_reso[4];
        for(int j=1;j<5;j++) factor_reso[0] += factor_reso[j];
        c_comb_methall /= factor_reso[0];

        t4_centr->Fill();
    }

    t4_centr->Write();
    */

    //break;

    //========================================================
    // === Draw

    int Nentries = (Int_t) t2_centr->GetEntriesFast();
    cout << "entries (baseline) = " << Nentries << endl;

    // STS
    dom_cut = 20;

    TCanvas* cDrawCut_STS = new TCanvas("c_drawcut_STS"," dom def STS ",200,10,500,500);
    stat_sample = 0.;

    /*for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_STS > " + index1 + " && c_STS <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_STS > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hSTS_drawcut";
	hname += i+1;
	detinfo = "StsEvent.fmult:McEvent.fB";
	t2_centr->Draw(detinfo +">>" + hname + "(170, 0., 17., 1000, 0., 1.)", "kSTS == 1 &&"+cut, "goff");
	hSTS_tmp = (TH1F*) t2_centr->GetHistogram();
	cout << "stat dom " << i+1 << ": " << 100*hSTS_tmp->Integral()/Nentries << endl;

	color_ind = i%4+1;
	hSTS_tmp->SetMarkerColor(color_ind);
	hSTS_tmp->SetMarkerStyle(7);

	if (i==0) hSTS_tmp->Draw("");
	else hSTS_tmp->Draw("same");

	}
	*/

    hname = "hSTS_drawcut";
    detinfo = "StsEvent.fmult:McEvent.fB";
    t2_centr->Draw(detinfo +">>" + hname + "(34, 0., 17., 200, 0., 1.)", "kSTS == 1", "colz");

    //LcteX = 2.;
    delta_c = 1.;
    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	if ((i+1)*step - delta_c > 0.) index1 += ((i+1)*step - delta_c);
        else index1 += 0.;

	index2 = "";
	if ((i+1)*step + delta_c < 100.) index2 += ((i+1)*step + delta_c);
        else index2 += 100.;

	cut = "c_STS > " + index1 + " && c_STS <= " + index2;
        cout << "cut (fit) = " << cut << endl;

        // fit functions = cuts
        hname = "hSTS_drawcut";
	hname += i+1;
	detinfo = "StsEvent.fmult:McEvent.fB";
	entry = t2_centr->Draw(detinfo +">>" + hname + "(170, 0., 17., 1000, 0., 1.)", "kSTS == 1 &&"+cut, "goff");
	hSTS_tmp = (TH1F*) t2_centr->GetHistogram();

        if (entry==0.) continue;

        hSTS_tmp->Fit("pol0","0, Q");
	funct = (TF1*) hSTS_tmp->GetFunction("pol0");

	// draw around <b>
        hname = "hSTS_drawcut2";
	hname += i+1;
        detinfo = "McEvent.fB";
	t2_centr->Draw(detinfo +">>" + hname + "(170, 0., 17.)", "kSTS == 1 &&"+cut, "goff");
	hSTS_b = (TH1F*) t2_centr->GetHistogram();

	LcteX_1 = hSTS_b->GetMean()-3*hSTS_b->GetRMS();
        LcteX_2 = hSTS_b->GetMean()+3*hSTS_b->GetRMS();

	funct->SetRange(LcteX_1, LcteX_2);
	color_ind = i%4+1;
	funct->SetLineColor(2);
        funct->Draw("same");
    }

    break;

    TCanvas* c_b_STS = new TCanvas("c_b_STS"," dom STS - b distr.",200,10,700,500);       
    gStyle->SetOptStat(0);
    c_b_STS->Divide(5, 4);

    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_STS > " + index1 + " && c_STS <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_STS > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hSTS_b";
	hname += i+1;
        detinfo = "McEvent.fB";
	t2_centr->Draw(detinfo +">>" + hname + "(170, 0., 17.)", "kSTS == 1 &&"+cut, "goff");
	hSTS_b = (TH1F*) t2_centr->GetHistogram();
	//cout << "stat dom " << i+1 << ": " << 100*hSTS_b->Integral()/Nentries << endl;

        // sigma from histo
	mean0[i] = hSTS_b->GetMean();
	emean0[i] = hSTS_b->GetMeanError();
	sigma0[i] = hSTS_b->GetRMS();
	esigma0[i] = hSTS_b->GetRMSError();

        // stat
	stat_sample += hSTS_b->Integral();

        // DO mean c_STS !!!
	//if (i==0) inc_evt_STS[i] = hSTS_b->Integral();
        //else inc_evt_STS[i] = inc_evt_STS[i-1] + hSTS_b->Integral();

        hname = "hSTS_c";
	hname += i+1;
        detinfo = "c_STS";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kSTS == 1 &&"+cut, "goff");
	hSTS_c = (TH1F*) t2_centr->GetHistogram();
        inc_evt_STS[i] = hSTS_c->GetMean();
        inc_evt_err_STS[i] = hSTS_c->GetMeanError();

        //draw
        c_b_STS->cd(i+1);
	color_ind = i%4+1;
	hSTS_b->SetLineColor(color_ind);
        hSTS_b->SetLineWidth(3);
	hSTS_b->Draw();
    }
    cout << "stat (selected) sample = " << stat_sample << endl;

    // PSD
    // exception
    dom_cut = 16;        //2agev: 16, 10agev: 11, 35agev: 20

    TCanvas* cDrawCut_PSD = new TCanvas("c_drawcut_PSD"," dom def PSD ",200,10,500,500);
    stat_sample = 0.;

    /*for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_PSD > " + index1 + " && c_PSD <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_PSD > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hPSD_drawcut";
	hname += i+1;
        //detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":PsdEvent.fedep_" + sPSDsub1;
	// exception: 35 agev
        detinfo = "PsdEvent.fedep_" + sPSDsub1 + ":PsdEvent.fedep_" + sPSDsub2;
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1., 100, 0., 1.)", "kPSD == 1 &&"+cut, "goff");
	hPSD_tmp = (TH1F*) t2_centr->GetHistogram();
	cout << "stat dom " << i+1 << ": " << 100*hPSD_tmp->Integral()/Nentries << endl;

	color_ind = i%4+1;
	hPSD_tmp->SetMarkerColor(color_ind);
	hPSD_tmp->SetMarkerStyle(7);

	if (i==0) hPSD_tmp->Draw("");
	else hPSD_tmp->Draw("same");

    }*/

    hname = "hPSD_drawcut";

    detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":PsdEvent.fedep_" + sPSDsub1;
    // exception: 35 agev
    //detinfo = "PsdEvent.fedep_" + sPSDsub1 + ":PsdEvent.fedep_" + sPSDsub2;

    t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1., 100, 0., 1.)", "kPSD == 1", "colz");

    //LcteX = 2.;
    delta_c = 0.1;
    for (int i=0;i<dom_cut;i++)
    {
        //if (i!=10) continue;

        index1 = "";
	if ((i+1)*step - delta_c > 0.) index1 += ((i+1)*step - delta_c);
        else index1 += 0.;

	index2 = "";
	if ((i+1)*step + delta_c < 100.) index2 += ((i+1)*step + delta_c);
        else index2 += 100.;

	cut = "c_PSD > " + index1 + " && c_PSD <= " + index2;
        cout << "cut (fit) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSD_drawcut";
	hname += i+1;

	detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":PsdEvent.fedep_" + sPSDsub1;
        // exception: 35 agev
        //detinfo = "PsdEvent.fedep_" + sPSDsub1 + ":PsdEvent.fedep_" + sPSDsub2;

	entry = t2_centr->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD == 1 &&"+cut, "goff");
	hPSD_tmp = (TH1F*) t2_centr->GetHistogram();

	//hPSD_tmp->Draw();
        //continue;

        if (entry==0.) continue;

        hPSD_tmp->Fit("pol1","0, Q");
	funct = (TF1*) hPSD_tmp->GetFunction("pol1");

	// draw around <b>
        hname = "hPSD_drawcut2";
	hname += i+1;

	detinfo = "PsdEvent.fedep_" + sPSDsub1;
	// exception: 35 agev
        //detinfo = "PsdEvent.fedep_" + sPSDsub2;

	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1.)", "kPSD == 1 &&"+cut, "goff");
	hPSD_b = (TH1F*) t2_centr->GetHistogram();

	LcteX_1 = hPSD_b->GetMean()-4*hPSD_b->GetRMS();
        LcteX_2 = hPSD_b->GetMean()+4*hPSD_b->GetRMS();

	funct->SetRange(LcteX_1, LcteX_2);
	color_ind = i%4+1;
	funct->SetLineColor(2);
        if (i < dom_cut-1) funct->Draw("same");
    }

    TCanvas* c_b_PSD = new TCanvas("c_b_PSD"," dom PSD - b distr.",200,10,700,500);         
    gStyle->SetOptStat(0);
    c_b_PSD->Divide(5, 4);

    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_PSD > " + index1 + " && c_PSD <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_PSD > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hPSD_b";
	hname += i+1;
        detinfo = "McEvent.fB";
	t2_centr->Draw(detinfo +">>" + hname + "(170, 0., 17.)", "kPSD == 1 &&"+cut, "goff");
	hPSD_b = (TH1F*) t2_centr->GetHistogram();
	//cout << "stat dom " << i+1 << ": " << 100*hPSD_b->Integral()/Nentries << endl;

        // sigma from histo
	mean1[i] = hPSD_b->GetMean();
	emean1[i] = hPSD_b->GetMeanError();
	sigma1[i] = hPSD_b->GetRMS();
	esigma1[i] = hPSD_b->GetRMSError();

        // stat
	stat_sample += hPSD_b->Integral();

        // DO mean c_PSD !!!
	//if (i==0) inc_evt_PSD[i] = hPSD_b->Integral();
        //else inc_evt_PSD[i] = inc_evt_PSD[i-1] + hPSD_b->Integral();

        hname = "hPSD_c";
	hname += i+1;
        detinfo = "c_PSD";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD == 1 &&"+cut, "goff");
	hPSD_c = (TH1F*) t2_centr->GetHistogram();
        inc_evt_PSD[i] = hPSD_c->GetMean();
        inc_evt_err_PSD[i] = hPSD_c->GetMeanError();

        //draw
        c_b_PSD->cd(i+1);
	color_ind = i%4+1;
	hPSD_b->SetLineColor(color_ind);
        hPSD_b->SetLineWidth(3);
	hPSD_b->Draw();
    }
    cout << "stat (selected) sample = " << stat_sample << endl;

    for (int i=dom_cut;i<20;i++)
    {
	mean1[i] = mean1[dom_cut-1];
	emean1[i] = emean1[dom_cut-1];
	sigma1[i] = sigma1[dom_cut-1];
	esigma1[i] = esigma1[dom_cut-1];

        inc_evt_PSD[i] = inc_evt_PSD[dom_cut-1];
    }

    // PSD vs STS
    // sub1
    // exception
    dom_cut = 18;  //2agev: 18, 10agev: 13, 35agev: 18

    TCanvas* cDrawCut_sub1vsM = new TCanvas("c_drawcut_PSD_sub1vsM"," dom def PSD (sub1) vs STS ",200,10,500,500);
    stat_sample = 0.;

    /*for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_PSD1vsSTS > " + index1 + " && c_PSD1vsSTS <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_PSD1vsSTS > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hPSD1vsSTS_drawcut";
	hname += i+1;
        detinfo = "PsdEvent.fedep_" + sPSDsub1 + ":StsEvent.fmult";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1., 100, 0., 1.)", "kPSD1vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t2_centr->GetHistogram();
	cout << "stat dom " << i+1 << ": " << 100*hPSDvsSTS_tmp->Integral()/Nentries << endl;

	color_ind = i%4+1;
	hPSDvsSTS_tmp->SetMarkerColor(color_ind);
	hPSDvsSTS_tmp->SetMarkerStyle(7);

	if (i==0) hPSDvsSTS_tmp->Draw("");
	else hPSDvsSTS_tmp->Draw("same");

    }*/

    hname = "hPSD1vsSTS_drawcut";
    detinfo = "PsdEvent.fedep_" + sPSDsub1 + ":StsEvent.fmult";
    t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1., 100, 0., 1.)", "kPSD1vsSTS == 1", "colz");

    //LcteX = 2.;
    delta_c = 0.1;
    for (int i=0;i<dom_cut;i++)
    {
        //if (i!=10) continue;

        index1 = "";
	if ((i+1)*step - delta_c > 0.) index1 += ((i+1)*step - delta_c);
        else index1 += 0.;

	index2 = "";
	if ((i+1)*step + delta_c < 100.) index2 += ((i+1)*step + delta_c);
        else index2 += 100.;

	cut = "c_PSD1vsSTS > " + index1 + " && c_PSD1vsSTS <= " + index2;
        cout << "cut (fit) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSD1vsSTS_drawcut";
	hname += i+1;
        detinfo = "PsdEvent.fedep_" + sPSDsub1 + ":StsEvent.fmult";
	entry = t2_centr->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD1vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t2_centr->GetHistogram();

	//hPSDvsSTS_tmp->Draw();
        //continue;

        if (entry==0.) continue;

        hPSDvsSTS_tmp->Fit("pol1","0, Q");
	funct = (TF1*) hPSDvsSTS_tmp->GetFunction("pol1");

	// draw around <b>
        hname = "hPSD1vsSTS_drawcut2";
	hname += i+1;
        detinfo = "StsEvent.fmult";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1.)", "kPSD1vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_b = (TH1F*) t2_centr->GetHistogram();

	LcteX_1 = hPSDvsSTS_b->GetMean()-4*hPSDvsSTS_b->GetRMS();
        LcteX_2 = hPSDvsSTS_b->GetMean()+4*hPSDvsSTS_b->GetRMS();

	funct->SetRange(LcteX_1, LcteX_2);
	color_ind = i%4+1;
	funct->SetLineColor(2);
        if (i < dom_cut-1) funct->Draw("same");
    }

    TCanvas* c_b_PSD_sub1vsM = new TCanvas("c_b_PSD_sub1vsM"," dom PSD (sub1) vs STS - b distr.",200,10,700,500);   
    gStyle->SetOptStat(0);
    c_b_PSD_sub1vsM->Divide(5, 4);

    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_PSD1vsSTS > " + index1 + " && c_PSD1vsSTS <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_PSD1vsSTS > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hPSD1vsSTS_b";
	hname += i+1;
        detinfo = "McEvent.fB";
	t2_centr->Draw(detinfo +">>" + hname + "(170, 0., 17.)", "kPSD1vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_b = (TH1F*) t2_centr->GetHistogram();
	//cout << "stat dom " << i+1 << ": " << 100*hPSDvsSTS_b->Integral()/Nentries << endl;

        // sigma from histo
	mean2[i] = hPSDvsSTS_b->GetMean();
	emean2[i] = hPSDvsSTS_b->GetMeanError();
	sigma2[i] = hPSDvsSTS_b->GetRMS();
	esigma2[i] = hPSDvsSTS_b->GetRMSError();

        // stat
	stat_sample += hPSDvsSTS_b->Integral();

        // DO mean c_PSD !!!
	//if (i==0) inc_evt_PSD1vsSTS[i] = hPSDvsSTS_b->Integral();
        //else inc_evt_PSD1vsSTS[i] = inc_evt_PSD1vsSTS[i-1] + hPSDvsSTS_b->Integral();

        hname = "hPSD1vsSTS_c";
	hname += i+1;
        detinfo = "c_PSD1vsSTS";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD1vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_c = (TH1F*) t2_centr->GetHistogram();
        inc_evt_PSD1vsSTS[i] = hPSDvsSTS_c->GetMean();
        inc_evt_err_PSD1vsSTS[i] = hPSDvsSTS_c->GetMeanError();

        //draw
        c_b_PSD_sub1vsM->cd(i+1);
	color_ind = i%4+1;
	hPSDvsSTS_b->SetLineColor(color_ind);
        hPSDvsSTS_b->SetLineWidth(3);
	hPSDvsSTS_b->Draw();
    }
    cout << "stat (selected) sample = " << stat_sample << endl;

    for (int i=dom_cut;i<20;i++)
    {
	mean2[i] = mean2[dom_cut-1];
	emean2[i] = emean2[dom_cut-1];
	sigma2[i] = sigma2[dom_cut-1];
	esigma2[i] = esigma2[dom_cut-1];

        inc_evt_PSD1vsSTS[i] = inc_evt_PSD1vsSTS[dom_cut-1];
    }

    //hPSDvsSTS_tmp->Delete();
    //hPSDvsSTS_b->Delete();

    // sub2
    // exception
    dom_cut = 19; //2agev:19, 10agev: 17, 35agev: 19

    TCanvas* cDrawCut_sub2vsM = new TCanvas("c_drawcut_PSD_sub2vsM"," dom def PSD (sub2) vs STS ",200,10,500,500);
    stat_sample = 0.;

    /*for (int i=0;i<dom_cut;i++)    
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_PSD2vsSTS > " + index1 + " && c_PSD2vsSTS <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_PSD2vsSTS > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hPSD2vsSTS_drawcut";
	hname += i+1;
        detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":StsEvent.fmult";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1., 100, 0., 1.)", "kPSD2vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t2_centr->GetHistogram();
	cout << "stat dom " << i+1 << ": " << 100*hPSDvsSTS_tmp->Integral()/Nentries << endl;

	color_ind = i%4+1;
	hPSDvsSTS_tmp->SetMarkerColor(color_ind);
	hPSDvsSTS_tmp->SetMarkerStyle(7);

	if (i==0) hPSDvsSTS_tmp->Draw("");
	else hPSDvsSTS_tmp->Draw("same");
    }*/

    hname = "hPSD2vsSTS_drawcut";
    detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":StsEvent.fmult";
    t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1., 100, 0., 1.)", "kPSD2vsSTS == 1", "colz");

    //LcteX = 2.;
    delta_c = 0.1;
    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	if ((i+1)*step - delta_c > 0.) index1 += ((i+1)*step - delta_c);
        else index1 += 0.;

	index2 = "";
	if ((i+1)*step + delta_c < 100.) index2 += ((i+1)*step + delta_c);
        else index2 += 100.;

	cut = "c_PSD2vsSTS > " + index1 + " && c_PSD2vsSTS <= " + index2;

	// exception: 10 agev ... to be improved later
        //if (i==9) cut += "&& (StsEvent.fmult>0.05 || PsdEvent.fedep_1stCorona>0.3)";

        // exception: 2 agev ... to be improved later
        if (i==3) cut += "&& (StsEvent.fmult<0.9)";

	cout << "cut (fit) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSD2vsSTS_drawcut";
	hname += i+1;
        detinfo = "PsdEvent.fedep_" + sPSDsub2 + ":StsEvent.fmult";
	entry = t2_centr->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD2vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t2_centr->GetHistogram();

	//hPSDvsSTS_tmp->Draw();
        //continue;

        if (entry==0.) continue;

        hPSDvsSTS_tmp->Fit("pol1","0, Q");
	funct = (TF1*) hPSDvsSTS_tmp->GetFunction("pol1");

	// draw around <b>
        hname = "hPSD2vsSTS_drawcut2";
	hname += i+1;
        detinfo = "StsEvent.fmult";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1.)", "kPSD2vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_b = (TH1F*) t2_centr->GetHistogram();

	LcteX_1 = hPSDvsSTS_b->GetMean()-4*hPSDvsSTS_b->GetRMS();
        LcteX_2 = hPSDvsSTS_b->GetMean()+4*hPSDvsSTS_b->GetRMS();

	funct->SetRange(LcteX_1, LcteX_2);
	color_ind = i%4+1;
	funct->SetLineColor(2);
        if (i < dom_cut-1) funct->Draw("same");
    }

    TCanvas* c_b_PSD_sub2vsM = new TCanvas("c_b_PSD_sub2vsM"," dom PSD (sub2) vs STS - b distr.",200,10,700,500);
    gStyle->SetOptStat(0);
    c_b_PSD_sub2vsM->Divide(5, 4);

    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_PSD2vsSTS > " + index1 + " && c_PSD2vsSTS <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_PSD2vsSTS > " + index1;
	}

	// exception: 10 agev ... to be improved later
        //if (i==8) cut += "&& (StsEvent.fmult>0.05 || PsdEvent.fedep_1stCorona>0.3)";

        // exception: 2 agev ... to be improved later
        if (i==2) cut += "&& (StsEvent.fmult<0.9)";

	cout << "cut = " << cut << endl;

        hname = "hPSD2vsSTS_b";
	hname += i+1;
        detinfo = "McEvent.fB";
	t2_centr->Draw(detinfo +">>" + hname + "(170, 0., 17.)", "kPSD2vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_b = (TH1F*) t2_centr->GetHistogram();
	//cout << "stat dom " << i+1 << ": " << 100*hPSDvsSTS_b->Integral()/Nentries << endl;

        // sigma from histo
	mean3[i] = hPSDvsSTS_b->GetMean();
	emean3[i] = hPSDvsSTS_b->GetMeanError();
	sigma3[i] = hPSDvsSTS_b->GetRMS();
	esigma3[i] = hPSDvsSTS_b->GetRMSError();

        // stat
	stat_sample += hPSDvsSTS_b->Integral();

        // DO mean c_PSD !!!
	//if (i==0) inc_evt_PSD2vsSTS[i] = hPSDvsSTS_b->Integral();
        //else inc_evt_PSD2vsSTS[i] = inc_evt_PSD2vsSTS[i-1] + hPSDvsSTS_b->Integral();

        hname = "hPSD2vsSTS_c";
	hname += i+1;
        detinfo = "c_PSD2vsSTS";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD2vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_c = (TH1F*) t2_centr->GetHistogram();
        inc_evt_PSD2vsSTS[i] = hPSDvsSTS_c->GetMean();
        inc_evt_err_PSD2vsSTS[i] = hPSDvsSTS_c->GetMeanError();

        //draw
        c_b_PSD_sub2vsM->cd(i+1);
	color_ind = i%4+1;
	hPSDvsSTS_b->SetLineColor(color_ind);
        hPSDvsSTS_b->SetLineWidth(3);
	hPSDvsSTS_b->Draw();
    }
    cout << "stat (selected) sample = " << stat_sample << endl;

    for (int i=dom_cut;i<20;i++)
    {
	mean3[i] = mean3[dom_cut-1];
	emean3[i] = emean3[dom_cut-1];
	sigma3[i] = sigma3[dom_cut-1];
	esigma3[i] = esigma3[dom_cut-1];

        inc_evt_PSD2vsSTS[i] = inc_evt_PSD2vsSTS[dom_cut-1];
    }

    //break;

    //hPSDvsSTS_tmp->Delete();
    //hPSDvsSTS_b->Delete();

    // sub3
    // exception
    dom_cut = 19; //2agev: 19, 10agev: 19, 35agev:19

    TCanvas* cDrawCut_sub3vsM = new TCanvas("c_drawcut_PSD_sub3vsM"," dom def PSD (sub3) vs STS ",200,10,500,500);
    stat_sample = 0.;

    /*for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_PSD3vsSTS > " + index1 + " && c_PSD3vsSTS <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_PSD3vsSTS > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hPSD3vsSTS_drawcut";
	hname += i+1;
        detinfo = "PsdEvent.fedep_" + sPSDsub3 + ":StsEvent.fmult";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1., 100, 0., 1.)", "kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t2_centr->GetHistogram();
	cout << "stat dom " << i+1 << ": " << 100*hPSDvsSTS_tmp->Integral()/Nentries << endl;

	color_ind = i%4+1;
	hPSDvsSTS_tmp->SetMarkerColor(color_ind);
	hPSDvsSTS_tmp->SetMarkerStyle(7);

	if (i==0) hPSDvsSTS_tmp->Draw("");
	else hPSDvsSTS_tmp->Draw("same");

	} */

    hname = "hPSD3vsSTS_drawcut";
    detinfo = "PsdEvent.fedep_" + sPSDsub3 + ":StsEvent.fmult";
    t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1., 100, 0., 1.)", "kPSD3vsSTS == 1", "colz");

    //LcteX = 2.;
    delta_c = 0.1;
    for (int i=0;i<dom_cut;i++)
    {
        //if (i!=10) continue;

        index1 = "";
	if ((i+1)*step - delta_c > 0.) index1 += ((i+1)*step - delta_c);
        else index1 += 0.;

	index2 = "";
	if ((i+1)*step + delta_c < 100.) index2 += ((i+1)*step + delta_c);
        else index2 += 100.;

	cut = "c_PSD3vsSTS > " + index1 + " && c_PSD3vsSTS <= " + index2;
        cout << "cut (fit) = " << cut << endl;

        // fit functions = cuts
        hname = "hPSD3vsSTS_drawcut";
	hname += i+1;
        detinfo = "PsdEvent.fedep_" + sPSDsub3 + ":StsEvent.fmult";
	entry = t2_centr->Draw(detinfo +">>" + hname + "(1000, 0., 1., 1000, 0., 1.)", "kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_tmp = (TH1F*) t2_centr->GetHistogram();

	//hPSDvsSTS_tmp->Draw();
        //continue;

        if (entry==0.) continue;

        hPSDvsSTS_tmp->Fit("pol1","0, Q");
	funct = (TF1*) hPSDvsSTS_tmp->GetFunction("pol1");

	// draw around <b>
        hname = "hPSD3vsSTS_drawcut2";
	hname += i+1;
        detinfo = "StsEvent.fmult";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 1.)", "kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_b = (TH1F*) t2_centr->GetHistogram();

	LcteX_1 = hPSDvsSTS_b->GetMean()-4*hPSDvsSTS_b->GetRMS();
        LcteX_2 = hPSDvsSTS_b->GetMean()+4*hPSDvsSTS_b->GetRMS();

	funct->SetRange(LcteX_1, LcteX_2);
	color_ind = i%4+1;
	funct->SetLineColor(2);
        if (i < dom_cut-1) funct->Draw("same");
    }

    TCanvas* c_b_PSD_sub3vsM = new TCanvas("c_b_PSD_sub3vsM"," dom PSD (sub3) vs STS - b distr.",200,10,700,500);
    gStyle->SetOptStat(0);
    c_b_PSD_sub3vsM->Divide(5, 4);

    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_PSD3vsSTS > " + index1 + " && c_PSD3vsSTS <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_PSD3vsSTS > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hPSD3vsSTS_b";
	hname += i+1;
        detinfo = "McEvent.fB";
	t2_centr->Draw(detinfo +">>" + hname + "(170, 0., 17.)", "kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_b = (TH1F*) t2_centr->GetHistogram();
	//cout << "stat dom " << i+1 << ": " << 100*hPSDvsSTS_b->Integral()/Nentries << endl;

        // sigma from histo
	mean4[i] = hPSDvsSTS_b->GetMean();
	emean4[i] = hPSDvsSTS_b->GetMeanError();
	sigma4[i] = hPSDvsSTS_b->GetRMS();
	esigma4[i] = hPSDvsSTS_b->GetRMSError();

        // stat
	stat_sample += hPSDvsSTS_b->Integral();

        // DO mean c_PSD !!!
	//if (i==0) inc_evt_PSD3vsSTS[i] = hPSDvsSTS_b->Integral();
        //else inc_evt_PSD3vsSTS[i] = inc_evt_PSD3vsSTS[i-1] + hPSDvsSTS_b->Integral();

        hname = "hPSD3vsSTS_c";
	hname += i+1;
        detinfo = "c_PSD3vsSTS";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_c = (TH1F*) t2_centr->GetHistogram();
        inc_evt_PSD3vsSTS[i] = hPSDvsSTS_c->GetMean();
        inc_evt_err_PSD3vsSTS[i] = hPSDvsSTS_c->GetMeanError();

        //draw
        c_b_PSD_sub3vsM->cd(i+1);
	color_ind = i%4+1;
	hPSDvsSTS_b->SetLineColor(color_ind);
        hPSDvsSTS_b->SetLineWidth(3);
	hPSDvsSTS_b->Draw();
    }
    cout << "stat (selected) sample = " << stat_sample << endl;

    for (int i=dom_cut;i<20;i++)
    {
	mean4[i] = mean4[dom_cut-1];
	emean4[i] = emean4[dom_cut-1];
	sigma4[i] = sigma4[dom_cut-1];
	esigma4[i] = esigma4[dom_cut-1];

        inc_evt_PSD3vsSTS[i] = inc_evt_PSD3vsSTS[dom_cut-1];
    }

    // Combination methods
    // - 1 + 2 -
    dom_cut = 20;

    TCanvas* c_b_comb_meth1_2 = new TCanvas("c_b_comb_meth1_2"," meth 1+2 - b distr.",200,10,700,500);
    gStyle->SetOptStat(0);
    c_b_comb_meth1_2->Divide(5, 4);

    stat_sample=0.;
    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_comb_meth1_2 > " + index1 + " && c_comb_meth1_2 <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_comb_meth1_2 > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hcomb_meth1_2_b";
	hname += i+1;
        detinfo = "McEvent.fB";
	t2_centr->Draw(detinfo +">>" + hname + "(170, 0., 17.)", "kSTS == 1 && kPSD == 1 &&"+cut, "goff");
	hPSDvsSTS_b = (TH1F*) t2_centr->GetHistogram();
	//cout << "stat dom " << i+1 << ": " << 100*hPSDvsSTS_b->Integral()/Nentries << endl;

        // sigma from histo
	mean5[i] = hPSDvsSTS_b->GetMean();
	emean5[i] = hPSDvsSTS_b->GetMeanError();
	sigma5[i] = hPSDvsSTS_b->GetRMS();
	esigma5[i] = hPSDvsSTS_b->GetRMSError();

        // stat
	stat_sample += hPSDvsSTS_b->Integral();

        // DO mean c_PSD !!!
	//if (i==0) inc_evt_PSD3vsSTS[i] = hPSDvsSTS_b->Integral();
        //else inc_evt_PSD3vsSTS[i] = inc_evt_PSD3vsSTS[i-1] + hPSDvsSTS_b->Integral();

        hname = "hcomb_meth1_2_c";
	hname += i+1;
        detinfo = "c_comb_meth1_2";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kSTS == 1 && kPSD == 1 &&"+cut, "goff");
	hPSDvsSTS_c = (TH1F*) t2_centr->GetHistogram();
        inc_evt_comb_meth1_2[i] = hPSDvsSTS_c->GetMean();
        inc_evt_err_comb_meth1_2[i] = hPSDvsSTS_c->GetMeanError();

        //draw
        c_b_comb_meth1_2->cd(i+1);
	color_ind = i%4+1;
	hPSDvsSTS_b->SetLineColor(color_ind);
        hPSDvsSTS_b->SetLineWidth(3);
	hPSDvsSTS_b->Draw();
    }
    cout << "stat (selected) sample = " << stat_sample << endl;

    for (int i=dom_cut;i<20;i++)
    {
	mean5[i] = mean5[dom_cut-1];
	emean5[i] = emean5[dom_cut-1];
	sigma5[i] = sigma5[dom_cut-1];
	esigma5[i] = esigma5[dom_cut-1];

        inc_evt_comb_meth1_2[i] = inc_evt_comb_meth1_2[dom_cut-1];
    }

    // - 3 + 4 + 5 -
    dom_cut = 20;

    TCanvas* c_b_comb_meth3_4_5 = new TCanvas("c_b_comb_meth3_4_5"," meth 3+4+5 - b distr.",200,10,700,500);
    gStyle->SetOptStat(0);
    c_b_comb_meth3_4_5->Divide(5, 4);

    stat_sample=0.;
    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_comb_meth3_4_5 > " + index1 + " && c_comb_meth3_4_5 <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_comb_meth3_4_5 > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hcomb_meth3_4_5_b";
	hname += i+1;
        detinfo = "McEvent.fB";
	t2_centr->Draw(detinfo +">>" + hname + "(170, 0., 17.)", "kPSD1vsSTS == 1 && kPSD2vsSTS == 1 && kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_b = (TH1F*) t2_centr->GetHistogram();
	//cout << "stat dom " << i+1 << ": " << 100*hPSDvsSTS_b->Integral()/Nentries << endl;

        // sigma from histo
	mean6[i] = hPSDvsSTS_b->GetMean();
	emean6[i] = hPSDvsSTS_b->GetMeanError();
	sigma6[i] = hPSDvsSTS_b->GetRMS();
	esigma6[i] = hPSDvsSTS_b->GetRMSError();

        // stat
	stat_sample += hPSDvsSTS_b->Integral();

        // DO mean c_PSD !!!
	//if (i==0) inc_evt_PSD3vsSTS[i] = hPSDvsSTS_b->Integral();
        //else inc_evt_PSD3vsSTS[i] = inc_evt_PSD3vsSTS[i-1] + hPSDvsSTS_b->Integral();

        hname = "hcomb_meth3_4_5_c";
	hname += i+1;
        detinfo = "c_comb_meth3_4_5";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kPSD1vsSTS == 1 && kPSD2vsSTS == 1 && kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_c = (TH1F*) t2_centr->GetHistogram();
        inc_evt_comb_meth3_4_5[i] = hPSDvsSTS_c->GetMean();
        inc_evt_err_comb_meth3_4_5[i] = hPSDvsSTS_c->GetMeanError();

        //draw
        c_b_comb_meth3_4_5->cd(i+1);
	color_ind = i%4+1;
	hPSDvsSTS_b->SetLineColor(color_ind);
        hPSDvsSTS_b->SetLineWidth(3);
	hPSDvsSTS_b->Draw();
    }
    cout << "stat (selected) sample = " << stat_sample << endl;

    for (int i=dom_cut;i<20;i++)
    {
	mean6[i] = mean6[dom_cut-1];
	emean6[i] = emean6[dom_cut-1];
	sigma6[i] = sigma6[dom_cut-1];
	esigma6[i] = esigma6[dom_cut-1];

        inc_evt_comb_meth3_4_5[i] = inc_evt_comb_meth3_4_5[dom_cut-1];
    }

    // - 1 + 2 + 3 + 4 + 5 -
    // exception:
    dom_cut = 19;   // 2agev: 19, 10agev: 13, 35: 20

    TCanvas* c_b_comb_methall = new TCanvas("c_b_comb_methall"," meth 1+2+3+4+5 - b distr.",200,10,700,500);
    gStyle->SetOptStat(0);
    c_b_comb_methall->Divide(5, 4);

    // iteration 0 (simple average)
    stat_sample=0.;
    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_comb_methall > " + index1 + " && c_comb_methall <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_comb_methall > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hcomb_methall_b";
	hname += i+1;
        detinfo = "McEvent.fB";
	t2_centr->Draw(detinfo +">>" + hname + "(170, 0., 17.)", "kSTS == 1 && kPSD == 1 && kPSD1vsSTS == 1 && kPSD2vsSTS == 1 && kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_b = (TH1F*) t2_centr->GetHistogram();
	//cout << "stat dom " << i+1 << ": " << 100*hPSDvsSTS_b->Integral()/Nentries << endl;

        // sigma from histo
	mean7[i] = hPSDvsSTS_b->GetMean();
	emean7[i] = hPSDvsSTS_b->GetMeanError();
	sigma7[i] = hPSDvsSTS_b->GetRMS();
	esigma7[i] = hPSDvsSTS_b->GetRMSError();

        // stat
	stat_sample += hPSDvsSTS_b->Integral();

        // DO mean c_PSD !!!
	//if (i==0) inc_evt_PSD3vsSTS[i] = hPSDvsSTS_b->Integral();
        //else inc_evt_PSD3vsSTS[i] = inc_evt_PSD3vsSTS[i-1] + hPSDvsSTS_b->Integral();

        hname = "hcomb_methall_c";
	hname += i+1;
        detinfo = "c_comb_methall";
	t2_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kSTS == 1 && kPSD == 1 && kPSD1vsSTS == 1 && kPSD2vsSTS == 1 && kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_c = (TH1F*) t2_centr->GetHistogram();

	inc_evt_comb_methall_it0[i] = hPSDvsSTS_c->GetMean();
        inc_evt_err_comb_methall_it0[i] = hPSDvsSTS_c->GetMeanError();

        //draw
        //c_b_comb_methall->cd(i+1);
	//color_ind = i%4+1;
	//hPSDvsSTS_b->SetLineColor(color_ind);
        //hPSDvsSTS_b->SetLineWidth(3);
	//hPSDvsSTS_b->Draw();
    }
    cout << "stat (selected) sample = " << stat_sample << endl;

    for (int i=dom_cut;i<20;i++)
    {
	mean7[i] = mean7[dom_cut-1];
	emean7[i] = emean7[dom_cut-1];
	sigma7[i] = sigma7[dom_cut-1];
	esigma7[i] = esigma7[dom_cut-1];

        inc_evt_comb_methall_it0[i] = inc_evt_comb_methall_it0[dom_cut-1];
    }

    // iteration 1 (refine average - accounting for different resolutions from the different methods)
    stat_sample=0.;
    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_comb_methall > " + index1 + " && c_comb_methall <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_comb_methall > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hcomb_methall_b";
	hname += i+1;
        detinfo = "McEvent.fB";
	t3_centr->Draw(detinfo +">>" + hname + "(170, 0., 17.)", "kSTS == 1 && kPSD == 1 && kPSD1vsSTS == 1 && kPSD2vsSTS == 1 && kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_b = (TH1F*) t3_centr->GetHistogram();
	//cout << "stat dom " << i+1 << ": " << 100*hPSDvsSTS_b->Integral()/Nentries << endl;

        // sigma from histo
	mean8[i] = hPSDvsSTS_b->GetMean();
	emean8[i] = hPSDvsSTS_b->GetMeanError();
	sigma8[i] = hPSDvsSTS_b->GetRMS();
	esigma8[i] = hPSDvsSTS_b->GetRMSError();

        // stat
	stat_sample += hPSDvsSTS_b->Integral();

        // DO mean c_PSD !!!
	//if (i==0) inc_evt_PSD3vsSTS[i] = hPSDvsSTS_b->Integral();
        //else inc_evt_PSD3vsSTS[i] = inc_evt_PSD3vsSTS[i-1] + hPSDvsSTS_b->Integral();

        hname = "hcomb_methall_c";
	hname += i+1;
        detinfo = "c_comb_methall";
	t3_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kSTS == 1 && kPSD == 1 && kPSD1vsSTS == 1 && kPSD2vsSTS == 1 && kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_c = (TH1F*) t3_centr->GetHistogram();

	inc_evt_comb_methall_it1[i] = hPSDvsSTS_c->GetMean();
        inc_evt_err_comb_methall_it1[i] = hPSDvsSTS_c->GetMeanError();

        //draw
        //c_b_comb_methall->cd(i+1);
	//color_ind = i%4+1;
	//hPSDvsSTS_b->SetLineColor(color_ind);
        //hPSDvsSTS_b->SetLineWidth(3);
	//hPSDvsSTS_b->Draw();
    }
    cout << "stat (selected) sample = " << stat_sample << endl;

    for (int i=dom_cut;i<20;i++)
    {
	mean8[i] = mean8[dom_cut-1];
	emean8[i] = emean8[dom_cut-1];
	sigma8[i] = sigma8[dom_cut-1];
	esigma8[i] = esigma8[dom_cut-1];

        inc_evt_comb_methall_it1[i] = inc_evt_comb_methall_it1[dom_cut-1];
    }

    // iteration 2 (refine average - accounting for different resolutions from the different methods)
    stat_sample=0.;
    for (int i=0;i<dom_cut;i++)
    {
        index1 = "";
	index2 = "";
	index1 += i*step;       
        index2 += (i+1)*step;

	cut = "c_comb_methall > " + index1 + " && c_comb_methall <= " + index2;

	if (i == dom_cut-1)
	{
	    cut = "c_comb_methall > " + index1;
	}
        cout << "cut = " << cut << endl;

        hname = "hcomb_methall_b";
	hname += i+1;
        detinfo = "McEvent.fB";
	t4_centr->Draw(detinfo +">>" + hname + "(170, 0., 17.)", "kSTS == 1 && kPSD == 1 && kPSD1vsSTS == 1 && kPSD2vsSTS == 1 && kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_b = (TH1F*) t4_centr->GetHistogram();
	//cout << "stat dom " << i+1 << ": " << 100*hPSDvsSTS_b->Integral()/Nentries << endl;

        // sigma from histo
	mean9[i] = hPSDvsSTS_b->GetMean();
	emean9[i] = hPSDvsSTS_b->GetMeanError();
	sigma9[i] = hPSDvsSTS_b->GetRMS();
	esigma9[i] = hPSDvsSTS_b->GetRMSError();

        // stat
	stat_sample += hPSDvsSTS_b->Integral();

        // DO mean c_PSD !!!
	//if (i==0) inc_evt_PSD3vsSTS[i] = hPSDvsSTS_b->Integral();
        //else inc_evt_PSD3vsSTS[i] = inc_evt_PSD3vsSTS[i-1] + hPSDvsSTS_b->Integral();

        hname = "hcomb_methall_c";
	hname += i+1;
        detinfo = "c_comb_methall";
	t4_centr->Draw(detinfo +">>" + hname + "(100, 0., 100.)", "kSTS == 1 && kPSD == 1 && kPSD1vsSTS == 1 && kPSD2vsSTS == 1 && kPSD3vsSTS == 1 &&"+cut, "goff");
	hPSDvsSTS_c = (TH1F*) t4_centr->GetHistogram();

	inc_evt_comb_methall_it2[i] = hPSDvsSTS_c->GetMean();
        inc_evt_err_comb_methall_it2[i] = hPSDvsSTS_c->GetMeanError();

        //draw
        c_b_comb_methall->cd(i+1);
	color_ind = i%4+1;
	hPSDvsSTS_b->SetLineColor(color_ind);
        hPSDvsSTS_b->SetLineWidth(3);
	hPSDvsSTS_b->Draw();
    }
    cout << "stat (selected) sample = " << stat_sample << endl;

    for (int i=dom_cut;i<20;i++)
    {
	mean9[i] = mean9[dom_cut-1];
	emean9[i] = emean9[dom_cut-1];
	sigma9[i] = sigma9[dom_cut-1];
	esigma9[i] = esigma9[dom_cut-1];

        inc_evt_comb_methall_it2[i] = inc_evt_comb_methall_it2[dom_cut-1];
    }


    // =================================================================
    // =================================================================
    // --------------------------------------------- Draw STS resolution
    for (int i=0; i<20;i++)
    {
    //    inc_evt_err_STS[i] = (inc_evt_STS[i]/Nentries)*TMath::Sqrt( 1./inc_evt_STS[i] + 1./Nentries);
    //    inc_evt_err_STS[i] *= 100;
    //
    //    inc_evt_STS[i] /= Nentries;
    //    inc_evt_STS[i] *= 100;
    //
	cout << "% most central coll. (/all_evt_STS): " << inc_evt_STS[i] << " +/- " << inc_evt_err_STS[i] << endl;
    }

    for (int i=0; i<20;i++)
    {
	percent_STS[i] = inc_evt_STS[i];
        percent_err_STS[i] = inc_evt_err_STS[i];
    }

    for (int i=0; i<20;i++)
    {
	esigmaovermean0[i] = (sigma0[i]/mean0[i])*TMath::Sqrt(TMath::Power((esigma0[i]/sigma0[i]), 2) + TMath::Power((emean0[i]/mean0[i]), 2));
	sigmaovermean0[i] = sigma0[i]/mean0[i];

    }

    // =========================================================== Draw PSD reso
    for (int i=0; i<20;i++)
    {
	//inc_evt_err_PSD[i] = (inc_evt_PSD[i]/Nentries)*TMath::Sqrt( 1./inc_evt_PSD[i] + 1./Nentries);
	//inc_evt_err_PSD[i] *= 100;

	//inc_evt_PSD[i] /= Nentries;
	//inc_evt_PSD[i] *= 100;

	cout << "% most central coll. (/all_evt_PSD): " << inc_evt_PSD[i] << endl;
    }
    
    for (int i=0; i<20;i++)
    {
	percent_PSD[i] = inc_evt_PSD[i];
        percent_err_PSD[i] = inc_evt_err_PSD[i];
    }

    for (int i=0; i<20;i++)
    {
	esigmaovermean1[i] = (sigma1[i]/mean1[i])*TMath::Sqrt(TMath::Power((esigma1[i]/sigma1[i]), 2) + TMath::Power((emean1[i]/mean1[i]), 2));
	sigmaovermean1[i] = sigma1[i]/mean1[i];
    }

    // =========================================================== Draw PSD vs STS reso
    // sub1
    for (int i=0; i<20;i++)
    {
	//inc_evt_err_PSD1vsSTS[i] = (inc_evt_PSD1vsSTS[i]/Nentries)*TMath::Sqrt( 1./inc_evt_PSD1vsSTS[i] + 1./Nentries);
	//inc_evt_err_PSD1vsSTS[i] *= 100;

	//inc_evt_PSD1vsSTS[i] /= Nentries;
	//inc_evt_PSD1vsSTS[i] *= 100;

	cout << "% most central coll. (/all_evt_PSD1vsSTS): " << inc_evt_PSD1vsSTS[i] << endl;
    }
    
    for (int i=0; i<20;i++)
    {
	percent_PSD1vsSTS[i] = inc_evt_PSD1vsSTS[i];
        percent_err_PSD1vsSTS[i] = inc_evt_err_PSD1vsSTS[i];
    }

    for (int i=0; i<20;i++)
    {
	esigmaovermean2[i] = (sigma2[i]/mean2[i])*TMath::Sqrt(TMath::Power((esigma2[i]/sigma2[i]), 2) + TMath::Power((emean2[i]/mean2[i]), 2));
	sigmaovermean2[i] = sigma2[i]/mean2[i];
    }


    // sub2
    for (int i=0; i<20;i++)
    {
	//inc_evt_err_PSD2vsSTS[i] = (inc_evt_PSD2vsSTS[i]/Nentries)*TMath::Sqrt( 1./inc_evt_PSD2vsSTS[i] + 1./Nentries);
	//inc_evt_err_PSD2vsSTS[i] *= 100;

	//inc_evt_PSD2vsSTS[i] /= Nentries;
	//inc_evt_PSD2vsSTS[i] *= 100;

	cout << "% most central coll. (/all_evt_PSD2vsSTS): " << inc_evt_PSD2vsSTS[i] << endl;
    }
    
    for (int i=0; i<20;i++)
    {
	percent_PSD2vsSTS[i] = inc_evt_PSD2vsSTS[i];
        percent_err_PSD2vsSTS[i] = inc_evt_err_PSD2vsSTS[i];
    }

    for (int i=0; i<20;i++)
    {
	esigmaovermean3[i] = (sigma3[i]/mean3[i])*TMath::Sqrt(TMath::Power((esigma3[i]/sigma3[i]), 2) + TMath::Power((emean3[i]/mean3[i]), 2));
	sigmaovermean3[i] = sigma3[i]/mean3[i];
    }

    // sub3
    for (int i=0; i<20;i++)
    {
	//inc_evt_err_PSD3vsSTS[i] = (inc_evt_PSD3vsSTS[i]/Nentries)*TMath::Sqrt( 1./inc_evt_PSD3vsSTS[i] + 1./Nentries);
	//inc_evt_err_PSD3vsSTS[i] *= 100;

	//inc_evt_PSD3vsSTS[i] /= Nentries;
	//inc_evt_PSD3vsSTS[i] *= 100;

	cout << "% most central coll. (/all_evt_PSD3vsSTS): " << inc_evt_PSD3vsSTS[i] << endl;
    }
    
    for (int i=0; i<20;i++)
    {
	percent_PSD3vsSTS[i] = inc_evt_PSD3vsSTS[i];
        percent_err_PSD3vsSTS[i] = inc_evt_err_PSD3vsSTS[i];
    }

    for (int i=0; i<20;i++)
    {
	esigmaovermean4[i] = (sigma4[i]/mean4[i])*TMath::Sqrt(TMath::Power((esigma4[i]/sigma4[i]), 2) + TMath::Power((emean4[i]/mean4[i]), 2));
	sigmaovermean4[i] = sigma4[i]/mean4[i];
    }

    // Combination methods
    // - 1 + 2 -
    for (int i=0; i<20;i++)
    {
	percent_comb_meth1_2[i] = inc_evt_comb_meth1_2[i];
        percent_err_comb_meth1_2[i] = inc_evt_err_comb_meth1_2[i];
    }

    for (int i=0; i<20;i++)
    {
	esigmaovermean5[i] = (sigma5[i]/mean5[i])*TMath::Sqrt(TMath::Power((esigma5[i]/sigma5[i]), 2) + TMath::Power((emean5[i]/mean5[i]), 2));
	sigmaovermean5[i] = sigma5[i]/mean5[i];
    }

    // - 3 + 4 + 5 -
    for (int i=0; i<20;i++)
    {
	percent_comb_meth3_4_5[i] = inc_evt_comb_meth3_4_5[i];
        percent_err_comb_meth3_4_5[i] = inc_evt_err_comb_meth3_4_5[i];
    }

    for (int i=0; i<20;i++)
    {
	esigmaovermean6[i] = (sigma6[i]/mean6[i])*TMath::Sqrt(TMath::Power((esigma6[i]/sigma6[i]), 2) + TMath::Power((emean6[i]/mean6[i]), 2));
	sigmaovermean6[i] = sigma6[i]/mean6[i];
    }

    // - 1 + 2 + 3 + 4 + 5 -
    // iteration 0
    for (int i=0; i<20;i++)
    {
	percent_comb_methall_it0[i] = inc_evt_comb_methall_it0[i];
        percent_err_comb_methall_it0[i] = inc_evt_err_comb_methall_it0[i];
    }

    for (int i=0; i<20;i++)
    {
	esigmaovermean7[i] = (sigma7[i]/mean7[i])*TMath::Sqrt(TMath::Power((esigma7[i]/sigma7[i]), 2) + TMath::Power((emean7[i]/mean7[i]), 2));
	sigmaovermean7[i] = sigma7[i]/mean7[i];
    }
    // iteration 1
    for (int i=0; i<20;i++)
    {
	percent_comb_methall_it1[i] = inc_evt_comb_methall_it1[i];
        percent_err_comb_methall_it1[i] = inc_evt_err_comb_methall_it1[i];
    }

    for (int i=0; i<20;i++)
    {
	esigmaovermean8[i] = (sigma8[i]/mean8[i])*TMath::Sqrt(TMath::Power((esigma8[i]/sigma8[i]), 2) + TMath::Power((emean8[i]/mean8[i]), 2));
	sigmaovermean8[i] = sigma8[i]/mean8[i];
    }
    // iteration 2
    for (int i=0; i<20;i++)
    {
	percent_comb_methall_it2[i] = inc_evt_comb_methall_it2[i];
        percent_err_comb_methall_it2[i] = inc_evt_err_comb_methall_it2[i];
    }

    for (int i=0; i<20;i++)
    {
	esigmaovermean9[i] = (sigma9[i]/mean9[i])*TMath::Sqrt(TMath::Power((esigma9[i]/sigma9[i]), 2) + TMath::Power((emean9[i]/mean9[i]), 2));
	sigmaovermean9[i] = sigma9[i]/mean9[i];
    }

    // ==================================================== Final Graphs

    TCanvas* c_reso = new TCanvas("c_reso"," sigma(b)/<b> vs % centrality ",200,10,700,500);
    c_reso->SetFillColor(0);
    c_reso->SetGrid();

    // draw a frame to define the range
    TH1F *hr2 = c_reso->DrawFrame(0.,0.,100.,1.);

    //hr2->SetXTitle("<b> [fm]");
    hr2->SetXTitle("% most central collisions");
    hr2->GetXaxis()->CenterTitle();

    hr2->SetYTitle("#sigma_{b} / <b>");
    hr2->GetYaxis()->CenterTitle();

    Int_t n = 20;

    gr10 = new TGraphErrors(n, percent_STS, sigmaovermean0, percent_err_STS, esigmaovermean0);
    gr10->SetName("gr10");
    gr10->SetMarkerColor(kBlack);
    gr10->SetLineColor(kBlack);
    gr10->SetMarkerStyle(25);
    gr10->Draw("P");

    gr11 = new TGraphErrors(n, percent_PSD, sigmaovermean1, percent_err_PSD, esigmaovermean1);
    gr11->SetName("gr11");
    gr11->SetMarkerColor(kRed);
    gr11->SetLineColor(kRed);
    gr11->SetMarkerStyle(25);
    gr11->Draw("P");

    gr12 = new TGraphErrors(n, percent_PSD1vsSTS, sigmaovermean2, percent_err_PSD1vsSTS, esigmaovermean2);
    gr12->SetName("gr12");
    gr12->SetMarkerColor(kGreen);
    gr12->SetLineColor(kGreen);
    gr12->SetMarkerStyle(25);
    gr12->Draw("P");

    gr13 = new TGraphErrors(n, percent_PSD2vsSTS, sigmaovermean3, percent_err_PSD2vsSTS, esigmaovermean3);
    gr13->SetName("gr13");
    gr13->SetMarkerColor(kBlue);
    gr13->SetLineColor(kBlue);
    gr13->SetMarkerStyle(25);
    gr13->Draw("P");

    gr14 = new TGraphErrors(n, percent_PSD3vsSTS, sigmaovermean4, percent_err_PSD3vsSTS, esigmaovermean4);
    gr14->SetName("gr14");
    gr14->SetMarkerColor(kMagenta);
    gr14->SetLineColor(kMagenta);
    gr14->SetMarkerStyle(25);
    gr14->Draw("P");

    gr15 = new TGraphErrors(n, percent_comb_meth1_2, sigmaovermean5, percent_err_comb_meth1_2, esigmaovermean5);
    gr15->SetName("gr15");
    gr15->SetMarkerColor(kGreen);
    gr15->SetLineColor(kGreen);
    gr15->SetMarkerStyle(25);
    //gr15->Draw("P");

    gr16 = new TGraphErrors(n, percent_comb_meth3_4_5, sigmaovermean6, percent_err_comb_meth3_4_5, esigmaovermean6);
    gr16->SetName("gr16");
    gr16->SetMarkerColor(kMagenta);
    gr16->SetLineColor(kMagenta);
    gr16->SetMarkerStyle(25);
    //gr16->Draw("P");

    gr17 = new TGraphErrors(n, percent_comb_methall_it0, sigmaovermean7, percent_err_comb_methall_it0, esigmaovermean7);
    gr17->SetName("gr17");
    gr17->SetMarkerColor(kGreen);
    gr17->SetLineColor(kGreen);
    gr17->SetMarkerStyle(25);
    //gr17->Draw("P");

    gr18 = new TGraphErrors(n, percent_comb_methall_it1, sigmaovermean8, percent_err_comb_methall_it1, esigmaovermean8);
    gr18->SetName("gr18");
    gr18->SetMarkerColor(kBlue);
    gr18->SetLineColor(kBlue);
    gr18->SetMarkerStyle(25);
    //gr18->Draw("P");

    gr19 = new TGraphErrors(n, percent_comb_methall_it2, sigmaovermean9, percent_err_comb_methall_it2, esigmaovermean9);
    gr19->SetName("gr19");
    gr19->SetMarkerColor(kMagenta);
    gr19->SetLineColor(kMagenta);
    gr19->SetMarkerStyle(25);
    //gr19->Draw("P");

    TLegend *leg = new TLegend(0.3,0.1,0.5,0.4);
    leg->SetHeader("Method");
    leg->AddEntry("gr10","STS","p");
    leg->AddEntry("gr11","PSD","p");
    leg->AddEntry("gr12","PSD sub1 vs STS","p");
    leg->AddEntry("gr13","PSD sub2 vs STS","p");
    leg->AddEntry("gr14","PSD sub3 vs STS","p");
    //leg->AddEntry("gr15","comb. meth. 1+2","p");
    //leg->AddEntry("gr16","comb. meth. 3+4+5","p");
    //leg->AddEntry("gr17","comb. all meth. - it. 0","p");
    //leg->AddEntry("gr18","comb. all meth. - it. 1","p");
    //leg->AddEntry("gr19","comb. all meth. - it. 2","p");

    leg->Draw();

    TCanvas* c_mean_reso = new TCanvas("c_mean_reso"," <b> +/- sigma(b) vs % centrality ",200,10,700,500);
    c_mean_reso->SetFillColor(0);
    c_mean_reso->SetGrid();

    // draw a frame to define the range
    TH1F *hr3 = c_mean_reso->DrawFrame(0.,0.,100.,20.);

    hr3->SetXTitle("% most central collisions");
    hr3->GetXaxis()->CenterTitle();

    hr3->SetYTitle("<b> +/- #sigma_{b}");
    hr3->GetYaxis()->CenterTitle();

    Int_t n = 20;

    gr20 = new TGraphErrors(n, percent_STS, mean0, percent_err_STS, sigma0);
    gr20->SetName("gr20");
    gr20->SetMarkerColor(kBlack);
    gr20->SetLineColor(kBlack);
    gr20->SetMarkerStyle(25);
    gr20->Draw("P");

    gr21 = new TGraphErrors(n, percent_PSD, mean1, percent_err_PSD, sigma1);
    gr21->SetName("gr21");
    gr21->SetMarkerColor(kRed);
    gr21->SetLineColor(kRed);
    gr21->SetMarkerStyle(25);
    gr21->Draw("P");

    gr22 = new TGraphErrors(n, percent_PSD1vsSTS, mean2, percent_err_PSD1vsSTS, sigma2);
    gr22->SetName("gr22");
    gr22->SetMarkerColor(kGreen);
    gr22->SetLineColor(kGreen);
    gr22->SetMarkerStyle(25);
    gr22->Draw("P");

    gr23 = new TGraphErrors(n, percent_PSD2vsSTS, mean3, percent_err_PSD2vsSTS, sigma3);
    gr23->SetName("gr23");
    gr23->SetMarkerColor(kBlue);
    gr23->SetLineColor(kBlue);
    gr23->SetMarkerStyle(25);
    gr23->Draw("P");

    gr24 = new TGraphErrors(n, percent_PSD3vsSTS, mean4, percent_err_PSD3vsSTS, sigma4);
    gr24->SetName("gr24");
    gr24->SetMarkerColor(kMagenta);
    gr24->SetLineColor(kMagenta);
    gr24->SetMarkerStyle(25);
    gr24->Draw("P");

    gr25 = new TGraphErrors(n, percent_comb_meth1_2, mean5, percent_err_comb_meth1_2, sigma5);
    gr25->SetName("gr25");
    gr25->SetMarkerColor(kGreen);
    gr25->SetLineColor(kGreen);
    gr25->SetMarkerStyle(25);
    //gr25->Draw("P");

    gr26 = new TGraphErrors(n, percent_comb_meth3_4_5, mean6, percent_err_comb_meth3_4_5, sigma6);
    gr26->SetName("gr26");
    gr26->SetMarkerColor(kMagenta);
    gr26->SetLineColor(kMagenta);
    gr26->SetMarkerStyle(25);
    //gr26->Draw("P");

    gr27 = new TGraphErrors(n, percent_comb_methall_it0, mean7, percent_err_comb_methall_it0, sigma7);
    gr27->SetName("gr27");
    gr27->SetMarkerColor(kGreen);
    gr27->SetLineColor(kGreen);
    gr27->SetMarkerStyle(25);
    //gr27->Draw("P");

    gr28 = new TGraphErrors(n, percent_comb_methall_it1, mean8, percent_err_comb_methall_it1, sigma8);
    gr28->SetName("gr28");
    gr28->SetMarkerColor(kBlue);
    gr28->SetLineColor(kBlue);
    gr28->SetMarkerStyle(25);
    //gr28->Draw("P");

    gr29 = new TGraphErrors(n, percent_comb_methall_it2, mean9, percent_err_comb_methall_it2, sigma9);
    gr29->SetName("gr29");
    gr29->SetMarkerColor(kMagenta);
    gr29->SetLineColor(kMagenta);
    gr29->SetMarkerStyle(25);
    //gr29->Draw("P");

    TLegend *leg2 = new TLegend(0.3,0.1,0.5,0.4);
    leg2->SetHeader("Method");
    leg2->AddEntry("gr20","STS","p");
    leg2->AddEntry("gr21","PSD","p");
    leg2->AddEntry("gr22","PSD sub1 vs STS","p");
    leg2->AddEntry("gr23","PSD sub2 vs STS","p");
    leg2->AddEntry("gr24","PSD sub3 vs STS","p");
    //leg2->AddEntry("gr25","comb. meth. 1+2","p");
    //leg2->AddEntry("gr26","comb. meth. 3+4+5","p");
    //leg2->AddEntry("gr27","comb. all meth. - it. 0","p");
    //leg2->AddEntry("gr28","comb. all meth. - it. 1","p");
    //leg2->AddEntry("gr29","comb. all meth. - it. 2","p");

    leg2->Draw();

    //break;

    //eps
    /*cDrawCut_STS->SaveAs("plot/centrality_310714/STS_domDef_au2au.eps");
    c_b_STS->SaveAs("plot/centrality_310714/STS_domBdistr_au2au.eps");

    cDrawCut_PSD->SaveAs("plot/centrality_310714/PSD_domDef_au2au.eps");
    c_b_PSD->SaveAs("plot/centrality_310714/PSD_domBdistr_au2au.eps");

    cDrawCut_sub1vsM->SaveAs("plot/centrality_310714/PSD1vsSTS_domDef_au2au.eps");
    c_b_PSD_sub1vsM->SaveAs("plot/centrality_310714/PSD1vsSTS_domBdistr_au2au.eps");

    cDrawCut_sub2vsM->SaveAs("plot/centrality_310714/PSD2vsSTS_domDef_au2au.eps");
    c_b_PSD_sub2vsM->SaveAs("plot/centrality_310714/PSD2vsSTS_domBdistr_au2au.eps");

    cDrawCut_sub3vsM->SaveAs("plot/centrality_310714/PSD3vsSTS_domDef_au2au.eps");
    c_b_PSD_sub3vsM->SaveAs("plot/centrality_310714/PSD3vsSTS_domBdistr_au2au.eps");
    */
    c_reso->SaveAs("plot/centrality_310714/STS_PSD_PSD123vsSTS_sigmaovermean_au2au.eps");
    c_mean_reso->SaveAs("plot/centrality_310714/STS_PSD_PSD123vsSTS_meanSigma_au2au.eps");

    //c_reso->SaveAs("plot/centrality_310714/STS_PSD_comball_sigmaovermean_au2au.eps");
    //c_mean_reso->SaveAs("plot/centrality_310714/STS_PSD_comball_meanSigma_au2au.eps");

    //macro .C
    /*cDrawCut_STS->SaveAs("plot/centrality_310714/STS_domDef_au2au.C");
    c_b_STS->SaveAs("plot/centrality_310714/STS_domBdistr_au2au.C");

    cDrawCut_PSD->SaveAs("plot/centrality_310714/PSD_domDef_au2au.C");
    c_b_PSD->SaveAs("plot/centrality_310714/PSD_domBdistr_au2au.C");

    cDrawCut_sub1vsM->SaveAs("plot/centrality_310714/PSD1vsSTS_domDef_au2au.C");
    c_b_PSD_sub1vsM->SaveAs("plot/centrality_310714/PSD1vsSTS_domBdistr_au2au.C");

    cDrawCut_sub2vsM->SaveAs("plot/centrality_310714/PSD2vsSTS_domDef_au2au.C");
    c_b_PSD_sub2vsM->SaveAs("plot/centrality_310714/PSD2vsSTS_domBdistr_au2au.C");

    cDrawCut_sub3vsM->SaveAs("plot/centrality_310714/PSD3vsSTS_domDef_au2au.C");
    c_b_PSD_sub3vsM->SaveAs("plot/centrality_310714/PSD3vsSTS_domBdistr_au2au.C");
    */
    c_reso->SaveAs("plot/centrality_310714/STS_PSD_PSD123vsSTS_sigmaovermean_au2au.C");
    c_mean_reso->SaveAs("plot/centrality_310714/STS_PSD_PSD123vsSTS_meanSigma_au2au.C");

    //c_reso->SaveAs("plot/centrality_310714/STS_PSD_comball_sigmaovermean_au2au.C");
    //c_mean_reso->SaveAs("plot/centrality_310714/STS_PSD_comball_meanSigma_au2au.C");

    //gif
    /*cDrawCut_STS->SaveAs("plot/centrality_310714/STS_domDef_au2au.gif");
    c_b_STS->SaveAs("plot/centrality_310714/STS_domBdistr_au2au.gif");

    cDrawCut_PSD->SaveAs("plot/centrality_310714/PSD_domDef_au2au.gif");
    c_b_PSD->SaveAs("plot/centrality_310714/PSD_domBdistr_au2au.gif");

    cDrawCut_sub1vsM->SaveAs("plot/centrality_310714/PSD1vsSTS_domDef_au2au.gif");
    c_b_PSD_sub1vsM->SaveAs("plot/centrality_310714/PSD1vsSTS_domBdistr_au2au.gif");

    cDrawCut_sub2vsM->SaveAs("plot/centrality_310714/PSD2vsSTS_domDef_au2au.gif");
    c_b_PSD_sub2vsM->SaveAs("plot/centrality_310714/PSD2vsSTS_domBdistr_au2au.gif");

    cDrawCut_sub3vsM->SaveAs("plot/centrality_310714/PSD3vsSTS_domDef_au2au.gif");
    c_b_PSD_sub3vsM->SaveAs("plot/centrality_310714/PSD3vsSTS_domBdistr_au2au.gif");
    */
    c_reso->SaveAs("plot/centrality_310714/STS_PSD_PSD123vsSTS_sigmaovermean_au2au.gif");
    c_mean_reso->SaveAs("plot/centrality_310714/STS_PSD_PSD123vsSTS_meanSigma_au2au.gif");

    //c_reso->SaveAs("plot/centrality_310714/STS_PSD_comball_sigmaovermean_au2au.gif");
    //c_mean_reso->SaveAs("plot/centrality_310714/STS_PSD_comball_meanSigma_au2au.gif");

}