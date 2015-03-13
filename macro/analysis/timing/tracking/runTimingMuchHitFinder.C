void runTimingMuchHitFinder(){
  TString dir = "data/";
  TString inFile    = dir + "epoch.root";
  TString muchFile  = dir + "much.reco.root";
  TString parFile   = dir + "param.root";
  TString histoFile = dir + "histo.muchhits.root";
  TString outFile   = dir + "dummy.root";
  TString digiFile = gSystem->Getenv("VMCWORKDIR");
  digiFile += "/parameters/much/much_v11a.digi.root";


  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/hyperon_style.C");
  hyperon_style();
  
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(muchFile);
  fRun->SetOutputFile(outFile);

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  CbmAnaTimingMuchHitFinder* task = new CbmAnaTimingMuchHitFinder("TimingMuchHitFinder",digiFile,histoFile);
  task->SetVerbose(2);
  fRun->AddTask(task);
  fRun->Init();
  fRun->Run(1);
}
