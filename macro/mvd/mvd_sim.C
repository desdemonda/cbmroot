// -------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// CBM setup with MVD only
//
// V. Friese   06/02/2007
//
// --------------------------------------------------------------------------
void mvd_sim()
{
  // ========================================================================
  //          Adjust this part according to your requirements

  // Input file
  TString inDir   = gSystem->Getenv("VMCWORKDIR");
  TString inFile = inDir + "/input/urqmd.ftn14";

  // Number of events
  Int_t   nEvents = 5;

  // Output file name
  TString outFile = "data/mvd.mc.root";

  // Parameter file name
  TString parFile = "data/params.root";

  // Cave geometry
  TString caveGeom = "cave.geo";

  // Beam pipe geometry
  TString pipeGeom = "pipe/pipe_v14l.root";

  // Magnet geometry and field map
  TString magnetGeom  = "magnet/magnet_v12b.geo.root";
  TString fieldMap    = "field_v12b";   // name of field map
  Int_t fieldZ     = 40.;             // field centre z position
  Int_t fieldScale =  1.;             // field scaling factor
  Int_t fieldSymType = 3;

  // MVD geometry

  TString mvdGeom = "mvd/mvd_v14a.geo.root";

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
  gSystem->Load("libCbmGenerators");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  // ------------------------------------------------------------------------

 
 
  // -----   Create simulation run   ----------------------------------------
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  // ------------------------------------------------------------------------


  // -----   Create media   -------------------------------------------------
  fRun->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------


  // -----   Create geometry   ----------------------------------------------
  FairModule* cave= new CbmCave("CAVE");
  cave->SetGeometryFileName(caveGeom);
  fRun->AddModule(cave);

  FairModule* pipe= new CbmPipe("PIPE");
  pipe->SetGeometryFileName(pipeGeom);
  fRun->AddModule(pipe);
  
  FairModule* target= new CbmTarget(79, 0.25);
  fRun->AddModule(target);		

  FairModule* magnet = new CbmMagnet("MAGNET");
  magnet->SetGeometryFileName(magnetGeom);
  fRun->AddModule(magnet);
  
  FairDetector* mvd= new CbmMvd("MVD", kTRUE);
  mvd->SetGeometryFileName(mvdGeom); 
  mvd->SetMotherVolume("pipevac1");
  fRun->AddModule(mvd);
  // ------------------------------------------------------------------------



  // -----   Create magnetic field   ---------------------------------------
  if ( 2 == fieldSymType ) {
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  }  else if ( 3 == fieldSymType ) {
    CbmFieldMap* magField = new CbmFieldMapSym3(fieldMap);
  } 
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  // ------------------------------------------------------------------------



  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);
  primGen->AddGenerator(urqmdGen);
  fRun->SetGenerator(primGen);     
  // ------------------------------------------------------------------------


 
  // -----   Run initialisation   -------------------------------------------
  fRun->Init();
  // ------------------------------------------------------------------------

  
  
  // -----   Runtime database   ---------------------------------------------
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------


 
  // -----   Start run   ----------------------------------------------------
  fRun->Run(nEvents);
  fRun->CreateGeometryFile("data/fullgeometry.root");
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
  // ------------------------------------------------------------------------

}

