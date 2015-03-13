// --------------------------------------------------------------------------
//
// Macro for visualization of MC points and reconstructed hits
// in the CBM muon setup:
//
//
// E.Kryshen 22.11.2007
//
// --------------------------------------------------------------------------
void much_vis(
    const char* mcFile="data_pol_1M/mc.root",
    const char* rcFile="data_pol_1M/much.hits.root",
    const char* digiFile="$VMCWORKDIR/parameters/much/much_standard_monolithic.digi.root"
)
{
  TString outFile  = "data/dummy.root";

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
  gSystem->Load("libVis");

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->AddFriend(rcFile);
  fRun->SetOutputFile(outFile);

  CbmVisMuch* vis = new CbmVisMuch();
  vis->SetDigiFileName(digiFile);
  fRun->AddTask(vis);
  fRun->Init();
}
