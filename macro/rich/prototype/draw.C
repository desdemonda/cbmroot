#include "/misc/kresan/tools/drawStyle.C"


void draw_points(Int_t nev = 1)
{
    TPstyle();


    LoadLibs();


    TString inFileName = "/d/cbm02/kresan/rich_prot/may11/epi.standard.eve.mc.root";
    TFile *inFile = new TFile(inFileName);
    TTree *cbmsim = (TTree*) inFile->Get("cbmsim");


    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.points.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    TH2F *h1 = new TH2F("h1", "", 300, -15., 15., 300, 20., 50.);
    Style(h1, "x (cm)", "y (cm)");
    cbmsim->Draw("RichPoint.fY:RichPoint.fX>>h1", "", "", nev, 0);
    h1->Scale(1./(Double_t)nev);
    h1->Draw("COL");
    ps1->Close();
}



void draw_theta_beam(Int_t nev = 1)
{
    TPstyle();


    LoadLibs();


    TString inFileName = "/d/cbm02/kresan/rich_prot/may11/epi.standard.mc.root";
    TFile *inFile = new TFile(inFileName);
    TClonesArray *tracks = new TClonesArray("CbmMCTrack");
    TTree *cbmsim = (TTree*) inFile->Get("cbmsim");
    cbmsim->SetBranchAddress("MCTrack", &tracks);
    cbmsim->SetBranchStatus("*", 0);
    cbmsim->SetBranchStatus("MCTrack.fMotherId", 1);
    cbmsim->SetBranchStatus("MCTrack.fPx", 1);
    cbmsim->SetBranchStatus("MCTrack.fPy", 1);
    cbmsim->SetBranchStatus("MCTrack.fPz", 1);
    Int_t nTracks;
    CbmMCTrack *track;
    Int_t motherId;
    Float_t px, py, pz, pt;
    Float_t theta, phi;


    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.theta.beam.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    TH2F *h1 = new TH2F("h1", "", 100, -100., 100., 100, 0., 180.);
    Style(h1, "#theta (mrad)", "#phi (deg)");
    for(Int_t iev = 0; iev < nev; iev++) {
	cbmsim->GetEntry(iev);
	nTracks = tracks->GetEntriesFast();
	for(Int_t i = 0; i < nTracks; i++) {
	    track = (CbmMCTrack*) tracks->At(i);
	    if(NULL == track) continue;
	    motherId = track->GetMotherId();
	    if(-1 != motherId) continue;
            px = track->GetPx();
            py = track->GetPy();
	    pz = track->GetPz();
	    pt = TMath::Sqrt(px*px + py*py);
	    theta = TMath::ATan2(pt, pz)*1e3;
	    phi = TMath::ATan2(py, px)*TMath::RadToDeg();
	    if(phi < 0.) {
		phi += 180.;
                theta *= -1.;
	    }
            h1->Fill(theta, phi);
	}
        if(0==(iev%1000)) cout << iev << endl;
    }
    h1->Draw("col");
    ps1->Close();
}



void draw_hits(Int_t nev = 1)
{
    TPstyle();


    LoadLibs();


    TString inFileName = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile = new TFile(inFileName);
    TTree *cbmsim = (TTree*) inFile->Get("cbmsim");


    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.hits.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    TH2F *h1 = new TH2F("h1", "", 300, -15., 15., 300, 20., 50.);
    Style(h1, "x (cm)", "y (cm)");
    cbmsim->Draw("RichHit.fY:RichHit.fX>>h1", "", "", nev, 0);
    h1->Scale(1./(Double_t)nev);
    h1->Draw("col");
    ps1->Close();
}



void draw_hits_rings(Int_t i1 = 0, Int_t i2 = 100)
{
    TPstyle();


    LoadLibs();


    TString inFileName1 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.mc.root";
    TString inFileName2 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile1 = new TFile(inFileName1);
    TFile *inFile2 = new TFile(inFileName2);
    TTree *cbmsim1 = (TTree*) inFile1->Get("cbmsim");
    TTree *cbmsim2 = (TTree*) inFile2->Get("cbmsim");
    TClonesArray *tracks = new TClonesArray("CbmMCTrack");
    TClonesArray *rings = new TClonesArray("CbmRichRing");
    TClonesArray *hits = new TClonesArray("CbmRichHit");
    TClonesArray *projs = new TClonesArray("FairTrackParam");
    Int_t nr;
    cbmsim1->SetBranchAddress("MCTrack", &tracks);
    cbmsim2->SetBranchAddress("RichRing", &rings);
    cbmsim2->SetBranchAddress("RichHit", &hits);
    cbmsim2->SetBranchAddress("RichProjection", &projs);
    char strPsFileName[1000];
    TPostScript *ps1;
    Int_t nTracks;
    CbmMCTrack *track;
    Int_t pdg;
    Float_t p;
    CbmRichRing *ring;
    Float_t a, b;
    Int_t nhits;
    UShort_t hitIndex;
    CbmRichHit *hit;
    Float_t x, y;
    FairTrackParam *proj;


    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    TH2F *h1 = new TH2F("h1", "", 300, -15., 15., 300, 5., 35.);
    for(Int_t i = i1; i < i2; i++) {
	sprintf(strPsFileName, "plots/events/rich.prot.ring.%d.eps", i);
	ps1 = new TPostScript(strPsFileName, -113);
        ps1->Range(16, 16);
	cbmsim1->GetEntry(i);
	cbmsim2->GetEntry(i);
	nTracks = tracks->GetEntriesFast();
	for(Int_t it = 0; it < nTracks; it++) {
	    track = (CbmMCTrack*) tracks->At(it);
	    if(-1 != track->GetMotherId()) continue;
	    pdg = track->GetPdgCode();
            p = track->GetP();
	}
	h1->Reset();
        Style(h1, "x (cm)", "y (cm)");
	cbmsim->Draw("RichHit.fY:RichHit.fX>>h1", "", "", 1, i);
	h1->Draw();
        nr = rings->GetEntriesFast();
	if(nr > 1) {
	    cout << "Event: " << i << "  :  " << nr << " rings!" << endl;
	}
	for(Int_t r = 0; r < nr; r++) {
	    ring = (CbmRichRing*) rings->At(r);
	    if(NULL == ring) {
		continue;
	    }
	    a = ring->GetAaxis();
	    b = ring->GetBaxis();
	    cout << pdg << "   " << p << " GeV/c   " << r << "   " << a << "   " << b << endl;
/*	    if((a/b) <= 0.9 || (b/a) <= 0.9) {
		cout << "Event: " << i << ",  ring: " << r << "  :  " << a << " / " << b << endl;
	    }*/
	    TEllipse *el1 = new TEllipse(ring->GetCenterX(), ring->GetCenterY(), a, b);
            el1->SetFillStyle(0);
	    el1->Draw();
	    el1->SetLineColor(r+2);
	    nhits = ring->GetNofHits();
	    for(Int_t ih = 0; ih < nhits; ih++) {
		hitIndex = ring->GetHit(ih);
		hit = (CbmRichHit*) hits->At(hitIndex);
		x = hit->GetX();
		y = hit->GetY();
		TEllipse *el2 = new TEllipse(x, y, hit->GetDx(), hit->GetDy());
		el2->SetLineColor(r+2);
                el2->SetFillStyle(0);
                el2->Draw();
	    }
	}
	proj = (FairTrackParam*) projs->At(0);
	TEllipse *el3 = new TEllipse(proj->GetX(), proj->GetY(),
				     30*TMath::Sqrt(proj->GetCovariance(0,0)),
				     30*TMath::Sqrt(proj->GetCovariance(1,1)));
	el3->SetLineColor(42);
        el3->SetFillColor(42);
	el3->Draw();
	TBox *b1 = new TBox(-10.5, 10., 10.5, 31.);
	b1->SetFillStyle(0);
        b1->Draw();
        h1->Draw("same");
	c1->Modified();
	c1->Update();
	char strLabel[100];
	if(11 == pdg) {
            sprintf(strLabel, "e");
	} else if(-211 == pdg) {
	    sprintf(strLabel, "#pi");
	}
	TPaveLabel *l1 = new TPaveLabel(0.785, 0.803, 0.989, 0.989, strLabel, "NDC");
	Style(l1);
        l1->Draw();
	ps1->Close();
        system("sleep 5");
    }


}



