
void yields_urqmd()
{
    Float_t timeResolution = 80;

    // Load libraries
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libField");
    gSystem->Load("libCbm");
    gSystem->Load("libGen");
    gSystem->Load("libPassive");
    gSystem->Load("libSts");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libTof");
    gSystem->Load("libEcal");
    gSystem->Load("libGlobal");
    gSystem->Load("libHadron");



    char strMCFile[1000];
    char strParamFile[1000];
    char strRecoStsFile[1000];
    char strRecoFile[1000];
    char strOutputFile[1000];

    char *ver = getenv("CBMVER");

    sprintf(strOutputFile, "dummy.root");

    sprintf(strMCFile, "/d/cbm02/kresan/mc/%s/urqmd/\
auau/25gev/centr/urqmd.auau.25gev.centr.0000.mc.root", ver);
    sprintf(strParamFile, "/d/cbm02/kresan/mc/%s/urqmd/\
auau/25gev/centr/urqmd.auau.25gev.centr.0000.mc.param.root", ver);
    sprintf(strRecoStsFile, "/d/cbm02/kresan/reco/%s/urqmd/\
auau/25gev/centr/urqmd.auau.25gev.centr.0000.recoSts.root", ver);
    sprintf(strRecoFile, "/d/cbm02/kresan/reco/%s/urqmd/\
auau/25gev/centr/%1.0fps/urqmd.auau.25gev.centr.0000.%1.0fps.reco.root",
	    ver, timeResolution, timeResolution);


    CbmRunAna* fRun = new CbmRunAna();
    fRun->SetInputFile(strMCFile);
    fRun->AddFriend(strRecoStsFile);
    fRun->AddFriend(strRecoFile);

    for(Int_t iFile = 1; iFile < 10; iFile++) {
	if(28==iFile || 41==iFile || 52==iFile ||
	   55==iFile || 73==iFile) continue;

        sprintf(strMCFile, "/d/cbm02/kresan/mc/%s/urqmd/\
auau/25gev/centr/urqmd.auau.25gev.centr.%4d.mc.root", ver, iFile);
	sprintf(strRecoStsFile, "/d/cbm02/kresan/reco/%s/urqmd/\
auau/25gev/centr/urqmd.auau.25gev.centr.%4d.recoSts.root", ver, iFile);
	sprintf(strRecoFile, "/d/cbm02/kresan/reco/%s/urqmd/\
auau/25gev/centr/%1.0fps/urqmd.auau.25gev.centr.%4d.%1.0fps.reco.root",
		ver, timeResolution, iFile, timeResolution);
	for(Int_t i = 0; i < 1000; i++) {
            if(' ' == strMCFile[i]) strMCFile[i] = '0';
	    if(' ' == strRecoStsFile[i]) strRecoStsFile[i] = '0';
	    if(' ' == strRecoFile[i]) strRecoFile[i] = '0';
	}
	fRun->AddFile(strMCFile);
	fRun->AddFriend(strRecoStsFile);
	fRun->AddFriend(strRecoFile);
    }

    fRun->SetOutputFile(strOutputFile);

    // Init Simulation Parameters from Root File
    CbmRuntimeDb* rtdb=fRun->GetRuntimeDb();
    CbmParRootFileIo* input=new CbmParRootFileIo();
    input->open(strParamFile);
    rtdb->setFirstInput(input);



    // --------- Hadron Spectra --------------------------------
    CbmHadronYields* hadronYields = new CbmHadronYields("HadronYields", 1);
    fRun->AddTask(hadronYields);
    // ---------------------------------------------------------



    fRun->Init();


    fRun->Run();
}


