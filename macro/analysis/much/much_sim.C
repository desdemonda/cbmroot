// --------------------------------------------------------------------------
//
// Macro for standard transport simulation with GEEANT3
// in the CBM muon setup: STS + MUCH + TRD + TOF
//
// No MVD nor ECAL
//
// To be defined as arguments:
//    Signal input file (PLUTO file)
//    Background input file (UrQMD file)
//    Output file
//    Number of events to be processed (default = 1)
//    First event to be processed (default = 1)
//
// Parameters (geometry and field) are stored in the output file.
//
// M.Ryzhinskiy
// --------------------------------------------------------------------------

void much_sim(char* dir = "data", Int_t fileNo=0, Int_t nEvents=10, Int_t seed=1){
  //  const char* inputSignal="data_signal/Jpsi_100000ev.root",
  //../../urqmd/urqmd.auau.25gev.centr.0000.ftn14",
  const char* inputBgr="";
  gSystem->mkdir(dir);
  gSystem->mkdir(Form("%s/%02i",dir,fileNo));
  TString outFile=Form("%s/%02i/mc.root",dir,fileNo);

  // ========================================================================
  //          Adjust this part according to your requirements

  // -----   Confirm input parameters    ------------------------------------
  cout << endl;
  cout << "========  CBMROOT Macro much_sim  =================" << endl;

  cout << "Output file       is " << outFile << endl;
  cout << "Events to process:   " << nEvents << endl;
  cout << "===================================================" << endl;
  cout << endl;

  // -----   Specify MUCH related geometry   --------------------------------
  // much_standard.geo: 6 absorbers, 6 stations (12 detectors)
  // much_compact.geo: 5 absorbers, 5 stations (10 detectors)
  // Use pipe_much.geo for the beam pipe in both cases.
  // In case you want the addtional W shielding around the pipe,
  // use shield_standard.geo or shield_compact.geo, respective to the
  // MUCH geometry. Otherwise, define an empty string.
  TString muchGeom   = "much_standard_monolithic.geo";
  TString pipeGeom   = "pipe_much.geo";
  TString shieldGeom = "shield_standard.geo";

  // -----   Other geometries   ---------------------------------------------
  TString caveGeom   = "cave.geo";
  TString targetGeom = "target_au_250mu.geo";
  TString magnetGeom = "magnet_standard.geo";
  TString stsGeom    = "sts_standard.geo";
  TString trdGeom    = "";
  TString tofGeom    = "";

  // -----   Magnetic field   -----------------------------------------------
  TString  fieldMap   = "FieldMuonMagnet";   // name of field map
  Double_t fieldZ     = 50.;                 // field centre z position
  Double_t fieldScale =  1.;                 // field scaling factor

  // In general, the following parts need not be touched
  // ========================================================================


  // -----   Set unique random generator seed   -----------------------------
  // Comment this out if you want to have a defined seed for reproducability.
  gRandom->SetSeed(seed);
  // ------------------------------------------------------------------------


  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------


  // ----  Load libraries   -------------------------------------------------
  cout << endl << "=== much_sim.C : Loading libraries ..." << endl;
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
  gSystem->Load("libGlobal");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libMuch");
  gSystem->Load("libStt");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  gSystem->Load("libL1");
  gSystem->Load("libAnalysis");
  // -----------------------------------------------------------------------

  // -----   Create simulation run   ----------------------------------------
  cout << endl << "=== much_sim.C : Creating run and database ..." << endl;
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile.Data());          // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  // ------------------------------------------------------------------------

  // -----   Create media   -------------------------------------------------
  cout << endl << "=== much_sim.C : Set materials ..." << endl;
  fRun->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------

  // -----   Create detectors and passive volumes   -------------------------
  cout << endl << "=== much_sim.C : Create geeometry ..." << endl;
  if ( caveGeom != "" ) {
    FairModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
    cout << "    --- " << caveGeom << endl;
  }

  if ( pipeGeom != "" ) {
    FairModule* pipe = new CbmPipe("PIPE");
    pipe->SetGeometryFileName(pipeGeom);
    cout << "    --- " << pipeGeom << endl;
    fRun->AddModule(pipe);
  }

   if ( shieldGeom != "" ) {
    FairModule* shield = new CbmShield("SHIELD");
    shield->SetGeometryFileName(shieldGeom);
    cout << "    --- " << shieldGeom << endl;
    fRun->AddModule(shield);
  }

  if ( targetGeom != "" ) {
    FairModule* target = new CbmTarget("Target");
    target->SetGeometryFileName(targetGeom);
    cout << "    --- " << targetGeom << endl;
    fRun->AddModule(target);
  }

  if ( magnetGeom != "" ) {
    FairModule* magnet = new CbmMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    cout << "    --- " << magnetGeom << endl;
    fRun->AddModule(magnet);
  }

  if ( stsGeom != "" ) {
    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName(stsGeom);
    cout << "    --- " << stsGeom << endl;
    fRun->AddModule(sts);
  }

  if ( muchGeom != "" ) {
    FairDetector* much = new CbmMuch("MUCH", kTRUE);
    much->SetGeometryFileName(muchGeom);
    cout << "    --- " << muchGeom << endl;
    fRun->AddModule(much);
  }
  // ------------------------------------------------------------------------


  // -----   Create magnetic field   ----------------------------------------
  cout << endl << "=== much_sim.C : Create magnetic field ..." << endl;
  CbmFieldMap* magField = NULL;
  if ( fieldMap == "FieldActive" || fieldMap == "FieldIron")
    magField = new CbmFieldMapSym3(fieldMap);
  else if ( fieldMap == "FieldAlligator" )
    magField = new CbmFieldMapSym2(fieldMap);
  else if ( fieldMap == "FieldMuonMagnet" )
    magField = new CbmFieldMapSym3(fieldMap);
  else {
    cout << "===> ERROR: Field map " << fieldMap << " unknown! " << endl;
    exit;
  }
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  cout << "    ---  Field    is " << fieldMap << endl;
  cout << "    ---  Position is z = " << fieldZ << " cm" << endl;
  cout << "    ---  Scale    is " << fieldScale << endl;
   // ------------------------------------------------------------------------


  // -----   Create PrimaryGenerator   --------------------------------------
  cout << endl << "=== much_sim.C : Create generators ..." << endl;
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

  // --- Polarized generator
  CbmPolarizedGenerator *polGen= new CbmPolarizedGenerator(443);
  polGen->SetDistributionPt(0.176);        // 25 GeV
  polGen->SetDistributionY(1.9875,0.228);  // 25 GeV
  polGen->SetRangePt(0.,3.);
  polGen->SetRangeY(1.,3.);
  polGen->SetBox(1);
  polGen->SetRefFrame(CbmPolarizedGenerator::kHelicity);
  polGen->SetDecayMode(CbmPolarizedGenerator::kDiMuon);
  polGen->SetAlpha(0);
  polGen->Init();
  primGen->AddGenerator(polGen);
  if ( inputBgr != "" ) {
    FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator(inputBgr);
    primGen->AddGenerator(urqmdGen);
  }
  fRun->SetGenerator(primGen);
  // ------------------------------------------------------------------------
  
