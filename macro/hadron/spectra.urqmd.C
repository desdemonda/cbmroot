
void spectra_urqmd(Float_t beamMomentum,
		   Float_t timeResolution,
		   Float_t purity)
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


    char strMCFile[1000];
    char strParamFile[1000];
    char strHadronFile[1000];
    char strOutputFile[1000];
    char strMkdir[2000];
    char strPdfFile[1000];

    char *ver = getenv("CBMVER");

    sprintf(strOutputFile, "/lustre/cbm/user/kresan/hadron/%s/urqmd/\
auau/%1.0fgev/centr", ver, beamMomentum);
    sprintf(strMkdir, "mkdir -p %s", strOutputFile);
    system(strMkdir);
    sprintf(strOutputFile, "%s/urqmd.auau.%1.0fgev.centr.%1.0fps.%1.0fpercent.hadron.spectra",
	    strOutputFile, beamMomentum, timeResolution, purity);
    sprintf(strOutputFile, "%s.root", strOutputFile);

    sprintf(strMCFile, "/lustre/cbm/user/kresan/mc/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.0000.mc.root", ver, beamMomentum, beamMomentum);
    sprintf(strParamFile, "/lustre/cbm/user/kresan/mc/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.0000.mc.param.root", ver, beamMomentum, beamMomentum);
    sprintf(strHadronFile, "/lustre/cbm/user/kresan/dst/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.0000.dst.root", ver, beamMomentum, beamMomentum);

    sprintf(strPdfFile, "/lustre/cbm/user/kresan/hadron/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.%1.0fps.%1.0fpercent.fitted.reco.root",
	    ver, beamMomentum, beamMomentum, timeResolution, purity);
/*    sprintf(strPdfFile, "/lustre/cbm/user/kresan/hadron/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.%1.0fps.%1.0fpercent.fitted.root",
	    ver, beamMomentum, beamMomentum, timeResolution, purity);*/


    FairRunAna* fRun = new FairRunAna();
    fRun->SetInputFile(strMCFile);
    fRun->AddFriend(strHadronFile);

    for(Int_t iFile = 1; iFile < 200; iFile++) {
        sprintf(strMCFile, "/lustre/cbm/user/kresan/mc/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.%4d.mc.root", ver, beamMomentum, beamMomentum, iFile);
	sprintf(strHadronFile, "/lustre/cbm/user/kresan/dst/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.%4d.dst.root", ver, beamMomentum, beamMomentum, iFile);
	for(Int_t i = 0; i < 1000; i++) {
            if(' ' == strMCFile[i]) strMCFile[i] = '0';
	    if(' ' == strHadronFile[i]) strHadronFile[i] = '0';
	}
	fRun->AddFile(strMCFile);
	fRun->AddFriend(strHadronFile);
    }

    fRun->SetOutputFile(strOutputFile);

    // Init Simulation Parameters from Root File
    FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
    FairParRootFileIo* input=new FairParRootFileIo();
    input->open(strParamFile);
    rtdb->setFirstInput(input);



    // --------- Hadron Spectra --------------------------------
    CbmHadronSpectra* hadronSpectra = new CbmHadronSpectra("HadronSpectra", 1);
    hadronSpectra->SetPdfFileName(TString(strPdfFile));
    hadronSpectra->SetBeamMomentum(TMath::Nint(beamMomentum));
    fRun->AddTask(hadronSpectra);
    // ---------------------------------------------------------



    fRun->Init();


    fRun->Run();


    hadronSpectra->WriteHistogramms();
}


