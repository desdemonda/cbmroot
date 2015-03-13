// -------------------------------------------------------------------------
// ----- Macro to draw the reaction plane resolution versus % most centrality (based on STS multiplicity)

graph_RP_cos_vsCentrality()
{
    // files to CHANGE - here: 7 different energies

    TFile *f1 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au2au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8_Qcorr_FlatBin_FixStep.root","R");
    TFile *f2 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au4au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8_Qcorr_FlatBin_FixStep.root","R");
    TFile *f3 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au6au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8_Qcorr_FlatBin_FixStep.root","R");
    TFile *f4 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au10au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/8m/ana/ana_10Kevt_RECOtracks_STScutY_0.8_Qcorr_FlatBin_FixStep.root","R");
    TFile *f5 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au15au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/15m/ana/ana_10Kevt_RECOtracks_STScutY_0.8_Qcorr_FlatBin_FixStep.root","R");
    TFile *f6 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au25au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/15m/ana/ana_10Kevt_RECOtracks_STScutY_0.8_Qcorr_FlatBin_FixStep.root","R");
    TFile *f7 = new TFile("/hera/cbm/users/sseddiki/auau_TDR/shield/au35au/sts_13d_PSDhole6cm_44mods_beampipe/v15/flat_detinfo/15m/ana/ana_10Kevt_RECOtracks_STScutY_0.8_Qcorr_FlatBin_FixStep.root","R");

    // TO DO: error <% most central.>

    //===========================================
    
    TTree* t[7];

    t[0] = (TTree*) f1->Get("cbmsim");
    t[1] = (TTree*) f2->Get("cbmsim");
    t[2] = (TTree*) f3->Get("cbmsim");
    t[3] = (TTree*) f4->Get("cbmsim");
    t[4] = (TTree*) f5->Get("cbmsim");
    t[5] = (TTree*) f6->Get("cbmsim");
    t[6] = (TTree*) f7->Get("cbmsim");

    const int n = 10;

    Float_t sigma1[n], esigma1[n];
    Float_t sigma2[n], esigma2[n];
    Float_t sigma3[n], esigma3[n];
    Float_t sigma4[n], esigma4[n];
    Float_t sigma5[n], esigma5[n];
    Float_t sigma6[n], esigma6[n];
    Float_t sigma7[n], esigma7[n];

    Float_t smear1[n], esmear1[n];
    Float_t smear2[n], esmear2[n];
    Float_t smear3[n], esmear3[n];
    Float_t smear4[n], esmear4[n];
    Float_t smear5[n], esmear5[n];
    Float_t smear6[n], esmear6[n];
    Float_t smear7[n], esmear7[n];

    Float_t sigma[7][n], esigma[7][n];
    Float_t smear[7][n], esmear[7][n];

    //TString sdelta="(PsdEvent.fdeltaEP)* 180/TMath::Pi()";
    //TString range = "(100, -80, 80)";
    TString sdelta="TMath::Cos( 1 * (PsdEvent.fEP_RECO - McEvent.fRP ))";
    TString range = "(1000, -1, 1)";
    TString scutvariable="StsEvent.fmult";//"McEvent.fB";
    TString scut="";
    TString num;

    TH1F* h[n];
    TH1F* hh[n];
    TF1* funct[n];

    TH1F* hmult[7];
    TAxis *Xaxis;
    int NXbin=0;
    double Nentries=0.;
    double step = 1./n;
    cout << "step = " << step << endl;
    int j=0;
    double entry=0.;

    // == centrality definition
    double centr[7][n-1];
    TString scentr[7][n-1];

    // MC b - fixed step
    //for (int i=0;i<n-1;i++) centr[i] = 2*(i+1);
    //for (int i=0;i<n-1;i++) cout << "centr:" << centr[i] << endl;

    // detector info
    TString maxmult="";
    for (int k=0;k<7;k++)
    {
        num="";
	num+=k;

	t[k]->Draw("StsEvent.fmult>>mult_tmp"+num,"","");
	hmult[k] = (TH1F*) t[k]->GetHistogram();

        maxmult="";
        maxmult+=hmult[k]->GetBinCenter(hmult[k]->FindLastBinAbove())+hmult[k]->GetBinWidth(0)/2.;

        t[k]->Draw("StsEvent.fmult>>mult"+num+"(1000, 0, "+maxmult+")","","");
	hmult[k] = (TH1F*) t[k]->GetHistogram();

	Xaxis = hmult[k]->GetXaxis();
	NXbin = Xaxis->GetLast();
	Nentries = hmult[k]->Integral();
	cout << "===" << endl;

	j=0;
	for (int i=0;i<n-1;i++) centr[k][i] = 0.;
	entry=0.;
	for(int i=NXbin;i>0;i--)
	{
	    entry += hmult[k]->GetBinContent(i);
	    if (entry >= ((j+1)*step*Nentries) && centr[k][j] == 0.)
	    {
		centr[k][j] = hmult[k]->GetBinCenter(i);
		j++;
	    }
	}
	for (int i=0;i<n-1;i++) scentr[k][i] = "";
	for (int i=0;i<n-1;i++) scentr[k][i] += centr[k][i];

	//for (int i=0;i<n-1;i++) cout << "centr:" << scentr[k][i] << endl;
	//for (int i=0;i<n-1;i++) cout << "% most central:" << 100*hmult[k]->Integral(hmult[k]->FindBin(centr[k][i]), hmult[k]->GetNbinsX())/Nentries << endl;
    }

    //TCanvas* c2 = new TCanvas("c2"," test ",200,10,700,500);
    //gStyle->SetOptStat(0);
    //c2->Divide(4, 2);

    double meantmp=0.;
    //===========================================
    for (int k=0;k<7;k++)
    {
	for (int i=0;i<n;i++)
	{
	    num="";
            num+=k;
	    num+=i;

            // MC b
	    /*if (i==0) scut = scutvariable+"<"+ scentr[i];
	    else
	    {
		if (i==n-1) scut = scutvariable+">="+ scentr[i-1];
		else
		{
		    scut = scutvariable+">="+ scentr[i-1]+"&&"+scutvariable+"<"+ scentr[i];
		}
	    }*/

            // STS mult
            if (i==0) scut = scutvariable+">="+ scentr[k][i];
	    else
	    {
		if (i==n-1) scut = scutvariable+"<"+ scentr[k][i-1];
		else
		{
		    scut = scutvariable+"<"+ scentr[k][i-1]+"&&"+scutvariable+">="+ scentr[k][i];
		}
	    }
	    //cout << "scut:" << scut << endl;

	    t[k]->Draw(sdelta+" >>h"+num+range,scut,"goff");
	    h[i] = (TH1F*) t[k]->GetHistogram();

            //if (k==0) c2->cd(i+1);
	    t[k]->Draw(scutvariable+" >>hh"+num,scut,"goff");
	    hh[i] = (TH1F*) t[k]->GetHistogram();
            meantmp=hh[i]->GetMean();

	    smear[k][i] = 100*hmult[k]->Integral(hmult[k]->FindBin(meantmp), hmult[k]->GetNbinsX())/hmult[k]->Integral();
            //esmear[k][i] = hh[i]->GetMeanError();
            esmear[k][i]=0.;

	    //h[i]->Fit("gaus","0, Q","",-80, 80);
	    //funct[i] = (TF1*) h[i]->GetFunction("gaus");
	    //sigma[k][i] = funct[i]->GetParameter(2);
	    //esigma[k][i] = funct[i]->GetParError(2);

            sigma[k][i] = h[i]->GetMean();
	    esigma[k][i] = h[i]->GetMeanError();
	}
    }

    for (int i=0;i<n;i++) sigma1[i]=sigma[0][i];
    for (int i=0;i<n;i++) sigma2[i]=sigma[1][i];
    for (int i=0;i<n;i++) sigma3[i]=sigma[2][i];
    for (int i=0;i<n;i++) sigma4[i]=sigma[3][i];
    for (int i=0;i<n;i++) sigma5[i]=sigma[4][i];
    for (int i=0;i<n;i++) sigma6[i]=sigma[5][i];
    for (int i=0;i<n;i++) sigma7[i]=sigma[6][i];

    for (int i=0;i<n;i++) esigma1[i]=esigma[0][i];
    for (int i=0;i<n;i++) esigma2[i]=esigma[1][i];
    for (int i=0;i<n;i++) esigma3[i]=esigma[2][i];
    for (int i=0;i<n;i++) esigma4[i]=esigma[3][i];
    for (int i=0;i<n;i++) esigma5[i]=esigma[4][i];
    for (int i=0;i<n;i++) esigma6[i]=esigma[5][i];
    for (int i=0;i<n;i++) esigma7[i]=esigma[6][i];

    for (int i=0;i<n;i++) smear1[i]=smear[0][i];
    for (int i=0;i<n;i++) smear2[i]=smear[1][i];
    for (int i=0;i<n;i++) smear3[i]=smear[2][i];
    for (int i=0;i<n;i++) smear4[i]=smear[3][i];
    for (int i=0;i<n;i++) smear5[i]=smear[4][i];
    for (int i=0;i<n;i++) smear6[i]=smear[5][i];
    for (int i=0;i<n;i++) smear7[i]=smear[6][i];

    for (int i=0;i<n;i++) esmear1[i]=esmear[0][i];
    for (int i=0;i<n;i++) esmear2[i]=esmear[1][i];
    for (int i=0;i<n;i++) esmear3[i]=esmear[2][i];
    for (int i=0;i<n;i++) esmear4[i]=esmear[3][i];
    for (int i=0;i<n;i++) esmear5[i]=esmear[4][i];
    for (int i=0;i<n;i++) esmear6[i]=esmear[5][i];
    for (int i=0;i<n;i++) esmear7[i]=esmear[6][i];

    //===========================================

    gROOT->Reset();
    c1 = new TCanvas("c1"," sigmaRP vs b ",200,10,700,500);
    c1->SetFillColor(0);
    c1->SetGrid();

    // draw a frame to define the range
    TH1F *hr = c1->DrawFrame(0,0,100,1);

    //hr->SetXTitle("Collision (MC) impact parameter [fm]");
    hr->SetXTitle("Most central collisions [%]");
    hr->GetXaxis()->CenterTitle();

    //hr->SetYTitle(" #sigma (#Psi_{1} - #Psi_{MC}) [degree]");
    hr->SetYTitle(" <Cos(#Psi_{1} - #Psi_{MC})> ");
    hr->GetYaxis()->CenterTitle();

    gr1 = new TGraphErrors(n,smear1,sigma1,esmear1,esigma1);
    gr1->SetName("gr1");
    gr1->SetMarkerColor(1);
    gr1->SetMarkerStyle(20);
    gr1->SetLineColor(1);
    gr1->Draw("P");

    gr2 = new TGraphErrors(n,smear2,sigma2,esmear2,esigma2);
    gr2->SetName("gr2");
    gr2->SetMarkerColor(2);
    gr2->SetMarkerStyle(21);
    gr2->SetLineColor(2);
    gr2->Draw("P");

    gr3 = new TGraphErrors(n,smear3,sigma3,esmear3,esigma3);
    gr3->SetName("gr3");
    gr3->SetMarkerColor(3);
    gr3->SetMarkerStyle(22);
    gr3->SetLineColor(3);
    gr3->Draw("P");

    gr4 = new TGraphErrors(n,smear4,sigma4,esmear4,esigma4);
    gr4->SetName("gr4");
    gr4->SetMarkerColor(4);
    gr4->SetMarkerStyle(23);
    gr4->SetLineColor(4);
    gr4->Draw("P");

    gr5 = new TGraphErrors(n,smear5,sigma5,esmear5,esigma5);
    gr5->SetName("gr5");
    gr5->SetMarkerColor(5);
    gr5->SetMarkerStyle(24);
    gr5->SetLineColor(5);
    gr5->Draw("P");

    gr6 = new TGraphErrors(n,smear6,sigma6,esmear6,esigma6);
    gr6->SetName("gr6");
    gr6->SetMarkerColor(6);
    gr6->SetMarkerStyle(25);
    gr6->SetLineColor(6);
    gr6->Draw("P");

    gr7 = new TGraphErrors(n,smear7,sigma7,esmear7,esigma7);
    gr7->SetName("gr7");
    gr7->SetMarkerColor(7);
    gr7->SetMarkerStyle(26);
    gr7->SetLineColor(7);
    gr7->Draw("P");

    TLegend *leg = new TLegend(0.3,0.1,0.5,0.4);
    leg->SetHeader("Beam kinetic energy [AGeV]");
    
    leg->AddEntry("gr1","2","p");                        //field factor = 0.632
    leg->AddEntry("gr2","4","p");                       //field factor = 1
    leg->AddEntry("gr3","6","p");
    leg->AddEntry("gr4","10","p");
    leg->AddEntry("gr5","15","p");
    leg->AddEntry("gr6","25","p");
    leg->AddEntry("gr7","35","p");
    leg->Draw();

    break;

    c1->SaveAs("plot/EP/cosRP_vsPercMostCentr_STSmult_afterflatdetinfo_FixStep_2_4_6_10_15_25_35AGeV_PSD.C");
    c1->SaveAs("plot/EP/cosRP_vsPercMostCentr_STSmult_afterflatdetinfo_FixStep_2_4_6_10_15_25_35AGeV_PSD.eps");
    c1->SaveAs("plot/EP/cosRP_vsPercMostCentr_STSmult_afterflatdetinfo_FixStep_2_4_6_10_15_25_35AGeV_PSD.gif");

//t->SetTextColor(kGreen);
//t->DrawLatex(0.5,0.760,"star : UrQMD collisions + #delta electrons + digitizer");
//t->DrawLatex(0.60,0.720,"Digitizer");

//t->SetTextColor(kBlack);
//t->DrawLatex(0.5,0.860,"Circular absorber, FieldMuonMagnet + 1T");


   /*
t->SetTextColor(kRed);
   t->DrawLatex(0.44,0.840,"thickness = 800 #mum");

t->SetTextColor(42);
   t->DrawLatex(0.66,0.920,"thickness = 1000 #mum");*/
}
