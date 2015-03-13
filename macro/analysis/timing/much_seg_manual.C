void much_seg_manual(){
  TString mcFile      = "data4/mc.root";
  TString parFile     = "data4/param.root";
  TString inDigiFile  = "data4/much_digi.seg";
  TString outDigiFile = "data4/much_digi.root";

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Dummy ROOT file (needed as an output)
  TString outFile  = "dummy.root";

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  // ------------------------------------------------------------------------

  // -----   Analysis run   -------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->SetOutputFile(outFile.Data());
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  // -----  Segmentation task  ----------------------------------------------
  CbmMuchSegmentManual* seg = new CbmMuchSegmentManual(inDigiFile.Data(), outDigiFile.Data());
  fRun->AddTask(seg);
  // ------------------------------------------------------------------------

  // Run segmentation
  fRun->Init();

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}
