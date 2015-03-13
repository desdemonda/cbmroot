{
  // ========================================================================
	  Int_t iVerbose = 0;
	 TString parFile = "params.root";
	  //Int_t nEvents = 10;
	  Int_t startEvent = 0;
	  Int_t stopEvent = 10;
	  // ----  Load libraries   -------------------------------------------------
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
  	gSystem->Load("libEcal");
  	gSystem->Load("libKF");
  	gSystem->Load("libL1");
	gSystem->Load("libMCMatch");
	  // ------------------------------------------------------------------------
	  // Output file

	  TString linkFile = "sts.links.root";
	  TString outFile = "sts.match.root";


	  std::cout << "LinkFile: " << linkFile.Data() << std::endl;




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

  fRun->SetInputFile(linkFile);

  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
//   FairParAsciiFileIo* parInput1 = new FairParAsciiFileIo();
//   parInput1->open(parFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  Bool_t kParameterMerged=kTRUE;
//  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
//  output->open(parOutFile.Data(),"RECREATE");
  rtdb->setOutput(parInput1);
  rtdb->print();
//  fRun->LoadGeometry();

  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================

  // -----    MVD hit producer   --------------------------------------------


  CbmMCMatchLoaderTask* mcMatch = new CbmMCMatchLoaderTask();
  fRun->AddTask(mcMatch);

  CbmMCMatchSelectorTask* mcSelector = new CbmMCMatchSelectorTask(5,1); // MC Match for charged particles
  mcSelector->SetAllWeights(1.);
  fRun->AddTask(mcSelector);
  
  CbmMCTestHitCompare* hitComp = new CbmMCTestHitCompare();
  fRun->AddTask(hitComp);
//   CbmMCMatchSelectorTask* mcSelector2 = new CbmMCMatchSelectorTask(4,1); // MC Match for neutral particles
//   mcSelector2->SetAllWeights(1.);
//   fRun->AddTask(mcSelector2);

  fRun->Init();

  fRun->Run(startEvent,stopEvent);


  // ------------------------------------------------------------------------

// rtdb->saveOutput();
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
