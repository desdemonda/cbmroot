void much_tracks_epoch(){
  Int_t nEvents=100;
  TString dir = "data1/";
  TString epochFile    = dir + "epoch.root";
  TString stsRecoFile  = dir + "sts.reco.root";
  TString muchRecoFile = dir + "much.reco.root";
  TString outFile      = dir + "much.tracks.root";
  TString parFile   = dir + "param.root";
  
  TString digiFile = gSystem->Getenv("VMCWORKDIR");
  digiFile += "/parameters/much/much_v11a.digi.root";

  
  TChain* mcFileChain = new TChain("cbmsim");
  mcFileChain->AddFile(dir+"/mc.root");
  
  Int_t iVerbose = 2;
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(epochFile);
  fRun->AddFriend(stsRecoFile);
  fRun->AddFriend(muchRecoFile);
  fRun->SetOutputFile(outFile);

  CbmLitFindMuchTracksEpoch* finder = new CbmLitFindMuchTracksEpoch();
  finder->SetSliceSeparationTime(2);
  finder->SetHitTimeResolution(10);
  finder->SetTrackingType("branch");
  finder->SetMergerType("nearest_hit");
  fRun->AddTask(finder);
  
  CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
  muchMatchTracks->SetEpoch(1);
  fRun->AddTask(muchMatchTracks);
  
  // ------------------------------------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  fRun->Init();
  fRun->Run(0,nEvents);
  return;
}
