// --------------------------------------------------------------------------
//
// Macro for STS Simulation QA
//
// R. Karabowicz   02/02/2007
//
// --------------------------------------------------------------------------


{

  // ========================================================================
  //          Adjust this part according to your requirements

  gStyle->SetPalette(1);

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Number of events to process
  Int_t nEvents = 1;

  // MC file
  TString simFile = "sts.mc.root";

  // Parameter file
  TString parFile = "params.root";

  // Output file
  TString outFile = "sim.qa.root";

  // In general, the following parts need not be touched
  // ========================================================================


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // ----  Load libraries   -------------------------------------------------
  //gROOT->Reset();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
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
  // ------------------------------------------------------------------------
  
  // -----  Analysis run   --------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  
  fRun->SetInputFile(simFile);
  
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------


  // -----   STS track finder QA   ------------------------------------------
  CbmStsSimulationQa* stsSimulationQa = new CbmStsSimulationQa(kTRUE);
  fRun->AddTask(stsSimulationQa);
  // ------------------------------------------------------------------------



  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
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


}
