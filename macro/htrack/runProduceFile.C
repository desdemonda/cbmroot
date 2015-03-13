// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// CBM setup with STS only
//
// C. Steinle   23/01/2007
//
// --------------------------------------------------------------------------

Int_t runProduceFile(Int_t nEvents=2)
{

  // ========================================================================
  //          Adjust this part according to your requirements

  // Output folder for the files
  TString folder      = "data";

  // Collision system
  TString system      = "auau";

  // Beam momentum
  TString beam        = "25gev";

  // Trigger (centrality)
  TString trigger     = "centr";

  // Output file name
  TString outFile     = folder + "/" + system + "." + beam + "." + trigger + ".mc.root";

  // Parameter file name
  TString parFile     = folder + "/" + system + "." + beam + "." + trigger + ".params.root";

  // Cave geometry
  TString caveGeom    = "cave.geo";

  CbmTarget* target = new CbmTarget("Gold", 0.025);

  // Beam pipe geometry
  TString pipeGeom    = "pipe/pipe_standard.geo";

  // Magnet geometry and field map
  TString magnetGeom  = "magnet/magnet_v09e.geo";
  TString fieldMap    = "field_v12b";
  Double_t fieldZ     = 50.;     // z position of field centre
  Double_t fieldScale = 1.;      // field scaling factor

  // STS geometry
  TString stsGeom     = "sts/sts_v13d.geo.root";
  //TString stsGeom    = "sts_Standard_s3055AAFK5.SecD.geo";


  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Input file name   ----------------------------------------------
  TString inDir   = gSystem->Getenv("VMCWORKDIR");
  TString inFile  = inDir + "/input/urqmd.ftn14";
/*
  TString inFile      = "/d/cbm03/urqmd/" + system + "/" + beam + "/"
                      + trigger + "/urqmd." + system + "." + beam + "."
                      + trigger + ".0000.ftn14";
*/
  // ------------------------------------------------------------------------

  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Create detectors and passive volumes   -------------------------
  FairModule* cave   = new CbmCave("CAVE");
  cave->SetGeometryFileName(caveGeom);

  FairModule* pipe   = new CbmPipe("PIPE");
  pipe->SetGeometryFileName(pipeGeom);

  FairModule* magnet = new CbmMagnet("MAGNET");
  magnet->SetGeometryFileName(magnetGeom);

  FairDetector* sts  = new CbmStsMC(kTRUE);
  sts->SetGeometryFileName(stsGeom);
  // ------------------------------------------------------------------------

  // -----   Create magnetic field   ----------------------------------------
  if (fieldMap == "field_electron_standard" ) 
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  else if (fieldMap == "field_muon_standard" )
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  else if (fieldMap == "field_v10e" )
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  else if (fieldMap == "field_v12b" )
    CbmFieldMap* magField = new CbmFieldMapSym3(fieldMap);
  else {
    cout << "===> ERROR: Unknown field map " << fieldMap << endl;
    exit;
  }
  if (magField != NULL) {
    magField->SetPosition(0., 0., fieldZ);
    magField->SetScale(fieldScale);
  }
  // ------------------------------------------------------------------------

  // -----   Create PrimaryGenerator   --------------------------------------
//  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);
  primGen->AddGenerator(urqmdGen);
  // ------------------------------------------------------------------------

  // -----   Create simulation run   ----------------------------------------
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  fRun->SetGenerator(primGen);           // PrimaryGenerator
  fRun->SetMaterials("media.geo");       // Materials
  fRun->AddModule(cave);
  fRun->AddModule(pipe);
  if ( target ) fRun->AddModule(target);
  fRun->AddModule(magnet);
  fRun->AddModule(sts);
  fRun->SetField(magField);
  fRun->Init();
  // ------------------------------------------------------------------------

  // -----   Fill parameter containers   ------------------------------------
  FairRuntimeDb* rtdb       = fRun->GetRuntimeDb();
  if (magField != NULL) {
    CbmFieldPar* fieldPar  = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();
  }
  Bool_t kParameterMerged  = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------

  // -----   Start run   ----------------------------------------------------
  fRun->Run(nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime
       << "s" << endl << endl;

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
//  exit(0);

  // ------------------------------------------------------------------------

}
