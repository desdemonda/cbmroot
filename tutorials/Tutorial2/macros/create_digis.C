void create_digis(){

    TStopwatch timer;
    timer.Start();

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();


    // Load this example libraries
    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libBase");
    gSystem->Load("libCbmBase");
    gSystem->Load("libPassive");
    gSystem->Load("libGen");
    gSystem->Load("libTutorial2");

    TString dir = getenv("VMCWORKDIR");
    TString tutdir = dir + "/tutorials/Tutorial2";

    TString inFile = tutdir + "/macros/tutorial2_pions.mc_p2.000_t0_n10.root";
    TString paramFile = tutdir + "/macros/tutorial2_pions.params_p2.000_t0_n10.root";
    TString outFile = "./digis.mc.root";

    cout << "******************************" << endl;
    cout << "InFile: " << inFile << endl;
    cout << "ParamFile: " << paramFile << endl;
    cout << "OutFile: " << outFile << endl;
    cout << "******************************" << endl;

    FairRunAna *fRun= new FairRunAna();
    fRun->SetInputFile(inFile);
    fRun->SetOutputFile(outFile);


    // Init Simulation Parameters from Root File
    FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
    FairParRootFileIo* io1=new FairParRootFileIo();
    io1->open(paramFile.Data(),"UPDATE");
 
    FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
    TString tutDetDigiFile = gSystem->Getenv("VMCWORKDIR");
    tutDetDigiFile += "/tutorials/Tutorial2/macros/tutdet.digi.par";
    parInput2->open(tutDetDigiFile.Data(),"in");

    rtdb->setFirstInput(io1);
    rtdb->setSecondInput(parInput2);

    rtdb->print();

    //**  TUt Det Digi Producer **//

    CbmTutorialDetDigitizer *digi = new CbmTutorialDetDigitizer("tutdet","tut det task");

    // add the task
    fRun->AddTask( digi );
    


    fRun->LoadGeometry();
    fRun->Init();

    rtdb->getContainer("CbmTutorialDetDigiPar")->print();

    CbmTutorialDetDigiPar* DigiPar = (CbmTutorialDetDigiPar*) 
                                      rtdb->getContainer("CbmTutorialDetDigiPar");

    DigiPar->setChanged();
    DigiPar->setInputVersion(fRun->GetRunId(),1);
//    DigiPar->printparams();
    rtdb->setOutput(io1);
    rtdb->saveOutput();
    rtdb->print();

    fRun->Run();


    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}
