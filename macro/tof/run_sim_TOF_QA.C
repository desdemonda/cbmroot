// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// V. Friese   06/03/2006
// Diego: taken from the one of Jun06, slightly modified. In particular RICH
// is not included since some problems appeared during loading the library
// --------------------------------------------------------------------------

{
  // ========================================================================
  //          Adjust this part according to your requirements

  // Generator
  //TString generator = "pluto"; 
  TString generator = "urqmd";
  
  // Primary
  TString primary   = "rho0";

  // Collision system
  TString system  = "auau";

  // Beam momentum
  TString beam    = "25gev";

  // Trigger (centrality)
  TString trigger = "centr";

  // Collision system
  TString system  = "auau";

  // Parameter file name
  TString parFile = "params.root";

  // Cave geometry
  TString caveGeom = "cave.geo";

  // Target geometry
  TString targetGeom = "target.geo";

  // Beam pipe geometry
  TString pipeGeom = "pipe_standard.geo";

  // Output file name
  //TString outFile = "/d/cbm01/diego/CBM/JUN06/" + generator + "." + system + "." + beam + "_1gap.mc.root";
  TString outFile = "/misc/diego/cbmroot/macro/run/bor.root";
  //TString outFile   = "/d/cbm01/diego/CBM/FEB07/SIMUL/AUAU_CENT_25GEV/bor_mono.root";

  //Add number of events!!

  // Number of events
  Int_t   nEvents = 10;  


  // Magnet geometry and field map
  TString magnetGeom  = "magnet_active.geo";
  TString fieldMap    = "FieldActive";
  Double_t fieldZ     = 50.;      // z position of field centre
  Double_t fieldScale = 1.0;      // field scaling factor

  // STS geometry
  TString stsGeom = "sts_standard.geo";

  // TRD geometry
  TString trdGeom = "trd_standard.geo";

  // TOF geometry
  TString tofGeom = "tof_standard.geo";
  //TString tofGeom = "tof_monolithic.geo";

  //TString tofGeom = "test.geo";

  //TString tofGeom = "tof_010906.geo";
  
  // ECAL geometry. 
  // Put here ecal_FullMC.geo for full Monte-Carlo (takes much more time!)
  TString ecalGeom = "ecal_FastMC.geo";

  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Input file name   ----------------------------------------------
  TString inFile = "/d/cbm03/urqmd/" + system + "/" + beam + "/"
                 + trigger + "/urqmd." + system + "." + beam + "."
                 + trigger + ".0000.ftn14";
  
  //TString inFile_pluto = "/d/cbm02/galatyuk/" + generator + "/" + system + "/" + beam + "/" +
  //               primary + "/epem/" + generator + "." + system + "." + beam + "."
  //               + primary + ".epem.10k.root";

  // ------------------------------------------------------------------------


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
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
 // ------------------------------------------------------------------------



  // -----   Create detectors and passive volumes   -------------------------
  FairModule* cave= new CbmCave("CAVE");
  cave->SetGeometryFileName(caveGeom);

  FairModule* pipe= new CbmPipe("PIPE");
  pipe->SetGeometryFileName(pipeGeom);
  
  FairModule* target= new CbmTarget("Target");
  target->SetGeometryFileName(targetGeom);

  FairModule* magnet= new CbmMagnet("MAGNET");
  magnet->SetGeometryFileName(magnetGeom);
  
  FairDetector* sts= new CbmSts("STS", kTRUE);
  sts->SetGeometryFileName(stsGeom);

  FairDetector* trd= new CbmTrd("TRD",kTRUE );
  trd->SetGeometryFileName(trdGeom);

  FairDetector* tof= new CbmTof("TOF", kTRUE);
  tof->SetGeometryFileName(tofGeom);
  
  FairDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data());
  
  // ------------------------------------------------------------------------



  // -----   Create magnetic field   ----------------------------------------
  if ( fieldMap == "FieldActive" || fieldMap == "FieldIron")
    magField = new CbmFieldMapSym3(fieldMap);
  else if ( fieldMap == "FieldAlligator" )
    magField = new CbmFieldMapSym2(fieldMap);
  else {
    cout << "===> ERROR: Field map " << fieldMap << " unknown! " << endl;
    exit;
  }
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);

  // ------------------------------------------------------------------------


  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen   = new FairPrimaryGenerator();
  FairUrqmdGenerator* urqmdGen  = new FairUrqmdGenerator(inFile);
  //FairPlutoGenerator*   plutoGen  = new FairPlutoGenerator(inFile_pluto);

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
  fRun->AddModule(target);		
  fRun->AddModule(magnet);
  fRun->AddModule(sts);
  fRun->AddModule(trd);
  fRun->AddModule(tof);
  fRun->AddModule(ecal);
  fRun->SetField(magField);
  fRun->Init();
  // ------------------------------------------------------------------------



  // -----   Fill parameter containers   ------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();

  // ------------------------------------------------------------------------
  
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  if ( magField ) {  fieldPar->SetParameters(magField); }
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);

 
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
  // ------------------------------------------------------------------------

}

