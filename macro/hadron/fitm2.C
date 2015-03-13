
void fitm2(Float_t beamMomentum,
	   Float_t timeResolution,
	   Float_t purity,
	   Int_t level)
{
    if(level<1 || level>3) return;

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


    char strInputFile[1000];
    char strParamFile[1000];
    char strOutputFile[1000];
    char strMkdir[2000];

    char *ver = getenv("CBMVER");

    sprintf(strOutputFile, "/lustre/cbm/user/kresan/hadron/%s/urqmd/\
auau/%1.0fgev/centr", ver, beamMomentum);
    sprintf(strMkdir, "mkdir -p %s", strOutputFile);
    system(strMkdir);
    sprintf(strParamFile, "/lustre/cbm/user/kresan/mc/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.0000.mc.param.root", ver, beamMomentum, beamMomentum);
    sprintf(strInputFile, "/lustre/cbm/user/kresan/hadron/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.%1.0fps.m2mom", ver, beamMomentum, beamMomentum, timeResolution);
    sprintf(strOutputFile, "%s/urqmd.auau.%1.0fgev.centr.%1.0fps.%1.0fpercent.fitted",
	    strOutputFile, beamMomentum, timeResolution, purity);
    if(2 == level) {
	sprintf(strInputFile, "%s.recoIdeal", strInputFile);
	sprintf(strOutputFile, "%s.recoIdeal", strOutputFile);
    } else if(3 == level) {
	sprintf(strInputFile, "%s.reco", strInputFile);
	sprintf(strOutputFile, "%s.reco", strOutputFile);
    }
    sprintf(strInputFile, "%s.root", strInputFile);
    sprintf(strOutputFile, "%s.root", strOutputFile);



    FairRunAna* fRun = new FairRunAna();
    fRun->SetOutputFile(strOutputFile);
    fRun->SetInputFile(strInputFile);



    // Init Simulation Parameters from Root File
    FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
    FairParRootFileIo* input=new FairParRootFileIo();
    input->open(strParamFile);
    rtdb->setFirstInput(input);



    // --------- Hadron Identification -----------------------
    // create and add hadronTask
    CbmFitM2* hadronTask = new CbmFitM2("FitM2", 1);
    hadronTask->SetPurity(purity / 100.);
    hadronTask->SetLevel(level);
    fRun->AddTask(hadronTask);
    // -------------------------------------------------------



    fRun->Init();


//    fRun->Run();


    hadronTask->CreatePDF();


    hadronTask->WriteHistogramms();
}


