void runTimingMuchDigitizer(){
  TString dir = "data6/";
  TString inFile    = dir + "epoch.root";
//  TString inFile    = dir + "mc.root";
  TString parFile   = dir + "param.root";
  TString digFile   = dir + "dig.root";
  TString histoFile = dir + "histo.root";
  TString digiFile  = dir + "much_digi.root";
  TString outFile   = dir + "dummy.root";


  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/hyperon_style.C");
  hyperon_style();
  
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(digFile);
  fRun->SetOutputFile(outFile);

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  CbmAnaTimingMuchDigitizer* task = new CbmAnaTimingMuchDigitizer("TimingMuchDigitizer",digiFile,histoFile);
  task->SetModuleId(4);
  task->SetSectorId(37);
  task->SetChannelId(3);
  task->SetEpoch(1);
  task->SetVerbose(2);
  fRun->AddTask(task);
  fRun->Init();
  fRun->Run(1000);
}
