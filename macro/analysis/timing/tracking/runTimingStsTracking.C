void runTimingStsTracking(){
  TString dir = "data/";
  TString inFile    = dir + "epoch.root";
//  TString inFile    = dir + "mc.root";
  TString parFile   = dir + "param.root";
  TString digFile   = dir + "sts.reco.root";
  TString histoFile = dir + "histo.tracking.root";
  TString outFile   = dir + "dummy.root";
  TString digiFile    = "sts_v11a.digi.par";


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

  CbmAnaTimingStsTracking* task = new CbmAnaTimingStsTracking("TimingStsTracking",histoFile);
  fRun->AddTask(task);
  fRun->Init();
  fRun->Run(10);
}
