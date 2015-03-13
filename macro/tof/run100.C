// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  gROOT->Reset();
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  TString targetGeom = "target_au_250mu.geo";
  TString pipeGeom   = "pipe_standard.geo";
  TString magnetGeom = "passive/magnet_v12a.geo";
  TString stsGeom    = "sts/sts_v11a.geo";
  TString richGeom   = "rich/rich_v08a.geo";
  TString trdGeom    = "trd/trd_v10b.geo";
  // TString tofGeom    = "tof/tof_v07a.geo";
   TString tofGeom    = "tof/test.geo";
//  TString ecalGeom   = "ecal/ecal_v08a.geo";
// ----- Generator ------
  TString inDir   = gSystem->Getenv("VMCWORKDIR");
  //  TString inFile  = inDir + "/input/urqmd.AuAu.10gev.centr.0000.ftn14";
  TString inFile  = inDir + "/input/urqmd.auau.8gev.mbias.0000.ftn14";

  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load all needed libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libCbmGenerators");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libEcal");
  gSystem->Load("libKF");
  // gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libLittrack");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");

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
  
  FairModule *Target= new CbmTarget("Target");
  Target->SetGeometryFileName(targetGeom);
  fRun->AddModule(Target);		

  FairModule *Magnet= new CbmMagnet("MAGNET");
  Magnet->SetGeometryFileName(magnetGeom);
  fRun->AddModule(Magnet);
  
  FairDetector *Sts= new CbmSts("STS", kTRUE);
  Sts->SetGeometryFileName(stsGeom);
  fRun->AddModule(Sts);

  //  FairDetector *Trd= new CbmTrd("TRD",kTRUE );
  //Trd->SetGeometryFileName(trdGeom);
  //fRun->AddModule(Trd);

  //FairDetector *Rich= new CbmRich("RICH", kTRUE);
  //Rich->SetGeometryFileName(richGeom);
  //fRun->AddModule(Rich);
  
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
 //  FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
 //  primGen->AddGenerator(fIongen);
  FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);
  primGen->AddGenerator(urqmdGen);


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
  trajFilter->SetVertexCut(-2000., -2000., -1000., 2000., 2000., 1000.);
  trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  trajFilter->SetEnergyCut(0.1, 100.02); // 0 < Etot < 1.04 GeV
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

  Int_t nEvents = 100;
  fRun->Run(nEvents);
  fRun->CreateGeometryFile("data/geofile_100.root");

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << "Realtime: " << rtime << " seconds" << endl;
  cout << " Cputime: " << ctime << " seconds" << endl; 

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}  
  
