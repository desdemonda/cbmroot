



{
    const Float_t c = 2.998e8;
    const Float_t h = 6.626e-34;
    const Float_t e = 1.6022e-19;
    const Float_t n = 1.0004462;


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
    gSystem->Load("libMvd");
    gSystem->Load("libSts");
    gSystem->Load("libEcal");
    gSystem->Load("libKF");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libTof");


    TFile *file1 = new TFile("/d/cbm02/kresan/rich_prot/jul10/e.standard.30.mc.root");
    TFile *file2 = new TFile("/d/cbm02/kresan/rich_prot/jul10/e.standard.30.reco.root");
    TTree *tree1 = (TTree*) file1->Get("cbmsim");
    TTree *tree2 = (TTree*) file2->Get("cbmsim");
    TClonesArray *tracks = new TClonesArray("CbmMCTrack");
    TClonesArray *points = new TClonesArray("CbmRichPoint");
    TClonesArray *hits = new TClonesArray("CbmRichHit");
    tree1->SetBranchAddress("MCTrack", &tracks);
    tree1->SetBranchAddress("RichPoint", &points);
    tree2->SetBranchAddress("RichHit", &hits);
    Int_t nev = tree1->GetEntries();


    TFile *ofile = new TFile("photons.root", "RECREATE");
    TH1F *h_all = new TH1F("h_all", "", 1000, 0., 1000.);
    TH1F *h_acc = new TH1F("h_acc", "", 1000, 0., 1000.);
    TH1F *h_phot = new TH1F("h_phot", "", 1000, 0., 1000.);
    TH1F *h_hits = new TH1F("h_hits", "", 1000, 0., 1000.);


    Int_t nTracks;
    Int_t nPoints;
    Int_t nHits;
    CbmMCTrack *track;
    Int_t pdg;
    Float_t energy;
    Float_t lambda;
    CbmRichPoint *point;
    Int_t trackId;
    CbmRichHit *hit;
    Int_t pointId;
    for(Int_t iev = 0; iev < nev; iev++) {
        tree1->GetEntry(iev);
	tree2->GetEntry(iev);

	nTracks = tracks->GetEntriesFast();
	for(Int_t it = 0; it < nTracks; it++){
	    track = (CbmMCTrack*) tracks->At(it);
	    if(NULL == track) continue;
	    pdg = track->GetPdgCode();
            if(50000050 != pdg) continue;
	    energy = TMath::Sqrt(TMath::Power(track->GetPx(),2)+
				 TMath::Power(track->GetPy(),2)+
				 TMath::Power(track->GetPz(),2));
            lambda = c/n*h/e/energy;
            h_all->Fill(lambda);
	}// tracks

	nPoints = points->GetEntriesFast();
	for(Int_t ip = 0; ip < nPoints; ip++){
	    point = (CbmRichPoint*) points->At(ip);
	    if(NULL == point) continue;
	    trackId = point->GetTrackID();
	    track = (CbmMCTrack*) tracks->At(trackId);
            if(NULL == track) continue;
	    pdg = track->GetPdgCode();
            if(50000050 != pdg) continue;
	    energy = TMath::Sqrt(TMath::Power(track->GetPx(),2)+
				 TMath::Power(track->GetPy(),2)+
				 TMath::Power(track->GetPz(),2));
            lambda = c/n*h/e/energy;
            h_acc->Fill(lambda);
	}// points

	nHits = hits->GetEntriesFast();
	for(Int_t ih = 0; ih < nHits; ih++){
	    hit = (CbmRichHit*) hits->At(ih);
	    if(NULL == hit) continue;
	    pointId = hit->GetRefId();
            if(-1 == pointId) continue; // noise hit
	    point = (CbmRichPoint*) points->At(pointId);
	    if(NULL == point) continue;
	    trackId = point->GetTrackID();
	    track = (CbmMCTrack*) tracks->At(trackId);
            if(NULL == track) continue;
	    pdg = track->GetPdgCode();
            if(50000050 != pdg) continue;
	    energy = TMath::Sqrt(TMath::Power(track->GetPx(),2)+
				 TMath::Power(track->GetPy(),2)+
				 TMath::Power(track->GetPz(),2));
            lambda = c/n*h/e/energy;
            h_phot->Fill(lambda, hit->GetNPhotons());
            h_hits->Fill(lambda);
	}// points

	if(0 == (iev%100)) cout << iev << endl;
    }// events


    h_all->Scale(1./(Float_t)nev);
    h_acc->Scale(1./(Float_t)nev);
    h_phot->Scale(1./(Float_t)nev);
    h_hits->Scale(1./(Float_t)nev);
    h_all->Write();
    h_acc->Write();
    h_phot->Write();
    h_hits->Write();
    ofile->Close();
}






