void runTimingStsDigitizer(){
  TString dir = "data/";
  TString inFile    = dir + "epoch.root";
//  TString inFile    = dir + "mc.root";
  TString parFile   = dir + "param.root";
  TString digFile   = dir + "sts.reco.root";
  TString histoFile = dir + "histo.digis.root";
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

  
  // -----  Parameter database   --------------------------------------------
  TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  stsDigiFile += "/parameters/sts/";
  stsDigiFile += digiFile;
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  CbmAnaTimingStsDigitizer* task = new CbmAnaTimingStsDigitizer("TimingMuchDigitizer",histoFile);
//  task->SetModuleId(4);
//  task->SetSectorId(37);
//  task->SetChannelId(3);
  task->SetEpoch(1);
  task->SetVerbose(2);
  fRun->AddTask(task);
  fRun->Init();
  fRun->Run(100);
}
