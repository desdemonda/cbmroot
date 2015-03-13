// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// CBM setup with STS only
//
// V. Friese   14/09/2006
//
// --------------------------------------------------------------------------

void sts_sim(Int_t nEvents = 1000)
{
  // ========================================================================
  //          Adjust this part according to your requirements

  // Input file
  TString inPath = "/d/cbm03/urqmd/auau/25gev/centr/";
  TString inFile = inPath + "urqmd.auau.25gev.centr.0000.ftn14";
  
  // Output file
  TString outFile = Form("sts.mc.root",nEvents);
  
  // Parameter file
  TString parFile = Form("params.root",nEvents);

  // Cave geometry
  TString caveGeom = "cave.geo";
  
  // Target geometry
  TString targetGeom = "target_au_250mu.geo";

  // Beam pipe geometry
  TString pipeGeom = "pipe_standard.geo";

  // Magnet geometry and field map
  TString magnetGeom = "passive/magnet_v09e.geo";
  TString fieldMap    = "field_v10e";
  Double_t fieldZ     = 50.;     // z position of field centre
  Double_t fieldScale = 1.;      // field scaling factor

  // MVD geometry
  TString mvdGeom    = "mvd/mvd_v07a.geo";

  // STS geometry
  TString stsGeom    = "sts/sts_v11a.geo";
  
  //STS geometry for the same z position of all sensors
  //TString stsGeom = "sts_same_z.geo";

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
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  // ------------------------------------------------------------------------

   
 
  // -----   Create simulation run   ----------------------------------------
  FairRunSim* run = new FairRunSim();
  run->SetName("TGeant3");              // Transport engine
  run->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  // ------------------------------------------------------------------------
  
  

  // -----   Create media   -------------------------------------------------
  run->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------

    
  
  // -----   Create geometry   ----------------------------------------------
  FairModule* cave= new CbmCave("CAVE");
  cave->SetGeometryFileName(caveGeom);
  run->AddModule(cave);

  FairModule* pipe= new CbmPipe("PIPE");
  pipe->SetGeometryFileName(pipeGeom);
  run->AddModule(pipe);
  
  FairModule* target= new CbmTarget("Target");
  target->SetGeometryFileName(targetGeom);
  run->AddModule(target);

  FairModule* magnet= new CbmMagnet("MAGNET");
  magnet->SetGeometryFileName(magnetGeom);
  run->AddModule(magnet);
  
  FairDetector* mvd= new CbmMvd("MVD", kTRUE);
  mvd->SetGeometryFileName(mvdGeom); 
  run->AddModule(mvd);

  FairDetector* sts= new CbmSts("STS", kTRUE);
  sts->SetGeometryFileName(stsGeom); 
  run->AddModule(sts);
  // ------------------------------------------------------------------------

  
  
  // -----   Create magnetic field   ----------------------------------------
  CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  run->SetField(magField);
  // ------------------------------------------------------------------------

  
  
  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);
  primGen->AddGenerator(urqmdGen);
  run->SetGenerator(primGen);
  // ------------------------------------------------------------------------

//   run->SetStoreTraj(kTRUE);
  
  // -----   Initialize simulation run   ------------------------------------
  run->Init();
  // ------------------------------------------------------------------------

  
  
  // -----   Runtime database   ---------------------------------------------
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(run->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------

  
   
  // -----   Start run   ----------------------------------------------------
  run->Run(nEvents);
  // ------------------------------------------------------------------------
  run->CreateGeometryFile("data/geofile_full.root");
  
  
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

