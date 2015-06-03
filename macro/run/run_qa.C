// --------------------------------------------------------------------------
//
// Macro for STS QA
//
// V. Friese   13/01/2006
//
// --------------------------------------------------------------------------


void run_qa(Int_t nEvents = 1, const char* setup = "sis300_electron")
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  TString outDir  = "data/";

  // MC file
  TString simFile = outDir + setup + "_test.mc.root";   // Input file (MC events)

  // Reco file
  TString recFile = outDir + setup + "_test.eds.root";  // Output file

  // Parameter file
  TString parFile = outDir + setup + "_params.root";    // Parameter file

  // Output file
  TString outFile = outDir + setup + "_test.qa.root";   // Output file
  
  TList *parFileList = new TList();

  TString inDir = gSystem->Getenv("VMCWORKDIR");
  TString paramDir = inDir + "/parameters/";

  TString setupFile = inDir + "/geometry/setup/" + setup + "_setup.C";
  TString setupFunct = setup;
  setupFunct += "_setup()";

  gROOT->LoadMacro(setupFile);
  gInterpreter->ProcessLine(setupFunct);

  // STS digitisation file
  TObjString stsDigiFile = paramDir + stsDigi;
  parFileList->Add(&stsDigiFile);
  cout << "macro/run/run_qa.C using: " << stsDigi << endl;

//  TObjString trdDigiFile = paramDir + trdDigi;
//  parFileList->Add(&trdDigiFile);
//  cout << "macro/run/macro_qa.C using: " << trdDigi << endl;
//
//  TObjString tofDigiFile = paramDir + tofDigi;
//  parFileList->Add(&tofDigiFile);
//  cout << "macro/run/macro_qa.C using: " << tofDigi << endl;

  // In general, the following parts need not be touched
  // ========================================================================



  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----  Analysis run   --------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile);
  fRun->AddFriend(recFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(parFileList, "in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------


  // -----   STS track finder QA   ------------------------------------------
  // Is legacy; to be checked
  /*CbmStsFindTracksQa* stsTrackFindQa = new CbmStsFindTracksQa(4, 0.7,
							      iVerbose);
  fRun->AddTask(stsTrackFindQa);
  */
  // ------------------------------------------------------------------------
    
    // -------- Performance of PID algorithm ------------
    CbmTrdTracksPidQa*  trdTrackPidQa =
        new CbmTrdTracksPidQa("PidPerformance");
    fRun->AddTask(trdTrackPidQa);
    // -------------------------------------------------
    
 // ------  Rich ring finder QA ---------------------------------------------
   Int_t normType = 1; //0 - normalize by number of MC hits
                      //1 - normalize by number of found hits;
//  CbmRichRingQa* richQa   =  new CbmRichRingQa("Qa","qa", iVerbose,normType);
//  fRun->AddTask(richQa);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  cout << "Starting run" << endl;
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
  // ------------------------------------------------------------------------

  //  delete run;

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}
