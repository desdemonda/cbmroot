
#include "drawStyle.C"
#include "/u/kresan/cbm/trunk_feb12/cbmroot/cbmbase/CbmDetectorList.h"




void acceptance(Float_t beamMomentum)
{
    // Load libraries
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
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libTof");
    gSystem->Load("libEcal");
    gSystem->Load("libGlobal");
    gSystem->Load("libHadron");



    TChain *chain = new TChain("cbmsim");
    char strFileName[1000];
    for(Int_t i = 0; i < 10; i++) {
	sprintf(strFileName, "/lustre/cbm/user/kresan/mc/head/urqmd/auau/%1.0fgev/centr/\
urqmd.auau.%1.0fgev.centr.%4d.mc.root", beamMomentum, beamMomentum, i);
	for(Int_t k = 0; k < 1000; k++) {
	    if(' ' == strFileName[k]) {
                strFileName[k] = '0';
	    }
	}

        chain->AddFile(strFileName);
    }



    TClonesArray *mcTracks = new TClonesArray("CbmMCTrack");
    chain->SetBranchAddress("MCTrack", &mcTracks);



    CbmMCTrack *track;
    Int_t pdgCode;
    Int_t n_pip_all = 0;
    Int_t n_kap_all = 0;
    Int_t n_prot_all = 0;
    Int_t n_pip_acc = 0;
    Int_t n_kap_acc = 0;
    Int_t n_prot_acc = 0;
    for(Int_t iev = 0; iev < chain->GetEntries(); iev++) {
	chain->GetEntry(iev);


	for(Int_t itrack = 0; itrack < mcTracks->GetEntriesFast(); itrack++) {
	    track = (CbmMCTrack*) mcTracks->At(itrack);
	    if(NULL == track) continue;

	    if(-1 != track->GetMotherId()) continue;

	    pdgCode = track->GetPdgCode();

	    if(211 == pdgCode) {
		n_pip_all += 1;
	    } else if(321 == pdgCode) {
		n_kap_all += 1;
	    } else if(2212 == pdgCode) {
		n_prot_all += 1;
	    }

	    // Select accepted tracks
	    if(track->GetNPoints(kSTS) >= 4 &&
	       track->GetNPoints(kTRD) >= 9 &&
	       track->GetNPoints(kTOF) >= 1) {
		if(211 == pdgCode) {
		    n_pip_acc += 1;
		} else if(321 == pdgCode) {
		    n_kap_acc += 1;
		} else if(2212 == pdgCode) {
		    n_prot_acc += 1;
		}
	    }
	}


	if(0 == (iev%100)) {
	    cout << "Event: " << iev << ",  MCTracks: " << mcTracks->GetEntriesFast() << endl;
	}
    }



    cout << endl
	<< "pip  : " << (Float_t)n_pip_acc/(Float_t)n_pip_all*100 << endl
	<< "kap  : " << (Float_t)n_kap_acc/(Float_t)n_kap_all*100 << endl
	<< "prot : " << (Float_t)n_prot_acc/(Float_t)n_prot_all*100 << endl
        << endl;
}







