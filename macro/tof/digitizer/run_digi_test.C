// --------------------------------------------------------------------------
//
// Macro for TOF digitzer testing
//
// Digitizer in TOF
// Clusterizer in TOF
// Test class for TOF (Point/Digi/Hit)
//
// P.-A. Loizeau   13/03/2015
// Version         13/03/2015 (P.-A. Loizeau)
//
// --------------------------------------------------------------------------


void run_digi_test(Int_t nEvents = 2, const char* setup = "sis100_electron")
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // ----- Paths and file names  --------------------------------------------
  TString inDir   = gSystem->Getenv("VMCWORKDIR");

  TString outDir  = "data/";

  // Input file (MC events)
  TString inFile = outDir + setup + "_test.mc.root";
  // Parameter file
  TString parFile = outDir + setup + "_params.root";

  // Output file
  TString outFile = outDir + setup + "_digitizerTest.eds.root";

  TString setupFile = inDir + "/geometry/setup/" + setup + "_setup.C";
  TString setupFunct = setup;
  setupFunct += "_setup()";
  
  gROOT->LoadMacro(setupFile);
  gInterpreter->ProcessLine(setupFunct);

  //  Digitisation files.
  // Add TObjectString containing the different file names to
  // a TList which is passed as input to the FairParAsciiFileIo.
  // The FairParAsciiFileIo will take care to create on the fly
  // a concatenated input parameter file which is then used during
  // the reconstruction.
  TList *parFileList = new TList();

  TString workDir = gSystem->Getenv("VMCWORKDIR");
  TString paramDir = workDir + "/parameters/";

  TObjString tofDigiFile = paramDir + tofDigi;
  parFileList->Add(&tofDigiFile);

  TObjString tofDigiBdfFile = paramDir + "tof/tof.digibdf.par";
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
  tofDigitizerBdf->SetInputFileName( paramDir + "tof/test_bdf_input.root"); // Required as input file name not read anymore by param class
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
