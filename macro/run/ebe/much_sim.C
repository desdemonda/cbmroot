void much_sim(
    TString inputBgr = "/home/ekryshen/cbm/urqmd/urqmd.auau.35gev.centr.0000.ftn14",
    TString outFile = "data/mc.root",
    Int_t nEvents = 10){
  TString parFile    = "data/par.root";
  TString muchGeom   = "much/much_v12c.geo";
  TString pipeGeom   = "pipe/pipe_much.geo";
  TString shieldGeom = "shield_standard.geo";
  TString caveGeom   = "cave.geo";
  TString targetGeom = "target/target_au_250mu.geo";
  TString magnetGeom = "magnet/magnet_v10e.root";
  TString stsGeom    = "sts/sts_v12b.geo.root";
  TString fieldMap   = "field_v12a";
  gRandom->SetSeed(1);
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");
  fRun->SetOutputFile(outFile);
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  fRun->SetMaterials("media.geo");
  
  FairModule* cave = new CbmCave("CAVE");
  cave->SetGeometryFileName(caveGeom);
  fRun->AddModule(cave);
  FairModule* pipe = new CbmPipe("PIPE");
  pipe->SetGeometryFileName(pipeGeom);
  fRun->AddModule(pipe);
  FairModule* shield = new CbmShield("SHIELD");
  shield->SetGeometryFileName(shieldGeom);
  fRun->AddModule(shield);
  FairModule* target = new CbmTarget("Target");
  target->SetGeometryFileName(targetGeom);
  fRun->AddModule(target);
//  FairModule* magnet = new CbmMagnet("MAGNET");
//  magnet->SetGeometryFileName(magnetGeom);
//  fRun->AddModule(magnet);
  FairDetector* sts = new CbmSts("STS", kTRUE);
  sts->SetGeometryFileName(stsGeom);
  fRun->AddModule(sts);
  FairDetector* much = new CbmMuch("MUCH", kTRUE);
  much->SetGeometryFileName(muchGeom);
  fRun->AddModule(much);

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inputBgr);
  primGen->AddGenerator(urqmdGen);
  fRun->SetGenerator(primGen);


  CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  magField->SetPosition(0.,0.,50.);
  magField->SetScale(1.);
  fRun->SetField(magField);

  fRun->Init();

  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  FairParRootFileIo* parOut = new FairParRootFileIo(1);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();

  fRun->Run(nEvents);
  fRun->CreateGeometryFile("data/geometry.root");
}