void draw_hits_rings_electrons()
{
    TPstyle();


    LoadLibs();


    TString inFileName1 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.mc.root";
    TString inFileName2 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile1 = new TFile(inFileName1);
    TFile *inFile2 = new TFile(inFileName2);
    TTree *cbmsim1 = (TTree*) inFile1->Get("cbmsim");
    TTree *cbmsim2 = (TTree*) inFile2->Get("cbmsim");
    TClonesArray *tracks = new TClonesArray("CbmMCTrack");
    TClonesArray *rings = new TClonesArray("CbmRichRing");
    TClonesArray *hits = new TClonesArray("CbmRichHit");
    TClonesArray *projs = new TClonesArray("FairTrackParam");
    Int_t nr;
    cbmsim1->SetBranchAddress("MCTrack", &tracks);
    cbmsim2->SetBranchAddress("RichRing", &rings);
    cbmsim2->SetBranchAddress("RichHit", &hits);
    cbmsim2->SetBranchAddress("RichProjection", &projs);
    char strPsFileName[1000];
    TPostScript *ps1;
    Int_t nTracks;
    CbmMCTrack *track;
    Int_t pdg;
    Float_t p;
    CbmRichRing *ring;
    Float_t a, b;
    Int_t nhits;
    UShort_t hitIndex;
    CbmRichHit *hit;
    Float_t x, y;
    FairTrackParam *proj;


    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    TH2F *h1 = new TH2F("h1", "", 300, -15., 15., 300, 20., 50.);
    for(Int_t i = 0; i < 1000; i++) {
	cbmsim1->GetEntry(i);
	cbmsim2->GetEntry(i);
	nTracks = tracks->GetEntriesFast();
	for(Int_t it = 0; it < nTracks; it++) {
	    track = (CbmMCTrack*) tracks->At(it);
	    if(-1 != track->GetMotherId()) continue;
	    pdg = track->GetPdgCode();
            p = track->GetP();
	}
	h1->Reset();
        Style(h1, "x (cm)", "y (cm)");
	cbmsim->Draw("RichHit.fY:RichHit.fX>>h1", "", "", 1, i);
	h1->Draw();
	nr = rings->GetEntriesFast();
	if(11 != pdg || 0 == nr) {
            continue;
	}
	sprintf(strPsFileName, "plots/events/rich.prot.electrons.ring.%d.eps", i);
	ps1 = new TPostScript(strPsFileName, -113);
        ps1->Range(16, 16);
	if(nr > 1) {
	    cout << "Event: " << i << "  :  " << nr << " rings!" << endl;
	}
	for(Int_t r = 0; r < nr; r++) {
	    ring = (CbmRichRing*) rings->At(r);
	    if(NULL == ring) {
		continue;
	    }
	    a = ring->GetRadius();
	    //b = ring->GetBaxis();
	    cout << pdg << "   " << p << " GeV/c   " << r << "   " << a << "   " << b << endl;
/*	    if((a/b) <= 0.9 || (b/a) <= 0.9) {
		cout << "Event: " << i << ",  ring: " << r << "  :  " << a << " / " << b << endl;
	    }*/
	    TEllipse *el1 = new TEllipse(ring->GetCenterX(), ring->GetCenterY(), a, a);
            el1->SetFillStyle(0);
	    el1->Draw();
	    el1->SetLineColor(r+2);
	    nhits = ring->GetNofHits();
	    for(Int_t ih = 0; ih < nhits; ih++) {
		hitIndex = ring->GetHit(ih);
		hit = (CbmRichHit*) hits->At(hitIndex);
		x = hit->GetX();
		y = hit->GetY();
		TEllipse *el2 = new TEllipse(x, y, hit->GetDx(), hit->GetDy());
		el2->SetLineColor(r+2);
                el2->SetFillStyle(0);
                el2->Draw();
	    }
	}
	proj = (FairTrackParam*) projs->At(0);
	TEllipse *el3 = new TEllipse(proj->GetX(), proj->GetY(),
				     30*TMath::Sqrt(proj->GetCovariance(0,0)),
				     30*TMath::Sqrt(proj->GetCovariance(1,1)));
	el3->SetLineColor(42);
        el3->SetFillColor(42);
	el3->Draw();
	TBox *b1 = new TBox(-10.5, 25., 10.5, 45.8);
	b1->SetFillStyle(0);
        b1->Draw();
        h1->Draw("same");
	c1->Modified();
	c1->Update();
	char strLabel[100];
	if(11 == pdg) {
            sprintf(strLabel, "e");
	} else if(-211 == pdg) {
	    sprintf(strLabel, "#pi");
	}
	TPaveLabel *l1 = new TPaveLabel(0.785, 0.803, 0.989, 0.989, strLabel, "NDC");
	Style(l1);
        l1->Draw();
	ps1->Close();
        system("sleep 5");
    }


}



