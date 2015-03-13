void much_reco(TString dir = "data", Int_t fileNo=0, Int_t nEvents=10){
  TString mcFile         = dir + Form("/%02i/",fileNo) + "mc.root";
  TString stsFile        = dir + Form("/%02i/",fileNo) + "sts.reco.root";
  TString muchHitsFile   = dir + Form("/%02i/",fileNo) + "much.hits.root";
  TString muchTracksFile = dir + Form("/%02i/",fileNo) + "much.tracks.root";
  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->AddFriend(stsFile);
  fRun->AddFriend(muchHitsFile);
//  fRun->AddFile(dir + "mc_1.root");
  fRun->SetOutputFile(muchTracksFile);

  CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
  finder->SetTrackingType("branch");
  finder->SetMergerType("nearest_hit");
  fRun->AddTask(finder);
  CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
  fRun->AddTask(muchMatchTracks);

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(gFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  fRun->LoadGeometry();
  fRun->Init();
  fRun->Run(0,nEvents);
}
