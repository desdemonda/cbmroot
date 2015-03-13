{
  // ========================================================================
	  Int_t iVerbose = 0;
	  TString MCFile = "sts.mc.root";
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

	  
	  TString RecoFile = "sts.reco.root";
	  
	  TString outFile = "sts.links.root";

	  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
	  
	  std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;


	 


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

  fRun->SetInputFile(MCFile);

  fRun->AddFriend(RecoFile);


  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");

  rtdb->setFirstInput(parInput1);
  Bool_t kParameterMerged=kTRUE;
  rtdb->setOutput(parInput1);
  rtdb->print();


  // ------------------------------------------------------------------------

  CbmMCMatchCreatorTask* mcMatch = new CbmMCMatchCreatorTask();
  fRun->AddTask(mcMatch);
 CbmMCTestHitCompare* hitComp = new CbmMCTestHitCompare();
  fRun->AddTask(hitComp);
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