void draw_hits_rings_pions()
{
    TPstyle();


    LoadLibs();


    TString inFileName1 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.mc.root";
    TString inFileName2 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile1 = new TFile(inFileName1);
    TFile *inFile2 = new TFile(inFileName2);
    TTree *cbmsim1 = (TTree*) inFile1->Get("cbmsim");
    TTree *cbmsim2 = (TTree*) inFile2->Get("cbmsim");
    TClonesArray *tracks = new TClonesArray("CbmMCTrack");
    TClonesArray *rings = new TClonesArray("CbmRichRing");
    TClonesArray *hits = new TClonesArray("CbmRichHit");
    TClonesArray *projs = new TClonesArray("FairTrackParam");
    Int_t nr;
    cbmsim1->SetBranchAddress("MCTrack", &tracks);
    cbmsim2->SetBranchAddress("RichRing", &rings);
    cbmsim2->SetBranchAddress("RichHit", &hits);
    cbmsim2->SetBranchAddress("RichProjection", &projs);
    char strPsFileName[1000];
    TPostScript *ps1;
    Int_t nTracks;
    CbmMCTrack *track;
    Int_t pdg;
    Float_t p;
    CbmRichRing *ring;
    Float_t a, b;
    Int_t nhits;
    UShort_t hitIndex;
    CbmRichHit *hit;
    Float_t x, y;
    FairTrackParam *proj;


    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    TH2F *h1 = new TH2F("h1", "", 300, -15., 15., 300, 20., 50.);
    for(Int_t i = 0; i < 1000; i++) {
	cbmsim1->GetEntry(i);
	cbmsim2->GetEntry(i);
	nTracks = tracks->GetEntriesFast();
	for(Int_t it = 0; it < nTracks; it++) {
	    track = (CbmMCTrack*) tracks->At(it);
	    if(-1 != track->GetMotherId()) continue;
	    pdg = track->GetPdgCode();
            p = track->GetP();
	}
	h1->Reset();
        Style(h1, "x (cm)", "y (cm)");
	cbmsim->Draw("RichHit.fY:RichHit.fX>>h1", "", "", 1, i);
	h1->Draw();
	nr = rings->GetEntriesFast();
	if(-211 != pdg || 0 == nr) {
            continue;
	}
	sprintf(strPsFileName, "plots/events/rich.prot.pions.ring.%d.eps", i);
	ps1 = new TPostScript(strPsFileName, -113);
        ps1->Range(16, 16);
	if(nr > 1) {
	    cout << "Event: " << i << "  :  " << nr << " rings!" << endl;
	}
	for(Int_t r = 0; r < nr; r++) {
	    ring = (CbmRichRing*) rings->At(r);
	    if(NULL == ring) {
		continue;
	    }
	    a = ring->GetRadius();
	    //b = ring->GetBaxis();
	    cout << pdg << "   " << p << " GeV/c   " << r << "   " << a << "   " << b << endl;
/*	    if((a/b) <= 0.9 || (b/a) <= 0.9) {
		cout << "Event: " << i << ",  ring: " << r << "  :  " << a << " / " << b << endl;
	    }*/
	    TEllipse *el1 = new TEllipse(ring->GetCenterX(), ring->GetCenterY(), a, a);
            el1->SetFillStyle(0);
	    el1->Draw();
	    el1->SetLineColor(r+2);
	    nhits = ring->GetNofHits();
	    for(Int_t ih = 0; ih < nhits; ih++) {
		hitIndex = ring->GetHit(ih);
		hit = (CbmRichHit*) hits->At(hitIndex);
		x = hit->GetX();
		y = hit->GetY();
		TEllipse *el2 = new TEllipse(x, y, hit->GetDx(), hit->GetDy());
		el2->SetLineColor(r+2);
                el2->SetFillStyle(0);
                el2->Draw();
	    }
	}
	proj = (FairTrackParam*) projs->At(0);
	TEllipse *el3 = new TEllipse(proj->GetX(), proj->GetY(),
				     30*TMath::Sqrt(proj->GetCovariance(0,0)),
				     30*TMath::Sqrt(proj->GetCovariance(1,1)));
	el3->SetLineColor(42);
        el3->SetFillColor(42);
	el3->Draw();
	TBox *b1 = new TBox(-10.5, 25., 10.5, 45.8);
	b1->SetFillStyle(0);
        b1->Draw();
        h1->Draw("same");
	c1->Modified();
	c1->Update();
	char strLabel[100];
	if(11 == pdg) {
            sprintf(strLabel, "e");
	} else if(-211 == pdg) {
	    sprintf(strLabel, "#pi");
	}
	TPaveLabel *l1 = new TPaveLabel(0.785, 0.803, 0.989, 0.989, strLabel, "NDC");
	Style(l1);
        l1->Draw();
	ps1->Close();
        system("sleep 5");
    }


}



void draw_proj(Int_t nev = 1)
{
    TPstyle();


    LoadLibs();


    TString inFileName = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile = new TFile(inFileName);
    TTree *cbmsim = (TTree*) inFile->Get("cbmsim");
    TClonesArray *projs = new TClonesArray("FairTrackParam");
    cbmsim->SetBranchAddress("RichProjection", &projs);
    FairTrackParam *proj;


    gStyle->SetPaperSize(32, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 1020, 510);
    c1->Divide(2, 1);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.proj.eps", -113);
    ps1->Range(32, 16);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    TH2F *h1 = new TH2F("h1", "", 300, -15., 15., 300, 5., 35.);
    Style(h1, "x (cm)", "y (cm)");
    for(Int_t i = 0; i < nev; i++) {
	cbmsim->GetEntry(i);
	proj = (FairTrackParam*) projs->At(0);
        h1->Fill(proj->GetX(), proj->GetY());
    }
    h1->Draw("col");
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    TH1D *h2 = h1->ProjectionX("h2", 155, 155);
    h2->Draw();
    TF1 *f1 = new TF1("f1", "gaus", -15., 15.);
    f1->SetParameters(100, 0., 1.);
    h2->Fit(f1, "Q");
    cout << "sigma = " << f1->GetParameter(2) << endl;
    c1->cd(0);
    ps1->Close();
}



void draw_nhits(Int_t nev = 1)
{
    TPstyle();


    LoadLibs();


    TString inFileName = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile = new TFile(inFileName);
    TTree *cbmsim = (TTree*) inFile->Get("cbmsim");


    gStyle->SetPaperSize(32, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 1020, 510);
    c1->Divide(2, 1);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.nhits.eps", -113);
    ps1->Range(32, 16);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    TH1F *h1 = new TH1F("h1", "", 50, -0.5, 49.5);
    Style(h1, "N of true hits / ring", "counts");
    cbmsim->Draw("RichRingMatch.fNofTrueHits>>h1", "", "", nev, 0);
    h1->Draw();
    char str[256];
    sprintf(str, "%d", TMath::Nint(h1->GetMean()));
    TPaveLabel *l1 = new TPaveLabel(0.76, 0.73, 0.91, 0.88, str, "NDC");
    Style(l1);
    l1->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    TH1F *h2 = new TH1F("h2", "", 10, -0.5, 9.5);
    Style(h2, "N of fake hits / ring", "counts");
    cbmsim->Draw("RichRingMatch.fNofFakeHits>>h2", "", "", nev, 0);
    h2->Draw();
    sprintf(str, "%d", TMath::Nint(h2->GetMean()));
    TPaveLabel *l2 = new TPaveLabel(0.76, 0.73, 0.91, 0.88, str, "NDC");
    Style(l2);
    l2->Draw();
    c1->cd(0);
    ps1->Close();
}



