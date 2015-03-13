// --------------------------------------------------------------------------
//
// Macro for visualization of MC points and reconstructed hits
// in the CBM muon setup:
//
//
// E.Kryshen 22.11.2007
//
// --------------------------------------------------------------------------
void much_vis(TString mcFile="", TString rcFile="", TString digiFile="")
{
  TString inputdir = gSystem->Getenv("VMCWORKDIR");
  if (mcFile == "") {
     mcFile = inputdir + "/macro/much/data/mc.root";
  }
  if (digiFile == "") {
     digiFile = inputdir + "/macro/much/data/much_digi.root";
  }
  if (rcFile == "") {
     rcFile = inputdir + "/macro/much/data/hits_gem.root";
  }

  TString outFile  = inputdir + "/macro/much/data/dummy.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  gSystem->Load("libVis");

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile.Data());
  fRun->AddFriend(rcFile.Data());
  fRun->SetOutputFile(outFile.Data());

  CbmVisMuch* vis = new CbmVisMuch();
  vis->SetDigiFileName(digiFile.Data());
  fRun->AddTask(vis);
  fRun->Init();
}
