
void run_urqmd(Float_t beamMomentum, Int_t index)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;



  // Load libraries -------------------------------------------------
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
  gSystem->Load("libRich");
  gSystem->Load("libMuch");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  // ----------------------------------------------------------------



  // File names -----------------------------------------------------
  char strInputFile[1000];
  char strOutputFile[1000];
  char strParamFile[1000];

  char *ver = getenv("CBMVER");

  sprintf(strInputFile, "/home/kresan/data/urqmd/auau/%1.0fgev/centr", beamMomentum);
  sprintf(strOutputFile, "/home/kresan/data/mc/%s/urqmd/auau/%1.0fgev/centr",
	  ver, beamMomentum);

  sprintf(strInputFile, "%s/urqmd.auau.%1.0fgev.centr.%4d.ftn14",
	  strInputFile, beamMomentum, index);
  sprintf(strParamFile, "%s/urqmd.auau.%1.0fgev.centr.%4d.mc.param.root",
	  strOutputFile, beamMomentum, index);
  sprintf(strOutputFile, "%s/urqmd.auau.%1.0fgev.centr.%4d.mc.root",
	  strOutputFile, beamMomentum, index);
  for(Int_t i = 0; i < 1000; i++) {
      if(' ' == strInputFile[i]) strInputFile[i] = '0';
      if(' ' == strOutputFile[i]) strOutputFile[i] = '0';
      if(' ' == strParamFile[i]) strParamFile[i] = '0';
  }
  // ----------------------------------------------------------------



  FairRunSim *fRun = new FairRunSim();
  // set the MC version used
  fRun->SetName("TGeant3");
  // Set Output file name
  fRun->SetOutputFile(strOutputFile);
  // Set Material file Name
  fRun->SetMaterials("media.geo");



  // Create and add detectors ---------------------------------------
  FairModule *Cave= new CbmCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe= new CbmPipe("PIPE");
  Pipe->SetGeometryFileName("pipe_standard.geo");
  fRun->AddModule(Pipe);

  FairModule *Target= new CbmTarget("Target");
  Target->SetGeometryFileName("target_au_250mu.geo");
  fRun->AddModule(Target);		

  FairModule *Magnet= new CbmMagnet("MAGNET");
  Magnet->SetGeometryFileName("passive/magnet_v09e.geo");
  fRun->AddModule(Magnet);

  FairDetector* mvd = new CbmMvd("MVD", kTRUE);
  mvd->SetGeometryFileName("mvd/mvd_v07a.geo");
  fRun->AddModule(mvd);
  
  FairDetector *Sts= new CbmSts("STS", kTRUE);
  Sts->SetGeometryFileName("sts/sts_v11a.geo");
  fRun->AddModule(Sts);

  FairDetector *Rich= new CbmRich("RICH", kFALSE);
  Rich->SetGeometryFileName("rich/rich_v08a.geo");
  fRun->AddModule(Rich);
  
  FairDetector *Trd= new CbmTrd("TRD",kTRUE );
  Trd->SetGeometryFileName("trd/trd_v11c.geo");
  fRun->AddModule(Trd);

  FairDetector *Tof= new CbmTof("TOF", kTRUE );
  Tof->SetGeometryFileName("tof/tof_v07a.geo");
  fRun->AddModule(Tof);
  // ----------------------------------------------------------------



  // Create and Set Event Generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator(strInputFile);
  primGen->AddGenerator(urqmdGen);



  // Field Map Definition
  CbmFieldMap *fMagField = new CbmFieldMapSym2("field_v10e");
  fMagField->SetPosition(0., 0., 50.);
  fMagField->SetScale(1.0);
  fRun->SetField(fMagField);



  // Trajectories visualization
  fRun->SetStoreTraj(kFALSE);



  fRun->Init();


/*
  CbmTrajFilter* trajFilter = CbmTrajFilter::Instance();
  trajFilter->SetStepSizeCut(0.01); // 1 cm
  trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 1000.);
  trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//  trajFilter->SetEnergyCut(0., 10.); // 0 < Etot < 10 GeV
  trajFilter->SetStorePrimaries(kTRUE);
  trajFilter->SetStoreSecondaries(kTRUE);
*/


  // Set the random seed
  gRandom->SetSeed(1);
  UInt_t seed;
  for(Int_t i = 0; i < index+1; i++) {
      seed = TMath::Nint( gRandom->Uniform(1., 65539.) );
  }
  cout << endl
      << "Random seed is : "
      << seed << endl << endl;
  TRandom* rnd = new TRandom(seed);
  gMC->SetRandom(rnd);



  // Stack filtering
  CbmStack *stack = FairMCApplication::Instance()->GetStack();
  stack->SetMinPoints(1);



  // Fill the Parameter containers for this run
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(fMagField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(strParamFile);
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();



  // Transport nEvents
  Int_t nEvents = 1000;
  fRun->Run(nEvents);



  FairRootManager::Instance()->CloseOutFile();
  parOut->close();



  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  