void draw_nhits_epi()
{
    TPstyle();


    LoadLibs();


    TString inFileName1 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.mc.root";
    TString inFileName2 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile1 = new TFile(inFileName1);
    TFile *inFile2 = new TFile(inFileName2);
    TTree *cbmsim1 = (TTree*) inFile1->Get("cbmsim");
    TTree *cbmsim2 = (TTree*) inFile2->Get("cbmsim");
    TClonesArray *tracks = new TClonesArray("CbmMCTrack");
    TClonesArray *rings = new TClonesArray("CbmRichRingMatch");
    cbmsim1->SetBranchAddress("MCTrack", &tracks);
    cbmsim2->SetBranchAddress("RichRingMatch", &rings);
    cbmsim1->SetBranchStatus("*", 0);
    cbmsim2->SetBranchStatus("*", 0);
    cbmsim1->SetBranchStatus("MCTrack.fMotherId", 1);
    cbmsim1->SetBranchStatus("MCTrack.fPdgCode", 1);
    cbmsim2->SetBranchStatus("RichRingMatch.fNofTrueHits", 1);
    cbmsim2->SetBranchStatus("RichRingMatch.fNofFakeHits", 1);
    TH1F *h1_el = new TH1F("h1_el", "", 50, -0.5, 49.5);
    Style(h1_el, "N of true hits / ring", "counts");
    TH1F *h2_el = new TH1F("h2_el", "", 10, -0.5, 9.5);
    Style(h2_el, "N of fake hits / ring", "counts");
    TH1F *h1_pi = new TH1F("h1_pi", "", 50, -0.5, 49.5);
    TH1F *h2_pi = new TH1F("h2_pi", "", 10, -0.5, 9.5);
    h1_pi->SetLineStyle(2);
    h2_pi->SetLineStyle(2);
    h1_pi->SetLineColor(2);
    h2_pi->SetLineColor(2);
    Int_t nev = cbmsim2->GetEntries();
    Int_t nTracks;
    CbmMCTrack *track;
    Int_t pdg;
    Int_t nRings;
    CbmRichRingMatch *ring;
    Int_t nTrueHits;
    Int_t nFakeHits;
    for(Int_t iev = 0; iev < nev; iev++) {
        cbmsim1->GetEntry(iev);
	cbmsim2->GetEntry(iev);
        Int_t nTracks = tracks->GetEntriesFast();
	for(Int_t it = 0; it < nTracks; it++) {
	    track = (CbmMCTrack*) tracks->At(it);
	    if(NULL == track) continue;
	    if(-1 != track->GetMotherId()) continue;
	    pdg = track->GetPdgCode();
            break;
	}// tracks
	Int_t nRings = rings->GetEntriesFast();
	for(Int_t ir = 0; ir < nRings; ir++) {
	    ring = (CbmRichRingMatch*) rings->At(ir);
	    if(NULL == ring) continue;
            nTrueHits = ring->GetNofTrueHits();
	    nFakeHits = ring->GetNofFakeHits();
	    if(11 == pdg) {
                h1_el->Fill(nTrueHits);
                h2_el->Fill(nFakeHits);
	    } else if(-211 == pdg) {
                h1_pi->Fill(nTrueHits);
                h2_pi->Fill(nFakeHits);
	    }
	}// rings
	if(0 == (iev%1000)) cout << iev << endl;
    }// events


    gStyle->SetPaperSize(32, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 1020, 510);
    c1->Divide(2, 1);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.nhits.epi.eps", -113);
    ps1->Range(32, 16);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    h1_el->Draw();
    h1_pi->Draw("same");
    char str[256];
    sprintf(str, "%d", TMath::Nint(h1_el->GetMean()));
    TPaveLabel *l1 = new TPaveLabel(0.76, 0.73, 0.91, 0.88, str, "NDC");
    Style(l1);
//    l1->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    h2_el->Draw();
    h2_pi->Draw("same");
    sprintf(str, "%d", TMath::Nint(h2_el->GetMean()));
    TPaveLabel *l2 = new TPaveLabel(0.76, 0.73, 0.91, 0.88, str, "NDC");
    Style(l2);
//    l2->Draw();
    c1->cd(0);
    ps1->Close();
}



void draw_chi2(Int_t nev = 1)
{
    TPstyle();


    LoadLibs();


    TString inFileName = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile = new TFile(inFileName);
    TTree *cbmsim = (TTree*) inFile->Get("cbmsim");


    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.chi2.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    TH1F *h1 = new TH1F("h1", "", 50, 0., 5.);
    Style(h1, "#chi^{2}", "counts");
    cbmsim->Draw("RichRing.fChi2>>h1", "", "", nev, 0);
    h1->Draw();
    ps1->Close();
}



void draw_a_b(Int_t nev = 1)
{
    TPstyle();


    LoadLibs();


    TString inFileName = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile = new TFile(inFileName);
    TTree *cbmsim = (TTree*) inFile->Get("cbmsim");


    gStyle->SetPaperSize(32, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 1020, 510);
    c1->Divide(2, 1);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.ab.eps", -113);
    ps1->Range(32, 16);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    TH1F *h1 = new TH1F("h1", "", 60, 2., 8.);
    Style(h1, "a (cm)", "counts");
    cbmsim->Draw("RichRing.fAaxis>>h1", "", "", nev, 0);
    h1->Draw();
    TF1 *f1 = new TF1("f1", "gaus", 0., 10.);
    f1->SetParameters(10, 4.4, 0.1);
    h1->Fit(f1, "Q");
    char str[256];
    sprintf(str, "%3.2f #pm %3.2f", f1->GetParameter(1), f1->GetParameter(2));
    TPaveLabel *l1 = new TPaveLabel(0.58, 0.77, 0.92, 0.88, str, "NDC");
    Style(l1);
    l1->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    TH1F *h2 = new TH1F("h2", "", 60, 2., 8.);
    Style(h2, "b (cm)", "counts");
    cbmsim->Draw("RichRing.fBaxis>>h2", "", "", nev, 0);
    h2->Draw();
    TF1 *f2 = new TF1("f2", "gaus", 0., 10.);
    f2->SetParameters(10, 4.4, 0.1);
    h2->Fit(f2, "Q");
    sprintf(str, "%3.2f #pm %3.2f", f2->GetParameter(1), f2->GetParameter(2));
    TPaveLabel *l2 = new TPaveLabel(0.58, 0.77, 0.92, 0.88, str, "NDC");
    Style(l2);
    l2->Draw();
    c1->cd(0);
    ps1->Close();
}




