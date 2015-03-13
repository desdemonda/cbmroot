// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
void run()
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;




  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  CbmTarget* target = new CbmTarget("Gold", 0.025);
  TString pipeGeom   = "pipe/pipe_standard.geo";
  TString magnetGeom = "magnet/magnet_v09e.geo";
  TString stsGeom    = "sts/sts_v13d.geo.root";
  TString richGeom   = "rich/rich_v08a.geo";
  TString trdGeom    = "trd/trd_v13g.geo.root";
  TString tofGeom    = "tof/tof_v07a.geo";
//  TString ecalGeom   = "ecal/ecal_v08a.geo";


  FairLogger *logger = FairLogger::GetLogger();
  logger->SetLogFileName("MyLog.log");
  logger->SetLogToScreen(kTRUE);
  logger->SetLogToFile(kTRUE);
  logger->SetLogVerbosityLevel("HIGH");
  logger->SetLogFileLevel("DEBUG4");
  logger->SetLogScreenLevel("INFO");


  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  
  fRun->SetOutputFile("data/auaumbias.root");

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media.geo");
  
  // Create and add passive materials and detectors
  //-------------------------
  FairModule *Cave= new CbmCave("CAVE");
  Cave->SetGeometryFileName(caveGeom);
  fRun->AddModule(Cave);

  FairModule *Pipe= new CbmPipe("PIPE");
  Pipe->SetGeometryFileName(pipeGeom);
  fRun->AddModule(Pipe);
  
  fRun->AddModule(target);		

  FairModule *Magnet= new CbmMagnet("MAGNET");
  Magnet->SetGeometryFileName(magnetGeom);
  fRun->AddModule(Magnet);
  
  FairDetector *Sts= new CbmStsMC(kTRUE);
  Sts->SetGeometryFileName(stsGeom);
  fRun->AddModule(Sts);

  FairDetector *Trd= new CbmTrd("TRD",kTRUE );
  Trd->SetGeometryFileName(trdGeom);
  fRun->AddModule(Trd);

  FairDetector *Rich= new CbmRich("RICH", kTRUE);
  Rich->SetGeometryFileName(richGeom);
  fRun->AddModule(Rich);
  
  FairDetector *Tof= new CbmTof("TOF", kTRUE );
  Tof->SetGeometryFileName(tofGeom);
  fRun->AddModule(Tof);
	
  //FairDetector *Ecal= new CbmEcal("ECAL", kTRUE);
  //Ecal->SetGeometryFileName(ecalGeom);
  //fRun->AddModule(Ecal);


 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

 // Ion Generator
  FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
  primGen->AddGenerator(fIongen);


  // Field Map Definition
 // --------------------
  // Constant Field
  CbmFieldConst *fMagField1=new CbmFieldConst();
  fMagField1->SetField(0, 14 ,0 ); // values are in kG
  // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
  fMagField1->SetFieldRegion(-74, -39 ,-22 , 74, 39 , 160 );
  
  fRun->SetField(fMagField1);
  
  // Store the partile trajectories

  fRun->SetStoreTraj(kTRUE);
  
  // Initialize the simulation

  fRun->Init();

 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------

 // Set cuts for storing the trajectories
  FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  trajFilter->SetStepSizeCut(0.01); // 1 cm
  trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
  trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
  trajFilter->SetStorePrimaries(kTRUE);
  trajFilter->SetStoreSecondaries(kTRUE);
 
  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("data/testparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------

  Int_t nEvents = 3;
  fRun->Run(nEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << "Realtime: " << rtime << " seconds" << endl;
  cout << " Cputime: " << ctime << " seconds" << endl; 

  cout << " Test passed" << endl;
  cout << " All ok " << endl;

  exit(1);
}  
  
