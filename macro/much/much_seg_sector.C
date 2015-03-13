/**
 * Performs segmentation of Much stations based on
 * user specified conditions.
 *
 * @author M.Ryzhinskiy m.ryzhinskiy@gsi.de
 * @param mcFile      Input transport file name
 * @param inDigiFile  Input file name containing initial segmentation parameters
 * @param outDigiFile Output file name containing segmentation parameters
 */
void much_seg_sector(const char* mcFile = "",
                       const char* inDigiFile = "",
                       const char* outDigiFile = "")
{
  // ========================================================================
  //          Adjust this part according to your requirements

  if (mcFile == "") {
    mcFile = "data/mc.root";
  }
  if (inDigiFile == "") {
    inDigiFile = "data/much_digi_sector.seg";
  }
  if (outDigiFile == "") {
    outDigiFile = "data/much_digi_sector.root";
  }

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Dummy ROOT file (needed as an output)
  TString outFile  = "data/dummy.root";
  TString parFile = "data/params.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();
  
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(mcFile);
  fRun->SetOutputFile(outFile);

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile);
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  // -----  Segmentation task  ----------------------------------------------
  CbmMuchSegmentSector* seg = new CbmMuchSegmentSector(inDigiFile, outDigiFile);
  seg->DebugSwitchOn();
  fRun->AddTask(seg);
  // ------------------------------------------------------------------------

  // Run segmentation
  fRun->Init();

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}
