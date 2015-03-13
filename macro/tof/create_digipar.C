// --------------------------------------------------------------------------
//
// Macro to create digi parameters for the TRD
// This macro needs a geometry file with the TRD
// geometry only.
//
// The way how the pad layout looks like has to be
// implemented in a task called in this macro.
// CbmTrdCreateDigiPar is the implementation used
// here.
//
// F.Uhlig 07.05.2010
//
// --------------------------------------------------------------------------


void create_digipar(TString geomFile="full.root", Int_t nEvents = 0)
{

  //Extract Filename without extension and path 
  //TODO: Do this in a better way.
  TString fileName = geomFile;
  fileName.ReplaceAll(".root","");
  if (fileName.Contains("/")){
    fileName=fileName("/.*$"); //extract substring with filename
    fileName.ReplaceAll("/","");
  }
  cout<<"fileName: "<<fileName<<endl;


  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "data/test.mc." + fileName + ".root";

  // Geometry File
  TString geoFile = "data/geofile_" + fileName + ".root";

  // Output file
  TString outFile = "data/test.esd." + fileName + ".root";

  // Digi Parameter Output File
  TString digiFile = fileName + ".digi.par.long";
  //  TString digiFile = fileName + ".digi.par";

  // In general, the following parts need not be touched
  // ========================================================================


 
  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------



  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



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
  gSystem->Load("libTof");

  gLogger->SetLogScreenLevel("DEBUG2");
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run= new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  run->SetGeomFile(geoFile);
  // ------------------------------------------------------------------------

  FairRuntimeDb* rtdb = run->GetRuntimeDb();

  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo2->open(digiFile,"out");
  rtdb->setOutput(parIo2);

  CbmTofCreateDigiPar* tofDigiProducer = new CbmTofCreateDigiPar("TOF Digi Producer",
                                                        "TOF task");
  run->AddTask(tofDigiProducer);

  // -------------------------------------------------------------------------

  rtdb->saveOutput();
 
  // -----   Intialise and run   --------------------------------------------
  //  run->LoadGeometry();
  run->Init();
   
  rtdb->print();
  

  CbmTofDigiPar* DigiPar = (CbmTofDigiPar*)
                           rtdb->getContainer("CbmTofDigiPar");

  DigiPar->setChanged();
  DigiPar->setInputVersion(run->GetRunId(),1);
  rtdb->print();
  rtdb->saveOutput();


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);
}
