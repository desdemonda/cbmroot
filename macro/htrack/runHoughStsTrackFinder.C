// --------------------------------------------------------------------------
//
// Macro for reconstruction in STS from MC data
//
// Tasks:  CbmStsDigitise
//         CbmStsFindHits
//         CbmStsMatchHits
//         CbmStsFindTracks
//
// C. Steinle   23/01/2007
//
// --------------------------------------------------------------------------

Int_t runHoughStsTrackFinder(Int_t nEvents=2)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Input folder for the files
  TString inputFolder  = "data";

  // Input folder for the files
  TString outputFolder = "data";

  // Collision system
  TString system       = "auau";

  // Beam momentum
  TString beam         = "25gev";

  // Trigger (centrality)
  TString trigger      = "centr";

  // Input file (MC events)
  TString inFile       = inputFolder  + "/" + system + "." + beam + "." + trigger + ".mc.root";

  // Output file
  TString outFile      = outputFolder + "/" + system + "." + beam + "." + trigger + ".reco.root";

  // Parameter file
  TString parFile      = inputFolder  + "/" + system + "." + beam + "." + trigger + ".params.root";

  // Analysis file
  TString anaFile      = outputFolder + "/" + system + "." + beam + "." + trigger + ".ana.root";

  // Digitisation file
  TString digiFile     = "sts_v13d_std.digi.par";
  //TString digiFile = "sts_Standard_s3055AAFK5.SecD.digi.par";

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose       = 1;


  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----   GEANE   initialization   ---------------------------------------
  FairGeane* Geane            = new FairGeane(inFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  TString stsDigiFile        = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile               += "/parameters/sts/";
  stsDigiFile               += digiFile;
  FairRuntimeDb* rtdb         = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1  = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2  = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  // -----   STS digitizer   ------------------------------------------------
  CbmStsDigitize* stsDigitize         = new CbmStsDigitize();
  fRun->AddTask(stsDigitize);
  // ------------------------------------------------------------------------


  // -----   STS Cluster Finder   --------------------------------------------
  FairTask* stsClusterFinder = new CbmStsClusterFinder();
  fRun->AddTask(stsClusterFinder);
  // -------------------------------------------------------------------------


  // ---  STS hit finding   -------------------------------------------------
  CbmStsFindHits* findHits            = new CbmStsFindHits();
  fRun->AddTask(findHits);
  // ------------------------------------------------------------------------

  // ---  STS hit matching   ------------------------------------------------
  CbmStsMatchHits* matchHits          = new CbmStsMatchHits();
  fRun->AddTask(matchHits);
  // ------------------------------------------------------------------------

  // ---  STS Track finding   -------------------------------------------------
  CbmStsFindTracks* findTracks        = new CbmStsFindTracks(iVerbose, NULL, kFALSE, "STS Track Finder");
  fRun->AddTask(findTracks);
  // ------------------------------------------------------------------------

  // ---  STS TrackFinder     -------------------------------------------------
  CbmHoughStsTrackFinder* trackFinder = new CbmHoughStsTrackFinder();
  trackFinder->SetOutputFile(anaFile);
  findTracks->UseFinder(trackFinder);
  // ------------------------------------------------------------------------

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  Geane->SetField(fRun->GetField());
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
//  exit(0);

  // ------------------------------------------------------------------------

}
