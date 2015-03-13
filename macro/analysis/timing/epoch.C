void epoch(){
  TString dir = "data3";
  TString inFile    = dir + "/epoch.0000.root";
  TString parFile   = dir + "/param.0000.root";
  TString outFile   = dir + "/ana_epoch.root";
  TString histoFile = dir + "/histo_epoch.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/hyperon_style.C");
  hyperon_style();
  gSystem->Load("libRun");
  
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  CbmAnaEpochTest* test = new CbmAnaEpochTest("EpochTest",histoFile);
  test->SetVerbose(0);
  fRun->AddTask(test);
  fRun->Init();
  fRun->Run(200);
}
