void much_digitize_epoch(){
  Int_t nEvents=1000;
  TString dir = "data6/";
  TString epochFile = dir + "epoch.root";
  TString outFile   = dir + "dig.root";
  TString parFile   = dir + "param.root";
  TString digiFile  = dir + "much_digi.root";
  
  TChain* mcFileChain = new TChain("cbmsim");
  mcFileChain->AddFile(dir+"/mc.root");
  
  Int_t iVerbose = 2;
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(epochFile);
  fRun->SetOutputFile(outFile);

  // ---  MuCh digitizer ----------------------------------------------------
  CbmMuchDigitizeAdvancedGem* digitize = new CbmMuchDigitizeAdvancedGem("MuchDigitizeAdvancedGem", digiFile, iVerbose);
  digitize->SetSpotRadius(0.05);
  digitize->SetQMaximum(500000);
  digitize->SetNADCChannels(256);
  digitize->SetMeanGasGain(17000);
  digitize->SetEpoch(1);
  digitize->SetMcChain(mcFileChain);
  digitize->SetDeadTime(500); // ns
  digitize->SetDTime(0.01);      // ns
  
  fRun->AddTask(digitize);
  // ------------------------------------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  fRun->LoadGeometry();
  fRun->Init();
  fRun->Run(0,nEvents);
  return;
}
