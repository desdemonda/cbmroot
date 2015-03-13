void ana_la(Int_t file_nr=0, Int_t nEvents=10){
  Char_t filenr[4];
  sprintf(filenr,"%04d",file_nr);
  printf("Filenr: %s\n", filenr);

  TString signal = "la";

  TString inDir = TString(filenr);
  inDir+="/";
  TString mcFile   = inDir+signal+".mc.root";
  TString parFile  = inDir+signal+".par.root";
  TString rcFile   = inDir+signal+".rc.root";
  TString anaFile  = inDir+signal+".ana.root";

  Int_t iVerbose = 1;

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/analysislibs.C");
  analysislibs();

  FairRunAna* run= new FairRunAna();
  run->SetInputFile(mcFile);
  run->AddFriend(rcFile);
  run->SetOutputFile(anaFile);

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);

  CbmKF* kf = new CbmKF();
  CbmAnaHypRecLambda* task = new CbmAnaHypRecLambda("Task",iVerbose);
  task->SetCheckChi(1);
  task->SetCutChi1(4.4);
  task->SetCutChi2(3.8);

  run->AddTask(kf);
  run->AddTask(task);
  run->Init();

  TStopwatch timer;
  timer.Start();

  run->Run(0,nEvents);

  timer.Stop();
  cout << "CPU time  " << timer.CpuTime() << "s" << endl;
}