void draw_a_epi()
{
    TPstyle();


    LoadLibs();


    TString inFileName1 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.mc.root";
    TString inFileName2 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.root";
    TFile *inFile1 = new TFile(inFileName1);
    TFile *inFile2 = new TFile(inFileName2);
    TTree *cbmsim1 = (TTree*) inFile1->Get("cbmsim");
    TTree *cbmsim2 = (TTree*) inFile2->Get("cbmsim");
    TClonesArray *tracks = new TClonesArray("CbmMCTrack");
    TClonesArray *rings = new TClonesArray("CbmRichRing");
    cbmsim1->SetBranchAddress("MCTrack", &tracks);
    cbmsim2->SetBranchAddress("RichRing", &rings);
    cbmsim1->SetBranchStatus("*", 0);
    cbmsim2->SetBranchStatus("*", 0);
    cbmsim1->SetBranchStatus("MCTrack.fMotherId", 1);
    cbmsim1->SetBranchStatus("MCTrack.fPdgCode", 1);
    cbmsim1->SetBranchStatus("MCTrack.fPx", 1);
    cbmsim1->SetBranchStatus("MCTrack.fPy", 1);
    cbmsim1->SetBranchStatus("MCTrack.fPz", 1);
    cbmsim2->SetBranchStatus("RichRing.fAaxis", 1);
    cbmsim2->SetBranchStatus("RichRing.fNofHitsOnRing", 1);

    TH1F *h1_el = new TH1F("h1_el", "", 80, 0., 8.);
    Style(h1_el, "a (cm)", "counts");
    TH1F *h1_pi = new TH1F("h1_pi", "", 80, 0., 8.);
    Style(h1_pi, "a (cm)", "counts");
    h1_pi->SetLineStyle(2);
    h1_pi->SetLineColor(2);

    TH2F *h2 = new TH2F("h2", "", 90., 1., 10., 80, 0., 8.);
    TH2F *h2_el = new TH2F("h2_el", "", 90., 1., 10., 80, 0., 8.);
    TH2F *h2_pi = new TH2F("h2_pi", "", 90., 1., 10., 80, 0., 8.);
    Style(h2, "p (GeV/c)", "a (cm)");
    Style(h2_el, "p (GeV/c)", "a (cm)");
    h2->GetXaxis()->SetTitleSize(0.05);
    h2->GetXaxis()->SetTitleOffset(0.9);
    h2->GetYaxis()->SetTitleSize(0.05);
    h2->GetYaxis()->SetTitleOffset(0.9);

    TH1F *h_mpid = new TH1F("h_mpid", "", 90, 1., 10.);
    Style(h_mpid, "p (GeV/c)", "pion misidentification (%)");
    h_mpid->GetXaxis()->SetTitleSize(0.05);
    h_mpid->GetXaxis()->SetTitleOffset(0.9);
    h_mpid->GetYaxis()->SetTitleSize(0.05);
    h_mpid->GetYaxis()->SetTitleOffset(0.9);

    Int_t nev = cbmsim2->GetEntries();
    Int_t nTracks;
    CbmMCTrack *track;
    Int_t pdg;
    Float_t p;
    Int_t nRings;
    CbmRichRing *ring;
    Float_t a;
    for(Int_t iev = 0; iev < nev; iev++) {
        cbmsim1->GetEntry(iev);
	cbmsim2->GetEntry(iev);

	nTracks = tracks->GetEntriesFast();
	for(Int_t it = 0; it < nTracks; it++) {
	    track = (CbmMCTrack*) tracks->At(it);
	    if(-1 != track->GetMotherId()) continue;
	    pdg = track->GetPdgCode();
            p = track->GetP();
            break;
	}

        nRings = rings->GetEntriesFast();
	for(Int_t ir = 0; ir < nRings; ir++) {
	    ring = (CbmRichRing*) rings->At(ir);
            if(ring->GetNofHitsOnRing() <= 5) continue;
	    a = ring->GetAaxis();
            h2->Fill(p, a);
	    if(11 == pdg) {
                h1_el->Fill(a);
                h2_el->Fill(p, a);
	    } else if(-211 == pdg) {
                h1_pi->Fill(a);
                h2_pi->Fill(p, a);
	    }
	}

	if(0 == (iev%1000)) {
            cout << iev << endl;
	}
    }

    TH1D *h_pi;
    Float_t npi_all;
    Float_t npi_el;
    for(Int_t i = 45; i < 290; i++) {
	h_pi = h2_pi->ProjectionY("h_pi", i+1, i+1);
        npi_el = 0;
	for(Int_t j = 45; j < 80; j++) {
            npi_el += h_pi->GetBinContent(j+1);
	}
	npi_all = h_pi->Integral();
	if(npi_all) {
	    h_mpid->SetBinContent(i+1, 100*npi_el/npi_all);
	}
    }


    /*TF1 *f1_el = new TF1("f1_el", f_radius, 1., 10., 1);
    f1_el->SetParameter(0, mass_el);
    f1_el->SetLineStyle(2);

    TF1 *f1_pi = new TF1("f1_pi", f_radius, 1., 10., 1);
    f1_pi->SetParameter(0, mass_pi);
    f1_pi->SetLineStyle(2);
    f1_pi->SetLineColor(2);*/


    gStyle->SetPaperSize(32, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 1020, 510);
    c1->Divide(2, 1);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.a.epi.eps", -113);
    ps1->Range(32, 16);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h2->Draw("col");
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_mpid->Draw();
    c1->cd(0);
    ps1->Close();

    /*gStyle->SetPaperSize(16, 16);
    TCanvas *c2 = new TCanvas("c2", "", 100, 10, 510, 510);
    TPostScript *ps2 = new TPostScript("plots/rich.prot.mpid.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_mpid->Draw();
    ps2->Close();*/

    //TF1 *f1 = new TF1("f1", "gaus", 0., 8.);
    //h1_el->Fit(f1);
}



void draw_b_epi()
{
    TPstyle();


    LoadLibs();


    TString inFileName1 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.mc.root";
    TString inFileName2 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile1 = new TFile(inFileName1);
    TFile *inFile2 = new TFile(inFileName2);
    TTree *cbmsim1 = (TTree*) inFile1->Get("cbmsim");
    TTree *cbmsim2 = (TTree*) inFile2->Get("cbmsim");
    TClonesArray *tracks = new TClonesArray("CbmMCTrack");
    TClonesArray *rings = new TClonesArray("CbmRichRing");
    cbmsim1->SetBranchAddress("MCTrack", &tracks);
    cbmsim2->SetBranchAddress("RichRing", &rings);

    TH1F *h1_el = new TH1F("h1_el", "", 80, 0., 8.);
    Style(h1_el, "b (cm)", "counts");
    TH1F *h1_pi = new TH1F("h1_pi", "", 80, 0., 8.);
    Style(h1_pi, "b (cm)", "counts");
    h1_pi->SetLineStyle(2);

    TH2F *h2_el = new TH2F("h2_el", "", 100., 1., 6., 80, 0., 8.);
    TH2F *h2_pi = new TH2F("h2_pi", "", 100., 1., 6., 80, 0., 8.);
    Style(h2_el, "p (GeV/c)", "b (cm)");

    Int_t nev = cbmsim2->GetEntries();
    Int_t nTracks;
    CbmMCTrack *track;
    Int_t pdg;
    Float_t p;
    Int_t nRings;
    CbmRichRing *ring;
    Float_t b;
    for(Int_t iev = 0; iev < nev; iev++) {
        cbmsim1->GetEntry(iev);
	cbmsim2->GetEntry(iev);

	nTracks = tracks->GetEntriesFast();
	for(Int_t it = 0; it < nTracks; it++) {
	    track = (CbmMCTrack*) tracks->At(it);
	    if(-1 != track->GetMotherId()) continue;
	    pdg = track->GetPdgCode();
            p = track->GetP();
            break;
	}

        nRings = rings->GetEntriesFast();
	for(Int_t ir = 0; ir < nRings; ir++) {
	    ring = (CbmRichRing*) rings->At(ir);
	    b = ring->GetBaxis();
	    if(11 == pdg) {
                h1_el->Fill(b);
                h2_el->Fill(p, b);
	    } else if(-211 == pdg) {
                h1_pi->Fill(b);
                h2_pi->Fill(p, b);
	    }
	}

	if(0 == (iev%1000)) {
            cout << iev << endl;
	}
    }

    gStyle->SetPaperSize(32, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 1020, 510);
    c1->Divide(2, 1);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.b.epi.eps", -113);
    ps1->Range(32, 16);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    h1_el->Draw();
    h1_pi->Draw("same");
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h2_el->Draw("col");
    h2_pi->Draw("colsame");
    c1->cd(0);
    ps1->Close();
}



