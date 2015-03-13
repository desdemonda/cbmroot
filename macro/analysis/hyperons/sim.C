void sim(Int_t file_nr=0, Int_t nEvents=1, Int_t s=0, Int_t seed=1){
  Char_t filenr[4];
  sprintf(filenr,"%04d",file_nr);
  printf("Filenr: %s\n", filenr);

  TString signal;
  if (s!=3312 && s!=3334) signal = "la";
  else if (s==3312) signal = "xi";
  else if (s==3334) signal = "om";

  // ----- Paths and file names  --------------------------------------------
  TString inDir   = gSystem->Getenv("URQMD_INPUT_PATH");
  TString inFile  = inDir + "/urqmd.auau.25gev.centr." + filenr + ".ftn14";
  TString outDir= TString(filenr);
  outDir+= "/";
  gSystem->mkdir(outDir.Data());
  TString outFile = outDir+signal+".mc.root";
  TString parFile = outDir+signal+".par.root";

  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  TString pipeGeom   = "pipe_standard.geo";
  TString targetGeom = "target_au_250mu.geo";
  TString magnetGeom = "magnet_standard.geo";
  TString stsGeom    = "sts_Standard_s3055AAFK5.SecD.geo";

  // -----   Magnetic field   -----------------------------------------------
  TString fieldMap    = "FieldMuonMagnet";   // name of field map
  Double_t fieldZ     = 50.;                 // field centre z position
  Double_t fieldScale =  1.;                 // field scaling factor

  gDebug = 0;
  gRandom->SetSeed(seed);

  // ----  Load libraries   -------------------------------------------------
  cout << endl << "=== much_sim.C : Loading libraries ..." << endl;
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/analysis/hyperon/analysislibs.C");
  analysislibs();

  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");
  fRun->SetOutputFile(outFile.Data());
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  fRun->SetMaterials("media.geo");

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
    fRun->AddModule(pipe);
    cout << "    --- " << pipeGeom << endl;
  }

  if ( targetGeom != "" ) {
    FairModule* target = new CbmTarget("Target");
    target->SetGeometryFileName(targetGeom);
    fRun->AddModule(target);
    cout << "    --- " << targetGeom << endl;
  }

  if ( magnetGeom != "" ) {
    FairModule* magnet = new CbmMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    fRun->AddModule(magnet);
    cout << "    --- " << magnetGeom << endl;
  }

  if ( stsGeom != "" ) {
    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName(stsGeom);
    cout << "    --- " << stsGeom << endl;
    fRun->AddModule(sts);
  }


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

  // -----   Create PrimaryGenerator   --------------------------------------
  cout << endl << "=== much_sim.C : Create generators ..." << endl;
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

  if (signal=="om"){
    delete gRandom;
    gRandom = new TRandom3();
    CbmAnaHypYPtGenerator* gen = new CbmAnaHypYPtGenerator(3334);
    gen->SetDistributionPt(0.1603);          //  6 GeV
    gen->SetDistributionY(1.277,0.412);      //  6 GeV
    //gen->SetDistributionPt(0.149808);        // 25 GeV
    //gen->SetDistributionY(1.9875,0.546669);  // 25 GeV
    gen->Init();
    primGen->AddGenerator(gen);
  } else if (signal=="xi"){
    delete gRandom;
    gRandom = new TRandom3();
    CbmAnaHypYPtGenerator* gen = new CbmAnaHypYPtGenerator(3312);
    gen->SetDistributionPt(0.1603);            //  6 GeV
    gen->SetDistributionY(1.277,0.412);        //  6 GeV
    //gen->SetDistributionPt(0.154319);          //  25 GeV
    //gen->SetDistributionY(1.98604,0.617173);   //  25 GeV
    gen->Init();
    primGen->AddGenerator(gen);
  }

  FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile.Data());
  primGen->AddGenerator(urqmdGen);

  fRun->SetGenerator(primGen);
  // ------------------------------------------------------------------------


  fRun->Init();

  // ------------------------------------------------------------------------
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile);
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------

  TStopwatch timer;
  timer.Start();

  fRun->Run(nEvents);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "=== sim.C : Macro finished successfully." << endl;
  cout << "=== sim.C : Output file is " << outFile << endl;
  cout << "=== sim.C : Real time used: " << rtime << "s " << endl;
  cout << "=== sim.C : CPU time used : " << ctime << "s " << endl;
  cout << endl << endl;
  // ------------------------------------------------------------------------
}

