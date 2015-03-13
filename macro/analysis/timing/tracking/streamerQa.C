void streamerQa(){
  TString dir = "data";
  TString inFile    = dir + "/epoch.root";
  TString parFile   = dir + "/param.root";
  TString outFile   = dir + "/streamerQa.root";

  TChain* mcFileChain = new TChain("cbmsim");
  mcFileChain->AddFile(dir+"/mc.root");
  
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

  CbmMCStreamerQa* streamerQa = new CbmMCStreamerQa("StreamerQa",mcFileChain);
  streamerQa->SetVerbose(0);
  fRun->AddTask(streamerQa);
  fRun->Init();
  fRun->Run();
}