void draw_nhits_vs_ba(Int_t nev = 1)
{
    TPstyle();


    LoadLibs();


    TString inFileName = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile = new TFile(inFileName);
    TTree *cbmsim = (TTree*) inFile->Get("cbmsim");
    TClonesArray *rings = new TClonesArray("CbmRichRing");
    Int_t nr;
    cbmsim->SetBranchAddress("RichRing", &rings);
    CbmRichRing *ring;
    Float_t a, b;
    Int_t nhits;
    TH2F *h1 = new TH2F("h1", "", 50, -0.5, 49.5, 100, 0.7, 1.1);
    Style(h1, "N_{hits} on ring", "b / a");
    for(Int_t i = 0; i < nev; i++) {
	cbmsim->GetEntry(i);
        nr = rings->GetEntriesFast();
	for(Int_t r = 0; r < nr; r++) {
	    ring = (CbmRichRing*) rings->At(r);
	    if(NULL == ring) {
		continue;
	    }
	    a = ring->GetAaxis();
	    b = ring->GetBaxis();
	    nhits = ring->GetNofHits();
            h1->Fill(nhits, (b/a));
	}
    }
    TH1D *h2 = h1->ProjectionY("h2");
    cout << h2->GetMean() << endl;


    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    ps1 = new TPostScript("plots/rich.prot.quality_vs_nhits.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.12);
    h1->Draw("colz");
    TLine *l1 = new TLine(0., 0.9, 49., 0.9);
    l1->SetLineColor(2);
    l1->Draw();
    ps1->Close();
}



void draw_distance_vs_ba(Int_t nev = 1)
{
    TPstyle();


    LoadLibs();


    TString inFileName = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile = new TFile(inFileName);
    TTree *cbmsim = (TTree*) inFile->Get("cbmsim");
    TClonesArray *rings = new TClonesArray("CbmRichRing");
    TClonesArray *projs = new TClonesArray("FairTrackParam");
    Int_t nr;
    cbmsim->SetBranchAddress("RichRing", &rings);
    cbmsim->SetBranchAddress("RichProjection", &projs);
    CbmRichRing *ring;
    Float_t a, b;
    Int_t nhits;
    FairTrackParam *proj;
    TH2F *h1 = new TH2F("h1", "", 200, 0., 2., 100, 0.7, 1.1);
    Style(h1, "ring center - track projection", "b / a");
    for(Int_t i = 0; i < nev; i++) {
	cbmsim->GetEntry(i);
	nr = rings->GetEntriesFast();
	proj = (FairTrackParam*) projs->At(0);
	if(NULL == proj) {
	    continue;
	}
	for(Int_t r = 0; r < nr; r++) {
	    ring = (CbmRichRing*) rings->At(r);
	    if(NULL == ring) {
		continue;
	    }
	    a = ring->GetAaxis();
	    b = ring->GetBaxis();
	    nhits = ring->GetNofHits();
	    h1->Fill(TMath::Sqrt(TMath::Power(ring->GetCenterX()-proj->GetX(),2)+TMath::Power(ring->GetCenterY()-proj->GetY(),2)), (b/a));
	}
    }


    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    ps1 = new TPostScript("plots/rich.prot.quality_vs_distance.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.12);
    h1->Draw("colz");
    TLine *l1 = new TLine(0., 0.9, 2., 0.9);
    l1->SetLineColor(2);
    l1->Draw();
    ps1->Close();
}



void draw_ntruehits_vs_ba(Int_t nev = 1)
{
    TPstyle();


    LoadLibs();


    TString inFileName = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.circ.root";
    TFile *inFile = new TFile(inFileName);
    TTree *cbmsim = (TTree*) inFile->Get("cbmsim");
    TClonesArray *rings = new TClonesArray("CbmRichRing");
    TClonesArray *ringMatches = new TClonesArray("CbmRichRingMatch");
    Int_t nr;
    cbmsim->SetBranchAddress("RichRing", &rings);
    cbmsim->SetBranchAddress("RichRingMatch", &ringMatches);
    CbmRichRing *ring;
    CbmRichRingMatch *ringMatch;
    Float_t a, b;
    Int_t ntruehits;
    TH2F *h1 = new TH2F("h1", "", 50, -0.5, 49.5, 100, 0.7, 1.1);
    Style(h1, "N_{true hits} on ring", "b / a");
    for(Int_t i = 0; i < nev; i++) {
	cbmsim->GetEntry(i);
        nr = rings->GetEntriesFast();
	for(Int_t r = 0; r < nr; r++) {
	    ring = (CbmRichRing*) rings->At(r);
	    ringMatch = (CbmRichRingMatch*) ringMatches->At(r);
	    if(NULL == ring || NULL == ringMatch) {
		continue;
	    }
	    a = ring->GetAaxis();
	    b = ring->GetBaxis();
	    ntruehits = ringMatch->GetNofTrueHits();
            h1->Fill(ntruehits, (b/a));
	}
    }


    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    ps1 = new TPostScript("plots/rich.prot.quality_vs_ntruehits.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.12);
    h1->Draw("colz");
    TLine *l1 = new TLine(0., 0.9, 29., 0.9);
    l1->SetLineColor(2);
    l1->Draw();
    ps1->Close();
}



void draw_gas_box()
{
    TPstyle();

    gStyle->SetPaperSize(16, 12);

    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 383);

    TPostScript *ps1 = new TPostScript("plots/gasbox.eps", -113);
    ps1->Range(16, 12);

    const Double_t d = 2100.;
    const Double_t g = 100.;
    const Double_t scale_y = 1./(1200.+2*g);
    const Double_t scale_z = 1./(d+2*g);

    TLine *l1 = new TLine(g*scale_z, g*scale_y, (g+2100.)*scale_z, g*scale_y);
    l1->SetLineWidth(2);
    l1->Draw();

    TLine *l2 = new TLine(g*scale_z, g*scale_y, g*scale_z, (g+700)*scale_y);
    l2->SetLineWidth(2);
    l2->Draw();

    TLine *l3 = new TLine(g*scale_z, (g+700)*scale_y, (g+140)*scale_z, (g+700)*scale_y);
    l3->SetLineWidth(2);
    l3->Draw();

    TLine *l4 = new TLine((g+140)*scale_z, (g+700)*scale_y, (g+258)*scale_z, (g+1200)*scale_y);
    l4->SetLineWidth(2);
    l4->Draw();

    TLine *l5 = new TLine((g+258)*scale_z, (g+1200)*scale_y, (g+2100)*scale_z, (g+1200)*scale_y);
    l5->SetLineWidth(2);
    l5->Draw();

    TLine *l6 = new TLine((g+2100)*scale_z, (g+1200)*scale_y, (g+2100)*scale_z, g*scale_y);
    l6->SetLineWidth(2);
    l6->Draw();

    TLine *l7 = new TLine((g+140)*scale_z, (g+600)*scale_y, (g+140)*scale_z, (g+700)*scale_y);
    l7->SetLineStyle(2);
    l7->Draw();

    TLine *l8 = new TLine((g+258)*scale_z, (g+500-50)*scale_y, (g+258)*scale_z, (g+1200)*scale_y);
    l8->SetLineStyle(2);
    l8->Draw();

    TLine *ay = new TLine(g*scale_z, (g+600)*scale_y, g*scale_z, (g+1250.)*scale_y);
    ay->Draw();

    TLine *az = new TLine(g*scale_z, (g+600)*scale_y, (g+d)*scale_z, (g+600)*scale_y);
    az->Draw();

    TLine *bw = new TLine(g*scale_z, (g+500)*scale_y, g*scale_z, (g+700)*scale_y);
    bw->SetLineColor(2);
    bw->SetLineWidth(3);
    bw->Draw();

    TLine *pd = new TLine((g+178)*scale_z, (g+600+253)*scale_y, (g+226)*scale_z, (g+600+455)*scale_y);
    pd->SetLineColor(3);
    pd->SetLineWidth(3);
    pd->Draw();

    TPaveLabel *la1 = new TPaveLabel(0.053, 0.58, 0.10, 0.63, "100", "NDC");
    Style(la1);
    la1->Draw();

    TPaveLabel *la2 = new TPaveLabel(0.053, 0.848, 0.122, 0.896, "600", "NDC");
    Style(la2);
    la2->Draw();

    TPaveLabel *la3 = new TPaveLabel(0.053, 0.096, 0.144, 0.146, "-600", "NDC");
    Style(la3);
    la3->Draw();

    TPaveLabel *la4 = new TPaveLabel(0.061, 0.445, 0.150, 0.493, "240", "NDC");
    Style(la4);
    la4->Draw();

    TPaveLabel *la5 = new TPaveLabel(0.115, 0.386, 0.204, 0.434, "258", "NDC");
    Style(la5);
    la5->Draw();

    TPaveLabel *la6 = new TPaveLabel(0.812, 0.442, 0.903, 0.493, "2100", "NDC");
    Style(la6);
    la6->Draw();

    TPaveLabel *lay = new TPaveLabel(0.053, 0.907, 0.123, 0.955, "Y", "NDC");
    Style(lay);
    lay->Draw();

    TPaveLabel *laz = new TPaveLabel(0.915, 0.442, 0.986, 0.487, "Z", "NDC");
    Style(laz);
    laz->Draw();

    ps1->Close();
}



void draw_efficiency_e()
{
    TPstyle();


    const Int_t n = 12;
    Float_t s_theta[] = {
	0., 1., 2., 3., 4., 5.,
	10., 20., 30., 40., 50., 100.
    };
    Float_t eff[] = {
	99.2, 99.2, 99.2, 99.3, 99.1, 98.4,
        89.6, 64.2, 47.2, 37.0, 30.4, 15.1
    };


    TGraph *gr = new TGraph(n, s_theta, eff);
    gr->SetMarkerStyle(20);
    gr->SetMarkerColor(4);
    gr->SetLineColor(4);


    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.stheta.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogx();
    gr->Draw("APC");
    Style(gr->GetHistogram(), "#sigma_{#theta} (mrad)", "efficiency (%)");
    gr->GetHistogram()->GetYaxis()->SetRangeUser(0., 100.);
    TLine *l1 = new TLine(4., 0., 4., 100.);
    l1->SetLineColor(2);
    l1->Draw();
    ps1->Close();
}



void draw_spectra()
{
    TPstyle();

    TFile *file = new TFile("photons.root");
    TH1F *h_all = (TH1F*) file->Get("h_all");
    TH1F *h_acc = (TH1F*) file->Get("h_acc");
    TH1F *h_phot = (TH1F*) file->Get("h_phot");
    TH1F *h_hits = (TH1F*) file->Get("h_hits");
    TH1F *h_expd = new TH1F("h_expd", "", 1000, 0., 1000.);
    Style(h_all, "#lambda (nm)", "dn/d#lambda (nm^{-1})");
    h_all->SetLineStyle(2);
    h_phot->SetLineColor(2);
    h_hits->SetFillColor(42);
    h_expd->SetFillColor(35);

    TF1 *f1 = new TF1("f1", spectrum, 0., 1000., 0);
    f1->SetLineWidth(1.2);
    f1->SetLineColor(4);

    Double_t lambda;
    Int_t index;
    for(Int_t i = 0; i < 1000; i++) {
	lambda = 1.*((Double_t)i+0.5);
	if(lambda < fLambdaMin || lambda > fLambdaMax) continue;
	index = (Int_t)((lambda-fLambdaMin)/20.);
	h_expd->SetBinContent(i+1, h_acc->GetBinContent(i+1)*
			      fEfficiency[index]*
			      (1.-0.09615));
    }

    gStyle->SetPaperSize(16, 16);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 510, 510);
    TPostScript *ps1 = new TPostScript("plots/rich.prot.spectra.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetLogy();
    h_all->Draw();
    h_acc->Draw("same");
    h_expd->Draw("same");
    h_phot->Draw("same");
    h_hits->Draw("same");
    f1->Draw("same");
    TLegend *leg1 = new TLegend(0.524, 0.488, 0.985, 0.985);
    Style(leg1);
    leg1->AddEntry(f1, "formula", "l");
    leg1->AddEntry(h_all, "all photons", "l");
    leg1->AddEntry(h_acc, "MC points", "l");
    leg1->AddEntry(h_phot, "measured photons", "l");
    leg1->AddEntry(h_hits, "RICH hits", "f");
    leg1->AddEntry(h_expd, "MC points * eff", "f");
    leg1->Draw();
    ps1->Close();
    Double_t integral = f1->Integral(150., 650.);
    cout << "  Precision : " << 100.*(integral-h_all->Integral(151, 651))/integral << " %" << endl;
    cout << "   Accepted : " << h_acc->Integral() << endl;
    cout << "   Expected : " << h_expd->Integral() << endl;
    cout << "   Observed : " << h_phot->Integral() << endl;
    cout << "       Hits : " << h_hits->Integral() << endl;
    cout << "Efficiency1 : " << 100.*h_hits->Integral()/h_acc->Integral() << " %" << endl;
    cout << "Efficiency2 : " << 100.*h_hits->Integral(201,500)/h_acc->Integral(201,500) << " %" << endl;
}



Double_t spectrum(Double_t *x, Double_t *par)
{
    Double_t lambda = x[0]*1e-9;      // m
    if(lambda <= 0.) return 0.;
    Double_t energy = c/n*h/e/lambda; // eV
    if(energy<en[0] || energy>en[64]) return 0.;
    Double_t n1;
    for(Int_t i = 0; i < en_num; i++) {
	if(energy<en[i]) {
	    n1 = en_n[i];
	    break;
	}
    }
    Double_t beta = 10./TMath::Sqrt(10.*10. + mass_el*mass_el);
    Double_t theta = TMath::ACos(1./beta/n1);
    Double_t dn = 1./lambda/lambda*(2.1-0.5e-3-1e-2)*TMath::TwoPi()/137.036*
	TMath::Power(TMath::Sin(theta),2) * 1e-9;
    return dn;
}



const Double_t mass_el = 0.000510998902;
const Double_t c = 2.998e8;
const Double_t h = 6.626e-34;
const Double_t e = 1.6022e-19;
const Double_t n = 1.0004462;



const Int_t en_num = 65;
Double_t en[en_num];
Double_t en_n[en_num];
en[0]=1.77;   en_n[0]=1.000446242;
en[1]=2.0;    en_n[1]=1.000447917;
en[2]=2.1;    en_n[2]=1.000448714;
en[3]=2.2;    en_n[3]=1.000449552;
en[4]=2.3;    en_n[4]=1.000450433;
en[5]=2.4;    en_n[5]=1.000451357;
en[6]=2.5;    en_n[6]=1.000452324;
en[7]=2.6;    en_n[7]=1.000453335;
en[8]=2.7;    en_n[8]=1.00045439;
en[9]=2.8;    en_n[9]=1.00045549;
en[10]=2.9;   en_n[10]=1.000456636;
en[11]=3.0;   en_n[11]=1.000457829;
en[12]=3.1;   en_n[12]=1.000459068;
en[13]=3.2;   en_n[13]=1.000460355;
en[14]=3.3;   en_n[14]=1.00046169;
en[15]=3.4;   en_n[15]=1.000463075;
en[16]=3.5;   en_n[16]=1.00046451;
en[17]=3.6;   en_n[17]=1.000465996;
en[18]=3.7;   en_n[18]=1.000467533;
en[19]=3.8;   en_n[19]=1.000469123;
en[20]=3.9;   en_n[20]=1.000470767;
en[21]=4.0;   en_n[21]=1.000472465;
en[22]=4.1;   en_n[22]=1.00047422;
en[23]=4.2;   en_n[23]=1.000476031;
en[24]=4.3;   en_n[24]=1.0004779;
en[25]=4.4;   en_n[25]=1.000479829;
en[26]=4.5;   en_n[26]=1.000481818;
en[27]=4.6;   en_n[27]=1.000483869;
en[28]=4.7;   en_n[28]=1.000485983;
en[29]=4.8;   en_n[29]=1.000488161;
en[30]=4.9;   en_n[30]=1.000490406;
en[31]=5.0;   en_n[31]=1.000492718;
en[32]=5.1;   en_n[32]=1.0004951;
en[33]=5.2;   en_n[33]=1.000497553;
en[34]=5.3;   en_n[34]=1.000500078;
en[35]=5.4;   en_n[35]=1.000502678;
en[36]=5.5;   en_n[36]=1.000505355;
en[37]=5.6;   en_n[37]=1.00050811;
en[38]=5.7;   en_n[38]=1.000510946;
en[39]=5.8;   en_n[39]=1.000513865;
en[40]=5.9;   en_n[40]=1.000516869;
en[41]=6.0;   en_n[41]=1.00051996;
en[42]=6.1;   en_n[42]=1.000523142;
en[43]=6.2;   en_n[43]=1.000526416;
en[44]=6.3;   en_n[44]=1.000529786;
en[45]=6.4;   en_n[45]=1.000533255;
en[46]=6.5;   en_n[46]=1.000536824;
en[47]=6.6;   en_n[47]=1.000540499;
en[48]=6.7;   en_n[48]=1.000544281;
en[49]=6.89;  en_n[49]=1.000551777;
en[50]=7.09;  en_n[50]=1.00056013;
en[51]=7.29;  en_n[51]=1.000568991;
en[52]=7.51;  en_n[52]=1.000579367;
en[53]=7.75;  en_n[53]=1.000591499;
en[54]=8.0;   en_n[54]=1.000605123;
en[55]=8.27;  en_n[55]=1.000621082;
en[56]=8.55;  en_n[56]=1.000639155;
en[57]=8.86;  en_n[57]=1.000661203;
en[58]=9.18;  en_n[58]=1.000686535;
en[59]=9.64;  en_n[59]=1.000728383;
en[60]=9.92;  en_n[60]=1.000757597;
en[61]=10.33; en_n[61]=1.000806643;
en[62]=10.78; en_n[62]=1.000871174;
en[63]=11.27; en_n[63]=1.000958375;
en[64]=11.81; en_n[64]=1.00108348;


const Double_t fLambdaMin = 200.;
const Double_t fLambdaMax = 640.;
const Double_t fLambdaStep = 20.;

Double_t fEfficiency[22];
fEfficiency[0] = 0.095;
fEfficiency[1] = 0.13;
fEfficiency[2] = 0.16;
fEfficiency[3] = 0.2;
fEfficiency[4] = 0.23;
fEfficiency[5] = 0.24;
fEfficiency[6] = 0.25;
fEfficiency[7] = 0.25;
fEfficiency[8] = 0.24;
fEfficiency[9] = 0.24;
fEfficiency[10] = 0.23;
fEfficiency[11] = 0.22;
fEfficiency[12] = 0.2;
fEfficiency[13] = 0.16;
fEfficiency[14] = 0.14;
fEfficiency[15] = 0.1;
fEfficiency[16] = 0.065;
fEfficiency[17] = 0.045;
fEfficiency[18] = 0.02;
fEfficiency[19] = 0.017;
fEfficiency[20] = 0.007;
fEfficiency[21] = 0.0033;



const Double_t n = 1.0004951;//1.0004462;
const Double_t mass_el = 0.000510998902;
const Double_t mass_pi = 0.13957018;



Double_t f_radius(Double_t *x, Double_t *par)
{
    if(x[0] <= 0.) {
        return 0.;
    }
    Double_t e = TMath::Sqrt(x[0]*x[0]+par[0]*par[0]);
    Double_t beta = x[0]/e;
    Double_t t = 1./(beta*n);
    Double_t a = 0.;
    if(t <= 1.) {
	a = TMath::ACos(t);
    }
    Double_t r = 170.*TMath::Tan(a);
    return r;
}




void LoadLibs()
{
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libBase");
    gSystem->Load("libCbmBase");
    gSystem->Load("libCbmData");
    gSystem->Load("libField");
    gSystem->Load("libGen");
    gSystem->Load("libPassive");
    gSystem->Load("libSts");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libTof");
    gSystem->Load("libEcal");
}


