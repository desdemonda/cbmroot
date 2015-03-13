void sim(){
  TString dir = "data5/";
  TString inputSignal="";
  TString inputBgr = "data0/urqmd.auau.25gev.mbias.0000.ftn14";
  TString outFile  = dir + "mc.root";
  TString parFile  = dir + "param.root";
  Int_t nEvents = 1000;
  TString muchGeom   = "../../work/time_stamps/mbias/data5/much_compact_1cm.geo";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");
  fRun->SetOutputFile(outFile.Data());

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  fRun->SetMaterials("media.geo");

  FairModule* cave = new CbmCave("CAVE");
  cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(cave);

  FairModule* pipe = new CbmPipe("PIPE");
  pipe->SetGeometryFileName("pipe_much.geo");
  fRun->AddModule(pipe);

  FairModule* shield = new CbmShield("SHIELD");
  shield->SetGeometryFileName("shield_standard.geo");
  fRun->AddModule(shield);

  FairModule* target = new CbmTarget("Target");
  target->SetGeometryFileName("target_au_250mu.geo");
  fRun->AddModule(target);

  FairModule* magnet = new CbmMagnet("MAGNET");
  magnet->SetGeometryFileName("magnet_muon_standard.geo");
  fRun->AddModule(magnet);

  FairDetector* sts = new CbmSts("STS", kTRUE);
  sts->SetGeometryFileName("sts_standard.geo");
  fRun->AddModule(sts);

  FairDetector* much = new CbmMuch("MUCH", kTRUE);
  much->SetGeometryFileName(muchGeom);
  fRun->AddModule(much);

  CbmFieldMap* magField = new CbmFieldMapSym3("FieldMuonMagnet");
  magField->SetPosition(0., 0., 50.);
  fRun->SetField(magField);
  
  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(inputBgr.Data());
  primGen->AddGenerator(urqmdGen);
  fRun->SetGenerator(primGen);
  fRun->Init();

  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  FairParRootFileIo* parOut = new FairParRootFileIo(kTRUE);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();

  fRun->Run(nEvents);
  fRun->CreateGeometryFile("data5/geofile.root");

}

