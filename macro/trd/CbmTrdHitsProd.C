//  Macro for producing hits in the TRD + computing the Transition
//  Radiation (see CbmTrdRadiator.cxx & .h)
{
    TStopwatch timer;
    timer.Start();

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();


    // Load this example libraries
    gSystem->Load("libGeoBase");
    gSystem->Load("libBase");
    gSystem->Load("libPassive");
    gSystem->Load("libSts");
    gSystem->Load("libTof");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libEcal");
    gSystem->Load("libGen");
    gSystem->Load("libKF");


    TString inFile = "test.mc.root";
    TString paramFile = "test.mc.param.root";
    TString bgFile = "test.mc.root";
    TString outFile = "test.trdHits.root";
    TString version = "v01a";

    FairRunAna *fRun= new FairRunAna();
    fRun->SetInputFile(inFile);
    fRun->SetOutputFile(outFile);


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


     // Update of the values for the radiator F.U. 17.08.07
     Int_t trdNFoils    = 130;      // number of polyetylene foils
     Float_t trdDFoils = 0.0013;    // thickness of 1 foil [cm]
     Float_t trdDGap   = 0.02;      // thickness of gap between foils [cm]
     Bool_t simpleTR = kTRUE;       // use fast and simple version for TR
                                    // production
   
     CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , trdNFoils,
                                          trdDFoils, trdDGap);
   
     CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
     run->AddTask(trdHitProd);

    // -------- Performance of hit producer ------------
    CbmTrdHitProducerQa*  trdHitQa =
        new CbmTrdHitProducerQa("HitProducerPerformance");
    fRun->AddTask(trdHitQa);
    // -------------------------------------------------



    fRun->Init();
    fRun->Run();

    // ---- store the performance information
    trdHitQa->WriteHistograms();
    // --------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}


