void runTimingMuchHitFinder(){
  TString dir = "data4/";
//  TString inFile    = dir + "mc.root";
  TString inFile    = dir + "epoch.root";
  TString parFile   = dir + "param.root";
  TString digFile   = dir + "dig.root";
  TString hitFile   = dir + "hits.root";
  TString histoFile = dir + "histo.hits.root";
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
  fRun->AddFriend(hitFile);
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
