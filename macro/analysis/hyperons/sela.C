void sela(Int_t file_nr=0, Int_t nEvents=1, Int_t pdg=3312){
  Char_t filenr[4];
  sprintf(filenr,"%04d",file_nr);
  printf("Filenr: %s\n", filenr);

  TString signal;
  if      (pdg==3312) signal = "xi";
  else if (pdg==3334) signal = "om";
  else {
    printf("wrong signal: %i\n",pdg);
    exit(1);
  }

  TString inDir = TString(filenr);
  inDir+="/";
  TString mcFile   = inDir+signal+".mc.root";
  TString parFile  = inDir+signal+".par.root";
  TString rcFile   = inDir+signal+".rc.root";
  TString laFile   = inDir+signal+".la.root";

  Int_t iVerbose = 1;

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/analysislibs.C");
  analysislibs();

  FairRunAna* run= new FairRunAna();
  run->SetInputFile(mcFile);
  run->AddFriend(rcFile);
  run->SetOutputFile(laFile);

  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);

  CbmKF* kf = new CbmKF();
  run->AddTask(kf);
  CbmAnaHypSelectLambda* task = new CbmAnaHypSelectLambda("Task",iVerbose);
  task->SetCutChi1(4.4);
  task->SetCutChi2(4.0);
  task->SetCutB1  (0.0);
  task->SetCutB2  (0.0);
  task->SetCutChi (8.2);
  task->SetCutDca (0.228);
  task->SetCutPca (2.0);
  task->SetCutPcaMax(50.0);
  task->SetCutMmin(1.1157-3*0.0013);
  task->SetCutMmax(1.1157+3*0.0013);
  run->AddTask(task);

  run->Init();
  TStopwatch timer;
  timer.Start();
  run->Run(0,nEvents);
  timer.Stop();
  cout << "CPU time  " << timer.CpuTime() << "s" << endl;
}
