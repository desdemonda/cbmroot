// --------------------------------------------------------------------------
//
// Macro for local MVD reconstruction from MC data
//
// Tasks:  CbmMvdHitProducer
// 
//
// V. Friese   06/02/2007
//
// --------------------------------------------------------------------------


{

    // ========================================================================
    //          Adjust this part according to your requirements

    TString inDir   = gSystem->Getenv("VMCWORKDIR");

    // Input file (MC events)
    TString inFile = inDir+ "/macro/mvd/qaData/mvd.mcCentral.root";

    // Parameter file name
    TString parFile =inDir+ "/macro/mvd/qaData/params.root";

    // Output file
    TString outFile =inDir+ "/macro/mvd/qaData/mvd.reco.root";

    // Background file (MC events, for pile-up)
    TString bgFile = inFile;

    // Delta file (Au Ions)
    TString deltaFile =inDir+ "/macro/mvd/qaData/mvd.mcDelta.root";

    // Number of events to process
    Int_t nEvents = 5;

    // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
    Int_t iVerbose = 0;

    // In general, the following parts need not be touched
    // ========================================================================


    // ----    Debug option   -------------------------------------------------
    gDebug = 0;
    // ------------------------------------------------------------------------


    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // ------------------------------------------------------------------------

    // ----  Load libraries   -------------------------------------------------
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libCLHEP");
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
    gSystem->Load("libGlobal");
    gSystem->Load("libEcal");
    gSystem->Load("libKF");
    gSystem->Load("libL1");

    // ------------------------------------------------------------------------



    // -----   Reconstruction run   -------------------------------------------
    FairRunAna *fRun= new FairRunAna();
    fRun->SetInputFile(inFile);
    fRun->SetOutputFile(outFile);
    // ------------------------------------------------------------------------



    // -------   MVD Digitiser   ----------------------------------------------
    CbmMvdDigitizeL* digi = new CbmMvdDigitizeL("MVDDigitiser", 0, iVerbose);
    cout << "Adding Task:  CbmMvdDigitiser... " << endl;
    fRun->AddTask(digi);

    // Parameters for tuning the digitizer, please use default except you are MAPS expert
    //digi->SetEpiThickness(0.0014);
    //digi->SetElectronsPerKeV(300);
    //digi->SetSegmentLength(0.0003);
    //digi->SetPixelSizeX(0.003);
    //digi->SetPixelSizeY(0.003);
    //digi->SetDiffusionCoef(0.0055); // adapted for 30um pixels
    //digi->SetDiffusionCoef(0.0018); // adapted for 10um pixels
    //digi->SetWidthOfCluster(3);
    
       
    
    //--- Pile Up -------
    
    Int_t pileUpInMVD=2; // use 1 or higher
    
    digi->SetBgFileName(bgFile);
    digi->SetBgBufferSize(5); //for simulation this buffer should contain > 2 * pileUpInMVD mBias events.
			      //default = few 100 (increase for high pile-up, reduce to limit memory consumption)
    digi->SetPileUp(pileUpInMVD-1);


    //--- Delta electrons -------
    digi->SetDeltaName(deltaFile);
    digi->SetDeltaBufferSize(50); //for simulation, this buffer must contain at least pileUpInMVD*200 delta electrons
    digi->SetDeltaEvents(2); //valid for QA-purposes only, use next line for simulations
    //digi->SetDeltaEvents(pileUpInMVD*100); // for simulation assumes 1% target

    //digi->ShowDebugHistograms();
    

    //----------------------------------------------------------------------------
    // -----   MVD Cluster Finder   ----------------------------------------------

    
    CbmMvdFindHits* mvd_hit   = new CbmMvdFindHits("MVDFindHits", 0, iVerbose);
    mvd_hit->SetSigmaNoise(15,kTRUE);  // kTRUE = add noise to digis, kFALSE = ideal detector
    mvd_hit->SetSeedThreshold(50); //in electrons!
    mvd_hit->SetNeighbourThreshold(1);
    mvd_hit->SetAdcBits(12);
    mvd_hit->SetAdcDynamic(4096);
    mvd_hit->SetHitPosErrX(0.0005);
    mvd_hit->SetHitPosErrY(0.0005);
    mvd_hit->ShowDebugHistograms();
    fRun->AddTask(mvd_hit);

    




















    //----------------------------------------------------------------------------
    // -----  Parameter database   -----------------------------------------------
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo*  parIo1 = new FairParRootFileIo();
    parIo1->open(parFile.Data());

    rtdb->setFirstInput(parIo1);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
    rtdb->print();
    fRun->LoadGeometry();
    // ---------------------------------------------------------------------------



    // -----   Run initialisation   ----------------------------------------------
    fRun->Init();
    // ---------------------------------------------------------------------------



    // -----   Start run   -------------------------------------------------------
    fRun->Run(0,nEvents);
    // ---------------------------------------------------------------------------



    // -----   Finish   ----------------------------------------------------------
    timer.Stop();

    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is "    << outFile << endl;
    cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ---------------------------------------------------------------------------


}
