// Macro to produce Tof Hit from transported file

{
    TStopwatch timer;
    timer.Start();
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libBase");
    gSystem->Load("libCbmBase");
    gSystem->Load("libCbmData");
    gSystem->Load("libCbmGenerators");
    gSystem->Load("libField");
    gSystem->Load("libGen");
    gSystem->Load("libPassive");
    gSystem->Load("libEcal");
    gSystem->Load("libKF");
    gSystem->Load("libMvd");
    gSystem->Load("libSts");
    gSystem->Load("libLittrack");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libTof");


    FairRunAna *fRun= new FairRunAna();
    //  fRun->SetInputFile("../run/auau.25gev.centr.mc.root");
    //fRun->SetOutputFile("auau.25gev.centr.tofHits.root");    
    fRun->SetInputFile("./data/auaumbias.root");
    fRun->SetOutputFile("./data/auaumbiasHits.root");


    // Init Simulation Parameters from Root File
    FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
    FairParRootFileIo* io1=new FairParRootFileIo();
//    io1->open("../run/parfiles/params.root");
    io1->open("./data/testparams.root");

    // Init Digitization Parameters from Ascii File
    //FairParAsciiFileIo* input2 = new FairParAsciiFileIo();
    //TString StsDigiFile(gSystem->Getenv("VMCWORKDIR"));
    //StsDigiFile = StsDigiFile + "/parameters/sts/sts_digi.par";
    //input2->open(StsDigiFile.Data(),"in");

    rtdb->setFirstInput(io1);
    //rtdb->setSecondInput(input2);


    CbmTofHitProducer *hp = new CbmTofHitProducer("tof", 1);
    Double_t SigmaXY = 0.2 ;   // check unit
    Double_t SigmaT = 8.E-2; // tof resol in ns
    hp->SetSigmaXY(SigmaXY);
    hp->SetSigmaT(SigmaT);
    fRun->AddTask( hp );


    fRun->Init();

    fRun->Run();


    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}
