// --------------------------------------------------------------------------
//
// Macro for visualization of MC points and reconstructed hits
// in the CBM muon setup:
//
//
// E.Kryshen 22.11.2007
//
// --------------------------------------------------------------------------
void test_much_vis(TString mcFile="", TString rcFile="", TString digiFile="")
{
  TString inputdir = gSystem->Getenv("VMCWORKDIR");
  /*if (mcFile == "") {
     //mcFile = inputdir + "/macro/much/data/Jpsi.auau.25gev.centr.mc.root";
	  mcFile = inputdir + "/macro/clustering/data/mc.0001.root";
  }*/
  if (digiFile == "") {
     //digiFile = inputdir + "/macro/much/data/much_digi.root";//"/parameters/much/much_standard.digi.root";
	  digiFile = inputdir + "/macro/clustering/data/much_v11a.digi.root";
  }
  /*if (rcFile == "") {
     //rcFile = inputdir + "/macro/much/data/Jpsi.auau.25gev.centr.muchhits.root";
	  rcFile = inputdir + "/macro/clustering/data/global.hits.0001.root";
  }*/
  TString dir = "/home/kozlov/cbm/cbmroot_new/events/03/"; // Input/Output directory
  mcFile = dir + "mc.0000.root"; // MC transport file
  rcFile = dir + "global.reco.0000.root"; // Reconstruction file

  TString outFile  = inputdir + "/macro/clustering/data/dummy.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libSts");
  gSystem->Load("libMuch");
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
