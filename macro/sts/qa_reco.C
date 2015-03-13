// --------------------------------------------------------------------------
//
// Macro for STS Reconstruction QA
//
// R. Karabowicz   06/02/2007
//
// --------------------------------------------------------------------------


{

  gStyle->SetPalette(1);
  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Number of events to process
  Int_t nEvents = 1;

  // MC file
  TString simFile = "sts.mc.root";

  // Reco file
  TString recFile = "sts.reco.root";

  // Parameter file
  TString parFile = "params.root";

  // Output file
  TString outFile = "reco.qa.root";

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
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libGen");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  // ------------------------------------------------------------------------
  
  // -----  Analysis run   --------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  
  fRun->SetInputFile(simFile);
  fRun->AddFriend(recFile);
  
  
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/sts_digi.par";
  parInput2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parInput2);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------


  // -----   STS track finder QA   ------------------------------------------
  CbmStsReconstructionQa* stsReconstructionQa = new CbmStsReconstructionQa(kTRUE,4, 0.7,iVerbose);
  stsReconstructionQa->SetShowStation1(4);
  fRun->AddTask(stsReconstructionQa);
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
