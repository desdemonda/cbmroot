void much_hits_gem(){
  Int_t nEvents = 1;
  TString inFile   = "data/mc.root";
  TString outFile  = "data/hits.root";
  TString digiFile = "data/much_digi.root";
  TString parFile  = "data/params.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  CbmMuchDigitizeGem* digitize = new CbmMuchDigitizeGem(digiFile);
  CbmMuchFindHitsGem* findHits = new CbmMuchFindHitsGem(digiFile);
  fRun->AddTask(digitize);
  fRun->AddTask(findHits);
  fRun->Init();
  fRun->Run(0,nEvents);
}
