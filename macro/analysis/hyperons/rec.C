void rec(Int_t file_nr=0, Int_t nEvents=1, Int_t s=0){
  Char_t filenr[4];
  sprintf(filenr,"%04d",file_nr);
  printf("Filenr: %s\n", filenr);

  TString signal;
  if (s!=3312 && s!=3334) signal = "la";
  else if (s==3312) signal = "xi";
  else if (s==3334) signal = "om";

  // ----- Paths and file names  --------------------------------------------
  TString inDir = TString(filenr);
  inDir+="/";
  TString mcFile   = inDir+signal+".mc.root";
  TString parFile  = inDir+signal+".par.root";
  TString rcFile   = inDir+signal+".rc.root";
  TString digiFile = "sts_Standard_s3055AAFK5.SecD.digi.par";

  Int_t iVerbose = 1;
  TStopwatch timer;
  timer.Start();

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/analysislibs.C");
  analysislibs();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna* run= new FairRunAna();
  run->SetInputFile(mcFile);
  run->SetOutputFile(rcFile);
  // ------------------------------------------------------------------------

  // -----   STS digitiser   ------------------------------------------------
  FairTask* stsDigitize = new CbmStsDigitize("STSDigitize", iVerbose);
  run->AddTask(stsDigitize);
  // ------------------------------------------------------------------------

  // ---  STS hit finding   -------------------------------------------------
  FairTask* findHits = new CbmStsFindHits("STSFindHits", iVerbose);
  run->AddTask(findHits);
  // ------------------------------------------------------------------------

  // ---  STS hit matching   ------------------------------------------------
  FairTask* matchHits = new CbmStsMatchHits("STSMatchHits",
          iVerbose);
  run->AddTask(matchHits);
  // ------------------------------------------------------------------------


  // -----   STS track finding   --------------------------------------------
  FairTask* kalman= new CbmKF();
  run->AddTask(kalman);
  FairTask* l1 = new CbmL1();
  run->AddTask(l1);
  CbmStsTrackFinder* trackFinder    = new CbmL1StsTrackFinder();
  FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
  run->AddTask(findTracks);
  // ------------------------------------------------------------------------


  // -----   STS track matching   -------------------------------------------
  FairTask* matchTracks = new CbmStsMatchTracks("Match tracks", iVerbose);
  run->AddTask(matchTracks);
  // ------------------------------------------------------------------------

  // -----   STS track fitting   --------------------------------------------
  CbmStsTrackFitter* trackFitter = new CbmStsKFTrackFitter();
  FairTask* fitTracks = new CbmStsFitTracks("STS Track Fitter", trackFitter, iVerbose);
  run->AddTask(fitTracks);
  // ------------------------------------------------------------------------

  // -----   Primary vertex finder   ----------------------------------------
  CbmPVFinderKF *vfinder = new CbmPVFinderKF();
  FairTask *vTask = new CbmFindPrimaryVertex("PV finder", "FairTask", vfinder);
  run->AddTask(vTask);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/";
  stsDigiFile += digiFile;
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  // -----   Initialise and run   -------------------------------------------
  run->LoadGeometry();
  run->Init();
  run->Run(0, nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "         << rcFile << endl;
  cout << "Parameter file is "      << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
}
