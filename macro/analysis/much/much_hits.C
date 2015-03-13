void much_hits(TString dir = "data", Int_t fileNo=0, Int_t nEvents=10){
  TString inFile   = dir + Form("/%02i/",fileNo) + "mc.root";
  TString outFile  = dir + Form("/%02i/",fileNo) + "much.hits.root";
  TString digiFile = "$VMCWORKDIR/parameters/much/much_standard_monolithic.digi.root";

  Int_t iVerbose = 1;

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libMuch");
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
//  fRun->AddFile(dir + "mc_1.root");
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(gFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------


// ---  MuCh digitizer ----------------------------------------------------
//   CbmMuchDigitizeSimpleGem* digitize = new CbmMuchDigitizeSimpleGem("MuchDigitizeSimpleGem", digiFile, iVerbose);
  CbmMuchDigitizeAdvancedGem* digitize = new CbmMuchDigitizeAdvancedGem("MuchDigitizeAdvancedGem", digiFile, iVerbose);
  digitize->SetSpotRadius(0.05);
  fRun->AddTask(digitize);
  // ------------------------------------------------------------------------

  // ---  MuCh hit finder ---------------------------------------------------
  // CbmMuchFindHitsSimpleGem* findHits = new CbmMuchFindHitsSimpleGem("MuchFindHitsSimpleGem", digiFile, iVerbose);
  CbmMuchFindHitsAdvancedGem* findHits = new CbmMuchFindHitsAdvancedGem("MuchFindHitsAdvancedGem", digiFile, iVerbose);
  fRun->AddTask(findHits);
  // ------------------------------------------------------------------------


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------
}
