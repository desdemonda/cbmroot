
/**
 * Performs some basic analysis of clustering.
 *
 * @param mcFile    Input file containing MC transport data
 * @param recoFile  Input file containing digitizer and hitfinder data
 * @param digiFile  Input file containing segmentation parameters
 */
void much_cluster_analyze(TString mcFile="",//"data/mc.standard.500.root",
                        TString recoFile="",//"data/hits.standard.500.root",
                        TString digiFile = "",
                        Int_t nEvents = 5){

  TString inputdir = gSystem->Getenv("VMCWORKDIR");
  if (mcFile == "") {
    mcFile = "data/Jpsi.auau.25gev.centr.mc.root";
  }
  if (recoFile == "") {
    recoFile = "data/Jpsi.auau.25gev.centr.muchhits.root";
  }
  if (digiFile == "") {
    digiFile = inputdir + "/parameters/much/much_v11a.digi.root";
  }
  TString outFile = "data/dummy.root";

  Int_t iVerbose = 1;

  TString inputdir = gSystem->Getenv("VMCWORKDIR");

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
  fRun->AddFriend(recoFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  TString muchDigiFile = gSystem->Getenv("VMCWORKDIR");
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(mcFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------

  // ---  MuCh cluster analysis ---------------------------------------------
  CbmMuchClusterAnalysis* analysis = new CbmMuchClusterAnalysis("ClusterAnalysis",2);
  analysis->SetGeoFileName(digiFile);
  fRun->AddTask(analysis);
  // ------------------------------------------------------------------------

  // -----   Initialize and run --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------
}
