void run_daqQa(Int_t nSlices=10) {
  TString inFile  = "data/raw.root";
  TString hitFile = "data/hits.root";
  TString parFile = "data/par.root";
  TString outFile = "data/rawQa.root";
  gDebug = 0;
  TStopwatch timer;
  timer.Start();
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  TChain* mcChain = new TChain("cbmsim");
  mcChain->AddFile("data/mc.root");
  
  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *run = new CbmRunAna();
  run->SetInputFile(inFile);
  run->AddFriend(hitFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  FairTask* daqQa = new CbmDaqQa(mcChain);
  run->AddTask(daqQa);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------


  // -----   Intialise and run   --------------------------------------------
  run->Init();
  cout << "Starting run" << endl;
  run->Run(0, nSlices);
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
