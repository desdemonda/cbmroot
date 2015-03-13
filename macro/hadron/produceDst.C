
void produceDst(Float_t beamMomentum, Int_t index)
{
    TStopwatch timer;
    timer.Start();
    gDebug=0;
    Int_t nEvents = 1000;



    // Load libraries -----------------------------------------------
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
    gSystem->Load("libKF");
    gSystem->Load("libHadron");
    // --------------------------------------------------------------



    // File names ---------------------------------------------------
    char strInputFile[1000];
    char strParamFile[1000];
    char strRecoStsFile[1000];
    char strRecoFile[1000];
    char strOutputFile[1000];

    char *ver = getenv("CBMVER");

    sprintf(strInputFile, "/home/kresan/data/mc/%s/urqmd/auau/%1.0fgev/centr",
	    ver, beamMomentum);
    sprintf(strOutputFile, "/home/kresan/data/dst/%s/urqmd/auau/%1.0fgev/\
centr/", ver, beamMomentum);

    sprintf(strParamFile, "%s/urqmd.auau.%1.0fgev.centr.%4d.mc.param.root",
	    strInputFile, beamMomentum, index);
    sprintf(strInputFile, "%s/urqmd.auau.%1.0fgev.centr.%4d.mc.root",
	    strInputFile, beamMomentum, index);
    sprintf(strRecoFile, "/home/kresan/data/reco/%s/urqmd/auau/%1.0fgev/\
centr/urqmd.auau.%1.0fgev.centr.%4d.reco.root",
            ver, beamMomentum, beamMomentum, index);
    sprintf(strOutputFile, "%s/urqmd.auau.%1.0fgev.centr.%4d.dst.root",
	    strOutputFile, beamMomentum, index);

    for(Int_t i = 0; i < 1000; i++) {
        if(' ' == strInputFile[i]) strInputFile[i] = '0';
        if(' ' == strParamFile[i]) strParamFile[i] = '0';
        if(' ' == strRecoFile[i]) strRecoFile[i] = '0';
        if(' ' == strOutputFile[i]) strOutputFile[i] = '0';
    }
    // --------------------------------------------------------------



    FairRunAna *fRun= new FairRunAna();
    fRun->SetInputFile(strInputFile);
    fRun->AddFriend(strRecoFile);
    fRun->SetOutputFile(strOutputFile);



    // -----  Parameter database   --------------------------------------------
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    parIo1->open(strParamFile);
    rtdb->setFirstInput(parIo1);
    // ------------------------------------------------------------------------



    // -----  Produce DST  ----------------------------------------------------
    CbmKF* kalman = new CbmKF();
    fRun->AddTask(kalman);

    CbmGlobalTrackFitterKF *globalTrackFitter = new CbmGlobalTrackFitterKF();
    CbmFitGlobalTracks *fitGlobal = new CbmFitGlobalTracks("FitGlobalTracks", 1,
							   globalTrackFitter);
    fRun->AddTask(fitGlobal);

    CbmProduceDst *produceDst = new CbmProduceDst();
    fRun->AddTask(produceDst);
    // ------------------------------------------------------------------------



    fRun->Init();


    fRun->Run(0, nEvents);


    produceDst->WriteHisto();


    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}

