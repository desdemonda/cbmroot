

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


    TString inFileName = "/d/cbm02/kresan/rich_prot/jul10/epi.standard.reco.root";
    TFile *inFile = new TFile(inFileName);
    TTree *cbmsim = (TTree*) inFile->Get("cbmsim");
    TClonesArray *rings = new TClonesArray("CbmRichRing");
    TClonesArray *projs = new TClonesArray("FairTrackParam");
    cbmsim->SetBranchAddress("RichRing", &rings);
    cbmsim->SetBranchAddress("RichProjection", &projs);


    Int_t nr;
    Int_t nr_det;
    Int_t nev = cbmsim->GetEntriesFast();
    Int_t nev_acc = 0;
    Int_t nev_cl = 0;
    Int_t nev_bad = 0;
    CbmRichRing *ring;
    FairTrackParam *proj;
    Float_t a;
    Float_t b;
    cout << endl;
    for(Int_t i = 0; i < nev; i++) {
	cbmsim->GetEntry(i);
        nr = rings->GetEntriesFast();
	if(0 == nr) {
	    cout << "Event: " << i << " - NO RINGS!!!" << endl;
	} else if(1 == nr) {
            nev_acc += 1;
	    ring = (CbmRichRing*) rings->At(0);
	    a = ring->GetAaxis();
	    b = ring->GetBaxis();
	    if((b/a) < 0.9) {
		nev_bad += 1;
		cout << "Event: " << i << ", b/a=" << (b/a) << " !!!!!" << endl;
	    }
	} else {
            cout << "Event: " << i << " - " << nr << " rings. Running selection..." << endl;
	    nr_det = 0;
	    Float_t dist;
	    Float_t min_dist = 100;
            Int_t index = -1;
	    for(Int_t ir = 0; ir < nr; ir++) {
		ring = (CbmRichRing*) rings->At(ir);
                proj = (FairTrackParam*) projs->At(0);
		if(NULL == ring || NULL == proj) {
                    continue;
		}
		dist = TMath::Sqrt(TMath::Power(ring->GetCenterX()-proj->GetX(),2)+
				   TMath::Power(ring->GetCenterY()-proj->GetY(),2));
		if(dist < min_dist) {
		    min_dist = dist;
                    index = ir;
		}
	    }
	    if(min_dist < 10.4 && index >= 0) {
                nr_det += 1;
	    }
	    if(0 == nr_det) {
		cout << "Event: " << i << " - NO DETECTED RINGS!!!" << endl;
	    } else if(1 == nr_det) {
		cout << "             selected ring " << index
		    << " with distance " << min_dist << endl;
                ring = (CbmRichRing*) rings->At(index);
		a = ring->GetAaxis();
		b = ring->GetBaxis();
		if((b/a) < 0.9) {
		    nev_bad += 1;
		    cout << "!!! Event: " << i << ", b/a=" << (b/a) << " !!!!!" << endl;
		}
		nev_acc += 1;
	    } else {
		cout << "Event: " << i << " - " << nr_det << " rings." << endl;
		nev_acc += 1;
		nev_cl += 1;
	    }
	}
    }


    cout << endl;
    cout << "Eff. : " << 100*(Float_t)nev_acc/(Float_t)nev << " %" << endl;
    if(nev_acc > 0) {
	cout << " Bad : " << 100*(Float_t)nev_bad/(Float_t)nev_acc << " %" << endl;
    }
    cout << "Fake : " << 100*(Float_t)nev_cl/(Float_t)nev << " %" << endl;
    cout << endl;
}



