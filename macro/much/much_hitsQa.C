// --------------------------------------------------------------------------
// Macro for qualitycheck of the MUCH hit producer
// E.Kryshen 22.11.2007
// --------------------------------------------------------------------------
void much_hitsQa(){
  Int_t nEvents = 1;
  Int_t iVerbose = 1;

  TString  dir            = "data/";
  TString  mcFile         = dir + "mc.root";
  TString  parFile        = dir + "params.root";
  TString  digiFile       = dir + "much_digi.root";
  TString  muchHitsFile   = dir + "hits_gem.root";
  TString  outFile        = dir + "hits_gem.qa.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
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
  // ------------------------------------------------------------------------

  // ---  MuCh hit finder QA-------------------------------------------------
  CbmMuchHitFinderQa* qa = new CbmMuchHitFinderQa("HitQa",2);
  qa->SetGeoFileName(digiFile);
  qa->SetPerformanceFileName(outFile);
  qa->SetPrintToFile(0);
  qa->SetPullsQa(0);
  qa->SetOccupancyQa(1);
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
