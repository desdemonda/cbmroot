void much_hits(){
//  Int_t nEvents=100;
  Int_t nEvents=1;
  TString dir = "data4/";
  TString epochFile = dir + "epoch.root";
//  TString epochFile = dir + "mc.root";
  TString digFile   = dir + "dig.root";
  TString outFile   = dir + "hits.root";
  TString parFile   = dir + "param.root";
  TString digiFile  = dir + "much_digi.root";
  
//  TChain* mcFileChain = new TChain("cbmsim");
//  mcFileChain->AddFile(dir+"/mc.root");
  
  Int_t iVerbose = 2;
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(epochFile);
  fRun->AddFriend(digFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  CbmMuchFindHitsAdvancedGem* findHits = new CbmMuchFindHitsAdvancedGem("MuchFindHitsAdvancedGem", digiFile, iVerbose);
  findHits->SetAlgorithm(3);
  findHits->SetEpoch(1);
//  findHits->SetEpoch(0);
  fRun->AddTask(findHits);

  fRun->Init();
  fRun->Run(0,nEvents);
}
