// Macro for merging of STS and TRD tracks and TOF hits

{
    TStopwatch timer;
    timer.Start();
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    gSystem->Load("libGeoBase");
    gSystem->Load("libBase");
    gSystem->Load("libPassive");
    gSystem->Load("libSts");
    gSystem->Load("libTof");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libGen");
    gSystem->Load("libKF");
    gSystem->Load("libGlobal");
    gSystem->Load("libL1");


    TString mcFile = "test.mc.root";
    TString paramFile = "test.mc.param.root";
    TString stsFile = "test.stsTracks.root";
    TString trdFile = "test.trdHitsTracks.root";
    TString outFile = "test.globalTracks.root";


    FairRunAna *fRun= new FairRunAna();
    fRun->SetInputFile(mcFile.Data());
    fRun->AddFriend(stsFile.Data());
    fRun->AddFriend(trdFile.Data());
    fRun->SetOutputFile(outFile.Data());


    // Init Simulation Parameters from Root File
    FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
    FairParRootFileIo* io1=new FairParRootFileIo();
    io1->open(paramFile.Data());

    // Init Digitization Parameters from Ascii File
    //FairParAsciiFileIo* input2 = new FairParAsciiFileIo();
    //TString StsDigiFile(gSystem->Getenv("VMCWORKDIR"));
    //StsDigiFile = StsDigiFile + "/parameters/sts/sts_digi.par";
    //input2->open(StsDigiFile.Data(),"in");

    rtdb->setFirstInput(io1);
    //rtdb->setSecondInput(input2);


    // TOF Hit producer ---------------------------------------------
    CbmTofHitProducer *hp = new CbmTofHitProducer("tof", 1);
    Double_t SigmaXY = 0.05 ;
    Double_t SigmaT = 8.E-2;
    hp->SetSigmaXY(SigmaXY);
    hp->SetSigmaT(SigmaT);
    fRun->AddTask(hp);
    // --------------------------------------------------------------


    // Kalman filter
    CbmKF* kf = new CbmKF();

    // Track Merger
    CbmL1TrackMerger* trackMerger = new CbmL1TrackMerger();
    trackMerger->SetMethod(1);

    // Tof Merger
    CbmL1TofMerger* tofMerger = new CbmL1TofMerger();


    // Global track finding -----------------------------------------
    CbmFindGlobalTracks* findGlobal = new CbmFindGlobalTracks(trackMerger,
							      NULL,
							      tofMerger,
							      1);
    fRun->AddTask(findGlobal);
    // --------------------------------------------------------------


    CbmFindGlobalTracksQa* findGlobalQa =
	new CbmFindGlobalTracksQa("FindGlobalQa", 1);
    findGlobalQa->SetRequireTof(kTRUE);
    fRun->AddTask(findGlobalQa);


    fRun->Init();


    fRun->Run();


    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);


    findGlobalQa->WriteHistogramms();
}
