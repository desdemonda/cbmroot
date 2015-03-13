// --------------------------------------------------------------------------
// Macro for qualitycheck of the MUCH hit producer
// E.Kryshen 22.11.2007
// --------------------------------------------------------------------------
void much_hitsQa(){
  Int_t nEvents = 500;
  Int_t iVerbose = 1;

  TString  dir            = "data/";
  TString  mcFile         = dir + "mc.root";
  TString  parFile        = dir + "mc.root";
  TString  digiFile = "$VMCWORKDIR/parameters/much/much_standard_monolithic.digi.root";
  TString  muchHitsFile   = dir + "much.hits.root";
  TString  outFile        = dir + "much.hits.qa.root";

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
  //gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libMuch");

  gROOT->LoadMacro("$VMCWORKDIR/much/much_histo_style.C");
  much_histo_style();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->AddFriend(muchHitsFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  TString muchDigiFile = gSystem->Getenv("VMCWORKDIR");
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  // ---  MuCh hit finder QA-------------------------------------------------
  CbmMuchHitFinderQa* qa = new CbmMuchHitFinderQa("HitQa",2);
  qa->SetGeoFileName(digiFile);
  qa->SetPerformanceFileName(outFile);
  qa->SetPrintToFile(0);
  qa->SetPullsQa(1);
  qa->SetOccupancyQa(0);
  qa->SetDigitizerQa(0);
  qa->SetStatisticsQa(0);
  qa->SetClusterDeconvQa(0);

  fRun->AddTask(qa);
  // ------------------------------------------------------------------------

  // -----   Initialize and run --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------
}
