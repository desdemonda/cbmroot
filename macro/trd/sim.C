// Macro for transport
{


  TStopwatch timer;
  timer.Start();
  gDebug=0;


  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();



  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libBase");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libTof");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libEcal");
  gSystem->Load("libGen");



  // In- and out-file names
  TString inFile = "/d/cbm01/cbmsim/urqmd/auau/25gev/centr/\
urqmd.auau.25gev.centr.0000.ftn14";
  TString outFile = "test.mc.root";
  TString paramFile = "test.mc.param.root";


  // Number of events
  Int_t nEvents = 1;



  // Create run object
  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  fRun->SetName("TGeant3");

  fRun->SetOutputFile(outFile.Data());

  // Set Material file Name
  fRun->SetMaterials("media.geo");




  // Create and add detectors
  //-------------------------

  FairModule *Cave= new CbmCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe= new CbmPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);

  // Active shielding Geometry
  FairModule *Magnet= new CbmMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet_active.geo");
  fRun->AddModule(Magnet);
  
  FairDetector *Sts= new CbmSts("STS", kTRUE);
  Sts->SetGeometryFileName("sts.geo");
  fRun->AddModule(Sts);

  FairModule *Target= new CbmTarget("Target");
  Target->SetGeometryFileName("target.geo");
  fRun->AddModule(Target);		
/*
  FairDetector *Tof= new CbmTof("TOF", kTRUE );
  Tof->SetGeometryFileName("tof.geo");
  fRun->AddModule(Tof);
*/
  FairDetector *Trd= new CbmTrd("TRD",kTRUE );
  Trd->SetGeometryFileName("trd_9.geo");
  fRun->AddModule(Trd);
/*
  FairDetector *Rich= new CbmRich("RICH", kTRUE);
  Rich->SetGeometryFileName("rich.geo");
  fRun->AddModule(Rich);

  FairDetector *Ecal= new CbmEcal("ECAL", kTRUE);
  Ecal->SetGeometryFileName("ecal.geo");
  fRun->AddModule(Ecal);
*/




  // Create and Set Event Generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Urqmd  Generator
  FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator(inFile.Data());
  primGen->AddGenerator(urqmdGen);



  // Magnet with active Shielding
  CbmField *fMagField=new CbmField("Magnet Active Shielding map");
     fMagField->readRootfile("$VMCWORKDIR/input/FieldActive.root", "NewMap");
   fRun->SetField(fMagField);



// -Trajectories Visualization (TGeoManager Only )
//   fRun->SetStoreTraj(kTRUE);



   fRun->Init();



 // Set cuts for storing the trajectpries
  /* FairTrajFilter* trajFilter = FairTrajFilter::Instance();
     trajFilter->SetStepSizeCut(0.01); // 1 cm
     trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
     trajFilter->SetStorePrimaries(kTRUE);
     trajFilter->SetStoreSecondaries(kTRUE);
   */



  // Fill the Parameter containers for this run
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(paramFile.Data());
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();



  // Transport nEvents
  fRun->Run(nEvents);



  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
