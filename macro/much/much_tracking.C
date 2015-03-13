void much_tracking(Int_t nEvents=1000){
  TString mcFile     = "data/mc.root";
  TString parFile    = "data/params.root";
  TString stsFile    = "data/sts.root";
  TString hitsFile   = "data/hits.root";
  TString tracksFile = "data/tracks.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
  cbmrootlibs();

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->AddFriend(stsFile);
  fRun->AddFriend(hitsFile);
  fRun->SetOutputFile(tracksFile);

  CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
  finder->SetTrackingType("branch");
  finder->SetMergerType("nearest_hit");
  fRun->AddTask(finder);
  CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
  fRun->AddTask(muchMatchTracks);

  CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
  trackingQa->SetUseConsecutivePointsInSts(true);
  trackingQa->SetMinNofPointsSts(4);
  trackingQa->SetMinNofPointsMuch(10);
  trackingQa->SetQuota(0.7);
  trackingQa->SetMinNofHitsMuch(10);
  trackingQa->SetVerbose(0);
  trackingQa->SetOutputDir("qa/");
  fRun->AddTask(trackingQa);
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);

  fRun->Init();
  fRun->Run(0,nEvents);
}
