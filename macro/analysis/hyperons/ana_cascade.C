void ana_cascade(Int_t file_nr=0, Int_t nEvents=1, Int_t pdg=3312){
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
  TString anaFile  = inDir+signal+".ana.root";

  Int_t iVerbose = 1;


  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/analysislibs.C");
  analysislibs();

  FairRunAna* run= new FairRunAna();
  run->SetInputFile(mcFile);
  run->AddFriend(rcFile);
  run->AddFriend(laFile);
  run->SetOutputFile(anaFile);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  // ------------------------------------------------------------------------

  CbmKF* kf = new CbmKF();
  run->AddTask(kf);
  CbmAnaHypRecCascade* task = new CbmAnaHypRecCascade("Task",1);

  if (pdg==3334){
    task->SetMotherPdg(3334);
    task->SetSisterPdg(-321);
    task->SetCheckChi(1);
    task->SetCheckB(1);
    task->SetCutBla(0.16);
    task->SetCutChiDa(4.6);
//    task->SetCheckChi(0);
//    task->SetCheckB(0);
//    task->SetCutBla(0.15);
//    task->SetCutChiDa(4.0);
  } else if (pdg==3312){ // TODO Check cuts
    task->SetMotherPdg(3312);
    task->SetSisterPdg(-211);
    task->SetCheckChi(1);
    task->SetCheckB(1);
    task->SetCutBla(0.16);
    task->SetCutChiDa(4.6);
//    task->SetCheckChi(0);
//    task->SetCheckB(0);
//    task->SetCutBla(0.15);
//    task->SetCutChiDa(4.0);
  }

  run->AddTask(task);
  run->Init();

  TStopwatch timer;
  timer.Start();
  run->Run(0,nEvents);
  timer.Stop();
  cout << "CPU time  " << timer.CpuTime() << "s" << endl;
}
