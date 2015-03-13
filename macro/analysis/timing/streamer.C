void streamer(Int_t nEvents=100){
  TString dir = "data6";
  TString inFile         = dir + "/mc.root";
  TString parFile        = dir + "/param.root";
  TString outFile        = dir + "/epoch.root";
  Double_t interactionRate = 1.e7; // Hz
  Int_t beamProfile = 1;           // 1 - exponential
  Double_t epochLength = 1000000;    // ns
  
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  fRun->SetAsync(kTRUE);
  CbmMCStreamer* streamer = new CbmMCStreamer(interactionRate,beamProfile,epochLength);
  fRun->AddTask(streamer);
  
  fRun->Init();
  fRun->Run(nEvents);
}
