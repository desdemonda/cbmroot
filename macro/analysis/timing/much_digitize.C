void much_digitize(){
  Int_t nEvents=1000;
  TString dir = "data5/";
  TString mcFile   = dir + "mc.root";
  TString outFile  = dir + "dig1.root";
  TString digiFile = dir + "much_digi.root";
  TString parFile  = dir + "param.root";
  
  Int_t iVerbose = 1;
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->SetOutputFile(outFile);

  // ---  MuCh digitizer ----------------------------------------------------
  CbmMuchDigitizeAdvancedGem* digitize = new CbmMuchDigitizeAdvancedGem("MuchDigitizeAdvancedGem", digiFile, iVerbose);
  digitize->SetSpotRadius(0.05);
  digitize->SetQMaximum(500000);
  digitize->SetNADCChannels(256);
  digitize->SetMeanGasGain(17000);
  digitize->SetDTime(4);      // ns

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