//  // -----   Create PrimaryGenerator   --------------------------------------
//  cout << endl << "=== much_sim.C : Create generators ..." << endl;
//  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
//  if ( inputSignal != "" ) {
//  	FairPlutoGenerator *plutoGen= new FairPlutoGenerator(inputSignal);
//  	primGen->AddGenerator(plutoGen);
//  }
//  if ( inputBgr != "" ) {
//  	FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inputBgr);
//  	primGen->AddGenerator(urqmdGen);
//  }
//  fRun->SetGenerator(primGen);
//  // ------------------------------------------------------------------------

//  // -----   Create PrimaryGenerator   --------------------------------------
//  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
//
//  cout << endl << "=== much_sim.C : Create generators ..." << endl;
//  FairBoxGenerator* boxGen1 = new FairBoxGenerator(13,20);
//  boxGen1->SetPRange(2.5,25);
//  boxGen1->SetPhiRange(0.,360.);
//  boxGen1->SetThetaRange(2.5, 25.);
//  boxGen1->SetCosTheta();
//  boxGen1->Init();
//  primGen->AddGenerator(boxGen1);
//
//  FairBoxGenerator* boxGen2 = new FairBoxGenerator(-13,20);
//  boxGen2->SetPRange(2.5,25);
//  boxGen2->SetPhiRange(0.,360.);
//  boxGen2->SetThetaRange(2.5, 25.);
//  boxGen2->SetCosTheta();
//  boxGen2->Init();
//  primGen->AddGenerator(boxGen2);
//
//  fRun->SetGenerator(primGen);
//  // ------------------------------------------------------------------------



  // -----   Run initialisation   -------------------------------------------
  cout << endl << "=== much_sim.C : Initialise run ..." << endl;
  fRun->Init();
  // ------------------------------------------------------------------------


  // -----   Runtime database   ---------------------------------------------
  cout << endl << "=== much_sim.C : Set up database ..." << endl;
  cout << "                Parameters will be saved to output file" << endl;
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(gFile);
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  cout << endl << "=== much_sim.C : Starting timer ..." << endl;
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Start run   ----------------------------------------------------
  cout << endl << "=== much_sim.C : Start run ..." << endl;
  fRun->Run(nEvents);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "=== much_sim.C : Macro finished successfully." << endl;
  cout << "=== much_sim.C : Output file is " << outFile << endl;
  cout << "=== much_sim.C : Real time used: " << rtime << "s " << endl;
  cout << "=== much_sim.C : CPU time used : " << ctime << "s " << endl;
  cout << endl << endl;
  // ------------------------------------------------------------------------

}

