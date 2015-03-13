
void much_ana(Int_t nEvents=1000){
  TTree::SetMaxTreeSize(90000000000);
  TString script = TString(gSystem->Getenv("LIT_SCRIPT"));
  TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

  // Input and output data
  TString dir = "events/much_anna_omega_8gev_10k/"; // Output directory
  TString mcFile = dir + "mc.0000.root"; // MC transport file
  TString parFile = dir + "param.0000.root"; // Parameters file
  TString globalRecoFile = dir + "global.reco.0000.root"; // File with reconstructed tracks and hits
  TString analysisFile = dir + "analysis.0000.root"; // Output analysis file

  TString muchDigiFile = parDir + "/much/much_v12c.digi.root"; // MUCH digi file

  if (script == "yes") {
     mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
     parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
     globalRecoFile = TString(gSystem->Getenv("LIT_GLOBAL_RECO_FILE"));
     analysisFile = TString(gSystem->Getenv("LIT_ANALYSIS_FILE"));
     muchDigiFile = TString(gSystem->Getenv("LIT_MUCH_DIGI"));
  }

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->AddFriend(globalRecoFile);
  fRun->SetOutputFile(analysisFile);

  TString muchDigiFile = gSystem->Getenv("VMCWORKDIR");
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  CbmKF* kf = new CbmKF();
  CbmAnaDimuonAnalysis* ana = new CbmAnaDimuonAnalysis("DimuonAnalysis", muchDigiFile, 1);
  ana->SetVerbose(0);
  ana->SetStsPointsAccQuota(4);
  ana->SetStsTrueHitQuota(0.7);

  fRun->AddTask(kf);
  fRun->AddTask(ana);
  fRun->Init();
  fRun->Run(0,nEvents);
}
