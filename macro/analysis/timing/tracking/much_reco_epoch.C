void much_reco_epoch(){
  Int_t nEvents=100;
  TString dir = "data/";
  TString epochFile   = dir + "epoch.root";
  TString outFile   = dir + "much.reco.root";
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
  fRun->SetOutputFile(outFile);

  // ---  MuCh digitizer ----------------------------------------------------
  CbmMuchDigitizeAdvancedGem* digitize = new CbmMuchDigitizeAdvancedGem("MuchDigitizeAdvancedGem", digiFile, iVerbose);
  digitize->SetSpotRadius(0.05);
  digitize->SetQMaximum(500000);
  digitize->SetNADCChannels(256);
  digitize->SetMeanGasGain(17000);
  digitize->SetEpoch(1);
  digitize->SetMcChain(mcFileChain);
  digitize->SetDeadTime(10); // ns
  digitize->SetDTime(0.1);     // ns
  
  fRun->AddTask(digitize);
  
  CbmMuchFindHitsAdvancedGem* findHits = new CbmMuchFindHitsAdvancedGem("MuchFindHitsAdvancedGem", digiFile, iVerbose);
  findHits->SetAlgorithm(3);
  findHits->SetClusterSeparationTime(10);
  findHits->SetEpoch(1);
  fRun->AddTask(findHits);
//  
//  CbmLitFindMuchTracksEpoch* finder = new CbmLitFindMuchTracksEpoch();
//  finder->SetTrackingType("branch");
//  finder->SetMergerType("nearest_hit");
//  fRun->AddTask(finder);
//  
//  CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
//  muchMatchTracks->SetEpoch(1);
//  fRun->AddTask(muchMatchTracks);
  
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
