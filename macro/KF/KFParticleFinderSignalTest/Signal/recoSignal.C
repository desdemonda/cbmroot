//---------------------------------------------------------------------------------
// @author  M. Zyzak
// @version 1.0
// @since   15.08.14
// 
// macro to reconstruct signal events for KFParticleFinder
//_________________________________________________________________________________

void recoSignal(Int_t nEvents = 10000) {

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  TString inFile = "signal.mc.root";
  TString parFile = "signal.params.root";
  TString outFile = "signal.reco.root";

  //  Digitisation files.
  // Add TObjectString containing the different file names to
  // a TList which is passed as input to the FairParAsciiFileIo.
  // The FairParAsciiFileIo will take care to create on the fly 
  // a concatenated input parameter file which is then used during
  // the reconstruction.
  TList *parFileList = new TList();

  TString workDir = gSystem->Getenv("VMCWORKDIR");
  TString paramDir = workDir + "/parameters";

  TObjString stsDigiFile = paramDir + "/sts/sts_v13d_std.digi.par";
  parFileList->Add(&stsDigiFile);

  TObjString trdDigiFile =  paramDir + "/trd/trd_v13g.digi.par";
  parFileList->Add(&trdDigiFile);

  TObjString tofDigiFile =  paramDir + "/tof/tof_v13b.digi.par";
  parFileList->Add(&tofDigiFile);


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


  // -----   MVD Digitiser   -------------------------------------------------
  CbmMvdDigitizeL* mvdDigi =
  		new CbmMvdDigitizeL("MVD Digitiser", 0, iVerbose);
  run->AddTask(mvdDigi);
  // -------------------------------------------------------------------------
 

  // -----   STS digitizer   -------------------------------------------------
  Double_t threshold  =  4;
  Double_t noiseWidth =  0.01;
  Int_t    nofBits    = 12;
  Double_t electronsPerAdc    =  10;
  Double_t StripDeadTime = 0.1;
  CbmStsDigitize* stsDigitize = new CbmStsDigitize();
  stsDigitize->SetRealisticResponse();
  stsDigitize->SetFrontThreshold (threshold);
  stsDigitize->SetBackThreshold  (threshold);
  stsDigitize->SetFrontNoiseWidth(noiseWidth);
  stsDigitize->SetBackNoiseWidth (noiseWidth);
  stsDigitize->SetFrontNofBits   (nofBits);
  stsDigitize->SetBackNofBits    (nofBits);
  stsDigitize->SetFrontNofElPerAdc(electronsPerAdc);
  stsDigitize->SetBackNofElPerAdc(electronsPerAdc);
  stsDigitize->SetStripDeadTime  (StripDeadTime);
  run->AddTask(stsDigitize);
  // -------------------------------------------------------------------------


  // =========================================================================
  // ===                     MVD local reconstruction                      ===
  // =========================================================================


  // -----   MVD Hit Finder   ------------------------------------------------
  CbmMvdFindHits* mvdHitFinder = new CbmMvdFindHits("MVD Hit Finder", 0,
  		iVerbose);
  run->AddTask(mvdHitFinder);
  // -------------------------------------------------------------------------


  // ===                 End of MVD local reconstruction                   ===
  // =========================================================================




  // =========================================================================
  // ===                      STS local reconstruction                     ===
  // =========================================================================


  // -----   STS Cluster Finder   --------------------------------------------
  FairTask* stsClusterFinder = new CbmStsClusterFinder();
  run->AddTask(stsClusterFinder);
  // -------------------------------------------------------------------------


  // -----   STS hit finder   ------------------------------------------------
  FairTask* stsFindHits = new CbmStsFindHits();
  run->AddTask(stsFindHits);
  // -------------------------------------------------------------------------


  // -----  STS hit matching   -----------------------------------------------
  FairTask* stsMatchHits = new CbmStsMatchHits();
  run->AddTask(stsMatchHits);
  // -------------------------------------------------------------------------


  // ---  STS track finding   ------------------------------------------------
  CbmKF* kalman = new CbmKF();
  run->AddTask(kalman);
  CbmL1* l1 = new CbmL1("CbmL1",1, 3);
  TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
  const TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v13d.root";
  l1->SetMaterialBudgetFileName(stsMatBudgetFileName.Data());
  run->AddTask(l1);
  CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
  FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder);
  run->AddTask(stsFindTracks);
  // -------------------------------------------------------------------------


  // ---   STS track matching   ----------------------------------------------
   CbmMatchRecoToMC* matchTask = new CbmMatchRecoToMC();
   run->AddTask(matchTask);
  // -------------------------------------------------------------------------

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
