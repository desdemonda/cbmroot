

{
    const Float_t el_a_min = 4.4;
    const Float_t el_a_max = 8.;


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


    TString inFileName1 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.mc.root";
    TString inFileName2 = "/d/cbm02/kresan/rich_prot/may11/epi.standard.reco.root";
    TFile *inFile1 = new TFile(inFileName1);
    TFile *inFile2 = new TFile(inFileName2);
    TTree *cbmsim1 = (TTree*) inFile1->Get("cbmsim");
    TTree *cbmsim2 = (TTree*) inFile2->Get("cbmsim");
    TClonesArray *tracks = new TClonesArray("CbmMCTrack");
    TClonesArray *rings = new TClonesArray("CbmRichRing");
    TClonesArray *projs = new TClonesArray("FairTrackParam");
    cbmsim1->SetBranchAddress("MCTrack", &tracks);
    cbmsim2->SetBranchAddress("RichRing", &rings);
    cbmsim2->SetBranchAddress("RichProjection", &projs);

    cbmsim1->SetBranchStatus("*", 0);
    cbmsim2->SetBranchStatus("*", 0);
    cbmsim1->SetBranchStatus("MCTrack.fMotherId", 1);
    cbmsim1->SetBranchStatus("MCTrack.fPdgCode", 1);
    cbmsim1->SetBranchStatus("MCTrack.fPx", 1);
    cbmsim1->SetBranchStatus("MCTrack.fPy", 1);
    cbmsim1->SetBranchStatus("MCTrack.fPz", 1);
    cbmsim2->SetBranchStatus("RichRing.fCenterX", 1);
    cbmsim2->SetBranchStatus("RichRing.fCenterY", 1);
    cbmsim2->SetBranchStatus("RichRing.fAaxis", 1);
    cbmsim2->SetBranchStatus("RichRing.fBaxis", 1);
    //cbmsim2->SetBranchStatus("RichRing.fRadius", 1);
    cbmsim2->SetBranchStatus("RichRing.fNofHitsOnRing", 1);
    cbmsim2->SetBranchStatus("RichProjection.fX", 1);
    cbmsim2->SetBranchStatus("RichProjection.fY", 1);


    Int_t nr;
    Int_t nr_det;
    Int_t nev = cbmsim2->GetEntriesFast();
    Int_t nev_el_true = 0;
    Int_t nev_el_found = 0;
    Int_t nev_el_pid = 0;
    Int_t nev_el_fake = 0;
    Int_t nev_el_pidpi = 0;
    Int_t nev_el_bad = 0;
    Int_t nev_pi_true = 0;
    Int_t nev_pi_cnkv = 0;
    Int_t nev_pi_found = 0;
    Int_t nev_pi_pid = 0;
    Int_t nev_pi_fake = 0;
    Int_t nev_pi_pidel = 0;
    Int_t nev_pi_bad = 0;
    Int_t nTracks;
    CbmMCTrack *track;
    Int_t pdg;
    CbmRichRing *ring;
    FairTrackParam *proj;
    Float_t dist;
    Float_t min_dist;
    Int_t index;
    Float_t a;
    Float_t b;
    Float_t p;
    Int_t pdg1;
    for(Int_t i = 0; i < nev; i++) {
	cbmsim1->GetEntry(i);
	cbmsim2->GetEntry(i);

	// Loop over primary tracks and determine event type
        nTracks = tracks->GetEntriesFast();
	for(Int_t it = 0; it < nTracks; it++) {
            // Get pointer to track
	    track = (CbmMCTrack*) tracks->At(it);
            // Check pointer
	    if(NULL == track) continue;
            // Selct only primaries
	    if(-1 != track->GetMotherId()) continue;
            // Get pdg code
	    pdg = track->GetPdgCode();
            // Check event type and increment respective counter
	    if(11 == pdg) {
                // electron counter
                nev_el_true += 1;
	    } else if(-211 == pdg) {
                // pion counter
                nev_pi_true += 1;
	    }
            // Get momentum
	    p = track->GetP();
            break;
	}

	// Check if pion is above the cherenkov threshold
	if(-211 == pdg && p > 5.6) {
            // Increment counter
            nev_pi_cnkv += 1;
	}

	// Select one ring
        // Get number of found rings
	nr = rings->GetEntriesFast();
	if(0 == nr) {
            // If no rings found, set index to invalid value
            index = -1;
	} else if(1 == nr) {
            // If single ring is found, set index to 0
            index = 0;
	} else {
            // Run selection
            cout << "Event: " << i << " - " << nr << " rings. Running selection..." << endl;
	    nr_det = 0;
	    min_dist = 100;
            index = -1;
            // Chose the closest ring to projection
	    for(Int_t ir = 0; ir < nr; ir++) {
                // Get ring
		ring = (CbmRichRing*) rings->At(ir);
                // Get projection
		proj = (FairTrackParam*) projs->At(0);
                // Check memory
		if(NULL == ring || NULL == proj) continue;
                // Calculate distance
		dist = TMath::Sqrt(TMath::Power(ring->GetCenterX()-proj->GetX(),2)+
				   TMath::Power(ring->GetCenterY()-proj->GetY(),2));
		// Cut on Nhits
                //if(ring->GetNofHitsOnRing() <= 5) continue;
                // Select closest
		if(dist < min_dist) {
		    min_dist = dist;
                    index = ir;
		}
	    }
            // Check for distance
	    if(index >= 0) {
                nr_det += 1;
	    }
	    if(0 == nr_det) {
		cout << "Event: " << i << " - NO DETECTED RINGS!!!" << endl;
	    } else if(1 == nr_det) {
		//cout << "             selected ring " << index
		//    << " with distance " << min_dist << endl;
	    }
	}

	// Identify ring
        // Check index
	if(index < 0) continue;
        // Get ring
	ring = (CbmRichRing*) rings->At(index);
	// Check memory
	if(NULL == ring) continue;
        // Get ellipse parameters
	a = ring->GetAaxis();
	b = ring->GetBaxis();
        // Cut on the a axis value
	if(a > el_a_max || a < el_a_min) {
            // Definitly pion
            pdg1 = -211;
	} else {
            // Probably electron
            pdg1 = 11;
	}
	// Increment respective counter
	if(11 == pdg1 && 11 == pdg) {
            nev_el_pid += 1;
	    if((b/a) < 0.9) {
		nev_el_bad += 1;
		//cout << "Event: " << i << ", b/a=" << (b/a) << " !!!!!" << endl;
	    }
	} else if(-211 == pdg1 && -211 == pdg) {
	    if(p <= 5.) {
		nev_pi_fake += 1;
	    } else {
		nev_pi_pid += 1;
	    }
	    if((b/a) < 0.9) {
		nev_pi_bad += 1;
		//cout << "Event: " << i << ", b/a=" << (b/a) << " !!!!!" << endl;
	    }
	}
	// Check for missidentification
	if(pdg1 != pdg) {
	    if(11 == pdg1 && -211 == pdg) {
                nev_el_fake += 1;
	    } else if(-211 == pdg1 && 11 == pdg) {
                nev_pi_fake += 1;
	    }
	    if(11 == pdg) {
                nev_el_pidpi += 1;
	    } else if(-211 == pdg) {
                nev_pi_pidel += 1;
	    }
	}


	if(0 == (i%1000)) {
            cout << i << endl;
	}
    }


    cout << endl;
    cout << "Number of electrons: " << nev_el_true << endl;
    cout << "Number of identified electrons: " << nev_el_pid << "   " << (100.*nev_el_pid/nev_el_true) << "%" << endl;
    cout << "Number of fake electrons: " << nev_el_fake << "   " << (100.*nev_el_fake/nev_el_true) << "%" << endl;
    cout << "Number of MISSidentified electrons: " << nev_el_pidpi << "   " << (100.*nev_el_pidpi/nev_el_true) << "%" << endl;
    cout << endl;
    cout << "Number of pions: " << nev_pi_true << endl;
    cout << "Number of pions above threshold: " << nev_pi_cnkv << "   " << (100.*nev_pi_cnkv/nev_pi_true) << "%" << endl;
    cout << "Number of identified pions: " << nev_pi_pid << "   " << (100.*nev_pi_pid/nev_pi_cnkv) << "%" << endl;
    cout << "Number of fake pions: " << nev_pi_fake << "   " << (100.*nev_pi_fake/nev_pi_cnkv) << "%" << endl;
    cout << "Number of MISSidentified pions: " << nev_pi_pidel << "   " << (100.*nev_pi_pidel/nev_pi_true) << "%" << endl;
    cout << endl;
    cout << "Bad electron rings: " << nev_el_bad << "   " << (100.*nev_el_bad/nev_el_pid) << "%" << endl;
    cout << "Bad pion rings: " << nev_pi_bad << "   " << (100.*nev_pi_bad/nev_pi_pid) << "%" << endl;
    cout << endl;
}



