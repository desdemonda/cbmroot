// --------------------------------------------------------------------------
//
// Macro for TOF digitzer testing
//
// Digitizer in TOF
// Clusterizer in TOF
// Test class for TOF (Point/Digi/Hit)
//
// P.-A. Loizeau   30/09/2013
// Version     30/09/2013 (P.-A. Loizeau)
//
// --------------------------------------------------------------------------


void run_digi(Int_t nEvents = 2) {

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
//  TString inFile = "data/25agev.mc.root";
  TString inFile = "/buffalo/ploizeau/cbmroot/2013_08_23/25agev_mbias.mc.root";
//  TString inFile = "/buffalo/ploizeau/cbmroot/pal_2013_08_27/300/25agev_mbias.mc.root";
  // Parameter file
//  TString parFile = "data/25agev.params.root";
  TString parFile = "/buffalo/ploizeau/cbmroot/2013_08_23/25agev_mbias.params.root";
//  TString parFile = "/buffalo/ploizeau/cbmroot/pal_2013_08_27/300/25agev_mbias.params.root";

  // Output file
  TString outFile = "digitizerTest.eds.root";

  //  Digitisation files.
  // Add TObjectString containing the different file names to
  // a TList which is passed as input to the FairParAsciiFileIo.
  // The FairParAsciiFileIo will take care to create on the fly
  // a concatenated input parameter file which is then used during
  // the reconstruction.
  TList *parFileList = new TList();

  TString workDir = gSystem->Getenv("VMCWORKDIR");
  TString paramDir = workDir + "/parameters";

  TObjString tofDigiFile =  paramDir + "/tof/tof_v13b.digi.par";
  parFileList->Add(&tofDigiFile);

  TObjString tofDigiBdfFile =  "./tof.digibdf.par";
  parFileList->Add(&tofDigiBdfFile);


  // In general, the following parts need not be touched
  // ========================================================================


  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run = new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------




  // =========================================================================
  // ===             Detector Response Simulation (Digitiser)              ===
  // ===                          (where available)                        ===
  // =========================================================================

  // -----   TOF digitizer   -------------------------------------------------
  CbmTofDigitizerBDF* tofDigitizerBdf = new CbmTofDigitizerBDF("TOF Digitizer BDF",iVerbose, kFALSE);
  run->AddTask(tofDigitizerBdf);


  // =========================================================================
  // ===                     TOF local reconstruction                      ===
  // =========================================================================

  // Cluster/Hit builder
  CbmTofSimpClusterizer* tofSimpClust = new CbmTofSimpClusterizer("TOF Simple Clusterizer",iVerbose, kFALSE);
  run->AddTask(tofSimpClust);

  // ------   TOF hit producer   ---------------------------------------------
/*
  CbmTofHitProducerNew* tofHitProd = new CbmTofHitProducerNew("TOF HitProducerNew",iVerbose);
  //  tofHitProd->SetParFileName(std::string(TofGeoPar));
  tofHitProd->SetInitFromAscii(kFALSE);
//  tofHitProd->SetSigmaT(  0.070 );
//  tofHitProd->SetSigmaEl( 0.040 );
  run->AddTask(tofHitProd);
*/
  // -------------------------------------------------------------------------

  // ===                   End of TOF local reconstruction                 ===
  // =========================================================================

  // =========================================================================
  // ===                     TOF evaluation                                ===
  // =========================================================================

  // Digitizer/custerizer testing
  CbmTofTests* tofTests = new CbmTofTests("TOF Tests",iVerbose);
  run->AddTask(tofTests);

  // ===                   End of TOF evaluation                           ===
  // =========================================================================
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(parFileList, "in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------


  // -----   Intialise and run   --------------------------------------------
  run->Init();
  cout << "Starting run" << endl;
  run->Run(0, nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is " << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

//  delete run;

   cout << " Test passed" << endl;
   cout << " All ok " << endl;
}
