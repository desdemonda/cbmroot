void much_ana(TString dir="data", int nEvents=1000){
  TString mcFile   = Form("%s/mc.root",dir.Data());
  TString parFile  = Form("%s/par.root",dir.Data());
  TString stsFile  = Form("%s/sts.root",dir.Data());
  TString recFile  = Form("%s/rec.root",dir.Data());
  TString outFile  = Form("%s/ana.root",dir.Data());
  TString digiFile = Form("%s/much_digi.root",dir.Data());

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->AddFriend(stsFile);
  fRun->AddFriend(recFile);
  fRun->SetOutputFile(outFile);

  TString muchDigiFile = gSystem->Getenv("VMCWORKDIR");
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  CbmKF* kf = new CbmKF();
  CbmAnaDimuonAnalysis* ana = new CbmAnaDimuonAnalysis("DimuonAnalysis",digiFile,10);
  ana->SetVerbose(0);
  ana->SetStsPointsAccQuota(4);
  ana->SetStsTrueHitQuota(0.7);

  fRun->AddTask(kf);
  fRun->AddTask(ana);
  fRun->Init();
  fRun->Run(0,nEvents);
}
